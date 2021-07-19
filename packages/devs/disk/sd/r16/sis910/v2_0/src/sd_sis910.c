//==========================================================================
//
//      sd_sis910.c
//
//      Provide a disk device driver for MMC/SD cards
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
// Author:       bartv
// Date:         2004-04-25
// Author:       Sidney Shih
// Date:         2008-09-11
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_disk_sd_sis910.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_if.h>             // delays
#include <cyg/hal/hal_intr.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>

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
#if SIS910_SD_DEBUG > 0
# define DEBUG1(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG1(format, ...)
#endif
#if SIS910_SD_DEBUG > 1
# define DEBUG2(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG2(format, ...)
#endif

#define FUNC_DBG_PTR     DEBUG1("[INFO] %s()\n", __FUNCTION__)
#define CURRENT_POS_PTR  diag_printf("%s:%d\n", __FUNCTION__, __LINE__)

//
// Select the card R/W by whom
//
#ifdef SIS910_READ_SD_THRU_CPU
#undef SIS910_READ_SD_THRU_GATEWAY
#else
#define SIS910_READ_SD_THRU_GATEWAY
#endif

#ifdef SIS910_WRITE_SD_THRU_CPU
#undef SIS910_WRITE_SD_THRU_GATEWAY
#else
#define SIS910_WRITE_SD_THRU_GATEWAY
#endif

//
// Function prototype
//
static Cyg_ErrNo sd_read_disk_block(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 block);
#ifdef SIS910_READ_SD_THRU_CPU
//static Cyg_ErrNo sd_read_disk_multiple_blocks(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block);
#endif
#ifdef SIS910_READ_SD_THRU_GATEWAY
static Cyg_ErrNo sd_gateway_read_disk_block(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 block);
//static Cyg_ErrNo sd_gateway_read_disk_multiple_blocks(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block);
#endif
#ifdef SIS910_WRITE_SD_THRU_CPU
static Cyg_ErrNo sd_write_disk_block(cyg_mmc_disk_info_t* disk, const cyg_uint8* buf, cyg_uint32 block);
#endif
#ifdef SIS910_WRITE_SD_THRU_GATEWAY
static Cyg_ErrNo sd_gateway_write_disk_block(cyg_mmc_disk_info_t* disk, const cyg_uint8* buf, cyg_uint32 block);
#endif
void sd_gateway_init(void);
void sd_hw_init(cyg_mmc_disk_info_t* disk);
static Cyg_ErrNo mmc_set_RCA(cyg_uint32 *RCA);
static Cyg_ErrNo sd_get_RCA(cyg_uint32 *RCA);
static Cyg_ErrNo mmc_get_EXT_CSD(cyg_mmc_disk_info_t* disk, cyg_uint8 *ext_csd);
static Cyg_ErrNo sd_get_CSD(mmc_csd_register *CSD, cyg_uint32 *RCA);
static Cyg_ErrNo sd_get_CID(mmc_cid_register *CID);
static Cyg_ErrNo mmc_media_type_identify(cyg_mmc_disk_info_t* disk);
static Cyg_ErrNo sd_media_type_identify(cyg_mmc_disk_info_t* disk);
static Cyg_ErrNo mmc_parse_attribute(cyg_mmc_disk_info_t* disk);
static Cyg_ErrNo sd_parse_attribute(cyg_mmc_disk_info_t* disk);
static Cyg_ErrNo sd_send_command(cyg_uint16 cmd, cyg_uint16 argh, cyg_uint16 argl, cyg_uint16 cmd_dat_cont);
void sd_send_command_no_resp(cyg_uint16 cmd, cyg_uint16 argh, cyg_uint16 argl, cyg_uint16 cmd_dat_cont);
static Cyg_ErrNo sd_send_init(cyg_mmc_disk_info_t* disk);
static cyg_bool sd_disk_init(struct cyg_devtab_entry* tab);
static cyg_bool sd_disk_changed(cyg_mmc_disk_info_t* disk);
static Cyg_ErrNo sd_check_for_disk(cyg_mmc_disk_info_t* disk);
static char* sd_disk_lookup_itoa(cyg_uint32 num, char* where);
static Cyg_ErrNo sd_disk_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry *sub_tab, const char* name);
static Cyg_ErrNo sd_disk_read(disk_channel* chan, void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block);
static Cyg_ErrNo sd_disk_write(disk_channel* chan, const void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block);
static Cyg_ErrNo sd_disk_erase(disk_channel* chan, cyg_uint32 blocks, cyg_uint32 first_block);
static Cyg_ErrNo sd_disk_get_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static Cyg_ErrNo sd_disk_set_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len);

void sd_gateway_init(void)
{
    FUNC_DBG_PTR;

    // SD pad enable
    HAL_WRITE_UINT32(padreg1, 2);
    // ch 6 select sd card (4)
    HAL_WRITE_UINT32(dev_rarb_ctrl, 0x32410);
    // ideh ->gateway arb
    HAL_WRITE_UINT32(dev_dir_ctrl, DIR_SD2G);
    // modify for new peri and gateway write arbiter setting
    HAL_WRITE_UINT32(dev_warb_ctrl1, 0xf6ffff01);

    // Enable Gateway FIFO: 256-byte per channel
    HAL_WRITE_UINT32(gw_fifo_reset, 0xffffffff);
    HAL_WRITE_UINT32(gw_fifo_reset, 0x0);
    HAL_WRITE_UINT32(gw_wff_val0, 0x06000608);
    HAL_WRITE_UINT32(gw_wff_val1, 0x06100618);

    HAL_WRITE_UINT32(gw_rff_val0, 0x08000808);
    HAL_WRITE_UINT32(gw_rff_val1, 0x08100818);

    HAL_WRITE_UINT32(gw_lpc0, 0);
    HAL_WRITE_UINT32(gw_lpc1, 0);
    HAL_WRITE_UINT32(gw_lpc2, 0);
    HAL_WRITE_UINT32(gw_lpc3, 0);
    HAL_WRITE_UINT32(gw_lpc4, 0);
    HAL_WRITE_UINT32(gw_lpc5, 0);
    HAL_WRITE_UINT32(gw_lpc6, 0);
    HAL_WRITE_UINT32(gw_lpc7, 0);
}

void sd_hw_init(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    cyg_uint32 val = 0;
    cyg_uint32 divisor = 0x4;   // Identification mode freq < 400 MHz

    // Set SD card clock rate to meet identification mode max. frequency rate
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, divisor);
    disk->mmc_saved_baudrate = divisor;

    // SD & MS Shared PAD Switch to SD
    HAL_READ_UINT32(0x90004000, val);
    val &= ~0x200;
    HAL_WRITE_UINT32(0x90004000, val);
}

static Cyg_ErrNo
mmc_set_RCA(cyg_uint32 *RCA)
{
    FUNC_DBG_PTR;
//    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0;
    Cyg_ErrNo err = ENOERR;

    // Send CMD 3
    DEBUG2("CMD 3 sent.\n");
    err = sd_send_command(MMC_REQUEST_SET_RELATIVE_ADDR, *RCA, 0, CMD_DAT_CONT_R1_RESPONSE);
//    if (err == ENOERR)
//    {
//        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
//        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
//        *RCA = ((reply_1 & 0x00FF) << 8) | ((reply_2 & 0xFF00) >> 8);
//        DEBUG2(" RCA = %X\n", *RCA);
//    }
//    else
//    {
//        DEBUG2("Cannot get RCA.\n");
//        return -EIO;
//    }

    return err;
}

static Cyg_ErrNo
sd_get_RCA(cyg_uint32 *RCA)
{
    FUNC_DBG_PTR;
    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0;
    Cyg_ErrNo err = ENOERR;

    // Send CMD 3
    DEBUG2("CMD 3 sent.\n");
    err = sd_send_command(SD_REQUEST_SEND_RELATIVE_ADDR, 0, 0, CMD_DAT_CONT_R1_RESPONSE);
    if (err == ENOERR)
    {
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
        *RCA = ((reply_1 & 0x00FF) << 8) | ((reply_2 & 0xFF00) >> 8);
        DEBUG2(" RCA = %X\n", *RCA);
    }
    else
    {
        DEBUG2("Cannot get RCA.\n");
        return -EIO;
    }
    return ENOERR;
}

static Cyg_ErrNo
mmc_get_EXT_CSD(cyg_mmc_disk_info_t* disk, cyg_uint8 *ext_csd)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 divisor = disk->mmc_saved_baudrate;

    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    // Set read through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, (0x10 | divisor));

    // Send CMD 8
    err = sd_send_command(MMC_REQUEST_SEND_EXT_CSD, 0, 0,
                         (CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE));
    if (err == ENOERR)
    {
        cyg_uint32 i;
        cyg_uint16 val = 0x0;

        // Read data from SD_DATA_BUFFERS register
        for (i = 0; i < 256; i++)
        {
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
            } while (!(val & 0x40)); // Xport ready
            // Read data
            HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);

            *ext_csd++ = (val & 0xFF);
            *ext_csd++ = (val >> 8);
        }

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_DATA_TRANSFER_DONE));

        do {
            // End command response
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_END_CMD_RESPONSE));
    }

    return err;
}

static Cyg_ErrNo
sd_get_CSD(mmc_csd_register *CSD, cyg_uint32 *RCA)
{
    FUNC_DBG_PTR;
    cyg_uint16 argh = 0x0;
    cyg_uint16 reply = 0x0;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 i = 0;

    // Send CMD 9
    DEBUG2("CMD 9 sent.\n");
    argh = *RCA;
    err = sd_send_command(MMC_REQUEST_SEND_CSD, argh, 0, CMD_DAT_CONT_R2_RESPONSE);
    if (err == ENOERR)
    {
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply);
        CSD->csd_data[0] = (reply & 0xFF);
        for (i = 1; i < 15; i += 2)
        {
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply);
            CSD->csd_data[i] = ((reply & 0xFF00) >> 8);
            CSD->csd_data[i + 1] = (reply & 0xFF);
        }
    }
    else
    {
        DEBUG2("Cannot get CSD.\n");
        return -EIO;
    }
    return ENOERR;
}

