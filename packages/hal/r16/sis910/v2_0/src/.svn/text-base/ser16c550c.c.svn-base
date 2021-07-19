//=============================================================================
//
//      ser16c550c.c
//
//      Simple driver for the SiS910 serial controllers
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Chaoban Wang
// Contributors:
// Date:        2008-3-21
// Description: Simple driver for the SiS910 serial controller
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

/* ==== FOR DDR-II SERIAL CLOCK ==== */
#ifdef CYG_HAL_DDRII
#define CYG_DDRII_CLOCK
#endif

/* To patch HW UART bug.		*/
//#define CYG_HAL_FIX_UARTRX
#ifdef CYG_DDRII_CLOCK
#define CYG_HAL_FIX_UART			//FOR DEMO BOARD 2 and SIS910 REAL CHIP
#endif

/* ==== PATCH TIMER DELAY ==== */
#define CYG_HAL_PATCH_DELAY

bool ver910b = false;

//-----------------------------------------------------------------------------
// Define the serial registers. The SiS910 board are equipped with a 16550C
// serial chips and different addresses and clocked at different rates.
// Details are in CDL.

/* ==== FOR UART CLOCK DIV ==== */
#ifdef __CHAOBAN_RC
#define UART_PCLK_DIV	6
#else
#ifdef CYG_DDRII_CLOCK
#if (CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD > 9600)
#define UART_PCLK_DIV	4
#else
#define UART_PCLK_DIV	6
#endif
#else	//DDR-I
#define UART_PCLK_DIV	1
#endif
#endif

#define SIS910_SER_CLOCK	(CYGHWR_HAL_SH_PROCESSOR_SPEED/UART_PCLK_DIV)
//#define CYG_DEV_SERIAL_BAUD_DIVISOR		(CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD*16*32767)/SIS910_SER_CLOCK

// Because define will make overflow in uint32, so we calculate it by ourself.
#if (CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD == 115200)
#define HIGHBAUDRATE
#ifdef __CHAOBAN_RC
#define CYG_DEV_SERIAL_BAUD_DIVISOR     0x726
#else
#define CYG_DEV_SERIAL_BAUD_DIVISOR     0x2752
#endif
#else
#if (CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD == 57600)
#define HIGHBAUDRATE
#ifdef __CHAOBAN_RC
#define CYG_DEV_SERIAL_BAUD_DIVISOR     0x393
#else
#define CYG_DEV_SERIAL_BAUD_DIVISOR     0x13A9
#endif
#else // 9600 ~ 38400 bps
#define CYG_DEV_SERIAL_BAUD_DIVISOR		(SIS910_SER_CLOCK/16/CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD)
#endif
#endif

#define SIS910_SER_16550_BASE_A    0x90000000

//-----------------------------------------------------------------------------
#ifdef CYG_HAL_FIX_UART
#define nopx {asm("nop");asm("nop");asm("nop");}

#define BUS_or32(_a_, _d_)											\
	CYG_MACRO_START 												\
    ((*(volatile unsigned int *)(_a_)) |= (unsigned int)(_d_));		\
    nopx;															\
    CYG_MACRO_END

#define UART_WRITE_UINT32( _register_, _value_ )                 	\
    CYG_MACRO_START                                             	\
    (*((volatile CYG_WORD32 *)(_register_)) = (_value_));       	\
    nopx;															\
    CYG_MACRO_END
#else
#define BUS_or32(_a_, _d_)											\
	CYG_MACRO_START 												\
    ((*(volatile unsigned int *)(_a_)) |= (unsigned int)(_d_));		\
    CYG_MACRO_END

#define UART_WRITE_UINT32( _register_, _value_ )                 	\
    CYG_MACRO_START                                             	\
    (*((volatile CYG_WORD32 *)(_register_)) = (_value_));       	\
    CYG_MACRO_END
#endif

#define UART_READ_UINT8(_a_, _d_)               					\
    CYG_MACRO_START                             					\
    cyg_uint32 t;                               					\
    HAL_READ_UINT32((_a_), t);                  					\
    (_d_) = (t) & 0xff;                    							\
    CYG_MACRO_END

//-----------------------------------------------------------------------------
typedef struct {
    cyg_uint8* base;
    cyg_int32 msec_timeout;
    int isr_vector;
} channel_data_t;

static channel_data_t channels[1] = {
    { (cyg_uint8*)SIS910_SER_16550_BASE_A, 1000, CYGNUM_HAL_INTERRUPT_COM1 },
};

