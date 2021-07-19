#include <stdio.h>
#include <string.h>		// contain memcpy
#include <pkgconf/devs_aud_mp3.h>

#include "mpglib.h"
#include "common.h"

#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif

extern const int tabsel_123 [2] [3] [16];
extern const long freqs[9];

int head_check(unsigned long head,int check_layer)
{
  /*
    look for a valid header.  
    if check_layer > 0, then require that
    nLayer = check_layer.  
   */

  /* bits 13-14 = layer 3 */
  int nLayer=4-((head>>17)&3);

  if( (head & 0xffe00000) != 0xffe00000) {		// here happened an error once!
    /* syncword */
	return FALSE;
  }

  if (nLayer == 4 || nLayer == 2 || nLayer == 1)
      return FALSE;

  if (check_layer > 0 && nLayer != check_layer)
      return FALSE;

  if( ((head>>12)&0xf) == 0xf) {
    /* bits 16,17,18,19 = 1111  invalid bitrate */
    return FALSE;
  }
  if( ((head>>10)&0x3) == 0x3 ) {
    /* bits 20,21 = 11  invalid sampling freq */
    return FALSE;
  }
  if ((head&0x3) == 0x2 )
      /* invalid emphasis */
      return FALSE;
  return TRUE;
}

/*
 * the code a header and write the information
 * into the frame structure
 */
int decode_header(struct frame *fr,unsigned long newhead)
{
    if( newhead & (1<<20) ) {
      fr->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
      fr->mpeg25 = 0;
    }
    else {
      fr->lsf = 1;
      fr->mpeg25 = 1;
    }
    
    fr->lay = 4-((newhead>>17)&3);
    if( ((newhead>>10)&0x3) == 0x3) {
      EPRINTF(("Stream error\n"));
      return -1;
    }
    if(fr->mpeg25) {
      fr->sampling_frequency = 6 + ((newhead>>10)&0x3);
    }
    else
      fr->sampling_frequency = ((newhead>>10)&0x3) + (fr->lsf*3);

    fr->error_protection = ((newhead>>16)&0x1)^0x1;

    if(fr->mpeg25) /* allow Bitrate change for 2.5 ... */
      fr->bitrate_index = ((newhead>>12)&0xf);

    fr->bitrate_index = ((newhead>>12)&0xf);
    fr->padding   = ((newhead>>9)&0x1);
    fr->extension = ((newhead>>8)&0x1);
    fr->mode      = ((newhead>>6)&0x3);
    fr->mode_ext  = ((newhead>>4)&0x3);
    fr->copyright = ((newhead>>3)&0x1);
    fr->original  = ((newhead>>2)&0x1);
    fr->emphasis  = newhead & 0x3;

    fr->stereo    = (fr->mode == MPG_MD_MONO) ? 1 : 2;

    switch(fr->lay)
    {
      case 3:
		if (fr->bitrate_index==0)
		  fr->framesize=0;
		else{
	          fr->framesize  = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
	          fr->framesize /= freqs[fr->sampling_frequency]<<(fr->lsf);
	          fr->framesize = fr->framesize + fr->padding - 4;
		}
	        break; 
      default:
        EPRINTF(("Sorry, layer %d not supported\n",fr->lay)); 
        return (0);
    }

    return 1;
}

unsigned int getbits(PMPSTR mp, int number_of_bits)
{
  unsigned int rval;

  if (number_of_bits <= 0 || !mp->wordpointer)
    return 0;

  {
    rval = mp->wordpointer[0];
    rval <<= 8;
    rval |= mp->wordpointer[1];
    rval <<= 8;
    rval |= mp->wordpointer[2];
    rval <<= mp->bitindex;
    rval &= 0xffffff;

    mp->bitindex += number_of_bits;

    rval >>= (24-number_of_bits);

    mp->wordpointer += (mp->bitindex>>3);
    mp->bitindex &= 7;
  }
  return rval;
}

#if 0
unsigned int getbits_fast(PMPSTR mp, int number_of_bits)
{
  unsigned int rval;

  {
    rval = mp->wordpointer[0];
    rval <<= 8;	
    rval |= mp->wordpointer[1];
    rval <<= mp->bitindex;
    rval &= 0xffff;
    mp->bitindex += number_of_bits;

    rval >>= (16-number_of_bits);

    mp->wordpointer += (mp->bitindex>>3);
    mp->bitindex &= 7;
  }
  return rval;
}
#else		// modified by zhangsen(sis5236)

#endif

int set_pointer( PMPSTR mp, long backstep)
{
  unsigned char *bsbufold;

  if(mp->fsizeold < 0 && backstep > 0) {
    EPRINTF(("Can't step back %ld!\n",backstep));
    return MP3_ERR; 
  }
  bsbufold = mp->bsspace[1-mp->bsnum] + 512;
  mp->wordpointer -= backstep;
  if (backstep)
    memcpy(mp->wordpointer,bsbufold+mp->fsizeold-backstep, backstep);
  mp->bitindex = 0;
  return MP3_OK;
}