static Cyg_ErrNo
sd_get_CID(mmc_cid_register *CID)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint16 reply = 0x0;
    cyg_uint32 i = 0;

    // Send CMD 2
    DEBUG2("CMD 2 sent.\n");
    err = sd_send_command(MMC_REQUEST_ALL_SEND_CID, 0, 0, CMD_DAT_CONT_R2_RESPONSE);
    if (err == ENOERR)
    {
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply);
        CID->cid_data[0] = (reply & 0xFF);
        for (i = 1; i < 15; i += 2)
        {
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply);
            CID->cid_data[i] = ((reply & 0xFF00) >> 8);
            CID->cid_data[i + 1] = (reply & 0xFF);
        }
    }
    else
    {
        DEBUG2("Cannot get CID.\n");
        return -EIO;
    }
    return ENOERR;
}

static Cyg_ErrNo
sd_get_SCR(cyg_mmc_disk_info_t* disk, cyg_uint32 *RCA)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 divisor = disk->mmc_saved_baudrate;

    // Send CMD55
    err = sd_send_command(MMC_REQUEST_APP_CMD, *RCA, 0, CMD_DAT_CONT_R1_RESPONSE);
    if (err != ENOERR)
    {
        return -EIO;
    }

    // Set data size to 8 bytes (64 bits)
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, 8);
    // Set read through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, (0x10 | divisor));

    // Send ACMD51 (CMD51 afetr CMD55)
    err = sd_send_command(SD_REQUEST_APP_SEND_SCR, 0, 0,
                         (CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE));
    if (err == ENOERR)
    {
        cyg_uint8 i;
        cyg_uint16 val = 0;
        cyg_uint8 SCR[8] = {0};

        for (i = 0; i < 4; i++)
        {
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
            } while (!(val & 0x40)); // Xport ready
            // Read data
            HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);

            SCR[i << 1] = (val & 0xFF);
            SCR[(i << 1) + 1] = (val >> 8);
        }

        switch (SCR[0] & 0xF)
        {
            case 0:
                DEBUG2("It's SD 1.00 Standard Capacity Card.\n");
                disk->mmc_card_type = CARD_TYPE_SD_1_00;
                break;
            case 1:
                DEBUG2("It's SD 1.10 Standard Capacity Card.\n");
                disk->mmc_card_type = CARD_TYPE_SD_1_10;
                break;
            case 2:
            default:
                break;
        }
        HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    }

    return err;
}

static Cyg_ErrNo
mmc_switch_speed_mode(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint16 argh = 0x0, argl = 0x0;

    // Send CMD 6
    // [31:26] Set to 0
    // [25:24] Access:Write Byte(0x03)
    // [23:16] Index: HS_TIMING(185)
    // [15:8]  Value: Timing_High(1)
    // [7:3]   Set to 0
    // [2:0]   Cmd set: 0
    argh = 0x03B9;
    argl = 0x8000;
    err = sd_send_command(MMC_REQUEST_SWITCH, argh, argl,
                         (CMD_DAT_CONT_BUSY_BIT_ON | CMD_DAT_CONT_R1_RESPONSE));

    return err;
}

static Cyg_ErrNo
mmc_switch_bus_width(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint16 argh = 0x0, argl = 0x0;

    // Send CMD 6
    // [31:26] Set to 0
    // [25:24] Access:Write Byte(0x03)
    // [23:16] Index: BUS_WIDTH(183)
    // [15:8]  Value: 4 bit data bus(1)
    // [7:3]   Set to 0
    // [2:0]   Cmd set: 0
    argh = 0x03B7;
    argl = 0x8000;
    err = sd_send_command(MMC_REQUEST_SWITCH, argh, argl,
                         (CMD_DAT_CONT_BUSY_BIT_ON | CMD_DAT_CONT_R1_RESPONSE));
    if (err == ENOERR)
    {
        DEBUG2("MMC Bus Width = 4\n");
        disk->mmc_bus_width = 4;
    }
    else
    {
        DEBUG2("MMC Bus Width = 1\n");
        disk->mmc_bus_width = 1;
    }

    return err;
}

static Cyg_ErrNo
sd_switch_speed_mode(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    cyg_uint16 argh = 0x0, argl = 0x0;
    cyg_uint8 i;
    cyg_uint16 val = 0;
    cyg_uint16 func_status[32] = {0};

    // Set data size to 64 bytes (512 bits)
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, 64);
    // Set read through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x10);

    // Check function (Access mode)
    // Mode 0 operation with access mode Default => High-Speed
    argh = 0x00FF;
    argl = 0xFF01;
    err = sd_send_command(MMC_REQUEST_SWITCH, argh, argl,
                         (CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE));
    if (err == ENOERR)
    {
        for (i = 0; i < 32; i++)
        {
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
            } while (!(val & 0x40)); // Xport ready
            // Read data
            HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);

            func_status[i] = (((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8));
        }
        if (func_status[0] == 0x0)
        {
            // Check function with error!
            DEBUG2("Check function with error!\n");
            //return -EPERM;
            err = -EPERM;
            goto END_SWITCH_FUNC;
        }
    }
    else
    {
        DEBUG2("No response to CMD6.\n");
        goto END_SWITCH_FUNC;
        // return err;
    }

    // Set function
    // If Function Group No. 1 High-Speed supported and no error
    if ((func_status[6] == 0x8003) &&               /* function supported */
       (((func_status[8] & 0x0F00) >> 8) == 0x1))   /* function with no error */
    {
        DEBUG2("High-Speed function supported.\n");
        DEBUG2("Set High-Speed function.\n");
        // Mode 1 operation with access mode Default => High-Speed
        argh = 0x80FF;
        argl = 0xFF01;
        err = sd_send_command(MMC_REQUEST_SWITCH, argh, argl,
                             (CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE));
        if (err == ENOERR)
        {
            for (i = 0; i < 32; i++)
            {
                do {
                    HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
                } while (!(val & 0x40)); // Xport ready
                // Read data
                HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);

                func_status[i] = (((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8));
            }
            if ((func_status[6] == 0x8003) &&               /* function supported */
               (((func_status[8] & 0x0F00) >> 8) == 0x1))   /* function with no error */
            {
                DEBUG2("High-Speed function switched.\n");
            }
            else
            {
                DEBUG2("High-Speed function cannot switch.\n");
                err = -EPERM;
            }
        }
    }

END_SWITCH_FUNC:
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    return err;
}

static Cyg_ErrNo
sd_switch_bus_width(cyg_mmc_disk_info_t* disk, cyg_uint32 *RCA)
{
    Cyg_ErrNo err = ENOERR;

    // Send ACMD 6
    err = sd_send_command(MMC_REQUEST_APP_CMD, *RCA, 0, CMD_DAT_CONT_R1_RESPONSE);
    if (err != ENOERR)
    {
        return -EIO;
    }

    err = sd_send_command(SD_REQUEST_APP_SET_BUS_WIDTH, 0, 0x10, CMD_DAT_CONT_R1_RESPONSE);
    if (err == ENOERR)
    {
        DEBUG2("SD Bus Width = 4\n");
        disk->mmc_bus_width = 4;
    }
    else
    {
        DEBUG2("SD Bus Width = 1\n");
        disk->mmc_bus_width = 1;
    }

    return err;
}

static Cyg_ErrNo
mmc_media_type_identify(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    cyg_uint16 argh = 0x0, argl = 0x0;
    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 OCR;

    // Soft reset the card
    // Go to idle state
    // Send CMD 0
    sd_send_command_no_resp(MMC_REQUEST_GO_IDLE_STATE, 0, 0, CMD_DAT_CONT_CARD_INIT);

    // Send CMD 1
    // Vdd: 2.7 ~ 3.6 and sector mode capability
    argh = 0x40FF;
    argl = 0x8000;
    do {
        err = sd_send_command(MMC_REQUEST_SEND_OP_COND, argh, argl, CMD_DAT_CONT_R3_RESPONSE);
        if (err != ENOERR)
        {
            DEBUG2("No response to CMD1.\n");
            return err;
        }
        // Parse OCR
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);

        OCR = ((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8);
    } while (!(OCR & 0x80000000)); // Repeat until busy_bit in OCR is cleared

    DEBUG2(" OCR = %X\n", OCR);

    if (OCR | 0xFF8000)
    {
        // The Vdd can work on this card
        disk->OCR = OCR;
        if (OCR & 0x40000000)
        {
            // Sector mode
            // This is MMC high capacity card (MMCA 4.2)
            disk->mmc_card_type = CARD_TYPE_MMC_HC;
            DEBUG2("It's MMC high capacity card (MMCA 4.2).\n");
        }
        else
        {
            // Byte mode
            // XXX: Assume it is a MMC 4.X card
            // We check it (SEPC_VERS) later to confirm the spec version
            disk->mmc_card_type = CARD_TYPE_MMC_4_x;
            DEBUG2("It's MMC standard capacity card.\n");
        }
        return ENOERR;
    }
    else
    {
        disk->mmc_card_type = CARD_TYPE_UNUSED;
        return -EIO;
    }
}

