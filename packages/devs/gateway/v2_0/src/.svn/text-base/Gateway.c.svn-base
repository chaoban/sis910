/*
   File        : Gateway.c   
   Description : There are only 3 functions : read , write , FIFO_flush need "mutex,conf" strategy .
                 As only the three function need HW interrupt support .
                 several channels can't use GATEWAY at the same moment .  
   The source code is for Gateway device driver .
   And , SW AP polling global variable temp to know whether the request is finished by Gateway HW .
   Author      : Tracy Tsui .
   Tel         : # 4232 .
   Time        : 2008/05/14 .
*/

#include <stdio.h>
#include <string.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include "Gateway.h"
#include <cyg/io/gateway/io_gateway.h>    

static cyg_interrupt     GATEWAY_inter_data;
static cyg_handle_t      GATEWAY_inter_handle;
static cyg_devio_table_t GATEWAY_devio;  

inline void enable_gateway_irq();
inline void clear_gateway_irq(int bit_index);
void gateway_dsr(cyg_vector_t vect);
void gateway_isr(cyg_vector_t vect);

static void gateway_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo gateway_lookup(struct cyg_devtab_entry **tab, 
                               struct cyg_devtab_entry *sub_tab,
                               const char *name);

extern cyg_mutex_t ch_mutex_lock[MAX_CH_NUM];

inline void gateway_fifo_set()
{
    cyg_uint32 value = 0;
    value = (sizeW[0]<<24)|(baseW[0]<<16)|(sizeW[1]<<8)|(baseW[1]) ;
    GATEWAY_MMIOW(WFF_CH01_BASE_SIZE ,value);             
    value = (sizeW[2]<<24)|(baseW[2]<<16)|(sizeW[3]<<8)|(baseW[3]) ;
    GATEWAY_MMIOW(WFF_CH23_BASE_SIZE ,value);            
    value = (sizeR[0]<<24)|(baseR[0]<<16)|(sizeR[1]<<8)|(baseR[1]) ;
    GATEWAY_MMIOW(RFF_CH01_BASE_SIZE ,value);             
    value = (sizeR[2]<<24)|(baseR[2]<<16)|(sizeR[3]<<8)|(baseR[3]) ;
    GATEWAY_MMIOW(RFF_CH23_BASE_SIZE ,value);             
}

static void gateway_fifo_allocate()
{
    /*
         FIFO 的大小是32 .
         這個函數的目的是決定出　sizeR[j], baseR[j].
         而且　channel 0 所能使用的FIFO 的位置是特定的 : 0~5   .
               channel 1 所能使用的FIFO 的位置是特定的 : 6~11  .
               channel 2 所能使用的FIFO 的位置是特定的 : 12~17 .
               channel 3 所能使用的FIFO 的位置是特定的 : 18~23 .
               channel 4 所能使用的FIFO 的位置是特定的 : 24~31 .
               
        因為雖然FIFO有空 ,但是如果 channel 被佔據,則,也是沒辦法. 
        所以　決定以channel 作為這個 function能不能被放行的 標準.
    */
    int channel = 0;
    int j = 0;
    for(j=0;j<5;j++)
    {
        baseR[j]= j*6; 
        if(j=!4)
            sizeR[j]= 6; 
        else
            sizeR[j]= 8;
    } 
    for(j=1;j<=3;j++)
    {
        baseW[j]= j*10;
        if(j!=3)
           sizeW[j] = 10;
        else 
           sizeW[j]= 12;
    }         
}

static bool gateway_dma_stop(int devid,int RW)
{
    //OPERATION_STOP_DMA
    int ch= 0;
    cyg_uint32 value;
    ch = getchannel(devid,RW);
    cyg_uint32 mask;
      
    if(RW == READ)     
    { 
        mask = 0X00000100<<ch;
    }
    else  // WRITE .
    {      
        mask = 0X00000001<<ch;
    }
    GATEWAY_MMIOR(OPERATION_STOP_DMA ,value );
    value |= mask;   
    GATEWAY_MMIOW(OPERATION_STOP_DMA ,value );      
    return true;
}

