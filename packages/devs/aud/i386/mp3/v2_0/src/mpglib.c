#include "common.h"
#include "mpglib.h"

#include <cyg/io/aud/adec.h>
#include <pkgconf/devs_aud_mp3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif

extern int input_format;
extern MPSTR *mp;

extern const long freqs[9];
extern const int tabsel_123 [2] [3] [16];
extern const int  bitrate_table[3][16];
extern const int  samplerate_table[3][4] ;
extern real decwin[512+32];
extern real *pnts[];

//-----------------------------------------------------
int mp3_stop = 0;

extern int do_layer3_sideinfo(PMPSTR mp);
extern int  do_layer3( PMPSTR mp,unsigned char *pcm_sample,int *pcm_point,
                int (*synth_1to1_mono_ptr)(PMPSTR,real *,unsigned char *,int *),
                int (*synth_1to1_ptr)(PMPSTR,real *,int,unsigned char *, int *) );

/* copy mono samples */
#define COPY_MONO(DST_TYPE, SRC_TYPE)                                                           \
    DST_TYPE *pcm_l = (DST_TYPE *)pcm_l_raw;                                                    \
    SRC_TYPE *p_samples = (SRC_TYPE *)p;                                                        \
    for (i = 0; i < processed_samples; i++)                                                     \
      *pcm_l++ = (DST_TYPE)*p_samples++; 

/* copy stereo samples */
#define COPY_STEREO(DST_TYPE, SRC_TYPE)                                                         \
    DST_TYPE *pcm_l = (DST_TYPE *)pcm_l_raw, *pcm_r = (DST_TYPE *)pcm_r_raw;                    \
    SRC_TYPE *p_samples = (SRC_TYPE *)p;                                                        \
    for (i = 0; i < processed_samples; i++) {                                                   \
      *pcm_l++ = (DST_TYPE)*p_samples++;                                                        \
      *pcm_r++ = (DST_TYPE)*p_samples++;                                                        \
    }   


MPSTR *mp_init(void)
{
	MPSTR *tmp_mp;

	tmp_mp = (MPSTR *)malloc(sizeof(MPSTR));
	if(tmp_mp == NULL)
		return NULL;

	memset(tmp_mp, 0, sizeof(MPSTR));
	tmp_mp->mpstr_allocated = 1;
	tmp_mp->framesize = 0;
    tmp_mp->num_frames = 0;
    tmp_mp->vbr_header=0;
	tmp_mp->header_parsed=0;
	tmp_mp->side_parsed=0;
	tmp_mp->data_parsed=0;
	tmp_mp->free_format=0;
	tmp_mp->old_free_format=0;
	tmp_mp->ssize = 0;
	tmp_mp->dsize=0;
	tmp_mp->fsizeold = -1;
	tmp_mp->bsize = 0;
	tmp_mp->head = tmp_mp->tail = NULL;
	tmp_mp->fr.single = -1;
	tmp_mp->bsnum = 0;
	tmp_mp->wordpointer = tmp_mp->bsspace[tmp_mp->bsnum] + 512;
    tmp_mp->bitindex = 0;
	tmp_mp->synth_bo = 1;
	tmp_mp->sync_bitstream = 1;
	
	return tmp_mp;
}

/*
 * Please check this and don't kill me if there's a bug
 * This is a (nearly?) complete header analysis for a MPEG-1/2/2.5 Layer I, II or III
 * data stream
 */
int
is_syncword_mp123(const void *const headerptr)
{
    const unsigned char *const p = headerptr;
    static const char abl2[16] =
        { 0, 7, 7, 7, 0, 7, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8 };

    if ((p[0] & 0xFF) != 0xFF){			
        return 0;       /* first 8 bits must be '1' */
	}
    if ((p[1] & 0xE0) != 0xE0){
        return 0;       /* next 3 bits are also */
	}
    if ((p[1] & 0x18) == 0x08){
        return 0;       /* no MPEG-1, -2 or -2.5 */
	}
    if ((p[1] & 0x06) == 0x00){
        return 0;       /* no Layer I, II and III */
	}

    if (!((p[1] & 0x06) == 0x01*2 && input_format == AUDTYPE_MP3)){		
		return 0; 
	}
    if ((p[2] & 0xF0) == 0xF0){
        return 0;       /* bad bitrate */
	}
    if ((p[2] & 0x0C) == 0x0C){
        return 0;       /* no sample frequency with (32,44.1,48)/(1,2,4)     */
	}
    if ((p[1] & 0x18) == 0x18 && (p[1] & 0x06) == 0x04
	&& abl2[p[2] >> 4] & (1 << (p[3] >> 6))){
		return 0;
	}
    if ((p[3] & 3) == 2){
		return 0;       /* reserved enphasis mode */
	}
	
    return 1;
}


#define OUTSIZE_CLIPPED   4800*sizeof(short)	//4096*sizeof(short)

/* Originally, the "out" is a static local which is working in PC i386 platform.
  * But now, we are porting decoder into eCos OS which is an embeded system,
  * whose stack is limited, so I make this array be global array avoiding overflow.
  * By zhang sen(SiS5236).
  * email: john_zhang@sis.com
  */
unsigned char out[OUTSIZE_CLIPPED];

int 
lame_decode_header(unsigned char *buffer, int len,
                     short pcm_l[], short pcm_r[], mp3data_struct * mp3data)
{
  	return lame_decode_header_clipchoice(buffer, len, (char *)pcm_l, (char *)pcm_r, mp3data, out, OUTSIZE_CLIPPED, sizeof(short), decodeMP3 );
}

/*
 * For lame_decode:  return code
 * -1     error
 *  0     ok, but need more data before outputing any samples
 *  n     number of samples output.  either 576 or 1152 depending on MP3 file.
 */

int
lame_decode_header_clipchoice(unsigned char *buffer, int len,
                     char pcm_l_raw[], char pcm_r_raw[], mp3data_struct * mp3data,
                     char *p, int psize, int decoded_sample_size,
                     int (*decodeMP3_ptr)(PMPSTR,unsigned char *,int,char *,int,int*) )
{
    static const int smpls[2][4] = {
/*Layer   I    II     III */
        {0, 384, 1152, 1152}, /* MPEG-1     */ 
        {0, 384, 1152, 576} /* MPEG-2(.5) */
    };

    int     processed_bytes;
    int     processed_samples; /* processed samples per channel */
    int     ret;
    int     i;

    mp3data->header_parsed = 0;

    ret = (*decodeMP3_ptr)(mp, buffer, len, p, psize, &processed_bytes);
		
    /* three cases:  
     * 1. headers parsed, but data not complete
     *       mp.header_parsed==1 
     *       mp.framesize=0           
     *       mp.fsizeold=size of last frame, or 0 if this is first frame
     *
     * 2. headers, data parsed, but ancillary data not complete
     *       mp.header_parsed==1 
     *       mp.framesize=size of frame           
     *       mp.fsizeold=size of last frame, or 0 if this is first frame
     *
     * 3. frame fully decoded:  
     *       mp.header_parsed==0 
     *       mp.framesize=0           
     *       mp.fsizeold=size of frame (which is now the last frame)
     *
     */
    if (mp->header_parsed || mp->fsizeold > 0 || mp->framesize > 0) {		
    	   mp3data->header_parsed = 1;
        mp3data->stereo = mp->fr.stereo;
        mp3data->samplerate = freqs[mp->fr.sampling_frequency];
	   mp3data->bitrate = tabsel_123[mp->fr.lsf][mp->fr.lay - 1][mp->fr.bitrate_index];
        mp3data->mode = mp->fr.mode;
        mp3data->mode_ext = mp->fr.mode_ext;
        mp3data->framesize = smpls[mp->fr.lsf][mp->fr.lay];
#if 0	
        if (mp->fsizeold > 0) 
            mp3data->bitrate = 8 * (4 + mp->fsizeold) * mp3data->samplerate /
                (1.e3 * mp3data->framesize) + 0.5;
        else if (mp->framesize > 0)
            mp3data->bitrate = 8 * (4 + mp->framesize) * mp3data->samplerate /
                (1.e3 * mp3data->framesize) + 0.5;
        else
            mp3data->bitrate =
                tabsel_123[mp->fr.lsf][mp->fr.lay - 1][mp->fr.bitrate_index];
#endif
        if (mp->num_frames > 0) {
            /* Xing VBR header found and num_frames was set */
            mp3data->totalframes = mp->num_frames;
            mp3data->nsamp = mp3data->framesize * mp->num_frames;
        }
    }

    switch (ret) {
    case MP3_OK:
        switch (mp->fr.stereo) {
        case 1: 
            processed_samples = processed_bytes / decoded_sample_size;
            if (decoded_sample_size == sizeof(short)) {
              COPY_MONO(short,short)
            }
            else {
              COPY_MONO(sample_t,real)                
            }
            break;
        case 2: 
            processed_samples = (processed_bytes / decoded_sample_size) >> 1; 
            if (decoded_sample_size == sizeof(short)) {
              COPY_STEREO(short,short)
            }
            else {
              COPY_STEREO(sample_t,real)
            }
            break;
        default:
            processed_samples = -1;
            break;
        }
        break;

    case MP3_NEED_MORE:
		//printf("IN %s, LINE %d: MP3 NEED MORE\n", __FILE__, __LINE__);
        processed_samples = 0;
        break;

    default:
    case MP3_ERR:
		//printf("IN %s, LINE %d: MP3 ERROR\n", __FILE__, __LINE__);
        processed_samples = -1;
        break;

    }

    return processed_samples;
}



