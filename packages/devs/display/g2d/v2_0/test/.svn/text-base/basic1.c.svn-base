#include <cyg/io/display/crt/Crt_io.h>
#include <cyg/io/display/g2d/G2D_io.h>
//#include <cyg/devs/display/g2d/G2D_dev.h>
#include <stdlib.h>
#include <stdio.h>
#include "image_test.h"

#define HAL_Value_Read_UINT32(_reg_)  *((volatile CYG_WORD32 *)(_reg_))

lcd_info lcd_info_QQ;

void write_frame1(int,int,cyg_uint8,cyg_uint8 *,cyg_uint8,cyg_uint8,cyg_uint8,cyg_uint8,cyg_uint8 *);
void lag(int);
void showregister(void);

void colorbar(cyg_uint8 *dst,int w,int h);

cyg_uint32 *testimage = NULL;
cyg_uint32 *testimage1 = NULL;
cyg_uint32 *testimage2 = NULL;
cyg_uint32 *alphatemp = NULL;

int src_x = 0, src_y = 0;
int dst_x = 0, dst_y  = 0;
int nCount = 0;


void lag(int x)
{
	int j;
	cyg_uint32 i;
	for(j=0;j<x;j++)
	{
		for(i=0;i<4000000;i++)
		{
			asm("NOP");
		}
	}
}
inline void SW_HW_BITBLT (G2D_ROPF op, cyg_uint32 pattern, cyg_uint8  *dst )
{
    
   	src_x = 0; src_y = 0;
    dst_x = 0; dst_y  = 0;    
	diag_printf (" fill_rect begin 0x%x \n",op);
	fill_rect ( 1, dst, (cyg_uint8 *)testimage1, 0xcc, 0, 800, 480, 800,800);
	lcd_flip(0);	
	lag(1);	
	
	src_x = 50; src_y = 20;                                                   
    dst_x = 500; dst_y  = 20;                                               
	fill_rect ( 1, dst, (cyg_uint8 *)testimage1, op, pattern, 275, 200, 800,800);   
	lcd_flip(0);	                                                        
	lag(1);                                                                 	
	src_x = 50; src_y =  260;                                                  
    dst_x = 500; dst_y = 260;                                              
	fill_rect ( 0, dst, (cyg_uint8 *)testimage1, op, pattern, 275, 200, 800,800);   
	lcd_flip(0);	                                                                                                \
	lag(1); 
}	
void cyg_user_start( void )
{
	int count=0, ret_crt = -1, ret_g2d =-1;
	int i = 0;

	diag_printf("Start Lcd test ap \n");
	lcd_getinfo(&lcd_info_QQ);
	cyg_uint8 *a = NULL, *b = NULL;	
	ret_crt = lcd_dev_test();
	ret_g2d = g2d_dev_test();

	testimage = malloc(sizeof(cyg_uint32)*800*480+0x3ff);
	testimage=((cyg_uint32)testimage+0x3ff)&(~0x3ff);
	testimage1 = malloc(sizeof(cyg_uint32)*800*480+0x3ff);
	testimage1=((cyg_uint32)testimage1+0x3ff)&(~0x3ff);
	alphatemp = malloc(sizeof(cyg_uint32)*800*480+0x3ff);
	alphatemp=((cyg_uint32)alphatemp+0x3ff)&(~0x3ff);
	testimage2 = malloc(sizeof(cyg_uint32)*480*800+0x3ff);
	testimage2=((cyg_uint32)testimage2+0x3ff)&(~0x3ff);
	diag_printf("ret_crt %d, ret_g2d %d \n",ret_crt, ret_g2d);
	a=(cyg_uint8 *)lcd_info_QQ.fb1;
	b=(cyg_uint8 *)lcd_info_QQ.fb2;
	diag_printf("Framebuffer1 : %x\n",a);
	diag_printf("Framebuffer2 : %x\n",b);

	//test mode
	diag_printf("CRT test mode\n");
	diag_printf("Framebuffer\n");
	//inverse_3to4(test_image1,(cyg_uint8 *)testimage,lcd_info_QQ.width,lcd_info_QQ.height);
	init_image();
	inverse_3to4(test_image1,(cyg_uint8 *)testimage,800,480);
    
	diag_printf (" write_frame1 SW begin\n");
	//write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,a,1 ,55,55,55,NULL);
	lcd_flip(0);
	lag(1);

        src_x = 0; src_y = 0;
        dst_x = 0; dst_y  = 0;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 400, 240, 0);       
         
        src_x = 0; src_y = 0;
        dst_x = 0; dst_y  = 240;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 200, 240, 0);
        
        src_x = 0; src_y = 0;
        dst_x = 200; dst_y  = 240;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 200, 120, 0);
        
        src_x = 0; src_y = 0;
        dst_x =200; dst_y  = 360;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 100, 120, 0);
        
        src_x = 0; src_y = 0;
        dst_x =300; dst_y  = 360;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 100, 60, 0);
        
        src_x = 0; src_y = 0;
        dst_x =400; dst_y  = 0;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 400, 480, 0);
        lag(1);
    write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,a,1 ,255,255,255,NULL);
    src_x = 0; src_y = 0;
    dst_x = 0; dst_y  = 0;
    //scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage2, src_x, src_y, 480, 800, 240, 400, 0);
    g2d_memscl(0,a,testimage2,0,0,0,0,0,2,1,0,2,1,480,800,480,800);
    lag(1);
    src_x = 0; src_y = 0;
    dst_x = 0; dst_y  = 0;
    rotate_rect ( 0, a, testimage2, 2, 480, 800, 480,800);
    
    src_x = 0; src_y = 0;
    dst_x = 0; dst_y  = 0;
    rotate_rect ( 0, a, testimage2, 2, 240, 255, 480,800);
    
    lag(1);
    src_x = 5; src_y = 255;
    dst_x = 255; dst_y  = 0;
    rotate_rect ( 0, a, testimage2, 4, 240, 255, 480,800);
    
    src_x = 10; src_y = 510;
    dst_x = 510; dst_y  = 0;
    rotate_rect ( 0, a, testimage2, 3, 240, 255, 480,800);
    write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,a,1 ,255,255,255,NULL);
    //rotate_rect ( 0, a, testimage2, 4, 480, 255, 480,800);
    
