#ifndef CYGONCE_DEVS_DISK_SD_SIS910_H
#define CYGONCE_DEVS_DISK_SD_SIS910_H
//==========================================================================
//
//      sd_sis910.h
//
//      Define the SiS 910 SD/MMC H/W register settings
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
// Date:         2008-05-29
//
//####DESCRIPTIONEND####
//==========================================================================
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>

// SiS 910 SD/MMC register table
#define REG_SIS910_SD_STR_STP_CLK              0x90003400
#define REG_SIS910_SD_STATUS                   0x90003404
#define REG_SIS910_SD_CLK_RATE                 0x90003408
#define REG_SIS910_SD_SPI_REG                  0x9000340C
#define REG_SIS910_SD_CMD_DAT                  0x90003410
#define REG_SIS910_SD_RES_TO                   0x90003414
#define REG_SIS910_SD_RCVD_TO                  0x90003418
#define REG_SIS910_SD_BLK_LEN                  0x9000341C
#define REG_SIS910_SD_NO_BLKS                  0x90003420
#define REG_SIS910_SD_INT_MASK                 0x90003424
#define REG_SIS910_SD_CMD_0                    0x90003428
#define REG_SIS910_SD_CMD_1                    0x9000342C
#define REG_SIS910_SD_CMD_2                    0x90003430
#define REG_SIS910_SD_RES_REG                  0x90003434
#define REG_SIS910_SD_DATA_BUFFERS             0x90003438
#define REG_SIS910_SD_BUF_RDY                  0x9000343C
#define REG_SIS910_SD_SWAPCTL                  0x90003478
#define REG_SIS910_SD_IRQCLR                   0x9000347C

//SD_STATUS			(0x90003404)		    	 (13 bits WR/RD)
// -------------------------------------------------------------------------------
//|    |    |   |end |prg |data|wrcrc|clk |full|emty|resp|spird|crc|crc |time|time|
//| x  | x  | x |cmd |done|tran|error|dis |fifo|fifo|crc |err  |rd |wr  |out |out |
//|    |    |   |resp|    |done|code |able|    |    |err |token|err|err |resp|read|
// -------------------------------------------------------------------------------
// b15                 b11                  b7                  b3             b0
//
// bit[0]    - time out read data
#define STATUS_READ_DATA_TIMEOUT        0x0001
// bit[1]    - time out response
#define STATUS_RESPONSE_TIMEOUT         0x0002
// bit[2]    - CRC write error
#define STATUS_CRC_WRITE_ERROR          0x0004
// bit[3]    - CRC read error
#define STATUS_CRC_READ_ERROR           0x0008
// bit[4]    - SPI data error token has been received
#define STATUS_SPI_DATA_ERROR           0x0010
// bit[5]    - response CRC error
#define STATUS_RESPONSE_CRC_ERROR       0x0020
// bit[6]    - Write Buffer empty (ready for Application write operation)
#define STATUS_WRITE_BUFFER_EMPTY       0x0040
// bit[7]    - Read Buffer full (ready for Application read)
#define STATUS_READ_BUFFER_FULL         0x0080
// bit[8]    - SD Memory Card clock stopped (at high level 1)
#define STATUS_CLOCK_DISABLE            0x0100
// bit[10:9] - valid only if B5 is high
//             0: CRC (Data Response Status = 101)
//	     1: WR_ERR (Data Response Status = 110)
//	     2: Illegal Data Response Status bits
#define STATUS_RESPONSE_CODE_CRC        0x0000
#define STATUS_RESPONSE_CODE_WR_ERR     0x0200
#define STATUS_RESPONSE_CODE_ILLEGAL    0x0400
// bit[11]   - In write and read operations, this bit indicates that the data
//             transfer is finished.
#define STATUS_DATA_TRANSFER_DONE       0x0800
// bit[12]   - Busy signal ended or deasserted from the SD Memory Card bus
#define STATUS_BUSY_SIGNAL_END          0x1000
#define STATUS_END_CMD_RESPONSE         0x2000

