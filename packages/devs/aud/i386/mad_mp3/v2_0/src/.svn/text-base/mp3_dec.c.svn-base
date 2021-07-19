//from libmad

#include <cyg/infra/cyg_type.h>
#include <cyg/io/aud/adecctrl.h>
#include <pkgconf/io_aud.h>
#include <stdio.h>
#include <unistd.h>
#include <cyg/fileio/fileio.h>
#include <pkgconf/devs_mad_mp3.h>
#include <cyg/io/iis/io_iis.h>
#include "mp3_info.h"
#include "mad.h"

#ifdef OUTPUT_WAV
#include "wav.h"
#endif

#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif

static struct mad_decoder decoder_mp3 = {0};
struct mad_stream *stream;
struct mad_frame *frame;
struct mad_synth *synth;
static	enum mad_flow (*error_func)(void *, struct mad_stream *, struct mad_frame *);
static	void *error_data;
static	int bad_last_frame = 0;

#ifdef OUTPUT_WAV
FILE *outf = NULL;
char outpath[50];
static int wavesize = 0;
#endif

AUD_tCtrlBlk *pAcb_MP3;
struct buffer
{
	unsigned char 	       *start;
	unsigned int			length;
};

//
//  The following utility routine performs simple rounding, clipping, and
//  scaling of MAD's high-resolution samples down to 16 bits. It does not
//  perform any dithering or noise shaping, which would be recommended to
//  obtain any exceptional audio quality. It is therefore not recommended to
//  use this routine if high-quality output is desired.
// 

static inline
signed int scale(mad_fixed_t sample)
{
	// round
	sample += (1L << (MAD_F_FRACBITS - 16));//4096

	// clip 
	if (sample >= MAD_F_ONE)
	{
		sample = MAD_F_ONE - 1;
	}
	else if (sample < -MAD_F_ONE)
	{
		sample = -MAD_F_ONE;
	}
	// quantize
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}



//
//  This is the input callback. The purpose of this callback is to (re)fill
//  the stream buffer which is to be decoded. In this example, an entire file
//  has been mapped into memory, so we just call mad_stream_buffer() with the
//  address and length of the mapping. When this callback is called a second
//  time, we are finished decoding.
// 

static enum mad_flow input(void *data, struct mad_stream *stream)
{
	struct buffer *buffer = data;

	if (!buffer->length)
	{
		return MAD_FLOW_STOP;
	}
	mad_stream_buffer(stream, buffer->start, buffer->length);

	buffer->length = 0;

	return MAD_FLOW_CONTINUE;
}


// 
//  This is the output callback function. It is called after each frame of
//  MPEG audio data has been completely decoded. The purpose of this callback
//  is to output (or play) the decoded out_file audio.
// 


#ifdef OUTPUT_WAV
static enum mad_flow output_dbv(void *data, struct mad_header const * header, struct mad_pcm *pcm)
{
	unsigned int nchannels, nsamples;
	mad_fixed_t const * left_ch, * right_ch;
    char* output_buf = NULL;

      if(outf == NULL)
      {
        return MAD_FLOW_CONTINUE;
      }

	nchannels = pcm->channels;
	nsamples = pcm->length;
	left_ch = pcm->samples[0];
	right_ch = pcm->samples[1];

	wavesize += nsamples * nchannels;
	output_buf = UB_getBufferData(&(pAcb_MP3->Dbv), nsamples * nchannels * 2, outf);

    //todo. need to remove to io layer
    if(!output_buf)
    {
        diag_printf("DBV is NULL!!\n");
    }
	while (nsamples--)
	{
		signed int sample;

		// output sample(s) in 16-bit signed little-endian PCM
		sample = scale(*left_ch++);
		output_buf[0] = (char)((sample >> 0) & 0xff);
		output_buf[1] = (char)((sample >> 8) & 0xff);
        output_buf += 2;
		if (nchannels == 2)
		{
			sample = scale(*right_ch++);
		    output_buf[0] = (char)((sample >> 0) & 0xff);
		    output_buf[1] = (char)((sample >> 8) & 0xff);
            output_buf += 2;
		}
	}
	UB_update_wrptr(&(pAcb_MP3->Dbv), output_buf + nsamples * nchannels *2);

