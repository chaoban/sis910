/* dependent include */
#include <stdlib.h>
#include <stdio.h>
#include <pkgconf/devs_aud_mp3.h>
#include <pkgconf/io_aud.h>
#include <cyg/io/aud/adecctrl.h>
#include <cyg/io/aud/UB.h>
#include <cyg/io/aud/data_transfer.h> 
#include "common.h"
#include "mpglib.h"
#include "tabinit.h"
#include "layer3.h"
#include "mp3_dec.h"

#ifdef OUTPUT_WAV
#include "wav.h"
#endif

#ifdef OUTPUT_DATA_BY_NET
#include <cyg/io/aud/data_transfer.h>
#endif

#ifdef MP3_DEC_COST
#include <time.h>
clock_t mp3_start, mp3_end;
#define TPRINTF(a)	printf a
#endif


#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif


/* Device driver routine hooker */
struct AUD_tModule AUD_ModuleMP3 =
{
	.Type = AUDTYPE_MP3,
	.Init = AUD_MP3_Init,
	.Decode = AUD_MP3_Decode,
	.Cleanup = AUD_MP3_Cleanup,
	.GetAttr = AUD_MP3_Get_Config,
};

/*
  *		extern globals
  */
extern FILE *pwav;
extern int pcmbitwidth;
extern UBuff *pABV;
extern uint abv_have_read;
#ifdef AUD_TIME_CONTROL
extern double aud_cur_time;
extern double aud_total_time;
#endif


/*
  * here are all globals
  */
int 	input_format;
int     count_samples_carefully;
int     pcmbitwidth;
int 	mp3_delay;
unsigned int num_samples_read;
MPSTR *mp = NULL;
mp3data_struct mp3data;
short output_pcm[2][1152];

