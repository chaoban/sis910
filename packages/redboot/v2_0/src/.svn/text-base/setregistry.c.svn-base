//==========================================================================
//
//      setregistry.c
//
//      RedBoot set registry support
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2002 Gary Thomas
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
// Author(s):    Chaoban/SiS
// Contributors: 
// Date:         2008-11-17
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#define UARTTXFIFO	0x90003704	//UART TX FIFO

RedBoot_cmd("wio", 
            "Write (hex) a value to HW registry", 
            "-r <registry> -v <hex value>",
            do_wreg 
    );

RedBoot_cmd("rio", 
            "Dump (hex) a value from HW registry", 
            "-r <registry>",
            do_rreg
    );

void
do_wreg(int argc, char *argv[])
{
    // Set a HW registry with a value
    struct option_info opts[2];
    unsigned long reg;
    cyg_uint32 val;
    bool reg_set, val_set;

    init_opts(&opts[0], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&reg, (bool *)&reg_set, "registry");
    init_opts(&opts[1], 'v', true, OPTION_ARG_TYPE_NUM, 
              (void **)&val, (bool *)&val_set, "value");

    if (!scan_opts(argc, argv, 1, opts, 2, 0, 0, "")) {
        return;
    }

    if (!reg_set || !val_set) {
        diag_printf("usage: wio -r <registry> -v <hex value>\n");
        return;
    }

    *((volatile unsigned *)reg) = val;
    
    if(reg == UARTTXFIFO)
    	diag_printf("\n");
}

void
do_rreg(int argc, char *argv[])
{
    struct option_info opts[1];
    unsigned long reg;
    cyg_uint32 val;
    bool reg_set;

    init_opts(&opts[0], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&reg, (bool *)&reg_set, "registry");
    
    if (!scan_opts(argc, argv, 1, opts, 1, 0, 0, "")) {
        return;
    }
    
    if (!reg_set) {
    	diag_printf("Dump what [registry]?\n");
    	diag_printf("usage: rio -r <registry>\n");
        return;
    }

	val = *((volatile unsigned *)reg);
	
	diag_printf("%p\n", val);
}
