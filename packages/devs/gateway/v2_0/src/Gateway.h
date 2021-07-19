/*************************************************
*      
*      File        : Gateway.h
*      Author      : Tracy Tsui .
*      tel         : #4232 .
*      Time        : 2008/05/14 .
*      Description : it is .h file for Gateway
*                    device driver on Ecos OS .
*
*************************************************/
#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <cyg/kernel/kapi.h>  //cyg_vector.
#include <errno.h>            // Cyg_ErrNo.
#include <cyg/io/devtab.h>

typedef unsigned int uint16;

#define GATEWAY_MMIOR(_register_, _value_)	 HAL_READ_UINT32(_register_, _value_)
#define	GATEWAY_MMIOW(_register_, _value_)	 HAL_WRITE_UINT32(_register_, _value_)
#define GATEWAY_MMIO_OR(_register_, _value_)\
		(*((volatile CYG_WORD32 *)(_register_)) |= (_value_))
#define GATEWAY_MMIO_AND(_register_, _value_)\
		(*((volatile CYG_WORD32 *)(_register_)) &= (_value_))
		
static Cyg_ErrNo  gateway_read(const void* buffer, cyg_uint32* len, cyg_uint32 pos);
static Cyg_ErrNo  gateway_write(const void* buffer, cyg_uint32* len, cyg_uint32 pos);

/*it include enable & disable channel .*/
/*******************************************
* For Gateway .
********************************************/
#define GATEWAY_IRQ_NUM                     12 
#define DX_MASK                             0X000001FC               
#define DY_MASK                             0X000FFE00    
#define START_ADDR_MASK                     0x00FFFFE0
#define EN_SLOT_IRQ_MASK                    0x00000004
#define COUNT_MASK                          0X02000000
#define BUSY_MASK                           0X01000000

#define REGISTER1_CHR0                      0X90001480         
#define REGISTER2_CHR0                      0X90001484
#define REGISTER1_CHR1                      0X90001490
#define REGISTER2_CHR1                      0X90001494
#define REGISTER1_CHR2                      0X900014A0
#define REGISTER2_CHR2                      0X900014A4
#define REGISTER1_CHR3                      0X900014B0
#define REGISTER2_CHR3                      0X900014B4

#define REGISTER1_CHW0                      0X90001400   
#define REGISTER2_CHW0                      0X90001404     
#define REGISTER1_CHW1                      0X90001410    
#define REGISTER2_CHW1                      0X90001414    
#define REGISTER1_CHW2                      0X90001420    
#define REGISTER2_CHW2                      0X90001424    
#define REGISTER1_CHW3                      0X90001430
#define REGISTER2_CHW3                      0X90001434

#define TYPE_DELX                           1
#define TYPE_DELY                           2
#define TYPE_START_ADDR                     3
#define TYPE_EN_SLOT_IRQ                    4
#define TYPE_COUNT                          5
#define TYPE_BUSY                           6

#define READ                                0
#define WRITE                               1

#define WRITE_REGISTER1_BASE                0X90001400 
#define WRITE_REGISTER2_BASE                0X90001404
#define WRITE_REGISTER3_BASE                0X90001408
#define WRITE_REGISTER4_BASE                0X9000140C
#define READ_REGISTER1_BASE                 0X90001480 
#define READ_REGISTER2_BASE                 0X90001484
#define READ_REGISTER3_BASE                 0X90001488
#define READ_REGISTER4_BASE                 0X9000148C

#define STATUS_FIFO_FLUSH_IRQ               0X90001700
#define STATUS_DMA_SLOT_VALID               0X90001708
#define STATUS_DMA_ACTIVE                   0X9000170C

#define OPERATION_STOP_DMA                  0X90001704
#define OPERATION_FIFO_FLUSH                0X90001714
#define WFF_CH01_BASE_SIZE                  0X90001740
#define WFF_CH23_BASE_SIZE                  0X90001744
#define RFF_CH01_BASE_SIZE                  0X90001760
#define RFF_CH23_BASE_SIZE                  0X90001764
#define OPERATION_FIFO_RESET                0X90001710

// 1704 IS OPERATION : STOP DMA .
// 1714 IS OPERATION : FIFO FLUSH .
// 1740 IS OPERATION : MAPPING BETWEEN BASE & SIZE .



#define DMA_SLOT_VALID 1 // put backend to here previously .


/*No matter it is read or write .*/
            /*EVEN*/
#define CH1_BASE_MASK                       0X0000007F
#define CH1_SIZE_MASK                       0X00001F00
            /*ODD*/
#define CH0_BASE_MASK                       0X007F0000
#define CH0_SIZE_MASK                       0X1F000000

#define ALL_FIFO_FLUSH_MASK                 0X0000000F

#define NEGATIVE_FROM_USED_TO_EMPTY_MASK    0XFFFFF0F0
//#define DMA_SLOT_VALID                    1
#define DMA_ACTIVE                          2
#define DMA_EOB                             3
#define MASK_EOB                            0X00000002

#define DMA_START_ADDR_IRQ_MASK             0X0FFFFFE0
#define NEGATIVE_EOB                        0XFFFFFFFD

#endif   /*  _GATEWAY_H */
