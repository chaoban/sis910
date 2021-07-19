##==========================================================================
##
##      hal_var_bank.inc
##
##      SH support code for variants using register banks
##
##==========================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
##
## Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
## at http://sources.redhat.com/ecos/ecos-license/
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##==========================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):    jskov
## Contributors: jskov
## Date:         2002-01-11
## Purpose:      SH support code for variants using register banks
##
######DESCRIPTIONEND####
##
##==========================================================================

#---------------------------------------------------------------------------
# The models with banked registers jump directly to code without storing
# anything on the stack. Instead, the CPU switches to use banked registers
# leaving initial saving of state to the VSRs.

# reset            0xa0000000
# other exceptions VBR+0x100
# tlb miss         VBR+0x400
# interrupts       VBR+0x600

#===========================================================================
# Reset code must be PC relative so it can be executed out of the shadow
# area during startup. Not until after hal_hardware_init can the system
# be expected to provide the proper address space (at that time we
# jump to the VMA base of the code).
        .org    0x000
FUNC_START(_reset)
		bra powerup
			nop
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
#chaoban test, Add Interrupt Table Jump. A 'nop' is 2-bytes.
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop

			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop

			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop

			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop

			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop

			nop
			nop
			nop
			nop
			nop
#endif //CYG_HAL_STARTUP
powerup:
#ifdef __CHAOBAN_RC
#		UART_INITIAL
#endif
        mov.l   $_reset_platform,r0  
        jmp     @r0
        nop
# Note: this is the unmapped, shadow address of the start of code
# Sadly, it is too far to just branch to.                        
        .align   2
$_reset_platform:
#ifdef CYG_HAL_STARTUP_RAM
//	  .long	CYG_LABEL_DEFN(_reset_platform)-CYGMEM_REGION_ram+CYGMEM_REGION_rom
      .long   CYG_LABEL_DEFN(_reset_platform)
#elif defined(CYG_HAL_STARTUP_ROMRAM)
        // Uncached "shadow" address but adjusted for VMA/LMA differences
        .long   CYG_LABEL_DEFN(_reset_platform)+CYGMEM_REGION_rom-CYGMEM_REGION_ram
#else //CYG_HAL_STARTUP_ROM
        // Uncached "shadow" address
    .long   CYG_LABEL_DEFN(_reset_platform)
#endif
            
#---------------------------------------------------------------------------
# Exception entry
        .org    0x100
__exception:

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov     #1,r7
#endif

        mov.l   $nCYGARC_REG_EXCEVT2,r1
        mov.l   @r1,r1
        shlr2   r1
        shlr    r1                      ! divide cause by 0x08
        mov.l   $hal_vsr_table2,r0
        mov.l   @(r0,r1),r1
        jmp     @r1
         nop
        
        .align   2
        SYM_PTR_REFn(hal_vsr_table,2)

#---------------------------------------------------------------------------
# TLB miss entry
#        .org    0x400
#chaoban test
		 .org    0x200	
__tlb_miss:
#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov      #2,r7
#endif

        mov.l   $nCYGARC_REG_EXCEVT2,r1
        mov.l   @r1,r1
        shlr2   r1
        shlr    r1                      ! divide cause by 0x08
        mov.l   $hal_vsr_table3,r0
        mov.l   @(r0,r1),r1
        jmp     @r1
         nop

        .align   2
        SYM_PTR_REFn(hal_vsr_table,3)
$nCYGARC_REG_EXCEVT2:
        .long   CYGARC_REG_EXCEVT

#---------------------------------------------------------------------------
# Interrupt entry
#        .org    0x600
#chaoban test
		 .org    0x400
__interrupt:

#ifdef CYGHWR_HAL_SH_IRQ_HANDLE_SPURIOUS_INTERRUPTS
        # A spurious interrupt with INTEVT=0 may be caused by
        # clearing of BL. Those interrupts need to be ignored.
        mov.l   1f,r0
        mov.l   @r0,r0
        cmp/eq  #0,r0
        bf      2f
        rte
         nop
	.align  2
1:	.long   CYGARC_REG_INTEVT
2:      
#endif        

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov      #3,r7
#endif
		
LED_1
!rte
!nop

        mov.l    $hal_vsr_table_int,r1
        mov.l    @r1,r1
        jmp      @r1
         nop

.align   4
$hal_vsr_table_int:
        .long    CYG_LABEL_DEFN(hal_vsr_table)+CYGNUM_HAL_VECTOR_INTERRUPT*4

#ifdef CYG_HAL_DDRII

#else
.align 4
LED_val:	.long	0x9000a000
#endif

##============================================================================
## Macros for saving/restoring register state on an exception.

#---------------------------------------------------------------------------
## Save registers on exception:
## At entry:
## r15 is location to be stored to
## r0  is the available scratch register

## At exit:
## r7  is the vector #
## all other registers (except sp) are available

	.macro hal_cpu_save_regs
	
        # We come here with all register containing their
        # pre-exception values except:
        # r0_b-r7_b  = saved r0-r7
        # r7    = vector #
        # spc   = saved pc
        # ssr   = saved sr
      
        # Save away some registers
        mov     r15,r1                  ! entry sp
#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        add     #-12,r15                ! Space for cause, gbr, and vbr
#else
        add     #-8,r15                 ! Space for cause, vbr

        stc     gbr,r0                  ! GBR
        mov.l   r0,@-r15
