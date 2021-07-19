
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "aacdec.h"
#include <cyg/io/aud/adecctrl.h>
#include "syntax.h"
#include "output.h"
#include "filtbank.h"
#include "drc.h"
#include "bits.h"
#include <cyg/io/aud/adec.h>
#include <cyg/io/aud/UB.h>
#include <pkgconf/devs_aud_aac.h>
#include "audio.h"
#include "mp4ff.h"
#ifdef SBR_DEC
#include "sbr_dec.h"
#include "sbr_syntax.h"
#endif
#ifdef SSR_DEC
#include "ssr.h"
#endif

#ifdef ANALYSIS
unsigned short dbg_count;
#endif
extern UBuff *pABV;
extern unsigned int file_size;
extern FILE *aaclog;
extern unsigned char filename[50];
unsigned int num_frame;
char *sndfile;


#define SPEAKER_FRONT_LEFT             0x1
#define SPEAKER_FRONT_RIGHT            0x2
#define SPEAKER_FRONT_CENTER           0x4
#define SPEAKER_LOW_FREQUENCY          0x8
#define SPEAKER_BACK_LEFT              0x10
#define SPEAKER_BACK_RIGHT             0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER   0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER  0x80
#define SPEAKER_BACK_CENTER            0x100
#define SPEAKER_SIDE_LEFT              0x200
#define SPEAKER_SIDE_RIGHT             0x400
#define SPEAKER_TOP_CENTER             0x800
#define SPEAKER_TOP_FRONT_LEFT         0x1000
#define SPEAKER_TOP_FRONT_CENTER       0x2000
#define SPEAKER_TOP_FRONT_RIGHT        0x4000
#define SPEAKER_TOP_BACK_LEFT          0x8000
#define SPEAKER_TOP_BACK_CENTER        0x10000
#define SPEAKER_TOP_BACK_RIGHT         0x20000
#define SPEAKER_RESERVED               0x80000000

long aacChannelConfig2wavexChannelMask(AACDecFrameInfo *hInfo)
{
    if (hInfo->channels == 6 && hInfo->num_lfe_channels)
    {
        return SPEAKER_FRONT_LEFT + SPEAKER_FRONT_RIGHT +
            SPEAKER_FRONT_CENTER + SPEAKER_LOW_FREQUENCY +
            SPEAKER_BACK_LEFT + SPEAKER_BACK_RIGHT;
    } else {
        return 0;
    }
}


static int aac_frame_decode(AACDecHandle hDecoder, AACDecFrameInfo *hInfo, unsigned char *buffer, unsigned int buffer_size);
static void create_channel_config(AACDecHandle hDecoder, AACDecFrameInfo *hInfo);

int AAC_Init(AUD_tCtrlBlk * pAcb)
{
    tAacAttr * p_tAacAttr;
    AACDecHandle hDecoder;
    int i;
    
    pAcb->pPrivAttr = (tAacAttr*)malloc(sizeof(tAacAttr));
    if (pAcb->pPrivAttr == 0) {
        return 40;
    }
    p_tAacAttr = (tAacAttr*) pAcb->pPrivAttr;

    memset(&(pAcb->Attr),0,sizeof(AUD_tAttr));
	
    hDecoder = p_tAacAttr->pDecoder = (AACDecStruct*) malloc(sizeof(AACDecStruct));     
    if (p_tAacAttr->pDecoder == 0) {
        return 40;
    }

    p_tAacAttr->pFrameInfo= (AACDecFrameInfo*) malloc(sizeof(AACDecFrameInfo));
    if (p_tAacAttr->pFrameInfo == 0) {
        return 40;
    }    
    memset(hDecoder, 0 , sizeof(AACDecStruct));
    
    //hDecoder->cmes = mes;
    hDecoder->config.useOldADTSFormat = 0;  //Default: 0
    hDecoder->config.outputFormat  = AAC_FMT_16BIT;
    hDecoder->config.defObjectType = LC;
    hDecoder->config.defSampleRate = 44100; /* Default: 44.1kHz */
    hDecoder->config.downMatrix = 1;
    hDecoder->upMatrix = 1;
    hDecoder->adts_header_present = 0;
    hDecoder->adif_header_present = 0;
#ifdef ERROR_RESILIENCE
    hDecoder->aacSectionDataResilienceFlag = 0;
    hDecoder->aacScalefactorDataResilienceFlag = 0;
    hDecoder->aacSpectralDataResilienceFlag = 0;
#endif

    hDecoder->sf_index = get_sr_index(hDecoder->config.defSampleRate);
    hDecoder->object_type = hDecoder->config.defObjectType;
    
    hDecoder->frameLength = 1024;

    hDecoder->frame = 0;
    hDecoder->sample_buffer = NULL;

    hDecoder->__r1 = 1;
    hDecoder->__r2 = 1;

    for (i = 0; i < MAX_CHANNELS; i++)
    {
        hDecoder->window_shape_prev[i] = 0;
        hDecoder->time_out[i] = NULL;
        hDecoder->fb_intermed[i] = NULL;
#ifdef SSR_DEC
        hDecoder->ssr_overlap[i] = NULL;
        hDecoder->prev_fmd[i] = NULL;
#endif
#ifdef MAIN_DEC
        hDecoder->pred_stat[i] = NULL;
#endif
#ifdef LTP_DEC
        hDecoder->ltp_lag[i] = 0;
        hDecoder->lt_pred_stat[i] = NULL;
#endif
    }

#ifdef SBR_DEC
    for (i = 0; i < MAX_SYNTAX_ELEMENTS; i++)
    {
        hDecoder->sbr[i] = NULL;
    }
#endif

    hDecoder->drc = drc_init(16384, 16384);
    return 0;
}