static bool gateway_fifo_reset(int devid,int RW)  // enable .
{
    int ch;
    cyg_uint32 mask;      
    cyg_uint32 value; 
    ch = getchannel(devid,RW);

    if(RW == READ)     
    { 
        mask = 0X00000100<<ch;
    }
    else  // WRITE .
    {      
        mask = 0X00000001<<ch;
    }
    GATEWAY_MMIOR(OPERATION_FIFO_RESET ,value );
    value |= mask;   
    GATEWAY_MMIOW(OPERATION_FIFO_RESET ,value );      
    return true; 
}

void gateway_set_sram_width(cyg_uint32 value )
{      
     value << 16;
     //GATEWAY_MMIOW(0x90002682,value );
}
inline void  setupdxdy(int ch, int size, cyg_uint32* dx, cyg_uint32* dy)
{
    if(ch == CH_READ_VDEC)
    {
        //special case for jpeg
        *dx = size>>5;
        *dy = 0;
        return;
    }
    else if(size & 0xFFFFF000)
	{
		//more than 4k
		*dx = 0x7F;
		*dy = size >>12;
	}
	else
	{
		//set less than 4k
		*dx = size>>5;
		*dy = 1;
	}
}

void set_start_delx_dely(int ch ,int RW, char* start, int size)
{
     cyg_uint32 value_delx = 0;
     cyg_uint32 value_dely = 0;
     cyg_uint32 value = 0;
     
     setupdxdy(ch, size, &value_delx, &value_dely);
     value = value_delx<<2 |value_dely<<9;
     if(RW == READ)
     {
        GATEWAY_MMIOW(READ_REGISTER1_BASE+ch*0x10 ,value );
        GATEWAY_MMIOW(READ_REGISTER2_BASE+ch*0X10 ,(cyg_uint32)start<<5 | 0x5);             
     }
     else
     {
        GATEWAY_MMIOW(WRITE_REGISTER1_BASE+ch*0x10 ,value );
        GATEWAY_MMIOW(WRITE_REGISTER2_BASE+ch*0X10 , (cyg_uint32)start<<5 | 0x5);    
     } 
}

inline void clear_gateway_irq(int bit_index)
{				
    /*
         It only clear SLOT_TO_VALID flag .
    */
	 cyg_uint32 value_irq;
     cyg_uint32 mask;
                
     GATEWAY_MMIOR(STATUS_FIFO_FLUSH_IRQ,value_irq);
     mask = 0x1<<bit_index;                    
     value_irq &= ~mask;
     GATEWAY_MMIOW(STATUS_FIFO_FLUSH_IRQ,value_irq);        
                                     
}
void gateway_init(struct cyg_devtab_entry *tab)
{   
    int i;       
    int j = 0;
    for(j = 0;j < MAX_CH_NUM;j++)
    {
        cyg_mutex_init(&ch_mutex_lock[j]);        
    }
    gateway_set_sram_width(64);
    gateway_fifo_set();
    cyg_drv_interrupt_mask(GATEWAY_IRQ_NUM);
    cyg_drv_interrupt_create(GATEWAY_IRQ_NUM,99,0,&gateway_isr,&gateway_dsr,
                     &GATEWAY_inter_handle ,  &GATEWAY_inter_data );
                     /* GATEWAY device driver priority is 99 ?*/   
    cyg_drv_interrupt_attach(GATEWAY_inter_handle);
    cyg_drv_interrupt_unmask(GATEWAY_IRQ_NUM);
}

static Cyg_ErrNo gateway_lookup(struct cyg_devtab_entry **tab, 
                               struct cyg_devtab_entry *sub_tab,
                               const char *name)
{
	return ENOERR;
}
 
static Cyg_ErrNo gateway_write(const void* buffer, cyg_uint32* len, cyg_uint32 pos)
{
    int ch = 0;
    cyg_gw_info_ptr pgwinfo; 
    if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
	pgwinfo = (cyg_gw_info_ptr)buffer;
    ch = getchannel(pgwinfo->devid,WRITE);
    set_start_delx_dely( ch ,WRITE, (char *)pgwinfo->base, pgwinfo->size);
    GATEWAY_MMIOW(OPERATION_FIFO_FLUSH,0x00000080|ch);// flush cmd                                                 
    return ENOERR;                               
}
 
