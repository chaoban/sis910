#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H
//=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
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
// Author(s):    michael anburaj <michaelanburaj@hotmail.com>
// Contributors: michael anburaj <michaelanburaj@hotmail.com>
// Date:         2003-08-01
// Purpose:      ARM9/SMDK2440 platform specific support routines
// Description: 
// Usage:        #include <cyg/hal/hal_platform_setup.h>
//     Only used by "vectors.S"         
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/system.h>           // System-wide configuration info
#include CYGBLD_HAL_VARIANT_H         // Variant specific configuration
#include CYGBLD_HAL_PLATFORM_H        // Platform specific configuration
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/hal/hal_mmu.h>          // MMU definitions
#include <cyg/hal/s3c2440x.h>         // Platform specific hardware definitions
#include <cyg/hal/memcfg.h>           // Platform specific memory configuration


#if (CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK == 176000000)
#define M_MDIV      80                  // Fin=12.0MHz Fout=176.0MHz
#define M_PDIV      1
#define M_SDIV      1
#elif (CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK == 180000000)
#define M_MDIV      82                  // Fin=12.0MHz Fout=180.0MHz
#define M_PDIV      1
#define M_SDIV      1
#elif (CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK == 184000000)
#define M_MDIV      84                  // Fin=12.0MHz Fout=184.0MHz
#define M_PDIV      1
#define M_SDIV      1
#elif (CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK == 192000000)
#define M_MDIV      88                  // Fin=12.0MHz Fout=192.0MHz
#define M_PDIV      1
#define M_SDIV      1
#elif (CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK == 200000000)
#define M_MDIV      92                  // Fin=12.0MHz Fout=200.0MHz
#define M_PDIV      1
#define M_SDIV      1
#elif (CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK == 405000000)
#define M_MDIV      127
#define M_PDIV      2
#define M_SDIV      1
#else
#error CYGNUM_HAL_ARM_SMDK2440_CPU_CLOCK not set to the right value
#endif

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
#define PLATFORM_SETUP1 _platform_setup1
#define CYGHWR_HAL_ARM_HAS_MMU
#define CYGSEM_HAL_ROM_RESET_USES_JUMP

// We need this here - can't rely on a translation table until MMU has
// been initialized
        .macro RAW_LED_MACRO x
        ldr     r0,=GPFDAT
        ldr     r1,[r0]
        bic     r1,r1,#(0xf<<4)
        orr     r1,r1,#((0xf & ~(\x))<<4)
        str     r1, [r0]
        .endm

        // Configure GPF[4:7] as Output & pull-up turned off
        .macro RAW_LED_PORT_INIT_MACRO
        ldr     r0,=GPFUP
        ldr     r1,[r0]
        orr     r1,r1,#((1<<7)|(1<<6)|(1<<5)|(1<<4))
        str     r1,[r0]

        RAW_LED_MACRO 0

        ldr     r0,=GPFCON
        ldr     r1,[r0]
        orr     r1,r1,#((1<<14)|(1<<12)|(1<<10)|(1<<8))
        str     r1,[r0]
        .endm
        
        //For Chaoban test
        //Test FLASH/DRAM address can or not write incorrectly.
        .macro TestRAM_RW x
        RAW_LED_MACRO 0
        mov r6, #0xffffffff
        mov r7, #(\x) //memory address to write
        str r6, [r7]
        ldr r8, [r7]
        cmp r6, r8
        beq rw_ok
        blt rw_fail
rw_fail:
        RAW_LED_MACRO 0xe
        b rw_fail   //keep in loop        
rw_ok:
//      PASS, GO!!
        .endm
        
// This macro represents the initial startup code for the platform        
        .macro  _platform_setup1
#ifndef CYG_HAL_STARTUP_RAM
        ldr    r0,=WTCON            // watch dog disable 
        ldr    r1,=0x0         
        str    r1,[r0]
#endif
        RAW_LED_PORT_INIT_MACRO

