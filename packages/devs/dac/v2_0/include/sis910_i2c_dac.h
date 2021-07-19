#ifndef CYGONCE_SIS910_I2C_H
#define CYGONCE_SIS910_I2C_H
//-------------------------------------------------------------------------
//=========================================================================
//####### DESCRIPTIONBEGIN #######
//
// sis910_i2c_dac.h
//
// Author:       Chaoban Wang
// Date:         2008-05-05
// Description:  Defined registry of I2C Driver for DAC.
//
//=========================================================================
// SIS910 I2C HEAD FILE FOR DAC

#include <pkgconf/hal.h>
#include <pkgconf/io_i2c.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/i2c.h>


/* FOR TEST I2C SENSOR USED */
//#define TEST_I2C_SENSOR
/* FOR PRINT EVERY FUNCTION NAME */
//#define P_I2C_FUNCNAME


/* I2C General Extern functions */
void sis910_i2c_wait(int us);
void sis910_i2c_enable(void);
void sis910_i2c_disable(void);
void sis910_i2c_clear_irq(void);
void sis910_i2c_clear(void);
void sis910_i2c_disINT(void);
void sis910_i2c_enINT(void);
void sis910_i2c_init(struct cyg_i2c_bus* bus);
void sis910_i2c_start(cyg_uint32 Timming, cyg_uint32 I2C_add, cyg_uint32 RW_status);
void sis910_i2c_stop(const cyg_i2c_device* dev);
//static void sis910_i2c_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
//static cyg_uint32 sis910_i2c_isr(cyg_vector_t vector, cyg_addrword_t data);
cyg_uint32 sis910_i2c_tx(const cyg_i2c_device* dev, cyg_bool send_start, const cyg_uint8* buf, 
							 cyg_uint32 count, cyg_bool send_stop);
cyg_uint32 sis910_i2c_rx(const cyg_i2c_device* dev, cyg_bool send_start, cyg_uint8* buf, 
							 cyg_uint32 count, cyg_bool nak, cyg_bool send_stop);
unsigned char i2c_read(const cyg_i2c_device* dev, cyg_uint32 readReg, cyg_uint32 buf);
cyg_bool i2c_write(const cyg_i2c_device* dev, cyg_uint32 writeReg, cyg_uint32 data);
cyg_bool AD_PUT(const cyg_i2c_device* dev, cyg_uint8* regs);
cyg_bool AD_GET(const cyg_i2c_device* dev, cyg_uint8* regs);
cyg_bool sis910_i2c_getAck(void);


#ifdef TEST_I2C_SENSOR
/* Sensor Device */
extern cyg_i2c_device sis910_i2c_sensr;
#define SENSOR_ID				0x34		// CMP1306
#define I2C_W(_reg_, _v_)		i2c_write(&sis910_i2c_sensr, _reg_, _v_)
#define I2C_R(_reg_, _i_)		i2c_read(&sis910_i2c_sensr, _reg_, _i_)
#else
/* DAC Device */
extern cyg_i2c_device sis910_i2c_dac;
//#define DAC_ID					0x11		// CS4341A
#ifdef __CHAOBAN_RC
#define DAC_ID					0x19		// RTK_5627
#else	//FPGA
#define DAC_ID					0x1A		// RTK_5621
#endif
#define I2C_W(_reg_, _v_)		i2c_write(&sis910_i2c_dac, _reg_, _v_)
#define I2C_R(_reg_, _i_)		i2c_read(&sis910_i2c_dac, _reg_, _i_)
#endif //TEST_I2C_SENSOR


/* I2C Registry  */
#define MBDR_ID					0
#define MBDR_REG				1
#define MBDR_DAT1				2
#define MBDR_DAT2				3
#define BYNUM_1					1
#define BYNUM_2					2
#define BYNUM_3					3
#define BYNUM_4					4
#define BYNUM					BYNUM_4
#define WRITE_CMD				0x0
#define READ_CMD				0x1
#define SIS910_I2C_ID	 		0x2
#define INIT_TIM				0x001000ef
#define REG_I2C_DIVCLK			0x90004008
#define CLK_50M					0x00		// CLK/4
#define CLK_25M					0x01		// CLK/8
#define CLK_12M					0x10		// CLK/16 12.5MHZ
#define CLK_06M					0x11		// CLK/32 6.25MHZ

//end of sis910_i2c_dac.h
//-------------------------------------------------------------------------
#endif //CYGONCE_SIS910_I2C_H