//--------------------------------------------------------------------------------------------------------------------------------------					
  int count_QQ = -1;
  while(1)
    {   
        count_QQ++;     
        diag_printf("============================================\n");
        diag_printf(" Count : %d\n",count_QQ);
        //{
        //   
        //    src_x = 0; src_y = 0;
        //    dst_x = 0; dst_y  = 0;    	
        //	fill_rect ( 1, alphatemp, (cyg_uint8 *)testimage1, 0xcc, 0, 800, 480, 800,800);
        //		
        //	for( i = 0; i < 25; i++)
        //	{
        //	    printf("alpha blending\n");
        //	    g2d_alpha_blend(alphatemp, testimage, 5, 800, 480, 800, 800,0 , 1, 0, 1, 255 - (i*10));
        //	    //fill_rect ( 1, a, (cyg_uint8 *)alphatemp, 0xcc, 0, 800, 480, 800,800);
        //	    //lag(3);	    
        //        g2d_alpha_blend(alphatemp, testimage, 7, 800, 480, 800, 800,0 , 1, 255 - (i*10), 1, 0);    
        //        fill_rect ( 1, a, (cyg_uint8 *)alphatemp, 0xcc, 0, 800, 480, 800,800);
        //        lag(2);
        //    }
        //    
        //}
                
        diag_printf ("ROP Function Begin\n");
        
        //SW_HW_BITBLT (ROPF_NOT_P_AND_NOT_D, 0xffdddddd, a);        
        //SW_HW_BITBLT (ROPF_P_XOR_D, 0xffdddddd, a);
        //SW_HW_BITBLT (ROPF_S_AND_P, 0xffdddddd, a);
        //SW_HW_BITBLT (ROPF_NOT_P_OR_S, 0xffdddddd, a);
        //SW_HW_BITBLT (ROPF_P, 0x00000088, a);
        //SW_HW_BITBLT (ROPF_NOT_S_OR_D_OR_P, 0xffdddddd, a);
 //--------------------------------------------------------------------------------------------------------------------------------------
        //g2d_rotation(a, src, 1, rotation, 800, 480, 480*4, 800*4);
 
//--------------------------------------------------------------------------------------------------------------------------------------
        src_x = 0; src_y = 0;
        dst_x = 0; dst_y  = 0;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 400, 240, 0);       
         
        src_x = 0; src_y = 0;
        dst_x = 0; dst_y  = 240;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 200, 240, 0);
        
        src_x = 0; src_y = 0;
        dst_x = 200; dst_y  = 240;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 200, 120, 0);
        
        src_x = 0; src_y = 0;
        dst_x =200; dst_y  = 360;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 100, 120, 0);
        
        src_x = 0; src_y = 0;
        dst_x =300; dst_y  = 360;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 100, 60, 0);
        
        src_x = 0; src_y = 0;
        dst_x =400; dst_y  = 0;
        scal_rect( 1, a, dst_x , dst_y, 800, 480, testimage, src_x, src_y, 800, 480, 400, 480, 0);
        lag(1);
        
        SW_HW_BITBLT (ROPF_8_ZEROES, 0, a);    
        SW_HW_BITBLT (ROPF_NOT_S_AND_NOT_D, 0, a);
        SW_HW_BITBLT (ROPF_NOT_S_AND_D, 0, a);
        SW_HW_BITBLT (ROPF_NOT_S, 0, a);
        SW_HW_BITBLT (ROPF_S_AND_NOT_D, 0, a);
        SW_HW_BITBLT (ROPF_NOT_D, 0, a);    
        SW_HW_BITBLT (ROPF_S_XOR_D, 0, a);
        SW_HW_BITBLT (ROPF_NOT_S_OR_NOT_D, 0, a);
        SW_HW_BITBLT (ROPF_S_AND_D, 0, a);
        SW_HW_BITBLT (ROPF_D, 0, a);
        SW_HW_BITBLT (ROPF_NOT_S_OR_D, 0, a);    
        SW_HW_BITBLT (ROPF_S, 0, a);    
        SW_HW_BITBLT (ROPF_S_OR_NOT_D, 0, a);
        SW_HW_BITBLT (ROPF_S_OR_D, 0, a);        
        SW_HW_BITBLT (ROPF_8_ONES, 0, a);
        SW_HW_BITBLT (ROPF_S_XOR_NOT_D, 0, a);
    
        nCount ++;
        diag_printf ("BitBlt and Scaling %d\n", nCount);
    
    }
