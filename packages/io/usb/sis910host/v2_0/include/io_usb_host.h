#ifndef __IO_USB_HOST_H__
#define	__IO_USB_HOST_H__


int usb_set_current_lun(cyg_io_handle_t *handle_usb, cyg_uint32 curr);

int usb_get_max_lun(cyg_io_handle_t *handle_usb, cyg_uint32 *max);

#endif
