//==========================================================================
//
//      memorystick_sis910.c
//
//      Provide a disk device driver for Memory Stick (Pro) cards
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2006 eCosCentric Limited
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author:       Zhou Li
// Date:         2008-05-20
// Author:       Sidney Shih
// Date:         2008-09-05
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_disk_memorystick_sis910.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_if.h>             // delays
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>
#include <cyg/io/memorystick_protocol.h>
#include <cyg/io/memorystick_sis910.h>

// XXX
// Definitions for gateway related registers
//
#define gw_fifo_reset       0x90001710
#define gw_wff_val0         0x90001740
#define gw_wff_val1         0x90001744
#define gw_rff_val0         0x90001760
#define gw_rff_val1         0x90001764
#define gw_lpc0             0x90001500
#define gw_lpc1             0x90001504
#define gw_lpc2             0x90001508
#define gw_lpc3             0x9000150C
#define gw_lpc4             0x90001510
#define gw_lpc5             0x90001514
#define gw_lpc6             0x90001518
#define gw_lpc7             0x9000151C
#define gw_rff3_dir         0x900017d0
#define dev_warb_ctrl0      0x90003620
#define dev_warb_ctrl1      0x90003624
#define dev_rarb_ctrl       0x90003628
#define dev_dir_ctrl        0x90003604
#define dev_fifo_en         0x9000362c
#define dev_peri_irqen      0x90003610
#define gw_wt1_init0        0x90001410
#define gw_wt1_init1        0x90001414
#define gw_rd3_init0        0x900014b0
#define gw_rd3_init1        0x900014b4
#define gw_irq              0x90001700
#define dev_peri_irqst      0x90003614
#define cat2(i,j)           i##j
#define cat3(i,j,k)         i##j##k

#define gw_xfer(ch, addr, dx, dy, ctl) do {                               \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init0), (dy << 9) | dx << 2);       \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init1), (addr & 0xfffffff0) | ctl); \
} while(0)

//
// Cache flush macro
//
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

//
// Debug support macro
//
#if SIS910_MSPRO_DEBUG > 0
# define DEBUG1(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG1(format, ...)    CYG_EMPTY_STATEMENT
#endif
#if SIS910_MSPRO_DEBUG > 1
# define DEBUG2(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG2(format, ...)    CYG_EMPTY_STATEMENT
#endif

#define FUNC_DBG_PTR     DEBUG1("[INFO] %s()\n", __FUNCTION__)
#define CURRENT_POS_PTR  diag_printf("%s:%d\n", __FUNCTION__, __LINE__)

//
// Select the card R/W by whom
//
#ifdef SIS910_READ_MSPRO_THRU_CPU
#undef SIS910_READ_MSPRO_THRU_GATEWAY
#else
#define SIS910_READ_MSPRO_THRU_GATEWAY
#endif

#ifdef SIS910_WRITE_MSPRO_THRU_CPU
#undef SIS910_WRITE_MSPRO_THRU_GATEWAY
#else
#define SIS910_WRITE_MSPRO_THRU_GATEWAY
#endif

