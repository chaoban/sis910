#include <cyg/io/ir.h>
#include <pkgconf/infra.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include "irkey.c"
#include <stdio.h>
#include <string.h>

/* For debug, to get IR_IRDIFF value when in a 
   diffenent system clock platform. */
//#define __DEBUG_GetIRDATA

/* For debug, to get an IR signal time when in a 
   diffenent system clock platform. */
//#define __DEBUG_GetPollCount

#define IR_INTR		CYGNUM_HAL_INTERRUPT_IR
cyg_priority_t IR_PRIOR = 20;

#define IR_timer_tbl_sz     sizeof(IR_timer_tbl)

/********************************************************
**        NEC routines                                  *
*********************************************************/
#define LEADER_MIN     ir_tbl[0]
#define LEADER_MAX     ir_tbl[1]
#define DATA_1_MIN     ir_tbl[2]
#define DATA_1_MAX     ir_tbl[3]
#define DATA_0_MIN     ir_tbl[4]
#define DATA_0_MAX     ir_tbl[5]
#define REPEAT_MIN     ir_tbl[6]
#define REPEAT_MAX     ir_tbl[7]

/************************************************************
* Local variables
*************************************************************/
unsigned short  dataIR;		   /*System/customer IR code*/
char 	        cntIRbits = 0;     /*Number of IR sys/cust.bits*/
volatile char   stateIR = IR_IDLE; /*IR state machine state*/
unsigned short  sysIRcode = 0;     /*IR's system code.	*/
unsigned char   previous_data;     /*Record the previous id code*/
int IR_table[IR_timer_tbl_sz];
unsigned short codeIR = 0;        /*Final IR code(OR 0x100)*/

/*************************************************************
* Local routines.
*************************************************************/

static cyg_devio_table_t IR_table_handler;
static cyg_interrupt sisir_inter_data;
static cyg_handle_t  sisir_inter_handle;
//chaoban test
//static cyg_sem_t data_ready;
//static cyg_drv_mutex_t  ir_lock;
//static cyg_drv_cond_t   ir_wait;

/*******************************************************
 Enable IR interrupt
 *******************************************************/
void 
initial_ir(void)
{	
     /* Enable Peripheral Interrupts in io_irq_en */
     enable_hw_ir();
     
	 /*enable adc_clk27*/
     HAL_WRITE_UINT32(IR_INTDBUS, IR_ADC_CLK27_ENABLE_VAL);//0x90003c84
     
     /*enable aux[7], in order to enable imput pad*/
     HAL_WRITE_UINT32(IR_PAD_ENREG4, IR_AUX_PAD_ENABLE_VAL);//0x90004210
     
     /* reset aux*/
     HAL_WRITE_UINT32(IR_AUX_REG3,IR_AUX_RESET_VAL);		//0x9000360c
     
     drv_delay(1);
     
     /*unreset aux*/
     HAL_WRITE_UINT32(IR_AUX_REG3,IR_AUX_UNRESET_VAL);		//0x9000360c
     
     /* enable irq interrupt in dev_per_irqen */
     IR_MMIO_OR(IO_IRQEN, IR_IRQ_ENABLE_VAL);				//0x90003610
     
     /**enable IR irq, detect falling edge IR*/	 
     HAL_WRITE_UINT32(IR_CTL, IR_IRQ_CTL_FALLING_EDGE);		//0x900034d4
     
     clear_ir_irq();										//0x900034d8
     
     /*enable aux2[7]*/
     IR_MMIO_OR(IR_AUX12_EN, 0x8000);						//0x900034a0
     
//     HAL_WRITE_UINT32(IR_FILTER_CNT, 0x80);					//chaoban test
     
     /*aux2[7] as input*/
     HAL_WRITE_UINT32(IR_AUX2_DC, 0x0);						//0x90003494
     
     /*AUX2[7] enabled as input pad*/
     IR_MMIO_AND(IR_AUXMODE, 0x0f);  						//0x90003480
     
     /*enable IR count*/
     IR_MMIO_OR(IR_COUNTER_EN, IR_COUNTER_EN_VAL);			//0x90003484
}

/******************************************************
 Disable IR interrupt
 ******************************************************/
inline void 
disable_ir_irq(void)
{
   IR_MMIO_AND(IO_IRQEN, ~IR_IRQ_ENABLE_VAL); //0x90003610
}

