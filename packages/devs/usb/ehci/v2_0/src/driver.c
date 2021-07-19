//******************************************
// driver.c  
// The USB interface for USB IO
//******************************************
#include <cyg/io/io.h>
#include <string.h>
#include <stdlib.h>
#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/kernel/kapi.h>        //cyg_vector
#include <cyg/io/devtab.h>
#include <cyg/io/driver.h>
#include <cyg/infra/diag.h>
#include <cyg/io/usb.h>
#include <cyg/io/otg.h>
#include <pkgconf/devs_usb_host.h>
#include <cyg/io/usberr.h>			//usb err / debug

/************************************************************
* Function Prototype
*************************************************************/
cyg_uint32 usb_irq_isr(cyg_vector_t vector, cyg_addrword_t data);
void usb_irq_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);


static Cyg_ErrNo sis910_usb_init(struct cyg_devtab_entry* tab);
static Cyg_ErrNo sis910_usb_lookup(struct cyg_devtab_entry **tab,struct cyg_devtab_entry *sub_tab,const char *name);
static Cyg_ErrNo sis910_usb_write(disk_channel* chan, const void* buf, cyg_uint32 blocks, cyg_uint32 first_block);
static Cyg_ErrNo sis910_usb_read(disk_channel* chan, void* buf, cyg_uint32 blocks, cyg_uint32 first_block);
//static Cyg_ErrNo sis910_usb_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info);
static Cyg_ErrNo sis910_usb_get_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static Cyg_ErrNo sis910_usb_set_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
/************************************************************
* Local variables
*************************************************************/
#define BLOCK_SIZE 512
//#define PORT MaxDevice

static cyg_devio_table_t sis910_usb_handlers;
static cyg_interrupt sis910_usb_int1;
static cyg_handle_t sis910_usb_int1_handle;
#define USB_IRQ_NUM 7	//[7]: PCI_IRQ(USB Host)/[8]: USB_DEV_IRQ(USB DEVICE)/[9]: OTG_IRQ           			           		
        
/************************************************************
* Main functions
*************************************************************/
cyg_uint32 usb_irq_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	cyg_interrupt_mask(vector);
	cyg_interrupt_acknowledge(vector);
	                      
	return (CYG_ISR_CALL_DSR|CYG_ISR_HANDLED);
}


void usb_irq_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	cyg_uint8 result=0;
	
	result = otg_interrupt();
	
	if(!result)
	{
		if(otg_getMode() == HOST)
			usb_host_interrupt();
		//else
		//	slave_interrupt();
	}
	cyg_interrupt_unmask(vector);
}


static Cyg_ErrNo sis910_usb_init(struct cyg_devtab_entry *tab)
{
    disk_channel* chan = (disk_channel*) tab->priv;
    cyg_usb_disk_info_t* disk  = (cyg_usb_disk_info_t*) chan->dev_priv;

//	D(("driver.c: sis910_usb_init: disk->usb_connected: %d\n", disk->usb_connected));
	
	disk->usb_connected = false;
    return (*chan->callbacks->disk_init)(tab);
}

static Cyg_ErrNo sis910_usb_lookup(struct cyg_devtab_entry **tab,struct cyg_devtab_entry *sub_tab,const char *name)
{			
	//Cyg_ErrNo result = ENOERR;
    disk_channel* chan = (disk_channel*) (*tab)->priv;
    cyg_usb_disk_info_t* disk  = (cyg_usb_disk_info_t*) chan->dev_priv;
    cyg_disk_identify_t *ident = NULL;	    
    
//    D(("driver.c: sis910_usb_lookup: START!!!\n"));

	if(disk->usb_connected)
	{
    	disk->usb_connected = false;
    	(*chan->callbacks->disk_disconnected)(chan);
    	//if (0 != chan->info->mounts)
    	    //return -ENODEV;
    	    chan->info->mounts = 0;    	    
	}
	
	if (!disk->usb_connected) {			
//		D(("driver.c: sis910_usb_lookup: in connect!!!\n"));
		//host init
		otg_init();
	
		//if(otg_getMode() == HOST)
	    	usb_host_init();
		//else
		//    slave_init();	    	
		
	    disk->usb_connected = true;

        ident = (cyg_disk_identify_t *)malloc(sizeof(cyg_disk_identify_t));
        if (ident)
        {
        	memset(ident, 0, sizeof(cyg_disk_identify_t));
        	
            // We don't have no cylinders, heads, or sectors, but
            // higher-level code may interpret partition data using C/H/S
            // addressing rather than LBA. Hence values for some of these
            // settings were calculated above.
            ident->cylinders_num     = 1;
            ident->heads_num         = 255;//disk->mmc_heads_per_cylinder;
            ident->sectors_num       = 255;//disk->mmc_sectors_per_head;
            
            //Need to add in the Total LBA Request
            //ident->lba_sectors_num   = 31296;//disk->mmc_block_count; ->read total lba from device
            ident->lba_sectors_num   = 3997664;//disk->mmc_block_count; ->read total lba from device
            
            ident->phys_block_size   = 1;//disk->mmc_write_block_length/512;
            ident->max_transfer      = 512;//disk->mmc_write_block_length;        				
			
			(*chan->callbacks->disk_init)(*tab);          
            (*chan->callbacks->disk_connected)(*tab, ident);
            free(ident);
            ident = NULL;
        }
        else
        {
            return -ENOMEM;
        }        	    
	}
//	cyg_vector_t int1_vector = USB_IRQ_NUM;
//	cyg_priority_t int1_priority = 4;
//	
//	cyg_drv_interrupt_create(
//	            int1_vector,
//	            int1_priority,
//	            0,
//	            &usb_irq_isr,
//	            &usb_irq_dsr,
//	            &sis910_usb_int1_handle,
//	            &sis910_usb_int1);	                       
//	            
//	cyg_interrupt_attach(sis910_usb_int1_handle);
//	cyg_interrupt_unmask(int1_vector);

//	D(("driver.c: sis910_usb_lookup: SKIPPed ISR -- END!!\n"));	
	return (*chan->callbacks->disk_lookup)(tab, sub_tab, name);;
}