//--------------------------------------------------------------------------------------------------------------------------------------
       		      

}
//delay x second

void colorbar(cyg_uint8 *dst,int w,int h)
{
	int pitch=4*w;
	int i,j;
	cyg_int32 temp=0;
	for(j=0;j<h;j++)
	{
		for(i=0;i<pitch;i+=4)
		{
			temp=j*pitch+i;
			if (i < (pitch/4))
			{
				dst[temp]  =0xff;
				dst[temp+1]=0xff;
				dst[temp+2]=0;
				dst[temp+3]=0;
			}
			if (i >= (pitch/4) && i < (2*pitch/4))
			{
				dst[temp]  =0xff;
				dst[temp+1]=0;
				dst[temp+2]=0xff;
				dst[temp+3]=0;
			}
			if (i >= (2*pitch/4) && i < (3*pitch/4))
			{
				dst[temp]  =0xff;
				dst[temp+1]=0;
				dst[temp+2]=0;
				dst[temp+3]=0xff;
			}
			if (i >= (3*pitch/4))
			{
				dst[temp]  =0xff;
				dst[temp+1]=0xff;
				dst[temp+2]=0xff;
				dst[temp+3]=0xff;
			}	
		}
	}
}

inline void ropf_opertion (cyg_uint8 *dst, cyg_uint8 *src, int ropf,int pattern)
{
    
    switch (ropf)
    {
        case ROPF_8_ZEROES:
            *dst = 0x0;
            break;
        case ROPF_NOT_P_AND_NOT_D:
            *dst = (~ropf) & (~ *dst);
            break;
        case ROPF_NOT_S_AND_NOT_D:
            *dst = (~ *src) & ( ~*dst);
            break;
        case ROPF_NOT_S_AND_D:
            *dst = (~ *src) & *dst;
            break;
        case ROPF_NOT_S:
            *dst = (~ *src);
            break;
        case ROPF_S_AND_NOT_D:
            *dst =  (*src) & (~*dst);
            break;
        case ROPF_NOT_D:
            *dst = ~ *dst;
            break;
        case ROPF_P_XOR_D:
            *dst = ropf ^ *dst;
            break;
        case ROPF_S_XOR_D:
            *dst = *src ^ *dst;
            break;
        case ROPF_NOT_S_OR_NOT_D:
            *dst = (~ *src) | (~ *dst);
            break;
        case ROPF_S_AND_D:
            *dst = *src & * dst;
            break;
        case ROPF_D:
            *dst = *dst;
            break;
        case ROPF_NOT_S_OR_D:
            *dst = (~ *src) | (*dst);
            break;
        case ROPF_S_AND_P:
            *dst = *src & ropf;
            break;
        case ROPF_S:
            *dst = *src;
            break;
        case ROPF_NOT_P_OR_S:
            *dst = ~ropf | *src;
            break;
        case ROPF_S_OR_NOT_D:
            *dst = (*src) | ( ~*dst);
            break;
        case ROPF_S_OR_D:
            *dst = (*src) | (*dst);
            break;
        case ROPF_P:
            *dst = ropf;
            break;
        case ROPF_NOT_S_OR_D_OR_P:
            *dst = (~*src) | (*dst)| ropf;
            break;
        case ROPF_8_ONES:
            *dst = 0xff;
            break;
        case ROPF_S_XOR_NOT_D:
            *dst = *src ^ (~*dst);
            break;
         default:
           *dst = 0x0;            
            break;
    }
    

}