/******************************************************
 Clear device interrupt
 ******************************************************/
inline void 
clear_ir_irq(void)
{
     IR_MMIO_OR(IR_SYS_STATUS, 0x4);   /*clear IR interrupt */
}

inline void 
disable_hw_ir(void)
{
	cyg_uint32 _VAL;
	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
	_VAL &= ~(1 << (IR_INTR + 16));
	HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);
}

inline void 
enable_hw_ir(void)
{
	cyg_uint32 _VAL;
	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
	_VAL |= (1 << (IR_INTR + 16));
	HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);
}

#ifdef __CHAOBAN_RC
void 
FixRealChipBug(void)
{
	cyg_uint32 _VAL;

	HAL_READ_UINT32(MISC_PIN, _VAL);
    _VAL |= (SPI_TO_GPIO | IR_TO_PWM);
    HAL_WRITE_UINT32(MISC_PIN, _VAL);

    HAL_READ_UINT32(REG_GPIO_EnOut, _VAL);
    _VAL &= ~(RGP16);
    HAL_WRITE_UINT32(REG_GPIO_EnOut, _VAL);
}
#endif

void 
sisir_initfun(void)
{
    int i;
// 	diag_printf("Initialize IR system code.\n");
 	
    sysIRcode = IR_SYSCODE; /* Initialize IR system code*/
	
    /** Set up IR table*/
    for (i=0; i<IR_timer_tbl_sz; i++)
//	IR_table[i] = IR_timer_tbl[i] * cpuclk;
	{
		IR_table[i] = IR_timer_tbl[i];
    }
    
#ifdef __CHAOBAN_RC
	FixRealChipBug();
#endif
    
    initial_ir();
    
    //chaoban test
    // Initialize the semaphore used for IR interrupt.
//	cyg_semaphore_init(&data_ready, 0);
//	cyg_drv_mutex_init(&ir_lock);
//  cyg_drv_cond_init(&ir_wait, &ir_lock);

    // It is also possible and desirable to install the interrupt
    // handler here, even though there will be no interrupts for a
    // while yet.
	cyg_drv_interrupt_create(IR_INTR,
                             IR_PRIOR,        // priority
                             0,         	  // data
                             sisir_isr,                             
                             sisir_dsr,
                             &sisir_inter_handle,
                             &sisir_inter_data);
                             
    cyg_drv_interrupt_attach(sisir_inter_handle);
    cyg_drv_interrupt_mask(IR_INTR);//chaoban test
    cyg_drv_interrupt_unmask(IR_INTR);
}

/*******************************************************
*  IR INTERUPT SERVICE
********************************************************/

/* FPGA: 	  Wait for  3000 loop at IR leader signal.  */
/* Real Chip: Wait for 24000 loop at IR leader signal. */
#ifdef __CHAOBAN_RC
#define PoolLimit	24000
#else
#define PoolLimit	3000
#endif