static Cyg_ErrNo gateway_read(const void* buffer, cyg_uint32* len, cyg_uint32 pos)
{
    int ch = 0;
    cyg_gw_info_ptr pgwinfo; 
    if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
	pgwinfo = (cyg_gw_info_ptr)buffer;
    ch = getchannel(pgwinfo->devid,WRITE);
    set_start_delx_dely( ch ,READ, (char *)pgwinfo->base, pgwinfo->size);                                                
    return ENOERR;             
}

void gateway_isr(cyg_vector_t vect)
{   
     int sys_status;        
     int j = 0;
     int BITS;
     int RW;
     int devid;
     //assert(GATEWAY_IRQ_NUM == vect);
                
     cyg_uint32 value_irq;
     cyg_uint32 value_from_slot_to_valid;           
     GATEWAY_MMIOR(STATUS_FIFO_FLUSH_IRQ,value_irq);           
     GATEWAY_MMIOR(STATUS_DMA_SLOT_VALID,value_from_slot_to_valid);           
     // check write operation ok .
     for(j=0;j<=3;j++)           
     {           
        if((value_irq&(1<<j) != 0)&&(value_from_slot_to_valid&(1<<j) != 0))
        {
            clear_gateway_irq(j);
            cyg_mutex_unlock(&ch_mutex_lock[j + MUTEX_CH_SHT]);
            return;
        } 
     }
     // check read operation ok.
     for(j=8;j<=11;j++)           
     {  
        if((value_irq&(1<<j) != 0)&&(value_from_slot_to_valid&(1<<j) != 0))
        {
            clear_gateway_irq(j);
            cyg_mutex_unlock(&ch_mutex_lock[j-8]);
            return;
        }         
     }                                                                
}

void gateway_dsr(cyg_vector_t vect)
{
	
}

static Cyg_ErrNo gateway_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{	
   
    cyg_gw_info_ptr pgwinfo;
    if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
	pgwinfo = (cyg_gw_info_ptr)buf;
    switch(key)
    {
       case SET_FIFOs_START_SIZE :
            gateway_fifo_set();                               
        break;
      case GATEWAY_RESET_CHANNEL_READ: 
            gateway_fifo_reset(pgwinfo->devid,READ) ;
        break;    
      case GATEWAY_RESET_CHANNEL_WRITE:
            gateway_fifo_reset(pgwinfo->devid,WRITE) ;
        break;
      case GATEWAY_STOP_CHANNEL_READ:
            gateway_dma_stop( pgwinfo->devid, READ);
        break;
      case GATEWAY_STOP_CHANNEL_WRITE:
            gateway_dma_stop( pgwinfo->devid, WRITE);
        break;
    }
}


static Cyg_ErrNo gateway_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buffer, cyg_uint32 *len)
{	
    cyg_uint32 value = 0;
    cyg_gw_info_ptr pgwinfo; 
    int ch = 0;
    int shift = 0;
    cyg_uint32 mask = 0;
	
	if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
	pgwinfo = (cyg_gw_info_ptr)buffer;
	switch(key)
	{
	    case QUERY_CHANNEL_STATUS_WRITE:
	         ch = getchannel(pgwinfo->devid,WRITE);
	         GATEWAY_MMIOR(STATUS_DMA_SLOT_VALID,value);	         
	         *(cyg_uint32*)(pgwinfo->base) = (value & 0x1<<ch);
	    break;
	    case QUERY_CHANNEL_STATUS_READ:
	        ch = getchannel(pgwinfo->devid,READ); 	                   	            	          
	        GATEWAY_MMIOR(STATUS_DMA_SLOT_VALID,value);
	        *(cyg_uint32*)(pgwinfo->base) = (value & 0x1<<(ch+8));
	    break;
	    default:
	}
	
}

static
     DEVIO_TABLE(GATEWAY_devio,
                 gateway_write,
                 gateway_read,
                 NULL,
                 gateway_get_config,                                 
                 gateway_set_config
      );
static DEVTAB_ENTRY(GW_devtab_entry,
                 "/dev/GATEWAY",
                 0,
                 &GATEWAY_devio,
                 gateway_init,
                 gateway_lookup,
                 0
      );
 