/**************** Interface *******************/
int AUD_MP3_Init(AUD_tCtrlBlk *pAcb)
{
	unsigned char buf[100] = {0};
	int ret = 0;
	int len = 0;

	/* init global variables */
	input_format = AUDTYPE_MP3;
	count_samples_carefully = 0;
	pcmbitwidth = 16;
	num_samples_read = 0;
	mp3_delay = 0;		
	pAcb->pPrivAttr = (void *)&mp3data;

	if(NULL == (mp = mp_init()))
	{
		EPRINTF(("fatal error during mp initialization.\n"));
		return FALSE;
	}
	memset(&mp3data, 0, sizeof(mp3data_struct));
	make_decode_tables(32767);
	/* find ID3V2 tag */
	len = 4;
	if( ABV_read( len, buf ) != len ){
		return FALSE;
	}
	if (buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3') 
	{	
		
		len = 6;
		if (ABV_read(len, buf) != len){
			EPRINTF(("ABV read error2\n"));
		    return FALSE;      
		}
		buf[2] &= 127; buf[3] &= 127; buf[4] &= 127; buf[5] &= 127;
		len = (((((buf[2] << 7) + buf[3]) << 7) + buf[4]) << 7) + buf[5];		
		ABV_skip(len, SEEK_CUR);
		
		len = 4;
		if (ABV_read(len, buf) != len){
			EPRINTF(("ABV read error3\n"));
		    return FALSE;      
		}
    }
	/* find the syncword of 1st frame */
	len = 4;
    while (!is_syncword_mp123(buf))
	{
        int i;
        for (i = 0; i < len - 1; i++)
            buf[i] = buf[i + 1];
        if (ABV_read(1, buf + len - 1) != 1)
        {		   
            return FALSE;  
        }
    }
	/* now parse the current buffer looking for MP3 headers.    */
    /* headers are parsed, no data will be decoded.   */	
	ret = lame_decode_header(buf, len, NULL, NULL, &mp3data);
	if(MP3_ERR == ret){
		return FALSE;
	}
	
	/* repeat until we decode a valid mp3 header.  */
    while (!mp3data.header_parsed) 
	{
        len = ABV_read(sizeof(buf), buf);
        if (len != sizeof(buf))
            return FALSE;
        ret = lame_decode_header(buf, len, NULL, NULL, &mp3data);
        if (-1 == ret)
            return FALSE;
    }
	pAcb->InitDone = 1;
	pAcb->Attr.NumCh = mp3data.stereo;
	pAcb->Attr.Type = AUDTYPE_MP3;
	pAcb->Attr.SampleFreq = mp3data.samplerate;
	pAcb->Attr.BitsPerSamp = 16;
	if (mp3data.totalframes > 0) 
	{  // XING header
		pAcb->Attr.NumFrames = mp3data.totalframes;
		pAcb->Attr.ByteRate = 0;
		pAcb->Attr.FrameSize = mp3data.framesize;
    }
    else 
	{
	 	pAcb->Attr.NumFrames = 0;
		pAcb->Attr.ByteRate = mp3data.bitrate;
		pAcb->Attr.FrameSize = mp3data.framesize;
        mp3data.nsamp = MAX_U_32_NUM;
    }
    return TRUE;	
}

#ifdef OUTPUT_WAV
FILE *outf = NULL;
char outpath[50];
#endif

int AUD_MP3_Decode(AUD_tCtrlBlk *pAcb)
{
	uchar buf[1024];
	uchar sync[4];
	int wavesize = 0;
	int i;
    volatile int     iread, len;	    

	if(input_format == AUDTYPE_MP3){
		if(mp3_delay == 0)
			mp3_delay = 576 + 528 + 1;
		else
			mp3_delay += 528 + 1;
	}	
	wavesize = -mp3_delay;
	/* 
	  * Now, we begin to decode the mp3 from the 2nd frame, however
	  * the mp3 is CBR or VBR. 
	  */	
	ABV_read(1024, buf);
	addbuf( mp, buf, 1024 );
	len = 4;
	sync[0] = read_buf_byte( mp );
	sync[1] = read_buf_byte( mp );
	sync[2] = read_buf_byte( mp );
	sync[3] = read_buf_byte( mp );
	while (!is_syncword_mp123(sync))
	{
		for (i = 0; i < len - 1; i++)
			sync[i] = sync[i + 1];
		sync[len-1] = read_buf_byte( mp);		     
	}			
#ifdef OUTPUT_WAV
	if(NULL == (outf = fopen(outpath, "w"))){
		EPRINTF(("Failed to create wav file\n"));
		return FALSE;
	}
	/*  write wav header */
	WriteWaveHeader(outf, 0x7FFFFFFF, mp3data.samplerate, 2, 16);
#endif

#ifdef MP3_DEC_COST
	mp3_start = clock();
#endif

#ifdef AUD_TIME_CONTROL
	aud_cur_time = 0.0;		// init current time
#endif
	/* decode loop */
    do{
        iread = get_audio16(pAcb, output_pcm); 
		if(iread <= 0)
			break;
		mp3data.framenum += iread / mp3data.framesize;
		wavesize += iread;
#ifdef OUTPUT_WAV
		/* 'i' samples are to skip in this frame */
		mp3_delay -= (i = mp3_delay < iread ? mp3_delay : iread); 
		for( ; i < iread; i++){
			Write16BitsLowHigh(outf, output_pcm[0][i]);
              Write16BitsLowHigh(outf, output_pcm[1][i]);
		}
#endif

#ifdef AUD_TIME_CONTROL
		Get_Current_Play_time(iread, mp3data.samplerate);
#endif

#if 1
     EPRINTF(("%dth Frame: %d samples\n", 
     mp3data.framenum, iread));
#else
	printf("%dth Frame: %d samples\n", 
     mp3data.framenum, iread);
#endif
    } while (iread);

#ifdef AUD_TIME_CONTROL
	printf("The Play Time %f\n", aud_cur_time);
#endif

#ifdef MP3_DEC_COST
	mp3_end = clock();
	TPRINTF(("Total time for decoding %d frames: %f s\n", 
		mp3data.framenum, 
		(double)(mp3_end - mp3_start) / (double)CLOCKS_PER_SEC));
	TPRINTF(("Bitrate = %d  Samplerate = %d\n",
		mp3data.bitrate, mp3data.samplerate));
#endif

#ifdef OUTPUT_WAV
	wavesize *= (2 * mp3data.stereo);

	rewind(outf);
	if (!fseek(outf, 0, SEEK_SET)){
		WriteWaveHeader(outf, wavesize, mp3data.samplerate, 2, 16);	
	}

	fclose(outf);
#endif

	return TRUE;		
}

int AUD_MP3_Cleanup(AUD_tCtrlBlk *pAcb)
{
	struct buf *b,*bn;
	
	b = mp->tail;
	while(b) {
		free(b->pnt);
		bn = b->next;
		free(b);
		b = bn;
	}

	if(NULL != mp) free(mp);
	return 0;
}

int AUD_MP3_Get_Config(AUD_tCtrlBlk *pAcb)
{
	return 0;
}