//SD_CMD_DAT			(0x90003410)		    	  (12 bits WR/RD)
// -------------------------------------------------------------------------------
//|    |    |    |nob |we  |wide| no |    |busy|    |mul |strm| wr/|dat |form|form|
//| x  | x  | x  |on  |wide|bus | cmd|init|bit | x  |blk |blk | rd |en  |resp|resp|
//|    |    |    |    |bus |    |    |    |    |    |    |    |    |    |[1] |[0] |
// -------------------------------------------------------------------------------
// b15                 b11                  b7                  b3             b0
//
// Writing to this register starts a transaction in the SD Memory Card Adapter
// unless 'no command' is used. Therefore it should be the last register updated.
//
// bit[1:0]  - Response formats are described in the next table:
//
// 		Description | FORMAT_OF_RES  | FORMAT_OF_RES
//		            | SD Memory Card |   SPI
//                ------------+----------------+------------------
// 		No response | 00	     | Not supported
// 		Format R1   | 01             | 01
// 		Format R2   | 10             | 10
// 		Format R3   | 11             | 11
//
#define CMD_DAT_CONT_NO_RESPONSE        0x0000
#define CMD_DAT_CONT_R1_RESPONSE        0x0001
#define CMD_DAT_CONT_R2_RESPONSE        0x0002
#define CMD_DAT_CONT_R3_RESPONSE        0x0003
// bit[2]    - Specifies whether this command includes a data transfer
//             ('1' = true).
#define CMD_DAT_CONT_DATA_ENABLE        0x0004
// bit[3]    - Specifies whether the data transfer of the current command is a
//             write or read operation ('1' = write, '0' = read)
#define CMD_DAT_CONT_DATA_WRITE         0x0008
#define CMD_DAT_CONT_DATA_READ          0x0000
// bit[4]    - Specifies whether the data transfer of the current command is in
//             stream or block mode ('1' = stream, '0' = block)
#define CMD_DAT_CONT_DATA_STREAM        0x0010
#define CMD_DAT_CONT_DATA_BLOCK         0x0000
// bit[5]    - Specifies whether the data transfer of the current command is a
//             multiple block mode command ('1' = multiple block, '0' = single
//	     block)
#define CMD_DAT_CONT_DATA_MULTIPLE      0x0020
#define CMD_DAT_CONT_DATA_SINGLE        0x0000
// bit[6]    - Reserved
// bit[7]    - Specifies whether a busy signal is expected after the current
//             command. ('1' = true).
#define CMD_DAT_CONT_BUSY_BIT_ON        0x0080
// bit[8]    - This bit enables the 80 bits for initializing the cards
//             ('1' = true).
#define CMD_DAT_CONT_CARD_INIT          0x0100
// bit[9]    - This bit enables the application to write to this register
//             without initiating a command ('1' = true).
#define CMD_DAT_CONT_NO_COMMAND         0x0200
// bit[10]   - Specifies whether the data transfer of the current command is in
//             wide bus mode (4 bit) or not (1 bit) ('1' = wide bus,
//	     '0' = single bus).
#define CMD_DAT_CONT_WIDE_BUS_MODE      0x0400
#define CMD_DAT_CONT_SINGLE_BUS_MODE    0x0000
// bit[11]   - Enables changing the B10 bit. If not set, B10 will retain its
//             value. This enables the application to set Wide Bus only once
//	     and not per command.
#define CMD_DAT_CONT_WIDE_BUS_ALWAYS    0x0800
// bit[12]   - Enables use of NUMBER_OF_BLOCKS in multiple mode.
#define CMD_DAT_CONT_NOB_ON             0x1000

// The size of each disk block
#define MMC_DEFAULT_BLOCK_LENGTH        512

//
// Structures and statics.
//
typedef struct cyg_mmc_device {
    struct cyg_mmc_bus*     mmc_bus;
    cyg_uint32              mmc_baseaddr;
    cyg_uint8               mmc_wide_bus_disable;
    cyg_uint8               mmc_gateway_xfer;
} cyg_mmc_device;

typedef struct cyg_mmc_bus {
    cyg_drv_mutex_t         mmc_lock;
} cyg_mmc_bus;

// Details of a specific MMC card
typedef struct cyg_mmc_disk_info_t {
    // cyg_spi_device*     mmc_spi_dev;
    cyg_mmc_device*         mmc_dev;
    cyg_uint32              mmc_saved_baudrate;
    cyg_uint32              mmc_block_count;
    cyg_bool                mmc_read_only;
    cyg_bool                mmc_connected;
    cyg_uint32              mmc_heads_per_cylinder;
    cyg_uint32              mmc_sectors_per_head;
    cyg_uint32              mmc_read_block_length;
    cyg_uint32              mmc_write_block_length;
    cyg_uint8               mmc_card_type;
    cyg_uint8               mmc_bus_width;
    mmc_cid_register        mmc_cid;
    mmc_csd_register        CSD;
    mmc_ext_csd_register    EXT_CSD;
    cyg_uint32              RCA;
    cyg_uint32              OCR;
} cyg_mmc_disk_info_t;

// Struct for private data carried by set_config/get_config
typedef struct cyg_mmc_disk_config_priv_t {
    cyg_uint32          erase_disk_start;
    cyg_uint32          erase_disk_end;
    cyg_bool            format_disk;
} cyg_mmc_disk_config_priv_t;

#define SD_CONFIG_DISK_FORMAT    0x55AA0000
#define SD_CONFIG_DISK_ERASE     0x55AA0001

#endif //  ifdef CYGONCE_DEVS_DISK_SD_SIS910_H

/* EOF sis_910.h */