//
// Function prototype
//
static Cyg_ErrNo mspro_cpu_read_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks);
#ifdef SIS910_READ_MSPRO_THRU_CPU
static Cyg_ErrNo mspro_read_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block);
#endif
#ifdef SIS910_WRITE_MSPRO_THRU_CPU
static Cyg_ErrNo mspro_cpu_write_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks);
static Cyg_ErrNo mspro_write_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block);
#endif
#ifdef SIS910_READ_MSPRO_THRU_GATEWAY
static Cyg_ErrNo mspro_gateway_read_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks);
static Cyg_ErrNo mspro_gateway_read_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block);
#endif
#ifdef SIS910_WRITE_MSPRO_THRU_GATEWAY
static Cyg_ErrNo mspro_gateway_write_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks);
static Cyg_ErrNo mspro_gateway_write_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block);
#endif
void mspro_gateway_init(void);
void mspro_hw_init(void);
static Cyg_ErrNo mspro_send_init(cyg_mspro_disk_info_t *disk);
static Cyg_ErrNo mspro_read_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len);
static Cyg_ErrNo mspro_write_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len);
static Cyg_ErrNo mspro_set_rw_reg_addr(cyg_uint8 READ_REG_ADRS, cyg_uint8 READ_REG_SIZE, cyg_uint8 WRITE_REG_ADRS, cyg_uint8 WRITE_REG_SIZE);
static Cyg_ErrNo mspro_get_int(cyg_uint8 *reg_int);
static Cyg_ErrNo mspro_set_cmd(const cyg_uint8 cmd);
static Cyg_ErrNo mspro_ex_set_cmd(const cyg_uint8 cmd, cyg_uint8 *param);
static Cyg_ErrNo mspro_read_attribute(cyg_mspro_disk_info_t *disk, cyg_uint8 *attrib_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_parse_attribute(cyg_mspro_disk_info_t *disk, cyg_uint8 *attrib_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_media_type_identify(cyg_mspro_disk_info_t *disk);
static Cyg_ErrNo mspro_confirm_cpu_startup(cyg_mspro_disk_info_t *disk);
static Cyg_ErrNo mspro_check_for_disk(cyg_mspro_disk_info_t *disk);

// XXX
extern void *malloc_align(size_t /* size */, size_t /* alignment */);

void mspro_gateway_init(void)
{
    FUNC_DBG_PTR;
    // Config GATEWAY FIFO base and size
    // Enable Gateway FIFO: 256-byte per channel
    HAL_WRITE_UINT32(gw_fifo_reset, 0xffffffff);
    HAL_WRITE_UINT32(gw_fifo_reset, 0x0);
    HAL_WRITE_UINT32(gw_wff_val0, 0x03000308);
    HAL_WRITE_UINT32(gw_wff_val1, 0x03100318);
    HAL_WRITE_UINT32(gw_rff_val0, 0x08000808);
    HAL_WRITE_UINT32(gw_rff_val1, 0x08100818);

    //  Config GATEWAY DMA channel base address
    HAL_WRITE_UINT32(gw_lpc0, 0);
    HAL_WRITE_UINT32(gw_lpc1, 0);
    HAL_WRITE_UINT32(gw_lpc2, 0);
    HAL_WRITE_UINT32(gw_lpc3, 0);
    HAL_WRITE_UINT32(gw_lpc4, 0);
    HAL_WRITE_UINT32(gw_lpc5, 0);
    HAL_WRITE_UINT32(gw_lpc6, 0);
    HAL_WRITE_UINT32(gw_lpc7, 0);

    //  RFF_CH3 is shared by RFF_CH3_0 and RFF_CH3_1,
    //  used register "gw_rff3_dir" to switch.
    //  0: switch to RFF_CH3_0
    //  1: switch to RFF_CH3_1
    HAL_WRITE_UINT32(gw_rff3_dir, 0x1);

    HAL_WRITE_UINT32(dev_rarb_ctrl, 0x23450);
    HAL_WRITE_UINT32(dev_warb_ctrl0, 0x76543210);
    HAL_WRITE_UINT32(dev_warb_ctrl1, 0x76321098);
    HAL_WRITE_UINT32(dev_dir_ctrl, (0x01<<14));
    HAL_WRITE_UINT32(dev_fifo_en, 0xFFFF8000);  // enable WFF[15] & RFF
    HAL_WRITE_UINT32(dev_peri_irqen, 0x1<<12);  // enable ms_irq
}

void mspro_hw_init(void)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0x0;

    // SD & MS Shared PAD Switch to MS, default is SD
    HAL_READ_UINT32(0x90004000, val);
    val |= 0x200;
    HAL_WRITE_UINT32(0x90004000, val);

    val |= CTRL_BYTE_SWAP | CTRL_RISC_MODE | CTRL_CLK_DIVIDER_IS_8
         | CTRL_MS_ENABLE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
    DEBUG2(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

    val &= 0;
    val |= SYS_SERIAL_ACCESS_MODE | SYS_FIFO_CLEAR_ENABLE | SYS_FIFO_WRITE_MODE
         | SYS_MS_INTERRUPT_ENABLE | SYS_RISE_EDGE_INPUT_ENABLE
         | SYS_RISE_EDGE_OUTPUT_DISABLE | SYS_DEFAULT_BUSY_COUNT;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);
}

static Cyg_ErrNo
mspro_send_init(cyg_mspro_disk_info_t *disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;

    // Initialize Gateway
    mspro_gateway_init();
    // Initialize H/W
    mspro_hw_init();

    return err;
}

static Cyg_ErrNo
mspro_cpu_read_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint32 i = 0, j = 0;
    cyg_uint8 int_val = 0;
    cyg_uint32 n = 0;

    for (n = 0; n < blocks; n++)
    {
        // Set TPC to MSPRO_TPC_READ_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD,
           (CMD_SET_TPC(MSPRO_TPC_READ_LONG_DATA) | CMD_DATA_SELECT_CPU | 512));

        // Read data back
        // Read REG_SIS910_MSTICK_DATA 4 times can get 8 bytes data back
        // So loop count is 512 / 8 = 64
        for (i = 0; i < 64; i++)
        {
            // Wait until FIFO_EMPTY signal clear
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
            } while (val & STATUS_FIFO_EMPTY);

            for (j = 0; j < 4; j++)
            {
                HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
                DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
                // Get high byte first, when data came back.
                *data_buf++ = (val >> 8);
                *data_buf++ = val & 0xFF;
            }
        }
        // Wait until MS INTERRUPT signal
        do {
            HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
            if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
            {
                return -EIO;
            }
        } while (!(val & STATUS_MS_INTERRUPT));
        // Get INT register value
        mspro_get_int(&int_val);
        if (int_val == MS_INT_CMD_NORMAL_END)
        {
            DEBUG1("MS_INT_CMD_NORMAL_END\n");
        }
        else if (int_val == MS_INT_DATA_REQUEST)
        {
            DEBUG1("MS_INT_DATA_REQUEST\n");
        }
        else
        {
            return -EIO;
        }
    }

    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    return ENOERR;
}

#ifdef SIS910_WRITE_MSPRO_THRU_CPU
static Cyg_ErrNo
mspro_cpu_write_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint32 i = 0, j = 0;
    cyg_uint8 int_val = 0;
    cyg_uint32 n = 0;

    for (n = 0; n < blocks; n++)
    {
        // Set TPC to MSPRO_TPC_WRITE_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD,
          (CMD_SET_TPC(MSPRO_TPC_WRITE_LONG_DATA) | CMD_DATA_SELECT_CPU | 512));
        // Write data back
        // Loop count is 512 / 8 = 64
        for (i = 0; i < 64; i++)
        {
            // Wait until FIFO_FULL signal clear
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
            } while (val & STATUS_FIFO_FULL);

            for (j = 0; j < 4; j++)
            {
                val = 0;
                val |= (*data_buf++ << 8);
                val |= *data_buf++;
                DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
                HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, val);
            }
        }

        // Wait until MS INTERRUPT signal
        do {
            HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
            DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
            if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
            {
                return -EIO;
            }
        } while (!(val & STATUS_MS_INTERRUPT));
        // Get INT register value
        mspro_get_int(&int_val);
        if (int_val == MS_INT_CMD_NORMAL_END)
        {
            DEBUG1("MS_INT_CMD_NORMAL_END\n");
        }
        else if (int_val == MS_INT_DATA_REQUEST)
        {
            DEBUG1("MS_INT_DATA_REQUEST\n");
        }
        else
        {
            return -EIO;
        }
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    return ENOERR;
}
#endif