struct buf *addbuf( PMPSTR mp, unsigned char *buf,int size)
{
	struct buf *nbuf;

	nbuf = (struct buf*) malloc( sizeof(struct buf) );
	if(!nbuf) {
		EPRINTF(("Out of memory!\n"));
		return NULL;
	}
	EPRINTF(("Size will be added into mp buf = %d\n", size));
	nbuf->pnt = (unsigned char*) malloc(size);
	if(!nbuf->pnt) {
		assert(0);
		free(nbuf);
		return NULL;
	}
	nbuf->size = size;
	memcpy(nbuf->pnt, buf, size);
	nbuf->next = NULL;
	nbuf->prev = mp->head;
	nbuf->pos = 0;

	if(!mp->tail) {
		mp->tail = nbuf;
	}
	else {
	  mp->head->next = nbuf;
	}

	mp->head = nbuf;
	mp->bsize += size;

	/*EPRINTF(("nbuf adr:0x%x  head adr:0x%x  tail adr:0x%x\nhead->pre:0x%x  head->nxt:0x%x  tail->pre:0x%x  tail->nxt:0x%x\n",
		nbuf, mp->head, mp->tail, mp->head->prev, mp->head->next,
		mp->tail->prev, mp->tail->next));*/

	return nbuf;
}



int sync_buffer(PMPSTR mp,int free_match) 
{
  /* traverse mp structure without modifing pointers, looking
   * for a frame valid header.
   * if free_format, valid header must also have the same
   * samplerate.   
   * return number of bytes in mp, before the header
   * return -1 if header is not found
   */
  unsigned int b[4]={0,0,0,0};
  int i,h,pos;
  struct buf *buf=mp->tail;
  if(!buf){
	return -1;
  }

  pos = buf->pos;
  for (i=0; i<mp->bsize; i++) {
    /* get 4 bytes */
    
    b[0]=b[1]; b[1]=b[2]; b[2]=b[3];
    while(pos >= buf->size) {
      buf  = buf->next;
      pos = buf->pos;
      if(!buf) {
		return -1;
		/* not enough data to read 4 bytes */
      }
    }
    b[3] = buf->pnt[pos];
    ++pos;

    if (i>=3) {
        struct frame *fr = &mp->fr;
		unsigned long head;

		head = b[0];
		head <<= 8;
		head |= b[1];
		head <<= 8;
		head |= b[2];
		head <<= 8;
		head |= b[3];		
		h = head_check(head,fr->lay);
		
		if (h && free_match) {
		  /* just to be even more thorough, match the sample rate */
		  int mode,stereo,sampling_frequency,mpeg25,lsf;

		  if( head & (1<<20) ) {
		    lsf = (head & (1<<19)) ? 0x0 : 0x1;
		    mpeg25 = 0;
		  }
		  else {
		    lsf = 1;
		    mpeg25 = 1;
		  }

		  mode      = ((head>>6)&0x3);
		  stereo    = (mode == MPG_MD_MONO) ? 1 : 2;

		  if(mpeg25) 
		    sampling_frequency = 6 + ((head>>10)&0x3);
		  else
		    sampling_frequency = ((head>>10)&0x3) + (lsf*3);
		  h = ((stereo==fr->stereo) && (lsf==fr->lsf) && (mpeg25==fr->mpeg25) && 
	                 (sampling_frequency == fr->sampling_frequency));
		}

		if (h) {
		  return i-3;
		}
    }
  }
  return -1;
}

const static char	VBRTag0[]={"Xing"};
const static char	VBRTag1[]={"Info"};

int IsVbrTag(const unsigned char* buf)
{
    int isTag0, isTag1;
    
    isTag0 = ((buf[0]==VBRTag0[0]) && (buf[1]==VBRTag0[1]) && (buf[2]==VBRTag0[2]) && (buf[3]==VBRTag0[3])); 
    isTag1 = ((buf[0]==VBRTag1[0]) && (buf[1]==VBRTag1[1]) && (buf[2]==VBRTag1[2]) && (buf[3]==VBRTag1[3])); 

    return (isTag0 || isTag1);
}

/*-------------------------------------------------------------*/
static int ExtractI4(unsigned char *buf)
{
	int x;
	/* big endian extract */
	x = buf[0];
	x <<= 8;
	x |= buf[1];
	x <<= 8;
	x |= buf[2];
	x <<= 8;
	x |= buf[3];
	return x;
}

int GetVbrTag(VBRTAGDATA *pTagData,  unsigned char *buf)
{
	int			i, head_flags;
	int			h_bitrate,h_id, h_mode, h_sr_index;
        int enc_delay,enc_padding; 

	/* get Vbr header data */
	pTagData->flags = 0;

	/* get selected MPEG header data */
	h_id       = (buf[1] >> 3) & 1;
	h_sr_index = (buf[2] >> 2) & 3;
	h_mode     = (buf[3] >> 6) & 3;
        h_bitrate  = ((buf[2]>>4)&0xf);
	h_bitrate = bitrate_table[h_id][h_bitrate];

        /* check for FFE syncword */
        if ((buf[1]>>4)==0xE) 
            pTagData->samprate = samplerate_table[2][h_sr_index];
        else
            pTagData->samprate = samplerate_table[h_id][h_sr_index];
        /*	if( h_id == 0 ) */
        /*		pTagData->samprate >>= 1; */



	/*  determine offset of header */
	if( h_id )
	{
                /* mpeg1 */
		if( h_mode != 3 )	buf+=(32+4);
		else				buf+=(17+4);
	}
	else
	{
                /* mpeg2 */
		if( h_mode != 3 ) buf+=(17+4);
		else              buf+=(9+4);
	}

    if (!IsVbrTag(buf))
        return 0;

	buf+=4;

	pTagData->h_id = h_id;

	head_flags = pTagData->flags = ExtractI4(buf); buf+=4;      /* get flags */

	if( head_flags & FRAMES_FLAG )
	{
		pTagData->frames   = ExtractI4(buf); buf+=4;
	}

	if( head_flags & BYTES_FLAG )
	{
		pTagData->bytes = ExtractI4(buf); buf+=4;
	}

	if( head_flags & TOC_FLAG )
	{
		if( pTagData->toc != NULL )
		{
			for(i=0;i<NUMTOCENTRIES;i++)
				pTagData->toc[i] = buf[i];
		}
		buf+=NUMTOCENTRIES;
	}

	pTagData->vbr_scale = -1;

	if( head_flags & VBR_SCALE_FLAG )
	{
		pTagData->vbr_scale = ExtractI4(buf); buf+=4;
	}

	pTagData->headersize = 
	  ((h_id+1)*72000*h_bitrate) / pTagData->samprate;

        buf+=21;
        enc_delay = buf[0] << 4;
        enc_delay += buf[1] >> 4;
        enc_padding= (buf[1] & 0x0F)<<8;
        enc_padding += buf[2];
        /* check for reasonable values (this may be an old Xing header, */
        /* not a INFO tag) */
        if (enc_delay<0 || enc_delay > 3000) enc_delay=-1;
        if (enc_padding<0 || enc_padding > 3000) enc_padding=-1;

        pTagData->enc_delay=enc_delay;
        pTagData->enc_padding=enc_padding;

	return 1;       /* success */
}


/* number of bytes needed by GetVbrTag to parse header */
#define XING_HEADER_SIZE 194

