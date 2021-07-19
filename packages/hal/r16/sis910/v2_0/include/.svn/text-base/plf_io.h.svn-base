#ifndef CYGONCE_PLF_IO_H
#define CYGONCE_PLF_IO_H

//=============================================================================
//
//      plf_io.h
//
//      Platform specific IO support
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
// Author(s):    Chaoban Wang
// Contributors:
// Date:         2008-5-30
// Purpose:      IO support macros
// Description:
// Usage:        #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

/********************/
/* For Normal debug */
/********************/
extern char Getch(void);
extern void drv_delay(int us);
extern void cyg_serial_read(cyg_uint8* __buf, cyg_uint32 __len);

// LED
// THIS REGISTRY NOT USE FOR A LONG
#ifndef CYG_HAL_DDRII
#define REG_LED									0x9000a000
#define rLED 			*((volatile unsigned *)(0x9000a000))
#endif

// MISC
#define MISC_PIN								0x90004000
#define MISC_PLL								0x90004014

//For 0x90004000
#define BIST_RSTN								0x1
#define I2C_TO_GPIO_PWM							0x2
#define I2CSCL_TO_PWM							0x4
#define I2CSDA_TO_PWM							0x8
#define I2S_TO_GPIO								0x10
#define SPI_TO_GPIO								0x20
#define UART_TO_GPIO							0x40
#define IR_TO_GPIO								0x80
#define IR_TO_PWM								0x100
#define SD_TO_MS								0x200

// UART
#define rdURXH0    		*((volatile unsigned *)(0x90003700))
#define rUTRSTAT0   	*((volatile unsigned *)(0x90003710))		//UART 0 Tx/Rx status

#define UARTRXFIFO								0x90003700
#define UARTTXFIFO								0x90003704
#define UARTCTLREG								0x90003708
#define UARTRFSREG								0x9000370c
#define UARTSTAREG								0x90003710
#define UARTDIVISR								0x90003714
#define UARTRFSTAUS                             0x90003720
#define UARTLEGACY								0x90003728
#define UARTENREGR								0x90004208

#define UART910A                                0xd6
#define UART910B                                0xd7

#define LEGACYBDM                               0x100

#define SER_16550_RBR 							0x00   // receiver buffer register, read, dlab = 0
#define SER_16550_THR 							0x00   // transmitter holding register, write, dlab = 0
#define SER_16550_DLL 							0x00   // divisor latch (LS), read/write, dlab = 1
#define SER_16550_IER 							0x02   // interrupt enable register, read/write, dlab = 0
#define SER_16550_DLM 							0x02   // divisor latch (MS), read/write, dlab = 1
#define SER_16550_IIR 							0x04   // interrupt identification reg, read, dlab = 0
#define SER_16550_FCR 							0x04   // fifo control register, write, dlab = 0
#define SER_16550_AFR 							0x04   // alternate function reg, read/write, dlab = 1
#define SER_16550_LCR 							0x06   // line control register, read/write
#define SER_16550_MCR 							0x08   // modem control register, read/write
#define SER_16550_LSR 							0x0a   // line status register, read
#define SER_16550_MSR 							0x0c   // modem status register, read
#define SER_16550_SCR 							0x0e   // scratch pad register

// The interrupt enable register bits.
#define SIO_IER_ELSI    						0x01            // enable receiver line status irq
#define SIO_IER_ERDAI   						0x02            // enable received data available irq
#define SIO_IER_ETHREI  						0x08            // enable THR empty interrupt
#define SIO_IER_EMSI    						0x10            // enable modem status interrupt

// The interrupt identification register bits.
#define SIO_IIR_IP      						0x01            // 0 if interrupt pending
#define SIO_IIR_ID_MASK 						0x1f            // mask for interrupt ID bits

// The line status register bits.
#define SIO_LSR_ERR     						0x80            // any error condition
#define SIO_LSR_DR      						0x100           // data ready
#define SIO_LSR_OE      						0x200           // overrun error
#define SIO_LSR_PE      						0x400           // parity error
#define SIO_LSR_FE      						0x800           // framing error
#define SIO_LSR_BI      						0x1000          // break interrupt
#define SIO_LSR_THRE    						0x2000			// transmitter holding register empty
#define SIO_LSR_TEMT    						0x4000          // transmitter register empty

// The modem status register bits.
#define SIO_MSR_DCTS  							0x01              // delta clear to send
#define SIO_MSR_DDSR  							0x02              // delta data set ready
#define SIO_MSR_TERI  							0x04              // trailing edge ring indicator
#define SIO_MSR_DDCD  							0x08              // delta data carrier detect
#define SIO_MSR_CTS   							0x10              // clear to send
#define SIO_MSR_DSR   							0x20              // data set ready
#define SIO_MSR_RI    							0x40              // ring indicator
#define SIO_MSR_DCD   							0x80              // data carrier detect