int AAC_Decode(AUD_tCtrlBlk * pAcb)
{    
    tAacAttr * p_tAacAttr = (tAacAttr *)pAcb->pPrivAttr;
    AACDecHandle hDecoder = p_tAacAttr->pDecoder;
    AACDecFrameInfo *hInfo = p_tAacAttr->pFrameInfo;
    unsigned int size;
    int ret;
#ifdef AAC_WRITE_WAV
    unsigned long samplerate;
    void *sample_buffer;   
#endif
    audio_file *aufile = NULL;
    unsigned char first_time = 1;
    num_frame = 0;
    if(p_tAacAttr->Tid == 0)
    {
        do
        {
            num_frame++;
            pABV->rdptr += hInfo->bytesconsumed;
            size = file_size - hInfo->bytesconsumed;
            ret = aac_frame_decode(hDecoder, hInfo, (unsigned char *)pABV->rdptr, size);
#ifdef AAC_WRITE_WAV            
            if (first_time && !hInfo->error)
            {
                samplerate = get_sample_rate(hDecoder->sf_index);
                aufile = open_audio_file(sndfile, samplerate, hInfo->channels,
                           1, 1, aacChannelConfig2wavexChannelMask(hInfo));

                if (aufile == NULL)
                {
                    AAC_Cleanup(pAcb);
                    return 40;
                }            
                first_time = 0;
            }
            if ((hInfo->error == 0) && (hInfo->samples > 0))
            {
                sample_buffer = hDecoder->sample_buffer;
                write_audio_file(aufile, sample_buffer, hInfo->samples, 0);
            }
#endif
            if(hInfo->error)
            {
                break;
            }
        }while(ret == 0);
    }
    else
    {
        ABV_reset();
        int sampleId, numSamples; 
        numSamples = mp4ff_num_samples(p_tAacAttr->infile, p_tAacAttr->track);
        for (sampleId = 0; sampleId < numSamples; sampleId++)
        {            
            num_frame++;
            size = mp4ff_read_sample(p_tAacAttr->infile, p_tAacAttr->track, sampleId);
            if(size == 0)
            {
                continue;
            }
            ret = aac_frame_decode(hDecoder, hInfo, (unsigned char *)pABV->rdptr, size);
#ifdef AAC_WRITE_WAV
            if (first_time && !hInfo->error)
            {           
                {
                     samplerate = get_sample_rate(hDecoder->sf_index);
		        aufile = open_audio_file(sndfile, samplerate, hInfo->channels,
                         1, 1, aacChannelConfig2wavexChannelMask(hInfo));

                    if (aufile == NULL)
                    {
                        AAC_Cleanup(pAcb);
                        return 40;
                    }
                }
                first_time = 0;
            }

            sample_buffer = hDecoder->sample_buffer;
            if ((hInfo->error == 0) && (hInfo->samples > 0))
            {
                write_audio_file(aufile, sample_buffer, hInfo->samples, 0);
            }
#endif
            if(hInfo->error)
            {
                break;
            }
        }        
    }
    if (!first_time)
    {
        if(aufile)
        {
            close_audio_file(aufile);
        }
    }
    if(hInfo->error)
    {
        return hInfo->error;
    }
    else
    {
        return 0;    
    }
}