static Cyg_ErrNo sis910_usb_write(disk_channel* chan, const void* buf, cyg_uint32 blocks, cyg_uint32 first_block)
{
	Cyg_ErrNo ret=ENOERR;
//	cyg_uint8 *data = NULL;
//	cyg_uint32 *size=NULL;

//	if(otg_getMode() == HOST)	
	{	
		//Send Reset to Device
		//usb_reset(PORT);	Not necessary
		
		//Do WRITE
		ret = usb_write((cyg_uint8 *)buf, (blocks)*BLOCK_SIZE, first_block);
	}
//	else
	{//Slave
		//*size = (*block)*BLOCK_SIZE;
		//usbs_devtab_cread(handle, (void*)buf, size);
	}
	
	return ENOERR;
}


static Cyg_ErrNo sis910_usb_read(disk_channel* chan, void* buf, cyg_uint32 blocks, cyg_uint32 first_block)
{
	Cyg_ErrNo ret=ENOERR;

//	if(otg_getMode() == HOST)
	{
		//Send Reset to Device
		//usb_reset(PORT); Not necessary
	
//	diag_printf("driver.c: sis910_usb_read: buf: %d; *block: %d; pos: %d\n", buf, blocks, first_block);

		//Do READ
		ret = usb_read((cyg_uint8 *)buf, (blocks)*BLOCK_SIZE, first_block);		
		
//		diag_dump_buf_with_offset(buf, 512, buf);
	}
//	else
	{//Slave
		//*size = (*block)*BLOCK_SIZE;
		//usbs_devtab_cwrite(handle, (const void *)buf, size);
	}
	
	return ret;			
}



//static Cyg_ErrNo 
//sis910_usb_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info)
//{
//	usb_select(which);
//
//	return ENOERR;
//}



static Cyg_ErrNo sis910_usb_get_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
	return -EINVAL;
}

static Cyg_ErrNo sis910_usb_set_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
	usb_set_config(key, buf, len);
	return ENOERR;
}

#ifdef ALIGN_USE_FUNC_PTR
void* cyg_usb_buf_alloc(cyg_uint32 sizes)
{
//    FUNC_DBG_PTR;

    return malloc_align(sizes, 32); // 32 bytes alignment
}
#endif
/*************************************************************
* Local functions.
*************************************************************/



/*************************************************************/
DISK_FUNS(	cyg_usb_disk_funs,
    		sis910_usb_read,
    		sis910_usb_write,
    		sis910_usb_get_config,
    		sis910_usb_set_config );

static cyg_usb_device   cyg_usb_dev = {
//    .mmc_gateway_xfer = 1,
};

static cyg_usb_disk_info_t cyg_usb_disk_hwinfo = {
//    .mmc_dev        = &cyg_usb_dev,
};


//disk	static BLOCK_DEVIO_TABLE(sis910_usb_handlers,
//disk	            			sis910_usb_write,
//disk	            			sis910_usb_read,
//disk	            			NULL,
//disk	            			sis910_usb_get_config,
//disk	            			sis910_usb_set_config);
            			
// No h/w controller structure is needed, but the address of the
// second argument is taken anyway.
DISK_CONTROLLER(cyg_usb_disk_controller, cyg_usb_disk_hwinfo);
            			
DISK_CHANNEL(	cyg_usb_disk_channel,
             	cyg_usb_disk_funs,
             	cyg_usb_disk_hwinfo,
             	cyg_usb_disk_controller,
             	true,							/* MBR support */
             	1 );							/* Number of partitions supported */
             	
            			
                            
#ifdef ALIGN_USE_FUNC_PTR                                  
BLOCK_DEVTAB_ENTRY(sis910_usb_devices,
             			CYGDAT_DEVS_DISK_USB_DISK_NAME,
             			0,
             			&cyg_io_disk_devio,
             			&sis910_usb_init,
             			&sis910_usb_lookup,
             			&cyg_usb_disk_channel,
             			&cyg_usb_buf_alloc);
#else
BLOCK_DEVTAB_ENTRY(sis910_usb_devices,
             			CYGDAT_DEVS_DISK_USB_DISK_NAME,
             			0,
             			&cyg_io_disk_devio,
             			&sis910_usb_init,
             			&sis910_usb_lookup,
             			&cyg_usb_disk_channel);
             			
             			
#endif