	return MAD_FLOW_CONTINUE;
}
#endif //OUTPUT_WAV
#ifdef OUTPUT_I2S
static enum mad_flow output_dbv(void *data, struct mad_header const * header, struct mad_pcm *pcm)
{
	unsigned int nchannels, nsamples;
	mad_fixed_t const * left_ch, * right_ch;
    char* output_buf = NULL;
    
	nchannels = pcm->channels;
	nsamples = pcm->length;
	left_ch = pcm->samples[0];
	right_ch = pcm->samples[1];
    output_buf = UB_getBufferData(&(pAcb_MP3->Dbv), nsamples * 2 * 2);

    //todo. need to remove to io layer
    if(!output_buf)
    {
        diag_printf("DBV is NULL!!\n");
    }	
	while (nsamples--)
	{
		signed int sample;

		sample = scale(*left_ch++);
		output_buf[1] = (char)((sample >> 0) & 0xff);
		output_buf[0] = (char)((sample >> 8) & 0xff);
        output_buf += 2;
        
		//nchannels = 1, right_ch = left_ch
		if (nchannels == 2)
		{
			sample = scale(*right_ch++);
        }
	    output_buf[1] = (char)((sample >> 0) & 0xff);
	    output_buf[0] = (char)((sample >> 8) & 0xff);
		output_buf += 2;
	}
	UB_update_wrptr(&(pAcb_MP3->Dbv), output_buf + nsamples * 2 * 2);

	return MAD_FLOW_CONTINUE;
}
#endif //OUTPUT_I2S


//
//  This is the error callback function. It is called whenever a decoding
//  error occurs. The error is indicated by stream->error; the list of
//  possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
//  header file.
// 

static
enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	struct buffer *buffer = data;

	fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n", stream->error, mad_stream_errorstr(stream), stream->this_frame - buffer->start);

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */

	return MAD_FLOW_CONTINUE;
}

static
enum mad_flow error_default(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	int *bad_last_frame = data;

	switch (stream->error)
	{
		case MAD_ERROR_BADCRC:
			if (*bad_last_frame)
			{
				mad_frame_mute(frame);
			}
			else
			{
				*bad_last_frame = 1;
			}

			return MAD_FLOW_IGNORE;

		default:
			return MAD_FLOW_CONTINUE;
	}
}


