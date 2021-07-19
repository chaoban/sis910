#ifndef CYGONCE_DEVS_SPI_SIS910_H
#define CYGONCE_DEVS_SPI_SIS910_H
//==========================================================================
//
//      spi_sis910.h
//
//      SiS 910 SPI driver defines
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Savin Zlobec <savin@elatec.si>
// Date:          2004-08-25
//                Sidney Shih <sidney_shih@sis.com>
//                2008-02-19
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>

//-----------------------------------------------------------------------------
// SiS 910 SPI BUS

typedef struct cyg_spi_sis910_bus_s
{
    // ---- Upper layer data ----

    cyg_spi_bus   spi_bus;                  // Upper layer SPI bus data

    // ---- Lower layer data ----

    cyg_interrupt     spi_interrupt;        // SPI interrupt object
    cyg_handle_t      spi_interrupt_handle; // SPI interrupt handle
    cyg_drv_mutex_t   transfer_mx;          // Transfer mutex
    cyg_drv_cond_t    transfer_cond;        // Transfer condition
    cyg_bool          transfer_end;         // Transfer end flag
    cyg_bool          cs_up;                // Chip Select up flag
    cyg_vector_t      interrupt_number;     // SPI Interrupt Number
    cyg_addrword_t    base;                 // Base Address of the SPI peripheral
    cyg_uint8         cs_en;                // The Configurations state for the CS
    cyg_uint32        ctrl_val;             // The CTRL register value of SPI controller
} cyg_spi_sis910_bus_t;

//-----------------------------------------------------------------------------
// SiS 910 SPI DEVICE

typedef struct cyg_spi_sis910_device_s
{
    // ---- Upper layer data ----

    cyg_spi_device spi_device;  // Upper layer SPI device data

    // ---- Lower layer data (configurable) ----

    cyg_uint8  dev_num;         // Device number
    cyg_uint8  cl_pol;          // Clock polarity (0 or 1)
    cyg_uint8  cl_pha;          // Clock phase    (0 or 1)
    cyg_uint32 cl_brate;        // Clock baud rate
    cyg_uint8  ass;             // Automatic polls CS high after transfer done
    cyg_uint16 cs_up_udly;      // Delay in us between CS up and transfer start
    cyg_uint16 cs_dw_udly;      // Delay in us between transfer end and CS down
    cyg_uint16 tr_bt_udly;      // Delay in us between two transfers

    // ---- Lower layer data (internal) ----

    cyg_bool   init;            // Is device initialized
    cyg_uint16  cl_divider;     // Value of SCBR (SPI clock) reg field
} cyg_spi_sis910_device_t;

//-----------------------------------------------------------------------------
// SiS910 SPI exported busses

/* For backwards compatability  */
#define cyg_spi_sis910_bus cyg_spi_sis910_bus0

externC cyg_spi_sis910_bus_t cyg_spi_sis910_bus0;

// SiS SPI IRQ number
//#define CYGNUM_HAL_INTERRUPT_SPI         0x3

//-----------------------------------------------------------------------------
// SiS 910 SPI register list
#define REG_SIS910_SPI_Rx0               0x90002420
#define REG_SIS910_SPI_Rx1               0x90002424
#define REG_SIS910_SPI_Rx2               0x90002428
#define REG_SIS910_SPI_Rx3               0x9000242C
#define REG_SIS910_SPI_Tx0               0x90002420
#define REG_SIS910_SPI_Tx1               0x90002424
#define REG_SIS910_SPI_Tx2               0x90002428
#define REG_SIS910_SPI_Tx3               0x9000242C
#define REG_SIS910_SPI_CTRL              0x90002430
#define REG_SIS910_SPI_DIVIDER           0x90002434
#define REG_SIS910_SPI_SS                0x90002438
#define REG_SIS910_SPI_MODE              0x9000243C

// The inactive state value of SPCK is logic level one.
#define CTRL_SIS910_SPI_CPOL_ZERO        (0x0 << 14)
#define CTRL_SIS910_SPI_CPOL_ONE         (0x1 << 14)
#define CTRL_SIS910_SPI_ASS              (0x1 << 13)
#define CTRL_SIS910_SPI_IE               (0x1 << 12)
#define CTRL_SIS910_SPI_LSB              (0x1 << 11)
#define CTRL_SIS910_SPI_MSB              (0x0 << 11)
//#define CTRL_SIS910_SPI_Tx_ODD           (0x1 << 10)
//#define CTRL_SIS910_SPI_Rx_EVEN          (0x1 << 9)
#define CTRL_SIS910_SPI_CPHA_ZERO        (0x2 << 9)
#define CTRL_SIS910_SPI_CPHA_ONE         (0x1 << 9)
#define CTRL_SIS910_SPI_GO               (0x1 << 8)
#define CTRL_SIS910_SPI_BUSY             (0x1 << 8)

