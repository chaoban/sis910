/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2005 M. Bakker, Nero AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** The "appropriate copyright message" mentioned in section 2c of the GPLv2
** must read: "Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Nero AG through Mpeg4AAClicense@nero.com.
**
** $Id: mp4ff.c,v 1.20 2007/11/01 12:33:29 menno Exp $
**/

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "mp4.h"
#include "filtbank.h"
#include "mp4ff.h"
#include "aacdec.h"
#include "syntax.h"
#include <cyg/io/aud/adec.h>

int GetAACTrack(mp4ff_t *infile)
{
    /* find AAC track */
    int i, rc;
    int numTracks = mp4ff_total_tracks(infile);

    for (i = 0; i < numTracks; i++)
    {
        unsigned char *buff = NULL;
        int buff_size = 0;
        mp4AudioSpecificConfig mp4ASC;

        mp4ff_get_decoder_config(infile, i, &buff, &buff_size);

        if (buff)
        {
            rc = NeAACDecAudioSpecificConfig(buff, buff_size, &mp4ASC);
            free(buff);

            if (rc < 0)
                continue;
            return i;
        }
    }

    /* can't decode this */
    return -1;
}
/*
unsigned long srates[] =
{
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000,
    12000, 11025, 8000
};
*/
unsigned int read_callback(unsigned char *buffer, unsigned int length)
{
    return ABV_read(length, buffer);
}

unsigned int seek_callback(unsigned long position)
{
    return Skip_forward(position, SEEK_SET);
}

/*
mp4ff_t *mp4ff_open_read_metaonly(mp4ff_callback_t *f)
{
    mp4ff_t *ff = malloc(sizeof(mp4ff_t));

    memset(ff, 0, sizeof(mp4ff_t));

    ff->stream = f;

    parse_atoms(ff,1);

    return ff;
}
*/
void mp4ff_close(mp4ff_t *ff)
{
    int i;
    if(ff)
    {
        for (i = 0; i < ff->total_tracks; i++)
        {
            if (ff->track[i])
            {
                if (ff->track[i]->stsz_table)
                    free(ff->track[i]->stsz_table);
                if (ff->track[i]->stts_sample_count)
                    free(ff->track[i]->stts_sample_count);
                if (ff->track[i]->stts_sample_delta)
                    free(ff->track[i]->stts_sample_delta);
                if (ff->track[i]->stsc_first_chunk)
                    free(ff->track[i]->stsc_first_chunk);
                if (ff->track[i]->stsc_samples_per_chunk)
                    free(ff->track[i]->stsc_samples_per_chunk);
                if (ff->track[i]->stsc_sample_desc_index)
                    free(ff->track[i]->stsc_sample_desc_index);
                if (ff->track[i]->stco_chunk_offset)
                    free(ff->track[i]->stco_chunk_offset);
                if (ff->track[i]->decoderConfig)
                    free(ff->track[i]->decoderConfig);
                if (ff->track[i]->ctts_sample_count)
                    free(ff->track[i]->ctts_sample_count);
                if (ff->track[i]->ctts_sample_offset)
                    free(ff->track[i]->ctts_sample_offset);
                free(ff->track[i]);
            }
        }
        free(ff->stream);
#ifdef USE_TAGGING
    mp4ff_tag_delete(&(ff->tags));
#endif
        //free(ff);
    }    
}

static void mp4ff_track_add(mp4ff_t *f)
{
    f->total_tracks++;

    f->track[f->total_tracks - 1] = malloc(sizeof(mp4ff_track_t));

    memset(f->track[f->total_tracks - 1], 0, sizeof(mp4ff_track_t));
}

static int need_parse_when_meta_only(unsigned char atom_type)
{
	switch(atom_type)
	{
	case ATOM_EDTS:
//	case ATOM_MDIA:
//	case ATOM_MINF:
	case ATOM_DRMS:
	case ATOM_SINF:
	case ATOM_SCHI:
//	case ATOM_STBL:
//	case ATOM_STSD:
	case ATOM_STTS:
	case ATOM_STSZ:
	case ATOM_STZ2:
	case ATOM_STCO:
	case ATOM_STSC:
//	case ATOM_CTTS:
	case ATOM_FRMA:
	case ATOM_IVIV:
	case ATOM_PRIV:
		return 0;
	default:
		return 1;
	}
}

