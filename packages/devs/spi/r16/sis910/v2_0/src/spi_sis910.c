//==========================================================================
//
//      spi_sis910.c
//
//      SiS 910 SPI driver
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
#include <pkgconf/devs_spi_sis910.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>
#include <cyg/io/spi_sis910.h>
#include <cyg/error/codes.h>

// Communication parameters. First some debug support
#define DEBUG   0
#if DEBUG > 0
# define DEBUG1(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG1(format, ...)
#endif
#if DEBUG > 1
# define DEBUG2(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG2(format, ...)
#endif

// -------------------------------------------------------------------------
static void spi_sis910_init_bus(cyg_spi_sis910_bus_t * bus);

static cyg_uint32 spi_sis910_ISR(cyg_vector_t vector, cyg_addrword_t data);

static void spi_sis910_DSR(cyg_vector_t   vector,
                              cyg_ucount32   count,
                              cyg_addrword_t data);

static void spi_sis910_transaction_begin(cyg_spi_device *dev);

static void spi_sis910_transaction_transfer(cyg_spi_device  *dev,
                                               cyg_bool         polled,
                                               cyg_uint32       count,
                                               const cyg_uint8 *tx_data,
                                               cyg_uint8       *rx_data,
                                               cyg_bool         drop_cs);

static void spi_sis910_transaction_tick(cyg_spi_device *dev,
                                           cyg_bool        polled,
                                           cyg_uint32      count);

static void spi_sis910_transaction_end(cyg_spi_device* dev);

static int spi_sis910_get_config(cyg_spi_device *dev,
                                    cyg_uint32      key,
                                    void           *buf,
                                    cyg_uint32     *len);

static int spi_sis910_set_config(cyg_spi_device *dev,
                                    cyg_uint32      key,
                                    const void     *buf,
                                    cyg_uint32     *len);

// -------------------------------------------------------------------------
// SiS 910 SPI BUS

//#ifdef CYGHWR_DEVS_SPI_SIS910_BUS0
cyg_spi_sis910_bus_t cyg_spi_sis910_bus0 = {
    .spi_bus.spi_transaction_begin    = spi_sis910_transaction_begin,
    .spi_bus.spi_transaction_transfer = spi_sis910_transaction_transfer,
    .spi_bus.spi_transaction_tick     = spi_sis910_transaction_tick,
    .spi_bus.spi_transaction_end      = spi_sis910_transaction_end,
    .spi_bus.spi_get_config           = spi_sis910_get_config,
    .spi_bus.spi_set_config           = spi_sis910_set_config,
    .interrupt_number                 = CYGNUM_HAL_INTERRUPT_SPI,
    .base                             = 0x0,
};

CYG_SPI_DEFINE_BUS_TABLE(cyg_spi_sis910_device_t, 0);
//#endif
// -------------------------------------------------------------------------

void
cyg_spi_sis910_bus_init(void)
{
//#ifdef CYGHWR_DEVS_SPI_SIS910_BUS0
   DEBUG1("cyg_spi_sis910_bus_init()\n");
   spi_sis910_init_bus(&cyg_spi_sis910_bus0);
//#endif
}

// -------------------------------------------------------------------------

static void spi_sis910_init_bus(cyg_spi_sis910_bus_t * spi_bus)
{
    //cyg_uint32 ctr;
    DEBUG1("spi_sis910_init_bus()\n");

    // Create and attach SPI interrupt object
    cyg_drv_interrupt_create(spi_bus->interrupt_number,
                             4,
                             (cyg_addrword_t)spi_bus,
                             spi_sis910_ISR,
                             spi_sis910_DSR,
                             &spi_bus->spi_interrupt_handle,
                             &spi_bus->spi_interrupt);

    //DEBUG1(" spi_bus->interrupt_number = %d.\n", spi_bus->interrupt_number);
    //DEBUG1(" spi_bus->spi_interrupt = %X.\n", spi_bus->spi_interrupt);

    cyg_drv_interrupt_attach(spi_bus->spi_interrupt_handle);

    // Init transfer mutex and condition
    cyg_drv_mutex_init(&spi_bus->transfer_mx);
    cyg_drv_cond_init(&spi_bus->transfer_cond,
                      &spi_bus->transfer_mx);
    //DEBUG1(" spi_bus->transfer_mx = %X.\n", spi_bus->transfer_mx);
    //DEBUG1(" spi_bus->transfer_cond = %X.\n", spi_bus->transfer_cond);

    // Init flags
    spi_bus->transfer_end = true;
    spi_bus->cs_up        = false;
    spi_bus->ctrl_val    |= (CTRL_SIS910_SPI_IE | CTRL_SIS910_SPI_ASS);
    //spi_bus->ctrl_val |= CTRL_SIS910_SPI_IE;

    // Select general SPI path
    HAL_WRITE_UINT32(REG_SIS910_SPI_MODE, MODE_SIS910_SPI_GENERIC);

    // Call upper layer bus init
    CYG_SPI_BUS_COMMON_INIT(&spi_bus->spi_bus);
}