#ifndef CYG_HAL_STARTUP_RAM
        ldr    r0,=INTMSK              //4a000008
        ldr    r1,=0xffffffff       // all interrupt disable
        str    r1,[r0]

        ldr    r0,=INTSUBMSK
        ldr    r1,=0x7ff            // all sub interrupt disable
        str    r1,[r0]

        // Disable and clear caches, defined in hal_mmu.h
        mrc  p15,0,r0,c1,c0,0
        bic  r0,r0,#0x1000              // disable ICache
        bic  r0,r0,#0xf                 // disable DCache, write buffer,
                                        // MMU and alignment faults
        mcr  p15,0,r0,c1,c0,0
        nop
        nop
        mov  r0,#0
        mcr  p15,0,r0,c7,c6,0           // clear data cache
#if 0
        mrc  p15,0,r0,c15,c1,0          // disable streaming
        orr  r0,r0,#0x80
        mcr  p15,0,r0,c15,c1,0
#endif
        // To reduce PLL lock time, adjust the LOCKTIME register. 
        ldr    r0,=LOCKTIME
        mvn    r1,#0xff000000
        str    r1,[r0]

        // We must set ratios, set memctl, then change FCLK.
        ldr     r0,=CLKDIVN     // Set ratios 1:2:4 for FCLK:HCLK:PCLK
        ldr     r1,=(7)         // 7, Set ratios 1:3:6 for FCLK:HCLK:PCLK
        str     r1,[r0]
        
//chaoban test, CLKCON
//      mov     r0,#LOCKTIME
//      ldr     r1,[r0,#0xc]  
//      orr     r1,r1,#0x8
//      str     r1,[r0,#0xc]

        // MMU_SetAsyncBusMode  //Must select, since we're setting HDIVN != 0
#define R1_iA   (1<<31)
#define R1_nF   (1<<30)
        ldr     r0,=LOCKTIME
        mrc     p15,0,r0,c1,c0,0
        orr     r0,r0,#(R1_nF|R1_iA) //0xc0000000, Asynchronous
        mcr     p15,0,r0,c1,c0,0

        // Set memory control registers
        adr    r0,1f
        ldr    r1,=BWSCON           // BWSCON Address
        add    r2, r0, #52          // End address of SMRDATA

0:
        ldr    r3, [r0], #4    
        str    r3, [r1], #4    
        cmp    r2, r0        
        bne    0b
        b      2f
1:
// Memory configuration should be optimized for best performance 
// The following parameter is not optimized.                     
// Memory access cycle parameter strategy
// 1) The memory settings is  safe parameters even at HCLK=75Mhz.
// 2) SDRAM refresh period is for HCLK=75Mhz. 
      // chaoban test
        .long 0x2211d110 //(0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
        .long ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))   //GCS0, 0x00000700
        .long ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))   //GCS1, 0x00000700 
        .long ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))   //GCS2, 0x00000700
      //  .long ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))   //GCS3, 0x00000700
        .long 0x00001f7c                                                                                  //GCS3, 0x00001f7c
        .long ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))   //GCS4, 0x00000700
        .long ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))   //GCS5, 0x00000700
        .long 0x00018009 //((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))    //GCS6, 0x00018005
        .long 0x00018009 //((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))    //GCS7, 0x00018005
     // .long ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Tsrc<<18)+(Tchr<<16)+REFCNT)
     // .long ((((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Tsrc<<18))&~0x3fff)+REFCNT) //chaoban test, see S3C2440 5-17
        .long 0x00ac03f4
     // .long 0x32                    // SCLK power saving mode, BANKSIZE 128M/128M
        .long 0xb2                    // chaoban test
        .long 0x30                    // MRSR6 CL=3clk
        .long 0x30                    // MRSR7
2:
        // Configure MPLL, see S3C2440 7-21
        ldr    r0,=MPLLCON
        ldr    r1,=((M_MDIV<<12)+(M_PDIV<<4)+M_SDIV)  // 127, 2, 1, r1=0x7f021
        str    r1,[r0]
#endif /* !CYG_HAL_STARTUP_RAM */