//***********************************************************************
//   Initialization Functions
// ***********************************************************************
static struct buffer buffer = {0};
int AUD_MP3_Init(AUD_tCtrlBlk *pAcb)
{
	unsigned char* first_frame = NULL;
	unsigned int c_sampling_rate;
	//need to do
	pAcb_MP3 = pAcb;
	if(!pAcb_MP3)
	{
	    return 0;
	}
	if(pAcb->Abv.wr_ok)
	{
        pAcb->Attr.c_ID3v1 = 0;
        pAcb->Attr.c_ID3v2 = 1;
        // 1. check for ID3 tags
        //
        // 1.1 check for ID3v1 tag

    	if(pAcb->Abv.size > 128 && memcmp(pAcb->Abv.end - 127,"TAG",3) == 0) 
    	{ // we found ID3v1 tag
    	    //pAcb->Abv.end -= 128;
    	    pAcb->Abv.size -= 128;
    		pAcb->Attr.c_ID3v1 = 1;
    	}
    	
    	// 1.2 check for ID3v2 tag
    	   // ID3V2 need to do.
        buffer.start  = (unsigned char *)pAcb->Abv.start;
        buffer.length = pAcb->Abv.size;
//        LoadID3(pAcb, ID3_VERSION1);
    }
    else
    {
        EPRINTF(("ABV ERROR \n"));
        return 0;
    }
#ifdef OUTPUT_WAV
	if(NULL == (outf = fopen(outpath, "w")))
	{
		EPRINTF(("Failed to create wav file\n"));
		return 0;
	}
	/*  write wav header */
	WriteWaveHeader(outf, 0x7FFFFFFF, 44100, 2, 16);//pAcb->Attr.SampleFreq
#endif //OUTPUT_WAV
    //try to allocate Dbv buffer
    mad_decoder_init(&decoder_mp3, &buffer, input, 0 /* header */, 0 /* filter */, output_dbv, error, 0 /* message */);
	decoder_mp3.sync = malloc(sizeof(*decoder_mp3.sync));
    if (decoder_mp3.sync == 0)
    {
        return 0;
    }
	stream = &((&decoder_mp3)->sync->stream);
	frame = &((&decoder_mp3)->sync->frame);
	synth = &((&decoder_mp3)->sync->synth);

	mad_stream_init(stream);
	mad_frame_init(frame);
	mad_synth_init(synth);
	
	if (decoder_mp3.input_func == 0)
	{
		return 0;
	}

	if (decoder_mp3.error_func)
	{
		error_func = decoder_mp3.error_func;
		error_data = decoder_mp3.cb_data;
	}
	else
	{
		error_func = error_default;
		error_data = &bad_last_frame;
	}
	mad_stream_options(stream, decoder_mp3.options);
	
	switch (decoder_mp3.input_func(decoder_mp3.cb_data, stream))
	{
		case MAD_FLOW_STOP:
			return 0;
		case MAD_FLOW_BREAK:
			return 0;
		case MAD_FLOW_IGNORE:
		case MAD_FLOW_CONTINUE:
			break;
	}
	// searching for first mp3 frame
	first_frame = (unsigned char *)pAcb->Abv.start;
	while(1)
	{
		while(mad_frame_decode(frame, stream)) 
		{
			if(MAD_RECOVERABLE(stream->error))
			{
				continue;
            }
            else
            {
			    return 0;
			}

		}

		first_frame =  (unsigned char*) stream->this_frame;

		// we have possible first frame, but we need more checking, now we will check more frames
		
		// remember some infos about stream
//		mp3->c_layer = frame.header.layer;
//		mp3->c_mode = frame.header.mode;
//		mp3->c_channel = ( frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
//		mp3->c_emphasis = frame.header.emphasis;
//		mp3->c_mode_extension = frame.header.mode_extension;
//		mp3->c_bitrate = frame.header.bitrate;
//		mp3->c_flags = frame.header.flags;
//		mp3->c_avg_bitrate = 0;
//		mp3->c_duration = frame.header.duration;
//		mp3->c_sample_per_frame = (frame.header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152;

		c_sampling_rate = frame->header.samplerate;
		pAcb->Attr.SampleFreq = frame->header.samplerate;
		pAcb->Attr.NumCh = ( frame->header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
		    		
		// check next frame
		if(mad_frame_decode(frame, stream))
		{
			if(MAD_RECOVERABLE(stream->error))
			{
				continue;
			}
			else
			{
			    return 0;
		    }
		}
		else 
		{
			// more checkinh
			if(c_sampling_rate != frame->header.samplerate)
			{
				continue;
			}
			else
			{						
			    break;
			}	
		}		
	}
	//restore to the begine
	mad_stream_buffer(stream, pAcb->Abv.start, pAcb->Abv.size);

	// now we have valid first frame
	return OKAY;
}

int AUD_MP3_Decode(AUD_tCtrlBlk *pAcb)
{
	struct mad_decoder *decoder = &decoder_mp3;
	
	//1.check if finish
	if(stream->error == MAD_ERROR_BUFLEN)
	{
#ifdef OUTPUT_WAV
            if(outf)
            {
                fwrite(pAcb->Dbv.rdptr,pAcb->Dbv.wrptr - pAcb->Dbv.rdptr, 1, outf);
                UB_reset_pointers(&(pAcb->Dbv));
            }
#endif
	   return 0;
	}
	if (decoder->header_func)
	{
		if(mad_header_decode(&frame->header, stream) == -1)
		{
			if (!MAD_RECOVERABLE(stream->error))
			{
				goto fail;
			}

			switch (error_func(error_data, stream, frame))
			{
				case MAD_FLOW_STOP:
					goto done;
				case MAD_FLOW_BREAK:
					goto fail;
				case MAD_FLOW_IGNORE:
				case MAD_FLOW_CONTINUE:
				    goto done;
				default:
			}
		}

		switch (decoder->header_func(decoder->cb_data, &frame->header))
		{
			case MAD_FLOW_STOP:
				goto done;
			case MAD_FLOW_BREAK:
				goto fail;
			case MAD_FLOW_IGNORE:
			    goto done;
				break;
			case MAD_FLOW_CONTINUE:
				goto fail;;
		}
	}

	if (mad_frame_decode(frame, stream) == -1)
	{
		if (!MAD_RECOVERABLE(stream->error))
		{
			goto fail;
		}

		switch (error_func(error_data, stream, frame))
		{
			case MAD_FLOW_STOP:
				goto done;
			case MAD_FLOW_BREAK:
				goto fail;
			case MAD_FLOW_IGNORE:
			    goto done;
				break;
			case MAD_FLOW_CONTINUE:
			    goto done;
			default:
			    goto done;
		}
	}
	else
	{
		bad_last_frame = 0;
	}

	if (decoder->filter_func)
	{
		switch (decoder->filter_func(decoder->cb_data, stream, frame))
		{
			case MAD_FLOW_STOP:
				goto done;
			case MAD_FLOW_BREAK:
				goto fail;
			case MAD_FLOW_IGNORE:
			case MAD_FLOW_CONTINUE:
				goto fail;
		}
	}

	mad_synth_frame(synth, frame);

	if (decoder->output_func)
	{
		switch (decoder->output_func(decoder->cb_data, &frame->header, &synth->pcm))
		{
			case MAD_FLOW_STOP:
				goto done;
			case MAD_FLOW_BREAK:
				goto fail;
			case MAD_FLOW_IGNORE:
			    goto done;
			    break;
			case MAD_FLOW_CONTINUE:
				goto done;
		    default:
		        goto fail;
		}
	}
fail:
    EPRINTF(("Decode fail\n"));
    return -1;

done:
	EPRINTF(("Decode done\n"));
    return 1;
} 

int AUD_MP3_Cleanup(AUD_tCtrlBlk *pAcb)
{
	//free infomation
	AUD_tAttr *pAttr = &(pAcb->Attr);
	
#ifdef OUTPUT_WAV
	if(outf)
	{
	    //flush out
        fwrite(pAcb->Dbv.rdptr, (pAcb->Dbv.wrptr - pAcb->Dbv.rdptr), 1, outf);
        fflush(outf);
        printf("flush to wav file!!\n");
	    wavesize *= 2;//for 16bit
  	    rewind(outf);//back to the begining
	    if (!fseek(outf, 0, SEEK_SET))
	    {
		    WriteWaveHeader(outf, wavesize, pAttr->SampleFreq, pAttr->NumCh, 16);
	    }
        fclose(outf);
        printf("close wav file\n");
     }
#endif //OUTPUT_WAV
#ifdef OUTPUT_I2S
    //flush out to iis
    cyg_thread_delay(200);
    iis_send_data(pAcb->Dbv.rdptr, (pAcb->Dbv.wrptr - pAcb->Dbv.rdptr));
    printf("flush to iis!!\n");
	I2S_free();
#endif //OUTPUT_I2S
	
	DBV_exit();//free Dbv
	ABV_exit();//free Abv
	
//	if(pAttr->Album)
//	{
//	    free(pAttr->Album);
//	}
//
//    if(pAttr->Artist)
//    {
//    	free(pAttr->Artist);
//    }
//    if(pAttr->Comment)
//    {
//    	free(pAttr->Comment);
//    }
//    if(pAttr->TrackTitle)
//    {
//    	free(pAttr->TrackTitle);
//    }
//    if(pAttr->Year)
//    {
//    	free(pAttr->Year);
//    }
//    if(pAttr->TrackNum)
//    {
//    	free(pAttr->TrackNum);
//    }
//	if(pAttr->Genre)
//	{
//    	free(pAttr->Genre);
//    }

    mad_synth_finish(synth);
	mad_frame_finish(frame);
	mad_stream_finish(stream);
		
	mad_decoder_finish(&decoder_mp3);
	return 0;
}

int AUD_MP3_Get_Config(AUD_tCtrlBlk *pAcb)
{
	AUD_tAttr *pAud = &(pAcb->Attr);
	pAud->NumCh = 2;
	pAud->BitsPerSamp = 16;
//	pAud->SampleFreq = 44100;
	pAud->ChOrder[0] = 0;
	pAud->ChOrder[1] = 1;
	pAud->ChOrder[2] = 2;
	pAud->ChOrder[3] = 3;
	pAud->ChOrder[4] = 4;
	pAud->ChOrder[5] = -1;
	pAud->ChOrder[6] = -1;
	pAud->ChOrder[7] = -1;
//	DisplayID3(pAcb, ID3_VERSION1);
	return OKAY;
}

// Device driver routine hooker
AUD_tModule MAD_ModuleMP3 =
{
	MADTYPE_MP3,
	AUD_MP3_Init,
	AUD_MP3_Decode,
	AUD_MP3_Cleanup,
	AUD_MP3_Get_Config,
};