#ifdef SIS910_READ_MSPRO_THRU_GATEWAY
static Cyg_ErrNo
mspro_gateway_read_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint32 n = 0;

    // Check the buffer address is 32 bytes alignment or not
    if ((cyg_uint32)data_buf & 31)
    {
        DEBUG1("The buffer address is not 32 bytes alignment.\n");
        return -EINVAL;
    }

    for (n = 0; n < blocks; n++)
    {
        // Reset REG_SIS910_MSTICK_SYS and REG_SIS910_MSTICK_CTL settings for gateway access
        HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
        val &= ~(SYS_INTERRUPT_DISABLE | SYS_INTERRUPT_CLEAR_DISABLE
             | SYS_FIFO_CLEAR_DISABLE | SYS_FIFO_WRITE_MODE
             | SYS_DMA_DUAL_ACCESS_MODE);
        val |= (SYS_INTERRUPT_ENABLE | SYS_INTERRUPT_CLEAR_ENABLE
             | SYS_FIFO_CLEAR_ENABLE | SYS_FIFO_READ_MODE
             | SYS_DMA_SINGLE_ACCESS_MODE);
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
        DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

        HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
        val &= ~(CTRL_RISC_MODE | CTRL_BYTE_SWAP);
        val |= (CTRL_DMA_MODE | CTRL_NO_BYTE_SWAP);
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
        DEBUG2(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

        // Reset gateway
        HAL_WRITE_UINT32(gw_fifo_reset, 0x02);  // Reset WFF_CH1
        HAL_WRITE_UINT32(gw_fifo_reset, 0x0);

        // Set TPC to MSPRO_TPC_READ_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_READ_LONG_DATA) | 512));

        // Flush cache
        r16_cache_flush(data_buf, 512);

        // Read data back
        DEBUG1("Prepare to read data through gateway.\n");
        gw_xfer(wt1, (cyg_uint32)data_buf, 16, 1, 0x2);

        data_buf += 512;

        // XXX
        do{
            HAL_READ_UINT32(gw_irq, val);
            DEBUG2(" gw_irq = 0x%04X\n", val);
        } while (!val);

        // Clear gw_irq
        HAL_READ_UINT32(gw_irq, val);
        HAL_WRITE_UINT32(gw_irq, val);

#if SIS910_MSPRO_DEBUG > 1
        DEBUG2("Buffer address = 0x%08X\n", data_buf);
        diag_dump_buf_with_offset(data_buf, 512, data_buf);
#endif
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    // Wait dev_peri_irqst.ms_irq
    do {
        HAL_READ_UINT32(dev_peri_irqst, val);
    } while (!(val & 0x1000));

    // Clear IRQ
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    val |= SYS_INTERRUPT_CLEAR_ENABLE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    // Restore REG_SIS910_MSTICK_SYS and REG_SIS910_MSTICK_CTL settings
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    val &= ~(SYS_INTERRUPT_ENABLE | SYS_DMA_SINGLE_ACCESS_MODE);
    val |= (SYS_INTERRUPT_DISABLE | SYS_DMA_DUAL_ACCESS_MODE);
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

    HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
    val &= ~(CTRL_DMA_MODE | CTRL_NO_BYTE_SWAP);
    val |= (CTRL_RISC_MODE | CTRL_BYTE_SWAP);
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
    DEBUG2(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

    return ENOERR;
}
#endif

#ifdef SIS910_WRITE_MSPRO_THRU_GATEWAY
static Cyg_ErrNo
mspro_gateway_write_long_data(cyg_uint8 *data_buf, cyg_uint32 blocks)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint32 n = 0;
    cyg_uint8 reg_val = 0;

    // Check the buffer address is 32 bytes alignment or not
    if ((cyg_uint32)data_buf & 31)
    {
        DEBUG1("The buffer address is not 32 bytes alignment.\n");
        return -EINVAL;
    }

    for (n = 0; n < blocks; n++)
    {
        // Reset REG_SIS910_MSTICK_SYS and REG_SIS910_MSTICK_CTL settings for gateway access
        HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
        val &= ~(SYS_INTERRUPT_DISABLE | SYS_INTERRUPT_CLEAR_DISABLE
             | SYS_FIFO_CLEAR_DISABLE | SYS_FIFO_READ_MODE
             | SYS_DMA_DUAL_ACCESS_MODE);
        val |= (SYS_INTERRUPT_ENABLE | SYS_INTERRUPT_CLEAR_ENABLE
             | SYS_FIFO_CLEAR_ENABLE | SYS_FIFO_WRITE_MODE
             | SYS_DMA_SINGLE_ACCESS_MODE);
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
        DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

        HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
        val &= ~(CTRL_RISC_MODE | CTRL_BYTE_SWAP);
        val |= (CTRL_DMA_MODE | CTRL_NO_BYTE_SWAP);
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
        DEBUG2(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

        // Reset gateway
        HAL_WRITE_UINT32(gw_fifo_reset, 0x80);  // Reset WFF_CH3
        HAL_WRITE_UINT32(gw_fifo_reset, 0x0);

        // Set TPC to MSPRO_TPC_WRITE_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_WRITE_LONG_DATA) | 512));

        // Flush cache
        r16_cache_flush(data_buf, 512);

#if SIS910_MSPRO_DEBUG > 1
        DEBUG2("Cached buffer address = 0x%08X\n", data_buf);
        diag_dump_buf_with_offset(data_buf, 512, data_buf);
#endif
        gw_xfer(rd3, (cyg_uint32)data_buf, 16, 1, 0x2);
        data_buf += 512;

        // XXX
        do{
            HAL_READ_UINT32(gw_irq, val);
            DEBUG2(" gw_irq = 0x%04X\n", val);
        } while (!val);

        //clear gw_irq
        HAL_READ_UINT32(gw_irq, val);
        HAL_WRITE_UINT32(gw_irq, val);
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    // Wait dev_peri_irqst.ms_irq.
    do {
        HAL_READ_UINT32(dev_peri_irqst, val);
    } while (!(val & 0x1000));

    // Clear IRQ
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    val |= SYS_INTERRUPT_CLEAR_ENABLE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    // Wait until MS_INTERRUPT signal
    DEBUG1("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_MS_INTERRUPT));

    // Restore REG_SIS910_MSTICK_SYS and REG_SIS910_MSTICK_CTL settings
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    val &= ~(SYS_INTERRUPT_ENABLE | SYS_DMA_SINGLE_ACCESS_MODE);
    val |= (SYS_INTERRUPT_DISABLE | SYS_DMA_DUAL_ACCESS_MODE);
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    DEBUG2(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

    HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
    val &= ~(CTRL_DMA_MODE | CTRL_NO_BYTE_SWAP);
    val |= (CTRL_RISC_MODE | CTRL_BYTE_SWAP);
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
    DEBUG2(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

    do {
        mspro_get_int(&reg_val);
    } while (!(reg_val & MS_INT_CMD_NORMAL_END));

    if (reg_val & MS_INT_HAS_ERROR)
    {
        return -EIO;
    }
    else
    {
        return ENOERR;
    }
}
#endif

static Cyg_ErrNo
mspro_read_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 i = 0;
    cyg_uint32 reg_tmp[4] = {0};
    cyg_uint8 reg[8] = {0};

    if (reg_buf == NULL)
    {
        return -EINVAL;
    }

    if (reg_len > 8)
    {
        return -EINVAL;
    }

    // Set TPC to READ_REG, and data bytes is reg_len bytes
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_READ_REG) | reg_len));
    DEBUG2(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_READ_REG) | reg_len));

    // Wait until data send into FIFO
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
    } while (val & STATUS_FIFO_EMPTY);

    // Read data to H/W register
    for (i = 0; i < 4; i++)
    {
        HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, reg_tmp[i]);
        DEBUG2("%s:%d REG_SIS910_MSTICK_DATA = 0x%04X\n", __FUNCTION__, __LINE__, reg_tmp[i]);
        reg[i] = (reg_tmp[i] >> 8);
        reg[i + 1] = (reg_tmp[i] & 0xFF);
    }

    // Copy data back to the buffer
    for (i = 0; i < reg_len; i++)
    {
        *reg_buf++ = reg[i];
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_write_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 i = 0, j = 0;
    cyg_uint16 reg_tmp[4] = {0};

    if (reg_buf == NULL)
    {
        return -EINVAL;
    }

    if (reg_len > 8)
    {
        return -EINVAL;
    }

    // Set TPC to WRITE_REG, and data bytes is reg_len bytes
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_WRITE_REG) | reg_len));
    DEBUG2(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_WRITE_REG) | reg_len));

    // Combine reg_buf into H/W acceptable form
    for (i = 0, j = 0; i < reg_len; i += 2, j++)
    {
        reg_tmp[j] = (reg_buf[i] << 8) |  reg_buf[i + 1];
        DEBUG2(" reg_tmp[%d] = 0x%04X\n", j, reg_tmp[j]);
    }

    // Write data to H/W register
    for (i = 0; i < 4; i++)
    {
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, reg_tmp[i]);
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_set_rw_reg_addr(cyg_uint8 READ_REG_ADRS, cyg_uint8 READ_REG_SIZE, cyg_uint8 WRITE_REG_ADRS, cyg_uint8 WRITE_REG_SIZE)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;

    // Set TPC to SET_R/W_REG_ADRS, and data bytes is 4 bytes
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_SET_RW_REG_ADRS) | 0x4));
    DEBUG2(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_SET_RW_REG_ADRS) | 0x4));

    // Set READ_REG address and size
    val = (READ_REG_ADRS << 8) | READ_REG_SIZE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, val);
    DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    // Set WRITE_REG address and size
    val = (WRITE_REG_ADRS << 8) | WRITE_REG_SIZE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, val);
    DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    // Don't care dummy write
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_get_int(cyg_uint8 *reg_int)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;

    // Set TPC to MSPRO_TPC_GET_INT, and data bytes is one byte
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_GET_INT) | 0x1));
    DEBUG2(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_GET_INT) | 0x1));

    // Wait until data send into FIFO
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
    } while (val & STATUS_FIFO_EMPTY);

    // Read data back
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    // Get the high byte
    *reg_int = (val >> 8);
    // Don't care dummy read
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_set_cmd(const cyg_uint8 cmd)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;

    switch(cmd)
    {
        case MSPRO_CMD_READ_DATA:
        case MSPRO_CMD_WRITE_DATA:
        case MSPRO_CMD_READ_INFO:
        case MSPRO_CMD_WRITE_INFO:
        case MSPRO_CMD_READ_ATRB:
        case MSPRO_CMD_STOP:
        case MSPRO_CMD_ERASE:
        case MSPRO_CMD_CHG_CLASS:
        case MSPRO_CMD_FORMAT:
        case MSPRO_CMD_SLEEP:
            // Set TPC to SET_CMD, and data bytes is 1 byte
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_SET_CMD) | 0x1));
            DEBUG2(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_SET_CMD) | 0x1));
            // Set CMD
            // Set to high byte
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, (cmd << 8));
            DEBUG2(" REG_SIS910_MSTICK_DATA = 0x%04X\n", (cmd << 8));
            // Don't care dummy write
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
            // Wait until READY signal
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
                if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
                {
                    return -EIO;
                }
            } while (!(val & STATUS_READY));
            return ENOERR;
        default:
            return -EINVAL;
    }
}

