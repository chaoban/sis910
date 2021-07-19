#include <stdio.h>
#include <stdlib.h>
#include <cyg/io/io.h>
#include <cyg/io/display/crt/Crt_io.h>

CRT_option sis_crt = {0};
cyg_uint32 len = sizeof(sis_crt);

cyg_io_handle_t siscrt_handle = NULL;
Cyg_ErrNo crt_err;

int lcd_dev_test(void)
{
	int re = 0;
	crt_err = cyg_io_lookup("/dev/display/crt", &siscrt_handle); 
	if(ENOERR != crt_err || NULL == siscrt_handle) 
    {
        diag_printf("LCD IO cyg_io_lookup error!!! \n");
        re = -1;
    }
	return re;
}
int lcd_fun_test(int err)	
{
    int re = 0;
    if(ENOERR != err) 
    {
        diag_printf("LCD IO cyg_io_set_config error!! \n");
		re = -1;
    }
    return re;
}	
void lcd_set_bg(cyg_uint8 r, cyg_uint8 g, cyg_uint8 b, bool on)
{
	sis_crt.CRT_r = r;
	sis_crt.CRT_g = g;
	sis_crt.CRT_b = b;
	sis_crt.on = on;
	lcd_dev_test();
	crt_err = cyg_io_set_config(siscrt_handle, 4, &sis_crt, &len);
	lcd_fun_test(crt_err);
}
void lcd_power_down(bool on)
{
	sis_crt.on = on;
	lcd_dev_test();
	crt_err = cyg_io_set_config(siscrt_handle, 6, &sis_crt, &len);
	lcd_fun_test(crt_err);
}
void lcd_test(bool on)
{
	sis_crt.on = on;
	lcd_dev_test();
	crt_err = cyg_io_set_config(siscrt_handle, 5, &sis_crt, &len);
	lcd_fun_test(crt_err);
}
bool lcd_flip(bool choice)
{
	sis_crt.choice = choice;
	lcd_dev_test();
	crt_err = cyg_io_set_config(siscrt_handle, 3, &sis_crt, &len);
	lcd_fun_test(crt_err);
	return choice;
}

void lcd_gamma_use(bool on)
{
	sis_crt.on = on;
	lcd_dev_test();
	crt_err = cyg_io_set_config(siscrt_handle, 1, &sis_crt, &len);
	lcd_fun_test(crt_err);
}	
void lcd_gamma_table_set(float g)
{
	sis_crt.ga = g;
	lcd_dev_test();
	crt_err = cyg_io_set_config(siscrt_handle, 2, &sis_crt, &len);
	lcd_fun_test(crt_err);
}

void lcd_on(bool enable)
{	
    lcd_flip(0);
}
int lcd_getinfo(lcd_info *info)
{
	lcd_dev_test();
	crt_err = cyg_io_read(siscrt_handle, info, &len);
	lcd_fun_test(crt_err);
	return 1;
}
void write_frame(int w, int h, cyg_uint8 choice, cyg_uint8 alpha, cyg_uint8 
        *dst, cyg_uint8 r, cyg_uint8 g, cyg_uint8 b, cyg_uint8 *src)
{
	int pitch = w << 2;
	int pitch_s = 3 * w;
	int i = 0, j = 0, k = 0;
	cyg_int32 temp = 0, temp1 = 0;
	if(choice == 0) //signle color
	{
		for(j = 0; j < h; j++)
		{
			for(i = 0; i < pitch; i+=4)
			{
				temp = j * pitch + i;
				dst[temp]   = alpha;
				dst[temp+1] = r;
				dst[temp+2] = g;
				dst[temp+3] = b;
			}
		}
	}
	else
	{
		for(j = 0; j < h; j++)
		{
			for(i = 0, k = 0; i < pitch; i+=4, k+=3)
			{
				temp = j * pitch + i;
				temp1 = j * pitch_s + k;
				dst[temp]   = alpha;
				dst[temp+1] = src[temp1];
				dst[temp+2] = src[temp1+1];
				dst[temp+3] = src[temp1+2];
			}
		}
	}
}