static cyg_uint32
spi_sis910_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_spi_sis910_bus_t * spi_bus = (cyg_spi_sis910_bus_t *)data;
    cyg_uint32 stat;

    DEBUG1("spi_sis910_ISR()\n");
    // Read the status register and disable
    // the SPI int events that have occoured
    HAL_READ_UINT32(REG_SIS910_SPI_CTRL, stat);
    //HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, (stat & ~0x1000));
// ===================== DSR code =======================
    HAL_READ_UINT32(REG_SIS910_SPI_CTRL, stat);

    if(stat & CTRL_SIS910_SPI_BUSY)
    {
        // Transfer still in progress - unmask the SPI
        // int so we can get more SPI int events
        cyg_drv_interrupt_unmask(vector);
    }
    else
    {
        // Transfer ended
        spi_bus->transfer_end = true;
        cyg_drv_cond_signal(&spi_bus->transfer_cond);
    }
// ===================== DSR code =======================

    cyg_drv_interrupt_mask(vector);
    cyg_drv_interrupt_acknowledge(vector);

    return CYG_ISR_CALL_DSR;
}

static void
spi_sis910_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_spi_sis910_bus_t *spi_bus = (cyg_spi_sis910_bus_t *) data;
    cyg_uint32 stat;

    DEBUG1("spi_sis910_DSR()\n");
    // Read the status register and
    // check for transfer completition

    HAL_READ_UINT32(REG_SIS910_SPI_CTRL, stat);

    if(stat & CTRL_SIS910_SPI_BUSY)
    {
        // Transfer still in progress - unmask the SPI
        // int so we can get more SPI int events
        cyg_drv_interrupt_unmask(vector);
    }
    else
    {
        // Transfer ended
        spi_bus->transfer_end = true;
        cyg_drv_cond_signal(&spi_bus->transfer_cond);
    }
}

static void
spi_sis910_drop_ss(cyg_spi_sis910_device_t *dev)
{
    cyg_spi_sis910_bus_t *spi_bus = (cyg_spi_sis910_bus_t *)dev->spi_device.spi_bus;

    if (!spi_bus->cs_up)
       return;

    // Drop CS
    HAL_WRITE_UINT32(REG_SIS910_SPI_SS, 0x0);
    spi_bus->cs_up = false;
}

// Caculate the DIVIDER of baud rate
static cyg_bool
spi_sis910_calc_divider(cyg_spi_sis910_device_t *dev)
{
    cyg_uint16 divider;
    cyg_bool   res = true;

    // Calculate DIVIDER from baud rate
    divider = CYGNUM_HAL_SIS910_CLOCK_SPEED / (dev->cl_brate << 1) - 1;

    if (divider > 0xFFFF)
    {
        dev->cl_divider = 0xFFFF;
        res = false;
    }
    else
    {
        dev->cl_divider  = divider;
    }

    return res;
}