int AAC_Cleanup(AUD_tCtrlBlk * pAcb)
{
    tAacAttr * p_tAacAttr = (tAacAttr *)pAcb->pPrivAttr;
    AACDecHandle hDecoder = p_tAacAttr->pDecoder;
    unsigned char i;

    if (hDecoder == NULL)
        return 41;

#ifdef PROFILE
    printf("AAC decoder total:  %I64d cycles\n", hDecoder->cycles);
    printf("requant:            %I64d cycles\n", hDecoder->requant_cycles);
    printf("spectral_data:      %I64d cycles\n", hDecoder->spectral_cycles);
    printf("scalefactors:       %I64d cycles\n", hDecoder->scalefac_cycles);
    printf("output:             %I64d cycles\n", hDecoder->output_cycles);
#endif

    if(p_tAacAttr->Tid == 1)
    {
        mp4ff_close(p_tAacAttr->infile);
    }

    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if (hDecoder->time_out[i]) 
	  {
	  	free(hDecoder->time_out[i]);
	  	hDecoder->time_out[i] = NULL;
        }
        if (hDecoder->fb_intermed[i])
	  {
	  	free(hDecoder->fb_intermed[i]);
	  	hDecoder->fb_intermed[i] = NULL;
        }
#ifdef SSR_DEC
        if (hDecoder->ssr_overlap[i])
	  {
	  	free(hDecoder->ssr_overlap[i]);
	  	hDecoder->ssr_overlap[i] = NULL;
	  }
        if (hDecoder->prev_fmd[i]) 
	  {
	  	free(hDecoder->prev_fmd[i]);
	  	hDecoder->prev_fmd[i] = NULL;
	  }
#endif
#ifdef MAIN_DEC
        if (hDecoder->pred_stat[i]) 
	  {
	  	free(hDecoder->pred_stat[i]);
		hDecoder->pred_stat[i] = NULL;
	  }
#endif
#ifdef LTP_DEC
        if (hDecoder->lt_pred_stat[i]) 
	  {
	  	free(hDecoder->lt_pred_stat[i]);
		hDecoder->lt_pred_stat[i] = NULL;
	  }
#endif
    }

#ifdef SSR_DEC
    if (hDecoder->object_type == SSR)
        ssr_filter_bank_end(hDecoder->fb);
    else
#endif
        filter_bank_end(hDecoder->fb);

    drc_end(hDecoder->drc);

    if (hDecoder->sample_buffer) 
    {
         free(hDecoder->sample_buffer);
	   hDecoder->sample_buffer = NULL;
    }

#ifdef SBR_DEC
    for (i = 0; i < MAX_SYNTAX_ELEMENTS; i++)
    {
        if (hDecoder->sbr[i])
            sbrDecodeEnd(hDecoder->sbr[i]);
    }
#endif

    if (hDecoder) 
    {
        free(hDecoder);
	  hDecoder = NULL;
    }
    if(pAcb->pPrivAttr)
    {
        free(pAcb->pPrivAttr);
	  pAcb->pPrivAttr = NULL;
    }
    return 0;
}

static int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};

static void bitrate_length(int samplerate, int frames, int total_len, int *bitrate, float *length)
{
    float frames_per_sec, bytes_per_frame;
    frames_per_sec = (float)samplerate / 1024.0f;
    if(frames != 0)
    {
        bytes_per_frame = (float)total_len / (float)(frames * 1000);
    }
    else
    {
        bytes_per_frame = 0;
    }
    *bitrate = (int)(8. * bytes_per_frame * frames_per_sec + 0.5);
    if(frames_per_sec != 0)
    {
        *length = (float)frames/frames_per_sec;
    }
    else
    {
        *length = 1;
    }
}

static void adts_parse(int *bitrate, float *length)
{
    int frames, frame_length;
    int t_framelength = 0;
    int samplerate;
    
    /* Read all frames to ensure correct time and bitrate */
    for (frames = 0; /* */; frames++)
    {       
        unsigned char *buffer = (unsigned char *)pABV->rdptr;
        /* check syncword */
        if (!((buffer[0] == 0xFF)&&((buffer[1] & 0xF6) == 0xF0)))
           break;

        if (frames == 0)
            samplerate = adts_sample_rates[(buffer[2]&0x3c)>>2];

        frame_length = ((((unsigned int)buffer[3] & 0x3)) << 11)
            | (((unsigned int)buffer[4]) << 3) | (buffer[5] >> 5);

        t_framelength += frame_length;
        if(t_framelength >= file_size)
        {
            break;
        }

        Skip_forward(frame_length, 1);        
    }
    bitrate_length(samplerate, frames, t_framelength, bitrate, length);
}