static Cyg_ErrNo
mspro_ex_set_cmd(const cyg_uint8 cmd, cyg_uint8 *param)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 parameter[6];
    cyg_uint8 i = 0;

    switch(cmd)
    {
        case MSPRO_CMD_READ_DATA:
        case MSPRO_CMD_WRITE_DATA:
        case MSPRO_CMD_READ_INFO:
        case MSPRO_CMD_WRITE_INFO:
        case MSPRO_CMD_READ_ATRB:
        case MSPRO_CMD_STOP:
        case MSPRO_CMD_ERASE:
        case MSPRO_CMD_CHG_CLASS:
        case MSPRO_CMD_FORMAT:
        case MSPRO_CMD_SLEEP:
            // Set TPC to EX_SET_CMD, and data bytes is 7 byte
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_EX_SET_CMD) | 0x7));
            DEBUG2(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_EX_SET_CMD) | 0x7));
            // Set CMD and parameter
            for (i = 0; i < 6; i++)
            {
                parameter[i] = *param++;
            }
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, (cmd << 8) | parameter[0]);
            // Don't care dummy write
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, (parameter[1] << 8) | parameter[2]);
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, (parameter[3] << 8) | parameter[4]);
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, parameter[5] << 8);
            // Wait until READY signal
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
                if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
                {
                    return -EIO;
                }
            } while (!(val & STATUS_READY));
            return ENOERR;
        default:
            return -EINVAL;
    }
}