//reset NAND Controller
        mov     r1, #NFCONF                         //0x4e000000
        ldr	    r2, =((7<<12)|(7<<8)|(7<<4)|(0<<0))
        str	    r2, [r1, #0x00]
	    ldr	    r2, [r1, #0x04]
	    
	    ldr	    r2, =((1<<4)|(0<<1)|(1<<0))         //Active low CE Control 
	    str	    r2, [r1, #0x04]
	    ldr	    r2, [r1, #0x04]

	    ldr	    r2, =(0x6)                          //RnB Clear
	    str	    r2, [r1, #0x20]
	    ldr	    r2, [r1, #0x20]
	
	    mov	    r2, #0xff                           //RESET command, NFCMD
	    strb	r2, [r1, #0x08]
	    
	    mov	    r3, #0                              //Wait
1:	    add	    r3, r3, #0x1
	    cmp	    r3, #0xa
	    blt	    1b
	    
//2:	    ldr	    r2, [r1, #0x20]                     //Wait ready
//	    tst	    r2, #0x4
//	    beq	    2b

	    ldr	    r2, [r1, #0x04]
	    orr	    r2, r2, #0x2                        //Flash Memory Chip Disable
	    str	    r2, [r1, #0x04]
//set GPIO for UART	    
	    ldr	    r1, =GPHCON//0x56000070                       
	    ldr	    r2, =0x0016faaa	
	    str	    r2, [r1]
	    ldr     r1, =GPHUP//0x56000078
	    ldr	    r2, =0x000007ff
	    str	    r2, [r1]  
//Initialize UART
	    ldr	r1, =ULCON0
	    mov	r2, #0x0
	    str	r2, [r1, #0x08]
	    str	r2, [r1, #0x0c]
	    mov	r2, #0x3
	    str	r2, [r1, #0x00]
	    ldr	r2, =0x245
	    str	r2, [r1, #0x04]
#define UART_BRD ((67500000/(115200*16))-1)
	    mov	r2, #UART_BRD
	    str	r2, [r1, #0x28]	    
	    
	    mov	r3, #100
	    mov	r2, #0x0
1:	    sub	r3, r3, #0x1
	    tst	r2, r3
	    bne	1b	   
	    
//Chaoban test for serial port
//      ldr	r1, =UTXH0  //0x50000020, UART 0 Transmission Hold
//	    mov	r2, #'H'
//	    str	r2, [r1]

        // Set up a stack [for calling C code]
        ldr     r1,=__startup_stack
        ldr     r2,=SMDK2440_SDRAM_PHYS_BASE //0x30000000
        orr     sp,r1,r2                        //sp=r13

        // Create MMU tables
RAW_LED_MACRO 0x1        
        bl      hal_mmu_init
RAW_LED_MACRO 0x2
        // Enable MMU
        ldr     r2,=10f
#ifdef CYG_HAL_STARTUP_ROMRAM
        ldr     r1,=__exception_handlers    //in vectors.S, 0x000008000, =32KB
        ldr     r9,=0x80000000    //when MMU enable, r9 has ROM offset
        sub     r1,r2,r1
        add     r2,r9,r1
#endif  
        ldr     r1,=MMU_Control_Init|MMU_Control_M
        mcr     p15,0,r1,c1,c0
        mov     pc,r2    // Change address spaces,    Jump to ROM 
        nop
        nop
        nop

RAW_LED_MACRO 0x3

mov     r1, #NFCONF                        //0x4e000004, enable NAND
ldr	    r2, [r1,#0x4]
bic	    r2, r2, #0x2
orr     r2, r2, #0x1
str     r2, [r1,#0x4]
           
1: ldr	   r2, [r1,#0x20]                       //0x4e000020, CLEAR RB
   orr	   r2, r2, #0x4
   str	   r2, [r1,#0x20]                           
   mov     r2, #0x0                             //0x4e000008, NFCMD
   str	   r2, [r1,#0x8]

//2: ldr	    r2, [r1, #0x20]                     //Wait ready
//   tst	    r2, #0x4
//   beq	    2b

RAW_LED_MACRO 0x4
10:
#ifdef CYG_HAL_STARTUP_ROMRAM
        mov     r0,r9                     // Relocate FLASH/ROM to RAM
        ldr     r1,=__exception_handlers  // ram base & length, 0x8000
        ldr     r2,=__rom_data_end        // 0x159xx
20:     ldr     r3,[r0],#4
        str     r3,[r1],#4
        cmp     r1,r2
        bne     20b
        ldr     r0,=30f
        mov     pc,r0
        nop
        nop
        nop
        nop
30:
#endif
RAW_LED_MACRO 0xc //12
        .endm
#else // defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
#define PLATFORM_SETUP1
#endif
//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
 