/* parse atoms that are sub atoms of other atoms */
int parse_sub_atoms(mp4ff_t *f, const unsigned long total_size,int meta_only)
{
    unsigned long size;
    unsigned char atom_type = 0;
    unsigned long counted_size = 0;
    unsigned char header_size = 0;

    while (counted_size < total_size)
    {
        size = mp4ff_atom_read_header(f, &atom_type, &header_size);
        counted_size += size;

        /* check for end of file */
        if (size == 0)
            break;

        /* we're starting to read a new track, update index,
         * so that all data and tables get written in the right place
         */
        if (atom_type == ATOM_TRAK)
        {
            mp4ff_track_add(f);
        }

        /* parse subatoms */
	if (meta_only && !need_parse_when_meta_only(atom_type))
	{
		mp4ff_set_position(f, mp4ff_position(f)+size-header_size);
	} else if (atom_type < SUBATOMIC)
       {
           parse_sub_atoms(f, size-header_size,meta_only);
       } else {
           mp4ff_atom_read(f, (unsigned int)size, atom_type);
       }
    }

    return 0;
}

/* parse root atoms */
int parse_atoms(mp4ff_t *f,int meta_only)
{
    unsigned long size;
    unsigned char atom_type = 0;
    unsigned char header_size = 0;

    f->file_size = 0;

    while ((size = mp4ff_atom_read_header(f, &atom_type, &header_size)) != 0)
    {
        f->file_size += size;
        f->last_atom = atom_type;

        if (atom_type == ATOM_MDAT && f->moov_read)
        {
            /* moov atom is before mdat, we can stop reading when mdat is encountered */
            /* file position will stay at beginning of mdat data */
//            break;
        }

        if (atom_type == ATOM_MOOV && size > header_size)
        {
            f->moov_read = 1;
            f->moov_offset = mp4ff_position(f)-header_size;
            f->moov_size = size;
        }

        /* parse subatoms */
		if (meta_only && !need_parse_when_meta_only(atom_type))
		{
			mp4ff_set_position(f, mp4ff_position(f)+size-header_size);
		} else if (atom_type < SUBATOMIC)
        {
            parse_sub_atoms(f, size-header_size,meta_only);
        } else {
            /* skip this atom */
            mp4ff_set_position(f, mp4ff_position(f)+size-header_size);
        }
    }

    return 0;
}

int mp4ff_get_decoder_config(const mp4ff_t *f, const int track,
                                 unsigned char** ppBuf, unsigned int* pBufSize)
{
    if (track >= f->total_tracks)
    {
        *ppBuf = NULL;
        *pBufSize = 0;
        return 1;
    }

    if (f->track[track]->decoderConfig == NULL || f->track[track]->decoderConfigLen == 0)
    {
        *ppBuf = NULL;
        *pBufSize = 0;
    } else {
        *ppBuf = malloc(f->track[track]->decoderConfigLen);
        if (*ppBuf == NULL)
        {
            *pBufSize = 0;
            return 1;
        }
        memcpy(*ppBuf, f->track[track]->decoderConfig, f->track[track]->decoderConfigLen);
        *pBufSize = f->track[track]->decoderConfigLen;
    }

    return 0;
}

int mp4ff_get_track_type(const mp4ff_t *f, const int track)
{
	return f->track[track]->type;
}

int mp4ff_total_tracks(const mp4ff_t *f)
{
    return f->total_tracks;
}

int mp4ff_time_scale(const mp4ff_t *f, const int track)
{
    return f->track[track]->timeScale;
}

unsigned int mp4ff_get_avg_bitrate(const mp4ff_t *f, const int track)
{
	return f->track[track]->avgBitrate;
}

unsigned int mp4ff_get_max_bitrate(const mp4ff_t *f, const int track)
{
	return f->track[track]->maxBitrate;
}

long mp4ff_get_track_duration(const mp4ff_t *f, const int track)
{
	return f->track[track]->duration;
}

long mp4ff_get_track_duration_use_offsets(const mp4ff_t *f, const int track)
{
	long duration = mp4ff_get_track_duration(f,track);
	if (duration!=-1)
	{
		long offset = mp4ff_get_sample_offset(f,track,0);
		if (offset > duration) duration = 0;
		else duration -= offset;
	}
	return duration;
}


int mp4ff_num_samples(const mp4ff_t *f, const int track)
{
    int i;
    int total = 0;

    for (i = 0; i < f->track[track]->stts_entry_count; i++)
    {
        total += f->track[track]->stts_sample_count[i];
    }
    return total;
}




unsigned int mp4ff_get_sample_rate(const mp4ff_t *f, const int track)
{
	return f->track[track]->sampleRate;
}

