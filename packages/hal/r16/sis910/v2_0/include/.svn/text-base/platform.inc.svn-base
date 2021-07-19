#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	SiS910 board assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 2003 Bart Veer
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
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	jskov
## Contributors:jskov
## Date:	2000-02-02
## Purpose:	SH/EDK7708 board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the SH/EDK7708 board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/sh_regs.h>
#include <cyg/hal/sh_offsets.inc>
#include <cyg/hal/hal_misc.h>

#------------------------------------------------------------------------------
.macro _DEBUG_HWTEST
	mov 	#0, r7
bra	uart_complete
	nop

uart_tx_dword:					! uart transmit r12
	mov.l	uart_iio_tx, r9		! r9 point to uart iio tx
	mov		#0x08, r13

uart_tx_convert:				! convert to ASCII
	mov 	r12, r10
	shlr16	r10
	shlr8	r10
	shlr2	r10
	shlr2	r10
	mov		#0x0f, r11
	and		r11, r10

	add 	#0x30, r10	! '0' ~ '9'

	mov		#0x39, r11
	cmp/gt	r11, r10
	bf		uart_tx_byte
	add 	#0x27, r10	! 'A' ~ 'F'

uart_tx_byte:
	mov.l	uart_status_txfull, r11
	mov.l	uart_status, r9

uart_tx_wait:		! wait uart tx
	mov.l	@r9, r8
	tst		r11, r8
	bt 		uart_tx_wait
	nop

	mov.l	uart_iio_tx, r9		! r9 point to uart iio tx
	mov.l 	r10, @r9

	shll2   r12
	shll2   r12
	dt 		r13
	bf 		uart_tx_convert
	nop

	rts
	nop

uart_tx_single_byte:
 	mov.l 	uart_cycle, r11
uart_tx_wait3:		! wait uart tx
 	nop
 	dt		r11
 	bf 		uart_tx_wait3
	nop

	mov.l	uart_iio_tx, r9		! r9 point to uart iio tx
 	mov.l 	r12, @r9
	rts
	nop

uart_complete:

	add 	#1, r7

!!! BIST Test
		mov.l 	bist_log, r3
		mov.l	bist_log_size, r4
		mov		#0, r5

!! clear 2KB internal SRAM
bist_log_clear:
		mov.l 	r5, @r3
		add		#4, r3
		dt		r4
		bf		bist_log_clear
		nop
!!
		mov.l	bist_begin, r0	! bist begin address
		mov.l	bist_end, r2	! bist end address

!! Store value is its address.
bist_write:
		! shuffle address
		mov		r0, r1
		shll16  r1
		or 		r0, r1

		mov.l	r1, @r0
		nop
		add		#4, r0
		cmp/eq	r0, r2
		bf	bist_write
		nop


!! initial
		mov.l	bist_begin, r0
		mov.l	bist_end, r2
		mov.l	bist_log, r3	! r3 = last fail address

bist_test:
		mov.l	@r0, r1

		! shuffle address
		mov		r0, r4
		shll16  r4
		or 		r0, r4

		nop
		cmp/eq	r4, r1
		bt 	bist_pass
		nop

bist_error:						! write fail address & data @ bist_log
		mov.l r0, @r3
		add #4, r3
		mov.l r1, @r3
		add #4, r3

bist_error_uart:
		bsr uart_tx_dword		! display fail address & data @ uart
		mov	r0, r12

		bsr uart_tx_single_byte
		mov	#' ', r12

		bsr uart_tx_dword
		mov	r1, r12

		bsr uart_tx_single_byte
		mov	#0x0d, r12

		bsr uart_tx_single_byte
		mov	#0x0a, r12

#ifdef __CHAOBAN_RC
		!reset DC
		mov.l	dc_reset_reg, r12
		mov.l	dc_reset_1, r11
		mov.l 	r11, @r12
		nop
		nop
		nop
		mov.l	dc_reset_0, r11
		mov.l 	r11, @r12

		!reset DC_PHY
		mov.l	phyreg4_t, r12
		mov.l	rtt_tune_1, r11
		mov.l 	r11, @r12
		nop
		nop
		nop
		mov.l	rtt_tune_t, r11
		mov.l 	r11, @r12
#endif

fail:
		bra fail
!		bra uart_complete
		 nop

bist_pass:						! continue bist test if addr < bist_end
		add	#4, r0
		cmp/eq	r0, r2
		bf	bist_test
		nop

