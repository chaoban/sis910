/*
 * jdebug.h
 *
 * Copyright (C) 2009, Jiunhau Wang.
 */
/*
*01)LED(x)
*02)compare_buffer(s, d, n, #, print)
*03)sw_memcpy(d, s, o_w, o_h, s_p, d_p)
*/

#ifndef JDEBUG_H
#define JDEBUG_H

#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>	// Kernel API.


//#define software_path_enable       1
//#define performance_testing_enable 1


/* GPIO LED */
#define REG_GPIO_Enable	0x90003740
#define REG_GPIO_EnOut	0x9000374c
#define REG_GPIO_Value	0x90003748
#define rGPIOLED	    *((volatile unsigned *)(REG_GPIO_Value))
#define GPLED(_x_)	    rGPIOLED=led_data[_x_]

cyg_uint8  led_data[16] = {0x40, 0x79, 0x24, 0x30,  //0,1,2,3
                           0x19, 0x12, 0x02, 0x78,  //4,5,6,7
                           0x00, 0x10, 0x08, 0x03,  //8,9,a,b
                           0x46, 0x21, 0x06, 0x0e   //c,d,e,f
                          };
#define LED(input_number) \
{ \
	HAL_WRITE_UINT32(REG_GPIO_Enable, 0x3f); \
	HAL_WRITE_UINT32(REG_GPIO_EnOut,  0x3f); \
	GPLED(input_number); \
}

#ifdef performance_testing_enable
#define TICKS_PER_SECOND	100
cyg_tick_count_t t_start = 0, t_end = 0;

void tick2time(cyg_tick_count_t t)
{
    cyg_uint32 tempseconds = t/TICKS_PER_SECOND;
    cyg_uint32 day = 0;
    cyg_uint32 hour = 0;
    cyg_uint32 minute = 0;
    cyg_uint32 sec = 0;
    cyg_uint32 msec = 0;
    if(tempseconds < 0)
    {
        diag_printf("Error tick is a minus number!\n");
    }

	/* DAY */
    if(tempseconds >= 86400){
		day = (tempseconds / 86400);
		tempseconds -= (day * 86400);
    }
    /* HOUR */
    if(tempseconds >= 3600){
		hour = (tempseconds/3600);
		tempseconds -= (hour*3600);
    }
    /* MINUTE */
    if(tempseconds >= 60){
		minute = (tempseconds/60);
		tempseconds -= (minute*60);
    }
    /* SECOND */
    sec = tempseconds;
    
    /* 1/100 SECOND */
//	msec = (tempseconds*1000);
//	msec = (msec%1000)/10;
    msec = t%TICKS_PER_SECOND;
    
    if(day > 0){
        diag_printf(" %d day %d hour %d min %d.%02d sec.", day, hour, minute, sec, msec);
    } 
    else if(hour > 0){
        diag_printf(" %d hour %d min %d.%02d sec.", hour, minute, sec, msec);
    }
    else if(minute > 0){
        diag_printf(" %d min %d.%02d sec.", minute, sec, msec);
    }
    else{
        diag_printf(" %d.%02d sec.\n", sec, msec);
    }
}
void wait_for_tick(void)
{
    cyg_tick_count_t tv0, tv1;
    tv0 = cyg_current_time();
    while (true) {
        tv1 = cyg_current_time();
        if (tv1 != tv0) break;
    }
}

void time_set(void)
{
    wait_for_tick();
    t_start = cyg_current_time();  
}
cyg_tick_count_t time_get(void)
{ 
    cyg_tick_count_t value = 0;
    t_end = cyg_current_time();
    value = t_end - t_start;
    tick2time(value);
    return value;
}
#endif // ~performance_testing_enable

void sw_memcpy(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, int obj_height, int src_width, int dst_width)
{
    src_width = src_width << 2; 
    dst_width = dst_width << 2;
    obj_width = obj_width << 2;
    int i = 0, j = 0, k = 0;
	cyg_int32 temp = 0, temp1 = 0;

	for(j = 0; j < obj_height; j++)
	{
		for(i = 0,k = 0; i < obj_width; i += 4, k += 4)
		{
			temp  = j * src_width + i;
			temp1 = j * dst_width + k;
			dst[temp]   = src[temp1];
			dst[temp+1] = src[temp1+1];
			dst[temp+2] = src[temp1+2];
			dst[temp+3] = src[temp1+3];
		}       
	}
}

int compare_buffer(cyg_uint8 *src, cyg_uint8 *dst, cyg_uint32 length, cyg_int16 label, boolean on)
{
    int i = 0;
    int diff = 0;
    #if 1
    for (i = 0; i < length; i++)
    {
        if (src[i] != dst[i])    
            diff++;
    }
    #endif
    #if 0
    cyg_uint32 *src_temp = (cyg_uint32 *)src;
    cyg_uint32 *dst_temp = (cyg_uint32 *)dst;
    for(i = 0; i < (length >> 2); i++)
    {
        if(src_temp[i + offset] != dst_temp[i])    
            diff++;
    }
    #endif
    if (diff != 0)
    {
        diag_printf("[#%d] : compare buffer FAIL ! =  %d different\n", label, diff);
        if (on)
        {
            diag_printf("[SRC]============================\n");
            diag_dump_buf_with_offset(src, length, 0);
            diag_printf("[DST]============================\n");
            diag_dump_buf_with_offset(dst, length, 0);
        }
    }
    else
    {
        diag_printf("[#%d] : compare buffer PASS !\n", label);
    }
    return diff;
}