#if 0
typedef enum {
    ROPF_8_ZEROES        = 0x00,  /*   0 */
    ROPF_NOT_P_AND_NOT_D = 0x05,  /*   5 */
    ROPF_NOT_S_AND_NOT_D = 0x11,  /*  17 */ 
    ROPF_NOT_S_AND_D     = 0x22,  /*  34 */
    ROPF_NOT_S           = 0x33,  /*  51 */
    ROPF_S_AND_NOT_D     = 0x44,  /*  68 */
    ROPF_NOT_D           = 0x55,  /*  85 */
    ROPF_P_XOR_D         = 0x5a,  /*  90 */
    ROPF_S_XOR_D         = 0x66,  /* 102 */
    ROPF_NOT_S_OR_NOT_D  = 0x77,  /* 119 */
    ROPF_S_AND_D         = 0x88,  /* 136 */
    ROPF_D               = 0xaa,  /* 170 */
    ROPF_NOT_S_OR_D      = 0xbb,  /* 187 */
    ROPF_S_AND_P         = 0xc0,  /* 192 */
    ROPF_S               = 0xcc,  /* 204 */
    ROPF_NOT_P_OR_S      = 0xcf,  /* 207 */
    ROPF_S_OR_NOT_D      = 0xdd,  /* 221 */
    ROPF_S_OR_D          = 0xee,  /* 238 */
    ROPF_P               = 0xf0,  /* 240 */
    ROPF_NOT_S_OR_D_OR_P = 0xfb,  /* 251 */
    ROPF_8_ONES          = 0xff,  /* 255 */
    ROPF_S_XOR_NOT_D     = 0x09,  /*   9 */
} G2D_ROPF; // Raster Operating functions

#endif

int submultiple(int a, int b)
{
    int c = b;
    if(a < b)
    {
        b = a;
        a = c;
        c = b;
    }
    while(c != 0)
    { 
        c = a%b;
        a = b;
        b = c;
    }
    return (a+b);
}

scal_rect( bool bHW, cyg_uint32 *dstaddr, int dstx, int dsty, int dstw,
		      int dsth, cyg_uint32 *srcaddr, int srcx, int srcy, int srcw, int srch,
		      int objw, int objh, long op)
		      