#ifdef SIS910_READ_MSPRO_THRU_CPU
static Cyg_ErrNo
mspro_read_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    Cyg_ErrNo ret = ENOERR;

    DEBUG2("[READ] first_block = %d, blocks = %d\n", first_block, blocks);

    // TODO: blocks is large than 0xFFFF

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Get blocks
    reg_buf[0] = blocks >> 8;       // Data Count [15:8]
    reg_buf[1] = blocks;            // Data Count [7:0]
    // For Data Address
    reg_buf[2] = first_block >> 24; // Data Address [31:24]
    reg_buf[3] = first_block >> 16; // Data Address [23:16]
    reg_buf[4] = first_block >> 8;  // Data Address [15:8]
    reg_buf[5] = first_block;       // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set READ_DATA command
    mspro_set_cmd(MSPRO_CMD_READ_DATA);

    // Wait until MS_INTERRUPT signal when Serial I/F mode
    DEBUG1("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if (reg_val == MS_INT_DATA_REQUEST)
    {
        ret = mspro_cpu_read_long_data(buf, blocks);
        return ret;
    }

    return -EIO;
}
#endif

#ifdef SIS910_WRITE_MSPRO_THRU_CPU
static Cyg_ErrNo
mspro_write_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    Cyg_ErrNo ret = ENOERR;

    DEBUG2("[WRITE] first_block = %d, blocks = %d\n", first_block, blocks);

    // TODO: blocks is large than 0xFFFF

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Write blocks
    reg_buf[0] = blocks >> 8;       // Data Count [15:8]
    reg_buf[1] = blocks;            // Data Count [7:0]
    // For Data Address
    reg_buf[2] = first_block >> 24; // Data Address [31:24]
    reg_buf[3] = first_block >> 16; // Data Address [23:16]
    reg_buf[4] = first_block >> 8;  // Data Address [15:8]
    reg_buf[5] = first_block;       // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set WRITE_DATA command
    mspro_set_cmd(MSPRO_CMD_WRITE_DATA);

    // Wait until MS_INTERRUPT signal when Serial I/F mode
    DEBUG1("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
    {
        ret = mspro_cpu_write_long_data(buf, blocks);
        return ret;
    }

    return -EIO;
}
#endif

#ifdef SIS910_READ_MSPRO_THRU_GATEWAY
static Cyg_ErrNo
mspro_gateway_read_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    Cyg_ErrNo ret = ENOERR;

    DEBUG2("[READ] first_block = %d, blocks = %d\n", first_block, blocks);

    // TODO: blocks is large than 0xFFFF

    // Set registers to write
    ret = mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);
    if (ret != ENOERR)
    {
        return ret;
    }

    // Fills the data count and data address
    // For Data Count
    // Get blocks
    reg_buf[0] = blocks >> 8;       // Data Count [15:8]
    reg_buf[1] = blocks;            // Data Count [7:0]
    // For Data Address
    reg_buf[2] = first_block >> 24; // Data Address [31:24]
    reg_buf[3] = first_block >> 16; // Data Address [23:16]
    reg_buf[4] = first_block >> 8;  // Data Address [15:8]
    reg_buf[5] = first_block;       // Data Address [7:0]
    // It's not necessary for EX_SET_CMD
    //ret = mspro_write_reg(reg_buf, 0x6);
    //if (ret != ENOERR)
    //{
    //    return ret;
    //}

    // Set READ_DATA command
    //ret = mspro_set_cmd(MSPRO_CMD_READ_DATA);
    ret = mspro_ex_set_cmd(MSPRO_CMD_READ_DATA, (cyg_uint8 *)&reg_buf);
    if (ret != ENOERR)
    {
        return ret;
    }

    // Wait until MS_INTERRUPT signal when Serial I/F mode
    DEBUG1("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }        
     } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
    {
        ret = mspro_gateway_read_long_data(buf, blocks);
        return ret;
    }

    return -EIO;
}
#endif

#ifdef SIS910_WRITE_MSPRO_THRU_GATEWAY
static Cyg_ErrNo
mspro_gateway_write_disk_multiple_blocks(cyg_mspro_disk_info_t *disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    Cyg_ErrNo ret = ENOERR;

    DEBUG2("[WRITE] first_block = %d, blocks = %d\n", first_block, blocks);

    // TODO: blocks is large than 0xFFFF

    // Set registers to write
    ret = mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);
    if (ret != ENOERR)
    {
        return ret;
    }

    // Fills the data count and data address
    // For Data Count
    // Write blocks
    reg_buf[0] = (blocks >> 8) & 0xFF;          // Data Count [15:8]
    reg_buf[1] = blocks & 0xFF;                 // Data Count [7:0]
    // For Data Address
    reg_buf[2] = (first_block >> 24) & 0xFF;    // Data Address [31:24]
    reg_buf[3] = (first_block >> 16) & 0xFF;    // Data Address [23:16]
    reg_buf[4] = (first_block >> 8) & 0xFF;     // Data Address [15:8]
    reg_buf[5] = first_block & 0xFF;            // Data Address [7:0]

    // It's not necessary for EX_SET_CMD
    //ret = mspro_write_reg(reg_buf, 0x6);
    //if (ret != ENOERR)
    //{
    //    return ret;
    //}

    // Set WRITE_DATA command
    //ret = mspro_set_cmd(MSPRO_CMD_WRITE_DATA);
    ret = mspro_ex_set_cmd(MSPRO_CMD_WRITE_DATA, (cyg_uint8 *)&reg_buf);
    if (ret != ENOERR)
    {
        return ret;
    }

    // Wait until MS_INTERRUPT signal
    DEBUG1("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
        {
            return -EIO;
        }
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if (reg_val == MS_INT_DATA_REQUEST)
    {
        ret = mspro_gateway_write_long_data(buf, blocks);
        return ret;
    }

    return -EIO;
}
#endif