bist_complete:					! display 'End' to indicate bist complete
		bsr uart_tx_single_byte
		mov	#'T', r12

		bsr uart_tx_single_byte
		mov	#'e', r12

		bsr uart_tx_single_byte
		mov	#'s', r12

		bsr uart_tx_single_byte
		mov	#'t', r12

		bsr uart_tx_single_byte
		mov	#' ', r12

		bsr uart_tx_single_byte
		mov	#'E', r12

		bsr uart_tx_single_byte
		mov	#'n', r12

		bsr uart_tx_single_byte
		mov	#'d', r12

		bsr uart_tx_single_byte
		mov	#' ', r12

		bsr uart_tx_dword
		mov	r7, r12

		bsr uart_tx_single_byte
		mov	#' ', r12

		bsr uart_tx_single_byte
		mov	#'t', r12

		bsr uart_tx_single_byte
		mov	#'i', r12

		bsr uart_tx_single_byte
		mov	#'m', r12

		bsr uart_tx_single_byte
		mov	#'e', r12

		bsr uart_tx_single_byte
		mov	#'s', r12

		bsr uart_tx_single_byte
		mov	#0x0d, r12

		bsr uart_tx_single_byte
		mov	#0x0a, r12

	bra uart_complete
!	bra 1f
	 nop

	.align 2
	uart_status:		.long	0x90003710
	uart_status_txfull:	.long	0x00002000
	uart_iio_tx:		.long	0x90003704
	uart_cycle:			.long	0x00010000

! bist setting
#ifdef __CHAOBAN_RC
	bist_begin:			.long	0x00000000
	bist_end:			.long	0x00000000 | __RC_RAMSIZE
#else
	bist_begin:			.long	0x00000000
	bist_end:			.long	0x00000000 | SZ_32M
#endif
	bist_log:			.long   0x90040000
	bist_log_size:		.long	0x00000800

	dc_reset_reg:		.long	0x90004008
	dc_reset_1:			.long   0x00400000
	dc_reset_0:			.long   0x00000000
	rtt_tune_1:			.long	0x007ff800
	rtt_tune_t:			.long	0x007ff80c
	phyreg4_t:        	.long   0x9000f010
1:
	.endm

	.macro UART_INITIAL
#ifndef __CHAOBAN_RC
		mov.l	HW_reg0, r0
		mov.l	HW_val0, r1
		mov.l	r1,@r0
#endif
		mov.l	uart_3708, r0
		mov.l 	reg_3708, r1
		mov.l 	r1, @r0
		mov.l	uart_3728, r0
		mov.l 	reg_3728, r1
		mov.l 	r1, @r0
		mov.l	uart_3714, r0
		mov.l 	reg_3714, r1
		mov.l 	r1, @r0
		bra init_ok
		 nop

		.align 4
#ifndef __CHAOBAN_RC
		HW_reg0:		.long	0x90004014
		HW_val0:		.long	0xc20e2c72
#endif
		uart_3708:		.long	0x90003708
		reg_3708:		.long 	0x03001b03
		uart_3728:		.long	0x90003728
		reg_3728:		.long	0x00000020
		uart_3714:		.long	0x90003714
#ifdef __CHAOBAN_RC
		reg_3714:		.long	0x36	! 200MHZ
#else
		reg_3714:		.long	0xa
#endif
init_ok:
	.endm


// Assembly codes of Getch by UART
	.macro GETCHC
		UART_INITIAL

		mov.l	UARTSTAREG, r1	! 0x90003710
		mov.l	LSR_DR, r2
DR_LOOP:
		mov.l	@r1, r3
		mov.l	@r1, r3
		tst		r2, r3
		bt		DR_LOOP
		 nop
		mov.l	UARTRXFIFO, r1	! 0x90003700
		mov.l	UARTCTLREG, r2	! 0x90003708
		mov.l	@r1,r0
		exts.b	r0,r0
		mov.l	UARTTXFIFO, r1	! 0x90003704
		mov.l	r0, @r1
		mov.l	@r2, r3
		mov.l	@r2, r3
		mov.l	CLEAR_RCVR, r1
		or		r1, r3
		mov.l	r3, @r2
		bra		2f
	 	 nop

		.align 4
	UARTRXFIFO:		.long	0x90003700
	UARTTXFIFO:		.long	0x90003704
	UARTCTLREG:		.long	0x90003708
	UARTSTAREG:		.long	0x90003710
	LSR_DR:			.long	0x100
	CLEAR_RCVR:		.long	0x10000
2:
	.endm

	.macro LOADUART
        mov.l   RXFIFO, r5
        mov.l   nc_addr0, r14
		mov.l   cnt_1k, r13
ld_4kbyte:
        mov.l   @r5, r8
        mov.l   r8, @r14
        mov.l   @r14, r9
        cmp/eq  r8, r9
        bt      next_4byte
         nop
next_4byte:                     ! compare OK
        add     #0x04, r14
        dt      r13
        bf      ld_4kbyte
       	 nop
		bra ld_4kbyte
		 nop

        .align  4
nc_addr0:       .long   0x40000000
cnt_1k:         .long   0x00000400
RXFIFO:			.long	0x90003700
2:
	.endm

##-----------------------------------------------------------------------------
## Hardware init macros

