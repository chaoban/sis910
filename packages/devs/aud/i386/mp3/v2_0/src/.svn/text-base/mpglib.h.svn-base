#ifndef CYGONCE_MPGLIB_H
#define CYGONCE_MPGLIB_H

#include <stdio.h>
#include <cyg/io/aud/adecctrl.h>

#include "mp3_def.h"

#define MP3_ERR -1
#define MP3_OK  0
#define MP3_NEED_MORE 1

struct buf{
    unsigned char *pnt;
	long size;
	long pos;
    struct buf *next;
    struct buf *prev;
};

struct framebuf {
	struct buf *buf;
	long pos;
	struct frame *next;
	struct frame *prev;
};

typedef struct mpstr_tag {
	struct buf *head,*tail;
	int mpstr_allocated;		 /* 1 if mem for mpstr allocated */
     int vbr_header;               /* 1 if valid Xing vbr header detected */
     int num_frames;               /* set if vbr header present */    
     int header_parsed;
     int side_parsed;  
     int data_parsed;  
     int free_format;             /* 1 = free format frame */
     int old_free_format;        /* 1 = last frame was free format */
	int bsize;
	int framesize;
	int ssize;									/* side info size */
	int dsize;									/* main data size */
    int fsizeold;								/* pre-frame size */
    int fsizeold_nopadding;				/* pre-frame size without padding bits */
	struct frame fr;
    unsigned char bsspace[2][MAXFRAMESIZE+512]; /* MAXFRAMESIZE */
	real hybrid_block[2][2][SBLIMIT*SSLIMIT];
	int hybrid_blc[2];
	unsigned long header;				/* content of every frame's header */
	int bsnum;
	real synth_buffs[2][2][0x110];
     int  synth_bo;
     int  sync_bitstream;

    int bitindex;
    unsigned char* wordpointer;	
} MPSTR, *PMPSTR;

/*********************************************************************
 *
 * decoding 
 *
 * a simple interface to mpglib, part of mpg123, is also included if
 * libmp3lame is compiled with HAVE_MPGLIB
 *
 *********************************************************************/
typedef struct {
  int header_parsed;   /* 1 if header was parsed and following data was
                          computed  */
  int stereo;          /* number of channels */
  int samplerate;      /* sample rate */
  int bitrate;         /* bitrate */
  int mode;            /* mp3 frame type  */
  int mode_ext;        /* mp3 frame type */
  int framesize;       /* number of samples per mp3 frame  */

  /* this data is only computed if mpglib detects a Xing VBR header */
  unsigned long nsamp; /* number of samples in mp3 file.  */
  int totalframes;     /* total number of frames in mp3 file   */

  /* this data is not currently computed by the mpglib routines */
  int framenum;        /* frames decoded counter  */
} mp3data_struct;

struct buf *addbuf( PMPSTR mp, unsigned char *buf,int size);
MPSTR *mp_init(void);
void show_buf_info(PMPSTR mp);
int read_buf_byte(PMPSTR mp);
int sync_buffer(PMPSTR mp,int free_match);

int is_syncword_mp123(const void *const headerptr);
int	 decodeMP3(PMPSTR mp,unsigned char *inmemory,int inmemsize,char *outmemory,int outmemsize,int *done);

int lame_decode_header(unsigned char *, int len,
                     short pcm_l[], short pcm_r[], mp3data_struct * mp3data);

int get_audio16(AUD_tCtrlBlk *pAcb, short buffer[2][1152]);

int
lame_decode_header_clipchoice(unsigned char *buffer, int len,
                     char pcm_l_raw[], char pcm_r_raw[], mp3data_struct * mp3data,
                     char *p, int psize, int decoded_sample_size,
                     int (*decodeMP3_ptr)(PMPSTR,unsigned char *,int,char *,int,int*) );

#endif