static Cyg_ErrNo
mspro_read_attribute(cyg_mspro_disk_info_t *disk, cyg_uint8 *attrib_buf, cyg_uint16 *buf_len)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint8 reg_val = 0;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 *buf = attrib_buf;
    cyg_uint32 blk_no = 1;
    cyg_uint8 i = 0;
    cyg_uint8 read_count = (*buf_len >> 9);

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Get read_count blocks (512 * read_count bytes)
    reg_buf[0] = 0x00; // Data Count [15:8]
    reg_buf[1] = read_count; // Data Count [7:0]
    // For Data Address
    reg_buf[2] = 0x00; // Data Address [31:24]
    reg_buf[3] = 0x00; // Data Address [23:16]
    reg_buf[4] = 0x00; // Data Address [15:8]
    reg_buf[5] = 0x00; // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set READ_ATRB command
    mspro_set_cmd(MSPRO_CMD_READ_ATRB);

    for (i = 0; i < read_count; i++)
    {
        // Wait until MS_INTERRUPT signal when Serial I/F mode
        DEBUG1("Wait until MS INTERRUPT signal.\n");
        do {
            HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_MSTICK_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
            if (val & (STATUS_TIMEOUT_ERROR | STATUS_CRC_ERROR))
            {
                return -EIO;
            }
        } while (!(val & STATUS_MS_INTERRUPT));

        DEBUG2("%s:%d\n", __FUNCTION__, __LINE__);
        mspro_get_int(&reg_val);
        if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
        {
            mspro_cpu_read_long_data(buf, blk_no);
            buf += 512;
        }
        else
        {
            DEBUG1("Read Attribute Error!!\n");
            return -EIO;
        }
    }
    return ENOERR;
}

static Cyg_ErrNo
mspro_parse_attribute(cyg_mspro_disk_info_t *disk, cyg_uint8 *attrib_buf, cyg_uint16 *buf_len)
{
    FUNC_DBG_PTR;
    cyg_uint8 entry_count = 0;
    cyg_uint16 i = 0;
    cyg_uint16 entry_base = 0;
    cyg_uint32 block_size = 0;
    cyg_uint32 total_blocks = 0;
    cyg_uint32 user_area_blocks = 0;
    cyg_uint32 page_size = 0;
    cyg_uint32 uint_size = 0;

    // Check Signature Code
    if ((attrib_buf[0] == 0xA5) && (attrib_buf[1] == 0xC3))
    {
        // Get Device Entry Count
        if ((attrib_buf[4] >= 0x1) && (attrib_buf[4] <= 0xC))
        {
            entry_count = attrib_buf[4];
            for (i = 0; i < entry_count; i++)
            {
                entry_base = 16 + (i * 12);
                if (attrib_buf[entry_base + 8] == 0x10)
                {
                    // It matchs System Information ID
                    cyg_uint32 address, size;

                    address = (attrib_buf[entry_base] << 24)
                            | (attrib_buf[entry_base + 1] << 16)
                            | (attrib_buf[entry_base + 2] << 8)
                            | (attrib_buf[entry_base + 3]);
                    size = (attrib_buf[entry_base + 4] << 24)
                         | (attrib_buf[entry_base + 5] << 16)
                         | (attrib_buf[entry_base + 6] << 8)
                         | (attrib_buf[entry_base + 7]);
                    //DEBUG2(" address = 0x%04X\n", address);
                    //DEBUG2(" size = 0x%04X\n", size);
                    //DEBUG2(" address + size = 0x%04X\n", address + size);
                    if ((size == 0x60) && (address >= 0x1A0)
                       && ((address + size) <= 0x8000))
                    {
                        // Confirm System Information
                        //DEBUG2(" attrib_buf[%d] = 0x%02X\n", (address + 1), attrib_buf[address + 1]);
                        DEBUG2(" attrib_buf[%d] = 0x%02X\n", address, attrib_buf[address]);
                        if (attrib_buf[address] == 0x2)
                        {

                            switch (attrib_buf[address + 56])
                            {
                                case 0x00:
                                    DEBUG1("Memory Stick PRO [Read/Write]\n");
                                    break;
                                case 0x01:
                                case 0x02:
                                case 0x03:
                                    DEBUG1("Memory Stick PRO [Read Only]\n");
                                    break;
                                default:
                                    DEBUG1("Failure of Obtaining Attribute Information Media Error. [Media breakdown]\n");
                                    return -EIO;
                            }
                            if (attrib_buf[address + 46] & 0xC0)
                            {
                                DEBUG1("Shifts to read/write Protected Status.\n");
                            }

                            // Get others information
                            block_size       = (attrib_buf[address + 2] << 8) | attrib_buf[address + 3];
                            total_blocks     = (attrib_buf[address + 4] << 8) | attrib_buf[address + 5];
                            user_area_blocks = (attrib_buf[address + 6] << 8) | attrib_buf[address + 7];
                            page_size        = (attrib_buf[address + 8] << 8) | attrib_buf[address + 9];
                            uint_size        = (attrib_buf[address + 44] << 8) | attrib_buf[address + 45];

                            DEBUG2("Block size = 0x%04X\n", block_size);
                            DEBUG2("Total blocks = 0x%04X\n", total_blocks);
                            DEBUG2("User area blocks = 0x%04X\n", user_area_blocks);
                            DEBUG2("Page size = 0x%04X\n", page_size);
                            DEBUG2("Unit size = 0x%04X\n", uint_size);
                            //DEBUG2("Interface type = 0x%02X\n", attrib_buf[address + 50]);
                            DEBUG2("Interface type = 0x%02X\n", attrib_buf[address + 51]);

                            disk->mspro_heads_per_cylinder = user_area_blocks;
                            disk->mspro_sectors_per_head = block_size;
                            disk->mspro_read_block_length = uint_size;
                            disk->mspro_write_block_length = uint_size;
                            disk->mspro_block_count = user_area_blocks * block_size * uint_size;

                            return ENOERR;
                        }
                    }
                }
            }
        }
    }

    DEBUG2("Failure of Obtaining Attribute Information Media Error. [Media breakdown]\n");
    return -EIO;
}

static cyg_bool
mspro_disk_changed(cyg_mspro_disk_info_t *disk)
{
    FUNC_DBG_PTR;

    //
    // TODO
    //

    return false;
}