.macro uart_test
// Send TX
	mov.l	TX_FIFO_iio, r0
	mov #'U', r1
	bsr uart_tx
	 nop
	mov #'R', r1
	bsr uart_tx
	 nop
	mov #'O', r1
	bsr uart_tx
	 nop
	mov #'K', r1
	bsr uart_tx
	 nop
	mov #0x0d, r1
	bsr uart_tx
	 nop
	mov #0x0a, r1
	bsr uart_tx
	 nop
	bra 1f
	 nop

uart_tx:
	mov.l 	r1, @r0
 	mov.l 	uart_cycles, r11
2:	nop
 	dt		r11
 	bf 		2b
	 nop
	rts
	 nop

	.align 4
	TX_FIFO_iio:	.long	0x90003704
	uart_cycles:	.long	0x100000
1:
.endm


## Chaoban test uart
	.macro TEST_UART
// Send TX
	mov.l		TX_FIFO_reg, r2
	mov.l		TX_FIFO_data, r1
	mov.l		r1,@r2
	bra 1f
	 nop

	.align 4
	TX_FIFO_reg:	.long	0x90003704
	TX_FIFO_data:	.long	0x4B4F5255		! UART OK.
1:
	.endm

//#define _TEST_NONCACHE

.macro _DEBUG_TEST_RAM
start:
		mov		#0x30, r3
		mov		#0x39, r10
retest:
		add		#1, r3
		cmp/eq	r3, r10
		bt start
		 nop
		bra times
         nop
1:
        mov.l   ram_start, r4    ! r4 = ram start
        mov.l   ram_end, r5      ! r5 = ram end
2:
//		mov   	#0, r0
//        mov.l   r0, @r4          ! store in RAM
//         nop
		mov   	#0xaa, r0
        mov.l   r0, @r4          ! store in RAM
#if 0
        mov.l	@r4, r1
        cmp/eq  r1, r0
        bf Print_error
         nop
#endif
		add     #4, r4
        cmp/eq  r4, r5            ! compare
        bf		2b			  	  ! loop if not yet done
         nop
#if 1
        bra Print_write_ok
         nop
wok:
        mov   	#0xaa, r0
        mov.l   ram_start, r4    ! r4 = ram start
3:      mov.l	@r4, r1
        cmp/eq  r1, r0
        bf Print_error
         nop
        add     #4, r4
        cmp/eq  r4, r5
        bf		3b
         nop
         bra Print_read_ok
         nop
#endif
		bra	retest
		 nop
times:
		mov.l	TX_iio, r0
		mov #'T', r1
		bsr uart_t
	 	nop
		mov #'E', r1
		bsr uart_t
	 	nop
		mov #'S', r1
		bsr uart_t
	 	nop
		mov #'T', r1
		bsr uart_t
	 	nop
	 	mov #' ', r1
		bsr uart_t
	 	nop
#ifdef _TEST_NONCACHE
		mov #'N', r1
		bsr uart_t
	 	nop
	 	mov #'O', r1
		bsr uart_t
	 	nop
	 	mov #'N', r1
		bsr uart_t
	 	nop
	 	mov #'C', r1
		bsr uart_t
	 	nop
	 	mov #'A', r1
		bsr uart_t
	 	nop
	 	mov #'H', r1
		bsr uart_t
	 	nop
	 	mov #' ', r1
		bsr uart_t
	 	nop
#endif
	 	mov #'R', r1
		bsr uart_t
	 	nop
	 	mov #'A', r1
		bsr uart_t
	 	nop
	 	mov #'M', r1
		bsr uart_t
	 	nop
	 	mov #' ', r1
		bsr uart_t
	 	nop
	 	mov r3, r1
		bsr uart_t
	 	nop
		mov #0x0d, r1
		bsr uart_t
	 	nop
		mov #0x0a, r1
		bsr uart_t
	 	nop
		bra 1b
		nop

Print_write_ok:
		mov.l	TX_iio, r0
		mov #'W', r1
		bsr uart_t
	 	nop
	 	mov #'O', r1
		bsr uart_t
	 	nop
	 	mov #'K', r1
		bsr uart_t
	 	nop
	 	mov #0x0d, r1
		bsr uart_t
	 	nop
		mov #0x0a, r1
		bsr uart_t
		nop
		bra wok
		 nop

Print_read_ok:
		mov.l	TX_iio, r0
		mov #'R', r1
		bsr uart_t
	 	nop
	 	mov #'O', r1
		bsr uart_t
	 	nop
	 	mov #'K', r1
		bsr uart_t
	 	nop
	 	mov #0x0d, r1
		bsr uart_t
	 	nop
		mov #0x0a, r1
		bsr uart_t
		nop
		bra retest
		 nop