void jpeg_lag(int x)//x = 0.001 sec
{
	cyg_uint32 i = 0, j = 0;
	for(j = 0; j < x; j++)
	{
		for(i = 0; i < 4000; i++)
		{
			asm("NOP");
		}
	}
}
#ifdef software_path_enable
int blocktileswblt (char *src, char *dst, int dstpitch, int blockpixelwidth, int blockpixelheight, int pixelbitcount)
{
    int i = 0, j = 0, k = 0;
    
    for (i = 0; i < blockpixelheight; i++)
    {
        int linestartoffset= i * dstpitch;
        int dstlinestartaddr = (cyg_uint32)dst + linestartoffset;

        for (j = 0; j < blockpixelwidth; j++)
        {
            cyg_uint32 dstpixeladdr = dstlinestartaddr + j * pixelbitcount;
            cyg_uint32 srcpixeladdr = (cyg_uint32)src + (((i * blockpixelwidth) + j) * pixelbitcount);

            for (k = 0; k < pixelbitcount; k++)
            {
                cyg_uint32 dstbyteaddr = dstpixeladdr + k;
                cyg_uint32 srcbyteaddr = srcpixeladdr + k;
                char *dstbyteaddrtemp = (char *)dstbyteaddr;

                *dstbyteaddrtemp= *((char *)srcbyteaddr);
            }
        }
    }
    return 0;
}

int tileswblt (char *src, int srcpitch, char *dst, int dstpitch, int bltpixelwidth, int 
bltpixelheight, int blockpixelwidth, int blockpixelheight, int pixelbitcount)
{
    int i = 0, j = 0;
    int bltblockrownum = 0, bltblockcolnum = 0;
    int srcpitchblockcolnum = 0, dstpitchblockcolnum = 0;
    int blockpixelsize = 0;

    if (srcpitch % (blockpixelwidth * pixelbitcount))
    {
        diag_printf("ERROR: src pitch alignment error!\n");
    }
    if (dstpitch % (blockpixelwidth * pixelbitcount))
    {
        diag_printf("ERROR: dst pitch alignment error!\n");
    }

    blockpixelsize = blockpixelwidth * blockpixelheight;
    bltblockrownum = (bltpixelheight + blockpixelheight - 1) / blockpixelheight;
    bltblockcolnum = (bltpixelwidth + blockpixelwidth - 1) / blockpixelwidth;
    srcpitchblockcolnum = (srcpitch + (blockpixelwidth - 1) * pixelbitcount) / 
    (blockpixelwidth * pixelbitcount);
    dstpitchblockcolnum = (dstpitch + (blockpixelwidth - 1) * pixelbitcount) / 
    (blockpixelwidth * pixelbitcount);

    for (i = 0; i < bltblockrownum; i++)
    {
        int srcstartblockrowoffsetnum = i * srcpitchblockcolnum;
        int dststartblockrowoffsetnum = i * dstpitchblockcolnum;
        cyg_uint32 srcstartblockrowaddr = (cyg_uint32)src + (srcstartblockrowoffsetnum * 
        blockpixelsize * pixelbitcount);
        cyg_uint32 dststartblockrowaddr = (cyg_uint32)dst + (dststartblockrowoffsetnum * 
        blockpixelsize * pixelbitcount);
        
        for (j = 0; j < bltblockcolnum; j++)
        {
            //diag_printf("blt block no: (%d, %d).\n", i, j);
            cyg_uint32 srcbltblockaddr = srcstartblockrowaddr + (j * blockpixelsize * pixelbitcount);
            cyg_uint32 dstbltblockaddr = dststartblockrowaddr + (j * blockpixelwidth* pixelbitcount);
            
            char *dstbltblockaddrtemp = (char *)dstbltblockaddr;
            *dstbltblockaddrtemp = 0x00;
            *(dstbltblockaddrtemp + 1) = 0x00;
            *(dstbltblockaddrtemp + 2) = 0xff;
            *(dstbltblockaddrtemp + 3) = 0x00;
            
            //diag_printf("srcbltblockaddr, dstbltblockaddr: (%d, %d).\n", srcbltblockaddr, dstbltblockaddr);
            //blocktileswblt ((char *)srcbltblockaddr, (char *)dstbltblockaddr, dstpitch, blockpixelwidth, blockpixelheight, pixelbitcount);
        }
    }
    return 0;
}

#endif// ~software_path_enable



#endif /* JDEBUG_H */