static Cyg_ErrNo
sd_media_type_identify(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    cyg_uint16 argh = 0x0, argl = 0x0;
    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 OCR;

    // Check SD specification version
    // Send CMD 8, and R7 returns
    DEBUG2("CMD 8 sent.\n");
    argl = (1 << 8) | 0xAA; // VHS = 0x1; check pattern = 0xAA;
    err = sd_send_command(SD_REQUEST_SEND_IF_COND, 0, argl, CMD_DAT_CONT_R1_RESPONSE);
    // Card returns response
    // SD 2.00
    if (err == ENOERR)
    {
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);

        if (((reply_2 & 0x000F) == 0x1) && ((reply_3 & 0xFF00) == 0xAA00))
        {
            DEBUG2("Response to CMD8, and check ok.\n");
        }
        else
        {
            DEBUG2("Response to CMD8, but check fail.\n");
            return -EIO;
        }

        // Send ACMD 41 with HCS = 1
        // CMD 55: APP_CMD
        // [31:16] RCA = 0x0000 in IDLE_STATE
        do {
            err = sd_send_command(MMC_REQUEST_APP_CMD, 0, 0, CMD_DAT_CONT_R1_RESPONSE);
            if (err != ENOERR)
            {
                return -EIO;
            }
            argh = 0x40FF;
            argl = 0x8000;
            err = sd_send_command(SD_REQUEST_APP_OP_COND, argh, argl, CMD_DAT_CONT_R3_RESPONSE);
            DEBUG2(" ACMD 41 sent.\n");

            if (err == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);

                OCR = ((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8);
                disk->OCR = OCR;
                DEBUG2(" OCR = %X\n", OCR);
            }
            else
            {
                DEBUG2("No response to ACMD 41.\n");
                return -EIO;
            }
        } while (!(OCR & 0x80000000)); // Repeat until busy_bit in OCR is cleared

        // CSS = 1
        // SD 2.00 High Capacity Card
        if ((OCR >> 30) == 0x3)
        {
            DEBUG2("It's SD 2.00 High Capacity Card.\n");
            disk->mmc_card_type = CARD_TYPE_SDHC;
        }
        // CSS = 0
        // SD 2.00 Standard Capacity Card
        else if ((OCR >> 30) == 0x2)
        {
            DEBUG2("It's SD 2.00 Standard Capacity Card.\n");
            disk->mmc_card_type = CARD_TYPE_SD_2_00;
        }
        else
        {
            DEBUG2("No CCS in ACMD 41 response.\n");
            disk->mmc_card_type = CARD_TYPE_UNUSED;
            return -EIO;
        }
    }
    // No response
    // SD 1.x or not SD
    // Send ACMD 41 with HCS = 0
    else
    {
        DEBUG2("No response to CMD8, and it's not SD 2.00 compatible.\n");
        // Send ACMD 41 with HCS = 0
        // CMD 55: APP_CMD
        // [31:16] RCA = 0x0000 in IDLE_STATE
        do {
            err = sd_send_command(MMC_REQUEST_APP_CMD, 0, 0, CMD_DAT_CONT_R1_RESPONSE);
            if (err != ENOERR)
            {
                // This may be a MMC card.
                DEBUG2("No response to ACMD 41.\n");
                //disk->mmc_card_type = CARD_TYPE_UNUSED;
                //return -ENODEV;
                return mmc_media_type_identify(disk);
            }
            argh = 0x00FF;
            argl = 0x8000;
            err = sd_send_command(SD_REQUEST_APP_OP_COND, argh, argl, CMD_DAT_CONT_R3_RESPONSE);
            DEBUG2(" ACMD 41 sent.\n");

            if (err == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                OCR = ((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8);
                disk->OCR = OCR;
                DEBUG2(" OCR = %X\n", OCR);
            }
            else
            {
                // This may be a MMC card.
                DEBUG2("No response to ACMD 41.\n");
                //disk->mmc_card_type = CARD_TYPE_UNUSED;
                //return -ENODEV;
                return mmc_media_type_identify(disk);
            }
        } while (!(OCR & 0x80000000)); // Repeat until busy_bit in OCR is cleared
        DEBUG2("It's SD 1.x Standard Capacity Card.\n");
        disk->mmc_card_type = CARD_TYPE_SD_1_x;
    }

    return ENOERR;
}

static Cyg_ErrNo
mmc_parse_attribute(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    mmc_csd_register *CSD = &disk->CSD;
    cyg_uint8   data[MMC_DEFAULT_BLOCK_LENGTH] = {0};
    cyg_uint8*  partition = NULL;
    cyg_uint32  lba_first, lba_size, lba_end, head, cylinder, sector;

    if ((0 != MMC_CSD_REGISTER_FILE_FORMAT_GROUP(CSD)) ||
        (0 != MMC_CSD_REGISTER_FILE_FORMAT(CSD)))
    {
        DEBUG2(" FILE_FORMAT_GRP = %d\n", MMC_CSD_REGISTER_FILE_FORMAT_GROUP(CSD));
        DEBUG2(" FILE_FORMAT = %d\n", MMC_CSD_REGISTER_FILE_FORMAT(CSD));
        return -ENOTDIR;
    }

    if ((0 != MMC_CSD_REGISTER_PERM_WRITE_PROTECT(CSD)) ||
        (0 != MMC_CSD_REGISTER_TMP_WRITE_PROTECT(CSD)))
    {
        disk->mmc_read_only   = true;
    }
    else
    {
        disk->mmc_read_only   = false;
    }
    DEBUG2(" Disk read-only flag %d\n", disk->mmc_read_only);

    if (disk->mmc_card_type == CARD_TYPE_MMC_HC)
    {
        // Sector mode
        mmc_ext_csd_register *ext_csd = &disk->EXT_CSD;
        cyg_uint32 *sec_count = (cyg_uint32 *)&ext_csd->ext_csd_data[295];
        disk->mmc_block_count = *sec_count;
    }
    else
    {
        // Byte mode
        disk->mmc_block_count = (((cyg_uint64)(0x01 << MMC_CSD_REGISTER_READ_BL_LEN(CSD))) *
                                 ((cyg_uint64)(0x01 << (MMC_CSD_REGISTER_C_SIZE_MULT(CSD) + 2))) *
                                 ((cyg_uint64)(MMC_CSD_REGISTER_C_SIZE(CSD) + 1))) / (cyg_uint64)MMC_DEFAULT_BLOCK_LENGTH;
    }
    DEBUG2("Disk blockcount %d (0x%08x)\n", disk->mmc_block_count, disk->mmc_block_count);

    disk->mmc_read_block_length  = 0x01 << MMC_CSD_REGISTER_READ_BL_LEN(CSD);
    disk->mmc_write_block_length = 0x01 << MMC_CSD_REGISTER_WRITE_BL_LEN(CSD);

    err = sd_read_disk_block(disk, data, 0);
    if (err)
    {
        return err;
    }

    if ((0x0055 != data[0x1FE]) || (0X00AA != data[0X1FF]))
    {
        DEBUG2(" (0x0055 != data[0x1fe]) || (0x00aa != data[0x1ff])\n");
        return -ENOTDIR;
    }
    partition   = &(data[0x1be]);
    lba_first   = (partition[11] << 24) | (partition[10] << 16) | (partition[9] << 8) | partition[8];
    lba_size    = (partition[15] << 24) | (partition[14] << 16) | (partition[13] << 8) | partition[12];
    lba_end     = lba_first + lba_size - 1;

    // First sector in c/h/s format
    cylinder    = ((partition[2] & 0xC0) << 2) | partition[3];
    head        = partition[1];
    sector      = partition[2] & 0x3F;

    // lba_start == (((cylinder * Nh) + head) * Ns) + sector - 1, where (Nh == heads/cylinder) and (Ns == sectors/head)
    // Strictly speaking we should be solving some simultaneous
    // equations here for lba_start/lba_end, but that gets messy.
    // The first partition is at the start of the card so cylinder will be 0,
    // and we can ignore Nh.
    CYG_ASSERT(0 == cylinder, "Driver assumption - partition 0 is at start of card\n");
    CYG_ASSERT(0 != head,     "Driver assumption - partition table is sensible\n");
    disk->mmc_sectors_per_head = ((lba_first + 1) - sector) / head;

    // Now for lba_end.
    cylinder    = ((partition[6] & 0xC0) << 2) | partition[7];
    head        = partition[5];
    sector      = partition[6] & 0x3F;
    disk->mmc_heads_per_cylinder = ((((lba_end + 1) - sector) / disk->mmc_sectors_per_head) - head) / cylinder;

    return ENOERR;
}

