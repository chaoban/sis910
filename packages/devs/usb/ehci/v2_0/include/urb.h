#ifndef _urb_H_
#define _urb_H_

//#include <cyg/hal/hal_if.h>             // delays
#include <stdio.h>
#include <cyg/infra/cyg_type.h> // base types
#include <cyg/io/hcd.h>
#include <cyg/io/usb.h>


int usb_control_msg(struct USB_DEVICE udev, struct hc_driver hcd, int direct, cyg_uint16 request, \
					cyg_uint16 value, cyg_uint16 index, cyg_uint8 *data, cyg_uint16 data_len);
					
cyg_uint32 usb_bulk_msg(struct USB_DEVICE udev, struct hc_driver hcd, int direct, \
						cyg_uint8 *setup_data, cyg_uint8 setup_len, \
           				cyg_uint8 *data, cyg_uint32 data_len, cyg_uint8 *status);

#endif
