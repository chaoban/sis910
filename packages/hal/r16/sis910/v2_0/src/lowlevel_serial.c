//-------------------------------------------------------------------------
//=========================================================================
//####### DESCRIPTIONBEGIN #######
//
// lowlevel_serial.c
//
// Author:       Chaoban Wang
// Date:         2009-04-10
// Purpose:      SiS910 HAL board support.
// Description:  Implementations of read data from uart rx buffer.
//
//=========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

void cyg_serial_read(cyg_uint8* __buf, cyg_uint32 __len);
static cyg_bool cyg_com_getc_nonblock(cyg_uint8* ch);
static cyg_uint8 cyg_com_read(void);
void InitUART(void);

#define UART_READ_UINT8(_a_, _d_)               					\
    CYG_MACRO_START                             					\
    cyg_uint32 t;                               					\
    HAL_READ_UINT32((_a_), t);                  					\
    (_d_) = (t) & 0xff;                    							\
    CYG_MACRO_END

void
cyg_serial_read(cyg_uint8* __buf, cyg_uint32 __len)
{
    while(__len-- > 0)
    {
        *__buf++ = cyg_com_read();
    }
}

static cyg_uint8
cyg_com_read(void)
{
    cyg_uint8 ch;

    while(!cyg_com_getc_nonblock(&ch)) ;

    return ch;
}

static cyg_bool
cyg_com_getc_nonblock(cyg_uint8* ch)
{
    cyg_uint32 _lsr, _fcr;

    HAL_READ_UINT32(UARTSTAREG, _lsr);

    if ((_lsr & SIO_LSR_DR) == 0)
    {
        return false;
    }

    UART_READ_UINT8(UARTRXFIFO, *ch);

//    HAL_WRITE_UINT32(UARTRXFIFO, 0x0);

    HAL_READ_UINT32(UARTCTLREG, _fcr);
    _fcr |= FCR_CLEAR_RCVR;
    HAL_WRITE_UINT32(UARTCTLREG, _fcr);

    return true;
}

void
InitUART(void)
{
    cyg_uint32 _VAL;

    HAL_READ_UINT32(UARTCTLREG, _VAL);
    _VAL &= ~LCR_PN;
    HAL_WRITE_UINT32(UARTCTLREG, _VAL);
}
