#ifndef _IO_IIS_H_
#define _IO_IIS_H_
#include <cyg/kernel/kapi.h> //cyg_vector
#include <errno.h>           // Cyg_ErrNo

//IIS control key
#define FLAG_BITRATE		0x00000001
#define FLAG_SAMPLERATE		0x00000002

//Gateway
#define FLAG_BUF_ADDR   	0x00000004
#define FLAG_BUF_SIZE   	0x00000008
#define FLAG_GW_EMPTY		0x00000010

//for IIS_INIT
#define	FLAG_IIS_ENABLE		0x00001000
#define	FLAG_IIS_MCLK_MODE	0x00002000
#define	FLAG_IIS_BCLK_MODE	0x00004000
#define	FLAG_IIS_LR_MODE	0x00008000
#define	FLAG_IIS_1CH		0x00010000
#define	FLAG_IIS_EN_MCLK	0x00022000

#define FIFO_SIZE       (127*256) //dx*dy dx <= 127 dy <= 2048
#define MAX_DX          127
#define MAX_DY          2048

int iis_set_value(cyg_uint32 bitrate, cyg_uint32 samplerate);
int iis_send_data(cyg_uint32 addr, cyg_uint32 size);
cyg_uint32 iis_get_status(cyg_uint32 key);
#endif //_IO_IIS_H_