Print_error:
		mov.l	TX_iio, r0
		mov #'F', r1
		bsr uart_t
	 	nop
		mov #'A', r1
		bsr uart_t
	 	nop
		mov #'I', r1
		bsr uart_t
	 	nop
		mov #'L', r1
		bsr uart_t
	 	nop
	 	mov #' ', r1
		bsr uart_t
	 	nop
	 	mov #'A', r1
		bsr uart_t
	 	nop
	 	mov #'D', r1
		bsr uart_t
	 	nop
	 	mov #'D', r1
		bsr uart_t
	 	nop
	 	mov #'R', r1
		bsr uart_t
	 	nop
	 	mov #' ', r1
		bsr uart_t
	 	nop
		mov #0x0d, r1
		bsr uart_t
	 	nop
		mov #0x0a, r1
		bsr uart_t
hang:	nop

		bra hang
		 nop

uart_t:
		mov.l 	r1, @r0
 		mov.l 	cycle, r11
4:		nop
 		dt		r11
 		bf 		4b
	 	nop
		rts
	 	nop

		.align 4
		TX_iio:		.long	0x90003704
		cycle:		.long	0x10000
#ifdef _TEST_NONCACHE
		ram_start:	.long	0x40000000
#else
		ram_start:	.long	0x00000000
#endif

#ifdef __CHAOBAN_RC
#ifdef _TEST_NONCACHE
		ram_end:	.long	0x40000000 | SZ_64M
#else
		ram_end:	.long	0x00000000 | SZ_64M
#endif
#else
#ifdef _TEST_NONCACHE
		ram_end:	.long	0x40000000 | SZ_16M
#else
		ram_end:	.long	0x00000000 | SZ_16M
#endif
#endif
.endm


#chaoban test LED
#ifdef __CHAOBAN_RC
	.macro LED_INIT
	.endm
	.macro LED_0
	.endm
	.macro LED_1
	.endm
	.macro LED_2
	.endm
	.macro LED_3
	.endm
	.macro LED_4
	.endm
	.macro LED_5
	.endm
	.macro LED_6
	.endm
	.macro LED_7
	.endm
	.macro LED_8
	.endm
	.macro LED_9
	.endm
	.macro LED_A
	.endm
	.macro LED_B
	.endm
	.macro LED_C
	.endm
	.macro LED_D
	.endm
	.macro LED_E
	.endm
	.macro LED_F
	.endm
#else
#ifdef CYG_HAL_DDRII
	.macro LED_INIT
		mov.l	rGPIOEN, r5
		mov		#0x3f, r2
		mov.l	r2, @r5
		mov.l	rGPIOOP, r5
		mov		#0x3f, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOEN:			.long	0x90003740
		rGPIOOP:			.long	0x9000374c
1:
	.endm
	.macro LED_0
		mov.l	rGPIOOV0, r5
		mov		#64, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV0:			.long	0x90003748
1:
	.endm
	.macro LED_1
		mov.l	rGPIOOV1, r5
		mov		#0x79, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV1:			.long	0x90003748
1:
	.endm
	.macro LED_2
		mov.l	rGPIOOV2, r5
		mov		#36, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV2:			.long	0x90003748
1:
	.endm
	.macro LED_3
		mov.l	rGPIOOV3, r5
		mov		#48, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV3:			.long	0x90003748
1:
	.endm
	.macro LED_4
		mov.l	rGPIOOV4, r5
		mov		#25, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV4:			.long	0x90003748
1:
	.endm
	.macro LED_5
		mov.l	rGPIOOV5, r5
		mov		#18, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV5:			.long	0x90003748
1:
	.endm
	.macro LED_6
		mov.l	rGPIOOV6, r5
		mov		#2, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV6:			.long	0x90003748
1:
	.endm
	.macro LED_7
		mov.l	rGPIOOV7, r5
		mov		#120, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV7:			.long	0x90003748
1:
	.endm
	.macro LED_8
		mov.l	rGPIOOV8, r5
		mov		#0, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV8:			.long	0x90003748
1:
	.endm
	.macro LED_9
		mov.l	rGPIOOV9, r5
		mov		#16, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOV9:			.long	0x90003748
1:
	.endm
	.macro LED_A
		mov.l	rGPIOOVA, r5
		mov		#8, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVA:			.long	0x90003748
1:
	.endm
	.macro LED_B
		mov.l	rGPIOOVB, r5
		mov		#3, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVB:			.long	0x90003748
1:
	.endm
	.macro LED_C
		mov.l	rGPIOOVC, r5
		mov		#70, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVC:			.long	0x90003748
1:
	.endm
	.macro LED_D
		mov.l	rGPIOOVD, r5
		mov		#33, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVD:			.long	0x90003748
1:
	.endm
	.macro LED_E
		mov.l	rGPIOOVE, r5
		mov		#6, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVE:			.long	0x90003748
1:
	.endm
	.macro LED_F
		mov.l	rGPIOOVF, r5
		mov		#14, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVF:			.long	0x90003748
1:
	.endm
	.macro LED_X
		mov.l	rGPIOOVX, r5
		mov		#0x4f, r2
		mov.l	r2, @r5

		bra 1f
	 	 nop

		.align 4
		rGPIOOVX:			.long	0x90003748