static cyg_uint32 
sisir_isr(cyg_vector_t vect, cyg_addrword_t data)
{
//	diag_printf("Enter IR ISR.\n");
#ifdef __DEBUG_GetIRDATA
	int result[33] = {0};
	unsigned int *ir_tbl = IR_table;
#endif

#ifdef __DEBUG_GetPollCount
	int testirtime[33][1] = {{0},{0}};	// For debug
#endif

	cyg_uint32 ir_diffs = 0;
	cyg_uint32 _VAL = 0, _TMP = 0;
	cyg_uint32 i = 0;

/* Disable all interrupt */
	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
	_TMP = _VAL & 0x0000ffff;
	HAL_WRITE_UINT32(IO_IRQ_EN, _TMP);

	cyg_drv_interrupt_mask_intunsafe(vect);
	cyg_drv_interrupt_acknowledge(vect);
	
	HAL_READ_UINT32(IR_SYS_STATUS, _TMP);
	
	if (_TMP & 0x80) {    						//IR overflow
		_TMP |= 0x80;
		HAL_WRITE_UINT32(IR_SYS_STATUS, _TMP);	//clear IR overflow
	}
	
	for (i = 0; i < 33; i++)
	{
		int IRPooling = 0;
		
		do {
			HAL_READ_UINT32(IO_IRQ_WAIT, _TMP);	//0x90003614
#ifdef __DEBUG_GetPollCount
			testirtime[i][0] += 1;				// For debug
#endif
			IRPooling++;
		}
		while ((!(_TMP & IR_IRQ_ENABLE_VAL)) && (IRPooling < PoolLimit)) ;

		clear_ir_irq();

		HAL_READ_UINT32(IR_IRDIFF, ir_diffs);	//read diff

		IR_core_NEC(ir_diffs);

#ifdef __DEBUG_GetIRDATA
		result[i] = ir_diffs;
#endif
	}

	/* We have get an IR_plus_data, 
	   and now to decode what key be pressed. */
	IRKey_Decode();

#ifdef __DEBUG_GetPollCount
	diag_printf("\nIR signal polling count: ");
	for (i = 0; i < 33; i++)
	{
		diag_printf("%d, ", testirtime[i][0]);	
	}
	diag_printf("\n");
#endif

#ifdef __DEBUG_GetIRDATA
	diag_printf("\nIR diff data: ");
	for (i = 0; i < 33; i++)
	{
		diag_printf("%d, ", result[i]);
	}
	diag_printf("\n");

	cyg_uint32 irsignal = 0;
	for (i=1; i<33; i++)
	{
    	if ((result[i] <= DATA_1_MAX) && (result[i] >= DATA_1_MIN))
		{
    		irsignal |= (0x1 << (i-1));
    	}
    }
	diag_printf("IR signal =%p\n\n", irsignal);
#endif
	    
	drv_delay(60000);
	clear_ir_irq();
	
/* Restore interrupt */
	HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);


//	assert( IR_INTR == vect );

//	cyg_uint32 sys_status = 0;	
//  cyg_uint32 overflow = 0;	
//  cyg_uint32 ir_diff = 0;
//    
//	HAL_READ_UINT32(IR_SYS_STATUS, sys_status);	
//  if(sys_status & 0x80)
//  {
//   	/*IR overflow*/
//  	overflow = 1;
//      /*clear IR overflow */
//      IR_MMIO_OR(IR_SYS_STATUS, 0x80);   
//   }
//
//   HAL_READ_UINT32(IR_IRDIFF, ir_diff);
//   
//
//   clear_ir_irq();	       /*clear IR interrupt */
//   IR_core_NEC(ir_diff);
//    
//  // Block this interrupt from occurring until the DSR completes.
//	 cyg_drv_interrupt_mask_intunsafe(vect);

//	// Tell the processor that we have received the interrupt.
//	 cyg_drv_interrupt_acknowledge(vect);

//  // Tell the kernel that chained interrupt processing
//	// is done and the DSR needs to be executed next.
	//return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
	return CYG_ISR_HANDLED;	//chaoban test, No DSR
}

static void 
sisir_dsr(cyg_vector_t vect, cyg_ucount32 count, cyg_addrword_t data)
{
//chaoban test    
//    diag_printf("IR TEST INTERRUPT--DSR.\n");	
	//assert( IR_INTR == vect );
	
	// Signal the thread to run for further processing.
//	cyg_semaphore_post(&data_ready);	
	cyg_drv_interrupt_acknowledge(vect);
		
	// Allow this interrupt to occur again.
	cyg_drv_interrupt_unmask(vect);

}