static Cyg_ErrNo
mspro_switch_interface_mode(cyg_uint8 mode)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint8 reg_buf = 0x0;
    cyg_uint32 val = 0x0;

    if (mode == MS_PARALLEL_MODE)
    {
        DEBUG1("Switch to parallel interface mode.\n");
        // Set system parameter to write
        err = mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_SYSTEM_PARAMETER_REG, 0x1);
        if (err != ENOERR)
        {
            return err;
        }

        // Fills the system parameter
        reg_buf = MS_PARAM_PARALLEL_IF;
        err = mspro_write_reg(&reg_buf, 0x1);
        if (err != ENOERR)
        {
            return err;
        }

        // Change H/W transportation mode setting
        HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
        val &= ~(SYS_SERIAL_ACCESS_MODE | SYS_DMA_SINGLE_ACCESS_MODE);
        val |= (SYS_PARALLEL_ACCESS_MODE | SYS_DMA_DUAL_ACCESS_MODE);
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    }

    return ENOERR;
}

static Cyg_ErrNo
mspro_media_type_identify(cyg_mspro_disk_info_t *disk)
{
    FUNC_DBG_PTR;
    cyg_uint8 reg_buf[4] = {0};
    cyg_uint8 reg_len = 0x4;

    // Set continuous four registers to read (0x04, 0x06, 0x07)
    mspro_set_rw_reg_addr(MSPRO_TYPE_REG, reg_len, 0x0, 0x0);
    // Read registers back
    mspro_read_reg(reg_buf, reg_len);

    // Parse Type Register (0x04) and Category Register (0x06)
    if ((reg_buf[0] == MS_TYPE_HAS_PARALLEL_MODE)
     && (reg_buf[2] == MS_CATEGORY_MEMORY))
    {
        DEBUG1(" Media Type = 0x%02X\n", reg_buf[0]);
        DEBUG1(" Media Category = 0x%02X\n", reg_buf[2]);
        disk->mspro_card_type = MSPRO;
    }
    else
    {
        DEBUG1(" Media Type = 0x%02X\n", reg_buf[0]);
        DEBUG1(" Media Category = 0x%02X\n", reg_buf[2]);
        return -EIO;
    }

    // Parse Class Register (0x07)
    if (reg_buf[3] == MS_CLASS_RAM)
    {
        disk->mspro_read_only = false;
        DEBUG1(" Media Class = 0x%02X\n", reg_buf[3]);
        return ENOERR;
    }
    else if ((reg_buf[3] == MS_CLASS_ROM) || (reg_buf[3] == MS_CLASS_R) || (reg_buf[3] == MS_CLASS_OTHER_ROM))
    {
        disk->mspro_read_only = true;
        DEBUG1(" Media Class = 0x%02X\n", reg_buf[3]);
    }
    else
    {
        return -EIO;
    }

    return ENOERR;
}

static Cyg_ErrNo
mspro_confirm_cpu_startup(cyg_mspro_disk_info_t *disk)
{
    FUNC_DBG_PTR;
    cyg_uint8 int_val = 0;
    cyg_uint32 i = 0;
    Cyg_ErrNo ret = ENOERR;

    // Get INT register value
    for (i = 0; i < 10000; i++)
    {
        ret = mspro_get_int(&int_val);
        if (ret != ENOERR)
        {
            return ret;
        }

        if (int_val & MS_INT_CMD_NORMAL_END)
        {
            if (int_val & MS_INT_HAS_ERROR)
            {
                if (int_val & MS_INT_CMD_NON_EXECUTABLE)
                {
                    DEBUG1("shifted to Write-Disabled Status.\n");
                    // TODO: Set to read only state
                    return ENOERR;
                }
                else
                {
                    DEBUG1("Media Error!!\n");
                    return -EIO;
                }
            }
            else
            {
                DEBUG1("CPU Start up is completed.\n");
                return ENOERR;
            }
        }
    }
    // Fails to confirm CPU startup
    DEBUG1("Media Error!!\n");
    return -EIO;
}

static Cyg_ErrNo
mspro_check_for_disk(cyg_mspro_disk_info_t *disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo ret = ENOERR;
    cyg_uint8 attrib_buf[512] = {0};
    cyg_uint16 attrib_buf_len = 512;

    // Initialize H/W and Gateway
    mspro_send_init(disk);

    // Identify the media type
    DEBUG1("Prepare to identify media type.\n");
    ret = mspro_media_type_identify(disk);
    DEBUG1("Media type identified.\n");
    if (ret != ENOERR)
    {
        return ret;
    }

#ifdef SIS910_MSPRO_DEFAULT_PARALLEL_MODE
    // Change the transportation mode
    ret = mspro_switch_interface_mode(MS_PARALLEL_MODE);
    if (ret != ENOERR)
    {
        return ret;
    }
    else
    {
        disk->mspro_bus_width = 4;
    }
#else
        disk->mspro_bus_width = 1;
#endif

    // Confirm CPU startup
    ret = mspro_confirm_cpu_startup(disk);
    if (ret != ENOERR)
    {
        return ret;
    }

    // Read attribute
    ret = mspro_read_attribute(disk, attrib_buf, &attrib_buf_len);
    if (ret != ENOERR)
    {
        return ret;
    }
    else
    {
        ret = mspro_parse_attribute(disk, attrib_buf, &attrib_buf_len);
    }

    return ENOERR;
}

static Cyg_ErrNo
mspro_disk_read(disk_channel* chan, void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_mspro_disk_info_t *disk = (cyg_mspro_disk_info_t*)chan->dev_priv;

    if (!disk->mspro_connected)
    {
        DEBUG2(" Disk doesn't connected.\n");
        return -ENODEV;
    }

    if ((first_block + blocks) >= disk->mspro_block_count)
    {
        DEBUG2(" The block to read is out of bound.\n");
        return -EINVAL;
    }

#ifdef SIS910_READ_MSPRO_THRU_CPU
    return mspro_read_disk_multiple_blocks(disk, (cyg_uint8 *) buf_arg, blocks, first_block);
#else
    return mspro_gateway_read_disk_multiple_blocks(disk, (cyg_uint8 *) buf_arg, blocks, first_block);
#endif
}

