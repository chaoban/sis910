#ifndef CYGONCE_DEVS_DISK_MMC_PROTOCOL_H
#define CYGONCE_DEVS_DISK_MMC_PROTOCOL_H
//==========================================================================
//
//      mmc_protocol.h
//
//      Define the protocol used for interacting with MMC cards
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
// Author:       bartv
// Contributors: jlarmour
// Date:         2004-04-25
// Usage:        Only this package and implementing device drivers should
//               include this header file.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/cyg_type.h>       /* Common types */

// The MMC command set. A request is a six-byte sequence. First a
// single command byte, one of the following with bit 6 (0x40) or'd in
// and bit 7 clear as a start bit. Then a four-byte argument, usually
// a 32-bit integer most significant byte first. Finally a 7-bit CRC
// and a stop bit. In SPI mode the CRC is usually optional, except for
// the GO_IDLE_STATE command.
//
// Commands ALL_SEND_CID, SEND_RELATIVE_ADDR, SET_DSR,
// SELECT_DESELECT, READ_DATA_UNTIL_STOP, STOP_TRANSMISSION,
// GO_INACTIVE, READ_MULTIPLE_BLOCK, WRITE_DATA_UNTIL_STOP,
// WRITE_MULTIPLE_BLOCK, and PROGRAM_CID are MMC only, and not
// available in SPI mode.
//
// Device drivers may use values with bit 7 set to indicate
// driver-specific commands.

/* Standard MMC commands (4.2)                   type  argument     response */
   /* class 1 */
#define MMC_REQUEST_GO_IDLE_STATE         0   /* bc                          */
#define MMC_REQUEST_SEND_OP_COND          1   /* bcr  [31:0] OCR         R3  */
#define MMC_REQUEST_ALL_SEND_CID          2   /* bcr                     R2  */
#define MMC_REQUEST_SET_RELATIVE_ADDR     3   /* ac   [31:16] RCA        R1  */
#define MMC_REQUEST_SET_DSR               4   /* bc   [31:16] RCA            */
#define MMC_REQUEST_SWITCH                6   /* ac   [31:0] See below   R1b */
#define MMC_REQUEST_SELECT_DESELECT       7   /* ac   [31:16] RCA        R1  */
#define MMC_REQUEST_SEND_EXT_CSD          8   /* adtc                    R1  */
#define MMC_REQUEST_SEND_CSD              9   /* ac   [31:16] RCA        R2  */
#define MMC_REQUEST_SEND_CID             10   /* ac   [31:16] RCA        R2  */
#define MMC_REQUEST_READ_DATA_UNTIL_STOP 11   /* adtc [31:0] dadr        R1  */
#define MMC_REQUEST_STOP_TRANSMISSION    12   /* ac                      R1b */
#define MMC_REQUEST_SEND_STATUS          13   /* ac   [31:16] RCA        R1  */
#define MMC_REQUEST_GO_INACTIVE          15   /* ac   [31:16] RCA            */
#define MMC_REQUEST_READ_OCR             58   /* spi                  spi_R3 */
#define MMC_REQUEST_CRC_ON_OFF           59   /* spi  [0:0] flag      spi_R1 */

  /* class 2 */
#define MMC_REQUEST_SET_BLOCKLEN         16   /* ac   [31:0] block len   R1  */
#define MMC_REQUEST_READ_SINGLE_BLOCK    17   /* adtc [31:0] data addr   R1  */
#define MMC_REQUEST_READ_MULTIPLE_BLOCK  18   /* adtc [31:0] data addr   R1  */

  /* class 3 */
#define MMC_REQUEST_WRITE_DAT_UNTIL_STOP 20   /* adtc [31:0] data addr   R1  */

  /* class 4 */
#define MMC_REQUEST_SET_BLOCK_COUNT      23   /* ac   [31:16] always 0   R1  */
                                              /*      [15:0] # of blks       */        
#define MMC_REQUEST_WRITE_BLOCK          24   /* adtc [31:0] data addr   R1  */
#define MMC_REQUEST_WRITE_MULTIPLE_BLOCK 25   /* adtc                    R1  */
#define MMC_REQUEST_PROGRAM_CID          26   /* adtc                    R1  */
#define MMC_REQUEST_PROGRAM_CSD          27   /* adtc                    R1  */

  /* class 6 */