static void 
IRKey_Decode(void)
{
	/* You can know what FinalIRKEY is in keydef.h */
	unsigned short FinalIRKEY = 0; 
	
	FinalIRKEY = sisdecode_irkey(get_IR_key());
	
	switch (FinalIRKEY)
    {
    	/* You can define these cases for what IR buttons you want to use.*/
    	case _NO_KEY:
    		diag_printf("\nsis910_IR : NO KEY.\n");
    		break;
    	case _KEY_0:
    		diag_printf("\nsis910_IR : KEY 0.\n");
    		break;
    	case _KEY_1:
    		diag_printf("\nsis910_IR : KEY 1.\n");
    		break;
    	case _KEY_2:
    		diag_printf("\nsis910_IR : KEY 2.\n");
    		break;
    	case _KEY_3:
    		diag_printf("\nsis910_IR : KEY 3.\n");
    		break;
    	case _KEY_4:
    		diag_printf("\nsis910_IR : KEY 4.\n");
    		break;
    	case _KEY_5:
    		diag_printf("\nsis910_IR : KEY 5.\n");
    		break;
    	case _KEY_6:
    		diag_printf("\nsis910_IR : KEY 6.\n");
    		break;
    	case _KEY_7:
    		diag_printf("\nsis910_IR : KEY 7.\n");
    		break;
    	case _KEY_8:
    		diag_printf("\nsis910_IR : KEY 8.\n");
    		break;
    	case _KEY_9:
    		diag_printf("\nsis910_IR : KEY 9.\n");
    		break;
    	case _KEY_PLUS_10:
    		diag_printf("\nsis910_IR : KEY +10.\n");
    		break;
    	case _KEY_PLUS:
    		diag_printf("\nsis910_IR : _KEY_PLUS.\n");
    		break;
    	case _KEY_MINUS:
    		diag_printf("\nsis910_IR : _KEY_MINUS.\n");
    		break;
    	case _KEY_RESUME_KEY:
    		diag_printf("\nsis910_IR : _KEY_RESUME_KEY.\n");
    		break;
    	case _ECHO_PLUS:
    		diag_printf("\nsis910_IR : _ECHO_PLUS.\n");
    		break;
    	case _ECHO_MINUS:
    		diag_printf("\nsis910_IR : _ECHO_MINUS.\n");
    		break;
    	case _SURROUND_KEY:
    		diag_printf("\nsis910_IR : _SURROUND_KEY.\n");
    		break;
    	case _VOCAL_ASSIST_KEY:
    		diag_printf("\nsis910_IR : _VOCAL_ASSIST_KEY.\n");
    		break;
    	case _REPEAT_KEY:
    		diag_printf("\nsis910_IR : _REPEAT_KEY.\n");
    		break;
    	case _PBC_KEY:
    		diag_printf("\nsis910_IR : _PBC_KEY.\n");
    		break;
    	case _RESUME_KEY:
    		diag_printf("\nsis910_IR : _RESUME_KEY.\n");
    		break;
    	case _MUTE_KEY:
    		diag_printf("\nsis910_IR : _MUTE_KEY.\n");
    		break;
    	case _VOLUME_UP_KEY:
    		diag_printf("\nsis910_IR : _VOLUME_UP_KEY.\n");
    		break;
    	case _VOLUME_DOWN_KEY:
    		diag_printf("\nsis910_IR : _VOLUME_DOWN_KEY.\n");
    		break;
    	case _CLEAR_KEY:
    		diag_printf("\nsis910_IR : _CLEAR_KEY.\n");
    		break;
    	case _PROGRAM_KEY:
    		diag_printf("\nsis910_IR : _PROGRAM_KEY.\n");
    		break;
    	case _SHUFFLE_KEY:
    		diag_printf("\nsis910_IR : _SHUFFLE_KEY.\n");
    		break;
    	case _PLAY_KEY:
    		diag_printf("\nsis910_IR : _PLAY_KEY.\n");
    		break;
    	case _STOP_KEY:
    		diag_printf("\nsis910_IR : _STOP_KEY.\n");
    		break;
    	case _GOTO_KEY:
    		diag_printf("\nsis910_IR : _GOTO_KEY.\n");
    		break;
    	case _FB_KEY:
    		diag_printf("\nsis910_IR : _FB_KEY.\n");
    		break;
    	case _FF_KEY:
    		diag_printf("\nsis910_IR : _FF_KEY.\n");
    		break;
    	case _PAUSE_KEY:
    		diag_printf("\nsis910_IR : _PAUSE_KEY.\n");
    		break;
    	case _SLOW_KEY:
    		diag_printf("\nsis910_IR : _SLOW_KEY.\n");
    		break;
    	case _STEP_KEY:
    		diag_printf("\nsis910_IR : _STEP_KEY.\n");
    		break;
    	case _ADVANCE_KEY:
    		diag_printf("\nsis910_IR : _ADVANCE_KEY.\n");
    		break;
    	case _SET_A_KEY:
    		diag_printf("\nsis910_IR : _SET_A_B.\n");
    		break;
    	case _SETUP_KEY:
    		diag_printf("\nsis910_IR : _SETUP_KEY.\n");
    		break;
    	case _TITLE_KEY:
    		diag_printf("\nsis910_IR : _TITLE_KEY.\n");
    		break;
    	case _AUDIO_KEY:
    		diag_printf("\nsis910_IR : _AUDIO_KEY.\n");
    		break;
    	case _SUB_TITLE_KEY:
    		diag_printf("\nsis910_IR : _SUB_TITLE_KEY.\n");
    		break;
    	case _ANGLE_KEY:
    		diag_printf("\nsis910_IR : _ANGLE_KEY.\n");
    		break;
    	case _ENTER_KEY:
    		diag_printf("\nsis910_IR : _ENTER_KEY.\n");
    		break;
    	case _PAL_NTSC_KEY:
    		diag_printf("\nsis910_IR : _PAL_NTSC_KEY.\n");
    		break;	
    	case _POWER_KEY:
    		diag_printf("\nsis910_IR : _POWER_KEY.\n");
    		break;
    	case _EJECT_KEY:
    		diag_printf("\nsis910_IR : _EJECT_KEY.\n");
    		break;
    	case _PREVIOUS_KEY:
    		diag_printf("\nsis910_IR : _PREVIOUS_KEY.\n");
    		break;
    	case _NEXT_KEY:
    		diag_printf("\nsis910_IR : _NEXT_KEY.\n");
    		break;
    	case _ZOOM_IN_KEY:
    		diag_printf("\nsis910_IR : _ZOOM_IN_KEY.\n");
    		break;
    	case _ZOOM_OUT_KEY:
    		diag_printf("\nsis910_IR : _ZOOM_OUT_KEY.\n");
    		break;
    	case _ZOOM_UP_KEY:
    		diag_printf("\nsis910_IR : _ZOOM_UP_KEY.\n");
    		break;
    	case _ZOOM_LEFT_KEY:
    		diag_printf("\nsis910_IR : _ZOOM_LEFT_KEY.\n");
    		break;
    	case _ZOOM_RIGHT_KEY:
    		diag_printf("\nsis910_IR : _ZOOM_RIGHT_KEY.\n");
    		break;
    	case _ZOOM_DOWN_KEY:
    		diag_printf("\nsis910_IR : _ZOOM_DOWN_KEY.\n");
    		break;
    	case _DISPLAY_KEY:
    		diag_printf("\nsis910_IR : _DISPLAY_KEY.\n");
    		break;
    		
    	/* Do not use this key, sets default. */
    	case _DVD_DIGEST_KEY:
//    		diag_printf("\nsis910_IR : DVD DIGEST KEY.\n");
//    		break;
    	default:
    		diag_printf("\nsis910_IR : Undefined Key = %p\n", FinalIRKEY);
    		break;
    }
}

