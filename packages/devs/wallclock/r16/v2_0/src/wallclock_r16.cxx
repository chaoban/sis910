//==========================================================================
//
//      devs/wallclock/wallclock_r16.cxx
//
//      R16 RTC module driver.
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
// Author(s):     Chaoban
// Contributors:  Chaoban
// Date:          2009-02-02
// Purpose:       Wallclock driver for R16 CPU RTC module
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/wallclock.h>          // Wallclock device config
#include <cyg/hal/hal_intr.h>           // interrupt enable/disable
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/infra/cyg_type.h>         // Common type definitions and support

#include <cyg/io/wallclock.hxx>         // The WallClock API
#include <cyg/io/wallclock/wallclock.inl> // Helpers

#include <cyg/hal/sh_regs.h>            // RTC register definitions

#include <cyg/infra/diag.h>             // For debugging

//-----------------------------------------------------------------------------
// Functions for setting and getting the hardware clock counters

static void
set_r16_hwclock(cyg_uint32 year, cyg_uint32 month, cyg_uint32 mday,
                cyg_uint32 hour, cyg_uint32 minute, cyg_uint32 second)
{
    // Stop R16 RTC Count
    cyg_uint32 _VAL;
	HAL_READ_UINT32(CYGARC_REG_B, _VAL);
	_VAL |= REG_B_SET;
	HAL_WRITE_UINT32(CYGARC_REG_B, _VAL);


    HAL_WRITE_UINT32(CYGARC_REG_RYRCNT,  TO_BCD((cyg_uint8)(year - 2000)));
	HAL_WRITE_UINT32(CYGARC_REG_RMONCNT, TO_BCD((cyg_uint8)month));
	HAL_WRITE_UINT32(CYGARC_REG_RDAYCNT, TO_BCD((cyg_uint8)mday));
//	HAL_WRITE_UINT32(CYGARC_REG_RWKCNT,  TO_BCD((cyg_uint8)wk));
	HAL_WRITE_UINT32(CYGARC_REG_RHRCNT,  TO_BCD((cyg_uint8)hour));
	HAL_WRITE_UINT32(CYGARC_REG_RMINCNT, TO_BCD((cyg_uint8)minute));
	HAL_WRITE_UINT32(CYGARC_REG_RSECCNT, TO_BCD((cyg_uint8)second));


    // Start R16 RTC Count
    HAL_READ_UINT32(CYGARC_REG_B, _VAL);
	_VAL &= ~REG_B_SET;
	HAL_WRITE_UINT32(CYGARC_REG_B, _VAL);

#ifdef DEBUG
    // This will cause the test to eventually fail due to these printouts
    // causing timer interrupts to be lost...
    diag_printf("Set time -------------\n");
    diag_printf("year %02d\n", year);
    diag_printf("month %02d\n", month);
    diag_printf("mday %02d\n", mday);
    diag_printf("hour %02d\n", hour);
    diag_printf("minute %02d\n", minute);
    diag_printf("second %02d\n", second);
#endif
}

static void
get_r16_hwclock(cyg_uint32* year, cyg_uint32* month, cyg_uint32* mday,
                cyg_uint32* hour, cyg_uint32* minute, cyg_uint32* second)
{
	cyg_uint8   _reg, tmp;
	cyg_uint32  _old;

	// Wait for update flag clears
	do {
        HAL_READ_UINT32(CYGARC_REG_A, _reg);
    } while (_reg & REG_A_UIP) ;

    // Disable interrupts while reading to ensure it doesn't take more
    // than 244us.
    HAL_DISABLE_INTERRUPTS(_old);

	// Read time
	HAL_READ_UINT32(CYGARC_REG_RYRCNT,  tmp);
	*year = (cyg_uint32)TO_DEC(tmp);

	HAL_READ_UINT32(CYGARC_REG_RMONCNT, tmp);
	*month = (cyg_uint32)TO_DEC(tmp);

	HAL_READ_UINT32(CYGARC_REG_RDAYCNT, tmp);
	*mday = (cyg_uint32)TO_DEC(tmp);

	HAL_READ_UINT32(CYGARC_REG_RHRCNT,  tmp);
	*hour = (cyg_uint32)TO_DEC(tmp);

	HAL_READ_UINT32(CYGARC_REG_RMINCNT, tmp);
	*minute = (cyg_uint32)TO_DEC(tmp);

	HAL_READ_UINT32(CYGARC_REG_RSECCNT, tmp);
	*second = (cyg_uint32)TO_DEC(tmp);

	// Reenable interrupts
    HAL_RESTORE_INTERRUPTS(_old);
}

//-----------------------------------------------------------------------------
// Functions required for the hardware-driver API.

// Returns the number of seconds elapsed since 2000-01-01 00:00:00.
cyg_uint32
Cyg_WallClock::get_hw_seconds(void)
{
    cyg_uint32  year, month, mday, hour, minute, second;

    get_r16_hwclock(&year, &month, &mday, &hour, &minute, &second);

#ifdef DEBUG
    // This will cause the test to eventually fail due to these printouts
    // causing timer interrupts to be lost...
    diag_printf("Get_hw_seconds -------------\n");
    diag_printf("year %02d\n",   year);
    diag_printf("month %02d\n",  month);
    diag_printf("mday %02d\n",   mday);
    diag_printf("hour %02d\n",   hour);
    diag_printf("minute %02d\n", minute);
    diag_printf("second %02d\n", second);
#endif

//chaoban test
//#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
//    year += 2000;
//#else
//    // Need to use sliding window or similar to figure out what the
//    // century should be... Patent issue is unclear, and since there's
//    // no battery backup of the clock, there's little point in
//    // investigating.
//# error "Need some magic here to figure out century counter"
//#endif

    cyg_uint32 now = _simple_mktime_r16(year, month, mday, hour, minute, second);
    return now;
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
// Sets the clock. Argument is seconds elapsed since 2000-01-01 00:00:00.
void
Cyg_WallClock::set_hw_seconds( cyg_uint32 secs )
{
    cyg_uint32 year, month, mday, hour, minute, second;

    _simple_mkdate_r16(secs, &year, &month, &mday, &hour, &minute, &second);

    set_r16_hwclock(year, month, mday, hour, minute, second);
}
#endif

static inline void
init_r16_hwclock(void)
{
	HAL_WRITE_UINT32(CYGARC_REG_A, REG_A_DIV);
	HAL_WRITE_UINT32(CYGARC_REG_B, REG_B_24H);
}

void
Cyg_WallClock::init_hw_seconds(void)
{
	init_r16_hwclock();

#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
    // This is our base: 2000-01-01 00:00:00
    // Set the HW clock - if for nothing else, just to be sure it's in a
    // legal range. Any arbitrary base could be used.
    // After this the hardware clock is only read.
    set_r16_hwclock(2009, 1, 1, 0, 0, 0);
#endif // CYGSEM_WALLCLOCK_SET_GET_MODE
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/wallclock_r16.cxx