1:
	.endm
#else
	.macro LED_INIT
	.endm
	.macro LED_0
		mov.l	reg_LED0, r5
		mov		#0,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED0:			.long	0x9000a000
1:
	.endm
	.macro LED_1
		mov.l	reg_LED1, r5
		mov		#1,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED1:			.long	0x9000a000
1:
	.endm
	.macro LED_2
		mov.l	reg_LED2, r5
		mov		#2,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED2:			.long	0x9000a000
1:
	.endm
	.macro LED_3
		mov.l	reg_LED3, r5
		mov		#3,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED3:			.long	0x9000a000
1:
	.endm
	.macro LED_4
		mov.l	reg_LED4, r5
		mov		#4,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED4:			.long	0x9000a000
1:
	.endm
	.macro LED_5
		mov.l	reg_LED5, r5
		mov		#5,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED5:			.long	0x9000a000
1:
	.endm
	.macro LED_6
		mov.l	reg_LED6, r5
		mov		#6,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED6:			.long	0x9000a000
1:
	.endm
	.macro LED_7
		mov.l	reg_LED7, r5
		mov		#7,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED7:			.long	0x9000a000
1:
	.endm
	.macro LED_8
		mov.l	reg_LED8, r5
		mov		#8,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED8:			.long	0x9000a000
1:
	.endm
	.macro LED_9
		mov.l	reg_LED9, r5
		mov		#9,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LED9:			.long	0x9000a000
1:
	.endm
	.macro LED_A
		mov.l	reg_LEDA, r5
		mov		#10,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LEDA:			.long	0x9000a000
1:
	.endm
	.macro LED_B
		mov.l	reg_LEDB, r5
		mov		#11,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LEDB:			.long	0x9000a000
1:
	.endm
	.macro LED_C
		mov.l	reg_LEDC, r5
		mov		#12,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LEDC:			.long	0x9000a000
1:
	.endm
	.macro LED_D
		mov.l	reg_LEDD, r5
		mov		#13,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LEDD:			.long	0x9000a000
1:
	.endm
	.macro LED_E
		mov.l	reg_LEDE, r5
		mov		#14,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LEDE:			.long	0x9000a000
1:
	.endm
	.macro LED_F
		mov.l	reg_LEDF, r5
		mov		#15,r2
		mov.l	r2,@r5

		bra 1f
	 	 nop

		.align 4
		reg_LEDF:			.long	0x9000a000
1:
	.endm
#endif
#endif

#------------------------------------------------------------------------------
		.macro hal_dram_init
#ifdef CYG_HAL_DDRII
   		trapa   #0xfc                   ! get into user mode
        .word   0x8226                  ! GBR=2600, DDR_PHY setup
        mov.l   rom_delay, r1
#ifdef __CHAOBAN_RC
!
		mov.l	usb_reg, r12
		mov.l 	usb_host, r11
		mov.l 	r11, @r12
		nop
		mov.l   usb_host_reg, r11
		mov.l   osc_enres, r10
		mov.l   r10, @r11

		mov.l   phyreg0, r3
        mov.l   iopd_setup, r0			! set CH0_STBY_IO_PD , CHO_MA_IO_PD and CH0_DQ_IO_PD to '0'
        mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay
										!
		mov.l   enable_ck_oe, r0		! set R_CH0_CK_OE to '1'
        mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay
										!
		mov.l   enable_cmd_oe, r0		! set R_CH0_CMD_OE to '1'
        mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay
										!
										! Enable DQS DLL initialization

		mov.l   ck_phase, r0  		    ! set R_CH0_CK_PHASE to '10'
		mov.l   phyreg1, r3
		mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay

		mov.l   rtt_tune, r0			! set R_DYNA_DQS_INDLY to '1'
		mov.l   phyreg4, r3
		mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay

		mov.l   enable_dqs_dll, r0		! set R_DYNA_DQS_INDLY to '1'
		mov.l   phyreg5, r3
		mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay
										!
		mov.l   setup_dqs_dll, r0		! set vaules for R_DQSDLL_TRAP_RANGE, R_DQSDLL_COUNT, R_DQSDLL_INT_RANGE and R_DQSDLL_DLY_CANCEL
		mov.l   phyreg6, r3
		mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2                 ! delay
		!
		mov.l   dqs_dll_rstn, r0		! De-assert R_DQSDLL_RSTN
		mov.l   phyreg5, r3
		mov.l   r0, @(0x0, r3)
		mov.l   @r1, r2					! delay
										! End DQS DLL sequence
										!
		mov.l	edqs_tune, r0			! Tune EDQS_PHASE, EDQS_DLY setting
		mov.l	phyreg7, r3
		mov.l	r0, @(0x0, r3)
		mov.l	@r1, r2					! delay
										! End EDQS tunning
										!
		mov.l	madrv_tune, r0			! Tune MA Driving Strength
		mov.l	phyreg2, r3
		mov.l	r0, @(0x0, r3)
		mov.l	@r1, r2					! delay
										! End MA tunning
										!
		mov.l	dqdrv_tune, r0			! Tune DQ Driving Strength
		mov.l	phyreg3, r3
		mov.l	r0, @(0x0, r3)
		mov.l	@r1, r2					! delay