int AAC_GetAttr(AUD_tCtrlBlk * pAcb)
{
    AUD_tAttr *pAttr = &(pAcb->Attr);
    unsigned char *buffer = (unsigned char *)pAcb->Abv.start;    
    tAacAttr * p_tAacAttr = (tAacAttr *)pAcb->pPrivAttr;
    AACDecHandle hDecoder = p_tAacAttr->pDecoder;
    int bitrate;
    float length;
    unsigned char file_type;
    int ret;
	
     if((buffer[4] == 'f') && (buffer[5] == 't') &&
		(buffer[6] == 'y') && (buffer[7] == 'p'))
    {
        p_tAacAttr->Tid = 1;
        file_type = 2;
        ret = Init_MP4file(pAcb);
        if(ret)
        {
            return ret;
        }
        int sampleId, numSamples;     
        unsigned int total_frame_length = 0;
        numSamples = mp4ff_num_samples(p_tAacAttr->infile, p_tAacAttr->track);
        for (sampleId = 0; sampleId < numSamples; sampleId++)
        {                      
            total_frame_length += mp4ff_audio_frame_size(p_tAacAttr->infile, p_tAacAttr->track, sampleId);
        }
        bitrate_length(pAttr->SampleFreq, numSamples, total_frame_length, &bitrate, &length);
    }
    else
    {
        p_tAacAttr->Tid = 0;     
    
        adts_header *pAdts = &p_tAacAttr->Adts;
        adif_header *pAdif = &p_tAacAttr->Adif;

        unsigned long tagsize;	
    
        if((buffer[0] == 'I') && (buffer[1] == 'D') && (buffer[2] == '3'))
        {
            tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
                (buffer[8] <<  7) | (buffer[9] <<  0);
            tagsize += 10;
        }
        else
        {
            tagsize = 0; 
        }
        pABV->rdptr += tagsize;
        AACDecFrameInfo * pFrameInfo = p_tAacAttr->pFrameInfo;
          
        /* Check if an ADIF header is present */
        if ((buffer[tagsize] == 'A') && (buffer[tagsize + 1] == 'D') &&
            (buffer[tagsize + 2] == 'I') && (buffer[tagsize + 3] == 'F'))
        {
            bitfile ld;
            Initbits(&ld, (unsigned char *)pABV->rdptr, file_size);
            file_type = 0;
            hDecoder->adif_header_present = 1;
  
            get_adif_header(&ld, pAdif);        
 
            hDecoder->sf_index = pAdif->pce[0].sf_index;
            hDecoder->object_type = pAdif->pce[0].object_type + 1;

            pAttr->SampleFreq = get_sample_rate(hDecoder->sf_index);
            pAttr->NumCh = pAdif->pce[0].channels;

            memcpy(&(hDecoder->pce), &(pAdif->pce[0]), sizeof(program_config));
            hDecoder->pce_set = 1;
            Byte_align(&ld);
            pFrameInfo->bytesconsumed = bit2byte(Get_processed_bits(&ld)) + tagsize;
            bitrate = pAdif->bitrate;
       
            length = (float)file_size;
            if (length != 0)
            {
                length = ((float)length*8.f)/((float)bitrate) + 0.5f;
            }
        }
        else if (((buffer[tagsize] == 0xFF)&&((buffer[tagsize + 1] & 0xF6) == 0xF0))) 
        {
            bitfile ld;
            file_type = 1;
            adts_parse(&bitrate, &length);
            ABV_reset();
            Initbits(&ld, (unsigned char *)pABV->rdptr, file_size);
            hDecoder->adts_header_present = 1;

            pAdts->old_format = hDecoder->config.useOldADTSFormat;
            if ((pFrameInfo->error = adts_frame(&ld, pAdts)) > 0 ) 
            {
	         return pFrameInfo->error;
            }
            pFrameInfo->bytesconsumed = tagsize;
            hDecoder->sf_index = pAdts->sf_index;
            hDecoder->object_type = pAdts->profile + 1;

            pAttr->SampleFreq = get_sample_rate(hDecoder->sf_index);
            pAttr->NumCh = (pAdts->channel_configuration > 6) ?
               2 : pAdts->channel_configuration;
        }    

#if (defined(PS_DEC) || defined(DRM_PS))
    /* check if we have a mono file */
        if (pAttr->NumCh == 1)
        {
            /* upMatrix to 2 channels for implicit signalling of PS */
            pAttr->NumCh = 2;
        }
#endif

        pAttr->BitsPerSamp = 16;    
        pAttr->ChOrder[SPK_LEFT] = 0;
        pAttr->ChOrder[SPK_RIGHT] = 1;

        hDecoder->channelConfiguration = pAttr->NumCh;

#ifdef SBR_DEC
        /* implicit signalling */
        if(hDecoder->sbr_present_flag == 1)
        {
            if (pAttr->SampleFreq <= 24000 && !(hDecoder->config.dontUpSampleImplicitSBR))
            {
                pAttr->SampleFreq *= 2;
                hDecoder->forceUpSampling = 1;
            } else if (pAttr->SampleFreq > 24000 && !(hDecoder->config.dontUpSampleImplicitSBR)) {
                hDecoder->downSampledSBR = 1;
            }
        }
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
        if (can_decode_ot(hDecoder->object_type) < 0)
            return 1;
    }
/***************************************************************/  
#ifdef OUTPUT_AAC_PERFORMANCE
    unsigned char *ft[3] = {"ADIF file", "ADTS file", "MPEG-4 file"};
    char *ot[6] = { "NULL", "MAIN AAC", "LC AAC", "SSR AAC", "LTP AAC", "HE AAC" };  
    fprintf(aaclog, "***************   AAC DECODER   ***************\n");
    fprintf(aaclog, "File size:     %d Bytes\n", file_size);      
    fprintf(aaclog, "File type:     %s\n", ft[file_type]);
    fprintf(aaclog, "Profile:     %s\n", ot[hDecoder->object_type]);
    fprintf(aaclog, "Sample Frequency:     %d Hz\n",pAttr->SampleFreq);
    fprintf(aaclog, "Bitrate:     %d kbps\n", bitrate);
    fprintf(aaclog, "Sond Length:     %.2f secs\n", length);
    fprintf(aaclog, "Channels:     %d\n",pAttr->NumCh);
#endif
/**************************************************************/
    return 0;
}

