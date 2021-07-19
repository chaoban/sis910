#ifndef CYGONCE_DEVS_DISK_SD_PROTOCOL_H
#define CYGONCE_DEVS_DISK_SD_PROTOCOL_H
//==========================================================================
//
//      sd_protocol.h
//
//      Define the protocol used for interacting with SD cards
//
//===========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2005 eCosCentric Limited
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author:       Sidney Shih
// Date:         2008-03-18
// Usage:        Only this package and implementing device drivers should
//               include this header file.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/cyg_type.h>       /* Common types */

/* SD commands                                        type  argument     response */
  /* class 0 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_REQUEST_SEND_RELATIVE_ADDR          3   /* bcr                     R6  */
#define SD_REQUEST_SEND_IF_COND                8   /* bcr  [11:0] See below   R7  */

  /* class 5 */
#define SD_REQUEST_ERASE_WR_BLK_START_ADDR    32   /* ac   [31:0] data addr   R1  */
#define SD_REQUEST_ERASE_WR_BLK_END_ADDR      33   /* ac   [31:0] data addr   R1  */
#define SD_REQUEST_ERASE                      38   /* ac                      R1b */

  /* class 10 */
#define SD_REQUEST_SWITCH                      6   /* adtc [31:0] See below   R1  */

  /* Application commands */
#define SD_REQUEST_APP_SET_BUS_WIDTH           6   /* ac   [1:0] bus width    R1  */
#define SD_REQUEST_APP_SD_STATUS              13   /* adtc                    R1  */
#define SD_REQUEST_APP_SEND_NUM_WR_BLKS       22   /* adtc                    R1  */
#define SD_REQUEST_APP_SET_WR_BLK_ERASE_COUNT 23   /* ac   [22:0] # of blks   R1  */
#define SD_REQUEST_APP_OP_COND                41   /* bcr  [31:0] OCR         R3  */
#define SD_REQUEST_APP_SET_CLR_CARD_DETECT    42   /* ac   [0] set_cd         R1  */
#define SD_REQUEST_APP_SEND_SCR               51   /* adtc                    R1  */

/*
 * SD_SWITCH argument format:
 *
 *      [31] Check (0) or switch (1)
 *      [30:24] Reserved (0)
 *      [23:20] Function group 6
 *      [19:16] Function group 5
 *      [15:12] Function group 4
 *      [11:8] Function group 3
 *      [7:4] Function group 2
 *      [3:0] Function group 1
 */

/*
 * SD_SEND_IF_COND argument format:
 *
 *      [31:12] Reserved (0)
 *      [11:8] Host Voltage Supply Flags
 *      [7:0] Check Pattern (0xAA)
 */

// The CID register is generally treated as an opaque data structure
// used only for unique identification of cards.
//typedef struct mmc_cid_register {
//    cyg_uint8   cid_data[16];
//} mmc_cid_register;

#define SD_CID_MID(_data_)                       ((_data_)->cid_data[0])
#define SD_CID_OID(_data_)                       (((_data_)->cid_data[1] << 8)           | \
                                                 ((_data_)->cid_data[2]))
#define SD_CID_PNM(_data_)                       ((const char*)&((_data_)->cid_data[3]))
#define SD_CID_PRV(_data_)                       ((_data_)->cid_data[8])
#define SD_CID_PSN(_data_)                       (((_data_)->cid_data[9] << 24)          | \
                                                 ((_data_)->cid_data[10] << 16)          | \
                                                 ((_data_)->cid_data[11] <<  8)          | \
                                                 ((_data_)->cid_data[12]))
#define SD_CID_MDT(_data_)                       ((((_data_)->cid_data[13] & 0xFF) << 8) | \
                                                 ((_data_)->cid_data[14]))
#define SD_CID_CRC(_data_)                       ((_data_)->cid_data[15] >> 1)
                                                         

// The CSD register is just lots of small bitfields. For now keep it
// as an array of 16 bytes and provide macros to read the fields.
//typedef struct mmc_csd_register {
//    cyg_uint8   csd_data[16];
//} mmc_csd_register;