#else
#ifdef HW_SIM
			!
			! SiS DDR2 PHY initialization sequence start
			!
		mov.l   phyreg0, r3
        mov.l   iopd_setup, r0		! set CH0_STBY_IO_PD , CHO_MA_IO_PD and CH0_DQ_IO_PD to '0'
        mov.l   r0, @(0x0, r3)
			!
		mov.l   enable_ck_oe, r0	! set R_CH0_CK_OE to '1'
        mov.l   r0, @(0x0, r3)
			!
		mov.l   enable_cmd_oe, r0	! set R_CH0_CMD_OE to '1'
        mov.l   r0, @(0x0, r3)
			!
			! Enable DQS DLL initialization
        mov.l   enable_dqs_dll, r0	! set R_DYNA_DQS_INDLY to '1'
        mov.l   phyreg5, r3
        mov.l   r0, @(0x0, r3)
			!
        mov.l   setup_dqs_dll, r0	! set vaules for R_DQSDLL_TRAP_RANGE, R_DQSDLL_COUNT, R_DQSDLL_INT_RANGE and R_DQSDLL_DLY_CANCEL
        mov.l   phyreg6, r3
        mov.l   r0, @(0x0, r3)
			!
		mov.l   dqs_dll_rstn, r0	! De-assert R_DQSDLL_RSTN
        mov.l   phyreg5, r3
        mov.l   r0, @(0x0, r3)
        mov.l   @r1, r2
			! End DQS DLL sequence
!
			! Enable DQ output driving calibration
        mov.l   enable_drvcal, r0	! Set R_DRVCAL_ENA to '1' and R_DRVCAL_EXT_R to '1'
        mov.l   r0, @(0x0, r3)
        mov.l   @r1, r2
!
#endif
#endif
        mov.l   mode0, r0
        mov.l   r0, @(0x0, gbr)
        mov     #0x40, r0
        mov.b   r0, @(0x0, gbr)
        mov.l   @r1, r2                 ! delay
!
        mov.l   mode1, r0
        mov.l   r0, @(0x0, gbr)
        mov     #0x40, r0
        mov.b   r0, @(0x0, gbr)
        mov.l   @r1, r2                 ! delay
!
        mov.l   dm_setup, r0
        mov.l   r0, @(0x0, gbr)
                                        !
#ifndef __CHAOBAN_RC
#ifdef CYG_HAL_SIS910B
        mov.l   dm_setup2, r0		    ! Set cfg_rrdy to '10'
		mov.l   dc_ctl_reg0, r4
		mov.l   r0, @(0x0, r4)
									    !
		mov.l   dm_setup3, r0		    ! Set RTT/ODT length
		mov.l   dc_ctl_reg1, r4
		mov.l   r0, @(0x0, r4)
		                                !
#endif
#endif
#if defined(__CHAOBAN_RC) || defined(HW_SIM)
		mov.l   dm_setup2, r0			! Set cfg_rrdy to '10'
		mov.l   dc_ctl_reg0, r4
		mov.l   r0, @(0x0, r4)
		!
#endif
#ifdef __CHAOBAN_RC
		mov     #0x7f, r3
#else
#ifdef HW_SIM
		mov     #0x01, r3		! 0x40 => 0x01 by omega
#else
        mov     #0x40, r3
#endif
#endif
wait4ddr:
        mov.l   @r1, r2
        dt      r3
        bf      wait4ddr
         nop
#ifndef __CHAOBAN_RC
#ifdef HW_SIM
		clrmac                  ! Must clear MACH/L for proper simulation result
#endif
#endif

#else
		! FOR DDR-I
		.word   0x8226                  ! GBR=2600
		trapa	#0xfc
        mov.l   dm_setup, r0
        mov.l   rom_delay, r1
        mov.l   r0, @(0x000, gbr)       ! 0x9000_2600
        mov     #0x83, r0
        mov.b   r0, @(0x000, gbr)
        mov.l   @r1, r2                 ! delay
        mov     #0x03, r0
        mov.b   r0, @(0x000, gbr)       ! Disable DLL
        mov.l   @r1, r2                 ! delay
        mov     #0x80, r0
        mov.b   r0, @(0x000, gbr)
        mov.l   @r1, r2                 ! delay
        mov     #0x00, r0               ! DLL reset
        mov.b   r0, @(0x000, gbr)
        mov.l   @r1, r2                 ! delay
		mov		#0x80, r0
        mov.b   r0, @(0x000, gbr)
        mov.l   @r1, r2                 ! delay
        mov.w   dc_setup, r0            ! enable refresh, setup CL
        mov.w   r0, @(0x000, gbr)
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
        mov.l   @r1, r2                 ! delay