static void create_channel_config(AACDecHandle hDecoder, AACDecFrameInfo *hInfo)
{
    hInfo->num_front_channels = 0;
    hInfo->num_side_channels = 0;
    hInfo->num_back_channels = 0;
    hInfo->num_lfe_channels = 0;
    memset(hInfo->channel_position, 0, MAX_CHANNELS*sizeof(unsigned char));

    if (hDecoder->downMatrix)
    {
        hInfo->num_front_channels = 2;
        hInfo->channel_position[0] = FRONT_CHANNEL_LEFT;
        hInfo->channel_position[1] = FRONT_CHANNEL_RIGHT;
        return;
    }

    /* check if there is a PCE */
    if (hDecoder->pce_set)
    {
        unsigned char i, chpos = 0;
        unsigned char chdir, back_center = 0;

        hInfo->num_front_channels = hDecoder->pce.num_front_channels;
        hInfo->num_side_channels = hDecoder->pce.num_side_channels;
        hInfo->num_back_channels = hDecoder->pce.num_back_channels;
        hInfo->num_lfe_channels = hDecoder->pce.num_lfe_channels;

        chdir = hInfo->num_front_channels;
        if (chdir & 1)
        {
#if (defined(PS_DEC) || defined(DRM_PS))
            /* When PS is enabled output is always stereo */
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_RIGHT;
#else
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_CENTER;
            chdir--;
#endif
        }
        for (i = 0; i < chdir; i += 2)
        {
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_RIGHT;
        }

        for (i = 0; i < hInfo->num_side_channels; i += 2)
        {
            hInfo->channel_position[chpos++] = SIDE_CHANNEL_LEFT;
            hInfo->channel_position[chpos++] = SIDE_CHANNEL_RIGHT;
        }

        chdir = hInfo->num_back_channels;
        if (chdir & 1)
        {
            back_center = 1;
            chdir--;
        }
        for (i = 0; i < chdir; i += 2)
        {
            hInfo->channel_position[chpos++] = BACK_CHANNEL_LEFT;
            hInfo->channel_position[chpos++] = BACK_CHANNEL_RIGHT;
        }
        if (back_center)
        {
            hInfo->channel_position[chpos++] = BACK_CHANNEL_CENTER;
        }

        for (i = 0; i < hInfo->num_lfe_channels; i++)
        {
            hInfo->channel_position[chpos++] = LFE_CHANNEL;
        }

    } else {
        switch (hDecoder->channelConfiguration)
        {
        case 1:
#if (defined(PS_DEC) || defined(DRM_PS))
            /* When PS is enabled output is always stereo */
            hInfo->num_front_channels = 2;
            hInfo->channel_position[0] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[1] = FRONT_CHANNEL_RIGHT;
#else
            hInfo->num_front_channels = 1;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
#endif
            break;
        case 2:
            hInfo->num_front_channels = 2;
            hInfo->channel_position[0] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[1] = FRONT_CHANNEL_RIGHT;
            break;
        case 3:
            hInfo->num_front_channels = 3;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
            hInfo->channel_position[1] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[2] = FRONT_CHANNEL_RIGHT;
            break;
        case 4:
            hInfo->num_front_channels = 3;
            hInfo->num_back_channels = 1;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
            hInfo->channel_position[1] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[2] = FRONT_CHANNEL_RIGHT;
            hInfo->channel_position[3] = BACK_CHANNEL_CENTER;
            break;
        case 5:
            hInfo->num_front_channels = 3;
            hInfo->num_back_channels = 2;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
            hInfo->channel_position[1] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[2] = FRONT_CHANNEL_RIGHT;
            hInfo->channel_position[3] = BACK_CHANNEL_LEFT;
            hInfo->channel_position[4] = BACK_CHANNEL_RIGHT;
            break;
        case 6:
            hInfo->num_front_channels = 3;
            hInfo->num_back_channels = 2;
            hInfo->num_lfe_channels = 1;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
            hInfo->channel_position[1] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[2] = FRONT_CHANNEL_RIGHT;
            hInfo->channel_position[3] = BACK_CHANNEL_LEFT;
            hInfo->channel_position[4] = BACK_CHANNEL_RIGHT;
            hInfo->channel_position[5] = LFE_CHANNEL;
            break;
        case 7:
            hInfo->num_front_channels = 3;
            hInfo->num_side_channels = 2;
            hInfo->num_back_channels = 2;
            hInfo->num_lfe_channels = 1;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
            hInfo->channel_position[1] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[2] = FRONT_CHANNEL_RIGHT;
            hInfo->channel_position[3] = SIDE_CHANNEL_LEFT;
            hInfo->channel_position[4] = SIDE_CHANNEL_RIGHT;
            hInfo->channel_position[5] = BACK_CHANNEL_LEFT;
            hInfo->channel_position[6] = BACK_CHANNEL_RIGHT;
            hInfo->channel_position[7] = LFE_CHANNEL;
            break;
        default: /* channelConfiguration == 0 || channelConfiguration > 7 */
            {
                unsigned char i;
                unsigned char ch = hDecoder->fr_channels - hDecoder->has_lfe;
                if (ch & 1) /* there's either a center front or a center back channel */
                {
                    unsigned char ch1 = (ch-1)/2;
                    if (hDecoder->first_syn_ele == ID_SCE)
                    {
                        hInfo->num_front_channels = ch1 + 1;
                        hInfo->num_back_channels = ch1;
                        hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
                        for (i = 1; i <= ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }
                        for (i = ch1+1; i < ch; i+=2)
                        {
                            hInfo->channel_position[i] = BACK_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = BACK_CHANNEL_RIGHT;
                        }
                    } else {
                        hInfo->num_front_channels = ch1;
                        hInfo->num_back_channels = ch1 + 1;
                        for (i = 0; i < ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }
                        for (i = ch1; i < ch-1; i+=2)
                        {
                            hInfo->channel_position[i] = BACK_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = BACK_CHANNEL_RIGHT;
                        }
                        hInfo->channel_position[ch-1] = BACK_CHANNEL_CENTER;
                    }
                } else {
                    unsigned char ch1 = (ch)/2;
                    hInfo->num_front_channels = ch1;
                    hInfo->num_back_channels = ch1;
                    if (ch1 & 1)
                    {
                        hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
                        for (i = 1; i <= ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }
                        for (i = ch1+1; i < ch-1; i+=2)
                        {
                            hInfo->channel_position[i] = BACK_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = BACK_CHANNEL_RIGHT;
                        }
                        hInfo->channel_position[ch-1] = BACK_CHANNEL_CENTER;
                    } else {
                        for (i = 0; i < ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }
                        for (i = ch1; i < ch; i+=2)
                        {
                            hInfo->channel_position[i] = BACK_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = BACK_CHANNEL_RIGHT;
                        }
                    }
                }
                hInfo->num_lfe_channels = hDecoder->has_lfe;
                for (i = ch; i < hDecoder->fr_channels; i++)
                {
                    hInfo->channel_position[i] = LFE_CHANNEL;
                }
            }
            break;
        }
    }
}

