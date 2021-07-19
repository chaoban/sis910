#ifndef _driver_H_
#define _driver_H_

#include <errno.h>				// Cyg_ErrNo
#include <cyg/infra/cyg_type.h> // base types
#include <cyg/io/disk.h>

#include <cyg/io/usb.h>
#include <cyg/io/urb.h>
#include <cyg/io/ehci.h>

#include <cyg/io/mass.h>
#include <cyg/io/slave.h>
#include <cyg/io/hcd.h>     

//typedef struct cyg_usb_bus {
//    cyg_drv_mutex_t         usb_lock;
//} cyg_usb_bus;

//typedef struct cyg_usb_device {
//    struct cyg_usb_bus*     usb_bus;
//    cyg_uint32              usb_baseaddr;
//    cyg_uint8               usb_wide_bus_disable;
//    cyg_uint8               usb_gateway_xfer;
//} cyg_usb_device;

// Details of USB info
typedef struct cyg_usb_disk_info_t {
    // cyg_spi_device*     mmc_spi_dev;
//    cyg_usb_device*     usb_dev;
	cyg_uint32			usb_current_lun;
	cyg_uint32			usb_max_lun;
//    cyg_uint32          usb_block_count;
    cyg_bool            usb_connected;
    cyg_bool			usb_hw_inited;
//    cyg_uint32          usb_read_block_length;
//    cyg_uint32          usb_write_block_length;

} cyg_usb_disk_info_t;


#ifdef ALIGN_USE_FUNC_PTR
#define USB_DISK_INSTANCE(_number_,_clun_,_mlun_,_mbr_supp_,_name_)     \
static cyg_usb_disk_info_t cyg_usb_disk_hwinfo##_number_ = {            \
    usb_current_lun	: (cyg_uint32) _clun_,                              \
    usb_max_lun		: (cyg_uint32) _mlun_,                              \
	usb_connected	: (cyg_bool)false									\
};                                                         				\
DISK_CHANNEL(usb_disk_channel##_number_,                                \
             cyg_usb_disk_funs,                                         \
             cyg_usb_disk_hwinfo##_number_,                             \
             cyg_usb_disk_controller,                                       \
             _mbr_supp_,                                                \
             1                                                          \
);                                                                      \
BLOCK_DEVTAB_ENTRY(usb_disk_io##_number_,                               \
                   _name_,                         						\
                   0,                                        			\
                   &cyg_io_disk_devio,                         			\
                   sis910_usb_init,                         			\
                   sis910_usb_lookup,                         			\
                   &usb_disk_channel##_number_,                         \
                   cyg_usb_buf_alloc                        			\
);
//usb_disk_io##_number_
//_name_,                                             
//(const char*)NULL,                                  
//&cyg_io_disk_devio,                                 
//sis910_usb_init,                                    
//sis910_usb_lookup,                                  
//&usb_disk_channel##_number_,                        
//cyg_usb_buf_alloc                                    

#else
#define USB_DISK_INSTANCE(_number_,_clun_,_mlun_,_mbr_supp_,_name_)     \
static cyg_usb_disk_info_t cyg_usb_disk_hwinfo##_number_ = {            \
    usb_current_lun	: (cyg_uint32) _clun_,                              \
    usb_max_lun		: (cyg_uint32) _mlun_,                              \
	usb_connected	: (cyg_bool)false
};                                                                      \
DISK_CHANNEL(usb_disk_channel##_number_,                                \
             cyg_usb_disk_funs,                                         \
             cyg_usb_disk_hwinfo##_number_,                             \
             cyg_usb_disk_controller,                                       \
             _mbr_supp_,                                                \
             1                                                          \
);                                                                      \
BLOCK_DEVTAB_ENTRY(usb_disk_io##_number_,                               \
                   _name_,                                              \
                   0,                                                   \
                   &cyg_io_disk_devio,                                  \
                   sis910_usb_init,                                       \
                   sis910_usb_lookup,                                     \
                   &usb_disk_channel##_number_                          \
);

#endif

#endif