unsigned int mp4ff_get_channel_count(const mp4ff_t * f,const int track)
{
	return f->track[track]->channelCount;
}

unsigned int mp4ff_get_audio_type(const mp4ff_t * f,const int track)
{
	return f->track[track]->audioType;
}

int mp4ff_get_sample_duration_use_offsets(const mp4ff_t *f, const int track, const int sample)
{
	int d,o;
	d = mp4ff_get_sample_duration(f,track,sample);
	if (d!=-1)
	{
		o = mp4ff_get_sample_offset(f,track,sample);
		if (o>d) d = 0;
		else d -= o;
	}
	return d;
}

int mp4ff_get_sample_duration(const mp4ff_t *f, const int track, const int sample)
{
    int i, co = 0;

    for (i = 0; i < f->track[track]->stts_entry_count; i++)
    {
		int delta = f->track[track]->stts_sample_count[i];
		if (sample < co + delta)
			return f->track[track]->stts_sample_delta[i];
		co += delta;
    }
    return (int)(-1);
}

long mp4ff_get_sample_position(const mp4ff_t *f, const int track, const int sample)
{
    int i, co = 0;
	long acc = 0;

    for (i = 0; i < f->track[track]->stts_entry_count; i++)
    {
		int delta = f->track[track]->stts_sample_count[i];
		if (sample < co + delta)
		{
			acc += f->track[track]->stts_sample_delta[i] * (sample - co);
			return acc;
		}
		else
		{
			acc += f->track[track]->stts_sample_delta[i] * delta;
		}
		co += delta;
    }
    return (long)(-1);
}

int mp4ff_get_sample_offset(const mp4ff_t *f, const int track, const int sample)
{
    int i, co = 0;

    for (i = 0; i < f->track[track]->ctts_entry_count; i++)
    {
		int delta = f->track[track]->ctts_sample_count[i];
		if (sample < co + delta)
			return f->track[track]->ctts_sample_offset[i];
		co += delta;
    }
    return 0;
}

int mp4ff_find_sample(const mp4ff_t *f, const int track, const long offset,int * toskip)
{
	int i, co = 0;
	long offset_total = 0;
	mp4ff_track_t * p_track = f->track[track];

	for (i = 0; i < p_track->stts_entry_count; i++)
	{
		int sample_count = p_track->stts_sample_count[i];
		int sample_delta = p_track->stts_sample_delta[i];
		long offset_delta = (long)sample_delta * (long)sample_count;
		if (offset < offset_total + offset_delta)
		{
			long offset_fromstts = offset - offset_total;
			if (toskip) *toskip = (int)(offset_fromstts % sample_delta);
			return co + (int)(offset_fromstts / sample_delta);
		}
		else
		{
			offset_total += offset_delta;
		}
		co += sample_count;
	}
	return (int)(-1);
}

int mp4ff_find_sample_use_offsets(const mp4ff_t *f, const int track, const long offset,int * toskip)
{
	return mp4ff_find_sample(f,track,offset + mp4ff_get_sample_offset(f,track,0),toskip);
}

unsigned int mp4ff_read_sample(mp4ff_t *f, const int track, const int sample)
{
    unsigned int bytes;

    bytes = mp4ff_audio_frame_size(f, track, sample);

    if (bytes==0) return 0;

    mp4ff_set_sample_position(f, track, sample);

    return bytes;
}


int mp4ff_read_sample_v2(mp4ff_t *f, const int track, const int sample,unsigned char *buffer)
{
    int result = 0;
	int size = mp4ff_audio_frame_size(f,track,sample);
	if (size<=0) return 0;
	mp4ff_set_sample_position(f, track, sample);
	result = mp4ff_read_data(f,buffer,size);
	
    return result;
}

int mp4ff_read_sample_getsize(mp4ff_t *f, const int track, const int sample)
{
	int temp = mp4ff_audio_frame_size(f, track, sample);
	if (temp<0) temp = 0;
	return temp;
}