{
    float h_scale, v_scale;
    int vsr_i, vsr_n, vsr_m ,p;
    int hsr_i, hsr_n, hsr_m;
	
    cyg_uint32 *dsttemp = NULL;
    cyg_uint32 *srctemp = NULL;
    
    dsttemp = dstaddr + (dstw * dsty + dstx) ;
    srctemp = srcaddr + (srcw * srcy + srcx) ;
    
    if(bHW)
    {
        p = submultiple(dsth, objh);
        vsr_i = dsth / objh;
        vsr_n = (dsth % objh) / p;
        vsr_m = objh / p;
        p = submultiple(dstw, objw);
        hsr_i = dstw / objw;
        hsr_n = (dstw % objw) / p;
        hsr_m =objw / p;
        g2d_memscl(0,dstaddr, srcaddr, srcx, srcy, dstx, dsty,
                       vsr_i, vsr_n, vsr_m, hsr_i, hsr_n, hsr_m, srcw, srch, srcw, dstw);
        //printf("vsr_i %d, vsr_n %d, vsr_m %d, hsr_i %d, hsr_n %d, hsr_m %d, srcw %d, srch %d", vsr_i, vsr_n, vsr_m, hsr_i, hsr_n, hsr_m, srcw, srch);                       
        //g2d_memscl(dstaddr,srcaddr,0,0,0,0,0,2,1,0,2,1,800,480,800,800);
        //g2d_memscl(dstaddr,srcaddr,0,0,0,0,2,0,1,2,0,1,400,240,800,800);
        //g2d_memscl(dstaddr, srcaddr,  0, 0, 0, 0,
          //             0, 1, 1, 0, 1, 1, 800, 480,800, 800);
    }
    else
    {
        h_scale = dstw / srcw;
        v_scale = dsth / srch;
        g2d_bilinear(srctemp, srcw, srch, dsttemp, h_scale, v_scale);
    }
    
}

void rotate_rect ( bool bHW, cyg_uint8 *dstaddr, cyg_uint8 *objaddr,int rotate, int obj_width, int obj_height, int src_width,int dst_width)
{
    int src_offset = 0;
    int dst_offset = 0;
    
    cyg_uint8 *dsttemp = NULL;
    cyg_uint8 *srctemp = NULL;
    
    src_offset = (src_width * src_y + src_x) * 4;
    dst_offset = (dst_width * dst_y + dst_x) * 4;
    
    
    dsttemp = dstaddr + dst_offset;
    srctemp = objaddr + src_offset;
    
    if(bHW)
    {
        g2d_rotation(dsttemp, srctemp, 1, rotate, obj_width, obj_height, src_width, dst_width);
    }
    else
    {
        int  i = 0, j =0;
        cyg_uint8 *src = srctemp, *dst = dsttemp;
        for( j = 0; j < obj_height; j++)
        {
            
            for ( i = 0; i < obj_width; i++)
            {
                src = srctemp + (j*src_width + i)*4;
                if( rotate == 2)
                {
                    dst = dsttemp + ((obj_width - i -1)*dst_width + j)*4 ;
                }
                else if( rotate == 3)
                    dst = dsttemp + ((obj_height - j - 1)*dst_width + (obj_width - i -1))*4 ;
                else if( rotate == 4)
                    dst = dsttemp + (i*dst_width + (obj_height - j - 1))*4 ;
                *dst = *src;                
                src++; dst++;
                *dst = *src;                
                src++; dst++;
                *dst = *src;                
                src++; dst++;
                *dst = *src;                
                src++; dst++;
                                
            }
        }
        
    }
}

