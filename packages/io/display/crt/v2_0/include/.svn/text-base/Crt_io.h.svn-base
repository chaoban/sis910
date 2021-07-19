#ifndef _Crt_io_H_
#define _Crt_io_H_

#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <errno.h>   

typedef struct {
    cyg_uint16 height;
    cyg_uint16 width;        // Pixels
    cyg_uint8 bpp;           // Depth (bits/pixel)
    cyg_uint16 type;
    cyg_uint16 rlen;         // Length of one raster line in bytes
    void  *fb1;              // Frame buffer
    void  *fb2;              // Frame buffer
}lcd_info;

typedef struct{
	bool on;
	bool choice;
	float ga;
	cyg_uint8 CRT_r;
	cyg_uint8 CRT_g;
	cyg_uint8 CRT_b;
	bool returnvalue;
}CRT_option;

/*private*/
int lcd_dev_test(void);
int lcd_fun_test(int err);

/*public*/
void lcd_set_bg(cyg_uint8 r, cyg_uint8 g, cyg_uint8 b, bool on);
void lcd_power_down(bool on);
void lcd_test(bool on);
void lcd_gamma_use(bool on);
void lcd_gamma_table_set(float g);
int  lcd_getinfo(lcd_info *info);
void lcd_on(bool enable);
bool lcd_flip(bool choice);
void write_frame(int w, int h, cyg_uint8 choice, cyg_uint8 alpha, cyg_uint8 
*dst, cyg_uint8 r, cyg_uint8 g, cyg_uint8 b, cyg_uint8 *src);
#endif