//chaoban test
/****************************************************************************************************************/
/*  REPLACE WITH CYGACC_CALL_IF_DELAY_US																		*/
/*  If a "nop" instruction need a CPU clock, so we can know one time for-loop need 5 clock,						*/
/*  A clock time = (1/MCLK)*(10^9) ns																			*/
/*  So one time for-loop need 5*(1/MCLK)*(10^9) ns.																*/
/*  Then we can know one time for-loop must do how many times of "nop" instruction to implement 1us.			*/
/*  For example, when CPU clock is 24MHZ, one time for-loop spend 208ns = 0.208us. 								*/
/*  So we multipled "int us" by 4.8																				*/
/*  And for avoid dividing by zero issue, we add 0.1 to MCLK and ALOOP. Hahaha...								*/
/****************************************************************************************************************/
//#define	MCLK 	24000000						// Hz
//#define	ALOOP	5*(1/(MCLK+0.1))*1000000000		// ns
//#define	div_us	1000/(ALOOP+0.1)

void drv_delay(int us) {
	int counter;
#ifdef CYG_DDRII_CLOCK
#ifdef __CHAOBAN_RC
	int _us = 40 * us;	//REAL CHIP = 200000000
#else
	int _us = 4.8 * us;	//DDR-II MCLK=24000000
#endif
#else
	int _us = 2.4 * us;
#endif
	for ( counter = 0; counter < (_us); counter++ ) {
		asm volatile ("nop");
	}
}

//-----------------------------------------------------------------------------
// Set the baud rate

static void
cyg_hal_plf_serial_set_baud(cyg_uint8* port, cyg_uint16 baud_divisor)
{
	cyg_uint32 _lcr;

	HAL_READ_UINT32(UARTLEGACY, _lcr);
#ifdef CYG_HAL_FIX_UARTRX
	HAL_READ_UINT32(UARTLEGACY, _lcr);
#endif

#ifdef HIGHBAUDRATE
    _lcr &= ~LCR_DL;
#else
	_lcr |= LCR_DL;
#endif

    UART_WRITE_UINT32(UARTLEGACY, _lcr);
	UART_WRITE_UINT32(UARTDIVISR, baud_divisor);

//  Why marked LCR_DL ?
//	_lcr &= ~LCR_DL;
//	UART_WRITE_UINT32(UARTLEGACY, _lcr);
}

//-----------------------------------------------------------------------------
// The minimal init, get and put functions. All by polling.

void
cyg_hal_plf_serial_init_channel(void* __ch_data)
{
    cyg_uint8* port;
	cyg_uint32 _lcr;
	cyg_uint32 _VAL = 0;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    port = ((channel_data_t*)__ch_data)->base;

    //Default value is 0xc30e2c72
	if (UART_PCLK_DIV == 4)
	{
		HAL_READ_UINT32(MISC_PLL, _VAL);
		_VAL &= ~(0x1000000);				//0xc20e2c72
    	HAL_WRITE_UINT32(MISC_PLL, _VAL);	//PLL
    }

    UART_WRITE_UINT32(UARTDIVISR, 0x4);

    // Disable port interrupts while changing hardware
	UART_WRITE_UINT32(UARTCTLREG, 0x0);

	HAL_READ_UINT32(UARTRFSTAUS, _VAL);
	_VAL >>= 24;
	if (_VAL == UART910B) {
	    ver910b = true;
    }

    // Set databits, stopbits and parity.
    _lcr = LCR_WL8 | LCR_SB1 | LCR_PN | LCR_PE;
    BUS_or32(UARTCTLREG, _lcr);

    // Set baud rate.
    if (CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD >= 38400){
    	cyg_hal_plf_serial_set_baud(port, (CYG_DEV_SERIAL_BAUD_DIVISOR+1));
    }
    else {
    	cyg_hal_plf_serial_set_baud(port, CYG_DEV_SERIAL_BAUD_DIVISOR);
    }

    // Enable and clear FIFO
    HAL_READ_UINT32(UARTCTLREG, _lcr);
#ifdef CYG_HAL_FIX_UARTRX
    HAL_READ_UINT32(UARTCTLREG, _lcr);
#endif
    _lcr |= (FCR_CLEAR_RCVR | FCR_CLEAR_XMIT);
    BUS_or32(UARTCTLREG, _lcr);

    UART_WRITE_UINT32(UARTENREGR, 0x0); //RX enable
//	UART_WRITE_UINT32(UARTRXFIFO, 0x0); //pop command

    // enable RTS to keep host side happy. Also allow interrupts
    BUS_or32(UARTCTLREG, FCR_4B | MCR_DTR | MCR_RTS | SIO_IER_EMSI | SIO_IER_ERDAI | SIO_IER_ELSI);

    // Don't allow interrupts.
    //UART_WRITE_UINT8(port+SER_16550_IER, 0);
}