#endif //CYG_HAL_DDRII

# Memory Access Violation such as unaligned data access and invalid instuction
# will cause CPU to reboot. The difference between power on reboot and memory
# access violation reboot is that SR[31:16] is 0 for power on reboot and 4
# for memory access violation. For memory access violation reboot, the PC and
# and data memory address that causes the reboot are saved in mach and macl.
# So we always save the mach and macl in specific memory locations that are
# unlikly to be overwritten by reboot initializatin. We save the SR[31:16]
# as well. By checking the saved SR[31:16] we can know if it's a memory access
# violation.

        mov.l   L_mem_expt_pc, r0
        sts.l   mach, @-r0

        mov.l   L_mem_expt_data, r0
        sts.l   macl, @-r0

        mov.l   L_mem_expt, r0

        stc     sr, r2
        shlr16  r2
		mov.l	r2, @r0

		bra complete_ddr
		 nop

#ifdef CYG_HAL_DDRII
        .align  2
#ifdef __CHAOBAN_RC
	phyreg0:        .long   0x9000f000
	phyreg1:        .long   0x9000f004
	phyreg2:        .long   0x9000f008
	phyreg3:        .long   0x9000f00c
	phyreg4:        .long   0x9000f010
	phyreg5:        .long   0x9000f014
	phyreg6:        .long   0x9000f018
	phyreg7:		.long	0x9000f01c
	dc_ctl_reg0:	.long	0x9000f020
	iopd_setup:     .long   0x00840000
	enable_ck_oe:	.long   0x00840040
	enable_cmd_oe:	.long   0x008400c0
	enable_dqs_dll:	.long	0x01200000
	setup_dqs_dll:	.long	0x20840100
	dqs_dll_rstn:	.long	0x01300000
	edqs_tune:		.long	0x00270391
	rtt_tune:		.long	0x04fff80c
	madrv_tune:		.long	0x42004200
	dqdrv_tune:		.long	0x42108900
	mode0:          .long   0xff8f1233	!0xc00f1453
	mode1:          .long   0xff8f4016	!0xc00f4024
	dm_setup:       .long   0xc0cc5501	!0xc0cc5a23
	dm_setup2:		.long	0x64514004	!0x6c518004
	ck_phase: 		.long   0x42004202
!USB
	usb_reg:		.long   0x9000271c
	usb_host:		.long	0x00000001
	usb_host_reg:   .long	0x90002700
	osc_enres:		.long	0x00a00000
#else
#ifdef HW_SIM
	phyreg0:        .long   0x9000f000
    phyreg5:        .long   0x9000f014
    phyreg6:        .long   0x9000f018
    dc_ctl_reg0:	.long	0x9000f020
    iopd_setup:     .long   0x00840000
    enable_ck_oe:	.long   0x00840040
    enable_cmd_oe:	.long   0x008400c0
    enable_dqs_dll:	.long	0x01200000
    setup_dqs_dll:	.long	0x20840100
    dqs_dll_rstn:	.long	0x01300000
    enable_drvcal:	.long	0x013a0000
    mode0:          .long   0xff8f1233      ! Max. tRFC during mode setting
    mode1:          .long   0xff8f4016
    dm_setup:       .long   0xc0cc5501
    dm_setup2:		.long	0x04518044
#else
#ifdef CYG_HAL_SIS910B
    mode0:		    .long   0xff8f1453
    mode1:		    .long   0xff8f4427
    dm_setup:	    .long   0xc0cc5a23
    dm_setup2:	    .long	0x3a3b8804
    dm_setup3:	    .long	0x00008906
    dc_ctl_reg0:	.long	0x9000f030
    dc_ctl_reg1:	.long	0x9000f034
#else
	mode0:          .long   0xff8f0233
	mode1:          .long   0xff8f4417
	dm_setup:       .long   0xc0cc5503
#endif
#endif
#endif
    rom_delay:      .long   0xa0000000
#else
	.align 4
	cpumode_switch:	.long	0x00001000
	rom_delay:		.long	0xa0000000
	dm_setup:		.long   0x80302309
	dc_setup:   	.word   0xff30
#endif
	.align 4
	L_mem_expt:    	.long	0x40c00004
	L_mem_expt_pc: 	.long	0x40c00018
	L_mem_expt_data:.long	0x40c0001c

complete_ddr:
	.endm


#------------------------------------------------------------------------------
# Hardware initialization.
		.macro  hal_hardware_init
			hal_dram_init
			LED_INIT
			LED_2
        .endm

