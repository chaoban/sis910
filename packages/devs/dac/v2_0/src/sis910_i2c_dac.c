//-------------------------------------------------------------------------
//=========================================================================
//####### DESCRIPTIONBEGIN #######
//
// sis910_i2c_dac.c
//
// Author:       Chaoban Wang
// Date:         2008-05-05
// Purpose:      SiS910 HAL board support.
// Description:  Implementations of I2C Driver for DAC.
//
//=========================================================================
// SIS910 I2C DRIVER FOR DAC

#include <pkgconf/infra.h>
#include <pkgconf/io_i2c.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/io/sis910_i2c_dac.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include <stdio.h>
#include <string.h>

//static cyg_interrupt intr_i2c_object;
//static cyg_handle_t intr_i2c_handle;
//static cyg_sem_t data_ready;
//static cyg_drv_mutex_t  i2c_lock;
//static cyg_drv_cond_t   i2c_wait;

#define I2C_INTR	CYGNUM_HAL_INTERRUPT_I2C
//#define I2C_INTR	15	//CHAOBAN TEST
cyg_priority_t I2C_PRIOR = 8;

/* I2C General functions */
void 
sis910_i2c_disable(void) 
{
	cyg_uint32 _VAL = 0;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_disable.\n");
#endif
// 0x90003610
	HAL_READ_UINT32(IO_IRQEN, _VAL);
	_VAL &= ~EN_I2C;
	HAL_WRITE_UINT32(IO_IRQEN, _VAL);
}


void 
sis910_i2c_enable(void) 
{
	cyg_uint32 _VAL = 0;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_enable.\n");
#endif
// 0x90003610
	HAL_READ_UINT32(IO_IRQEN, _VAL);
	_VAL |= EN_I2C;
	HAL_WRITE_UINT32(IO_IRQEN, _VAL);
}

void 
sis910_i2c_disINT(void)
{
	cyg_uint32 _VAL = 0;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_disINT.\n");
#endif
	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
	_VAL &= ~(1 << (I2C_INTR + 16));
	HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);
}


void 
sis910_i2c_enINT(void)
{
	cyg_uint32 _VAL = 0;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_enINT.\n");
#endif
	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
	_VAL |= (1 << (I2C_INTR + 16));
	HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);
}

