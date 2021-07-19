#include "../include/UB.h"

#include <cyg/io/iis/io_iis.h>

#include <stdio.h>
#include <stdlib.h>
#include <pkgconf/devs_mad_mp3.h>
#ifdef OUTPUT_I2S
#include <cyg/io/iis/io_iis.h>
#endif

#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif

  // 
  // globals
  //
static unsigned int file_size = 0;
static unsigned int abv_have_read = 0, abv_have_write = 0;
static unsigned int dbv_have_write = 0;
cyg_handle_t  ABV_thread, decoder_thread;
cyg_thread ABV_handle, decoder_handle;
static UBuff *pABV = NULL, *pDBV = NULL;
static unsigned int i2s_buf0 = 0, i2s_buf1 = 0;
int i2s_flag = 0;

void I2S_alloc(int size)
{
    do{
        i2s_buf0 = (unsigned int)malloc_align(size, 32);//32 byte align
    }while(i2s_buf0 == 0);

    do{
        i2s_buf1 = (unsigned int)malloc_align(size, 32);//32 byte align
    }while(i2s_buf1 == 0);

}
void I2S_free()
{
    if(i2s_buf0)
        free(i2s_buf0);
    if(i2s_buf1)
        free(i2s_buf1);
}
    //
    //  XBV Buffer operation
    //   	XBV is a circle buffer which is used to store input audio data. 
    //   	'X' presents ABV or DBV
    //  
unsigned int XBV_init(UBuff *p, int ub_t, int ubsize, int fsize)
{
	unsigned int begin = 0;
	if(ub_t == ABV){
		begin = (unsigned int)malloc(ubsize);
		
		pABV = p;
		pABV->type = ub_t;
		pABV->start = begin;
		pABV->end = begin + ubsize;
		pABV->size = ubsize;
		pABV->rdptr = pABV->wrptr = pABV->start; 
		pABV->wr_ok	= 0;
		pABV->fullness = -1;	 
		file_size = fsize;
		abv_have_read = 0;
		abv_have_write = 0;
	}
	else if( ub_t == DBV )
	{
//		begin = (unsigned int)malloc(ubsize);
		begin = (unsigned int)malloc_align(ubsize, 32);//32 byte align

		pDBV = p;
		pDBV->type = ub_t;
		pDBV->start = begin;
		pDBV->end = begin + ubsize;
		pDBV->size = ubsize;
		pDBV->rdptr = pDBV->wrptr = pDBV->start;
		pDBV->fullness = 0;	 
		pDBV->wr_ok = 1;	// not OK

		dbv_have_write = 0;
	}

	return begin;	
}


void XBV_exit(int ub_t)
{
	// ABV
	if( ub_t == ABV )
	{
		if(NULL != (void *)pABV->start)
		{
			free((void *)pABV->start);
		}
		pABV = NULL;
	}
	// DBV
	else if( ub_t == DBV )
	{
		if(NULL != (void *)pDBV->start)
		{
			free((void *)pDBV->start);
		}
		pDBV = NULL;
	}
}


int ABV_read(int size, uchar *dst)
{
	uchar *src = (uchar *)pABV->rdptr;
	int i, j = 0;

	if(file_size <= 0 || size <= 0)
	{
		printf("Error Occur in ABV_read\n");
		return 0;
	}

#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE	
	while((abv_have_read < file_size) && size){
		i = 0;
		src = (uchar *)pABV->rdptr;
		while( size && pABV->wr_ok )
		{						
			dst[i] = src[i];
			pABV->rdptr++;
			abv_have_read++;
			i++;
			j++;
			size--;
			if(pABV->rdptr >= pABV->end)
			{
				EPRINTF(("READ TO THE TOP\n"));
				pABV->rdptr = pABV->start;
				pABV->wr_ok = 0;				
				#if 0 
				cyg_thread_resume(ABV_thread);				
				cyg_thread_suspend(decoder_thread);
				cyg_thread_yield();
				#endif
			}
		}		
	}
#else
	EPRINTF(("Not Support \"CYGSEM_KERNEL_SCHED_TIMESLICE\"\n"));
#endif

	return j;
}
    //
    //Add by Jiao Rongmei,for the decoding of MPEG-4 files
    //read ABV data from the very beginning once angain  
    //
void ABV_reset(void)
{
#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE
    pABV->rdptr = pABV->start;
    abv_have_read = 0;
    pABV->wr_ok = 1;	
#else
#endif
}

/*
  *
  */