#define CTRL_SIS910_SPI_CHAR_LEN_1_BIT    0x01
#define CTRL_SIS910_SPI_CHAR_LEN_2_BITS   0x02
#define CTRL_SIS910_SPI_CHAR_LEN_4_BITS   0x04
#define CTRL_SIS910_SPI_CHAR_LEN_8_BITS   0x08
#define CTRL_SIS910_SPI_CHAR_LEN_16_BITS  0x10
#define CTRL_SIS910_SPI_CHAR_LEN_24_BITS  0x18
#define CTRL_SIS910_SPI_CHAR_LEN_32_BITS  0x20
#define CTRL_SIS910_SPI_CHAR_LEN_40_BITS  0x28
#define CTRL_SIS910_SPI_CHAR_LEN_48_BITS  0x30
#define CTRL_SIS910_SPI_CHAR_LEN_56_BITS  0x38
#define CTRL_SIS910_SPI_CHAR_LEN_64_BITS  0x40
#define CTRL_SIS910_SPI_CHAR_LEN_72_BITS  0x48
#define CTRL_SIS910_SPI_CHAR_LEN_80_BITS  0x50
#define CTRL_SIS910_SPI_CHAR_LEN_88_BITS  0x58
#define CTRL_SIS910_SPI_CHAR_LEN_96_BITS  0x60
#define CTRL_SIS910_SPI_CHAR_LEN_104_BITS 0x68
#define CTRL_SIS910_SPI_CHAR_LEN_112_BITS 0x70
#define CTRL_SIS910_SPI_CHAR_LEN_120_BITS 0x78
#define CTRL_SIS910_SPI_CHAR_LEN_127_BITS 0x7F
#define CTRL_SIS910_SPI_CHAR_LEN_128_BITS 0x00
#define CTRL_SIS910_SPI_CHAR_LEN_1_BYTE   CTRL_SIS910_SPI_CHAR_LEN_8_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_2_BYTES  CTRL_SIS910_SPI_CHAR_LEN_16_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_3_BYTES  CTRL_SIS910_SPI_CHAR_LEN_24_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_4_BYTES  CTRL_SIS910_SPI_CHAR_LEN_32_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_5_BYTES  CTRL_SIS910_SPI_CHAR_LEN_40_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_6_BYTES  CTRL_SIS910_SPI_CHAR_LEN_48_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_7_BYTES  CTRL_SIS910_SPI_CHAR_LEN_56_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_8_BYTES  CTRL_SIS910_SPI_CHAR_LEN_64_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_9_BYTES  CTRL_SIS910_SPI_CHAR_LEN_72_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_10_BYTES CTRL_SIS910_SPI_CHAR_LEN_80_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_11_BYTES CTRL_SIS910_SPI_CHAR_LEN_88_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_12_BYTES CTRL_SIS910_SPI_CHAR_LEN_96_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_13_BYTES CTRL_SIS910_SPI_CHAR_LEN_104_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_14_BYTES CTRL_SIS910_SPI_CHAR_LEN_112_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_15_BYTES CTRL_SIS910_SPI_CHAR_LEN_120_BITS
#define CTRL_SIS910_SPI_CHAR_LEN_16_BYTES CTRL_SIS910_SPI_CHAR_LEN_128_BITS
#define MODE_SIS910_SPI_ROM              0x0
#define MODE_SIS910_SPI_GENERIC          0x1

// Defines four SPI modes
#define SPI_MODE_ZERO      (CTRL_SIS910_SPI_CPOL_ZERO | CTRL_SIS910_SPI_CPHA_ZERO)
#define SPI_MODE_ONE       (CTRL_SIS910_SPI_CPOL_ZERO | CTRL_SIS910_SPI_CPHA_ONE)
#define SPI_MODE_TWO       (CTRL_SIS910_SPI_CPOL_ONE | CTRL_SIS910_SPI_CPHA_ZERO)
#define SPI_MODE_THREE     (CTRL_SIS910_SPI_CPOL_ONE| CTRL_SIS910_SPI_CPHA_ONE)

// The system clock of SiS 910
#define CYGNUM_HAL_SIS910_CLOCK_SPEED    200000000 // 200 MHz

#endif // CYGONCE_DEVS_SPI_SIS910_H

//-----------------------------------------------------------------------------
// End of spi_sis910.h