static Cyg_ErrNo
sd_parse_attribute(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;
    mmc_csd_register *CSD = &disk->CSD;
    cyg_uint8 CSD_ver = 0;

    CSD_ver = SD_CSD_STRUCTURE(CSD);
    if (CSD_ver == 0)
    {
        DEBUG2(" It's CSD version 1.0\n");
        // There is information available about the file format, e.g.
        // partitioned vs. simple FAT. With the current version of the
        // generic disk code this needs to be known statically, via
        // the mbr field of the disk channel structure. If the card
        // is inappropriately formatted, reject the mount request.
        if ((0 != SD_CSD_v1_FILE_FORMAT_GROUP(CSD)) ||
            (0 != SD_CSD_v1_FILE_FORMAT(CSD))) {
            DEBUG2(" FILE_FORMAT_GRP = %d\n", SD_CSD_v1_FILE_FORMAT_GROUP(CSD));
            DEBUG2(" FILE_FORMAT = %d\n", SD_CSD_v1_FILE_FORMAT(CSD));
            return -ENOTDIR;
        }

        // Look for a write-protect bit (permanent or temporary), and set
        // the disk as read-only or read-write as appropriate. The
        // temporary write-protect could be cleared by rewriting the CSD
        // register (including recalculating the CRC) but the effort
        // involves does not seem worth-while.
        if ((0 != SD_CSD_v1_PERM_WRITE_PROTECT(CSD)) || (0 != SD_CSD_v1_TMP_WRITE_PROTECT(CSD))) {
            disk->mmc_read_only   = true;
        } else {
            disk->mmc_read_only   = false;
        }
        DEBUG2(" Disk read-only flag %d\n", disk->mmc_read_only);

        // Calculate the disk size, primarily for assertion purposes.
        // By design MMC cards are limited to 4GB, which still doesn't
        // quite fit into 32 bits.
        disk->mmc_block_count = (((cyg_uint64)(0x01 << SD_CSD_v1_READ_BL_LEN(CSD))) *
                                 ((cyg_uint64)(0x01 << (SD_CSD_v1_C_SIZE_MULT(CSD) + 2))) *
                                 ((cyg_uint64)(SD_CSD_v1_C_SIZE(CSD) + 1))) / (cyg_uint64)MMC_DEFAULT_BLOCK_LENGTH;
        DEBUG2("Disk blockcount %d (0x%08x)\n", disk->mmc_block_count, disk->mmc_block_count);

        // Assume for now that the block length is 512 bytes. This is
        // probably a safe assumption since we have just got the card
        // initialized out of idle state. If it ever proves to be a problem
        // the SET_BLOCK_LEN command can be used.
        // Nevertheless store the underlying block sizes
        disk->mmc_read_block_length  = 0x01 << SD_CSD_v1_READ_BL_LEN(CSD);
        disk->mmc_write_block_length = 0x01 << SD_CSD_v1_WRITE_BL_LEN(CSD);

        // Read the partition table off the card. This is a way of
        // checking that the card is not password-locked. It also
        // provides information about the "disk geometry" which is
        // needed by higher-level code.
        // FIXME: the higher-level code should be made to use LBA
        // addressing instead.
        {
            cyg_uint8   data[MMC_DEFAULT_BLOCK_LENGTH] = {0};
            cyg_uint8*  partition = NULL;
            cyg_uint32  lba_first, lba_size, lba_end, head, cylinder, sector;

            err = sd_read_disk_block(disk, data, 0);
            if (err)
            {
                return err;
            }

            if ((0x0055 != data[0x1FE]) || (0X00AA != data[0X1FF]))
            {
                DEBUG2(" (0x0055 != data[0x1fe]) || (0x00aa != data[0x1ff])\n");
                return -ENOTDIR;
            }
            partition   = &(data[0x1be]);
            lba_first   = (partition[11] << 24) | (partition[10] << 16) | (partition[9] << 8) | partition[8];
            lba_size    = (partition[15] << 24) | (partition[14] << 16) | (partition[13] << 8) | partition[12];
            lba_end     = lba_first + lba_size - 1;

            // First sector in c/h/s format
            cylinder    = ((partition[2] & 0xC0) << 2) | partition[3];
            head        = partition[1];
            sector      = partition[2] & 0x3F;

            // lba_start == (((cylinder * Nh) + head) * Ns) + sector - 1, where (Nh == heads/cylinder) and (Ns == sectors/head)
            // Strictly speaking we should be solving some simultaneous
            // equations here for lba_start/lba_end, but that gets messy.
            // The first partition is at the start of the card so cylinder will be 0,
            // and we can ignore Nh.
            CYG_ASSERT(0 == cylinder, "Driver assumption - partition 0 is at start of card\n");
            CYG_ASSERT(0 != head,     "Driver assumption - partition table is sensible\n");
            disk->mmc_sectors_per_head = ((lba_first + 1) - sector) / head;

            // Now for lba_end.
            cylinder    = ((partition[6] & 0xC0) << 2) | partition[7];
            head        = partition[5];
            sector      = partition[6] & 0x3F;
            disk->mmc_heads_per_cylinder = ((((lba_end + 1) - sector) / disk->mmc_sectors_per_head) - head) / cylinder;
        }
    }
    else if (CSD_ver == 1)
    {
        DEBUG2(" It's CSD version 2.0\n");
        // There is information available about the file format, e.g.
        // partitioned vs. simple FAT. With the current version of the
        // generic disk code this needs to be known statically, via
        // the mbr field of the disk channel structure. If the card
        // is inappropriately formatted, reject the mount request.
        if ((0 != SD_CSD_v2_FILE_FORMAT_GROUP(CSD)) ||
            (0 != SD_CSD_v2_FILE_FORMAT(CSD))) {
            DEBUG2(" FILE_FORMAT_GRP = %d\n", SD_CSD_v2_FILE_FORMAT_GROUP(CSD));
            DEBUG2(" FILE_FORMAT = %d\n", SD_CSD_v2_FILE_FORMAT(CSD));
            return -ENOTDIR;
        }

        // Look for a write-protect bit (permanent or temporary), and set
        // the disk as read-only or read-write as appropriate. The
        // temporary write-protect could be cleared by rewriting the CSD
        // register (including recalculating the CRC) but the effort
        // involves does not seem worth-while.
        if ((0 != SD_CSD_v2_PERM_WRITE_PROTECT(CSD)) || (0 != SD_CSD_v2_TMP_WRITE_PROTECT(CSD))) {
            disk->mmc_read_only   = true;
        } else {
            disk->mmc_read_only   = false;
        }
        DEBUG2(" Disk read-only flag %d\n", disk->mmc_read_only);

        // Calculate the disk size, primarily for assertion purposes.
        // By design MMC cards are limited to 4GB, which still doesn't
        // quite fit into 32 bits.
        disk->mmc_block_count = ((cyg_uint64)(SD_CSD_v2_C_SIZE(CSD) + 1)) * 1024;
        DEBUG2("Disk blockcount %d (0x%08x)\n", disk->mmc_block_count, disk->mmc_block_count);

        // Assume for now that the block length is 512 bytes. This is
        // probably a safe assumption since we have just got the card
        // initialized out of idle state. If it ever proves to be a problem
        // the SET_BLOCK_LEN command can be used.
        // Nevertheless store the underlying block sizes
        disk->mmc_read_block_length  = 0x01 << SD_CSD_v2_READ_BL_LEN(CSD);
        disk->mmc_write_block_length = 0x01 << SD_CSD_v2_WRITE_BL_LEN(CSD);

        // Read the partition table off the card. This is a way of
        // checking that the card is not password-locked. It also
        // provides information about the "disk geometry" which is
        // needed by higher-level code.
        // FIXME: the higher-level code should be made to use LBA
        // addressing instead.
        {
            cyg_uint8   data[MMC_DEFAULT_BLOCK_LENGTH] = {0};
            cyg_uint8*  partition = NULL;
            cyg_uint32  lba_first, lba_size, lba_end, head, cylinder, sector;

            err = sd_read_disk_block(disk, data, 0);
            if (err)
            {
                return err;
            }

            if ((0x0055 != data[0x1FE]) || (0x00AA != data[0x1FF]))
            {
                DEBUG2(" (0x0055 != data[0x1fe]) || (0x00aa != data[0x1ff])\n");
                return -ENOTDIR;
            }
            partition   = &(data[0x1BE]);
            lba_first   = (partition[11] << 24) | (partition[10] << 16) | (partition[9] << 8) | partition[8];
            lba_size    = (partition[15] << 24) | (partition[14] << 16) | (partition[13] << 8) | partition[12];
            lba_end     = lba_first + lba_size - 1;

            // First sector in c/h/s format
            cylinder    = ((partition[2] & 0xC0) << 2) | partition[3];
            head        = partition[1];
            sector      = partition[2] & 0x3F;

            // lba_start == (((cylinder * Nh) + head) * Ns) + sector - 1, where (Nh == heads/cylinder) and (Ns == sectors/head)
            // Strictly speaking we should be solving some simultaneous
            // equations here for lba_start/lba_end, but that gets messy.
            // The first partition is at the start of the card so cylinder will be 0,
            // and we can ignore Nh.
            CYG_ASSERT(0 == cylinder, "Driver assumption - partition 0 is at start of card\n");
            CYG_ASSERT(0 != head,     "Driver assumption - partition table is sensible\n");
            disk->mmc_sectors_per_head = ((lba_first + 1) - sector) / head;

            // Now for lba_end.
            cylinder    = ((partition[6] & 0xC0) << 2) | partition[7];
            head        = partition[5];
            sector      = partition[6] & 0x3F;
            disk->mmc_heads_per_cylinder = ((((lba_end + 1) - sector) / disk->mmc_sectors_per_head) - head) / cylinder;
        }
    }
    else
    {
        DEBUG2(" It's reserved value.\n");
    }

    return ENOERR;
}

static Cyg_ErrNo
sd_send_command(cyg_uint16 cmd, cyg_uint16 argh, cyg_uint16 argl, cyg_uint16 cmd_dat_cont)
{
    FUNC_DBG_PTR;
    cyg_uint16 val = 0x0;
    Cyg_ErrNo code = ENOERR;

    DEBUG2("Prepare to send CMD %d.\n", cmd);
    DEBUG2(" argh = 0x%04X\n", argh);
    DEBUG2(" argl = 0x%04X\n", argl);
    DEBUG2(" cmd_dat_cont = 0x%04X\n", cmd_dat_cont);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_0, cmd);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_1, argh);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_2, argl);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_DAT, cmd_dat_cont);

    // Check busy or not
    if (cmd_dat_cont & CMD_DAT_CONT_BUSY_BIT_ON)
    {
        cyg_uint32 i = 0;
        do {
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            i++;
        } while (!(val & STATUS_BUSY_SIGNAL_END) && (i < 1000));
    }

    // Check end command response
    do {
        HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
        DEBUG2(" REG_SIS910_SD_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_END_CMD_RESPONSE));

    // Check response timeout or not
    HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
    if (val & STATUS_RESPONSE_TIMEOUT)
    {
        // No response return!!
        DEBUG2(" REG_SIS910_SD_STATUS = 0x%04X\n", val);
        DEBUG2(" Response Timeout!!\n");
        return -EIO;
    }

    return code;
}

void sd_send_command_no_resp(cyg_uint16 cmd, cyg_uint16 argh, cyg_uint16 argl, cyg_uint16 cmd_dat_cont)
{
    FUNC_DBG_PTR;
    cyg_uint16 val = 0x0;

    DEBUG2("Prepare to send CMD %d.\n", cmd);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_0, cmd);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_1, argh);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_2, argl);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_DAT, cmd_dat_cont);

    // Check busy or not
    do {
        HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
        DEBUG2(" REG_SIS910_SD_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_END_CMD_RESPONSE));
}

static Cyg_ErrNo
sd_send_init(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    cyg_mmc_device *dev = disk->mmc_dev;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 RCA = 0;
    mmc_csd_register *CSD = &disk->CSD;
    mmc_cid_register *CID = &disk->mmc_cid;

    // Initialize Gateway
    sd_gateway_init();

    // Initialize H/W
    sd_hw_init(disk);

    // Initialize SD card
    // Go to idle state
    // Send CMD 0
    sd_send_command_no_resp(MMC_REQUEST_GO_IDLE_STATE, 0, 0, CMD_DAT_CONT_CARD_INIT);

    // Media type identify
    err = sd_media_type_identify(disk);
    if (err != ENOERR)
    {
        return err;
    }

    // ==========================
    //  Get CID
    // ==========================
    err = sd_get_CID(CID);
    if (err != ENOERR)
    {
        return err;
    }

    // ==========================
    //  Get RCA (SD card) or
    //  Set RCA (MMC card)
    // ==========================
    if (disk->mmc_card_type & CARD_TYPE_SD_CARD)
    {
        // Get RCA from SD card
        err = sd_get_RCA(&RCA);
    }
    else if (disk->mmc_card_type & CARD_TYPE_MMC_CARD)
    {
        // Set RCA to MMC card
        RCA = 0x0001;
        err = mmc_set_RCA(&RCA);
    }
    if (err == ENOERR)
    {
        disk->RCA = RCA;
    }
    else
    {
        return err;
    }

    // ==========================
    //  Get CSD
    // ==========================
    err = sd_get_CSD(CSD, &RCA);
    if (err != ENOERR)
    {
        return err;
    }

    // ==========================
    //  Move to Transfer State
    // ==========================
    // Send CMD 7
    DEBUG2("CMD 7 sent.\n");

    err = sd_send_command(MMC_REQUEST_SELECT_DESELECT, RCA, 0, CMD_DAT_CONT_R1_RESPONSE);
    if (err == ENOERR)
    {
        DEBUG2("Card initialized and move into transfer state.\n");
    }
    else
    {
        DEBUG2("Cannot move into transfer state.\n");
        return -EIO;
    }

    // ==========================
    //  Get EXT CSD (MMC 4.x)
    // ==========================
    if (disk->mmc_card_type & CARD_TYPE_MMC_CARD)
    {
        if (MMC_CSD_REGISTER_SPEC_VERS(&disk->CSD) >= 4)
        {
            cyg_uint8 ext_csd[512] = {0};
            err = mmc_get_EXT_CSD(disk, (cyg_uint8 *)&ext_csd);
            if (err != ENOERR)
            {
                return err;
            }
        }
        else
        {
            disk->mmc_card_type = CARD_TYPE_MMC_3_x;
        }
    }

    // ==========================
    //  Read SCR
    // ==========================
    if (disk->mmc_card_type & CARD_TYPE_SD_CARD)
    {
        err = sd_get_SCR(disk, &RCA);
        if (err != ENOERR)
        {
            return err;
        }
    }

    // ==========================
    //  Change bus frequency
    // ==========================
    switch (disk->mmc_card_type)
    {
        case CARD_TYPE_SD_1_10:
        case CARD_TYPE_SD_2_00:
        case CARD_TYPE_SDHC:
            err = sd_switch_speed_mode(disk);
            if (err == ENOERR)
            {
                // Change divisor to meet 50MHz
            }
            break;
        case CARD_TYPE_MMC_HS:
            err = mmc_switch_speed_mode(disk);
            if (err == ENOERR)
            {
                // Change divisor to meet 50MHz
            }
            break;
        default:
            break;
    }

    // ==========================
    //  Set bus width
    // ==========================
    if (dev->mmc_wide_bus_disable == 1)
    {
        DEBUG2("SD Bus Width = 1\n");
        disk->mmc_bus_width = 1;
    }
    else
    {
        // ==========================
        //  Set bus width to 4 bits
        // ==========================
        switch (disk->mmc_card_type)
        {
            case CARD_TYPE_SD_1_10:
            case CARD_TYPE_SD_2_00:
            case CARD_TYPE_SDHC:
                sd_switch_bus_width(disk, &RCA);
                break;
            case CARD_TYPE_MMC_HS:
                mmc_switch_bus_width(disk);
                break;
            default:
                break;
        }
    }

    return ENOERR;
}

#ifdef SIS910_READ_SD_THRU_GATEWAY
static Cyg_ErrNo
sd_gateway_read_disk_block(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 block)
{
    FUNC_DBG_PTR;
    cyg_uint32 current_state = 0x0;
    cyg_uint16 RCA = disk->RCA;
    cyg_uint16 argh = 0, argl = 0;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 cont = CMD_DAT_CONT_R1_RESPONSE;
    cyg_uint32 val = 0;

    if (disk->mmc_bus_width == 4)
    {
        cont |= CMD_DAT_CONT_WIDE_BUS_MODE;
    }

    HAL_WRITE_UINT32(gw_fifo_reset,0x8);
    HAL_WRITE_UINT32(gw_fifo_reset,0x0);

    // Flush cache
    r16_cache_flush(buf, 512);
    gw_xfer(wt3, (cyg_uint32)buf, 16, 1, 0x2);

    // Set block length
    sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH, cont);
    HAL_WRITE_UINT32(REG_SIS910_SD_SWAPCTL, 0x1);
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x2);
    HAL_WRITE_UINT32(REG_SIS910_SD_NO_BLKS, 1);

    switch (disk->mmc_card_type)
    {
        case CARD_TYPE_SD_1_00:
        case CARD_TYPE_SD_1_10:
        case CARD_TYPE_SD_1_x:
        case CARD_TYPE_SD_2_00:
        case CARD_TYPE_MMC_3_x:
        case CARD_TYPE_MMC_HS:
            argh = (block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
            argl = (block * MMC_DEFAULT_BLOCK_LENGTH);
            break;
        case CARD_TYPE_SDHC:
        case CARD_TYPE_MMC_HC:
            argh = block >> 16;
            argl = block;
            break;
        case CARD_TYPE_UNUSED:
        default:
            return -EINVAL;
    }

    // CMD17: READ_SINGLE_BLOCK
    // Read single block from 'block'
    err = sd_send_command(MMC_REQUEST_READ_SINGLE_BLOCK, argh, argl, (cont | CMD_DAT_CONT_DATA_ENABLE));
    if (err == ENOERR)
    {
        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_DATA_TRANSFER_DONE));

        do {
            cyg_uint32 card_status;
            cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;

            err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, cont);
            if (err == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
                current_state = card_status >> 9;
                DEBUG2(" current_state = %x\n", current_state);
            }
            else
            {
                return -EIO;
            }
        } while (current_state != 4);
    }

    HAL_WRITE_UINT32(REG_SIS910_SD_SWAPCTL, 0x1);

    return ENOERR;
}
#endif