// The line control register bits.
#define SIO_LCR_WLS0   							0x01             // word length select bit 0
#define SIO_LCR_WLS1   							0x02             // word length select bit 1
#define SIO_LCR_STB    							0x04             // number of stop bits
#define SIO_LCR_PEN    							0x08             // parity enable
#define SIO_LCR_EPS    							0x10             // even parity select
#define SIO_LCR_SP     							0x20             // stick parity
#define SIO_LCR_SB     							0x40             // set break
#define SIO_LCR_DLAB   							0x80             // divisor latch access bit

// The FIFO control register
#define SIO_FCR_FCR0   							0x01             // enable xmit and rcvr fifos
#define SIO_FCR_FCR1   							0x02             // clear RCVR FIFO
#define SIO_FCR_FCR2   							0x04             // clear XMIT FIFO

/////////////////////////////////////////
// Interrupt Enable Register
#define IER_RCV 								0x01
#define IER_XMT 								0x02
#define IER_LS  								0x04
#define IER_MS  								0x08

// Line Control Register
#define LCR_WL5 								0x000    // Word length
#define LCR_WL6 								0x100
#define LCR_WL7 								0x200
#define LCR_WL8 								0x300
#define LCR_SB1 								0x000    // Number of stop bits
#define LCR_SB1_5 								0x400  // 1.5 -> only valid with 5 bit words
#define LCR_SB2 								0x400
#define LCR_PN  								0x800    // Parity mode
#define LCR_PE  								0x1000    // Parity mode - even
#define LCR_PO  								0x08    // Parity mode - odd
#define LCR_DL									0x20	// Enable baud rate latch, chaoban test
#define LCR_PM  								0x28    // Forced "mark" parity
#define LCR_PS  								0x38    // Forced "space" parity

//FIFOthreshold
#define FCR_1B									0<<20	// 1Bytes
#define FCR_4B									1<<20	// 4Bytes
#define FCR_16B									2<<20	// 16Bytes
#define FCR_24B									3<<20	// 24Bytes

// Line Status Register
#define LSR_RSR 								0x01
#define LSR_THE 								0x20

// Modem Control Register
#define MCR_DTR 								0x1000000
#define MCR_RTS 								0x2000000
#define MCR_INT 								0x08   // Enable interrupts
#define MCR_AFE 								0x20

// Interrupt status register
#define ISR_None             					0x0
#define ISR_Rx_Line_Status   					0x1
#define ISR_Rx_Avail         					0x2
#define ISR_Rx_Char_Timeout  					0x4
#define ISR_Tx_Empty         					0x8
#define IRS_Modem_Status     					0x10

// FIFO control register
#define FCR_ENABLE     							0x01
#define FCR_CLEAR_RCVR 							0x10000
#define FCR_CLEAR_XMIT 							0x20000

// SPI
#define rSPICTRL		*((volatile unsigned *)(0x90002430))
#define REG_SIS910_SPI_Rx0               		0x90002420
#define REG_SIS910_SPI_Rx1               		0x90002424
#define REG_SIS910_SPI_Rx2               		0x90002428
#define REG_SIS910_SPI_Rx3               		0x9000242C
#define REG_SIS910_SPI_Tx0               		0x90002420
#define REG_SIS910_SPI_Tx1               		0x90002424
#define REG_SIS910_SPI_Tx2               		0x90002428
#define REG_SIS910_SPI_Tx3               		0x9000242C
#define REG_SIS910_SPI_CTRL              		0x90002430
#define REG_SIS910_SPI_DIVIDER           		0x90002434
#define REG_SIS910_SPI_SS                		0x90002438
#define REG_SIS910_SPI_MODE              		0x9000243C

// GPIO
#define	rRGPEF		*((volatile unsigned int *)(0x90003744))
#define rRGPVAL		*((volatile unsigned int *)(0x90003748))
#define rINTSTAT	*((volatile unsigned int *)(0x90003760))
#define	rRGPEF1		*((volatile unsigned int *)(0x90003764))

// SHIFT BITS OF EVERY GPIO
#define SGP0									0
#define SGP1									2
#define SGP2									4
#define SGP3									6
#define SGP4									8
#define SGP5									10
#define SGP6									12
#define SGP7									14
#define SGP8									16
#define SGP9									18
#define SGP10									20
#define SGP11									22
#define SGP12									24
#define SGP13									26
#define SGP14									28
#define SGP15									30
#define SGP16									0
#define SGP17									2
#define SGP18									4
#define SGP19									6
#define SGP20									8

