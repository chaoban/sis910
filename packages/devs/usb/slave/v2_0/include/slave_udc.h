#ifndef _slave_udc_H_
#define _slave_udc_H_

#include <cyg/infra/cyg_type.h>
#include <errno.h>	// Cyg_ErrNo
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/ehci.h>
#include <cyg/io/slave.h>

#define	NEW_CODE	1	//1 for 0525, 0 for 0312 code version


#define	DRAM_BASE_DEBUG	1
#define	ACK_EN	0x18000000	//high speed
//#define	ACK_EN	0x080000000	//full speed

#define	DP_HIGH	1
#define	HW_SWAP	1

#define	SCSI_CMD_INVALID				0xFF
/* align 16N for DMA */
//#define	BULK_IO_BUF		0x40400000	//512*2 bytes
//#define	BULK_CMD_BUF	0x40400400	//31 bytes
//#define	BULK_STS_BUF	0x40400420	//13 bytes
//
//#define	RAM_DISK_START	0x40500000
//#define	RAM_DISK_SIZE	(55296)	//27MB

// XXX
// Definitions for gateway related registers
//

#define gw_rff3_dir         0x900017d0
#define dev_warb_ctrl0      0x90003620
#define dev_warb_ctrl1      0x90003624
#define dev_rarb_ctrl       0x90003628
#define dev_dir_ctrl        0x90003604
#define dev_fifo_en         0x9000362c
#define dev_peri_irqen      0x90003610
#define gw_rd2_init0        0x900014a0
#define gw_rd2_init1        0x900014a4
#define gw_wt2_init0        0x90001420
#define gw_wt2_init1        0x90001424
#define gw_wt3_init0        0x90001430
#define gw_wt3_init1        0x90001434
#define gw_rd3_init0        0x900014b0
#define gw_rd3_init1        0x900014b4
#define gw_irq              0x90001700
#define dev_peri_irqst      0x90003614
#define gw_wff_flush        0x90001714
#define padreg1				0x90004204
#define cat2(i,j)           i##j
#define cat3(i,j,k)         i##j##k
#define DIR_SD2G            (1<<12)


#define cat2(i,j)  	i##j
#define cat3(i,j,k)     i##j##k
#define gw_xfer(ch, addr, dx, dy, ctl) do {        \
		iiow(0x90001710,-1);	\
		iiow(0x90001710,0<<10);	\
		iiow(0x90001764, 0x08100000);	\
  iiow(cat3(gw_,ch,_init0), (dy << 9) | dx << 2);                 \
  iiow(cat3(gw_,ch,_init1), (addr & 0x0FFFFFF0) | ctl);      \
} while(0)   


#define r16_cache_flush(_buf_, _len_)                                     \
    CYG_MACRO_START                                                       \
        cyg_uint8 *_p_ = (cyg_uint8 *)_buf_;                              \
        _p_ -= ((cyg_uint32)_p_ & 31);                                    \
        do                                                                \
        {                                                                 \
            HAL_WRITE_UINT32(((cyg_uint32)_p_ | 0x70000000), 0x0);        \
            _p_ += 32;                                                    \
        } while (_p_ <= ((cyg_uint8 *)_buf_ + _len_));                    \
    CYG_MACRO_END
    
// From sd card define
/* #define gw_xfer(ch, addr, dx, dy, ctl) do {                               \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init0), (dy << 9) | dx << 2);       \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init1), (addr & 0xfffffff0) | ctl); \
} while(0) */

int slave_udc_start(void);
//int slave_udc_ctrl_end(void);
cyg_uint32 slave_udc_bout_getstatus(void);
cyg_uint32 slave_udc_bin_getstatus(void);
void slave_udc_bout_clrstatus(void);
void slave_udc_bin_clrstatus(void);

int slave_udc_bulkout(cyg_uint32 *data_len, cyg_uint8 *data);
int slave_udc_bulkin(cyg_uint32 *data_len, cyg_uint8 *data);

#endif
