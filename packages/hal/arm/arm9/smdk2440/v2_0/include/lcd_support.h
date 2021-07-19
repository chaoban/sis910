#ifndef _LCD_SUPPORT_H_
#define _LCD_SUPPORT_H_
//==========================================================================
//
//        lcd_support.h
//
//        qt2440, samsung s3c2440
//
//==========================================================================


struct lcd_info {
    short height, width;  // Pixels
    short bpp;            // Depth (bits/pixel)
    short type;
    short rlen;           // Length of one raster line in bytes
    void  *fb;            // Frame buffer
};

// Frame buffer types - used by MicroWindows
#define FB_TRUE_RGB555 0x02  

// Exported functions
void lcd_init(int depth);
void lcd_clear(void);
int  lcd_getinfo(struct lcd_info *info);
void lcd_on(bool enable);

void lcd_moveto(int X, int Y);
void lcd_putc(cyg_int8 c);
int  lcd_printf(char const *fmt, ...);
void lcd_setbg(int red, int green, int blue);
void lcd_setfg(int red, int green, int blue);
void lcd_comm_init(void);


#endif //  _LCD_SUPPORT_H_
