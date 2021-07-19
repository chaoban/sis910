//******************************************
// urb.c  
// transation for control and bulk
//******************************************
#include <cyg/io/urb.h>
#include <cyg/io/mass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/ohci.h>
/************************************************************
* Function Prototype
*************************************************************/

/************************************************************
* Local variables
*************************************************************/


/************************************************************
* Main functions
*************************************************************/
int usb_control_msg(struct USB_DEVICE udev, struct hc_driver hcd, int direct, cyg_uint16 request, 
										cyg_uint16 value, cyg_uint16 index, cyg_uint8 *data, cyg_uint16 data_len)
{
	struct usb_ctrlrequest dr;
	int retval = 0;
	cyg_uint8 *setup_data = NULL;
	int i=0;
	
	dr.wRequest = request; //(requestType + request)
	dr.wValue 	= value;	
	dr.wIndex 	= index;
	dr.wLength 	= data_len << 8;	//length should be the same as data length!!
	setup_data 	= (cyg_uint8 *) &dr;

#ifdef USB_DEBUG_CTRL
diag_printf("urb.c: usb_control_msg: \n");
diag_printf("  bDeviceAddress: %08x ; data %x ;data_len: %x\n", udev.bDeviceAddress, data, data_len);
for(i=0; i<8; i++)
{
	diag_printf("  ctrl[%d]: %02x  ", i, setup_data[i]);	
	if((i%3)==0)	diag_printf("\n");	
}
diag_printf("\n");
for(i=0; i<data_len; i++)
{
	diag_printf("  data[%d]: %02x  ",i, data[i]);
	if((i%3)==0)	diag_printf("\n");	
}
diag_printf("\n");
#endif		
	
	hcd.control_transfer(	udev.bDeviceAddress,		//address
							(cyg_uint8 *) &dr,			//setup data
							data, 
							data_len);
							
#ifdef USB_DEBUG_CTRL
for(i=0; i<data_len; i++)
{
	diag_printf("  returned data[%d]: %02x  ",i, data[i]);
	if((i%3)==0)	diag_printf("\n");
}
diag_printf("\n");
#endif				                 		
	return retval;
}


cyg_uint32 usb_bulk_msg(struct USB_DEVICE udev, struct hc_driver hcd, int direct,
						cyg_uint8 *setup_data, cyg_uint8 setup_len,
                 		cyg_uint8 *data, cyg_uint32 data_len, cyg_uint8 *status)
{
	int i=0;
	cyg_uint32 retval = 0;
	
#ifdef USB_DEBUG_BULK
diag_printf("urb.c: usb_bulk_msg: START: udev.bMaxLUN: %x\n", udev.bMaxLUN);
diag_printf("  udev.bDeviceAddress: %02x ; direct: %02x;  data_len: %02x\n", udev.bDeviceAddress, direct, data_len);
for(i=0;i<15+setup_len;i++)
{
	diag_printf("  setup_data[%d]: %02x  ", i, setup_data[i]);
	if((i%3)==0)	diag_printf("\n");
}
diag_printf("\n");
#endif
	
	hcd.bulk_transfer(	udev.bDeviceAddress,			//address							
						direct, 						//direction
						setup_data, 					//cbw: BOT-setup data
						data,
						data_len, 
						status );						//csw: BOT-status data	

#ifdef USB_DEBUG_BULK						
diag_printf("urb.c: usb_bulk_msg: RETURN\n");
for(i=0;i<13;i++)
{
	diag_printf("  urb.c: setup_len[%d]: %02x  ", i, status[i]);
	if((i%3)==0)	diag_printf("\n");
}
diag_printf("\n");

for(i=0;i<50;i++)
{
	diag_printf("  urb.c: data[%d]: %02x  ", i, data[i]);
	if((i%3)==0)	diag_printf("\n");
}
diag_printf("\n");
#endif
	
	return retval;
}