void fill_rect ( bool bHW, cyg_uint8 *dstaddr, cyg_uint8 *objaddr,int ropf,cyg_uint32 pattern, int obj_width, int obj_height, int src_width,int dst_width)
{
    int src_offset = 0;
    int dst_offset = 0;
    
    cyg_uint8 *dsttemp = NULL;
    cyg_uint8 *srctemp = NULL;
    
    src_offset = (src_width * src_y + src_x) * 4;
    dst_offset = (dst_width * dst_y + dst_x) * 4;
    
    
    dsttemp = dstaddr + dst_offset;
    srctemp = objaddr + src_offset;
    //printf(" src_x %d, src_y %d, obj_width %d, obj_height %d\n",src_x, src_y, obj_width, obj_height);
    //printf("objaddr %d , src_offset %d, srctemp %d\n", objaddr, src_offset, srctemp);
    //printf("dstaddr %d , dst_offset %d, dsttemp %d\n", dstaddr, dst_offset, dsttemp);
    
    if(bHW)
    {
        g2d_memmix( dsttemp, srctemp, ropf, pattern, obj_width, obj_height,src_width, dst_width);
        //g2d_memmix( dsttemp, srctemp, ropf, pattern, 32, 32,800*4, 800*4);
    }
    else
    {
        int  i = 0, j =0;
        cyg_uint8 *src = srctemp, *dst = dsttemp;
        for( j = 0; j < obj_height; j++)
        {
            src = srctemp + j*src_width*4;
            dst = dsttemp + (j*dst_width*4);
            for ( i = 0; i < obj_width; i++)
            {
                *dst = *src;
                //ropf_opertion(dst,src,ropf, pattern);
                src++; dst++;
                //*dst = *src;
                ropf_opertion(dst,src,ropf, (((pattern) >>0) & 0xff));
                src++; dst++;
                //*dst = *src;
                ropf_opertion(dst,src,ropf, (((pattern) >> 8) & 0xff));
                src++; dst++;
                //*dst = *src;
                ropf_opertion(dst,src,ropf, (((pattern) >> 16) & 0xff));
                src++; dst++;
                                
            }
        }
    }
}
void 
write_frame1(int w,int h,cyg_uint8 choice,cyg_uint8 *dst,cyg_uint8 alpha,cyg_uint8 r,cyg_uint8 g,cyg_uint8 b,cyg_uint8 *src)
{
	int pitch=4*w;
	int pitch_s=3*w;
	int i,j,k;
	cyg_int32 temp,temp1;
	if(choice==0) //signle color
	{
		for(j=0;j<h;j++)
		{
			for(i=0;i<pitch;i+=4)
			{
				temp=j*pitch+i;
				dst[temp]  =0xff;
				dst[temp+1]=r;
				dst[temp+2]=g;
				dst[temp+3]=b;
			}
		}
	}
	else
	{
		for(j=0;j<h;j++)
		{
			for(i=0,k=0;i<pitch;i+=4,k+=3)
			{
				temp =j*pitch+i;
				temp1=j*pitch_s+k;
				dst[temp]  =alpha;
				dst[temp+1]=src[temp1+2];
				dst[temp+2]=src[temp1+1];
				dst[temp+3]=src[temp1];
			}
		}
	}
}
void init_image()
{
    int i,j,k, l, m;
    int end = 480/2;
    cyg_uint32 src_temp1,src_temp2;
    cyg_uint32 dst_temp1,dst_temp2;
    cyg_uint8 *dst;
    int val = 0;
    dst = (cyg_uint8 *) testimage1;
     for(k = 0; k< 3; k++)
    {
        for(j =k*160; j < 160*(k+1); j++)
        {
            for( l =0; l < 4; l++)
            {
                for( i = l*200,m=0; i < (l+1)*200;i++,m++)
                {
                    dst_temp1 = (j*800+i)*4;
                    //dst[dst_temp1] = (j > 255) ? 0: 255 - j;
                    dst[dst_temp1] = 255;
                    //val = 255 - m;
                    val = 155;
                        
	                 if( l==0)
	                 {
	                     dst[dst_temp1+1] = 0;
	                     dst[dst_temp1+2] = 0;
	                     dst[dst_temp1+3] = val;
	                 }
	                 else if( l ==1)
	                 {
	                     dst[dst_temp1+1] = 0;
	                     dst[dst_temp1+2] = val;
	                     dst[dst_temp1+3] = 0;
	                 }
                        else if(l==2)
                        {
                            dst[dst_temp1+1] = val;
	                     dst[dst_temp1+2] = 0;
	                     dst[dst_temp1+3] = 0;
                        }
                        else if(l==3)
                        {
                            dst[dst_temp1+1] = val;
	                     dst[dst_temp1+2] = val;
	                     dst[dst_temp1+3] = val;
                        }
                     
                     //   if( k == 0)
                     //   {
	                 //       dst[dst_temp1+1]=0;
	                 //       dst[dst_temp1+2]=0;
	                 //       dst[dst_temp1+3]=val;
	                 //}
	                 //else if(k ==1)
	                 //{
	                 //    dst[dst_temp1+1]=0;
	                 //    dst[dst_temp1+2]=val;
	                 //    dst[dst_temp1+3]=0;
	                 //}
	                 //else
	                 //{
	                 //    dst[dst_temp1+1]=val;
	                 //    dst[dst_temp1+2]=0;
	                 //    dst[dst_temp1+3]=0;
	                 //}
                }
            }           
        }
    }
    
    dst = testimage2;
    for ( j = 0; j < 255; j++)
    {
        for( i = 0; i < 480; i++)
        {
            dst_temp1 = (j*480+i)*4;
            dst[dst_temp1] = 255;
            if ( i < 10)
            {
                dst[dst_temp1+1] = 0;
	            dst[dst_temp1+2] = 255;
	            dst[dst_temp1+3] = 0;
	        }
	        else
	        {
	            dst[dst_temp1+1] = 100;
	            dst[dst_temp1+2] = j+1;
	            dst[dst_temp1+3] = (i+1) * 255 / 480;
	        }
        }
    }     
    dst += 255*480*4;
    for ( j = 0; j < 255; j++)
    {
        for( i = 0; i < 480; i++)
        {
            dst_temp1 = (j*480+i)*4;            
            dst[dst_temp1] = 255;
            if( i < 10)
            {
                dst[dst_temp1+1] = 0;
	            dst[dst_temp1+2] = 0;
	            dst[dst_temp1+3] = 255;            
	        }
	        else
	        {
	            dst[dst_temp1+1] = (i+1) * 255 / 480;
	            dst[dst_temp1+2] = 100;
	            dst[dst_temp1+3] = j+1;            
	        }
        }
    }
    
    dst += 255*480*4;
    for ( j = 0; j < 255; j++)
    {
        for( i = 0; i < 480; i++)
        {
            dst_temp1 = (j*480+i)*4;
            dst[dst_temp1] = 255;
            if( i < 10)
            {
                dst[dst_temp1+1] = 255;            
                dst[dst_temp1+2] = 0;
	            dst[dst_temp1+3] = 0;
	        }
	        else
	        {	            
	            dst[dst_temp1+1] = j+1;            
                dst[dst_temp1+2] = (i+1) * 255 / 480;
	            dst[dst_temp1+3] = 100;
	        }
	        
        }
    }
    
    dst += 255*480*4;
    for ( j = 0; j < 35; j++)
    {
        for( i = 0; i < 480; i++)
        {
            dst_temp1 = (j*480+i)*4;
            dst[dst_temp1] = 255;
            dst[dst_temp1+1] = 255;            
            dst[dst_temp1+2] = 255;
	        dst[dst_temp1+3] = 255;
	        
        }
    }
}
void inverse_3to4(cyg_uint8 *src,cyg_uint8 *dst, int w,int h)
{
	int i,j,k, l;
	int end = h/2;
	cyg_uint32 src_temp1,src_temp2;
	cyg_uint32 dst_temp1,dst_temp2;
	int val = 0;
 
       for (j = 0 ; j < h ; j ++)
	{
		for (i = 0 ; i < w ; i++)
		{
			src_temp1=(j*w+i)*3;
			src_temp2=((h-1-j)*w+i)*3;
			dst_temp1=(j*w+i)*4;
			dst_temp2=((h-1-j)*w+i)*3;
			dst[dst_temp1] = 255;
			dst[dst_temp1+1]=src[src_temp2+2];
			dst[dst_temp1+2]=src[src_temp2+1];
			dst[dst_temp1+3]=src[src_temp2];						
			
		}
	}        	
}