//static Cyg_ErrNo
//sd_gateway_read_disk_multiple_blocks(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block)
//{
//    FUNC_DBG_PTR;
//    cyg_uint16 argh, argl;
//    cyg_uint16 RCA = disk->RCA;
//    Cyg_ErrNo err = ENOERR;
//    cyg_uint32 current_state = 0x0;
//    cyg_uint32 cont = 0x0;
//
//    // Flush cache
//    r16_cache_flush(buf, 512);
//
//    // Read SD data through Gateway
//    HAL_WRITE_UINT32(gw_fifo_reset, 0x8); // Must reset fifo to avoid fifo have residual data
//    HAL_WRITE_UINT32(gw_fifo_reset, 0x0);
//    gw_xfer(wt3, (cyg_uint32)buf, 16, blocks, 0x4); // 512 / 32 = 16
//    HAL_WRITE_UINT32(REG_SIS910_SD_SWAPCTL, 0x1); // byte swap input data  to correct data format
//    HAL_WRITE_UINT32(REG_SIS910_SD_NO_BLKS, blocks); // set up block number to read */
//
//    // Read 'MMC_DEFAULT_BLOCK_LENGTH' bytes through CPU
//    // Set block length to 'MMC_DEFAULT_BLOCK_LENGTH' bytes
//    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
//    // Set the clock rate
//    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x0);
//
//    // Default block length is specified in the CSD
//    // CMD16: SET_BLOCKLEN
//    err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH,
//                          CMD_DAT_CONT_R1_RESPONSE);
//    if (err != ENOERR)
//    {
//       return -EIO;
//    }
//    // CMD18: READ_MULTIPLE_BLOCK
//    // Read single block from 'block'
//    if ((disk->mmc_card_type == CARD_TYPE_SD_1_x) || (disk->mmc_card_type == CARD_TYPE_SD_2_00))
//    {
//        argh = (first_block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
//        argl = (first_block * MMC_DEFAULT_BLOCK_LENGTH);
//    }
//    else
//    {   // SDHC
//        argh = first_block >> 16;
//        argl = first_block;
//    }
//
//    if (disk->mmc_bus_width == 4)
//    {
//        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE |
//               CMD_DAT_CONT_WIDE_BUS_MODE | CMD_DAT_CONT_NOB_ON | CMD_DAT_CONT_DATA_MULTIPLE;
//    }
//    else
//    {
//        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE |
//               CMD_DAT_CONT_NOB_ON | CMD_DAT_CONT_DATA_MULTIPLE;
//    }
//
//    err = sd_send_command(MMC_REQUEST_READ_SINGLE_BLOCK, argh, argl, cont);
//    if (err == ENOERR)
//    {
//        cyg_uint16 val = 0x0;
//
//        do {
//            // Wait data transfer done
//            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
//            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
//        } while (!(val & STATUS_DATA_TRANSFER_DONE));
//
//        do {
//            // End command response
//            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
//            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
//        } while (!(val & STATUS_END_CMD_RESPONSE));
//
//        // CMD12: STOP_TRANSMISSION
//        // Request card to stop transmission
//        sd_send_command(MMC_REQUEST_STOP_TRANSMISSION, 0x0, 0x0, CMD_DAT_CONT_R1_RESPONSE);
//        if (err == ENOERR)
//        {
//            ;
//        }
//
//        // Wait SD card go back to tran state
//        do {
//            cyg_uint32 card_status;
//            cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;
//
//            err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, CMD_DAT_CONT_R1_RESPONSE);
//            if (err == ENOERR)
//            {
//                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
//                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
//                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
//                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
//                                current_state = card_status >> 9;
//                DEBUG2(" current_state = %x\n", current_state);
//            }
//            else
//            {
//                return -EIO;
//            }
//        } while (current_state != 4);
//    }
//    else
//    {
//       return -EIO;
//    }
//
//    HAL_WRITE_UINT32(REG_SIS910_SD_SWAPCTL, 0x1); // return default swap control
//
//    return ENOERR;
//}

static Cyg_ErrNo
sd_read_disk_block(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 block)
{
    FUNC_DBG_PTR;
    cyg_uint16 argh, argl;
    cyg_uint16 RCA = disk->RCA;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 current_state = 0x0;
    cyg_uint32 cont = 0x0;

    // Read 'MMC_DEFAULT_BLOCK_LENGTH' bytes through CPU
    // Set block length to 'MMC_DEFAULT_BLOCK_LENGTH' bytes
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    // Set read through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x10);

    switch (disk->mmc_card_type)
    {
        case CARD_TYPE_SD_1_00:
        case CARD_TYPE_SD_1_10:
        case CARD_TYPE_SD_1_x:
        case CARD_TYPE_SD_2_00:
        case CARD_TYPE_MMC_3_x:
        case CARD_TYPE_MMC_HS:
            // Default block length is specified in the CSD
            // CMD16: SET_BLOCKLEN
            err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN,
                                  0,
                                  MMC_DEFAULT_BLOCK_LENGTH,
                                  CMD_DAT_CONT_R1_RESPONSE);
            if (err != ENOERR)
            {
                return -EIO;
            }
            argh = (block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
            argl = (block * MMC_DEFAULT_BLOCK_LENGTH);
            break;
        case CARD_TYPE_SDHC:
        case CARD_TYPE_MMC_HC:
            argh = block >> 16;
            argl = block;
            break;
        case CARD_TYPE_UNUSED:
        default:
            return -EINVAL;
    }

//    if ((disk->mmc_card_type == CARD_TYPE_SD_1_x) || (disk->mmc_card_type == CARD_TYPE_SD_2_00))
//    {
//        // Default block length is specified in the CSD
//        // CMD16: SET_BLOCKLEN
//        err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH,
//                              CMD_DAT_CONT_R1_RESPONSE);
//        if (err != ENOERR)
//        {
//            CURRENT_POS_PTR;
//            return -EIO;
//        }
//        argh = (block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
//        argl = (block * MMC_DEFAULT_BLOCK_LENGTH);
//    }
//    else
//    {   // SDHC
//        argh = block >> 16;
//        argl = block;
//    }

    if (disk->mmc_bus_width == 4)
    {
        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE | CMD_DAT_CONT_WIDE_BUS_MODE;
    }
    else
    {
        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE;
    }

    // CMD17: READ_SINGLE_BLOCK
    // Read single block from 'block'
    err = sd_send_command(MMC_REQUEST_READ_SINGLE_BLOCK, argh, argl, cont);
    if (err == ENOERR)
    {
        cyg_uint32 i;
        cyg_uint16 val = 0x0;

        // Read data from SD_DATA_BUFFERS register
        for (i = 0; i < (MMC_DEFAULT_BLOCK_LENGTH / 2); i++)
        {
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
            } while (!(val & 0x40)); // Xport ready
            // Read data
            HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);

            *buf++ = (val & 0xFF);
            *buf++ = (val >> 8);
        }

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_DATA_TRANSFER_DONE));

        do {
            // End command response
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_END_CMD_RESPONSE));

        // Wait SD card go back to tran state
        do {
            cyg_uint32 card_status;
            cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;

            err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, CMD_DAT_CONT_R1_RESPONSE);
            if (err == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
                current_state = card_status >> 9;
                DEBUG2(" current_state = %x\n", current_state);
            }
            else
            {
                return -EIO;
            }
        } while (current_state != 4);
    }
    else
    {
        return -EIO;
    }

    return ENOERR;
}