void
cyg_hal_plf_serial_putc(void* __ch_data, cyg_uint8 __ch)
{
    cyg_uint8* port;
    cyg_uint32 _lsr;
    cyg_uint32 _lcr;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    port = ((channel_data_t*)__ch_data)->base;

    CYGARC_HAL_SAVE_GP();

    if (ver910b) {
        HAL_READ_UINT32(UARTLEGACY, _lcr);
	    _lcr |= LEGACYBDM;
	    UART_WRITE_UINT32(UARTLEGACY, _lcr);
	}

// Wait for Tx FIFO not full
    do {
		HAL_READ_UINT32(UARTSTAREG, _lsr);
#ifdef CYG_HAL_FIX_UARTRX
		HAL_READ_UINT32(UARTSTAREG, _lsr);
#endif
    } while ((_lsr & SIO_LSR_THRE) == 0);

    // Now, the transmit buffer is empty
    UART_WRITE_UINT32(UARTTXFIFO, __ch);

    // Hang around until the character has been safely sent.
	do {
		HAL_READ_UINT32(UARTSTAREG, _lsr);
#ifdef CYG_HAL_FIX_UARTRX
		HAL_READ_UINT32(UARTSTAREG, _lsr);
#endif
    } while ((_lsr & SIO_LSR_THRE) == 0);

    if (ver910b) {
        HAL_READ_UINT32(UARTLEGACY, _lcr);
	    _lcr &= ~LEGACYBDM;
	    UART_WRITE_UINT32(UARTLEGACY, _lcr);
	}

    CYGARC_HAL_RESTORE_GP();
}

static cyg_bool
cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8* port;
	cyg_uint32 _lsr, _fcr;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    port = ((channel_data_t*)__ch_data)->base;

    HAL_READ_UINT32(UARTSTAREG, _lsr);
#ifdef CYG_HAL_FIX_UARTRX
    HAL_READ_UINT32(UARTSTAREG, _lsr);
#endif
    if ((_lsr & SIO_LSR_DR) == 0) {
        return false;
    }

    UART_READ_UINT8(UARTRXFIFO, *ch);
#ifdef CYG_HAL_FIX_UARTRX
    UART_READ_UINT8(UARTRXFIFO, *ch);
#endif

    if (ver910b)
        HAL_WRITE_UINT32(UARTRXFIFO, 0x0);

    HAL_READ_UINT32(UARTCTLREG, _fcr);
#ifdef CYG_HAL_FIX_UARTRX
	HAL_READ_UINT32(UARTCTLREG, _fcr);
#endif
    _fcr |= FCR_CLEAR_RCVR;
    UART_WRITE_UINT32(UARTCTLREG, _fcr);

    return true;
}

cyg_uint8
cyg_hal_plf_serial_getc(void* __ch_data)
{
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

  	while(!cyg_hal_plf_serial_getc_nonblock(__ch_data, &ch)) ;

    CYGARC_HAL_RESTORE_GP();
    return ch;
}

static void
cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf,
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

#if 1
    while(__len-- > 0) {
        cyg_hal_plf_serial_putc(__ch_data, *__buf++);
    }
#else
    cyg_uint32 __Txdata = 0;
    int i = 0;

    while(__len >= 4) {
    	for (i = 0; i < 4; i++) {
    		__Txdata |= ((*__buf++) << (i*8));
    	}

    	cyg_hal_plf_serial_putc(__ch_data, __Txdata);
    	__len -= 4;
    }

	__Txdata = 0;

    for (i = 0; i < __len; i++) {
    	__Txdata |= ((*__buf++) << (i*8));
    }

    cyg_hal_plf_serial_putc(__ch_data, __Txdata);
    __len = 0;
#endif

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    while(__len-- > 0) {
        *__buf++ = cyg_hal_plf_serial_getc(__ch_data);
    }

    CYGARC_HAL_RESTORE_GP();
}


cyg_bool
cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    int delay_count;
    channel_data_t* chan;
    cyg_bool res;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    chan = (channel_data_t*)__ch_data;

    delay_count = chan->msec_timeout * 10; // delay in .1 ms steps

    for(;;) {
        res = cyg_hal_plf_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--) {
            break;
        }
       // chaoban test: This may an issue make Rx fail.
#ifdef CYG_HAL_PATCH_DELAY
		drv_delay(100);
#else
        CYGACC_CALL_IF_DELAY_US(100);
#endif
	}

    CYGARC_HAL_RESTORE_GP();
    return res;
}