// SD: CSD Version 1.0
#define SD_CSD_STRUCTURE(_data_)                 (((_data_)->csd_data[0] & 0x00C0) >> 6)
#define SD_CSD_v1_CSD_STRUCTURE(_data_)          SD_CSD_STRUCTURE(_data_)
#define SD_CSD_v1_TAAC_MANTISSA(_data_)          (((_data_)->csd_data[1] & 0x0078) >> 3)
#define SD_CSD_v1_TAAC_EXPONENT(_data_)          ((_data_)->csd_data[1] & 0x0007)
#define SD_CSD_v1_NSAC(_data_)                   ((_data_)->csd_data[2])
#define SD_CSD_v1_TRAN_SPEED_MANTISSA(_data_)    (((_data_)->csd_data[3] & 0x0078) >> 3)
#define SD_CSD_v1_TRAN_SPEED_EXPONENT(_data_)    ((_data_)->csd_data[3] & 0x0007)
#define SD_CSD_v1_CCC(_data_)                    (((_data_)->csd_data[4] << 4)             | \
                                                 (((_data_)->csd_data[5] & 0x00F0) >> 4))
#define SD_CSD_v1_READ_BL_LEN(_data_)            ((_data_)->csd_data[5] & 0x000F)
#define SD_CSD_v1_READ_BL_PARTIAL(_data_)        (((_data_)->csd_data[6] & 0x0080) >> 7)
#define SD_CSD_v1_WRITE_BLK_MISALIGN(_data_)     (((_data_)->csd_data[6] & 0x0040) >> 6)
#define SD_CSD_v1_READ_BLK_MISALIGN(_data_)      (((_data_)->csd_data[6] & 0x0020) >> 5)
#define SD_CSD_v1_DSR_IMP(_data_)                (((_data_)->csd_data[6] & 0x0010) >> 4)
#define SD_CSD_v1_C_SIZE(_data_)                 ((((_data_)->csd_data[6] & 0x0003) << 10) | \
                                                 ((_data_)->csd_data[7] << 2)              | \
                                                 (((_data_)->csd_data[8] & 0x00C0) >> 6))
#define SD_CSD_v1_VDD_R_CURR_MIN(_data_)         (((_data_)->csd_data[8] & 0x0038)  >> 3)
#define SD_CSD_v1_VDD_R_CURR_MAX(_data_)         ((_data_)->csd_data[8] & 0x0007)
#define SD_CSD_v1_VDD_W_CURR_MIN(_data_)         (((_data_)->csd_data[9] & 0x00E0) >> 5)
#define SD_CSD_v1_VDD_W_CURR_MAX(_data_)         (((_data_)->csd_data[9] & 0x001C) >> 2)
#define SD_CSD_v1_C_SIZE_MULT(_data_)            ((((_data_)->csd_data[9] & 0x0003) << 1)  | \
                                                 (((_data_)->csd_data[10] & 0x0080) >> 7))
#define SD_CSD_v1_ERASE_BLK_EN(_data_)           (((_data_)->csd_data[10] & 0x0040) >> 6)
#define SD_CSD_v1_SECTOR_SIZE(_data_)            ((((_data_)->csd_data[10] & 0x003F) << 1) | \
                                                 (((_data_)->csd_data[11] & 0x0080) >> 7))
#define SD_CSD_v1_WR_GRP_SIZE(_data_)            ((_data_)->csd_data[11] & 0x007F)
#define SD_CSD_v1_WR_GRP_ENABLE(_data_)          (((_data_)->csd_data[12] & 0x0080) >> 7)
#define SD_CSD_v1_R2W_FACTOR(_data_)             (((_data_)->csd_data[12] & 0x001C) >> 2)
#define SD_CSD_v1_WRITE_BL_LEN(_data_)           ((((_data_)->csd_data[12] & 0x0003) << 2) | \
                                                 (((_data_)->csd_data[13] & 0x00C0) >> 6))
#define SD_CSD_v1_WR_BL_PARTIAL(_data_)          (((_data_)->csd_data[13] & 0x0020) >> 5)
#define SD_CSD_v1_FILE_FORMAT_GROUP(_data_)      (((_data_)->csd_data[14] & 0x0080) >> 7)
#define SD_CSD_v1_COPY(_data_)                   (((_data_)->csd_data[14] & 0x0040) >> 6)
#define SD_CSD_v1_PERM_WRITE_PROTECT(_data_)     (((_data_)->csd_data[14] & 0x0020) >> 5)
#define SD_CSD_v1_TMP_WRITE_PROTECT(_data_)      (((_data_)->csd_data[14] & 0x0010) >> 4)
#define SD_CSD_v1_FILE_FORMAT(_data_)            (((_data_)->csd_data[14] & 0x000C) >> 2)
#define SD_CSD_v1_CRC(_data_)                    (((_data_)->csd_data[15] & 0xFE) >> 1)