//        .macro  hal_hardware_init
//        // Set up the Bus State Controller
//        mova     BSC_settings_table,r0
//        mov      r0,r3
//1:      mov.w    @r3+,r0                // Address (or zero)
//        cmp/eq   #0,r0
//        bt       2f
//        mov.w    @r3+,r1                // data
//        bra      1b
//        mov.w    r1,@r0                // delay slot
//
//        .align  2
//BSC_settings_table:
//        # These are the settings set by the Hitachi ROM Monitor.
//
//	.word   (CYGARC_REG_FRQCR & 0x0000FFFF)
//	.word   CYGARC_REG_FRQCR_INIT
//
//        # BCR2: Bus size of areas 1-6 to 32 bits
//        .word   (CYGARC_REG_BCR2 & 0x0000FFFF)
//        .word   0x3ffc
//        # BCR1: Areas 2 and 3 are SDRAM
//        .word   (CYGARC_REG_BCR1 & 0x0000FFFF)
//        .word   0x080c
//        # BCR2: Bus size of areas 1-6 to 32 bits [note: second write!]
//        .word   (CYGARC_REG_BCR2 & 0x0000FFFF)
//        .word   0x3ffc
//        # WCR1: 3 wait-state cycles inserted for all areas
//        .word   (CYGARC_REG_WCR1 & 0x0000FFFF)
//        .word   0x3fff
//        # WCR2: extra wait states and full pitch for burst
//        .word   (CYGARC_REG_WCR2 & 0x0000FFFF)
//        .word   0xffd7
//        # MCR: RAS/CAS & burst timing area 2/3
//        .word   (CYGARC_REG_MCR & 0x0000FFFF)
//        .word   0x963c
//        # RTCNT: refresh counter (needs a5 in top byte to accept write)
//        .word   (CYGARC_REG_RTCNT & 0x0000FFFF)
//        .word   (0xa500 | 0x0000)
//        # RTCOR: refresh time constant (needs a5 in top byte to accept write)
//        .word   (CYGARC_REG_RTCOR & 0x0000FFFF)
//        .word   (0xa500 | 0x003b)
//        # RFCR:  refresh count register (needs a4 in top byte to accept write)
//        .word   (CYGARC_REG_RFCR & 0x0000FFFF)
//        .word   (0xa400 | 0x0000)
//        # RTCSR: refresh timer control (needs a5 in top byte to accept write)
//        .word   (CYGARC_REG_RTCSR & 0x0000FFFF)
//        .word   (0xa500 | 0x0008)
//
//        # Set SDMR to 0x220
//        .word    0xd880
//        .word    0
//
//        # Table end
//        .word   0
//
//        .align  2
//2:
//        .endm


#------------------------------------------------------------------------------
# Monitor initialization.

#ifndef CYGPKG_HAL_SH_MON_DEFINED

#if	!defined(CYG_HAL_STARTUP_RAM) ||		\
	(	defined(CYG_HAL_STARTUP_RAM) &&		\
		!defined(CYGSEM_HAL_USE_ROM_MONITOR))

	# If we are not starting up from RAM, or we are starting in
	# RAM and NOT using a ROM monitor, initialize the VSR table.

	.macro	hal_mon_init
#    mov.l   $hal_vsr_table,r3
#	# Write exception vectors
#	mov.l   $cyg_hal_default_exception_vsr,r4
#	mov     #CYGNUM_HAL_VSR_EXCEPTION_COUNT,r5
#1:	mov.l   r4,@r3
#        add     #4,r3
#	dt	r5
#	bf	1b
	# Write interrupt vector
	mov.l   $cyg_hal_default_interrupt_vsr,r4
	mov.l   $hal_vsr_table,r3
	add		#CYGNUM_HAL_VECTOR_INTERRUPT*4,r3
	mov.l   r4,@r3
	bra	2f
	 nop
        .align  2

	SYM_PTR_REF(cyg_hal_default_exception_vsr)
	SYM_PTR_REF(cyg_hal_default_interrupt_vsr)
	SYM_PTR_REF(hal_vsr_table)
2:
	.endm

#elif defined(CYG_HAL_STARTUP_RAM) && defined(CYGSEM_HAL_USE_ROM_MONITOR)

	# Initialize the VSR table entries
	# We only take control of the interrupt vector,
	# the rest are left to the ROM for now...

	.macro	hal_mon_init
	# Write interrupt vector
        mov.l   $hal_vsr_table,r3
	mov.l   $cyg_hal_default_interrupt_vsr,r4
	add	#CYGNUM_HAL_VECTOR_INTERRUPT*4,r3
	mov.l   r4,@r3
	bra	2f
	 nop
        .align  2

	SYM_PTR_REF(cyg_hal_default_interrupt_vsr)
	SYM_PTR_REF(hal_vsr_table)
2:
	.endm

#else

	.macro	hal_mon_init
	.endm

#endif


#define CYGPKG_HAL_SH_MON_DEFINED

#endif // CYGPKG_HAL_SH_MON_DEFINED

#endif // CYGONCE_HAL_PLATFORM_INC
