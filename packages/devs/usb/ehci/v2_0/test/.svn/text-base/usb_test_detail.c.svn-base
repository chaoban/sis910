//******************************************
// mass.c  
// Fill the scsi commands for "Mass Storage"
// in usb
//******************************************
#include <stdio.h>                      /* printf */
#include <stdlib.h>                      /* printf */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <errno.h>	// Cyg_ErrNo
#include <cyg/infra/cyg_type.h> // base types
#include <cyg/io/usb/host/io_usb_host.h>
//=========================================
//Locals
//=========================================
extern char Getch(void);

#define USB_CTRL_DEVICE_DESCRIPTOR	1
#define USB_CTRL_CONFIG_DESCRIPTOR	2
#define	USB_CTRL_SET_ADDRESS		3
#define	USB_CTRL_SET_CONFIG			4
#define	USB_BULK_READ10_HC			5
#define	USB_BULK_WRITE10_HC			6
#define USB_BULK_READ10				7
//=========================================
//Function Prototype
//=========================================
// --------- READ/WRITE from IO -----------
//Block READ
void BlockRead_SingleBlock_repeat(cyg_io_handle_t handle_usb, cyg_uint8 blocknum);
void BlockRead_MultiBlock(cyg_io_handle_t handle_usb, cyg_uint8 blocknum);
//Block WRITE
void BlockWrite_SingleBlock_repeate(cyg_io_handle_t handle_usb, cyg_uint8 blocknum);
void BlockWrite_MultiBlock(cyg_io_handle_t handle_usb, cyg_uint8 blocknum);

//=========================================
//Main Functions
//=========================================
void cyg_user_start(void)
{
	cyg_io_handle_t handle_usb;
	Cyg_ErrNo err = ENOERR;
cyg_uint8 *buf = NULL;
cyg_uint32 len = 1;

buf = malloc(sizeof(cyg_uint8)*512*2);
	diag_printf("\n!!!Prepare to test USB OHCI.!!!\n");

	//USB init in lookup
	err = cyg_io_lookup("/dev/usb/ehci", &handle_usb);
	if (err == ENOERR)
 	{	
// 		diag_printf("======= TEST IO FUNCTIONs ======\n");	
// 		USB_setup();
		cyg_io_set_config(handle_usb, USB_CTRL_DEVICE_DESCRIPTOR, buf, &len);
		cyg_io_set_config(handle_usb, USB_CTRL_CONFIG_DESCRIPTOR, buf, &len);
		cyg_io_set_config(handle_usb, USB_CTRL_SET_ADDRESS, buf, &len);
		cyg_io_set_config(handle_usb, USB_CTRL_SET_CONFIG, buf, &len);
		
		cyg_io_set_config(handle_usb, USB_BULK_READ10_HC, buf, &len);
//		cyg_io_set_config(handle_usb, USB_BULK_WRITE10_HC, buf, &len);
		cyg_io_set_config(handle_usb, USB_BULK_READ10, buf, &len);


 		
// 		diag_printf("======= READ READ READ !!! ======\n");
//		BlockRead_SingleBlock_repeat(handle_usb, 1);
//		BlockRead_SingleBlock_repeat(handle_usb, 3);
//		BlockRead_MultiBlock(handle_usb, 5);
//		diag_printf("======= WRITE WRITE WRITE !!! ======\n");
//		BlockWrite_SingleBlock_repeate(handle_usb, 1);
//		BlockWrite_SingleBlock_repeate(handle_usb, 10);
//		BlockWrite_MultiBlock(handle_usb, 5);
	}
	else
	{
		diag_printf("USB init ERROR: cyg_io_lookup: %x !!!\n", err);
	}
	diag_printf("USB TEST PROGRAM END!!!.\n");	
}

//Read in a serious of blocks at a time
void BlockRead_MultiBlock(cyg_io_handle_t handle_usb, cyg_uint8 blocknum)
{
	Cyg_ErrNo err = ENOERR;
	cyg_uint8 *buf = NULL;
	cyg_uint32 blen=0, bpos=0;
	int i=0;	
	
	diag_printf("USB Single Block Read (from host) TEST.\n");	
	
	buf = malloc(sizeof(cyg_uint32)*512*4);
	diag_printf("\ntest: buf: %x\n", buf);		
	
    if (buf)
    {							
		//READ IN a block thro IO
		blen = blocknum;
		bpos = 0;
		
		err = cyg_io_bread(handle_usb, buf, &blen, bpos);
    	if (err == ENOERR)
    	{
    	    diag_printf("cyg_io_bread() success.\n");
    	    diag_printf("=======================================\n");
    	    diag_printf("          DATA READ BACK (M-Bs)	    \n");
    	    diag_printf("=======================================\n");
    	    diag_dump_buf_with_offset(buf, 512, buf);
    	    diag_printf("=======================================\n");
    	}
    	else
    	{
    	    diag_printf("cyg_io_bread() err = %d\n", err);
    	}
    	free(buf); 
    }			
	diag_printf("BlockRead_MultiBlock Finished!!!\n");							   	
}