#define MMC_REQUEST_SET_WRITE_PROT       28   /* ac   [31:0] data addr   R1b */
#define MMC_REQUEST_CLR_WRITE_PROT       29   /* ac   [31:0] data addr   R1b */
#define MMC_REQUEST_SEND_WRITE_PROT      30   /* adtc [31:0] wpdata addr R1  */

  /* class 5 */
#define MMC_REQUEST_ERASE_GROUP_START    35   /* ac   [31:0] data addr   R1  */
#define MMC_REQUEST_ERASE_GROUP_END      36   /* ac   [31:0] data addr   R1  */
#define MMC_REQUEST_ERASE                38   /* ac                      R1b */

  /* class 9 */
#define MMC_REQUEST_FAST_IO              39   /* ac   <Complex>          R4  */
#define MMC_REQUEST_GO_IRQ_STATE         40   /* bcr                     R5  */

  /* class 7 */
#define MMC_REQUEST_LOCK_UNLOCK          42   /* adtc                    R1b */

  /* class 8 */
#define MMC_REQUEST_APP_CMD              55   /* ac   [31:16] RCA        R1  */
#define MMC_REQUEST_GEN_CMD              56   /* adtc [0] RD/WR          R1  */

/*
 * MMC_SWITCH argument format:
 *
 *      [31:26] Always 0
 *      [25:24] Access Mode
 *      [23:16] Location of target Byte in EXT_CSD
 *      [15:08] Value Byte
 *      [07:03] Always 0
 *      [02:00] Command Set
 */

// ----------------------------------------------------------------------------
// SPI-specific parts of the MMC protocol.
//
// The main response byte. 0 indicates success, other bits
// indicate various error conditions.
/*
 * MMC/SD in SPI mode reports R1 status always, and R2 for SEND_STATUS
 * R1 is the low order byte; R2 is the next highest byte, when present.
 */
#define R1_SPI_SUCCESS                   0x0
#define R1_SPI_IN_IDLE_STATE             (1 << 0)
#define R1_SPI_ERASE_RESET               (1 << 1)
#define R1_SPI_ILLEGAL_COMMAND           (1 << 2)
#define R1_SPI_COM_CRC_ERROR             (1 << 3)
#define R1_SPI_ERASE_SEQUENCE_ERROR      (1 << 4)
#define R1_SPI_ADDRESS_ERROR             (1 << 5)
#define R1_SPI_PARAMETER_ERROR           (1 << 6)
/* R1 bit 7 is always zero */
// A send-status command generates a second response byte
#define R2_SPI_CARD_LOCKED              (1 << 0)
#define R2_SPI_WP_ERASE_SKIP            (1 << 1)
#define R2_SPI_LOCK_UNLOCK_FAILED       R2_SPI_WP_ERASE_SKIP
#define R2_SPI_ERROR                    (1 << 2)
#define R2_SPI_CC_ERROR                 (1 << 3)
#define R2_SPI_CARD_ECC_FAILED          (1 << 4)
#define R2_SPI_WP_VIOLATION             (1 << 5)
#define R2_SPI_ERASE_PARAM              (1 << 6)
#define R2_SPI_OUT_OF_RANGE             (1 << 7)
#define R2_SPI_CSD_OVERWRITE            R2_SPI_OUT_OF_RANGE

/*
  MMC status in R1, for native mode (SPI bits are different)
  Type
    e : error bit
    s : status bit
    r : detected and set for the actual command response
    x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
    a : according to the card state
    b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
    c : clear by read
 */

#define R1_OUT_OF_RANGE         (1 << 31)   /* er, c */
#define R1_ADDRESS_ERROR        (1 << 30)   /* erx, c */
#define R1_BLOCK_LEN_ERROR      (1 << 29)   /* er, c */
#define R1_ERASE_SEQ_ERROR      (1 << 28)   /* er, c */
#define R1_ERASE_PARAM          (1 << 27)   /* ex, c */
#define R1_WP_VIOLATION         (1 << 26)   /* erx, c */
#define R1_CARD_IS_LOCKED       (1 << 25)   /* sx, a */
#define R1_LOCK_UNLOCK_FAILED   (1 << 24)   /* erx, c */
#define R1_COM_CRC_ERROR        (1 << 23)   /* er, b */
#define R1_ILLEGAL_COMMAND      (1 << 22)   /* er, b */
#define R1_CARD_ECC_FAILED      (1 << 21)   /* ex, c */
#define R1_CC_ERROR             (1 << 20)   /* erx, c */
#define R1_ERROR                (1 << 19)   /* erx, c */
#define R1_UNDERRUN             (1 << 18)   /* ex, c */
#define R1_OVERRUN              (1 << 17)   /* ex, c */
#define R1_CID_CSD_OVERWRITE    (1 << 16)   /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP        (1 << 15)   /* sx, c */
#define R1_CARD_ECC_DISABLED    (1 << 14)   /* sx, a */
#define R1_ERASE_RESET          (1 << 13)   /* sr, c */
#define R1_STATUS(x)            (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)     ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA       (1 << 8)    /* sx, a */
#define R1_APP_CMD              (1 << 5)    /* sr, c */