/* Init the library using a DecoderSpecificInfo */
static char AACDecInit2(AACDecHandle hDecoder, unsigned char *pBuffer,
                                 unsigned int SizeOfDecoderSpecificInfo,
                                 unsigned int *samplerate, unsigned char *channels)
{
    char rc;
    mp4AudioSpecificConfig mp4ASC;

    if((hDecoder == NULL)
        || (pBuffer == NULL)
        || (SizeOfDecoderSpecificInfo < 2)
        || (samplerate == NULL)
        || (channels == NULL))
    {
        return -1;
    }

    hDecoder->adif_header_present = 0;
    hDecoder->adts_header_present = 0;

    /* decode the audio specific config */
    rc = AudioSpecificConfig2(pBuffer, SizeOfDecoderSpecificInfo, &mp4ASC,
        &(hDecoder->pce));

    /* copy the relevant info to the decoder handle */
    *samplerate = mp4ASC.samplingFrequency;
    if (mp4ASC.channelsConfiguration)
    {
        *channels = mp4ASC.channelsConfiguration;
    } else {
        *channels = hDecoder->pce.channels;
        hDecoder->pce_set = 1;
    }
#if (defined(PS_DEC) || defined(DRM_PS))
    /* check if we have a mono file */
    if (*channels == 1)
    {
        /* upMatrix to 2 channels for implicit signalling of PS */
        *channels = 2;
    }
#endif
    hDecoder->sf_index = mp4ASC.samplingFrequencyIndex;
    hDecoder->object_type = mp4ASC.objectTypeIndex;
#ifdef ERROR_RESILIENCE
    hDecoder->aacSectionDataResilienceFlag = mp4ASC.aacSectionDataResilienceFlag;
    hDecoder->aacScalefactorDataResilienceFlag = mp4ASC.aacScalefactorDataResilienceFlag;
    hDecoder->aacSpectralDataResilienceFlag = mp4ASC.aacSpectralDataResilienceFlag;
#endif
#ifdef SBR_DEC
    hDecoder->sbr_present_flag = mp4ASC.sbr_present_flag;
    hDecoder->downSampledSBR = mp4ASC.downSampledSBR;
    if (hDecoder->config.dontUpSampleImplicitSBR == 0)
        hDecoder->forceUpSampling = mp4ASC.forceUpSampling;
    else
        hDecoder->forceUpSampling = 0;

    /* AAC core decoder samplerate is 2 times as low */
    if (((hDecoder->sbr_present_flag == 1)&&(!hDecoder->downSampledSBR)) || hDecoder->forceUpSampling == 1)
    {
        hDecoder->sf_index = get_sr_index(mp4ASC.samplingFrequency / 2);
    }
#endif

    if (rc != 0)
    {
        return rc;
    }
    hDecoder->channelConfiguration = mp4ASC.channelsConfiguration;
    if (mp4ASC.frameLengthFlag)
#ifdef ALLOW_SMALL_FRAMELENGTH
        hDecoder->frameLength = 960;
#else
        return -1;
#endif

    /* must be done before frameLength is divided by 2 for LD */
#ifdef SSR_DEC
    if (hDecoder->object_type == SSR)
        hDecoder->fb = ssr_filter_bank_init(hDecoder->frameLength/SSR_BANDS);
    else
#endif
        hDecoder->fb = filter_bank_init(hDecoder->frameLength);

#ifdef LD_DEC
    if (hDecoder->object_type == LD)
        hDecoder->frameLength >>= 1;
#endif

    return 0;
}
static mp4ff_t global_infile;
int Init_MP4file(AUD_tCtrlBlk * pAcb)
{
    tAacAttr * p_tAacAttr = (tAacAttr *)pAcb->pPrivAttr;
    AACDecHandle hDecoder = p_tAacAttr->pDecoder;
    AUD_tAttr *pAttr = &(pAcb->Attr);
    p_tAacAttr->infile = &global_infile;
    mp4ff_t *infile = p_tAacAttr->infile;    
    memset(infile, 0, sizeof(mp4ff_t));

    infile->stream = (mp4ff_callback_t *)malloc(sizeof(mp4ff_callback_t));
    if(!infile->stream)
    {
        return 40;
    }
    infile->stream->read = read_callback;
    infile->stream->seek = seek_callback;
    parse_atoms(infile, 0);
	
    if ((p_tAacAttr->track= GetAACTrack(infile)) < 0)
    {
        return 1;
    }
	
    unsigned char *buffer = NULL;
    int buffer_size = 0;
    mp4ff_get_decoder_config(infile, p_tAacAttr->track, &buffer, &buffer_size);
    if(AACDecInit2(hDecoder, buffer, buffer_size, &pAttr->SampleFreq, &pAttr->NumCh) < 0)
    {
        /* If some error initializing occured, skip the file */        
        AAC_Cleanup(pAcb);
        return 1;
    }
    if (buffer)
        free(buffer);
     
    return 0;
}