#if 0
// Interrupt service routine for interrupt i2c.
static cyg_uint32 
sis910_i2c_isr(cyg_vector_t vector, cyg_addrword_t data)
{
#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_isr.\n");
#endif

/* ISR CODES */






/* ~ISR CODES */

// Block this interrupt from occurring until the DSR completes.
	cyg_drv_interrupt_mask_intunsafe(vector);

// Tell the processor that we have received the interrupt.
	cyg_drv_interrupt_acknowledge(vector);

// Tell the kernel that chained interrupt processing
// is done and the DSR needs to be executed next.
	return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

// Deferred service routine for interrupt i2c.
static void 
sis910_i2c_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{		
#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_dsr.\n");
#endif

// Signal the thread to run for further processing.
	cyg_semaphore_post(&data_ready);
		
	cyg_drv_interrupt_acknowledge(vector);
		
// Allow this interrupt to occur again.
	cyg_drv_interrupt_unmask(vector);
}
#endif

void 
sis910_i2c_init(struct cyg_i2c_bus* bus)
{
#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_init.\n");
#endif

	cyg_uint32 reg = 0;

	reg = (CR_mben|CR_men|CR_mien|CR_INT);

	sis910_i2c_disable();
	sis910_i2c_clear();
	sis910_i2c_enINT();
	sis910_i2c_enable();

	HAL_WRITE_UINT32(0x900033c0, 0x00008006);//for i2s

	HAL_WRITE_UINT32(REG_I2C_DIVCLK, CLK_50M);
	HAL_WRITE_UINT32(REG_I2C_TIMING, INIT_TIM);
	HAL_WRITE_UINT32(REG_I2C_ADDR, 	 SIS910_I2C_ID);
	HAL_WRITE_UINT32(REG_I2C_CR, 	 reg);

// SIS910 need not i2c interrupt.
// SIS810 need i2c interrupt.
#if 0
// Initialize the semaphore used for I2C interrupt.
	cyg_semaphore_init(&data_ready, 0);

//	cyg_drv_mutex_init(&i2c_lock);
//  cyg_drv_cond_init(&i2c_wait, &i2c_lock);

// Create i2c interrupt.
	cyg_drv_interrupt_create
	(
		I2C_INTR,
		I2C_PRIOR,
		0,	//data
		sis910_i2c_isr,
		sis910_i2c_dsr,
		&intr_i2c_handle,
		&intr_i2c_object
	);

// Attach the interrupt created to the vector.
	cyg_drv_interrupt_attach(intr_i2c_handle);

// Unmask the interrupt we just configured.
	cyg_drv_interrupt_mask(I2C_INTR);	//chaoban test added
	cyg_drv_interrupt_unmask(I2C_INTR);
#endif
}


void 
sis910_i2c_start(cyg_uint32 I2C_TIME, cyg_uint32 I2C_ADDR, cyg_uint32 I2C_CR)
{
#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_start.\n");
#endif

	sis910_i2c_disable();
	sis910_i2c_clear();
	sis910_i2c_enable();
	
	HAL_WRITE_UINT32(REG_I2C_TIMING, I2C_TIME);
	HAL_WRITE_UINT32(REG_I2C_ADDR, 	 I2C_ADDR);
	HAL_WRITE_UINT32(REG_I2C_CR, 	 I2C_CR);
}


void 
sis910_i2c_stop(const cyg_i2c_device* dev)
{
	cyg_uint32 _VAL = 0;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_stop.\n");
#endif

	HAL_READ_UINT32(REG_I2C_CR, _VAL);
	_VAL &= ~(CR_MASTER|CR_WRITE);
	HAL_WRITE_UINT32(REG_I2C_CR, _VAL);

	sis910_i2c_clear_irq();
	
	sis910_i2c_wait(300);
	
	sis910_i2c_clear();
}

/* Loop limit at IR leader signal. */
#ifdef __CHAOBAN_RC
#define PoolLimit	(3300*8)
#else
#define PoolLimit	3300
#endif

cyg_bool 
sis910_i2c_getAck(void)
{
	cyg_uint32 _VAL = 0;
	cyg_bool status = true;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_getAck.\n");
#endif
	
	cyg_uint32 iPool = 0;
	
	do {
		HAL_READ_UINT32(IO_IRQ_WAIT, _VAL);
		iPool++;
	} 
	while ((!(_VAL & EN_I2C)) && (iPool < PoolLimit)) ;	//wait irq, i2c=bit 9
	
//	diag_printf("I2C DRIVER: Get time = %d\n", iPool);
	
	HAL_READ_UINT32(REG_I2C_SR, _VAL);
	
/* no ack then clear irq and reset irq. */
	if ((_VAL & SR_NOACK) || (iPool >= PoolLimit))
	{
		diag_printf("I2C Error, can not receive device's Ack message.\n");
				
		HAL_READ_UINT32(REG_I2C_CR, _VAL);
		_VAL &= ~(CR_MASTER|CR_WRITE);
		HAL_WRITE_UINT32(REG_I2C_CR, _VAL);

		sis910_i2c_clear_irq();			

		sis910_i2c_wait(300);	
		
		sis910_i2c_clear();
		
		status = false;
	}
	
	return status;
}


void
sis910_i2c_clear(void)
{
#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_clear.\n");
#endif

	HAL_WRITE_UINT32(REG_I2C_TIMING, 0x0);
	HAL_WRITE_UINT32(REG_I2C_ADDR,   0x0);
	HAL_WRITE_UINT32(REG_I2C_CR,     0x0);	//generate stop
}


unsigned char 
i2c_read(const cyg_i2c_device* dev, cyg_uint32 readReg, cyg_uint32 buf)
{
	static unsigned char data = 0;
	cyg_uint8 regs[2];

#ifdef P_I2C_FUNCNAME
	diag_printf("i2c_read.\n");
#endif

	sis910_i2c_disINT();//for debug at temp.

	regs[MBDR_ID]  = ((dev->i2c_address)<<1) | WRITE_CMD;
	regs[MBDR_REG] = readReg & 0xff;
	
	if(false == AD_GET(dev, regs)) {
		diag_printf("I2C Read data failed.\n");
		return false;
	}
	
	sis910_i2c_enINT();//for debug at temp.
	
	HAL_READ_UINT32(buf, data);

	return data;
}


cyg_bool 
i2c_write(const cyg_i2c_device* dev, cyg_uint32 writeReg, cyg_uint32 data)
{
	cyg_uint8 regs[BYNUM];

#ifdef P_I2C_FUNCNAME
	diag_printf("i2c_write.\n");
#endif

	sis910_i2c_disINT();//for debug at temp.

	regs[MBDR_ID]  = ((dev->i2c_address)<<1) | WRITE_CMD;
	regs[MBDR_REG] = writeReg & 0xff;
	regs[MBDR_DAT1] = data & 0xff;
	regs[MBDR_DAT2] = (data >> 0x8) & 0xff;
	
	if(false == AD_PUT(dev, regs)) {
		diag_printf("I2C Write data failed.\n");
		return false;
	}
	
	sis910_i2c_enINT();//for debug at temp.
	
	return true;
}


cyg_bool 
AD_PUT(const cyg_i2c_device* dev, cyg_uint8* regs)
{
	cyg_bool status = true;
    cyg_uint8 tx_data[BYNUM];

#ifdef P_I2C_FUNCNAME
	diag_printf("I2C AD_PUT.\n");
#endif
    
    memcpy(&(tx_data[0]), regs, BYNUM);   
    
    status = cyg_i2c_tx(dev, tx_data, BYNUM);
    
    return status;
}


cyg_bool 
AD_GET(const cyg_i2c_device* dev, cyg_uint8* regs)
{	
    cyg_bool    status = true;
    cyg_uint8 	tx_data[2];

#ifdef P_I2C_FUNCNAME
	diag_printf("I2C AD_GET.\n");
#endif
	
    memcpy(&(tx_data[0]), regs, 2);

    cyg_i2c_transaction_begin(dev);

/* THIS IS FOR SENDOR 			*/
/* When change to Read mode, 	*/
/* We must send 0xff68 first. 	*/
#ifdef TEST_I2C_SENSOR    
    if (false == cyg_i2c_transaction_tx(dev, true, tx_data, 2, false)) {
        /* The device has not responded to the address byte. */
//      diag_printf("I2C TX responded fail.\n");
        status = false;
    } 
    else
#endif
    {
        /* Now fetch the data */
		tx_data[0] = ((dev->i2c_address)<<1) | READ_CMD;
        if(false == cyg_i2c_transaction_rx(dev, true, tx_data, 1, true, true)) {
//        	diag_printf("I2C RX responded fail.\n");
        	status = false;
        }
    }
    
    cyg_i2c_transaction_end(dev);
    
    return status;
}


cyg_uint32 
sis910_i2c_tx(const cyg_i2c_device* dev, cyg_bool send_start, const cyg_uint8* buf, 
					cyg_uint32 count, cyg_bool send_stop)
{	
	cyg_bool status = true;
	cyg_uint32 _data32 = 0x00000000;
	cyg_uint32 iT = 0, iSft = 0;
	cyg_uint32 _VAL = 0, CREG = 0;

#ifdef P_I2C_FUNCNAME
	diag_printf("sis910_i2c_tx.\n");
#endif

	CREG = (CR_mben|CR_men|CR_mien|CR_INT);

	for (iT = 0; iT < count; ) {
		_data32 = (_data32 | ((buf[iT]) << iSft));
		iSft = iSft + 8;
		iT++;
	}

#ifdef P_I2C_FUNCNAME
	diag_printf("I2C Put datas = %p\n", _data32);
//	diag_printf("Data count = %p\n", count);
#endif
	
	if(send_start) {
		sis910_i2c_start(INIT_TIM, SIS910_I2C_ID, CREG);
	}
	
	HAL_WRITE_UINT32(REG_I2C_DATA1, _data32);
	HAL_WRITE_UINT32(REG_IC2_BNUM,  count);

	HAL_READ_UINT32(REG_I2C_CR, _VAL);
	_VAL |= (CR_MASTER|CR_WRITE);
	HAL_WRITE_UINT32(REG_I2C_CR, _VAL);
	
	if(true != sis910_i2c_getAck()) {
//		diag_printf("I2C Error, can not receive device's Ack message.\n");
		return false;
	}
	
	cyg_uint32 iPool = 0;
	
	/* Make sure trans completed */
	HAL_READ_UINT32(REG_I2C_CR, _VAL);
	
	if (_VAL & (CR_MASTER | CR_WRITE)) {					//MASTER, WRITE	
		do {
			HAL_READ_UINT32(REG_I2C_SR, _VAL);
			iPool++;
		} while ((!(_VAL & SR_MSTTX_CMP)) && (iPool < PoolLimit)) ;
	} 
	else {													//SLAVE, WRITE
		do {
			HAL_READ_UINT32(REG_I2C_SR, _VAL);
			iPool++;
		} while ((!(_VAL & SR_SLVTX_CMP)) && (iPool < PoolLimit)) ;
	}
	
//	diag_printf("I2C DRIVER: iPool time = %d\n", iPool);
		
	if(send_stop) {
		cyg_i2c_transaction_stop(dev);
	}

	return status;
}


cyg_uint32 
sis910_i2c_rx(const cyg_i2c_device* dev, cyg_bool send_start, cyg_uint8* buf, 
					cyg_uint32 count, cyg_bool nak, cyg_bool send_stop) 
{
	cyg_bool status = true;
	cyg_uint32 _data32 = 0x00000000;
	cyg_uint32 iT = 0, iSft = 0;
	cyg_uint32 _VAL = 0, CREG = 0;

#ifdef P_I2C_FUNCNAME	
	diag_printf("sis910_i2c_rx.\n");
#endif

	CREG = (CR_mben|CR_men|CR_mien|CR_INT);

	for (iT = 0; iT < count; ) {
		_data32 = (_data32 | ((buf[iT]) << iSft));
		iSft = iSft + 8;
		iT++;
	}

	if(send_start) {
		sis910_i2c_start(INIT_TIM, SIS910_I2C_ID, CREG);	//clear i2c becouse will change to master read mode and stop bit i2c
	}
	
	HAL_WRITE_UINT32(REG_I2C_DATA1, _data32);				//Write slave address to register device read mode
	HAL_WRITE_UINT32(REG_IC2_BNUM, count);

	HAL_READ_UINT32(REG_I2C_CR, _VAL);
	_VAL |= CR_MASTER;
	_VAL &= ~CR_WRITE;
	HAL_WRITE_UINT32(REG_I2C_CR, _VAL);
	
	sis910_i2c_clear_irq();
	
	if(true != sis910_i2c_getAck())	{
//		diag_printf("I2C Error, can not receive device's Ack message.\n");
		return false;
	}
	
	HAL_WRITE_UINT32(REG_IC2_BNUM, (count | BNU_NOACK));	//test
		
	sis910_i2c_clear_irq();

	if(true != sis910_i2c_getAck()) {
//		diag_printf("I2C Error, can not receive device's Ack message.\n");
		return false;
	}
	
	cyg_uint32 iPool = 0;
	
	/* Make sure trans completed */
	HAL_READ_UINT32(REG_I2C_CR, _VAL);
	
	if (_VAL & CR_MASTER) {						//MASTER, READ	
		do {
			HAL_READ_UINT32(REG_I2C_SR, _VAL);
			iPool++;
		} while ((!(_VAL & SR_MSTRX_CMP)) && (iPool < PoolLimit)) ;
	} 
	else {										//SLAVE, READ
		do {
			HAL_READ_UINT32(REG_I2C_SR, _VAL);
			iPool++;
		} while ((!(_VAL & SR_SLVRX_CMP)) && (iPool < PoolLimit)) ;
	}
	
//	diag_printf("I2C DRIVER: iPool time = %d\n", iPool);
	
//	HAL_WRITE_UINT32(REG_IC2_BNUM, (count | BNU_NOACK));//test
	
//	sis910_i2c_clear_irq(); 							//test
	
//	if(true != sis910_i2c_getAck()) {					//test
//		diag_printf("I2C Error, can not receive device's Ack message.\n");
//		status = false;
//	}

	if(send_stop) {
		cyg_i2c_transaction_stop(dev);
	}
	
	return status;
}


void
sis910_i2c_clear_irq(void) 
{
	cyg_uint32 _VAL = 0;

#ifdef P_I2C_FUNCNAME	
	diag_printf("sis910_i2c_clear_irq.\n");
#endif

	HAL_READ_UINT32(REG_I2C_SR, _VAL);
	_VAL |= MSK_INT;
	HAL_WRITE_UINT32(REG_I2C_SR, _VAL);
}


void 
sis910_i2c_wait(int us) 
{
	int counter;
	
	for ( counter = 0; counter < (4*us); counter++ ) {
		asm volatile ("nop");
	}
}

/* Device Bus */
CYG_I2C_BUS
(
	sis_i2c_bus,							// bus name
	&sis910_i2c_init,						// init func,
	&sis910_i2c_tx,							// tx func.
	&sis910_i2c_rx,							// rx func,
	&sis910_i2c_stop,						// stop func,
	(void *) 0
);

#ifdef TEST_I2C_SENSOR
/* SENSOR Device, CMP1306 */
CYG_I2C_DEVICE
(
	sis910_i2c_sensr,						// device name
	&sis_i2c_bus,							// device bus
	SENSOR_ID,								// device id
	0x00,
	CYG_I2C_DEFAULT_DELAY
);
#else
/* DAC Device */
CYG_I2C_DEVICE
(
	sis910_i2c_dac,						// device name
	&sis_i2c_bus,							// device bus
	DAC_ID,								// device id
	0x00,
	CYG_I2C_DEFAULT_DELAY
);
#endif

//end of sis910_i2c_dac.c
