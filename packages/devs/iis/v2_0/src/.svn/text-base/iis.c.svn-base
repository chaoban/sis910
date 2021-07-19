#include "iis.h"
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/io/sis910_i2c_dac.h>
#include <cyg/io/iis/io_iis.h>
/************************************************************
* Local variables
*************************************************************/
#ifdef __CHAOBAN_RC
#define IIS_SYS_CLK				100000000	//for real chip (200/2Mhz)
#else
#define IIS_SYS_CLK				 12000000	//for FPGA (24/2Mhz)
#endif

static cyg_devio_table_t IIS_table_handler;
static cyg_uint32 bclk_mode = 0;
static cyg_uint32 mclk_mode = 0;
static cyg_uint32 lr_mode = -1;
static cyg_uint32 iis_enable = 0;
static cyg_uint32 mclk_enable = 0;
static cyg_uint32 buf_addr = 0;
static cyg_uint32 buf_dx = 0;
static cyg_uint32 buf_dy = 0;
static int i2s_flage = 0;

/*************************************************************
* Local routines.
*************************************************************/
void iis_initfun()
{
#ifdef __CHAOBAN_RC
    //enable DAC and set clk
    HAL_WRITE_UINT32(IIS_INIT, 0x00040002);

     //Initial DAC I2C
     I2C_W(0x81, 0x82);
     
//    I2C_W(0x04, 0x0000);  
//    I2C_W(0x0c, 0x0808);                                          
//    I2C_W(0x1c, 0x00c3);                                          
//    I2C_W(0x34, 0x0880);    //for 24bits mode
////    I2C_W(0x34, 0x0080);    //for 16bits mode
//    I2C_W(0x38, 0x0000);                            
//    I2C_W(0x3a, 0x7080);                            
//    I2C_W(0x3c, 0xf017);                            
//    I2C_W(0x3e, 0x0086);                            
//    I2C_W(0x42, 0x0080);    
#else
    //enable DAC and set clk
    HAL_WRITE_UINT32(IIS_INIT, 0x00008002);
 
     //Initial DAC I2C
    I2C_W(0x04, 0x0000);                             
    I2C_W(0x0c, 0x0868);                             
    I2C_W(0x1c, 0x00c3);                             
//    I2C_W(0x34, 0x0880);    //bit rate = 24
    I2C_W(0x34, 0x0080);    // bit rate = 16
    I2C_W(0x36, 0x0100);                             
    I2C_W(0x3a, 0x7081);                             
    I2C_W(0x3c, 0x3017);                             
    I2C_W(0x3e, 0x0086);                             
    I2C_W(0x42, 0x0080);    
#endif
                            
    // Enable Gateway FIFO, read channel 1 is for iis
    HAL_WRITE_UINT32(gw_fifo_reset, 0xffffffff);
    HAL_WRITE_UINT32(gw_fifo_reset, 0x0);
    HAL_WRITE_UINT32(gw_rff_val0, 0x08000608);   
                            
    HAL_WRITE_UINT32(gw_lpc0, 0);
    HAL_WRITE_UINT32(gw_lpc1, 0);
    HAL_WRITE_UINT32(gw_lpc2, 0);
    HAL_WRITE_UINT32(gw_lpc3, 0);
    HAL_WRITE_UINT32(gw_lpc4, 0);
    HAL_WRITE_UINT32(gw_lpc5, 0);
    HAL_WRITE_UINT32(gw_lpc6, 0);
    HAL_WRITE_UINT32(gw_lpc7, 0);
                
	mclk_enable = 1;
	bclk_mode = 4;  //for 44.1khz}
}

void iis_gw_size(cyg_uint32 size)
{
    int d = 0;
	if(size < FIFO_SIZE)//dx*dy
	{
	    d = MAX_DX;
	    while(d > 0)
	    {
	        if(((size % d) == 0) && ((size / d) <= MAX_DY))
	        {
        		buf_dx = d;
        		buf_dy = size / d;
        		break;
	        }
	        else
	        {
	            d /= 2;
	            continue;
	        }
	    }

	    if(d == 0)
	    {
    		buf_dx = MAX_DX;
    		buf_dy = size / MAX_DX;
	    }

	}
	else
	{
		buf_dx = MAX_DX;
		buf_dy = FIFO_SIZE / MAX_DX;
	}    
}

static Cyg_ErrNo 
iis_set_config(cyg_io_handle_t handle, cyg_uint32 key,const void *buffer,cyg_uint32 *len)
{
    cyg_uint32 samplerate = 0, size = 0, t = 0;
	if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
	switch(key)
	{
		case FLAG_BITRATE:
		    i2s_flage = 1;
			lr_mode = (*(cyg_uint32*)buffer == 24)? 1 : 0;
			break;
		case FLAG_SAMPLERATE:
		    i2s_flage = 1;
    	    //mclk_mode = sys_clk / (samplerate * 64 * bclk_mode)	
			samplerate = *(cyg_uint32*)buffer;
			t = (cyg_uint32)(IIS_SYS_CLK / (samplerate * 64 * bclk_mode));
			if((IIS_SYS_CLK - (t * samplerate * 64 * bclk_mode)) > (((t+1) * samplerate * 64 * bclk_mode) - IIS_SYS_CLK) )
			    mclk_mode = t+1;
			else
			    mclk_mode = t;
			break;
		case FLAG_BUF_ADDR:
			buf_addr = *(cyg_uint32*)buffer;
			break;
		case FLAG_BUF_SIZE:
			size = *(cyg_uint32*)buffer;
			size /= 32;
			iis_gw_size(size);
			break;
		default:
			return	-EINVAL;
	}
	return ENOERR;
}

