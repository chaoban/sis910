#ifndef _IIS_H_
#define _IIS_H_

#include <cyg/kernel/kapi.h> //cyg_vector
#include <errno.h>           // Cyg_ErrNo
#include <cyg/io/devtab.h>

/**************************
 * Gateway Definition	  *
 **************************/
#define gw_irq                  0x90001700
#define gw_fifo_reset           0x90001710

#define gw_wff_val0             0x90001740
#define gw_wff_val1             0x90001744

#define gw_rff_val0             0x90001760
#define gw_rff_val1             0x90001764

#define gw_rd1_init0            0x90001490
#define gw_rd1_init1            0x90001494

#define gw_lpc0                 0x90001500
#define gw_lpc1                 0x90001504
#define gw_lpc2                 0x90001508
#define gw_lpc3                 0x9000150c
#define gw_lpc4                 0x90001510
#define gw_lpc5                 0x90001514
#define gw_lpc6                 0x90001518
#define gw_lpc7                 0x9000151c

#define gw_rd1_empty            0x200

#define cat3(i,j,k)         i##j##k
#define gw_xfer(ch, addr, dx, dy, ctl) do {                               \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init0), (dy << 9) | dx << 2);       \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init1), (addr & 0xfffffff0) | ctl); \
} while(0)
		
/**************************************
**  Initial Register Definition  ******
***************************************/
#define IIS_INIT				0x900033c0
#define IIS_BUF_DATA			0x900033c8
/**************************************
**  Initial Register Value Definition***
***************************************/
//0x900033c0
#define IIS_ENABLE  				0x00000001
#define IIS_EN_MCLK  				0x00000002
#define IIS_BCLK_MSK				0x000000F0
#define IIS_LR_MODE					0x00001000
#define IIS_EN_1CH  				0x00002000
#define IIS_MCLK_MSK				0x00FF8000
#define IIS_ENABLE_SHT				0
#define IIS_EN_MCLK_SHT				1
#define IIS_BCLK_SHT				4
#define IIS_LR_MODE_SHT				12
#define IIS_EN_1CH_SHT				13
#define IIS_MCLK_SHT				15

#define	IIS_LR_16_VAL				0x0<<IIS_LR_MODE_SHT
#define IIS_LR_24_VAL				0x1<<IIS_LR_MODE_SHT

//0x900033c8
#define IIS_NO_DATA_MSK				0x00000001
#define IIS_BUF_EMPTY_MSK			0x00000002
#define	IIS_BUF_FULL_MSK			0x00000004
#define IIS_SEND_END_MSK			0x00000008
#define IIS_DEBUG_MSK				0x000000F0
#define IIS_POINT_COUNT_MSK			0x00003F00
#define IIS_STATE_MSK				0x0000C000


/************************************************************
 * global functions to be exported
 ************************************************************/
extern void iis_initfun();
#endif //_IIS_H_