/* traverse mp data structure without changing it */
/* (just like sync_buffer) */
/* pull out Xing bytes */
/* call vbr header check code from LAME */
/* if we find a header, parse it and also compute the VBR header size */
/* if no header, do nothing. */
/* */
/* bytes = number of bytes before MPEG header.  skip this many bytes */
/* before starting to read */
/* return value: number of bytes in VBR header, including syncword */
static int
check_vbr_header(PMPSTR mp,int bytes)
{
  int i,pos;
  struct buf *buf=mp->tail;
  unsigned char xing[XING_HEADER_SIZE];
  VBRTAGDATA pTagData;

  pos = buf->pos;
  /* skip to valid header */
  for (i=0; i<bytes; ++i) {
    while(pos >= buf->size) {
      buf  = buf->next;
      pos = buf->pos;
      if(!buf) 	return -1; /* fatal error */
    }
    ++pos;
  }
  /* now read header */
  for (i=0; i<XING_HEADER_SIZE; ++i) {
    while(pos >= buf->size) {
      buf  = buf->next;
      if(!buf) 	return -1; /* fatal error */
      pos = buf->pos;
    }
    xing[i] = buf->pnt[pos];
    ++pos;
  }

  /* check first bytes for Xing header */
  mp->vbr_header = GetVbrTag(&pTagData,xing);
  if (mp->vbr_header) {
    mp->num_frames=pTagData.frames; 
    
    if ( pTagData.headersize < 1 ) return 1;
    return pTagData.headersize;
  }
  return 0;
}


void remove_buf(PMPSTR mp)
{
  struct buf *buf = mp->tail;
  
  mp->tail = buf->next;
  if(mp->tail)
    mp->tail->prev = NULL;
  else {
    mp->tail = mp->head = NULL;
  }
  
  free(buf->pnt);
  free(buf);
}

void show_buf_info(PMPSTR mp)
{
	struct buf *buf = mp->tail;
	struct buf *nbuf = mp->tail->next;
	int pos = mp->tail->pos;

	while(pos >= buf->size){
		MEM_DUMP(buf->pnt, buf->size);
		buf = nbuf;
		pos = nbuf->pos;
		nbuf = nbuf->next;
	}
	MEM_DUMP(buf->pnt, buf->size);
	return;
}

int read_buf_byte(PMPSTR mp)
{
	unsigned int b;
	int pos;
	
	pos = mp->tail->pos;
	while(pos >= mp->tail->size) {
		 EPRINTF(("Remove Buffer obsolute\n"));
	     remove_buf(mp);
		if(!mp->tail) {
			EPRINTF(("Fatal error! tried to read past mp buffer\n"));
			return 0xffff;
		}
		pos = mp->tail->pos;
	}

	b = mp->tail->pnt[pos];
	mp->bsize--;
	mp->tail->pos++;

	//EPRINTF(("Read Byte 0x%04x\n", b));
	return b;
}


static void read_head(PMPSTR mp)
{
	unsigned long head;

	head = read_buf_byte(mp);
	head <<= 8;
	head |= read_buf_byte(mp);
	head <<= 8;
	head |= read_buf_byte(mp);
	head <<= 8;
	head |= read_buf_byte(mp);

	mp->header = head;
}

static void
copy_mp(PMPSTR mp,int size,unsigned char *ptr) 
{
  int len = 0;

  while(len < size && mp->tail) {
    int nlen;
    int blen = mp->tail->size - mp->tail->pos;
    if( (size - len) <= blen) {
      nlen = size-len;
    }
    else {
      nlen = blen;
    }
    memcpy(ptr+len,mp->tail->pnt+mp->tail->pos, nlen);
    len += nlen;
    mp->tail->pos += nlen;
    mp->bsize -= nlen;
    if(mp->tail->pos == mp->tail->size) {
	  EPRINTF(("Removing Buffer Obsolute\n"));
      remove_buf(mp);
    }
  }
}




static int
decodeMP3_clipchoice( PMPSTR mp,unsigned char *in,int isize,char *out,int *done,      
                           int (*synth_1to1_mono_ptr)(PMPSTR,real *,unsigned char *,int *),
                           int (*synth_1to1_ptr)(PMPSTR,real *,int,unsigned char *, int *) )
{
	int i,iret,bits,bytes;

	if (in && isize && addbuf(mp,in,isize) == NULL){
		EPRINTF(("MP3_ERR\n"));
	    return MP3_ERR;
	}	

	/* First decode header */
	if(!mp->header_parsed) {
		EPRINTF(("***** Head Parsing *****\n"));
	    if (mp->fsizeold==-1 || mp->sync_bitstream) {		   
	        int vbrbytes;
			mp->sync_bitstream=0;

	        /* This is the very first call.   sync with anything */
			/* bytes= number of bytes before header */
	        bytes=sync_buffer(mp,0); 
					
	        /* now look for Xing VBR header */
			if (mp->bsize >= bytes+XING_HEADER_SIZE ) { 
			    vbrbytes=check_vbr_header(mp,bytes);
			} else {
			    /* not enough data to look for Xing header */
			    return MP3_NEED_MORE;
			}

			if (mp->vbr_header) {
				EPRINTF(("MP3 has VBR header\n"));
			    /* do we have enough data to parse entire Xing header? */
			    if (bytes+vbrbytes > mp->bsize) return MP3_NEED_MORE;
			    
			    /* read in Xing header.  Buffer data in case it
			     * is used by a non zero main_data_begin for the next
			     * frame, but otherwise dont decode Xing header */
				    for (i=0; i<vbrbytes+bytes; ++i) 
					read_buf_byte(mp);
			    /* now we need to find another syncword */
			    /* just return and make user send in more data */
			    return MP3_NEED_MORE;
			}
            }else{
	        /* match channels, samplerate, etc, when syncing */
                bytes=sync_buffer(mp,1);
	    }

	    if (bytes<0){
				EPRINTF(("IN %s, LINE %d: MP3 NEED MORE\n", __FILE__, __LINE__));
				return MP3_NEED_MORE;
	    	}
	    if (bytes>0) {
		/* there were some extra bytes in front of header.
		 * bitstream problem, but we are now resynced 
		 * should try to buffer previous data in case new
		 * frame has nonzero main_data_begin, but we need
		 * to make sure we do not overflow buffer
		 */
		int size;
		mp->old_free_format=0;
         mp->sync_bitstream=1;
		/* skip some bytes, buffer the rest */
		size = (int) (mp->wordpointer - (mp->bsspace[mp->bsnum]+512));
		
		if (size > MAXFRAMESIZE) {
		    /* wordpointer buffer is trashed.  probably cant recover, but try anyway */
		    EPRINTF(("mpglib: wordpointer trashed.  size=%i (%i)  bytes=%i \n",
			    size,MAXFRAMESIZE,bytes));		  
		    size=0;
		    mp->wordpointer = mp->bsspace[mp->bsnum]+512;
		}
		
		/* buffer contains 'size' data right now 
		   we want to add 'bytes' worth of data, but do not 
		   exceed MAXFRAMESIZE, so we through away 'i' bytes */
		i = (size+bytes)-MAXFRAMESIZE;
		for (; i>0; --i) {
		    --bytes;
		    read_buf_byte(mp);
		}
		
		copy_mp(mp,bytes,mp->wordpointer);
		mp->fsizeold += bytes;
	    }
	    
	    read_head(mp);
		EPRINTF(("HEAD: 0x%04X\n", mp->header));		
	    decode_header(&mp->fr,mp->header);
		
	    mp->header_parsed=1;
	    mp->framesize = mp->fr.framesize;		
	    mp->free_format = (mp->framesize==0);
	    
	    if(mp->fr.lsf)
		mp->ssize = (mp->fr.stereo == 1) ? 9 : 17;
	    else
		mp->ssize = (mp->fr.stereo == 1) ? 17 : 32;
	    if (mp->fr.error_protection){
			EPRINTF(("This File Has CRC Check!\n"));
			mp->ssize += 2;
	    	}
	    
	    mp->bsnum = 1-mp->bsnum; /* toggle buffer */
	    mp->wordpointer = mp->bsspace[mp->bsnum] + 512;
	    mp->bitindex = 0;
	    
	    /* for very first header, never parse rest of data */
	    if (mp->fsizeold==-1){
				EPRINTF(("for very first header, never parse rest of data!\n"));
				return MP3_NEED_MORE;
	    	}
	}
	
	/* now decode side information */
	if (!mp->side_parsed) {
		EPRINTF(("***** Side Information Parsing *****\n"));		
		/* Layer 3 only */
		if (mp->fr.lay==3)
		{
              if (mp->bsize < mp->ssize){
				EPRINTF(("IN %s, LINE %d: MP3 NEED MORE\n", __FILE__, __LINE__));
				return MP3_NEED_MORE;
              }
			
			copy_mp(mp,mp->ssize,mp->wordpointer);
			
			if(mp->fr.error_protection)
			  getbits(mp,16);		
			
			bits = do_layer3_sideinfo(mp);
  
			/* bits = actual number of bits needed to parse this frame */
			/* can be negative, if all bits needed are in the reservoir */
			if (bits<0){
				EPRINTF(("ERROR: %d Bits to be parsed\n", bits));
				bits=0;
			}
			
			/* read just as many bytes as necessary before decoding */
			mp->dsize = (bits+7)/8;

			/* this will force mpglib to read entire frame before decoding */
			/* mp->dsize= mp->framesize - mp->ssize;*/
		}else{
			/* Layers 1 and 2 */
			EPRINTF(("ERROR LAYER!\n"));
			/* check if there is enough input data */
			if(mp->fr.framesize > mp->bsize)
				return MP3_NEED_MORE;

			/* takes care that the right amount of data is copied into wordpointer */
			mp->dsize=mp->fr.framesize;
			mp->ssize=0;
		}

		mp->side_parsed=1;
	}

	/* now decode main data */
	iret=MP3_NEED_MORE;
	if (!mp->data_parsed ) {
		EPRINTF(("***** Data Parsing *****\n")); 
	    if(mp->dsize > mp->bsize) {
			//EPRINTF(("IN %s, LINE %d: MP3 NEED MORE\n", __FILE__, __LINE__));
			return MP3_NEED_MORE;
		}

		copy_mp(mp,mp->dsize,mp->wordpointer); 

		*done = 0;

		switch (mp->fr.lay)
		{
			case 3:
				do_layer3(mp,(unsigned char *) out,done, synth_1to1_mono_ptr, synth_1to1_ptr);
			break;
			default:
				EPRINTF(("ERROR LAYER!\n"));
		}

		mp->wordpointer = mp->bsspace[mp->bsnum] + 512 + mp->ssize + mp->dsize;

		mp->data_parsed=1;
		iret=MP3_OK;
	}


	/* remaining bits are ancillary data, or reservoir for next frame 
	 * If free format, scan stream looking for next frame to determine
	 * mp->framesize */
	if (mp->free_format) {
	  if (mp->old_free_format) {
	    /* free format.  bitrate must not vary */
	    mp->framesize=mp->fsizeold_nopadding + (mp->fr.padding);
	  }else{
	    bytes=sync_buffer(mp,1);
	    if (bytes<0) 
			return iret;
	    mp->framesize = bytes + mp->ssize+mp->dsize;
	    mp->fsizeold_nopadding= mp->framesize - mp->fr.padding;	    
	  }
	}

	/* buffer the ancillary data and reservoir for next frame */
	bytes = mp->framesize-(mp->ssize+mp->dsize);
	if (bytes > mp->bsize) {
	  return iret;
	}

	if (bytes>0) {
	  int size;
	  copy_mp(mp,bytes,mp->wordpointer);
	  mp->wordpointer += bytes;

	  size = (int) (mp->wordpointer - (mp->bsspace[mp->bsnum]+512));
	  if (size > MAXFRAMESIZE) {
	    EPRINTF(("fatal error.  MAXFRAMESIZE not large enough.\n"));
	  }
	}

	/* the above frame is completey parsed.  start looking for next frame */
	mp->fsizeold = mp->framesize;
	mp->old_free_format = mp->free_format;
	mp->framesize =0;
	mp->header_parsed=0;
	mp->side_parsed=0;
	mp->data_parsed=0;

	return iret;
}