static void 
IR_core_NEC(int width)
{
//diag_printf("IR_core_NEC\n");
	unsigned char data;
	char reset = 0;
	unsigned int *ir_tbl;
	int i,tmp = 0;

    ir_tbl = IR_table;

    /* First IR interrupt should be a falling edge */
    if(stateIR == IR_IDLE) 
    {
		stateIR = IR_LEADER;
    }

    if(stateIR == IR_CUSTOM) 
    {	
    	/* 
    	 * Put most common case first
	     * We are collecting system code or custom code
	     */
	    dataIR <<= 1;
	    if ((width >= DATA_1_MIN) && (width <= DATA_1_MAX)) //846~886
	    {
			dataIR |= 0x1;
	    } 
	    else if((width < DATA_0_MIN) || (width > DATA_1_MAX)) // < 413 or > 886
	    {
			reset = 1;
	    }

	    cntIRbits++;

	    /* First 16 bits are syscode */
	    if ((cntIRbits == 16) && (dataIR != sysIRcode))	//sysIRcode=0
	    {
			reset = 1;
		}
	    if (cntIRbits == 24)
	    {
        /*
         * We will not wait for data bar. Base on data, we'll
         * decide whether we want to pass the IR to CD loader.
         * If we decided that the code shouldn't be passed on,
         * we'll intentionally destroy the output IR so CD
         * loader will not get it.
         */
			data = dataIR & 0xff;

			/* reverse data bits to fit look up table */
			for (i=0; i<8; i++)
			{
				tmp <<= 1;		
            	if (data&0x1)
            	{
		   	    	tmp |= 1;
		   		}
		    	data >>= 1;
			}
			previous_data = tmp;
			codeIR = tmp | 0x100;	/* Indicate a new code */
	    } 
	    else if (cntIRbits == 32)
	    {
	    	reset = 1;
	    }
	}
    else if (stateIR == IR_REPEAT) 
    {
	    if ((width >= REPEAT_MIN) && (width <= REPEAT_MAX))
	    { 
		/* the width is 0.56 ms */
   	       codeIR = previous_data | 0x100; /* Indicate a new code */
	    }
	    reset = 1;
    } 
    else //IR_LEADER
    {
	    if ((width >= LEADER_MIN) && (width <= LEADER_MAX))//5176~5216
	    {
	        dataIR = cntIRbits = 0;
	        stateIR = IR_CUSTOM;
	    } 
        else if ((width >= REPEAT_MIN) && (width <= REPEAT_MAX))//204~245
        { 
  	    	/* if the width is 2.25 ms, it is repeat code leader */
			stateIR = IR_REPEAT;
	    } 
  	    else
  	    {
  	    	reset = 1;
  	    }
    }

    if (reset) 
    {
	    /* Reset all, start from the very beginning */
	    stateIR = IR_IDLE; 
    }
}

