/* OggDec
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <string.h>
#include <cyg/io/aud/adecctrl.h>
#include <cyg/io/aud/UB.h>
#include <cyg/io/aud/Adec.h>
#include <cyg/io/aud/Adeccmd.h>

#include "vorbisfile.h"




#define VERSIONSTRING "OggDec 1.0.1\n"
static char buf[8192];
static int quiet = 0;
static int bits = 16;
static int endian = 0;
static int raw = 0;
static int sign = 1;
unsigned char headbuf[44]; /* The whole buffer */
char *outfilename = NULL;
char *outfile;



/* 
  * extern
  */
extern uint file_size;
extern uint abv_have_read;
extern uint abv_have_write;
extern UBuff *pABV;


/********wave**********/
#define WRITE_U32(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);\
                          *((buf)+2) = (unsigned char)(((x)>>16)&0xff);\
                          *((buf)+3) = (unsigned char)(((x)>>24)&0xff);

#define WRITE_U16(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);

/* Some of this based on ao/src/ao_wav.c */
int write_prelim_header(OggVorbis_File *vf, FILE *out, ogg_int64_t knownlength) {
    unsigned int size = 0x7fffffff;
    int channels = ov_info(vf,0)->channels;
    int samplerate = ov_info(vf,0)->rate;
    int bytespersec = channels*samplerate*bits/8;
    int align = channels*bits/8;
    int samplesize = bits;

    if(knownlength && knownlength*bits/8*channels < size)
        size = (unsigned int)(knownlength*bits/8*channels+44) ;

    memcpy(headbuf, "RIFF", 4);
    WRITE_U32(headbuf+4, size-8);
    memcpy(headbuf+8, "WAVE", 4);
    memcpy(headbuf+12, "fmt ", 4);
    WRITE_U32(headbuf+16, 16);
    WRITE_U16(headbuf+20, 1); /* format */
    WRITE_U16(headbuf+22, channels);
    WRITE_U32(headbuf+24, samplerate);
    WRITE_U32(headbuf+28, bytespersec);
    WRITE_U16(headbuf+32, align);
    WRITE_U16(headbuf+34, samplesize);
    memcpy(headbuf+36, "data", 4);
    WRITE_U32(headbuf+40, size - 44);

    if(fwrite(headbuf, 1, 44, out) != 44) {
        fprintf(stderr, "ERROR: Failed to write wav header: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}

int rewrite_header(FILE *out, unsigned int written) 
{
    unsigned int length = written;

    length += 44;

    WRITE_U32(headbuf+4, length-8);
    WRITE_U32(headbuf+40, length-44);
    if(fseek(out, 0, SEEK_SET) != 0)
        return 1;

    if(fwrite(headbuf, 1, 44, out) != 44) {
        fprintf(stderr, "ERROR: Failed to write wav header: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}


/********* interface ********************/
int AUD_OGG_Init(AUD_tCtrlBlk *pAcb)
{  
    OggVorbis_File *vf;
    int bs = 0;
    ogg_buf inbuf;
    int buflen = 8192;
    unsigned int written = 0;
    int ret;
    ogg_int64_t length = 0;
    ogg_int64_t done = 0;
    int size;
    int seekable = 0;
    int percent = 0;
   
    pABV=&(pAcb->Abv);
    printf("wrptr: %s\n",(uchar *)pABV->wrptr);
   
    vf=(OggVorbis_File *)malloc(sizeof(OggVorbis_File));
    
    if(ov_open(&inbuf, vf, NULL, 0) < 0) {
        fprintf(stderr, "ERROR: Failed to open input as vorbis\n");
       
        return 1;
    }
    printf("version:%d \n",vf->vi->version);
    printf("channel:%d\n",vf->vi->channels);
    printf("rate: %ld \n",vf->vi->rate);
    pAcb->pPrivAttr=vf;
    return 0;
   }

int AUD_OGG_Decode(AUD_tCtrlBlk *pAcb)
{  
    
    
   
    FILE *out;
    out=fopen(outfile,"wb");
    
    int bs = 0;
    ogg_buf inbuf;
    int buflen = 8192;
    unsigned int written = 0;
    int ret;
    ogg_int64_t length = 0;
    ogg_int64_t done = 0;
    int size;
    int seekable = 0;
    int percent = 0;

    if(!raw) {
        if(write_prelim_header((OggVorbis_File *)pAcb->pPrivAttr, out, length)) {
            ov_clear((OggVorbis_File *)pAcb->pPrivAttr);
            fclose(out);
            return 1;
        }
    }
	
    while((ret = ov_read((OggVorbis_File *)pAcb->pPrivAttr, buf, buflen, endian, bits/8, sign, &bs)) != 0) {
        if(bs != 0) {
            fprintf(stderr, "Only one logical bitstream currently supported\n");
            break;
        }
   
        if(ret < 0 ) {
           if( !quiet ) {
               fprintf(stderr, "Warning: hole in data\n");
           }
            continue;
        }
        if(fwrite(buf, 1, ret,out) != ret) {
            fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
            
            fclose(out);
            return 1;
        }
       written += ret;
    }
	if(!raw)
       rewrite_header(out, written); /* We don't care if it fails, too late */
	fclose(out);
	return 0;

}

int AUD_OGG_Cleanup(AUD_tCtrlBlk *pAcb)
{
free(pAcb->pPrivAttr);
return 0;}

int AUD_OGG_GetAttr(AUD_tCtrlBlk *pAcb)
{}


/* Device driver routine hooker */
struct AUD_tModule AUD_ModuleOGG =
{
	.Type = AUDTYPE_OGG,
	.Init = AUD_OGG_Init,
	.Decode = AUD_OGG_Decode,
	.Cleanup = AUD_OGG_Cleanup,
	.GetAttr = AUD_OGG_GetAttr,
};