// The CID register is generally treated as an opaque data structure
// used only for unique identification of cards.
typedef struct mmc_cid_register {
    cyg_uint8   cid_data[16];
} mmc_cid_register;

#define MMC_CID_REGISTER_MID(_data_)                    ((_data_)->cid_data[0])
#define MMC_CID_REGISTER_OID(_data_)                    (((_data_)->cid_data[1] << 8) | \
                                                         ((_data_)->cid_data[2]))
#define MMC_CID_REGISTER_PNM(_data_)                    ((const char*)&((_data_)->cid_data[3]))
#define MMC_CID_REGISTER_PRV(_data_)                    ((_data_)->cid_data[9])
#define MMC_CID_REGISTER_PSN(_data_)                    (((_data_)->cid_data[10] << 24) | \
                                                         ((_data_)->cid_data[11] << 16) | \
                                                         ((_data_)->cid_data[12] <<  8) | \
                                                         ((_data_)->cid_data[13]))
#define MMC_CID_REGISTER_MDT(_data_)                    ((_data_)->cid_data[14])
#define MMC_CID_REGISTER_CRC(_data_)                    ((_data_)->cid_data[15] >> 1)
                                                         

// The CSD register is just lots of small bitfields. For now keep it
// as an array of 16 bytes and provide macros to read the fields.
typedef struct mmc_csd_register {
    cyg_uint8   csd_data[16];
} mmc_csd_register;

// Beyond MMC 4.x
typedef struct mmc_ext_csd_register {
    cyg_uint8   ext_csd_data[512];
} mmc_ext_csd_register;

#define MMC_CSD_REGISTER_CSD_STRUCTURE(_data_)          (((_data_)->csd_data[0] & 0x00C0) >> 6)
#define MMC_CSD_REGISTER_SPEC_VERS(_data_)              (((_data_)->csd_data[0] & 0x003C) >> 2)
#define MMC_CSD_REGISTER_TAAC_MANTISSA(_data_)          (((_data_)->csd_data[1] & 0x0078) >> 3)
#define MMC_CSD_REGISTER_TAAC_EXPONENT(_data_)          ((_data_)->csd_data[1] & 0x0007)
#define MMC_CSD_REGISTER_NSAC(_data_)                   ((_data_)->csd_data[2])
#define MMC_CSD_REGISTER_TRAN_SPEED_MANTISSA(_data_)    (((_data_)->csd_data[3] & 0x0078) >> 3)
#define MMC_CSD_REGISTER_TRAN_SPEED_EXPONENT(_data_)    ((_data_)->csd_data[3] & 0x0007)
#define MMC_CSD_REGISTER_CCC(_data_)                    (((_data_)->csd_data[4] << 4) | (((_data_)->csd_data[5] & 0x00F0) >> 4))
#define MMC_CSD_REGISTER_READ_BL_LEN(_data_)            ((_data_)->csd_data[5] & 0x000F)
#define MMC_CSD_REGISTER_READ_BL_PARTIAL(_data_)        (((_data_)->csd_data[6] & 0x0080) >> 7)
#define MMC_CSD_REGISTER_WRITE_BLK_MISALIGN(_data_)     (((_data_)->csd_data[6] & 0x0040) >> 6)
#define MMC_CSD_REGISTER_READ_BLK_MISALIGN(_data_)      (((_data_)->csd_data[6] & 0x0020) >> 5)
#define MMC_CSD_REGISTER_DSR_IMP(_data_)                (((_data_)->csd_data[6] & 0x0010) >> 4)
#define MMC_CSD_REGISTER_C_SIZE(_data_)                 ((((_data_)->csd_data[6] & 0x0003) << 10) |     \
                                                          ((_data_)->csd_data[7] << 2)            |     \
                                                          (((_data_)->csd_data[8] & 0x00C0) >> 6))