//static Cyg_ErrNo
//sd_read_disk_multiple_blocks(cyg_mmc_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 blocks, cyg_uint32 first_block)
//{
//    FUNC_DBG_PTR;
//    cyg_uint16 argh, argl;
//    cyg_uint16 RCA = disk->RCA;
//    Cyg_ErrNo err = ENOERR;
//    cyg_uint32 current_state = 0x0;
//    cyg_uint32 cont = 0x0;
//
//    // Read 'MMC_DEFAULT_BLOCK_LENGTH' bytes through CPU
//    // Set block length to 'MMC_DEFAULT_BLOCK_LENGTH' bytes
//    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
//    // Set read through CPU
//    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x10);
//
//    // Set number of blocks to read
//    HAL_WRITE_UINT32(REG_SIS910_SD_NO_BLKS, blocks);
//
//    // Default block length is specified in the CSD
//    // CMD16: SET_BLOCKLEN
//    err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH,
//                          CMD_DAT_CONT_R1_RESPONSE);
//    if (err != ENOERR)
//    {
//       return -EIO;
//    }
//
//    // CMD18: READ_MULTIPLE_BLOCK
//    // Read single block from 'block'
//    if ((disk->mmc_card_type == CARD_TYPE_SD_1_x) || (disk->mmc_card_type == CARD_TYPE_SD_2_00))
//    {
//        argh = (first_block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
//        argl = (first_block * MMC_DEFAULT_BLOCK_LENGTH);
//    }
//    else
//    {   // SDHC
//        argh = first_block >> 16;
//        argl = first_block;
//    }
//
//    if (disk->mmc_bus_width == 4)
//    {
//        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE
//             | CMD_DAT_CONT_WIDE_BUS_MODE | CMD_DAT_CONT_NOB_ON | CMD_DAT_CONT_DATA_MULTIPLE;
//    }
//    else
//    {
//        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE
//             | CMD_DAT_CONT_NOB_ON | CMD_DAT_CONT_DATA_MULTIPLE;
//    }
//
//    err = sd_send_command(MMC_REQUEST_READ_MULTIPLE_BLOCK, argh, argl, cont);
//    if (err == ENOERR)
//    {
//        cyg_uint32 i;
//        cyg_uint16 val = 0x0;
//
//        // Read data from SD_DATA_BUFFERS register
//        for (i = 0; i < ((blocks * MMC_DEFAULT_BLOCK_LENGTH) / 2); i++)
//        {
//            do {
//                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
//            } while (!(val & 0x40)); // Xport ready
//            // Read data
//            HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);
//
//            *buf++ = (val & 0xFF);
//            *buf++ = (val >> 8);
//        }
//
//        do {
//            // Wait data transfer done
//            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
//            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
//        } while (!(val & STATUS_DATA_TRANSFER_DONE));
//
//        do {
//            // End command response
//            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
//            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
//        } while (!(val & STATUS_END_CMD_RESPONSE));
//
//        // CMD12: STOP_TRANSMISSION
//        // Request card to stop transmission
//        err = sd_send_command(MMC_REQUEST_STOP_TRANSMISSION, 0x0, 0x0, CMD_DAT_CONT_R1_RESPONSE);
//        if (err != ENOERR)
//        {
//            return -EIO;
//        }
//
//        // Wait SD card go back to tran state
//        do {
//            cyg_uint32 card_status;
//            cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;
//
//            err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, CMD_DAT_CONT_R1_RESPONSE);
//            if (err == ENOERR)
//            {
//                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
//                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
//                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
//                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
//                current_state = card_status >> 9;
//                DEBUG2(" current_state = %x\n", current_state);
//            }
//            else
//            {
//                return -EIO;
//            }
//        } while (current_state != 4);
//    }
//    else
//    {
//       return -EIO;
//    }
//
//    return ENOERR;
//}

#ifdef SIS910_WRITE_SD_THRU_GATEWAY
static Cyg_ErrNo
sd_gateway_write_disk_block(cyg_mmc_disk_info_t* disk, const cyg_uint8* buf, cyg_uint32 block)
{
    FUNC_DBG_PTR;
    cyg_uint16 argh, argl;
    cyg_uint16 RCA = disk->RCA;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 current_state = 0x0;
    cyg_uint32 cont = CMD_DAT_CONT_R1_RESPONSE;
    cyg_uint32 val = 0;

    if (disk->mmc_bus_width == 4)
    {
        cont |= CMD_DAT_CONT_WIDE_BUS_MODE;
    }

    // Flush cache
    r16_cache_flush(buf, 512);
	gw_xfer(rd3, (cyg_uint32)buf, 16, 1, 0x4);

    // Set block length
    sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH, cont);
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    HAL_WRITE_UINT32(REG_SIS910_SD_SWAPCTL, 0x3);
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x2);
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x2);

    switch (disk->mmc_card_type)
    {
        case CARD_TYPE_SD_1_00:
        case CARD_TYPE_SD_1_10:
        case CARD_TYPE_SD_1_x:
        case CARD_TYPE_SD_2_00:
        case CARD_TYPE_MMC_3_x:
        case CARD_TYPE_MMC_HS:
            argh = (block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
            argl = (block * MMC_DEFAULT_BLOCK_LENGTH);
            break;
        case CARD_TYPE_SDHC:
        case CARD_TYPE_MMC_HC:
            argh = block >> 16;
            argl = block;
            break;
        case CARD_TYPE_UNUSED:
        default:
            return -EINVAL;
    }

    // CMD24: WRITE_BLOCK
    // Write single block form 'block'
    err = sd_send_command(MMC_REQUEST_WRITE_BLOCK, argh, argl,
                         (cont | CMD_DAT_CONT_DATA_ENABLE | CMD_DAT_CONT_DATA_WRITE));
    if (err == ENOERR)
    {
        HAL_WRITE_UINT32(REG_SIS910_SD_BUF_RDY, 0x1);
        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_DATA_TRANSFER_DONE));

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_BUSY_SIGNAL_END));

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_END_CMD_RESPONSE));

        do {
            cyg_uint32 card_status;
            cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;

            err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, cont);
            if (err == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
                current_state = card_status >> 9;
                DEBUG2(" current_state = %x\n", current_state);
            }
            else
            {
                return -EIO;
            }
        } while (current_state != 4);
    }

    return ENOERR;
}
#endif

#ifdef SIS910_WRITE_SD_THRU_CPU
static Cyg_ErrNo
sd_write_disk_block(cyg_mmc_disk_info_t* disk, const cyg_uint8* buf, cyg_uint32 block)
{
    FUNC_DBG_PTR;
    cyg_uint16 argh, argl;
    cyg_uint16 RCA = disk->RCA;
    Cyg_ErrNo err = ENOERR;
    cyg_uint32 current_state = 0x0;
    cyg_uint32 cont = 0x0;

    // Write 'MMC_DEFAULT_BLOCK_LENGTH' bytes through CPU
    // Set block length to 'MMC_DEFAULT_BLOCK_LENGTH' bytes
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);
    // Set write through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x20);

    // Set the block length register to 512 Bytes
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, MMC_DEFAULT_BLOCK_LENGTH);

    switch (disk->mmc_card_type)
    {
        case CARD_TYPE_SD_1_00:
        case CARD_TYPE_SD_1_10:
        case CARD_TYPE_SD_1_x:
        case CARD_TYPE_SD_2_00:
        case CARD_TYPE_MMC_3_x:
        case CARD_TYPE_MMC_HS:
            // Default block length is specified in the CSD
            // CMD16: SET_BLOCKLEN
            err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN,
                                  0,
                                  MMC_DEFAULT_BLOCK_LENGTH,
                                  CMD_DAT_CONT_R1_RESPONSE);
            if (err != ENOERR)
            {
                return -EIO;
            }
            argh = (block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
            argl = (block * MMC_DEFAULT_BLOCK_LENGTH);
            break;
        case CARD_TYPE_SDHC:
        case CARD_TYPE_MMC_HC:
            argh = block >> 16;
            argl = block;
            break;
        case CARD_TYPE_UNUSED:
        default:
            return -EINVAL;
    }

//    if ((disk->mmc_card_type == CARD_TYPE_SD_1_x) || (disk->mmc_card_type == CARD_TYPE_SD_2_00))
//    {
//        // Default block length is specified in the CSD
//        // CMD16: SET_BLOCKLEN
//        err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH, CMD_DAT_CONT_R1_RESPONSE);
//        if (err != ENOERR)
//        {
//            return -EIO;
//        }
//        argh = (block * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
//        argl = (block * MMC_DEFAULT_BLOCK_LENGTH);
//    }
//    else
//    {   // SDHC
//        argh = block >> 16;
//        argl = block;
//    }

    if (disk->mmc_bus_width == 4)
    {
        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE
             | CMD_DAT_CONT_DATA_WRITE | CMD_DAT_CONT_WIDE_BUS_MODE;
    }
    else
    {
        cont = CMD_DAT_CONT_R1_RESPONSE | CMD_DAT_CONT_DATA_ENABLE
             | CMD_DAT_CONT_DATA_WRITE;
    }

    // CMD24: WRITE_BLOCK
    err = sd_send_command(MMC_REQUEST_WRITE_BLOCK, argh, argl, cont);
    if (err == ENOERR)
    {
        cyg_uint32 i;
        cyg_uint16 val = 0x0;

        // Set buffer ready signal and last buffer
        HAL_WRITE_UINT32(REG_SIS910_SD_BUF_RDY, 0x3);
        DEBUG2("  Set buffer ready signal and last buffer.\n");

        for (i = 0; i < (MMC_DEFAULT_BLOCK_LENGTH / 2); i++)
        {
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
            } while (val & 0x80);
            // Write data
            val = 0x0;
            val |= *buf++;
            val |= *buf++ << 8;

            HAL_WRITE_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);
        }

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_DATA_TRANSFER_DONE));

        do {
            // End command response
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            DEBUG2("%s:%d REG_SIS910_SD_STATUS = 0x%04X\n", __FUNCTION__, __LINE__, val);
        } while (!(val & STATUS_END_CMD_RESPONSE));

        // Wait SD card go back to tran state
        do {
            cyg_uint32 card_status;
            cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;

            err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, CMD_DAT_CONT_R1_RESPONSE);
            if (err == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
                current_state = card_status >> 9;
                DEBUG2(" current_state = %x\n", current_state);
            }
            else
            {
                return -EIO;
            }
        } while (current_state != 4);
    }

    return ENOERR;
}
#endif

