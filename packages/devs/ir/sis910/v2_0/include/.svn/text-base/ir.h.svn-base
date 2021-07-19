#ifndef _IR_H_
#define _IR_H_

#include <cyg/kernel/kapi.h> //cyg_vector
#include <errno.h>                 // Cyg_ErrNo
#include <cyg/io/devtab.h>
#include <cyg/io/ir/io_ir.h>

#define IR_SYSCODE              0x20df
#define ASSIGN_SYSCODE          sysIRcode = IR_SYSCODE
//#define IR_MMIOR(_register_, _value_)	 HAL_READ_UINT32(_register_, _value_)
//#define IR_MMIOW(_register_, _value_)	 HAL_WRITE_UINT32(_register_, _value_)
#define IR_MMIO_OR(_register_, _value_)\
		(*((volatile CYG_WORD32 *)(_register_)) |= (_value_))
#define IR_MMIO_AND(_register_, _value_)\
		(*((volatile CYG_WORD32 *)(_register_)) &= (_value_))

/************************************************************************
 * For IR.								*
 ************************************************************************/

#define CPUCLK                  1228
#define cpuclk                  CPUCLK
#define CLK_FACTOR              CPUCLK

#define IRERR 					20

/**************************************
**  State for receiving IR signals  ***
***************************************/
#define IR_IDLE 	      		0
#define IR_LEADER				1
#define IR_CUSTOM 	       		2
#define IR_REPEAT               3

//chaoban test
//move to hal\r16\sis910\v2_0\include\plf_io.h
/**************************************
**  Initial Register Definition  ******
***************************************/
//#define IR_INTDBUS					0x90003c84
//#define IR_PAD_ENREG4				0x90004210
//#define IR_AUX_REG3					0x9000360c
//#define IR_IRQ_EN					0x90003610
//#define IR_IRQ_WAIT					0x90003614

/**************************************
**  IR IIO Register Definition ********
***************************************/
//#define IR_AUXMODE					0x90003480
//#define IR_COUNTER_EN				0x90003484
//#define IR_AUX2_DC					0x90003494
//#define IR_AUX12_EN					0x900034a0
//#define IR_FILTER_CNT				0x900034bc
//#define IR_CTL						0x900034d4
//#define	IR_SYS_STATUS				0x900034d8
//#define	IR_IRDIFF					0x900034dc
//#define	IR_DEBUG					0x900034b4

/**************************************
**  Initial Register Value Definition***
***************************************/
#define IR_ADC_CLK27_ENABLE_VAL			2
#define IR_AUX_PAD_ENABLE_VAL			0x80
#define IR_AUX_RESET_VAL				1 << 13
#define IR_AUX_UNRESET_VAL				0 << 13
#define IR_IRQ_ENABLE_VAL				0x1 << 13

/**************************************
**  IR IIO Register Value Definition***
***************************************/
#define IR_IRQ_CTL_EN_VAL				0x1
#define IR_IRQ_CTL_RISING_EDGE			0x0 | IR_IRQ_CTL_EN_VAL
#define IR_IRQ_CTL_FALLING_EDGE			0x10 | IR_IRQ_CTL_EN_VAL
#define IR_COUNTER_EN_VAL				0x40


/************************************************************
 * global functions to be exported
 ************************************************************/
static Cyg_ErrNo sisir_lookup(struct cyg_devtab_entry **tab, struct cyg_devtab_entry *st, const char *name);
static Cyg_ErrNo sisir_read(cyg_io_handle_t handle, void *buffer, cyg_uint32 *len);
static cyg_uint32 sisir_isr(cyg_vector_t vect, cyg_addrword_t data);
static void sisir_dsr(cyg_vector_t vect, cyg_ucount32 count, cyg_addrword_t data);
static void IRKey_Decode(void);
static void IR_core_NEC(int);
inline void disable_ir_irq(void);
inline void clear_ir_irq(void);
inline void enable_hw_ir(void);
inline void disable_hw_ir(void);
void sisir_initfun(void);
void initial_ir(void);
void isr_polling(void);
int get_IR_key(void);
int sisdecode_irkey(unsigned short ir_key);
#ifdef __CHAOBAN_RC
void FixRealChipBug(void);
#endif

/*
 * When DSC_IRQ is defined, we use internal 3881 counters to keep
 * track of time.
 *
 * The counter increments at .1517ms interval (2048/13.5MHz). It only
 * triggers on one edge (default is falling) as oppose to the old
 * style where we triggers on both edge.
 */

/* Timer values when CPU is running at full speed */

/***************
** Timer values **
***************/
unsigned int IR_timer_tbl[] = {
//    (5196-IRERR),	/* Leader  minimum*/
//    (5196+IRERR),	/* Leader  maximum*/
//    (866-IRERR),	/* Data 1 minimum*/
//    (866+IRERR),	/* Data 1 maximum*/
//    (433-IRERR),	/* Data 0 minimum*/
//    (433+IRERR),	/* Data 0 maximum*/
//    (225-IRERR),	/* Repeat minimum*/
//    (225+IRERR),	/* Repeat mazimum*/
#ifdef __CHAOBAN_RC
	12000,	/* Leader  minimum*/
    30000,	/* Leader  maximum*/
    2300,	/* Data 1 minimum*/
    2400,	/* Data 1 maximum*/
    1100,	/* Data 0 minimum*/
    1200,	/* Data 0 maximum*/
#else
	2000,	/* Leader  minimum*/
    6000,	/* Leader  maximum*/
    400,	/* Data 1 minimum*/
    500,	/* Data 1 maximum*/
    200,	/* Data 0 minimum*/
    300,	/* Data 0 maximum*/
#endif
    0,		/* Repeat minimum*/
    100,	/* Repeat mazimum*/
};
#endif	/* _IR_H_ */
