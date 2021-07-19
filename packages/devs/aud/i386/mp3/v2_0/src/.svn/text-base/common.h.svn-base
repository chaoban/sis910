#ifndef CYGONCE_COMMON_H
#define CYGONCE_COMMON_H

#include <pkgconf/devs_aud_mp3.h>
#ifndef MP3_DEF_H
#include "mp3_def.h"
#endif
#include "mpglib.h"

#ifdef AUD_DEBUG 
#include <stdio.h>
#define assert(a)	do{	\
	printf("assertion failed in %s at line %d\n", __FILE__, __LINE__);	\
}while(a)

#define DELAY(a)	{	\
	int i = a;	\
	while(i){	\
	printf(".");	\
	i--;	}\
	printf("\n");	\
}

#define STOP()		{	\
	mp3_stop = 1;	\
	while(mp3_stop){};	\
}
#else
#define assert(a)

#define DELAY(a)
#define STOP()
#endif

#ifdef MEM_DEBUG
#include <stdio.h>
#define MEM_DUMP(ptr, num)	do{	\
	int i = 0;			\
	int j;					\
	int k = 0;			\
	printf("MEM AT 0x%x, DUMP %d BYTES:\n", (unsigned int)ptr, num);		\
	for(i = 0; i < num / 8; i++){	\
		for(j = 0; j < 8; j++)		\
			printf("0x%x ", ptr[i*8 + j]);		\
		printf("\n");					\
	}						\
	for(k = 0; k < num - i * 8; k++)printf("0x%x ", ptr[i*8 +k]);	\
	printf("\n");		\
}while(0)
#else
#define MEM_DUMP(ptr, num)
#endif

#define getbits_fast(mp, number_of_bits)		({	\
  unsigned int r;		\
  r = mp->wordpointer[0];	\
  r <<= 8;		\
  r |= mp->wordpointer[1];		\
  r <<= mp->bitindex;		\
  r &= 0xffff;		\
  mp->bitindex += number_of_bits;	\
  r >>= (16-number_of_bits);		\
  mp->wordpointer += (mp->bitindex>>3);	\
  mp->bitindex &= 7;		\
  r;		\
})

int head_check(unsigned long head,int check_layer);
int decode_header(struct frame *fr,unsigned long newhead);
unsigned int getbits(PMPSTR , int number_of_bits);
//unsigned int getbits_fast(PMPSTR mp, int number_of_bits);


int set_pointer( PMPSTR mp, long l);

#endif