static void
spi_sis910_transfer(cyg_spi_sis910_device_t *dev,
                  cyg_uint32             count,
                  const cyg_uint8       *tx_data,
                  cyg_uint8             *rx_data)
{
    cyg_spi_sis910_bus_t *spi_bus = (cyg_spi_sis910_bus_t *)dev->spi_device.spi_bus;
    cyg_uint32 tx_tmp[4] = {0};
    cyg_uint32 rx_tmp[4] = {0};
    cyg_uint8  tx_buf[16] = {0};
    cyg_uint8  rx_buf[16] = {0};
    cyg_uint16 first_tx_count = count % 16;
    cyg_uint32 loop_count = count / 16;
    cyg_int32 m, n;
    cyg_uint8 char_len;
    cyg_int32 val = 0;

    DEBUG1("spi_sis910_transfer()\n");

    if (first_tx_count > 0)
    {
        // Prepare data to send
        for (m = first_tx_count - 1; m > -1; m--)
        {
            tx_buf[m] = *tx_data;
            tx_data++;
        }

        // Set char length
        char_len = first_tx_count << 3;
        spi_bus->ctrl_val &= ~0x7F;
        spi_bus->ctrl_val |= char_len;
        DEBUG1("ctrl_val = %08X\n", spi_bus->ctrl_val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, spi_bus->ctrl_val);

        // Set data to Tx registers
        // tx_tmp[3] = (tx_buf[15] << 24) | (tx_buf[14] << 16) | (tx_buf[13] << 8) | tx_buf[12];
        // tx_tmp[2] = (tx_buf[11] << 24) | (tx_buf[10] << 16) | (tx_buf[9] << 8) | tx_buf[8];
        // tx_tmp[1] = (tx_buf[7] << 24) | (tx_buf[6] << 16) | (tx_buf[5] << 8) | tx_buf[4];
        // tx_tmp[0] = (tx_buf[3] << 24) | (tx_buf[2] << 16) | (tx_buf[1] << 8) | tx_buf[0];
        for (m = 0; m < 4; m++)
        {
            tx_tmp[m] = (tx_buf[(m << 2) + 3] << 24) | (tx_buf[(m << 2) + 2] << 16) |
                        (tx_buf[(m << 2) + 1] << 8) | tx_buf[(m << 2)];
        }

        DEBUG1("Send Tx.\n");
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx0, tx_tmp[0]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx0, val);
        DEBUG1("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx1, tx_tmp[1]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx1, val);
        DEBUG1("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx2, tx_tmp[2]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx2, val);
        DEBUG1("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx3, tx_tmp[3]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx3, val);
        DEBUG1("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);

        // Slave Select
        HAL_WRITE_UINT32(REG_SIS910_SPI_SS, dev->dev_num);
        spi_bus->cs_up = true;

        // Send Tx
        spi_bus->ctrl_val |= CTRL_SIS910_SPI_GO;
        HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, spi_bus->ctrl_val);

//        // Wait transfer done
//        cyg_drv_mutex_lock(&spi_bus->transfer_mx);
//        spi_bus->transfer_end = false;
//
//        cyg_drv_interrupt_unmask(spi_bus->interrupt_number);
//        cyg_drv_dsr_lock();
//        while (!spi_bus->transfer_end)
//        {
//            cyg_drv_cond_wait(&spi_bus->transfer_cond);
//            //spi_bus->transfer_end = true;
//        }
//        cyg_drv_dsr_unlock();
//        cyg_drv_mutex_unlock(&spi_bus->transfer_mx);

        // Poll the busy bit
        do {
            HAL_READ_UINT32(REG_SIS910_SPI_CTRL, val);
        } while (val & CTRL_SIS910_SPI_BUSY);

        // Read Rx
        if (NULL != rx_data)
        {
            DEBUG1("Read Rx.\n");
            HAL_READ_UINT32(REG_SIS910_SPI_Rx0, rx_tmp[0]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx0, val);
            DEBUG1("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);
            HAL_READ_UINT32(REG_SIS910_SPI_Rx1, rx_tmp[1]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx1, val);
            DEBUG1("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
            HAL_READ_UINT32(REG_SIS910_SPI_Rx2, rx_tmp[2]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx2, val);
            DEBUG1("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
            HAL_READ_UINT32(REG_SIS910_SPI_Rx3, rx_tmp[3]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx3, val);
            DEBUG1("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);

            // rx_buf[15] = rx_tmp[3] >> 24;
            // rx_buf[14] = rx_tmp[3] >> 16;
            // rx_buf[13] = rx_tmp[3] >> 8;
            // rx_buf[12] = rx_tmp[3] >> 0;
            // rx_buf[11] = rx_tmp[2] >> 24;
            // rx_buf[10] = rx_tmp[2] >> 16;
            // rx_buf[9] = rx_tmp[2] >> 8;
            // rx_buf[8] = rx_tmp[2] >> 0;
            // rx_buf[7] = rx_tmp[1] >> 24;
            // rx_buf[6] = rx_tmp[1] >> 16;
            // rx_buf[5] = rx_tmp[1] >> 8;
            // rx_buf[4] = rx_tmp[1] >> 0;
            // rx_buf[3] = rx_tmp[0] >> 24;
            // rx_buf[2] = rx_tmp[0] >> 16;
            // rx_buf[1] = rx_tmp[0] >> 8;
            // rx_buf[0] = rx_tmp[0] >> 0;
            for (m = 0; m < 4; m++)
            {
                for (n = 0; n < 4; n++)
                {
                    rx_buf[(m << 2) + n] = rx_tmp[m] >> (n << 3);
                }
            }

            for (n = first_tx_count - 1; n > -1; n--)
            {
                *rx_data = rx_buf[n];
                rx_data++;
            }
        }
    }

    // Send others
    for (m = loop_count; m > 0; m--)
    {
        // Prepare data to send
        for (n = 15; n > -1; n--)
        {
            tx_buf[n] = *tx_data;
            tx_data++;
        }
        // Set char length
        char_len = CTRL_SIS910_SPI_CHAR_LEN_16_BYTES;
        spi_bus->ctrl_val &= ~0x7F;
        spi_bus->ctrl_val |= char_len;
        DEBUG1("ctrl_val = %08X\n", spi_bus->ctrl_val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, spi_bus->ctrl_val);

        // Set data to Tx registers
        // tx_tmp[3] = (tx_buf[15] << 24) | (tx_buf[14] << 16) | (tx_buf[13] << 8) | tx_buf[12];
        // tx_tmp[2] = (tx_buf[11] << 24) | (tx_buf[10] << 16) | (tx_buf[9] << 8) | tx_buf[8];
        // tx_tmp[1] = (tx_buf[7] << 24) | (tx_buf[6] << 16) | (tx_buf[5] << 8) | tx_buf[4];
        // tx_tmp[0] = (tx_buf[3] << 24) | (tx_buf[2] << 16) | (tx_buf[1] << 8) | tx_buf[0];
        for (m = 0; m < 4; m++)
        {
            tx_tmp[m] = (tx_buf[(m << 2) + 3] << 24) | (tx_buf[(m << 2) + 2] << 16) |
                        (tx_buf[(m << 2) + 1] << 8) | tx_buf[(m << 2)];
        }

        DEBUG1("Send Tx.\n");
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx0, tx_tmp[0]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx0, val);
        DEBUG1("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx1, tx_tmp[1]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx1, val);
        DEBUG1("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx2, tx_tmp[2]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx2, val);
        DEBUG1("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx3, tx_tmp[3]);
        HAL_READ_UINT32(REG_SIS910_SPI_Tx3, val);
        DEBUG1("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);

        // Slave Select
        HAL_WRITE_UINT32(REG_SIS910_SPI_SS, dev->dev_num);
        spi_bus->cs_up = true;

        // Send Tx
        spi_bus->ctrl_val |= CTRL_SIS910_SPI_GO;
        HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, spi_bus->ctrl_val);

//        // Wait transfer done
//        cyg_drv_mutex_lock(&spi_bus->transfer_mx);
//        spi_bus->transfer_end = false;
//
//        cyg_drv_interrupt_unmask(spi_bus->interrupt_number);
//        cyg_drv_dsr_lock();
//        while (!spi_bus->transfer_end)
//        {
//            cyg_drv_cond_wait(&spi_bus->transfer_cond);
//            //spi_bus->transfer_end = true;
//        }
//        cyg_drv_dsr_unlock();
//
//        cyg_drv_mutex_unlock(&spi_bus->transfer_mx);

        // Poll the busy bit
        do {
            HAL_READ_UINT32(REG_SIS910_SPI_CTRL, val);
        } while (val & CTRL_SIS910_SPI_BUSY);

        // Read Rx
        if (NULL != rx_data)
        {
            DEBUG1("Read Rx.\n");
            HAL_READ_UINT32(REG_SIS910_SPI_Rx0, rx_tmp[0]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx0, val);
            DEBUG1("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);
            HAL_READ_UINT32(REG_SIS910_SPI_Rx1, rx_tmp[1]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx1, val);
            DEBUG1("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
            HAL_READ_UINT32(REG_SIS910_SPI_Rx2, rx_tmp[2]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx2, val);
            DEBUG1("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
            HAL_READ_UINT32(REG_SIS910_SPI_Rx3, rx_tmp[3]);
            HAL_READ_UINT32(REG_SIS910_SPI_Tx3, val);
            DEBUG1("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);

            // rx_buf[15] = rx_tmp[3] >> 24;
            // rx_buf[14] = rx_tmp[3] >> 16;
            // rx_buf[13] = rx_tmp[3] >> 8;
            // rx_buf[12] = rx_tmp[3] >> 0;
            // rx_buf[11] = rx_tmp[2] >> 24;
            // rx_buf[10] = rx_tmp[2] >> 16;
            // rx_buf[9] = rx_tmp[2] >> 8;
            // rx_buf[8] = rx_tmp[2] >> 0;
            // rx_buf[7] = rx_tmp[1] >> 24;
            // rx_buf[6] = rx_tmp[1] >> 16;
            // rx_buf[5] = rx_tmp[1] >> 8;
            // rx_buf[4] = rx_tmp[1] >> 0;
            // rx_buf[3] = rx_tmp[0] >> 24;
            // rx_buf[2] = rx_tmp[0] >> 16;
            // rx_buf[1] = rx_tmp[0] >> 8;
            // rx_buf[0] = rx_tmp[0] >> 0;
            for (m = 0; m < 4; m++)
            {
                for (n = 0; n < 4; n++)
                {
                    rx_buf[(m << 2) + n] = rx_tmp[m] >> (n << 3);
                }
            }

            for (n = 15; n > -1; n--)
            {
                *rx_data = rx_buf[n];
                rx_data++;
            }
        }
    }

    // Registers Dump
    diag_printf("======== Registers Dump ========\n");
    HAL_READ_UINT32(REG_SIS910_SPI_Rx0, val);
    diag_printf("0x90002420 Tx0/Rx0 = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_Rx1, val);
    diag_printf("0x90002424 Tx1/Rx1 = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_Rx2, val);
    diag_printf("0x90002428 Tx2/Rx2 = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_Rx3, val);
    diag_printf("0x9000242C Tx3/Rx3 = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    diag_printf("0x90002430 CTRL    = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_DIVIDER, val);
    diag_printf("0x90002434 DIVIDER = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_SS, val);
    diag_printf("0x90002438 SS      = 0x%08X\n", val);
    HAL_READ_UINT32(REG_SIS910_SPI_MODE, val);
    diag_printf("0x9000243C MODE    = 0x%08X\n", val);
    diag_printf("================================\n");
}

static void
spi_sis910_transfer_polled(cyg_spi_sis910_device_t *dev,
                         cyg_uint32             count,
                         const cyg_uint8       *tx_data,
                         cyg_uint8             *rx_data)
{
    cyg_uint32 val;
    cyg_spi_sis910_bus_t *spi_bus = (cyg_spi_sis910_bus_t *)dev->spi_device.spi_bus;

    DEBUG1("spi_sis910_transfer_polled()\n");
    // Transmit and receive byte by byte
    while (count-- > 0)
    {
        // Send next byte over the wire
        val = *tx_data++;
        HAL_WRITE_UINT32(REG_SIS910_SPI_Tx0, val);
        // Set CTRL register
        spi_bus->ctrl_val &= ~(0x3 << 12); // Clear ASS and IE
        spi_bus->ctrl_val |= CTRL_SIS910_SPI_CHAR_LEN_1_BYTE;
        HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, spi_bus->ctrl_val);
        // Set SS register
        //spi_sis910_raise_ss(dev);
        // Set CTRL register (data size is 8 bits and Interrupt Enable)
        spi_bus->ctrl_val |= CTRL_SIS910_SPI_ASS |
                             CTRL_SIS910_SPI_GO;
        HAL_WRITE_UINT32(REG_SIS910_SPI_CTRL, spi_bus->ctrl_val);

        // Poll the busy bit
        do {
            HAL_READ_UINT32(REG_SIS910_SPI_CTRL, val);
        } while (val & CTRL_SIS910_SPI_BUSY);

        // Store received byte
        HAL_READ_UINT32(REG_SIS910_SPI_Rx0, val);
        if (NULL != rx_data)
            *rx_data++ = val;
    }
}

// -------------------------------------------------------------------------

static void
spi_sis910_transaction_begin(cyg_spi_device *dev)
{
    cyg_spi_sis910_device_t *sis910_spi_dev = (cyg_spi_sis910_device_t *) dev;
    cyg_spi_sis910_bus_t *spi_bus = (cyg_spi_sis910_bus_t *)sis910_spi_dev->spi_device.spi_bus;
    cyg_uint32 val = 0;

    if (!sis910_spi_dev->init)
    {
        sis910_spi_dev->init = true;
        spi_sis910_calc_divider(sis910_spi_dev);
    }

    // Configure SPI channel 0 - this is the only channel we
    // use for all devices since we drive chip selects manually

    if (1 == sis910_spi_dev->cl_pol)
    {
        DEBUG1("CPOL set to ONE.\n");
        val |= CTRL_SIS910_SPI_CPOL_ONE;
    }
    else
    {
        DEBUG1("CPOL set to ZERO.\n");
    }

    if (1 == sis910_spi_dev->cl_pha)
    {
        DEBUG1("CPHA set to ONE.\n");
        val |= CTRL_SIS910_SPI_CPHA_ONE;
    }
    else
    {
        DEBUG1("CPHA set to ZERO.\n");
        val |= CTRL_SIS910_SPI_CPHA_ZERO;
    }

    if (1 == sis910_spi_dev->ass)
    {
        DEBUG1("ASS set.\n");
        val |= CTRL_SIS910_SPI_ASS;
    }
    else
    {
        DEBUG1("ASS clear.\n");
        val &= 0xFFFFDFFF;
    }

    spi_bus->ctrl_val |= val;
    DEBUG1("spi_bus->ctrl_val = 0x%08X\n", spi_bus->ctrl_val);

    HAL_WRITE_UINT32(REG_SIS910_SPI_MODE, MODE_SIS910_SPI_GENERIC);
    //
//    val |= AT91_SPI_CSR_SCBR(sis910_spi_dev->cl_divider);
}

static void
spi_sis910_transaction_transfer(cyg_spi_device  *dev,
                              cyg_bool         polled,
                              cyg_uint32       count,
                              const cyg_uint8 *tx_data,
                              cyg_uint8       *rx_data,
                              cyg_bool         drop_cs)
{
    cyg_spi_sis910_device_t *sis910_spi_dev = (cyg_spi_sis910_device_t *) dev;

    // TODO: DO it later??
    // Select the device if not already selected
    //spi_sis910_start_transfer(sis910_spi_dev);

    DEBUG1("spi_sis910_transaction_transfer()\n");

    // Perform the transfer
    if (polled)
        spi_sis910_transfer_polled(sis910_spi_dev, count, tx_data, rx_data);
    else
        spi_sis910_transfer(sis910_spi_dev, count, tx_data, rx_data);

    // Deselect the device if requested
    if (drop_cs)
        spi_sis910_drop_ss(sis910_spi_dev);
}

static void
spi_sis910_transaction_tick(cyg_spi_device *dev,
                          cyg_bool        polled,
                          cyg_uint32      count)
{
    const cyg_uint32 zeros[10] = { 0,0,0,0,0,0,0,0,0,0 };

    cyg_spi_sis910_device_t *sis910_spi_dev = (cyg_spi_sis910_device_t *) dev;

    DEBUG1("spi_sis910_transaction_tick()\n");
    // Transfer count zeros to the device - we don't touch the
    // chip select, the device could be selected or deselected.
    // It is up to the device driver to decide in wich state the
    // device will be ticked.

    while (count > 0)
    {
        int tcnt = count > 40 ? 40 : count;

        if (polled)
            spi_sis910_transfer_polled(sis910_spi_dev, tcnt,
                                     (const cyg_uint8 *) zeros, NULL);
        else
            spi_sis910_transfer(sis910_spi_dev, tcnt,
                              (const cyg_uint8 *) zeros, NULL);

        count -= tcnt;
    }
}

static void
spi_sis910_transaction_end(cyg_spi_device* dev)
{
    cyg_spi_sis910_device_t * sis910_spi_dev = (cyg_spi_sis910_device_t *)dev;

    DEBUG1("spi_sis910_transaction_end()\n");
    // Drop CS
    if (0 == sis910_spi_dev->ass)
    {
        DEBUG1("Drop CS.\n");
        spi_sis910_drop_ss(sis910_spi_dev);
    }

    // Reset to SPI ROM mode
    HAL_WRITE_UINT32(REG_SIS910_SPI_MODE, MODE_SIS910_SPI_ROM);
}

static int
spi_sis910_get_config(cyg_spi_device *dev,
                    cyg_uint32      key,
                    void           *buf,
                    cyg_uint32     *len)
{
    cyg_spi_sis910_device_t *sis910_spi_dev = (cyg_spi_sis910_device_t *) dev;

    DEBUG1("spi_sis910_get_config()\n");
    switch (key)
    {
        case CYG_IO_GET_CONFIG_SPI_CLOCKRATE:
        {
            if (*len != sizeof(cyg_uint32))
                return -EINVAL;
            else
            {
                cyg_uint32 *cl_brate = (cyg_uint32 *)buf;
                *cl_brate = sis910_spi_dev->cl_brate;
            }
        }
        break;
        case CYG_IO_GET_CONFIG_SPI_ASS:
        {
            if (*len != sizeof(cyg_uint32))
            {
                return -EINVAL;
            }
            else
            {
                cyg_uint8 *ass = (cyg_uint8 *)buf;
                *ass = sis910_spi_dev->ass;
            }
        }
        break;
        default:
            return -EINVAL;
    }
    return ENOERR;
}

static int
spi_sis910_set_config(cyg_spi_device *dev,
                    cyg_uint32      key,
                    const void     *buf,
                    cyg_uint32     *len)
{
    cyg_spi_sis910_device_t *sis910_spi_dev = (cyg_spi_sis910_device_t *) dev;

    DEBUG1("spi_sis910_get_config()\n");
    switch (key)
    {
        case CYG_IO_SET_CONFIG_SPI_CLOCKRATE:
        {
            if (*len != sizeof(cyg_uint32))
                return -EINVAL;
            else
            {
                cyg_uint32 cl_brate     = *((cyg_uint32 *)buf);
                cyg_uint32 old_cl_brate = sis910_spi_dev->cl_brate;

                sis910_spi_dev->cl_brate = cl_brate;

                if (!spi_sis910_calc_divider(sis910_spi_dev))
                {
                    sis910_spi_dev->cl_brate = old_cl_brate;
                    spi_sis910_calc_divider(sis910_spi_dev);
                    return -EINVAL;
                }
            }
        }
        break;
        case CYG_IO_SET_CONFIG_SPI_ASS:
        {
            if (*len != sizeof(cyg_uint32))
            {
                return -EINVAL;
            }
            else
            {
                cyg_uint8 ass = *((cyg_uint32 *)buf);
                if ((ass == 0) || (ass == 1))
                {
                    sis910_spi_dev->ass = ass;
                }
                else
                {
                    return -EINVAL;
                }
            }
        }
        break;
        default:
            return -EINVAL;
    }
    return ENOERR;
}

// -------------------------------------------------------------------------
// EOF spi_sis910.c