//Read in single block continously one by one
void BlockRead_SingleBlock_repeat(cyg_io_handle_t handle_usb, cyg_uint8 blocknum)
{
	Cyg_ErrNo err = ENOERR;
	cyg_uint8 *buf = NULL;
	cyg_uint32 blen=0, bpos=0;
	int i=0;
					
	diag_printf("BlockRead_SingleBlock_repeat (from host) TEST.\n");		
	
	buf = malloc(sizeof(cyg_uint32)*512*blocknum);
	diag_printf("\ntest: buf: %x\n", buf);		
	
    if (buf)
    {			
		//memset(buf, 0xFF, 512);
		for(i=0; i<blocknum; i++)
		{
			//READ IN blocks thro IO
			blen = 1;
			bpos = i;
			
			err = cyg_io_bread(handle_usb, buf, &blen, bpos);
        	if (err == ENOERR)
        	{
        	    diag_printf("cyg_io_bread() success.\n");
        	    diag_printf("=======================================\n");
        	    diag_printf("        DATA READ BACK (1B)            \n");
        	    diag_printf("=======================================\n");
        	    diag_dump_buf_with_offset(buf, 512, buf);
        	    diag_printf("=======================================\n");
        	}
        	else
        	{
        	    diag_printf("cyg_io_bread() err = %d\n", err);
        	}
        }
        free(buf);
    }        	
    diag_printf("BlockRead_SingleBlock_repeat Finished.\n");		
									
}

//Write out a serious of blocks at a time
void BlockWrite_MultiBlock(cyg_io_handle_t handle_usb, cyg_uint8 blocknum)
{
	Cyg_ErrNo err = ENOERR;
	cyg_uint8 *buf = NULL;
	cyg_uint32 blen=0, bpos=0;
	int i=0;
				
	diag_printf("BlockWrite_MultiBlock (from host) TEST.\n");			
	
	buf = malloc(sizeof(cyg_uint32)*512*blocknum);		
	diag_printf("\ntest: buf: %x\n", buf);			
	
	if (buf)
	{		
		//WRITE OUT a block thro IO		
		blen = blocknum;
		bpos = 933;	//assigned
		//memset(buf, 0xEE, sizeof(cyg_uint32)*1024);
		err = cyg_io_bwrite(handle_usb, buf, &blen, bpos);
		
		if (err == ENOERR)
		{
		    diag_printf("cyg_io_bread() success.\n");
		    diag_printf("=======================================\n");
		    diag_printf("         DATA WRITE BACK (1B)          \n");
		    diag_printf("=======================================\n");				
			diag_dump_buf_with_offset(buf, 512*2, buf);
			diag_printf("=======================================\n");
		}
		else          
		    diag_printf("cyg_io_bwrite() err = %d\n", err);
		    
		free(buf);
	}	    					    	                    								
	diag_printf("BlockWrite_MultiBlock Finished!!!\n");	
	
}

//Write out single block continously one by one
void BlockWrite_SingleBlock_repeate(cyg_io_handle_t handle_usb, cyg_uint8 blocknum)
{
	Cyg_ErrNo err = ENOERR;
	cyg_uint8 *buf = NULL;
	cyg_uint32 blen=0, bpos=0;
	int i=0;
 				
	diag_printf("BlockWrite_SingleBlock_repeate (from host) TEST.\n");		
	
	buf = malloc(sizeof(cyg_uint32)*512*blocknum);
	diag_printf("\ntest: buf: %x\n", buf);		
	 
    if (buf)
	{			
		//memset(buf, 0xFF, 512);
		for(i=0; i<blocknum; i++)
		{
			//WRITE OUT blocks thro IO
			blen = 1;
			bpos = i;
			
			err = cyg_io_bwrite(handle_usb, buf, &blen, bpos);
			if (err == ENOERR)
			{
			    diag_printf("cyg_io_bread() success.\n");
			    diag_printf("=======================================\n");
			    diag_printf("        DATA WRITE BACK (M-Bs)         \n");
			    diag_printf("=======================================\n");
			    diag_dump_buf_with_offset(buf, 512, buf);
			    diag_printf("=======================================\n");
			}
			else
			{
			    diag_printf("cyg_io_bwrite() err = %d\n", err);
			}
		}
		free(buf);
	}		
	diag_printf("BlockWrite_SingleBlock_repeate Finished.\n");		
}