/**
* return IR key which is or 0x100
**/
int 
get_IR_key(void)
{
     unsigned short temp = codeIR;

     codeIR = 0;
     return (temp);
}

int 
sisdecode_irkey(unsigned short ir_key)
{
    unsigned short irKey;
    unsigned short newKey;
    char * key_tbl = (char *)IR_key_table;

    irKey = ir_key & 0xff;

    /*
     * By IR_Key_tbl convention, there is no key between 0x20 and 0x3f!
     */
    if (irKey >= 0x40) 
    	irKey -= 0x20;

    /* Only assign key if it is defined within the IR_key_table */
    newKey = _NO_KEY;
    if (irKey < sizeof(IR_key_table))
    {
    	newKey = key_tbl[irKey];
    }
    return (newKey);
}

static Cyg_ErrNo 
sisir_read(cyg_io_handle_t handle, void *buffer, cyg_uint32 *len)
{
	  unsigned short *bp = (unsigned short *)buffer;
	  unsigned short ircode = get_IR_key();
	  *bp = sisdecode_irkey(ircode);
	  return ENOERR;
}

static Cyg_ErrNo 
sisir_lookup(struct cyg_devtab_entry **tab, 
          struct cyg_devtab_entry *st, 
          const char *name)
{
    return ENOERR;
}

void 
isr_polling(void)
{	
	int result[33] = {0};
	cyg_uint32 ir_diffs = 0; 
	cyg_uint32 wait_irq = 0;
	int i = 0;
	
	clear_ir_irq();
	
	do {
		HAL_READ_UINT32(IO_IRQ_WAIT, wait_irq);
	}
	while (!(wait_irq & IR_IRQ_ENABLE_VAL)) ;
	
	HAL_WRITE_UINT32(IR_SYS_STATUS, 0x84);//clear irq
	
	for(i = 0; i < 32; i++)
	{
		do {
			HAL_READ_UINT32(IO_IRQ_WAIT, wait_irq);
		}
		while (!(wait_irq & IR_IRQ_ENABLE_VAL)) ;

		clear_ir_irq();
		HAL_READ_UINT32(IR_IRDIFF, ir_diffs);	//read diff
		result[i] = ir_diffs;
		IR_core_NEC(ir_diffs);
		wait_irq = 0;
		ir_diffs = 0;
	}

	for(i = 0; i < 32; i++)
	{
		diag_printf("%d, ", result[i]);
		result[i] = 0;
	}
	
	diag_printf("\n");
}


// eCos Device Table Entries
//todo
static BLOCK_DEVIO_TABLE(IR_table_handler,
                        NULL,
                        sisir_read,
                        NULL,
                        NULL,
                        NULL);

#ifdef ALIGN_USE_FUNC_PTR
static BLOCK_DEVTAB_ENTRY(IR_sis910_devtab_entry,
                         "/dev/SISIR",
                         0,
                         &IR_table_handler,
                         sisir_initfun,
                         sisir_lookup,
                         0,
						 NULL
                         );
#else
static BLOCK_DEVTAB_ENTRY(IR_sis910_devtab_entry,
                         "/dev/SISIR",
                         0,
                         &IR_table_handler,
                         sisir_initfun,
                         sisir_lookup,
                         0
                         );
#endif