static Cyg_ErrNo
mspro_disk_write(disk_channel* chan, const void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_mspro_disk_info_t *disk = (cyg_mspro_disk_info_t*)chan->dev_priv;

    if (!disk->mspro_connected)
    {
        DEBUG2(" Disk doesn't connected.\n");
        return -ENODEV;
    }

    if (disk->mspro_read_only)
    {
        DEBUG2(" The disk is read only.\n");
        return -EROFS;
    }

    if ((first_block + blocks) >= disk->mspro_block_count)
    {
        DEBUG2(" The block to read is out of bound.\n");
        return -EINVAL;
    }

#ifdef SIS910_WRITE_MSPRO_THRU_CPU
    return mspro_write_disk_multiple_blocks(disk, (cyg_uint8 *) buf_arg, blocks, first_block);
#else
    return mspro_gateway_write_disk_multiple_blocks(disk, (cyg_uint8 *) buf_arg, blocks, first_block);
#endif
}

static Cyg_ErrNo
mspro_disk_get_config(disk_channel *priv, cyg_uint32 key, const void *xbuf, cyg_uint32 *len)
{
    FUNC_DBG_PTR;

    return -EINVAL;
}

static Cyg_ErrNo
mspro_disk_set_config(disk_channel *priv, cyg_uint32 key, const void *xbuf, cyg_uint32 *len)
{
    FUNC_DBG_PTR;

    return -EINVAL;
}

static cyg_bool
mspro_disk_init(struct cyg_devtab_entry *tab)
{
    FUNC_DBG_PTR;
    disk_channel *chan = (disk_channel*)tab->priv;

    return (*chan->callbacks->disk_init)(tab);
}

static Cyg_ErrNo
mspro_disk_lookup(struct cyg_devtab_entry **tab, struct cyg_devtab_entry *sub_tab, const char *name)
{
    FUNC_DBG_PTR;
    disk_channel *chan = (disk_channel*)(*tab)->priv;
    cyg_mspro_disk_info_t *disk = (cyg_mspro_disk_info_t*)chan->dev_priv;
    Cyg_ErrNo ret = ENOERR;
    cyg_disk_identify_t *ident = NULL;

    if (disk->mspro_connected)
    {
        DEBUG2(" Card still plugged.\n");
        if(mspro_disk_changed(disk))
        {
            DEBUG2(" The old card is gone.\n");
            disk->mspro_connected = false;
            (*chan->callbacks->disk_disconnected)(chan);
            if (0 != chan->info->mounts)
            {
                return -ENODEV;
            }
        }
    }

    if ((0 != chan->info->mounts)&&(0 == disk->mspro_connected))
    {
        // There are still mount points to an old card. We cannot accept
        // new mount requests until those have been cleaned out.
        DEBUG2(" The mount points need to clean out.\n");
        return -ENODEV;
    }

    if (!disk->mspro_connected)
    {
        DEBUG1(" Plug a new card.\n");

        ret = mspro_check_for_disk(disk);
        if (ret != ENOERR)
        {
            DEBUG1(" Disk check fails.\n");
            return ret;
        }
        // A card has been found. Tell the higher-level code about it.
        // This requires an identify structure, although it is not
        // entirely clear what purpose that serves.
        disk->mspro_connected = true;

        ident = (cyg_disk_identify_t *)malloc(sizeof(cyg_disk_identify_t));
        if (ident)
        {
            memset(ident, 0, sizeof(cyg_disk_identify_t));
            ident->cylinders_num   = 1;
            ident->heads_num       = disk->mspro_heads_per_cylinder;
            ident->sectors_num     = disk->mspro_sectors_per_head;
            ident->lba_sectors_num = disk->mspro_block_count;
            ident->phys_block_size = disk->mspro_write_block_length/512;
            ident->max_transfer    = disk->mspro_write_block_length;

           (*chan->callbacks->disk_connected)(*tab, ident);
           free(ident);
           ident = NULL;
        }
        else
        {
            return -ENOMEM;
        }
    }

    return (*chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

#ifdef ALIGN_USE_FUNC_PTR
void*
mspro_disk_buf_alloc(cyg_uint32 sizes)
{
    FUNC_DBG_PTR;

    return malloc_align(sizes, 32); // 32 bytes alignment
}
#endif

DISK_FUNS(cyg_mspro_disk_funs,
    mspro_disk_read,
    mspro_disk_write,
    mspro_disk_get_config,
    mspro_disk_set_config
    );

static cyg_mspro_bus cyg_mspro_bus0;
static cyg_mspro_device cyg_msbus_dev0 = {
    .mspro_bus = &cyg_mspro_bus0,
};

static cyg_mspro_disk_info_t cyg_mspro_disk_hwinfo = {
    .mspro_dev = &cyg_msbus_dev0,
    .mspro_connected = false,
    .mspro_bus_width = 1,
};

DISK_CONTROLLER(cyg_mspro_disk_controller, cyg_mspro_disk_hwinfo);

DISK_CHANNEL(cyg_mspro_disk_channel,
         cyg_mspro_disk_funs,
         cyg_mspro_disk_hwinfo,
         cyg_mspro_disk_controller,
         true,
         1);

#ifdef ALIGN_USE_FUNC_PTR
BLOCK_DEVTAB_ENTRY(cyg_mspro_disk0_devtab_entry,
         CYGDAT_IO_DISK_MEMORYSTICK_DISK0_NAME,
         0,
         &cyg_io_disk_devio,
         &mspro_disk_init,
         &mspro_disk_lookup,
         &cyg_mspro_disk_channel,
         &mspro_disk_buf_alloc
         );
#else
BLOCK_DEVTAB_ENTRY(cyg_mspro_disk0_devtab_entry,
         CYGDAT_IO_DISK_MEMORYSTICK_DISK0_NAME,
         0,
         &cyg_io_disk_devio,
         &mspro_disk_init,
         &mspro_disk_lookup,
         &cyg_mspro_disk_channel
         );
#endif
