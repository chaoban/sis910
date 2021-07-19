/*
 * printT.c
 * Only for use printf by SPI-Romter when in development,
 * NOT for release.
 */
#include <stdio.h>	//added by chaoban

typedef long int32;             /* 32-bit signed integer */
typedef unsigned long uint32;   /* 32-bit unsigned integer */
typedef unsigned short int16;   /* 16-bit unsigned integer */
typedef unsigned char byte_t;   /*  8-bit unsigned integer */
#define uchar(x) ((unsigned char)(x))
#define MAXINT16 65535U         /* Largest 16-bit integer */
#define MAXINT32 4294967295L    /* Largest 32-bit (unsigned) integer */

#define R16_SPACE       0x00001800
#define X86_SPACE       0x00000800
#define CMD_PRINT       0x00000040
#define CMD_BREAK       0x00000050
#define CMD_R           0x00000010
#define CMD_RW          0x00000011
#define CMD_RB          0x00000012
#define CMD_W           0x00000020
#define CMD_WW          0x00000021
#define CMD_WB          0x00000022

void put_spi_emu(unsigned char *);
void printT( char *fmt, ...);
void break_point(int);
void clean_rdt(void);
void port80(long);
unsigned short read_timer_reg(void);

unsigned char scr_buf[512];

unsigned long  *CTL_R16   = (unsigned long  *)0x90002400;
unsigned long  *CTL_X86   = (unsigned long  *)0x90002404;
unsigned long  *ADDR_R16  = (unsigned long  *)0x90002408;
unsigned long  *ADDR_X86  = (unsigned long  *)0x9000240c;
unsigned long  *DATA_R16  = (unsigned long  *)0x90002410;
unsigned short *DATA_R16W = (unsigned short *)0x90002410;
unsigned char  *DATA_R16B = (char           *)0x90002410;
unsigned long  *ptr;
unsigned short *ptr_w;
unsigned char  *ptr_b;

void printT(char *fmt, ...)
{
    unsigned char *buf = (char *)0x00800000;
    
	vsprintf(buf, fmt, (&fmt)+1);
    put_spi_emu (buf);
}

void put_spi_emu (unsigned char *txt)
{
int     i;

    do {
        i = *CTL_R16;
        i = i & 0x000000ff;
    } while (i);

    *ADDR_R16 = R16_SPACE;

    do {
        *DATA_R16B = *txt;
        txt++;
    } while (*txt!='\n');

    *DATA_R16B = *txt;

    *CTL_R16 = CMD_PRINT;
}

void clean_rdt ()
{
        *CTL_R16 = 0;
}

void break_point (int bp_number)
{
	int     breaking,i;

    do {
        breaking = *CTL_R16 & 0x000000ff;
    } while (breaking);

    *ADDR_R16 = bp_number;
    *CTL_R16  = CMD_BREAK;

    do {
        i = *CTL_X86 & 0x000000ff;              // Check X86 command
        if (i) {
                i = *CTL_X86 & 0x000000ff;      // Read twice to de-glitch
                *ADDR_R16 = X86_SPACE;

                if (i==CMD_R) {
                        ptr = (unsigned long *)*DATA_R16;
                        *DATA_R16 = *ptr;
                }

                if (i==CMD_W) {
                        ptr = (unsigned long *)*DATA_R16;
                        *ptr = *DATA_R16;
                }

                *CTL_X86 = 0;                   // Clear X86 command
        }
        breaking = *CTL_R16 & 0x000000ff;       // Check break point exit
    } while (breaking);
}

void port80(int32 i)
{
    unsigned long *ptr80;
    ptr80 = (unsigned long *)0xB0000080;     /* PC port 80h */
    *ptr80 = i;
}
/*This funcion read and return 16bit resigter of timer ffffffa2
about timer ffffffa2 --- 15 bit - means 1 sec
 0-14 -- Plus 1 each 1/32k sec
*/
unsigned short read_timer_reg()
{
    int16 *reg_timer = (int16*)0xffffffa2;
    return *reg_timer;
}
