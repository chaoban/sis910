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
//#include <cyg/io/disk.h>
//#include <cyg/io/usb.h>
//#include <cyg/io/urb.h>
//#include <cyg/io/mass.h>
//#include <cyg/io/otg.h>
//#include <cyg/io/hcd.h>
//#include <cyg/io/ohci.h>
//#include <cyg/io/ehci.h>
//#include <cyg/io/hc_init.h>
#include <cyg/io/usb/host/io_usb_host.h>
//=========================================
//Locals
//=========================================
extern char Getch(void);


//=========================================
//Function Prototype
//=========================================

//=========================================
//Main Functions
//=========================================
void cyg_user_start(void)
{
	Cyg_ErrNo err = ENOERR;
	cyg_io_handle_t handle_usb;
	cyg_uint8 *buf = NULL;
	cyg_uint32 blen=0, bpos=0;
	
	diag_printf("Prepare to test USB.\n");
	
	//USB init in lookup
	err = cyg_io_lookup("/dev/usb/ehci", &handle_usb);
	
	if (err == ENOERR)
 	{			 				
		diag_printf("USB Single Write (from host) TEST.\n");		
		
		//READ IN a block thro IO
		blen = 1;
		bpos = 0;	
		memset(buf, 0xFF, 512);
		err = cyg_io_bread(handle_usb, buf, &blen, bpos);
		diag_dump_buf_with_offset(buf, 512, buf);
		diag_printf("block io read Finished.\n");
		free(buf);	
Getch();		
		//WRITE OUT a block thro IO
		blen = 1;
		bpos = 933;
		memset(buf, 0xFF, 512);
		err = cyg_io_bwrite(handle_usb, buf, &blen, bpos);
		diag_dump_buf_with_offset(buf, 512, buf);
		diag_printf("block io write Finished.\n");
		free(buf);					
Getch();	
	}
}
