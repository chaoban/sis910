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

typedef struct cyg_usb_bus {
    cyg_drv_mutex_t         usb_lock;
} cyg_usb_bus;

typedef struct cyg_usb_device {
    struct cyg_usb_bus*     usb_bus;
    cyg_uint32              usb_baseaddr;
    cyg_uint8               usb_wide_bus_disable;
    cyg_uint8               usb_gateway_xfer;
} cyg_usb_device;

// Details of USB info
typedef struct cyg_usb_disk_info_t {
    // cyg_spi_device*     mmc_spi_dev;
    cyg_usb_device*     usb_dev;
//    cyg_uint32          mmc_saved_baudrate;
    cyg_uint32          usb_block_count;
//    cyg_bool            mmc_read_only;
    cyg_bool            usb_connected;
    cyg_uint32          usb_heads_per_cylinder;
    cyg_uint32          usb_sectors_per_head;
    cyg_uint32          usb_read_block_length;
    cyg_uint32          usb_write_block_length;
//    cyg_uint8           mmc_card_type;
    cyg_uint8           usb_bus_width;
//    mmc_cid_register    mmc_cid;
//    mmc_csd_register    CSD;
//    cyg_uint32          RCA;
//    cyg_uint32          OCR;
} cyg_usb_disk_info_t;




#endif