static Cyg_ErrNo 
iis_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buffer, cyg_uint32 *len)
{
	cyg_uint32 iis_mode = 0, iis_status = 0;
	if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
	switch(key)
	{
	 	case FLAG_IIS_ENABLE:
	 		HAL_READ_UINT32(IIS_INIT, iis_mode);//0x900033C0
        	*(cyg_uint32*)buffer = (iis_mode & IIS_ENABLE) >> IIS_ENABLE_SHT;
	 	break;
	 	case FLAG_IIS_BCLK_MODE:
	 		HAL_READ_UINT32(IIS_INIT, iis_mode);//0x900033C0
        	*(cyg_uint32*)buffer = (iis_mode & IIS_BCLK_MSK) >> IIS_BCLK_SHT;	 		 		
	 	break;
	 	case FLAG_IIS_LR_MODE:
	 		HAL_READ_UINT32(IIS_INIT, iis_mode);//0x900033C0
        	*(cyg_uint32*)buffer = (iis_mode & IIS_LR_MODE) >> IIS_LR_MODE_SHT;	 		
	 	break;
	 	case FLAG_IIS_MCLK_MODE:
	 		HAL_READ_UINT32(IIS_INIT, iis_mode);//0x900033C0
        	*(cyg_uint32*)buffer = (iis_mode & IIS_MCLK_MSK) >> IIS_MCLK_SHT;	 		
	 	break;
	 	case FLAG_GW_EMPTY:
	 		HAL_READ_UINT32(gw_irq, iis_status);//0x90001700
//        	*(cyg_uint32*)buffer = (iis_status & gw_rd1_empty) >> 9;	 //0x200		
        	*(cyg_uint32*)buffer = iis_status;	 //0x200		
	 	break;

	 	default:
	 		return	-EINVAL;
	 }
	 return ENOERR;
}

static Cyg_ErrNo 
iis_lookup(struct cyg_devtab_entry **tab, struct cyg_devtab_entry *st, const char *name)
{
    return ENOERR;
}

static Cyg_ErrNo 
iis_bwrite(cyg_io_handle_t handle, const void* buffer, cyg_uint32* len, cyg_uint32 pos)
{
    cyg_uint32 iis_mode = 0, value = 0;
    mclk_enable = 1;
    if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}
    	
	HAL_WRITE_UINT32(gw_irq, gw_rd1_empty); //??    
    
    gw_xfer(rd1, (cyg_uint32)buf_addr, (buf_dx&0x7f), (buf_dy&0x7ff), 0x4); //gw_xfer(ch, addr, dx, dy, ctl)
	
	if(i2s_flage == 1) //set iis
	{
	    i2s_flage = 0;
    	if(lr_mode!= -1)
    	{
    		iis_mode |= lr_mode ? IIS_LR_24_VAL : IIS_LR_16_VAL;
    	}
    	if(mclk_enable && mclk_mode)
    	{
    		iis_mode |= 0x1 << IIS_EN_MCLK_SHT;
    		iis_mode |= mclk_mode << IIS_MCLK_SHT;
    	}
    	if(bclk_mode)
    	{
    		iis_mode |= bclk_mode << IIS_BCLK_SHT;
    	}
        HAL_WRITE_UINT32(IIS_INIT, iis_mode);
        iis_mode |= 0x1 << IIS_ENABLE_SHT;
        HAL_WRITE_UINT32(IIS_INIT, iis_mode);

//        HAL_READ_UINT32(IIS_INIT, value);
//        printf("IIS_INIT = %x\n",value);        
	}
    return ENOERR;
}

static Cyg_ErrNo 
iis_bread(cyg_io_handle_t handle, void* buffer, cyg_uint32* len, cyg_uint32 pos)
{
	cyg_uint32 iis_mode = 0;
	if (*len < sizeof(cyg_uint32))
	{
		return -EINVAL;
	}

	HAL_READ_UINT32(IIS_INIT, iis_mode);//0x900033C0
	*(cyg_uint32*)buffer = iis_mode;

	return ENOERR;
}

// eCos Device Table Entries
//todo
static BLOCK_DEVIO_TABLE(IIS_table_handler,
                        iis_bwrite,
                        iis_bread,
                        NULL,
                        iis_get_config,
                        iis_set_config);

static BLOCK_DEVTAB_ENTRY(IIS_sis910_devtab_entry,
                         "/dev/SISIIS",
                         0,
                         &IIS_table_handler,
                         iis_initfun,
                         iis_lookup,
                         0
                        ,NULL
                         );