//chaoban test        
//        stc     vbr,r0                  ! VBR
//        mov.l   r0,@-r15
#endif

        stc     spc,r0
        mov.l   r0,@-r15                ! PC
        stc     ssr,r0
        mov.l   r0,@-r15                ! SR

        sts.l   mach,@-r15              ! mach
        sts.l   macl,@-r15              ! macl

        mov.l   r1,@-r15                ! entry sp

//        sts.l   pr,@-r15                ! PR
//        mov.l   r14,@-r15               ! r14-r0
//        mov.l   r13,@-r15
//        mov.l   r12,@-r15
//        mov.l   r11,@-r15
//        mov.l   r10,@-r15
//        mov.l   r9,@-r15
//        mov.l   r8,@-r15
//        mov.l   r7,@-r15
//        mov.l   r6,@-r15
//        mov.l   r5,@-r15
//        mov.l   r4,@-r15
//        mov.l   r3,@-r15
//        mov.l   r2,@-r15
//        mov.l   r1,@-r15
//        mov.l   r0,@-r15

        trapa   #0xFC                   !into user mode
         nop
        sts.l   pr,@-r15                ! PR
        mov.l   r14,@-r15               ! r14-r0
        mov.l   r13,@-r15
        mov.l   r12,@-r15
        mov.l   r11,@-r15
        mov.l   r10,@-r15
        mov.l   r9,@-r15
        mov.l   r8,@-r15
        mov.l   r7,@-r15
        mov.l   r6,@-r15
        mov.l   r5,@-r15
        mov.l   r4,@-r15
        mov.l   r3,@-r15
        mov.l   r2,@-r15
        mov.l   r1,@-r15
        mov.l   r0,@-r15
        trapa   #0xFD                   !into kernel mode
         nop

        # After setting the SR it will be possible to use breakpoints.
        mov.l   1f,r1
        ldc     r1,sr
	bra	2f
	 nop
        .align  2        
#1:	.long   CYG_SR #0x400000f0
#chaoban test
1:	.long	0x00001000
        # ------------------------------------------------------------
        # Register bank has changed now. 
2:
	.endm
	
# No additional magic needed. hal_cpu_save_regs does it all.	
	.macro hal_exception_entry_extras
	.endm
	.macro hal_interrupt_entry_extras
	.endm
    
#---------------------------------------------------------------------------
## Restore registers after exception:
## At entry:
## r15 is location to be loaded from
## all other registers (except sp) are available
    
## At exit:
##  Returns to interrupted code

	.macro hal_cpu_restore_regs_return
        # Disable interrupts and switch register bank during the restore 
        # operation
        mov.l   1f,r1
        ldc     r1,sr
        # ------------------------------------------------------------
        # Register bank has changed now. 

        #
        # Single step debugging becomes impossible after this point!
        #
#        mov     r15,r0

        trapa   #0xFC                   !into user mode
         nop
        mov.l   @r15+,r0
        mov.l   @r15+,r1
        mov.l   @r15+,r2
        mov.l   @r15+,r3
        mov.l   @r15+,r4
        mov.l   @r15+,r5
        mov.l   @r15+,r6
        mov.l   @r15+,r7
        mov.l   @r15+,r8
        mov.l   @r15+,r9
        mov.l   @r15+,r10
        mov.l   @r15+,r11
        mov.l   @r15+,r12
        mov.l   @r15+,r13
        mov.l   @r15+,r14
        lds.l   @r15+,pr                 ! PR

        trapa   #0xFD                   !into kernel mode
         nop

//        mov.l   @r15+,r0
//        mov.l   @r15+,r1
//        mov.l   @r15+,r2
//        mov.l   @r15+,r3
//        mov.l   @r15+,r4
//        mov.l   @r15+,r5
//        mov.l   @r15+,r6
//        mov.l   @r15+,r7
//        mov.l   @r15+,r8
//        mov.l   @r15+,r9
//        mov.l   @r15+,r10
//        mov.l   @r15+,r11
//        mov.l   @r15+,r12
//        mov.l   @r15+,r13
//        mov.l   @r15+,r14
//        lds.l   @r15+,pr                 ! PR

        mov.l   @r15+,r1                ! return SP

        lds.l   @r15+,macl               ! macl
        lds.l   @r15+,mach               ! mach

        mov.l   @r15+,r2
        ldc     r2,ssr                  ! return SR
        mov.l   @r15+,r2
        ldc     r2,spc                  ! return PC

#ifndef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
//chaoban test
//        mov.l   @r15+,r2
//        ldc     r2,vbr                  ! return VBR
        mov.l   @r15+,r2
        ldc     r2,gbr                  ! return GBR
        add     #8,r15                 ! Space for cause, vbr
#else
        add     #12,r15                ! Space for cause, gbr, and vbr
#endif
        mov     r1,r15                  ! entry sp
        
        rte
         nop
        .align  2        
1:	.long   CYG_SR_BANK1

	.endm

#---------------------------------------------------------------------------
# Translate cause of exception to a vector number
	.macro hal_exception_translate
        mov.l   1f,r0
        mov.l   @r0,r4
        mov     #-5,r0                  ! divide cause by 0x20
        shld    r0,r4
        mov     #CYGARC_SHREG_EVENT,r0
	bra     2f
         mov.l   r4,@(r0,r15)            ! store decoded vector number back
	.align  2
1:	.long   CYGARC_REG_EXCEVT
2:	
	.endm

#---------------------------------------------------------------------------
# end of hal_var_bank.inc