#define MMC_CSD_REGISTER_VDD_R_CURR_MIN(_data_)         (((_data_)->csd_data[8] & 0x0038)  >> 3)
#define MMC_CSD_REGISTER_VDD_R_CURR_MAX(_data_)         ((_data_)->csd_data[8] & 0x0007)
#define MMC_CSD_REGISTER_VDD_W_CURR_MIN(_data_)         (((_data_)->csd_data[9] & 0x00E0) >> 5)
#define MMC_CSD_REGISTER_VDD_W_CURR_MAX(_data_)         (((_data_)->csd_data[9] & 0x001C) >> 2)
#define MMC_CSD_REGISTER_C_SIZE_MULT(_data_)            ((((_data_)->csd_data[9] & 0x0003) << 1) |      \
                                                         (((_data_)->csd_data[10] & 0x0080) >> 7))
#define MMC_CSD_REGISTER_SECTOR_SIZE(_data_)            (((_data_)->csd_data[10] & 0x007C) >> 2)
#define MMC_CSD_REGISTER_ERASE_GRP_SIZE(_data_)         ((((_data_)->csd_data[10] & 0x0003) << 3) |     \
                                                         (((_data_)->csd_data[11] & 0x00E0) >> 5))
#define MMC_CSD_REGISTER_WR_GRP_SIZE(_data_)            ((_data_)->csd_data[11] & 0x001F)
#define MMC_CSD_REGISTER_WR_GRP_ENABLE(_data_)          (((_data_)->csd_data[12] & 0x0080) >> 7)
#define MMC_CSD_REGISTER_DEFAULT_ECC(_data_)            (((_data_)->csd_data[12] & 0x0060) >> 5)
#define MMC_CSD_REGISTER_R2W_FACTOR(_data_)             (((_data_)->csd_data[12] & 0x001C) >> 2)
#define MMC_CSD_REGISTER_WRITE_BL_LEN(_data_)           ((((_data_)->csd_data[12] & 0x0003) << 2) |     \
                                                         (((_data_)->csd_data[13] & 0x00C0) >> 6))
#define MMC_CSD_REGISTER_WR_BL_PARTIAL(_data_)          (((_data_)->csd_data[13] & 0x0020) >> 5)
#define MMC_CSD_REGISTER_FILE_FORMAT_GROUP(_data_)      (((_data_)->csd_data[14] & 0x0080) >> 7)
#define MMC_CSD_REGISTER_COPY(_data_)                   (((_data_)->csd_data[14] & 0x0040) >> 6)
#define MMC_CSD_REGISTER_PERM_WRITE_PROTECT(_data_)     (((_data_)->csd_data[14] & 0x0020) >> 5)
#define MMC_CSD_REGISTER_TMP_WRITE_PROTECT(_data_)      (((_data_)->csd_data[14] & 0x0010) >> 4)
#define MMC_CSD_REGISTER_FILE_FORMAT(_data_)            (((_data_)->csd_data[14] & 0x000C) >> 2)
#define MMC_CSD_REGISTER_ECC(_data_)                    ((_data_)->csd_data[14] & 0x0003)
#define MMC_CSD_REGISTER_CRC(_data_)                    (((_data_)->csd_data[15] & 0xFE) >> 1)

#define CARD_TYPE_MMC_CARD         0x10
#define CARD_TYPE_MMC_3_31         (0x1 | CARD_TYPE_MMC_CARD)
#define CARD_TYPE_MMC_3_x          CARD_TYPE_MMC_3_31
#define CARD_TYPE_MMC_4_0          (0x2 | CARD_TYPE_MMC_CARD)
#define CARD_TYPE_MMC_HS           CARD_TYPE_MMC_4_0
#define CARD_TYPE_MMC_4_x          CARD_TYPE_MMC_4_0
#define CARD_TYPE_MMC_4_1          CARD_TYPE_MMC_4_0
#define CARD_TYPE_MMC_4_2          (0x4 | CARD_TYPE_MMC_HS)
#define CARD_TYPE_MMC_HC           CARD_TYPE_MMC_4_2

#endif //  ifdef CYGONCE_DEVS_DISK_MMC_PROTOCOL_H

/* EOF mmc_protocol.h */