#if 1
static void dct64(real *out0,real *out1,real *samples)
{
 real b1[32], b2[32];
 {
  real *costab = pnts[0];
  real costab1, costab2, costab3, costab4, costab5, costab6, costab7, costab8, 
		costab9, costab10, costab11, costab12, costab13, costab14, costab15, costab16;

  costab1 = costab[0];costab2 = costab[1];
  costab3 = costab[2];costab4 = costab[3];
  costab5 = costab[4];costab6 = costab[5];
  costab7 = costab[6];costab8 = costab[7];
  costab9 = costab[8];costab10 = costab[9];
  costab11 = costab[10];costab12 = costab[11];
  costab13 = costab[12];costab14 = costab[13];
  costab15 = costab[14];costab16 = costab[15];

  b1[0x00] = samples[0x00] + samples[0x1F];
  b1[0x1F] = (samples[0x00] - samples[0x1F]) * costab1;

  b1[0x01] = samples[0x01] + samples[0x1E];
  b1[0x1E] = (samples[0x01] - samples[0x1E]) * costab2;

  b1[0x02] = samples[0x02] + samples[0x1D];
  b1[0x1D] = (samples[0x02] - samples[0x1D]) * costab3;

  b1[0x03] = samples[0x03] + samples[0x1C];
  b1[0x1C] = (samples[0x03] - samples[0x1C]) * costab4;

  b1[0x04] = samples[0x04] + samples[0x1B];
  b1[0x1B] = (samples[0x04] - samples[0x1B]) * costab5;

  b1[0x05] = samples[0x05] + samples[0x1A];
  b1[0x1A] = (samples[0x05] - samples[0x1A]) * costab6;

  b1[0x06] = samples[0x06] + samples[0x19];
  b1[0x19] = (samples[0x06] - samples[0x19]) * costab7;

  b1[0x07] = samples[0x07] + samples[0x18];
  b1[0x18] = (samples[0x07] - samples[0x18]) * costab8;

  b1[0x08] = samples[0x08] + samples[0x17];
  b1[0x17] = (samples[0x08] - samples[0x17]) * costab9;

  b1[0x09] = samples[0x09] + samples[0x16];
  b1[0x16] = (samples[0x09] - samples[0x16]) * costab10;

  b1[0x0A] = samples[0x0A] + samples[0x15];
  b1[0x15] = (samples[0x0A] - samples[0x15]) * costab11;

  b1[0x0B] = samples[0x0B] + samples[0x14];
  b1[0x14] = (samples[0x0B] - samples[0x14]) * costab12;

  b1[0x0C] = samples[0x0C] + samples[0x13];
  b1[0x13] = (samples[0x0C] - samples[0x13]) * costab13;

  b1[0x0D] = samples[0x0D] + samples[0x12];
  b1[0x12] = (samples[0x0D] - samples[0x12]) * costab14;

  b1[0x0E] = samples[0x0E] + samples[0x11];
  b1[0x11] = (samples[0x0E] - samples[0x11]) * costab15;

  b1[0x0F] = samples[0x0F] + samples[0x10];
  b1[0x10] = (samples[0x0F] - samples[0x10]) * costab16;
 }


 {
  real *costab = pnts[1];
  real costab1, costab2, costab3, costab4, costab5, costab6, costab7, costab8;
  costab1 = costab[0];costab2 = costab[1];
  costab3 = costab[2];costab4 = costab[3];
  costab5 = costab[4];costab6 = costab[5];
  costab7 = costab[6];costab8 = costab[7];

  b2[0x00] = b1[0x00] + b1[0x0F]; 
  b2[0x0F] = (b1[0x00] - b1[0x0F]) * costab1;
  b2[0x01] = b1[0x01] + b1[0x0E]; 
  b2[0x0E] = (b1[0x01] - b1[0x0E]) * costab2;
  b2[0x02] = b1[0x02] + b1[0x0D]; 
  b2[0x0D] = (b1[0x02] - b1[0x0D]) * costab3;
  b2[0x03] = b1[0x03] + b1[0x0C]; 
  b2[0x0C] = (b1[0x03] - b1[0x0C]) * costab4;
  b2[0x04] = b1[0x04] + b1[0x0B]; 
  b2[0x0B] = (b1[0x04] - b1[0x0B]) * costab5;
  b2[0x05] = b1[0x05] + b1[0x0A]; 
  b2[0x0A] = (b1[0x05] - b1[0x0A]) * costab6;
  b2[0x06] = b1[0x06] + b1[0x09]; 
  b2[0x09] = (b1[0x06] - b1[0x09]) * costab7;
  b2[0x07] = b1[0x07] + b1[0x08]; 
  b2[0x08] = (b1[0x07] - b1[0x08]) * costab8;

  b2[0x10] = b1[0x10] + b1[0x1F];
  b2[0x1F] = (b1[0x1F] - b1[0x10]) * costab1;
  b2[0x11] = b1[0x11] + b1[0x1E];
  b2[0x1E] = (b1[0x1E] - b1[0x11]) * costab2;
  b2[0x12] = b1[0x12] + b1[0x1D];
  b2[0x1D] = (b1[0x1D] - b1[0x12]) * costab3;
  b2[0x13] = b1[0x13] + b1[0x1C];
  b2[0x1C] = (b1[0x1C] - b1[0x13]) * costab4;
  b2[0x14] = b1[0x14] + b1[0x1B];
  b2[0x1B] = (b1[0x1B] - b1[0x14]) * costab5;
  b2[0x15] = b1[0x15] + b1[0x1A];
  b2[0x1A] = (b1[0x1A] - b1[0x15]) * costab6;
  b2[0x16] = b1[0x16] + b1[0x19];
  b2[0x19] = (b1[0x19] - b1[0x16]) * costab7;
  b2[0x17] = b1[0x17] + b1[0x18];
  b2[0x18] = (b1[0x18] - b1[0x17]) * costab8;
 }

 {
  real *costab = pnts[2];
  real costab1, costab2, costab3, costab4;
  costab1 = costab[0];costab2 = costab[1];
  costab3 = costab[2];costab4 = costab[3];

  b1[0x00] = b2[0x00] + b2[0x07];
  b1[0x07] = (b2[0x00] - b2[0x07]) * costab1;
  b1[0x01] = b2[0x01] + b2[0x06];
  b1[0x06] = (b2[0x01] - b2[0x06]) * costab2;
  b1[0x02] = b2[0x02] + b2[0x05];
  b1[0x05] = (b2[0x02] - b2[0x05]) * costab3;
  b1[0x03] = b2[0x03] + b2[0x04];
  b1[0x04] = (b2[0x03] - b2[0x04]) * costab4;

  b1[0x08] = b2[0x08] + b2[0x0F];
  b1[0x0F] = (b2[0x0F] - b2[0x08]) * costab1;
  b1[0x09] = b2[0x09] + b2[0x0E];
  b1[0x0E] = (b2[0x0E] - b2[0x09]) * costab2;
  b1[0x0A] = b2[0x0A] + b2[0x0D];
  b1[0x0D] = (b2[0x0D] - b2[0x0A]) * costab3;
  b1[0x0B] = b2[0x0B] + b2[0x0C];
  b1[0x0C] = (b2[0x0C] - b2[0x0B]) * costab4;

  b1[0x10] = b2[0x10] + b2[0x17];
  b1[0x17] = (b2[0x10] - b2[0x17]) * costab1;
  b1[0x11] = b2[0x11] + b2[0x16];
  b1[0x16] = (b2[0x11] - b2[0x16]) * costab2;
  b1[0x12] = b2[0x12] + b2[0x15];
  b1[0x15] = (b2[0x12] - b2[0x15]) * costab3;
  b1[0x13] = b2[0x13] + b2[0x14];
  b1[0x14] = (b2[0x13] - b2[0x14]) * costab4;

  b1[0x18] = b2[0x18] + b2[0x1F];
  b1[0x1F] = (b2[0x1F] - b2[0x18]) * costab1;
  b1[0x19] = b2[0x19] + b2[0x1E];
  b1[0x1E] = (b2[0x1E] - b2[0x19]) * costab2;
  b1[0x1A] = b2[0x1A] + b2[0x1D];
  b1[0x1D] = (b2[0x1D] - b2[0x1A]) * costab3;
  b1[0x1B] = b2[0x1B] + b2[0x1C];
  b1[0x1C] = (b2[0x1C] - b2[0x1B]) * costab4;
 }

 {
  real const cos0 = pnts[3][0];
  real const cos1 = pnts[3][1];

  b2[0x00] = b1[0x00] + b1[0x03];
  b2[0x03] = (b1[0x00] - b1[0x03]) * cos0;
  b2[0x01] = b1[0x01] + b1[0x02];
  b2[0x02] = (b1[0x01] - b1[0x02]) * cos1;

  b2[0x04] = b1[0x04] + b1[0x07];
  b2[0x07] = (b1[0x07] - b1[0x04]) * cos0;
  b2[0x05] = b1[0x05] + b1[0x06];
  b2[0x06] = (b1[0x06] - b1[0x05]) * cos1;

  b2[0x08] = b1[0x08] + b1[0x0B];
  b2[0x0B] = (b1[0x08] - b1[0x0B]) * cos0;
  b2[0x09] = b1[0x09] + b1[0x0A];
  b2[0x0A] = (b1[0x09] - b1[0x0A]) * cos1;
  
  b2[0x0C] = b1[0x0C] + b1[0x0F];
  b2[0x0F] = (b1[0x0F] - b1[0x0C]) * cos0;
  b2[0x0D] = b1[0x0D] + b1[0x0E];
  b2[0x0E] = (b1[0x0E] - b1[0x0D]) * cos1;

  b2[0x10] = b1[0x10] + b1[0x13];
  b2[0x13] = (b1[0x10] - b1[0x13]) * cos0;
  b2[0x11] = b1[0x11] + b1[0x12];
  b2[0x12] = (b1[0x11] - b1[0x12]) * cos1;

  b2[0x14] = b1[0x14] + b1[0x17];
  b2[0x17] = (b1[0x17] - b1[0x14]) * cos0;
  b2[0x15] = b1[0x15] + b1[0x16];
  b2[0x16] = (b1[0x16] - b1[0x15]) * cos1;

  b2[0x18] = b1[0x18] + b1[0x1B];
  b2[0x1B] = (b1[0x18] - b1[0x1B]) * cos0;
  b2[0x19] = b1[0x19] + b1[0x1A];
  b2[0x1A] = (b1[0x19] - b1[0x1A]) * cos1;

  b2[0x1C] = b1[0x1C] + b1[0x1F];
  b2[0x1F] = (b1[0x1F] - b1[0x1C]) * cos0;
  b2[0x1D] = b1[0x1D] + b1[0x1E];
  b2[0x1E] = (b1[0x1E] - b1[0x1D]) * cos1;
 }

 {
  real const cos0 = pnts[4][0];

  b1[0x00] = b2[0x00] + b2[0x01];
  b1[0x01] = (b2[0x00] - b2[0x01]) * cos0;
  b1[0x02] = b2[0x02] + b2[0x03];
  b1[0x03] = (b2[0x03] - b2[0x02]) * cos0;
  b1[0x02] += b1[0x03];

  b1[0x04] = b2[0x04] + b2[0x05];
  b1[0x05] = (b2[0x04] - b2[0x05]) * cos0;
  b1[0x06] = b2[0x06] + b2[0x07];
  b1[0x07] = (b2[0x07] - b2[0x06]) * cos0;
  b1[0x06] += b1[0x07];
  b1[0x04] += b1[0x06];
  b1[0x06] += b1[0x05];
  b1[0x05] += b1[0x07];

  b1[0x08] = b2[0x08] + b2[0x09];
  b1[0x09] = (b2[0x08] - b2[0x09]) * cos0;
  b1[0x0A] = b2[0x0A] + b2[0x0B];
  b1[0x0B] = (b2[0x0B] - b2[0x0A]) * cos0;
  b1[0x0A] += b1[0x0B];

  b1[0x0C] = b2[0x0C] + b2[0x0D];
  b1[0x0D] = (b2[0x0C] - b2[0x0D]) * cos0;
  b1[0x0E] = b2[0x0E] + b2[0x0F];
  b1[0x0F] = (b2[0x0F] - b2[0x0E]) * cos0;
  b1[0x0E] += b1[0x0F];
  b1[0x0C] += b1[0x0E];
  b1[0x0E] += b1[0x0D];
  b1[0x0D] += b1[0x0F];

  b1[0x10] = b2[0x10] + b2[0x11];
  b1[0x11] = (b2[0x10] - b2[0x11]) * cos0;
  b1[0x12] = b2[0x12] + b2[0x13];
  b1[0x13] = (b2[0x13] - b2[0x12]) * cos0;
  b1[0x12] += b1[0x13];

  b1[0x14] = b2[0x14] + b2[0x15];
  b1[0x15] = (b2[0x14] - b2[0x15]) * cos0;
  b1[0x16] = b2[0x16] + b2[0x17];
  b1[0x17] = (b2[0x17] - b2[0x16]) * cos0;
  b1[0x16] += b1[0x17];
  b1[0x14] += b1[0x16];
  b1[0x16] += b1[0x15];
  b1[0x15] += b1[0x17];

  b1[0x18] = b2[0x18] + b2[0x19];
  b1[0x19] = (b2[0x18] - b2[0x19]) * cos0;
  b1[0x1A] = b2[0x1A] + b2[0x1B];
  b1[0x1B] = (b2[0x1B] - b2[0x1A]) * cos0;
  b1[0x1A] += b1[0x1B];

  b1[0x1C] = b2[0x1C] + b2[0x1D];
  b1[0x1D] = (b2[0x1C] - b2[0x1D]) * cos0;
  b1[0x1E] = b2[0x1E] + b2[0x1F];
  b1[0x1F] = (b2[0x1F] - b2[0x1E]) * cos0;
  b1[0x1E] += b1[0x1F];
  b1[0x1C] += b1[0x1E];
  b1[0x1E] += b1[0x1D];
  b1[0x1D] += b1[0x1F];
 }

 out0[0x10*16] = b1[0x00];
 out0[0x10*12] = b1[0x04];
 out0[0x10* 8] = b1[0x02];
 out0[0x10* 4] = b1[0x06];
 out0[0x10* 0] = b1[0x01];
 out1[0x10* 0] = b1[0x01];
 out1[0x10* 4] = b1[0x05];
 out1[0x10* 8] = b1[0x03];
 out1[0x10*12] = b1[0x07];

 b1[0x08] += b1[0x0C];
 out0[0x10*14] = b1[0x08];
 b1[0x0C] += b1[0x0a];
 out0[0x10*10] = b1[0x0C];
 b1[0x0A] += b1[0x0E];
 out0[0x10* 6] = b1[0x0A];
 b1[0x0E] += b1[0x09];
 out0[0x10* 2] = b1[0x0E];
 b1[0x09] += b1[0x0D];
 out1[0x10* 2] = b1[0x09];
 b1[0x0D] += b1[0x0B];
 out1[0x10* 6] = b1[0x0D];
 b1[0x0B] += b1[0x0F];
 out1[0x10*10] = b1[0x0B];
 out1[0x10*14] = b1[0x0F];

 b1[0x18] += b1[0x1C];
 out0[0x10*15] = b1[0x10] + b1[0x18];
 out0[0x10*13] = b1[0x18] + b1[0x14];
 b1[0x1C] += b1[0x1a];
 out0[0x10*11] = b1[0x14] + b1[0x1C];
 out0[0x10* 9] = b1[0x1C] + b1[0x12];
 b1[0x1A] += b1[0x1E];
 out0[0x10* 7] = b1[0x12] + b1[0x1A];
 out0[0x10* 5] = b1[0x1A] + b1[0x16];
 b1[0x1E] += b1[0x19];
 out0[0x10* 3] = b1[0x16] + b1[0x1E];
 out0[0x10* 1] = b1[0x1E] + b1[0x11];
 b1[0x19] += b1[0x1D];
 out1[0x10* 1] = b1[0x11] + b1[0x19];
 out1[0x10* 3] = b1[0x19] + b1[0x15];
 b1[0x1D] += b1[0x1B];
 out1[0x10* 5] = b1[0x15] + b1[0x1D];
 out1[0x10* 7] = b1[0x1D] + b1[0x13];
 b1[0x1B] += b1[0x1F];
 out1[0x10* 9] = b1[0x13] + b1[0x1B];
 out1[0x10*11] = b1[0x1B] + b1[0x17];
 out1[0x10*13] = b1[0x17] + b1[0x1F];
 out1[0x10*15] = b1[0x1F];
}
#else
static void dct64(real *out0,real *out1,real *samples)
{
  real b1[32], b2[32];
	
 {
  real *costab = pnts[0];
	
  b1[0x00] = samples[0x00] + samples[0x1F];
  b1[0x1F] = (samples[0x00] - samples[0x1F]) * costab[0x0];

  b1[0x01] = samples[0x01] + samples[0x1E];
  b1[0x1E] = (samples[0x01] - samples[0x1E]) * costab[0x1];

  b1[0x02] = samples[0x02] + samples[0x1D];
  b1[0x1D] = (samples[0x02] - samples[0x1D]) * costab[0x2];

  b1[0x03] = samples[0x03] + samples[0x1C];
  b1[0x1C] = (samples[0x03] - samples[0x1C]) * costab[0x3];

  b1[0x04] = samples[0x04] + samples[0x1B];
  b1[0x1B] = (samples[0x04] - samples[0x1B]) * costab[0x4];

  b1[0x05] = samples[0x05] + samples[0x1A];
  b1[0x1A] = (samples[0x05] - samples[0x1A]) * costab[0x5];

  b1[0x06] = samples[0x06] + samples[0x19];
  b1[0x19] = (samples[0x06] - samples[0x19]) * costab[0x6];

  b1[0x07] = samples[0x07] + samples[0x18];
  b1[0x18] = (samples[0x07] - samples[0x18]) * costab[0x7];

  b1[0x08] = samples[0x08] + samples[0x17];
  b1[0x17] = (samples[0x08] - samples[0x17]) * costab[0x8];

  b1[0x09] = samples[0x09] + samples[0x16];
  b1[0x16] = (samples[0x09] - samples[0x16]) * costab[0x9];

  b1[0x0A] = samples[0x0A] + samples[0x15];
  b1[0x15] = (samples[0x0A] - samples[0x15]) * costab[0xA];

  b1[0x0B] = samples[0x0B] + samples[0x14];
  b1[0x14] = (samples[0x0B] - samples[0x14]) * costab[0xB];

  b1[0x0C] = samples[0x0C] + samples[0x13];
  b1[0x13] = (samples[0x0C] - samples[0x13]) * costab[0xC];

  b1[0x0D] = samples[0x0D] + samples[0x12];
  b1[0x12] = (samples[0x0D] - samples[0x12]) * costab[0xD];

  b1[0x0E] = samples[0x0E] + samples[0x11];
  b1[0x11] = (samples[0x0E] - samples[0x11]) * costab[0xE];

  b1[0x0F] = samples[0x0F] + samples[0x10];
  b1[0x10] = (samples[0x0F] - samples[0x10]) * costab[0xF];
 }


 {
  real *costab = pnts[1];

  b2[0x00] = b1[0x00] + b1[0x0F]; 
  b2[0x0F] = (b1[0x00] - b1[0x0F]) * costab[0];
  b2[0x01] = b1[0x01] + b1[0x0E]; 
  b2[0x0E] = (b1[0x01] - b1[0x0E]) * costab[1];
  b2[0x02] = b1[0x02] + b1[0x0D]; 
  b2[0x0D] = (b1[0x02] - b1[0x0D]) * costab[2];
  b2[0x03] = b1[0x03] + b1[0x0C]; 
  b2[0x0C] = (b1[0x03] - b1[0x0C]) * costab[3];
  b2[0x04] = b1[0x04] + b1[0x0B]; 
  b2[0x0B] = (b1[0x04] - b1[0x0B]) * costab[4];
  b2[0x05] = b1[0x05] + b1[0x0A]; 
  b2[0x0A] = (b1[0x05] - b1[0x0A]) * costab[5];
  b2[0x06] = b1[0x06] + b1[0x09]; 
  b2[0x09] = (b1[0x06] - b1[0x09]) * costab[6];
  b2[0x07] = b1[0x07] + b1[0x08]; 
  b2[0x08] = (b1[0x07] - b1[0x08]) * costab[7];

  b2[0x10] = b1[0x10] + b1[0x1F];
  b2[0x1F] = (b1[0x1F] - b1[0x10]) * costab[0];
  b2[0x11] = b1[0x11] + b1[0x1E];
  b2[0x1E] = (b1[0x1E] - b1[0x11]) * costab[1];
  b2[0x12] = b1[0x12] + b1[0x1D];
  b2[0x1D] = (b1[0x1D] - b1[0x12]) * costab[2];
  b2[0x13] = b1[0x13] + b1[0x1C];
  b2[0x1C] = (b1[0x1C] - b1[0x13]) * costab[3];
  b2[0x14] = b1[0x14] + b1[0x1B];
  b2[0x1B] = (b1[0x1B] - b1[0x14]) * costab[4];
  b2[0x15] = b1[0x15] + b1[0x1A];
  b2[0x1A] = (b1[0x1A] - b1[0x15]) * costab[5];
  b2[0x16] = b1[0x16] + b1[0x19];
  b2[0x19] = (b1[0x19] - b1[0x16]) * costab[6];
  b2[0x17] = b1[0x17] + b1[0x18];
  b2[0x18] = (b1[0x18] - b1[0x17]) * costab[7];
 }

 {
  real *costab = pnts[2];

  b1[0x00] = b2[0x00] + b2[0x07];
  b1[0x07] = (b2[0x00] - b2[0x07]) * costab[0];
  b1[0x01] = b2[0x01] + b2[0x06];
  b1[0x06] = (b2[0x01] - b2[0x06]) * costab[1];
  b1[0x02] = b2[0x02] + b2[0x05];
  b1[0x05] = (b2[0x02] - b2[0x05]) * costab[2];
  b1[0x03] = b2[0x03] + b2[0x04];
  b1[0x04] = (b2[0x03] - b2[0x04]) * costab[3];

  b1[0x08] = b2[0x08] + b2[0x0F];
  b1[0x0F] = (b2[0x0F] - b2[0x08]) * costab[0];
  b1[0x09] = b2[0x09] + b2[0x0E];
  b1[0x0E] = (b2[0x0E] - b2[0x09]) * costab[1];
  b1[0x0A] = b2[0x0A] + b2[0x0D];
  b1[0x0D] = (b2[0x0D] - b2[0x0A]) * costab[2];
  b1[0x0B] = b2[0x0B] + b2[0x0C];
  b1[0x0C] = (b2[0x0C] - b2[0x0B]) * costab[3];

  b1[0x10] = b2[0x10] + b2[0x17];
  b1[0x17] = (b2[0x10] - b2[0x17]) * costab[0];
  b1[0x11] = b2[0x11] + b2[0x16];
  b1[0x16] = (b2[0x11] - b2[0x16]) * costab[1];
  b1[0x12] = b2[0x12] + b2[0x15];
  b1[0x15] = (b2[0x12] - b2[0x15]) * costab[2];
  b1[0x13] = b2[0x13] + b2[0x14];
  b1[0x14] = (b2[0x13] - b2[0x14]) * costab[3];

  b1[0x18] = b2[0x18] + b2[0x1F];
  b1[0x1F] = (b2[0x1F] - b2[0x18]) * costab[0];
  b1[0x19] = b2[0x19] + b2[0x1E];
  b1[0x1E] = (b2[0x1E] - b2[0x19]) * costab[1];
  b1[0x1A] = b2[0x1A] + b2[0x1D];
  b1[0x1D] = (b2[0x1D] - b2[0x1A]) * costab[2];
  b1[0x1B] = b2[0x1B] + b2[0x1C];
  b1[0x1C] = (b2[0x1C] - b2[0x1B]) * costab[3];
 }

 {
  real const cos0 = pnts[3][0];
  real const cos1 = pnts[3][1];

  b2[0x00] = b1[0x00] + b1[0x03];
  b2[0x03] = (b1[0x00] - b1[0x03]) * cos0;
  b2[0x01] = b1[0x01] + b1[0x02];
  b2[0x02] = (b1[0x01] - b1[0x02]) * cos1;

  b2[0x04] = b1[0x04] + b1[0x07];
  b2[0x07] = (b1[0x07] - b1[0x04]) * cos0;
  b2[0x05] = b1[0x05] + b1[0x06];
  b2[0x06] = (b1[0x06] - b1[0x05]) * cos1;

  b2[0x08] = b1[0x08] + b1[0x0B];
  b2[0x0B] = (b1[0x08] - b1[0x0B]) * cos0;
  b2[0x09] = b1[0x09] + b1[0x0A];
  b2[0x0A] = (b1[0x09] - b1[0x0A]) * cos1;
  
  b2[0x0C] = b1[0x0C] + b1[0x0F];
  b2[0x0F] = (b1[0x0F] - b1[0x0C]) * cos0;
  b2[0x0D] = b1[0x0D] + b1[0x0E];
  b2[0x0E] = (b1[0x0E] - b1[0x0D]) * cos1;

  b2[0x10] = b1[0x10] + b1[0x13];
  b2[0x13] = (b1[0x10] - b1[0x13]) * cos0;
  b2[0x11] = b1[0x11] + b1[0x12];
  b2[0x12] = (b1[0x11] - b1[0x12]) * cos1;

  b2[0x14] = b1[0x14] + b1[0x17];
  b2[0x17] = (b1[0x17] - b1[0x14]) * cos0;
  b2[0x15] = b1[0x15] + b1[0x16];
  b2[0x16] = (b1[0x16] - b1[0x15]) * cos1;

  b2[0x18] = b1[0x18] + b1[0x1B];
  b2[0x1B] = (b1[0x18] - b1[0x1B]) * cos0;
  b2[0x19] = b1[0x19] + b1[0x1A];
  b2[0x1A] = (b1[0x19] - b1[0x1A]) * cos1;

  b2[0x1C] = b1[0x1C] + b1[0x1F];
  b2[0x1F] = (b1[0x1F] - b1[0x1C]) * cos0;
  b2[0x1D] = b1[0x1D] + b1[0x1E];
  b2[0x1E] = (b1[0x1E] - b1[0x1D]) * cos1;
 }

 {
  real const cos0 = pnts[4][0];

  b1[0x00] = b2[0x00] + b2[0x01];
  b1[0x01] = (b2[0x00] - b2[0x01]) * cos0;
  b1[0x02] = b2[0x02] + b2[0x03];
  b1[0x03] = (b2[0x03] - b2[0x02]) * cos0;
  b1[0x02] += b1[0x03];

  b1[0x04] = b2[0x04] + b2[0x05];
  b1[0x05] = (b2[0x04] - b2[0x05]) * cos0;
  b1[0x06] = b2[0x06] + b2[0x07];
  b1[0x07] = (b2[0x07] - b2[0x06]) * cos0;
  b1[0x06] += b1[0x07];
  b1[0x04] += b1[0x06];
  b1[0x06] += b1[0x05];
  b1[0x05] += b1[0x07];

  b1[0x08] = b2[0x08] + b2[0x09];
  b1[0x09] = (b2[0x08] - b2[0x09]) * cos0;
  b1[0x0A] = b2[0x0A] + b2[0x0B];
  b1[0x0B] = (b2[0x0B] - b2[0x0A]) * cos0;
  b1[0x0A] += b1[0x0B];

  b1[0x0C] = b2[0x0C] + b2[0x0D];
  b1[0x0D] = (b2[0x0C] - b2[0x0D]) * cos0;
  b1[0x0E] = b2[0x0E] + b2[0x0F];
  b1[0x0F] = (b2[0x0F] - b2[0x0E]) * cos0;
  b1[0x0E] += b1[0x0F];
  b1[0x0C] += b1[0x0E];
  b1[0x0E] += b1[0x0D];
  b1[0x0D] += b1[0x0F];

  b1[0x10] = b2[0x10] + b2[0x11];
  b1[0x11] = (b2[0x10] - b2[0x11]) * cos0;
  b1[0x12] = b2[0x12] + b2[0x13];
  b1[0x13] = (b2[0x13] - b2[0x12]) * cos0;
  b1[0x12] += b1[0x13];

  b1[0x14] = b2[0x14] + b2[0x15];
  b1[0x15] = (b2[0x14] - b2[0x15]) * cos0;
  b1[0x16] = b2[0x16] + b2[0x17];
  b1[0x17] = (b2[0x17] - b2[0x16]) * cos0;
  b1[0x16] += b1[0x17];
  b1[0x14] += b1[0x16];
  b1[0x16] += b1[0x15];
  b1[0x15] += b1[0x17];

  b1[0x18] = b2[0x18] + b2[0x19];
  b1[0x19] = (b2[0x18] - b2[0x19]) * cos0;
  b1[0x1A] = b2[0x1A] + b2[0x1B];
  b1[0x1B] = (b2[0x1B] - b2[0x1A]) * cos0;
  b1[0x1A] += b1[0x1B];

  b1[0x1C] = b2[0x1C] + b2[0x1D];
  b1[0x1D] = (b2[0x1C] - b2[0x1D]) * cos0;
  b1[0x1E] = b2[0x1E] + b2[0x1F];
  b1[0x1F] = (b2[0x1F] - b2[0x1E]) * cos0;
  b1[0x1E] += b1[0x1F];
  b1[0x1C] += b1[0x1E];
  b1[0x1E] += b1[0x1D];
  b1[0x1D] += b1[0x1F];
 }

 out0[0x10*16] = b1[0x00];
 out0[0x10*12] = b1[0x04];
 out0[0x10* 8] = b1[0x02];
 out0[0x10* 4] = b1[0x06];
 out0[0x10* 0] = b1[0x01];
 out1[0x10* 0] = b1[0x01];
 out1[0x10* 4] = b1[0x05];
 out1[0x10* 8] = b1[0x03];
 out1[0x10*12] = b1[0x07];

 b1[0x08] += b1[0x0C];
 out0[0x10*14] = b1[0x08];
 b1[0x0C] += b1[0x0a];
 out0[0x10*10] = b1[0x0C];
 b1[0x0A] += b1[0x0E];
 out0[0x10* 6] = b1[0x0A];
 b1[0x0E] += b1[0x09];
 out0[0x10* 2] = b1[0x0E];
 b1[0x09] += b1[0x0D];
 out1[0x10* 2] = b1[0x09];
 b1[0x0D] += b1[0x0B];
 out1[0x10* 6] = b1[0x0D];
 b1[0x0B] += b1[0x0F];
 out1[0x10*10] = b1[0x0B];
 out1[0x10*14] = b1[0x0F];

 b1[0x18] += b1[0x1C];
 out0[0x10*15] = b1[0x10] + b1[0x18];
 out0[0x10*13] = b1[0x18] + b1[0x14];
 b1[0x1C] += b1[0x1a];
 out0[0x10*11] = b1[0x14] + b1[0x1C];
 out0[0x10* 9] = b1[0x1C] + b1[0x12];
 b1[0x1A] += b1[0x1E];
 out0[0x10* 7] = b1[0x12] + b1[0x1A];
 out0[0x10* 5] = b1[0x1A] + b1[0x16];
 b1[0x1E] += b1[0x19];
 out0[0x10* 3] = b1[0x16] + b1[0x1E];
 out0[0x10* 1] = b1[0x1E] + b1[0x11];
 b1[0x19] += b1[0x1D];
 out1[0x10* 1] = b1[0x11] + b1[0x19];
 out1[0x10* 3] = b1[0x19] + b1[0x15];
 b1[0x1D] += b1[0x1B];
 out1[0x10* 5] = b1[0x15] + b1[0x1D];
 out1[0x10* 7] = b1[0x1D] + b1[0x13];
 b1[0x1B] += b1[0x1F];
 out1[0x10* 9] = b1[0x13] + b1[0x1B];
 out1[0x10*11] = b1[0x1B] + b1[0x17];
 out1[0x10*13] = b1[0x17] + b1[0x1F];
 out1[0x10*15] = b1[0x1F];
}
#endif
/*
 * the call via dct64 is a trick to force GCC to use
 * (new) registers for the b1,b2 pointer to the bufs[xx] field
 */
 