#define RGP0							 		0x1
#define RGP1							 		0x2
#define RGP2							 		0x4
#define RGP3							 		0x8
#define RGP4							 		0x10
#define RGP5							 		0x20
#define RGP6							 		0x40
#define RGP7							 		0x80
#define RGP8							 		0x100
#define RGP9							 		0x200
#define RGP10							 		0x400
#define RGP11							 		0x800
#define RGP12							 		0x1000
#define RGP13							 		0x2000
#define RGP14							 		0x4000
#define RGP15							 		0x8000
#define RGP16							 		0x10000
#define RGP17							 		0x20000
#define RGP18							 		0x40000
#define RGP19							 		0x80000
#define RGP20							 		0x100000
#define RGPMASK							 		0x001fffff
#define REG_GPIO_Enable					 		0x90003740
#define REG_GPIO_Edge					 		0x90003744
#define REG_GPIO_Value					 		0x90003748
#define REG_GPIO_EnOut					 		0x9000374c
#define REG_GPIO_INDAT					 		0x90003750
#define REG_GPIO_INTR					 		0x90003754
#define REG_GPIO_TRIGGER				 		0x90003758
#define REG_GPIO_CLRINTR				 		0x9000375c
#define REG_GPIO_INTStat				 		0x90003760
#define REG_GPIO_Edge1					 		0x90003764
#define REG_GPIO_TRIGGER1				 		0x90003768

// FOR TRIGGER MODE
#define TRIG_DISABLE							0x0
#define TRIG_RISE								0x1
#define TRIG_FAIL								0x2
#define TRIG_BOTH								0x3

// INTERRUPT ENABLE
#define IO_IRQ_EN						 		0x90002610
#define IO_IRQ_WAIT						 		0x90003614
#define IO_IRQEN								0x90003610

// I2C REGISTRY DEFINE
#define REG_I2C_TIMING							0x90003580
#define REG_I2C_ADDR							0x90003584
#define REG_I2C_CR								0x90003588
#define REG_I2C_SR								0x9000358c
#define REG_IC2_BNUM							0x90003590
#define REG_I2C_DATA1							0x90003594	// mbdr0~mbdr3
#define REG_I2C_DATA2							0x90003598	// mbdr5~mbdr7
#define REG_I2C_DATA3							0x9000359C	// mbdr8~mbdr11
#define REG_I2C_DATA4							0x900035a0	// mbdr12~mbdr15
#define REG_I2C_BUSMINT							0x900035a4
#define REG_I2C_RESET							0x9000360c

//3610
#define EN_I2C              					0x200

//3580
#define msk_mode            					0x00200000
#define msk_pclk            					0x001e0000
#define msk_dutyratio       					0x00010000
#define msk_prescale        					0x0000ffff

//3584
#define msk_addr            					0x7f

//3588
#define CR_gcd             						0x00800000
#define CR_mben            						0x00400000
#define CR_men             						0x00200000
#define CR_mien           						0x00100000
#define CR_MASTER           					0x00080000
#define CR_WRITE           						0x00040000
#define CR_txak           						0x00020000
#define CR_rsta           						0x00010000
#define CR_INT           						0x0000ffff

//358c
#define SR_CBN             						0xff000000
#define SR_MCF             						0x00800000
#define SR_MCT             						0x00400000
#define SR_MASS            						0x00200000
#define SR_SRW             						0x00100000
#define SR_MBB             						0x00080000
#define SR_RXAK           						0x00040000
#define SR_TBUF           						0x00020000
#define SR_MIF            						0x00010000
#define MSK_INT         						0x0000ffff

#define SR_ARBLST								0x1
#define SR_SLVTX								0x2
#define SR_SLVRX								0x4
#define SR_NOACK								0x8
#define SR_MSTTX_CMP							0x10
#define SR_MSTRX_CMP							0x20
#define SR_SLVTX_CMP							0x40
#define SR_SLVRX_CMP							0x80
#define SR_SLV_RESTAR							0x100
#define SR_SLV_STOP								0x200
#define SR_GENCALL								0x400

//3590
#define BNU_NOACK	          					0x100
#define MSK_CNT             					0x0ff

//3594~35a0
#define msk_data            					0xffffffff

//35a4
#define msk_sdain          						0x2
#define msk_sclin          						0x1

/**************************************
**  Initial Register Definition  ******
***************************************/
#define IR_INTDBUS								0x90003c84
#define IR_PAD_ENREG4							0x90004210
#define IR_AUX_REG3								0x9000360c

/**************************************
**  IR IIO Register Definition ********
***************************************/
#define IR_AUXMODE								0x90003480
#define IR_COUNTER_EN							0x90003484
#define IR_AUX2_DC								0x90003494
#define IR_AUX12_EN								0x900034a0
#define IR_FILTER_CNT							0x900034bc
#define IR_CTL									0x900034d4
#define	IR_SYS_STATUS							0x900034d8
#define	IR_IRDIFF								0x900034dc
#define	IR_DEBUG								0x900034b4

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_PLF_IO_H
