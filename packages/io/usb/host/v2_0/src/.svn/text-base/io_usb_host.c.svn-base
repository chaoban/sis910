#include <stdio.h>
#include <stdlib.h>                      /* printf */
#include <string.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>		// For diagnostic printing.
//#include <cyg/io/usb/sis910host/io_usb_host.h>
#include <pkgconf/io_usb_host.h>

cyg_io_handle_t usb_handle = NULL;
cyg_uint8 *buf = NULL;
cyg_uint32 len = 1;

int USB_setup(void)
{
	Cyg_ErrNo err;
	cyg_io_handle_t usb_handle = NULL;
//	cyg_uint32 buffer[ 1 ] = {-1};
//	cyg_uint32 len = 4;//4 byte
//	buffer[0] = value;
	buf = malloc(sizeof(cyg_uint8)*512*2);
	err = cyg_io_lookup("/dev/usb/ehci", &usb_handle);
	if(ENOERR != err || NULL == usb_handle) 
    {
        diag_printf("usb IO cyg_io_lookup error!!! \n");
        return -1;
    }

    return 0;
}

int USB_Read(void *buf, cyg_uint32 *block, cyg_uint32 pos)
{
	Cyg_ErrNo err = ENOERR;	
	
	err = cyg_io_bread(usb_handle, buf, block, pos);
	
    if (err != ENOERR)
    {
        diag_printf("USB: cyg_io_bread() err = %d\n", err);
    }  
    
    return err;  	
}

int USB_Write(void *buf, cyg_uint32 *block, cyg_uint32 pos)
{
	Cyg_ErrNo err = ENOERR;	
	
	err = cyg_io_bwrite(usb_handle, buf, block, pos);
	
    if (err != ENOERR)
    {
        diag_printf("USB: cyg_io_bwrite() err = %d\n", err);
    } 
    
	return err;         	
}