/* versions: clipped (when TYPE == short) and unclipped (when TYPE == real) of synth_1to1_mono* functions */
#define SYNTH_1TO1_MONO_CLIPCHOICE(TYPE,SYNTH_1TO1)                    \
  TYPE samples_tmp[64];                                                \
  TYPE *tmp1 = samples_tmp;                                            \
  int i,ret;                                                           \
  int pnt1 = 0;                                                        \
                                                                       \
  ret = SYNTH_1TO1 (mp,bandPtr,0,(unsigned char *) samples_tmp,&pnt1); \
  out += *pnt;                                                         \
                                                                       \
  for(i=0;i<32;i++) {                                                  \
    *( (TYPE *) out) = *tmp1;                                          \
    out += sizeof(TYPE);                                               \
    tmp1 += 2;                                                         \
  }                                                                    \
  *pnt += 32*sizeof(TYPE);                                             \
                                                                       \
  return ret; 

int synth_1to1(PMPSTR mp, real *bandPtr,int channel,unsigned char *out, int *pnt);

int synth_1to1_mono(PMPSTR mp, real *bandPtr,unsigned char *out,int *pnt)
{
  SYNTH_1TO1_MONO_CLIPCHOICE(short,synth_1to1)
}


#define WRITE_SAMPLE_CLIPPED(samples,sum,clip) \
  if( (sum) > 32767.0) { *(samples) = 0x7fff; (clip)++; } \
  else if( (sum) < -32768.0) { *(samples) = -0x8000; (clip)++; } \
  else { *(samples) = ((sum)>0 ? (sum)+0.5 : (sum)-0.5) ; }

