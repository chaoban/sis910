#ifndef UB_H_INCLUDED
#define UB_H_INCLUDED

#include <stdio.h>
#include <cyg/kernel/kapi.h>
#ifdef OUTPUT_I2S
#include <cyg/io/iis/io_iis.h>
#endif


#define SEEK_SET 	0
#define SEEK_CUR	1


#ifndef	uchar_type
typedef unsigned char uchar;
#define uchar_type
#endif

typedef enum{
	ABV	= 0,
	DBV,
	UB_UNKNOWN,
}UB_TYPE;

typedef struct {
    UB_TYPE	type;
    unsigned int        start;      /* Starting address pointer */
    unsigned int        end;        /* End address pointer */
    int                 size;       /* Size */
    unsigned int        rdptr;      /* Read pointer */
    unsigned int        wrptr;      /* Write pointer */    
	unsigned int 		wr_ok;		
    int                 fullness;   /* Fullness */    
} UBuff;

/******************************************************************************
  Decoded sample buffer (DBV) related
 ******************************************************************************/

//#define DBV_PER_CH_SIZE     (1024*1024) //1M for per channel, 11sec
#define DBV_PER_CH_SIZE       (32*FIFO_SIZE) //gateway 1 unit = 32 byte 

/* 
  * param
  *		UBuff *p		pointer to structure of UB buffer.
  *		int ubsize		size of UB buffer.
  *		int fsize			size of input file.
  */
#define ABV_init(p, ubsize, fsize) XBV_init(p, 0, ubsize, fsize)
#define DBV_init(p, ubsize, fsize) XBV_init(p, DBV, ubsize, fsize)

/*
  *
  */
#define ABV_exit()	XBV_exit(0)
#define DBV_exit()	XBV_exit(1)

void I2S_alloc(int size);
void I2S_free();
/* 
  * param
  *		int size		the size to be read from ABV buffer.
  *		uchar *dst		the destination address where stores the ABV buffer.
  */
int ABV_read(int size, uchar *dst);

/*
  * param
  *		int dis		the distance between where you go and wherence
  *		int wherence   same as fseek, SEEK_SET and SEEK_CUR, but no
  *							  SEEK_END.
  */  
int ABV_skip(int dis, int wherence);

/*
  * param: 
  *		FILE *src  File handler whose content will be writen into ABV buffer.
  *
  * description:
  *		This function will write "size" bytes into ABV buffer which is specified
  * by UBuff's member "size". System should create a thread for this function,
  * because the function will not return untill "file_size" bytes content have been
  * writen into ABV buffer.
  */
int ABV_write(FILE *src);


int DBV_write(void)	;


#endif		// #ifndef UB_H_INCLUDED