// SD: CSD Version 2.0
#define SD_CSD_v2_CSD_STRUCTURE(_data_)          SD_CSD_STRUCTURE(_data_) 
#define SD_CSD_v2_TAAC_MANTISSA(_data_)          (((_data_)->csd_data[1] & 0x0078) >> 3)
#define SD_CSD_v2_TAAC_EXPONENT(_data_)          ((_data_)->csd_data[1] & 0x0007)
#define SD_CSD_v2_NSAC(_data_)                   ((_data_)->csd_data[2])
#define SD_CSD_v2_TRAN_SPEED_MANTISSA(_data_)    (((_data_)->csd_data[3] & 0x0078) >> 3)
#define SD_CSD_v2_TRAN_SPEED_EXPONENT(_data_)    ((_data_)->csd_data[3] & 0x0007)
#define SD_CSD_v2_CCC(_data_)                    (((_data_)->csd_data[4] << 4)             | \
                                                 (((_data_)->csd_data[5] & 0x00F0) >> 4))
#define SD_CSD_v2_READ_BL_LEN(_data_)            ((_data_)->csd_data[5] & 0x000F)
#define SD_CSD_v2_READ_BL_PARTIAL(_data_)        (((_data_)->csd_data[6] & 0x0080) >> 7)
#define SD_CSD_v2_WRITE_BLK_MISALIGN(_data_)     (((_data_)->csd_data[6] & 0x0040) >> 6)
#define SD_CSD_v2_READ_BLK_MISALIGN(_data_)      (((_data_)->csd_data[6] & 0x0020) >> 5)
#define SD_CSD_v2_DSR_IMP(_data_)                (((_data_)->csd_data[6] & 0x0010) >> 4)
#define SD_CSD_v2_C_SIZE(_data_)                 ((((_data_)->csd_data[7] & 0x003F) << 16) | \
                                                 ((_data_)->csd_data[8] << 8)              | \
                                                 (_data_)->csd_data[9])
#define SD_CSD_v2_ERASE_BLK_EN(_data_)           (((_data_)->csd_data[10] & 0x0040) >> 6)
#define SD_CSD_v2_SECTOR_SIZE(_data_)            ((((_data_)->csd_data[10] & 0x003F) << 1) | \
                                                 (((_data_)->csd_data[11] & 0x0080) >> 7))
#define SD_CSD_v2_WR_GRP_SIZE(_data_)            ((_data_)->csd_data[11] & 0x007F)
#define SD_CSD_v2_WR_GRP_ENABLE(_data_)          (((_data_)->csd_data[12] & 0x0080) >> 7)
#define SD_CSD_v2_R2W_FACTOR(_data_)             (((_data_)->csd_data[12] & 0x001C) >> 2)
#define SD_CSD_v2_WRITE_BL_LEN(_data_)           ((((_data_)->csd_data[12] & 0x0003) << 2) | \
                                                 (((_data_)->csd_data[13] & 0x00C0) >> 6))
#define SD_CSD_v2_WR_BL_PARTIAL(_data_)          (((_data_)->csd_data[13] & 0x0020) >> 5)
#define SD_CSD_v2_FILE_FORMAT_GROUP(_data_)      (((_data_)->csd_data[14] & 0x0080) >> 7)
#define SD_CSD_v2_COPY(_data_)                   (((_data_)->csd_data[14] & 0x0040) >> 6)
#define SD_CSD_v2_PERM_WRITE_PROTECT(_data_)     (((_data_)->csd_data[14] & 0x0020) >> 5)
#define SD_CSD_v2_TMP_WRITE_PROTECT(_data_)      (((_data_)->csd_data[14] & 0x0010) >> 4)
#define SD_CSD_v2_FILE_FORMAT(_data_)            (((_data_)->csd_data[14] & 0x000C) >> 2)
#define SD_CSD_v2_CRC(_data_)                    (((_data_)->csd_data[15] & 0xFE) >> 1)

#define CARD_TYPE_UNUSED           0
#define CARD_TYPE_SD_CARD          0x20
#define CARD_TYPE_SD_1_00          (0x1 | CARD_TYPE_SD_CARD)
#define CARD_TYPE_SD_1_01          CARD_TYPE_SD_1_00
#define CARD_TYPE_SD_1_10          (0x2 | CARD_TYPE_SD_CARD)
#define CARD_TYPE_SD_1_x           (CARD_TYPE_SD_1_00 | CARD_TYPE_SD_1_10)
#define CARD_TYPE_SD_2_00          (0x4 | CARD_TYPE_SD_CARD)
#define CARD_TYPE_SDHC             (0x8 | CARD_TYPE_SD_CARD)

#endif //  ifdef CYGONCE_DEVS_DISK_SD_PROTOCOL_H

/* EOF sd_protocol.h */
