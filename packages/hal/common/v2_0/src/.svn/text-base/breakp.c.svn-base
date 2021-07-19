//==========================================================================
//
//      breakp.c
//
//      eCos break point routine
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Red Hat, Inc.
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
// Date:         2008-12-5
// Purpose:      
// Description:  
//              
// This code is part of eCos (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define  DEFINE_VARS
#include <cyg/kernel/kapi.h>	// Kernel API.
#include "redboot.h"

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __RedBoot_INIT_TAB__, RedBoot_inits );
CYG_HAL_TABLE_END( __RedBoot_INIT_TAB_END__, RedBoot_inits );
extern struct init_tab_entry __RedBoot_INIT_TAB__[], __RedBoot_INIT_TAB_END__;

CYG_HAL_TABLE_BEGIN( __RedBoot_CMD_TAB__, RedBoot_commands );
CYG_HAL_TABLE_END( __RedBoot_CMD_TAB_END__, RedBoot_commands );
extern struct cmd __RedBoot_CMD_TAB__[], __RedBoot_CMD_TAB_END__;

CYG_HAL_TABLE_BEGIN( __RedBoot_IDLE_TAB__, RedBoot_idle );
CYG_HAL_TABLE_END( __RedBoot_IDLE_TAB_END__, RedBoot_idle );
extern struct idle_tab_entry __RedBoot_IDLE_TAB__[], __RedBoot_IDLE_TAB_END__;

#define show_rm {								\
	cyg_uint32 Rm[16] = 0;						\
	cyg_uint32 i = 0;							\
	asm volatile(								\
		"mov r0, [%0],#4	  			  \n\t" \
		:"=r"(Rm)								\
		:										\
		:"r0"									\
	);											\
												\
	for(i = 0; i < 16; i++){					\
		diag_printf("R%d = %p\n", i, Rm[i]);	\
	}											\
}

#define UARTTXFIFO	0x90003704	//UART TX FIFO

extern void drv_delay(int us);
void breakp(int num);

//cyg_mutex_t cliblock;

RedBoot_cmd("help", 
            "Help about help?", 
            "[<topic>]",
            do_help 
    );

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
show_help(struct cmd *cmd, struct cmd *cmd_end, char *which, char *pre)
{
    bool show;
    int len = 0;

    if (which) {
        len = strlen(which);
    }
    
    while (cmd != cmd_end) {
        show = true;
        if (which && (strncasecmp(which, cmd->str, len) != 0)) {
            show = false;
        }
        
        if (show) {
            diag_printf("%s\n  %s %s %s\n", cmd->help, pre, cmd->str, cmd->usage);
            if ((cmd->sub_cmds != (struct cmd *)0) && (which != (char *)0)) {
                show_help(cmd->sub_cmds, cmd->sub_cmds_end, 0, cmd->str);
            }
        }   
        cmd++;
    }
}

void
do_help(int argc, char *argv[])
{
    struct cmd *cmd;
    char *which = (char *)0;

    if (!scan_opts(argc, argv, 1, 0, 0, (void **)&which, OPTION_ARG_TYPE_STR, "<topic>")) {
        diag_printf("Invalid argument\n");
        return;
    }
    
    cmd = __RedBoot_CMD_TAB__;
    show_help(cmd, &__RedBoot_CMD_TAB_END__, which, "");
    diag_printf("^P - Select previous line from history.\n");
	diag_printf("^N - Select next line from history.\n");
	diag_printf("^A - Move insertion [cursor] to start of line.\n");
	diag_printf("^E - Move cursor to end of line.\n");
	diag_printf("^B - Move cursor back [previous character].\n");
	diag_printf("^F - Move cursor forward [next character].\n");
	diag_printf("Press 'Ctrl+C' to go on!\n");
    return;
}

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

void
do_idle(bool is_idle)
{
    struct idle_tab_entry *idle_entry;

    for (idle_entry = __RedBoot_IDLE_TAB__; 
         idle_entry != &__RedBoot_IDLE_TAB_END__;  idle_entry++) {
        (*idle_entry->fun)(is_idle);
    }
}

void 
breakp(int num)
{
	int res = 0;
	bool prompt = true;
	static char line[256];
    char *command;
    struct cmd *cmd;
    console_echo = true;
    
#if 0    
    cyg_uint32 R0 = 0, R1 = 0, R2 = 0, R3 = 0;
    cyg_uint32 R4 = 0, R5 = 0, R6 = 0, R7 = 0;
	asm volatile(								
		"mov r0, %0	  			  				\n\t"
		"mov r1, %1	  			  				\n\t"
		"mov r2, %2	  			  				\n\t"
		"mov r3, %3	  			  				\n\t"
		"mov r4, %4	  			  				\n\t"
		"mov r5, %5	  			  				\n\t"
		"mov r6, %6	  			  				\n\t"
		:"=r"(R0),"=r"(R1),"=r"(R2),
		 "=r"(R3),"=r"(R4),"=r"(R5),
		 "=r"(R6)
		:
		:"r0","r1","r2","r3","r4","r5","r6"
	);
	diag_printf("R0 = %p\n", R0);
	diag_printf("R1 = %p\n", R1);
	diag_printf("R2 = %p\n", R2);
	diag_printf("R3 = %p\n", R3);
	diag_printf("R4 = %p\n", R4);
	diag_printf("R5 = %p\n", R5);
	diag_printf("R6 = %p\n", R6);
	diag_printf("R7 = %p\n", R7);
#endif

//    cyg_mutex_init(&cliblock);
	
//	diag_printf("Press 'Ctrl+C' to go on!\n");
	drv_delay(12000);
	
	while (true) {
        if (prompt) {
//    		cyg_mutex_lock(&cliblock);
            diag_printf("BreakP-%d> ", num);
//          cyg_mutex_unlock(&cliblock);
            prompt = false;
        }
		cmd_history = true;  // Enable history collection
        res = _rb_gets(line, sizeof(line), 10);
		cmd_history = false;  // Disable history collection
        if (res == _GETS_TIMEOUT) {
            // No input arrived
        }
        else if (res == _GETS_CTRLC) {	//press 'Ctrl+C' to go on.
          break;
    	}
    	else {
        	expand_aliases(line, sizeof(line));
	   		command = (char *)&line;
        	if ((*command == '#') || (*command == '=')) {
            // Special cases
            	if (*command == '=') {
                // Print line on console
                	diag_printf("%s\n", &line[2]);
            	}
        	}
        	else {
            	while (strlen(command) > 0) {
                	if ((cmd = parse(&command, &argc, &argv[0])) != (struct cmd *)0) {
                    	(cmd->fun)(argc, argv);
                	} 
                	else {
                    	diag_printf("** Error: Illegal command: \"%s\"\n", argv[0]);
                	}
            	}
        	}
        	prompt = true;
        }
    }
}