int ABV_skip(int dis, int wherence)
{
	if(dis <= 0){
		EPRINTF(("ABV error usage\n"));
		return -1;
	}
#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE
	pABV->rdptr = (wherence == SEEK_SET ? pABV->start : pABV->rdptr);
	abv_have_read = (wherence == SEEK_SET ? 0 : abv_have_read);
	while(dis){
		while( dis && pABV->wr_ok ) {
			pABV->rdptr++;			
			abv_have_read++;
			dis--;
			if(pABV->rdptr >= pABV->end){
				EPRINTF(("READ TO THE TOP\n"));
				pABV->rdptr = pABV->start;
				pABV->wr_ok = 0;
				
				if(ABV_thread > 0)
					cyg_thread_resume(ABV_thread);
				
				cyg_thread_suspend(decoder_thread);
				cyg_thread_yield();
			}
		}
	}
#else
#endif

	return 0;
}


int ABV_write(FILE *src)
{
	unsigned int ubsize = 0;
	unsigned int frest = 0;
#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE
	if( NULL != src ){
		ubsize = pABV->size;
		frest = file_size;
		while(frest){
			if( !pABV->wr_ok){
				if(frest < ubsize){
					abv_have_write += frest;
					fread((uchar *)pABV->wrptr, frest, 1, src);
					pABV->wrptr += frest;
					frest -= frest; //abv_have_write;
					pABV->wr_ok = 1;
					EPRINTF(("ABV write over\n"));
					/*
					if(decoder_thread > 0)
						cyg_thread_resume(decoder_thread);
					cyg_thread_yield();					
					*/	
						return 0; //cyg_thread_exit();				
				}
				else{				
					EPRINTF(("II write %d bytes ...\n", ubsize));
					abv_have_write += ubsize;
					fread((uchar *)pABV->wrptr, ubsize, 1, src);	
					EPRINTF(("II write end\n"));			
					pABV->wrptr = pABV->start;
					frest -= ubsize; //abv_have_write;
					pABV->wr_ok = 1;
					cyg_thread_yield();
					if(decoder_thread > 0)
						cyg_thread_resume(decoder_thread);
					cyg_thread_suspend(ABV_thread);
				}
			}
		}
	}	
#else
	EPRINTF(("Not Support \"CYGSEM_KERNEL_SCHED_TIMESLICE\"\n"));
#endif
}

void UB_update_rdptr(UBuff *p, unsigned int rdptr)
{
    p->rdptr = rdptr;
    int t = p->wrptr - rdptr;
    if (t < 0)
    {
        t += p->size;
    }
    p->fullness = t;
}

void UB_update_wrptr(UBuff *p, unsigned int wrptr)
{
    p->wrptr = wrptr;
    int t = wrptr - p->rdptr;
    if (t < 0) 
    {
        t += p->size;
    }
    p->fullness = t;
}

#ifdef OUTPUT_I2S
char* UB_getBufferData(UBuff *p, unsigned int size)
#endif
#ifdef OUTPUT_WAV
char* UB_getBufferData(UBuff *p, unsigned int size, FILE *outf)
#endif
{
    unsigned int buf_addr = 0;
    int t = p->wrptr - p->rdptr;
    if((p->size - t)>size)
    {
        return (char *)p->wrptr;
    }
    else
    {
        #ifdef OUTPUT_WAV
        fwrite(p->rdptr, t, 1, outf);
        fflush(outf);
        printf("Output to wav file!!\n");
        UB_reset_pointers(p);
        return (char *)p->wrptr;
        #endif //OUTPUT_WAV

        #ifdef OUTPUT_I2S
        if(i2s_flag == 0)
        {
            buf_addr = i2s_buf0;
            i2s_flag = 1;
            printf("Use i2s_buf0\n");
        }
        else
        {
            buf_addr = i2s_buf1;
            i2s_flag = 0;
            printf("Use i2s_buf1\n");
        }        
        memcpy(buf_addr, p->rdptr, t);
        iis_send_data(buf_addr, t);

        while(0x200 != (iis_get_status(FLAG_GW_EMPTY) & 0x200)) ;
        
        printf("Output to I2S!!\n");
        UB_reset_pointers(p);
        return (char *)p->wrptr;        
        #endif //OUTPUT_I2S
    }
    diag_printf("DBV is overflow!!\n");
    return NULL;
}


void UB_reset_pointers(UBuff *pBuff)
{
    pBuff->rdptr = pBuff->wrptr = pBuff->start;
    pBuff->fullness = 0;
}