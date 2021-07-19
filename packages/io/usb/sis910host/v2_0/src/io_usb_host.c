#include <stdio.h>
#include <stdlib.h>                      /* printf */
#include <string.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>		// For diagnostic printing.
#include <cyg/io/usb.h>

#include <pkgconf/io_usb_host.h>
#include <cyg/io/usberr.h>			//usb err / debug


int usb_set_current_lun(cyg_io_handle_t *handle_usb, cyg_uint32 curr)
{
	Cyg_ErrNo err = ENOERR;
    cyg_uint8 clun = 0;
    cyg_uint32 clen=1; //fixed value; can't be zero
diag_printf("usb_set_current_lun: START : curr= %x\n", curr);    
	clun = curr;
	
	cyg_io_set_config(*handle_usb, USB_SET_CONFIG_SET_CURRENT_LUN, &clun, &clen);	
	
	return err;
}

int usb_get_max_lun(cyg_io_handle_t *handle_usb, cyg_uint32 *max)
{
	Cyg_ErrNo err = ENOERR;	
	cyg_uint32 mlen=0xFF;
	cyg_uint8 *maxlun=malloc(sizeof(cyg_uint8)*512);	
diag_printf("usb_get_max_lun: START : max= %x\n", *max);
	err = cyg_io_get_config(*handle_usb, USB_GET_CONFIG_GET_MAX_LUN, maxlun, &mlen);
	
	if(0xFF == mlen)
		err = USB_GET_MAX_LUN_ERROR;
	else
		*max = mlen;
	
	return err;
}