#ifdef DRM

#define ERROR_STATE_INIT 6

static void conceal_output(AACDecHandle hDecoder, unsigned short frame_len,
                           unsigned char out_ch, void *sample_buffer)
{
    return;
}
#endif


static int aac_frame_decode(AACDecHandle hDecoder, AACDecFrameInfo *hInfo, unsigned char *buffer, unsigned int buffer_size)
{
    unsigned short i;
    unsigned char channels = 0;
    unsigned char output_channels = 0;
    unsigned int bitsconsumed;
    unsigned short frame_len;
    bitfile ld = {0};

#ifdef PROFILE
    unsigned short count = faad_get_ts();
#endif

    /* safety checks */
    if ((hDecoder == NULL) || (hInfo == NULL))
    {
        return 41;
    }

    frame_len = hDecoder->frameLength;


    memset(hInfo, 0, sizeof(AACDecFrameInfo));
    memset(hDecoder->internal_channel, 0, MAX_CHANNELS*sizeof(hDecoder->internal_channel[0]));

#ifdef USE_TIME_LIMIT
    if ((TIME_LIMIT * get_sample_rate(hDecoder->sf_index)) > hDecoder->TL_count)
    {
        hDecoder->TL_count += 1024;
    } else {
        hInfo->error = (NUM_ERROR_MESSAGES-1);
        goto error;
    }
#endif

    /* check for some common metadata tag types in the bitstream
     * No need to return an error
     */
    /* ID3 */
    if (memcmp(buffer, "TAG", 3) == 0)
    {
        /* found it */
        hInfo->bytesconsumed = 128; /* 128 bytes fixed size */
        /* no error, but no output either */
        return 0;
    }
    Initbits(&ld, buffer, buffer_size);

#ifdef DRM
    if (hDecoder->object_type == DRM_ER_LC)
    {
        /* We do not support stereo right now */
        if (0) //(hDecoder->channelConfiguration == 2)
        {
            hInfo->error = 28; // Throw CRC error
            goto error;
        }

        Getbits(&ld, 8);
    }
#endif


    if (hDecoder->adts_header_present)
    {
        adts_header adts;

        adts.old_format = hDecoder->config.useOldADTSFormat;
        if ((hInfo->error = adts_frame(&ld, &adts)) > 0)
            goto error;

        /* MPEG2 does byte_alignment() here,
         * but ADTS header is always multiple of 8 bits in MPEG2
         * so not needed to actually do it.
         */
    }


#ifdef ANALYSIS
    dbg_count = 0;
#endif

    /* decode the complete bitstream */
#ifdef DRM
    if (/*(hDecoder->object_type == 6) ||*/ (hDecoder->object_type == DRM_ER_LC))
    {
        DRM_aac_scalable_main_element(hDecoder, hInfo, &ld, &hDecoder->pce, hDecoder->drc);
    } else {
#endif
        raw_data_block(&ld, hDecoder, hInfo, &hDecoder->pce, hDecoder->drc);
#ifdef DRM
    }
#endif

    channels = hDecoder->fr_channels;

    if (hInfo->error > 0)
        goto error;

    /* safety check */
    if (channels == 0 || channels > MAX_CHANNELS)
    {
        /* invalid number of channels */
        hInfo->error = 12;
        goto error;
    }

    /* no more bit reading after this */
    bitsconsumed = Get_processed_bits(&ld);
    hInfo->bytesconsumed = bit2byte(bitsconsumed);
    
    if (!hDecoder->adts_header_present && !hDecoder->adif_header_present)
    {
        if (hDecoder->channelConfiguration == 0)
            hDecoder->channelConfiguration = channels;

        if (channels == 8) /* 7.1 */
            hDecoder->channelConfiguration = 7;
        if (channels == 7) /* not a standard channelConfiguration */
            hDecoder->channelConfiguration = 0;
    }

    if ((channels == 5 || channels == 6) && hDecoder->config.downMatrix)
    {
        hDecoder->downMatrix = 1;
        output_channels = 2;
    } else {
        output_channels = channels;
    }

#if (defined(PS_DEC) || defined(DRM_PS))
    hDecoder->upMatrix = 0;
    /* check if we have a mono file */
    if (output_channels == 1)
    {
        /* upMatrix to 2 channels for implicit signalling of PS */
        hDecoder->upMatrix = 1;
        output_channels = 2;
    }
#endif

    if(output_channels == 1)
    {
        if(hDecoder->upMatrix == 1)
        {
            output_channels = 2;
            hDecoder->downMatrix = 0;
        }
    }

    /* Make a channel configuration based on either a PCE or a channelConfiguration */
    create_channel_config(hDecoder, hInfo);

    /* number of samples in this frame */
    hInfo->samples = frame_len*output_channels;
    /* number of channels in this frame */
    hInfo->channels = output_channels;
    /* samplerate */
    hInfo->samplerate = get_sample_rate(hDecoder->sf_index);
    /* object type */
    hInfo->object_type = hDecoder->object_type;
    /* sbr */
    hInfo->sbr = NO_SBR;
    /* header type */
    hInfo->header_type = RAW;
    if (hDecoder->adif_header_present)
        hInfo->header_type = ADIF;
    if (hDecoder->adts_header_present)
        hInfo->header_type = ADTS;
#if (defined(PS_DEC) || defined(DRM_PS))
    hInfo->ps = hDecoder->ps_used_global;
#endif

    /* check if frame has channel elements */
    if (channels == 0)
    {
        hDecoder->frame++;
        return 42;
    }

    /* allocate the buffer for the final samples */
    if ((hDecoder->sample_buffer == NULL) ||
        (hDecoder->alloced_channels != output_channels))
    {
        static const unsigned char str[] = { sizeof(short), sizeof(int), sizeof(int),
            sizeof(float), sizeof(double), sizeof(short), sizeof(short),
            sizeof(short), sizeof(short), 0, 0, 0 };
        unsigned char stride = str[hDecoder->config.outputFormat-1];
#ifdef SBR_DEC
        if (((hDecoder->sbr_present_flag == 1)&&(!hDecoder->downSampledSBR)) || (hDecoder->forceUpSampling == 1))
        {
            stride = 2 * stride;
        }
#endif
        /* check if we want to use internal sample_buffer */
        if (hDecoder->sample_buffer)
        {
        	free(hDecoder->sample_buffer);
             hDecoder->sample_buffer = NULL;
        }
        
        hDecoder->sample_buffer = malloc(frame_len*output_channels*stride);
        
        hDecoder->alloced_channels = output_channels;
    }
    
#ifdef SBR_DEC
    if ((hDecoder->sbr_present_flag == 1) || (hDecoder->forceUpSampling == 1))
    {
        unsigned char ele;

        /* this data is different when SBR is used or when the data is upsampled */
        if (!hDecoder->downSampledSBR)
        {
            frame_len *= 2;
            hInfo->samples *= 2;
            hInfo->samplerate *= 2;
        }

        /* check if every element was provided with SBR data */
        for (ele = 0; ele < hDecoder->fr_ch_ele; ele++)
        {
            if (hDecoder->sbr[ele] == NULL)
            {
                hInfo->error = 25;
                goto error;
            }
        }

        /* sbr */
        if (hDecoder->sbr_present_flag == 1)
        {
            hInfo->object_type = HE_AAC;
            hInfo->sbr = SBR_UPSAMPLED;
        } else {
            hInfo->sbr = NO_SBR_UPSAMPLED;
        }
        if (hDecoder->downSampledSBR)
        {
            hInfo->sbr = SBR_DOWNSAMPLED;
        }
    }
#endif
    hDecoder->sample_buffer = output_to_PCM(hDecoder, hDecoder->time_out, hDecoder->sample_buffer,
        output_channels, frame_len, hDecoder->config.outputFormat);


#ifdef DRM
    //conceal_output(hDecoder, frame_len, output_channels, sample_buffer);
#endif


    hDecoder->postSeekResetFlag = 0;

    hDecoder->frame++;
#ifdef LD_DEC
    if (hDecoder->object_type != LD)
    {
#endif
        if (hDecoder->frame < 1)
            hInfo->samples = 0;
#ifdef LD_DEC
    } else {
        /* LD encoders will give lower delay */
        if (hDecoder->frame <= 0)
            hInfo->samples = 0;
    }
#endif

    /* cleanup */
#ifdef ANALYSIS
    fflush(stdout);
#endif

#ifdef PROFILE
    count = faad_get_ts() - count;
    hDecoder->cycles += count;
#endif

    return 0;

error:


#ifdef DRM
    hDecoder->error_state = ERROR_STATE_INIT;
#endif

    /* reset filterbank state */
    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if (hDecoder->fb_intermed[i] != NULL)
        {
            memset(hDecoder->fb_intermed[i], 0, hDecoder->frameLength*sizeof(real_t));
        }
    }
#ifdef SBR_DEC
    if(hDecoder->sbr_present_flag == 1)
    {
        for (i = 0; i < MAX_SYNTAX_ELEMENTS; i++)
        {
            if (hDecoder->sbr[i] != NULL)
            {
                sbrReset(hDecoder->sbr[i]);
            }
        }
    }
#endif


    /* cleanup */
#ifdef ANALYSIS
    fflush(stdout);
#endif

    return 1;
}

struct AUD_tModule AUD_ModuleAAC = {
    .Type = AUDTYPE_AAC,
    .Init = AAC_Init,
    .Decode = AAC_Decode,
    .Cleanup = AAC_Cleanup,
    .GetAttr = AAC_GetAttr,
};