// No hardware initialization is performed here. Even if a card is
// currently plugged in it may get removed before it gets mounted, so
// there is no point looking at the card here. It is still necessary
// to invoke the callback init function so that higher-level code gets
// a chance to do its bit.
static cyg_bool
sd_disk_init(struct cyg_devtab_entry* tab)
{
    FUNC_DBG_PTR;
    disk_channel* chan = (disk_channel*) tab->priv;

    //MMC_SPI_INIT_FF_DATA();
    return (*chan->callbacks->disk_init)(tab);
}

// lookup() is called during a mount() operation, so this is the right
// place to check whether or not there is a card.

static cyg_bool
sd_disk_changed(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    cyg_uint32 RCA = disk->RCA;
    cyg_uint32 cont = CMD_DAT_CONT_R1_RESPONSE;
    Cyg_ErrNo err = ENOERR;

    // Chang bus transportation mode
    if (disk->mmc_bus_width == 4)
    {
        cont |= CMD_DAT_CONT_WIDE_BUS_MODE;
    }

    // Send CMD 13: SEND_STATUS
    err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0, cont);

    if (err == ENOERR)
    {
        // The card response to SEND_STATUS, so it's alive.
        return false;
    }
    else
        {
        // Oops, the card is dead.
        return true;
        }
    }

static Cyg_ErrNo
sd_check_for_disk(cyg_mmc_disk_info_t* disk)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo err = ENOERR;

    // Perform card init
    err = sd_send_init(disk);
    if (err != ENOERR)
    {
        DEBUG2("Card initialization fails; err = %d\n", err);
        return err;
    }

    if (disk->mmc_card_type & CARD_TYPE_SD_CARD)
    {
        // Parse attribute of SD card
        err = sd_parse_attribute(disk);
    }
    else
    {
        // Parse attribute of MMC card
        err = mmc_parse_attribute(disk);
    }

    if (err != ENOERR)
    {
        DEBUG2("Card attribute parsing fails; err = %d\n", err);
        return err;
    }

    return err;
}

static char*
sd_disk_lookup_itoa(cyg_uint32 num, char* where)
{
    FUNC_DBG_PTR;
    if (0 == num) {
        *where++ = '0';
    } else {
        char local[10];  // 2^32 just fits into 10 places
        int  index = 9;
        while (num > 0) {
            local[index--] = (num % 10) + '0';
            num /= 10;
        }
        for (index += 1; index < 10; index++) {
            *where++ = local[index];
        }
    }
    return where;
}