static int
cyg_hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    static int irq_state = 0;
    channel_data_t* chan;
    cyg_uint32 ier;
    int ret = 0;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    chan = (channel_data_t*)__ch_data;

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;
        HAL_READ_UINT32(UARTCTLREG, ier);
#ifdef CYG_HAL_FIX_UARTRX
        HAL_READ_UINT32(UARTCTLREG, ier);
#endif
        ier |= (SIO_IER_ERDAI | SIO_IER_ELSI);
        UART_WRITE_UINT32(UARTCTLREG, ier);

        HAL_INTERRUPT_SET_LEVEL(chan->isr_vector, 1);
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;
        HAL_READ_UINT32(UARTCTLREG, ier);
#ifdef CYG_HAL_FIX_UARTRX
        HAL_READ_UINT32(UARTCTLREG, ier);
#endif
        ier &= ~(SIO_IER_ERDAI | SIO_IER_ELSI);
        UART_WRITE_UINT32(UARTCTLREG, ier);

        HAL_INTERRUPT_MASK(chan->isr_vector);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = chan->isr_vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
    {
        va_list ap;

        va_start(ap, __func);

        ret = chan->msec_timeout;
        chan->msec_timeout = va_arg(ap, cyg_uint32);

        va_end(ap);
    }
    break;
    case __COMMCTL_SETBAUD:
    {
        cyg_uint32 baud_rate;
        cyg_uint16 baud_divisor;
        cyg_uint8* port = chan->base;
        va_list ap;

        va_start(ap, __func);
        baud_rate = va_arg(ap, cyg_uint32);
        va_end(ap);

        baud_divisor = (SIS910_SER_CLOCK / 16 / baud_rate)+1.0;

        // Disable port interrupts while changing hardware
        HAL_READ_UINT32(UARTCTLREG, ier);
#ifdef CYG_HAL_FIX_UARTRX
        HAL_READ_UINT32(UARTCTLREG, ier);
#endif
        UART_WRITE_UINT32(UARTCTLREG, 0x0);

        // Set baud rate.
        cyg_hal_plf_serial_set_baud(port, baud_divisor);

        // Reenable interrupts if necessary
		UART_WRITE_UINT32(UARTCTLREG, ier);
    }
    break;

    case __COMMCTL_GETBAUD:
        break;
    default:
        break;
    }

    CYGARC_HAL_RESTORE_GP();
    return ret;
}

static int
cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc,
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    int res = 0;
    cyg_uint32 _iir;
    cyg_uint8 c;
    channel_data_t* chan;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0) {
    	__ch_data = (void*)&channels[0];
    }

    chan = (channel_data_t*)__ch_data;

    HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);

	UART_READ_UINT8(UARTSTAREG, _iir);
#ifdef CYG_HAL_FIX_UARTRX
	UART_READ_UINT8(UARTSTAREG, _iir);
#endif
	_iir &= SIO_IIR_ID_MASK;

    *__ctrlc = 0;

	if ((_iir & ISR_Rx_Avail) || (_iir & ISR_Rx_Char_Timeout)) {
		UART_READ_UINT8(UARTRXFIFO, c);
#ifdef CYG_HAL_FIX_UARTRX
		UART_READ_UINT8(UARTRXFIFO, c);
#endif

        if( cyg_hal_is_break( &c , 1 ) ) {
            *__ctrlc = 1;
        }

        res = CYG_ISR_HANDLED;
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

void
cyg_hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Disable interrupts.
    HAL_INTERRUPT_MASK(channels[0].isr_vector);

    // Init channels
    cyg_hal_plf_serial_init_channel((void*)&channels[0]);

    // Setup procs in the vector table

    // Set channel 0
#ifndef HW_SIM
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &channels[0]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
#endif
    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);

    return;
}

char Getch(void) {
	cyg_uint32 _VAL;
	char c;

	do {
		HAL_READ_UINT32(UARTSTAREG, _VAL);
#ifdef CYG_HAL_FIX_UARTRX
		HAL_READ_UINT32(UARTSTAREG, _VAL);
#endif
	}
	while(!(_VAL & SIO_LSR_DR));

	c = rdURXH0;

    if (ver910b)
        HAL_WRITE_UINT32(UARTRXFIFO, 0x0);

	HAL_READ_UINT32(UARTCTLREG, _VAL);
#ifdef CYG_HAL_FIX_UARTRX
	HAL_READ_UINT32(UARTCTLREG, _VAL);
#endif

	_VAL |= FCR_CLEAR_RCVR;

	UART_WRITE_UINT32(UARTCTLREG, _VAL);

	return c;
}
//-----------------------------------------------------------------------------
// end of ser16c550c.c