/* versions: clipped (when TYPE == short) and unclipped (when TYPE == real) of synth_1to1* functions */
#define SYNTH_1TO1_CLIPCHOICE(TYPE,WRITE_SAMPLE)         \
  static const int step = 2;                             \
  int bo;                                                \
  TYPE *samples = (TYPE *) (out + *pnt);                 \
                                                         \
  real *b0,(*buf)[0x110];                                \
  int clip = 0;                                          \
  int bo1;                                               \
                                                         \
  bo = mp->synth_bo;                                     \
                                                         \
  if(!channel) {                                         \
    bo--;                                                \
    bo &= 0xf;                                           \
    buf = mp->synth_buffs[0];                            \
  }                                                      \
  else {                                                 \
    samples++;                                           \
    buf = mp->synth_buffs[1];                            \
  }                                                      \
                                                         \
  if(bo & 0x1) {                                         \
    b0 = buf[0];                                         \
    bo1 = bo;                                            \
    dct64(buf[1]+((bo+1)&0xf),buf[0]+bo,bandPtr);        \
  }                                                      \
  else {                                                 \
    b0 = buf[1];                                         \
    bo1 = bo+1;                                          \
    dct64(buf[0]+bo,buf[1]+bo+1,bandPtr);                \
  }                                                      \
                                                         \
  mp->synth_bo = bo;                                     \
                                                         \
  {                                                      \
    int j;                                               \
    real *window = decwin + 16 - bo1;                    \
                                                         \
    for (j=16;j;j--,b0+=0x10,window+=0x20,samples+=step) \
    {                                                    \
      real sum;                                          \
      sum  = window[0x0] * b0[0x0];                      \
      sum -= window[0x1] * b0[0x1];                      \
      sum += window[0x2] * b0[0x2];                      \
      sum -= window[0x3] * b0[0x3];                      \
      sum += window[0x4] * b0[0x4];                      \
      sum -= window[0x5] * b0[0x5];                      \
      sum += window[0x6] * b0[0x6];                      \
      sum -= window[0x7] * b0[0x7];                      \
      sum += window[0x8] * b0[0x8];                      \
      sum -= window[0x9] * b0[0x9];                      \
      sum += window[0xA] * b0[0xA];                      \
      sum -= window[0xB] * b0[0xB];                      \
      sum += window[0xC] * b0[0xC];                      \
      sum -= window[0xD] * b0[0xD];                      \
      sum += window[0xE] * b0[0xE];                      \
      sum -= window[0xF] * b0[0xF];                      \
                                                         \
      WRITE_SAMPLE (samples,sum,clip);                 \
    }                                                    \
                                                         \
    {                                                    \
      real sum;                                          \
      sum  = window[0x0] * b0[0x0];                      \
      sum += window[0x2] * b0[0x2];                      \
      sum += window[0x4] * b0[0x4];                      \
      sum += window[0x6] * b0[0x6];                      \
      sum += window[0x8] * b0[0x8];                      \
      sum += window[0xA] * b0[0xA];                      \
      sum += window[0xC] * b0[0xC];                      \
      sum += window[0xE] * b0[0xE];                      \
      WRITE_SAMPLE (samples,sum,clip);                  \
      b0-=0x10,window-=0x20,samples+=step;               \
    }                                                    \
    window += bo1<<1;                                    \
                                                         \
    for (j=15;j;j--,b0-=0x10,window-=0x20,samples+=step) \
    {                                                    \
      real sum;                                          \
      sum = -window[-0x1] * b0[0x0];                     \
      sum -= window[-0x2] * b0[0x1];                     \
      sum -= window[-0x3] * b0[0x2];                     \
      sum -= window[-0x4] * b0[0x3];                     \
      sum -= window[-0x5] * b0[0x4];                     \
      sum -= window[-0x6] * b0[0x5];                     \
      sum -= window[-0x7] * b0[0x6];                     \
      sum -= window[-0x8] * b0[0x7];                     \
      sum -= window[-0x9] * b0[0x8];                     \
      sum -= window[-0xA] * b0[0x9];                     \
      sum -= window[-0xB] * b0[0xA];                     \
      sum -= window[-0xC] * b0[0xB];                     \
      sum -= window[-0xD] * b0[0xC];                     \
      sum -= window[-0xE] * b0[0xD];                     \
      sum -= window[-0xF] * b0[0xE];                     \
      sum -= window[-0x0] * b0[0xF];                     \
                                                         \
      WRITE_SAMPLE (samples,sum,clip);                   \
    }                                                    \
  }                                                      \
  *pnt += 64*sizeof(TYPE);                               \
                                                         \
  return clip;                                           

int synth_1to1(PMPSTR mp, real *bandPtr,int channel,unsigned char *out, int *pnt)
{
  SYNTH_1TO1_CLIPCHOICE(short,WRITE_SAMPLE_CLIPPED)
}

int decodeMP3( PMPSTR mp,unsigned char *in,int isize,char *out,
		int osize,int *done)
{
	if(osize < 4608) {
		EPRINTF(("To less out space\n"));
		return MP3_ERR;
	}

	/* passing pointers to the functions which clip the samples */
	return decodeMP3_clipchoice(mp, in, isize, out, done, synth_1to1_mono, synth_1to1);
}	

int get_audio16(AUD_tCtrlBlk *pAcb, short buffer[2][1152])
{
	int     samples_read = 0;
   
	/* 
     * NOTE: LAME can now handle arbritray size input data packets,
     * so there is no reason to read the input data in chuncks of
     * size "framesize".  EXCEPT:  the LAME graphical frame analyzer 
     * will get out of sync if we read more than framesize worth of data.
     */

	//if(AUDTYPE_MP3 == pAcb->Attr.Type)
	samples_read = decode_fromfile(pAcb, buffer[0], buffer[1]);
	
	return samples_read;
}