static Cyg_ErrNo
sd_disk_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry *sub_tab, const char* name)
{
    FUNC_DBG_PTR;
    disk_channel* chan = (disk_channel*) (*tab)->priv;
    cyg_mmc_disk_info_t* disk  = (cyg_mmc_disk_info_t*) chan->dev_priv;
    Cyg_ErrNo result;
    cyg_disk_identify_t *ident = NULL;

//    diag_printf("Let's lookup the device.\n");

    if (disk->mmc_connected) {
        DEBUG2("Is the card still plugged.\n");
//        diag_printf("Is the card still plugged.\n");
        // There was a card plugged in last time we looked. Is it still there?
        if (sd_disk_changed(disk)) {
            // The old card is gone. Either there is no card plugged in, or
            // it has been replaced with a different one. If the latter the
            // existing mounts must be removed before anything sensible
            // can be done.
            DEBUG2("No, the card is gone.\n");
//            diag_printf("No, the card is gone.\n");
            disk->mmc_connected = false;
            (*chan->callbacks->disk_disconnected)(chan);
            if (0 != chan->info->mounts) {
                return -ENODEV;
            }
        }
        else
        {
            DEBUG2("Yes, the card is still alive.\n");
//            diag_printf("Yes, the card is still alive.\n");
        }
    }

    if ((0 != chan->info->mounts) && !disk->mmc_connected) {
        // There are still mount points to an old card. We cannot accept
        // new mount requests until those have been cleaned out.
        DEBUG2(" The mount points need to clean out.\n");
//        diag_printf("The mount points need to clean out.\n");
        return -ENODEV;
    }

    if (!disk->mmc_connected) {
        //cyg_disk_identify_t ident;
        cyg_uint32 id_data = 0;
        char* where = NULL;
        int i = 0;

        DEBUG2("Is it a new card plugged?\n");
//        diag_printf("Is it a new card plugged?\n");

        // The world is consistent and the higher-level code does not
        // know anything about the current card, if any. Is there a
        // card?
        result = sd_check_for_disk(disk);
        if (ENOERR != result) {
            DEBUG2(" Disk check fails.\n");
//            diag_printf("Disk check fails.\n");
            return result;
        }
        // A card has been found. Tell the higher-level code about it.
        // This requires an identify structure, although it is not
        // entirely clear what purpose that serves.
        disk->mmc_connected = true;

        ident = (cyg_disk_identify_t *)malloc(sizeof(cyg_disk_identify_t));
        if (ident)
        {
            memset(ident, 0, sizeof(cyg_disk_identify_t));
            // Serial number, up to 20 characters; The CID register contains
            // various fields which can be used for this.
            where   = &(ident->serial[0]);
            id_data = disk->mmc_cid.cid_data[0];   // 1-byte manufacturer id -> 3 chars, 17 left
            where   = sd_disk_lookup_itoa(id_data, where);
            id_data = (disk->mmc_cid.cid_data[1] << 8) + disk->mmc_cid.cid_data[2]; // 2-byte OEM ID, 5 chars, 12 left
            where   = sd_disk_lookup_itoa(id_data, where);
            id_data = (disk->mmc_cid.cid_data[10] << 24) + (disk->mmc_cid.cid_data[11] << 16) +
                (disk->mmc_cid.cid_data[12] << 8) + disk->mmc_cid.cid_data[13];
            where   = sd_disk_lookup_itoa(id_data, where); // 4-byte OEM ID, 10 chars, 2 left
            // And terminate the string with a couple of places to spare.
            *where = '\0';
//            diag_printf("Serial : %s\n", ident->serial);

            // Firmware revision number. There is a one-byte product
            // revision number in the CID, BCD-encoded
            id_data = disk->mmc_cid.cid_data[9] >> 4;
            if (id_data <= 9) {
                ident->firmware_rev[0] = id_data + '0';
            } else {
                ident->firmware_rev[0] = id_data - 10 + 'A';
            }
            id_data = disk->mmc_cid.cid_data[9] & 0x0F;
            if (id_data <= 9) {
                ident->firmware_rev[1] = id_data + '0';
            } else {
                ident->firmware_rev[1] = id_data - 10 + 'A';
            }
            ident->firmware_rev[2]   = '\0';
//            diag_printf("Firmware rev. : %s\n", ident->firmware_rev);

            // Model number. There is a six-byte product name in the CID.
            for (i = 0; i < 6; i++) {
                if ((disk->mmc_cid.cid_data[i + 3] >= 0x20) && (disk->mmc_cid.cid_data[i+3] <= 0x7E)) {
                    ident->model_num[i] = disk->mmc_cid.cid_data[i + 3];
                } else {
                    break;
                }
            }
            ident->model_num[i] = '\0';
//            diag_printf("Model : %s\n", ident->model_num);

            // We don't have no cylinders, heads, or sectors, but
            // higher-level code may interpret partition data using C/H/S
            // addressing rather than LBA. Hence values for some of these
            // settings were calculated above.
            ident->cylinders_num     = 1;
            ident->heads_num         = disk->mmc_heads_per_cylinder;
            ident->sectors_num       = disk->mmc_sectors_per_head;
            ident->lba_sectors_num   = disk->mmc_block_count;
            ident->phys_block_size   = disk->mmc_write_block_length/512;
            ident->max_transfer      = disk->mmc_write_block_length;

            (*chan->callbacks->disk_connected)(*tab, ident);
            free(ident);
            ident = NULL;
            // We now have a valid card and higher-level code knows about it. Fall through.
        }
        else
        {
            return -ENOMEM;
        }
    }

    // And leave it to higher-level code to finish the lookup, taking
    // into accounts partitions etc.
    return (*chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

static Cyg_ErrNo
sd_disk_read(disk_channel* chan, void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_mmc_disk_info_t* disk = (cyg_mmc_disk_info_t*) chan->dev_priv;
    cyg_uint32 i;
    cyg_uint8* buf = (cyg_uint8*) buf_arg;
    Cyg_ErrNo err = ENOERR;

    DEBUG2("[READ] blocks = %d; first_block = %d\n", blocks, first_block);

    if (! disk->mmc_connected) {
        return -ENODEV;
    }
    if ((first_block + blocks) >= disk->mmc_block_count) {
        DEBUG2(" The block to read is out of bound.\n");
        return -EINVAL;
    }

    for (i = 0; (i < blocks) && (ENOERR == err); i++)
    {
#ifdef SIS910_READ_SD_THRU_CPU
        err = sd_read_disk_block(disk, buf, first_block + i);
#else
        err = sd_gateway_read_disk_block(disk, buf, first_block + i);
#endif
        buf += MMC_DEFAULT_BLOCK_LENGTH;
    }

    return err;
}

static Cyg_ErrNo
sd_disk_write(disk_channel* chan, const void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_mmc_disk_info_t* disk = (cyg_mmc_disk_info_t*) chan->dev_priv;
    cyg_uint32 i;
    const cyg_uint8* buf = (cyg_uint8*) buf_arg;
    Cyg_ErrNo err = ENOERR;

    DEBUG2("[WRITE] blocks = %d; first_block = %d\n", blocks, first_block);

    if (! disk->mmc_connected)
    {
        return -ENODEV;
    }

    if (disk->mmc_read_only)
    {
        return -EROFS;
    }

    if ((first_block + blocks) >= disk->mmc_block_count)
    {
        return -EINVAL;
    }

    for (i = 0; (i < blocks) && (ENOERR == err); i++)
    {
#ifdef SIS910_WRITE_SD_THRU_CPU
        err = sd_write_disk_block(disk, buf, first_block + i);
#else
        err = sd_gateway_write_disk_block(disk, buf, first_block + i);
#endif
        buf += MMC_DEFAULT_BLOCK_LENGTH;
    }

    return err;
}

static Cyg_ErrNo
sd_disk_erase(disk_channel* chan, cyg_uint32 blocks, cyg_uint32 first_block)
{
    FUNC_DBG_PTR;
    cyg_mmc_disk_info_t* disk = (cyg_mmc_disk_info_t*) chan->dev_priv;
    cyg_uint32 start = first_block;
    cyg_uint32 end = first_block + blocks;
    cyg_uint16 argh_start, argl_start, argh_end, argl_end;
    cyg_uint16 RCA = disk->RCA;
    cyg_uint32 current_state;
    Cyg_ErrNo err = ENOERR;

    DEBUG2("[ERASE] blocks = %d; first_block = %d\n", blocks, first_block);

    if (! disk->mmc_connected) {
        return -ENODEV;
    }
    if ((first_block + blocks) >= disk->mmc_block_count) {
        DEBUG2(" The block to erase is out of bound.\n");
        return -EINVAL;
    }

    switch (disk->mmc_card_type)
    {
        case CARD_TYPE_SD_1_00:
        case CARD_TYPE_SD_1_10:
        case CARD_TYPE_SD_1_x:
        case CARD_TYPE_SD_2_00:
            // Default block length is specified in the CSD
            // CMD16: SET_BLOCKLEN
            err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH,
                                  CMD_DAT_CONT_R1_RESPONSE);
            if (err != ENOERR)
            {
                return -EIO;
            }
            argh_start = (start * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
            argl_start = (start * MMC_DEFAULT_BLOCK_LENGTH);
            argh_end = (end * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
            argl_end = (end * MMC_DEFAULT_BLOCK_LENGTH);
            break;
        case CARD_TYPE_SDHC:
            // SDHC
            argh_start = start >> 16;
            argl_start = start;
            argh_end = end >> 16;
            argl_end = end;
            break;
        case CARD_TYPE_UNUSED:
        default:
            return -EINVAL;
    }

//    if ((disk->mmc_card_type == CARD_TYPE_SD_1_x) || (disk->mmc_card_type == CARD_TYPE_SD_2_00))
//    {
//        // Default block length is specified in the CSD
//        // CMD16: SET_BLOCKLEN
//        err = sd_send_command(MMC_REQUEST_SET_BLOCKLEN, 0, MMC_DEFAULT_BLOCK_LENGTH,
//                              CMD_DAT_CONT_R1_RESPONSE);
//        if (err != ENOERR)
//        {
//            return -EIO;
//        }
//        argh_start = (start * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
//        argl_start = (start * MMC_DEFAULT_BLOCK_LENGTH);
//        argh_end = (end * MMC_DEFAULT_BLOCK_LENGTH) >> 16;
//        argl_end = (end * MMC_DEFAULT_BLOCK_LENGTH);
//    }
//    else
//    {   // SDHC
//        argh_start = start >> 16;
//        argl_start = start;
//        argh_end = end >> 16;
//        argl_end = end;
//    }

    // Set ERASE start
    err = sd_send_command(SD_REQUEST_ERASE_WR_BLK_START_ADDR, argh_start, argl_start, CMD_DAT_CONT_R1_RESPONSE);
    if (err == ENOERR)
    {
        DEBUG2("Erase start set.\n");
    }
    else
    {
        DEBUG2("Cannot set erase start.\n");
        return -EIO;
    }

    // Set ERASE end
    err = sd_send_command(SD_REQUEST_ERASE_WR_BLK_END_ADDR, argh_end, argl_end, CMD_DAT_CONT_R1_RESPONSE);
    if (err == ENOERR)
    {
        DEBUG2("Erase end set.\n");
    }
    else
    {
        DEBUG2("Cannot set erase end.\n");
        return -EIO;
    }

    // ERASE the blocks
    err = sd_send_command(SD_REQUEST_ERASE, 0, 0, CMD_DAT_CONT_R1_RESPONSE);
    if (err == ENOERR)
    {
        DEBUG2("Erase end set.\n");
    }
    else
    {
        DEBUG2("Cannot set erase end.\n");
        return -EIO;
    }

    // Wait SD card go back to tran state
    do {
        cyg_uint32 card_status;
        cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;

        err = sd_send_command(MMC_REQUEST_SEND_STATUS, RCA, 0x0, CMD_DAT_CONT_R1_RESPONSE);
        if (err == ENOERR)
        {
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
            card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
            current_state = card_status >> 9;
            DEBUG2(" current_state = %x\n", current_state);
        }
        else
        {
            return -EIO;
        }
    } while (current_state != 4);

    return err;
}

//static Cyg_ErrNo
//sd_disk_format(disk_channel* chan, cyg_bool quick_format)
//{
//    FUNC_DBG_PTR;
//    cyg_mmc_disk_info_t* disk = (cyg_mmc_disk_info_t*) chan->dev_priv;
//    Cyg_ErrNo err = ENOERR;
//
//    DEBUG2("[FORMAT]\n",);
//    diag_printf("[FORMAT]\n");
//
//    if (! disk->mmc_connected) {
//        return -ENODEV;
//    }
//
//    return err;
//}

// get_config() and set_config(). There are no supported get_config() operations
// at this time.
static Cyg_ErrNo
sd_disk_get_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    FUNC_DBG_PTR;

    CYG_UNUSED_PARAM(disk_channel*, chan);
    CYG_UNUSED_PARAM(cyg_uint32, key);
    CYG_UNUSED_PARAM(const void*, buf);
    CYG_UNUSED_PARAM(cyg_uint32*, len);

    return -EINVAL;
}

static Cyg_ErrNo
sd_disk_set_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    FUNC_DBG_PTR;
    Cyg_ErrNo result = ENOERR;
    cyg_mmc_disk_info_t* disk = (cyg_mmc_disk_info_t*) chan->dev_priv;

    switch(key) {
        case CYG_IO_SET_CONFIG_DISK_MOUNT:
            // There will have been a successful lookup(), so there's
            // little point in checking the disk again.
            break;

        case CYG_IO_SET_CONFIG_DISK_UMOUNT:
            if (0 == chan->info->mounts) {
                // If this is the last unmount of the card, mark it as
                // disconnected. If the user then removes the card and
                // plugs in a new one everything works cleanly. Also
                // reset the SPI device's clockrate.
                disk->mmc_connected = false;
                //mmc_spi_restore_baud(disk);
                result = (chan->callbacks->disk_disconnected)(chan);
            }
            break;
        case SD_CONFIG_DISK_ERASE:
            if (*len == sizeof(cyg_mmc_disk_config_priv_t))
            {
                cyg_mmc_disk_config_priv_t* config_priv = (cyg_mmc_disk_config_priv_t *)buf;
                cyg_uint32 erase_start = config_priv->erase_disk_start;
                cyg_uint32 erase_end = config_priv->erase_disk_end;
                cyg_uint32 erase_len = erase_end - erase_start;

                result = sd_disk_erase(chan, erase_len, erase_start);
            }
            break;
    }

    return result;
}

#ifdef ALIGN_USE_FUNC_PTR
void*
cyg_sd_buf_alloc(cyg_uint32 sizes)
{
    FUNC_DBG_PTR;

    return malloc_align(sizes, 32); // 32 bytes alignment
}
#endif
// ----------------------------------------------------------------------------
// And finally the data structures that define this disk. Some of this
// should be moved into an exported header file so that applications can
// define additional disks.
//
// It is not obvious why there are quite so many structures. Apart
// from the devtab entries there are no tables involved, so there is
// no need to keep everything the same size. The cyg_disk_info_t could
// be the common part of a h/w info_t. The channel structure is
// redundant and its fields could be merged into the cyg_disk_info_t
// structure. That would leave a devtab entry, a disk info structure
// (h/w specific but with a common base), and a disk controller
// structure (ditto).

DISK_FUNS(cyg_mmc_disk_funs,
    sd_disk_read,
    sd_disk_write,
    sd_disk_get_config,
    sd_disk_set_config
    );

static cyg_mmc_device   cyg_mmc_dev = {
    .mmc_gateway_xfer = 1,
};

static cyg_mmc_disk_info_t cyg_mmc_disk_hwinfo = {
    .mmc_dev        = &cyg_mmc_dev,
};

// No h/w controller structure is needed, but the address of the
// second argument is taken anyway.
DISK_CONTROLLER(cyg_mmc_disk_controller, cyg_mmc_disk_hwinfo);

DISK_CHANNEL(cyg_mmc_disk_channel,
             cyg_mmc_disk_funs,
             cyg_mmc_disk_hwinfo,
             cyg_mmc_disk_controller,
             true,                            /* MBR support */
             1                                /* Number of partitions supported */
             );

//BLOCK_DEVTAB_ENTRY(cyg_mmc_disk_devtab_entry,
//                   CYGDAT_DEVS_DISK_MMC_DISK_NAME,
//                   0,
//                   &cyg_io_disk_devio,
//                   &sd_disk_init,
//                   &sd_disk_lookup,
//                   &cyg_mmc_disk_channel);
#ifdef ALIGN_USE_FUNC_PTR
BLOCK_DEVTAB_ENTRY(cyg_mmc_disk_devtab_entry,
                   CYGDAT_DEVS_DISK_SD_DISK_NAME,
                   0,
                   &cyg_io_disk_devio,
                   &sd_disk_init,
                   &sd_disk_lookup,
                   &cyg_mmc_disk_channel,
                   &cyg_sd_buf_alloc);
#else
BLOCK_DEVTAB_ENTRY(cyg_mmc_disk_devtab_entry,
                   CYGDAT_DEVS_DISK_SD_DISK_NAME,
                   0,
                   &cyg_io_disk_devio,
                   &sd_disk_init,
                   &sd_disk_lookup,
                   &cyg_mmc_disk_channel);
#endif


// EOF sd_sis910.c
