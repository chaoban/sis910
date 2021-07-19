#include <cyg/io/display/crt/Crt_io.h>
#include <stdlib.h>
#include <stdio.h>
#include "image_test.h"

#define HAL_Value_Read_UINT32(_reg_)  *((volatile CYG_WORD32 *)(_reg_))

lcd_info lcd_info_QQ;

void write_frame1(int,int,cyg_uint8,cyg_uint8 *,cyg_uint8,cyg_uint8,cyg_uint8,cyg_uint8,cyg_uint8 *);
void lag(int);
void showregister(void);
void inverse_array(cyg_uint8 *src,int pitch,int h);
void colorbar(cyg_uint8 *dst,int w,int h);


void cyg_user_start( void )
{
	int count=0;
	diag_printf("Start Lcd test ap \n");
	lcd_getinfo(&lcd_info_QQ);
	cyg_uint8 *a,*b;	
	lcd_dev_test();
	a=(cyg_uint8 *)lcd_info_QQ.fb1;
	b=(cyg_uint8 *)lcd_info_QQ.fb2;
	diag_printf("Framebuffer1 : %x\n",a);
	diag_printf("Framebuffer2 : %x\n",b);

	//test mode
	diag_printf("test mode\n");
	lcd_test(1); 
    lag(5);
    lcd_test(0);
    lag(1);
	lcd_set_bg(255,0,0,1);
	lag(3);
	lcd_set_bg(0,255,0,1);
	lag(3);
	lcd_set_bg(0,0,255,1);
	lag(3);
	//DAC power down
	diag_printf("DAC power down\n");
	lcd_power_down(1);
    lag(3);
    lcd_power_down(0);
    lag(2);
	lcd_set_bg(0,0,0,1);
	lag(3);
	lcd_set_bg(255,255,255,1);
	lag(3);
	//gamma use
	//lcd_gamma_table_set(2.1);
	//lcd_gamma_use(1);
	//lcd_gamma_use(0);
	//write frame buffer
	diag_printf("Framebuffer\n");
	inverse_array(test_image1,3*lcd_info_QQ.width,lcd_info_QQ.height);
	for(count=0;count<10;count++)
	{
		write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,a,0,0,0,0,NULL);
		lcd_flip(0);
		lag(3);
		write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,b,0,255,0,0,NULL);	
		lcd_flip(1);
		lag(3);
		write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,a,0,0,255,0,NULL);
		lcd_flip(0);
		lag(3);
		write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,b,0,0,0,255,NULL);
		lcd_flip(1);
		lag(3);
		write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,0,a,0,255,255,255,NULL);
		lcd_flip(0);
		lag(3);
		write_frame1(lcd_info_QQ.width, lcd_info_QQ.height,1,b,0,0,0,0,test_image1);
		lag(3);
		lcd_flip(1);
	}
}
//delay x second
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

void inverse_array(cyg_uint8 *src,int pitch,int h)
{
	int i,j;
	int end = h/2;
	cyg_uint8 temp;
	cyg_uint32 temp1,temp2; 
	for (j = 0 ; j < end ; j ++)
	{
		for (i = 0 ; i < pitch ; i++)
		{
			temp1=j*pitch+i;
			temp2=(h-1-j)*pitch+i;
			temp=src[temp1];
			src[temp1]=src[temp2];
			src[temp2]=temp;
		}
	}
}

