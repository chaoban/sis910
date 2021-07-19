#ifndef _usb_H_
#define _usb_H_

#include <cyg/infra/cyg_type.h> // base types
#include <errno.h>	// Cyg_ErrNo

//************************************************************
// Data Declaration
//************************************************************

struct USB_DEVICE {
        cyg_uint8      bActive;
        cyg_uint8      bHubDeviceNumber;
        cyg_uint8      bHubPortNumber;
        cyg_uint8      bDeviceAddress;
        cyg_uint8      bEndpointSpeed;
        cyg_uint8      bEndpointNum;

        cyg_uint16     wEndp0MaxPacket;
        cyg_uint8      bBulkInEndpoint;
        cyg_uint16     wBulkInMaxPkt;
        cyg_uint8      bBulkOutEndpoint;
        cyg_uint16     wBulkOutMaxPkt;
        cyg_uint8      bDataSync;

        cyg_uint8      bNumConfigs;
        cyg_uint8      bNumInterfaces;
        cyg_uint8      bConfigNum;
        cyg_uint8      bInterfaceNum;
        cyg_uint8      bAltSettingNum;
        cyg_uint8      bBaseClass;
        cyg_uint8      bSubClass;
        cyg_uint8      bProtocol;
        
        cyg_uint8      bLUN;
        cyg_uint8	   bMaxLUN;   
} __attribute__ ((aligned (16)));

#define EHCI									1
#define OHCI									0

#define MaxDevice								1
#define PORT 									1

#define ISROOTHUB                               0x80
#define ROOTHUB                                 0x81
#define MAX_BULK_DATA_SIZE						(1024*16)

#define USB_CONTROL_STALLED                     (1 << 2)
#define USB_BULK_TIMEDOUT                       (1 << 1)
#define USB_BULK_STALLED                        (1 << 0)
#define USB_COMPLETE                            0

#define OUT                                     0
#define IN                                      1
#define OFF                                     0
#define ON                                      1
#define WRITE									0
#define	READ									1

#define USB_EPSPEED_FULLSPEED                   (1 << 1)
#define USB_EPSPEED_LOWSPEED                    (1 << 0)
#define USB_EPSPEED_HISPEED                     0

#define USB_PORT_STAT_DEV_CONNECT_CHANGED       (1 << 3)
#define USB_PORT_STAT_DEV_CONNECTED             (1 << 2)
#define USB_PORT_STAT_DEV_FULLSPEED             (1 << 1)
#define USB_PORT_STAT_DEV_LOWSPEED              (1 << 0)
#define USB_PORT_STAT_DEV_HISPEED               (0 << 0)
#define USB_PORT_STAT_DEV_SPEED_MASK            0x03

#define BASE_CLASS_MASS_STORAGE                 0x08
#define PROTOCOL_CBI                            0x00
#define PROTOCOL_CB								0x01
#define PROTOCOL_BOT                            0x50

#define USB_COMMAND_SCSI						0x06
#define	USB_COMMAND_UFI							0x04

//-------------------------------------------------------------
struct usb_ctrlrequest {
	//cyg_uint8      bRequestType;
	//cyg_uint8      bRequest;
	cyg_uint16		wRequest;
	cyg_uint16		wValue;
	cyg_uint16		wIndex;
	cyg_uint16		wLength;
};

#define USB_DIR_OUT                     0               /* to device */
#define USB_DIR_IN                      0x80            /* to host */
#define USB_TYPE_MASK                   (0x03 << 5)
#define USB_TYPE_STANDARD               (0x00 << 5)
#define USB_TYPE_CLASS                  (0x01 << 5)
#define USB_TYPE_VENDOR                 (0x02 << 5)
#define USB_TYPE_RESERVED               (0x03 << 5)

#define USB_RECIP_MASK                  0x1F
#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02
#define USB_RECIP_OTHER                 0x03

#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
#define USB_REQ_SET_FEATURE             0x03
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C

#define USB_DT_DEVICE                   0x01
#define USB_DT_CONFIG                   0x02
#define USB_DT_STRING                   0x03
#define USB_DT_INTERFACE                0x04
#define USB_DT_ENDPOINT                 0x05
#define USB_DT_DEVICE_QUALIFIER         0x06
#define USB_DT_OTHER_SPEED_CONFIG       0x07
#define USB_DT_INTERFACE_POWER          0x08
#define USB_DT_OTG                      0x09

#define USB_DEVICE_SELF_POWERED         0       /* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP        1       /* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE            2       /* (wired high speed only) */
#define USB_DEVICE_BATTERY              2       /* (wireless) */
#define USB_DEVICE_B_HNP_ENABLE         3       /* (otg) dev may initiate HNP */
#define USB_DEVICE_WUSB_DEVICE          3       /* (wireless)*/
#define USB_DEVICE_A_HNP_SUPPORT        4       /* (otg) RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT    5       /* (otg) other RH port does */
#define USB_DEVICE_DEBUG_MODE           6       /* (special devices only) */

#define USB_ENDPOINT_HALT               0       /* IN/OUT will STALL */
#define USB_DEVICE_REMOTE_WAKEUP        1       /* dev may initiate wakeup */
#define USB_ENDPOINT_HALT               0       /* IN/OUT will STALL */

#define USB_CTRLREQUEST_SIZE            8

//	Descriptor Structures
// --------------------------------------------------------------------------
struct usb_device_descriptor {
        cyg_uint8      bLength;
        cyg_uint8      bDescriptorType;

        cyg_uint16     bcdUSB;
        cyg_uint8      bDeviceClass;
        cyg_uint8      bDeviceSubClass;
        cyg_uint8      bDeviceProtocol;
        cyg_uint8      bMaxPacketSize0;
        cyg_uint16     idVendor;
        cyg_uint16     idProduct;
        cyg_uint16     bcdDevice;
        cyg_uint8      iManufacturer;
        cyg_uint8      iProduct;
        cyg_uint8      iSerialNumber;
        cyg_uint8      bNumConfigurations;
};// __attribute__ ((aligned (32)));


#define USB_DT_DEVICE_SIZE              18


// -------------------------------------------------------------------------- 
struct usb_config_descriptor {
        cyg_uint8      bLength;
        cyg_uint8      bDescriptorType;

        cyg_uint16     wTotalLength;
        cyg_uint8      bNumInterfaces;
        cyg_uint8      bConfigurationValue;
        cyg_uint8      iConfiguration;
        cyg_uint8      bmAttributes;
        cyg_uint8      bMaxPower;
};// __attribute__ ((aligned (32)));

#define USB_DT_CONFIG_SIZE              9
#define USB_MAX_CONTROL_DATA_SIZE       0x200

#define USB_CONFIG_ATT_ONE              (1 << 7)        /* must be set */
#define USB_CONFIG_ATT_SELFPOWER        (1 << 6)        /* self powered */
#define USB_CONFIG_ATT_WAKEUP           (1 << 5)        /* can wakeup */
#define USB_CONFIG_ATT_BATTERY          (1 << 4)        /* battery powered */



// -------------------------------------------------------------------------- 
struct usb_interface_descriptor {
        cyg_uint8      bLength;
        cyg_uint8      bDescriptorType;

        cyg_uint8      bInterfaceNumber;
        cyg_uint8      bAlternateSetting;
        cyg_uint8      bNumEndpoints;
        cyg_uint8      bInterfaceClass;
        cyg_uint8      bInterfaceSubClass;
        cyg_uint8      bInterfaceProtocol;
        cyg_uint8      iInterface;
};// __attribute__ ((aligned (8)));

#define USB_DT_INTERFACE_SIZE           9

// -------------------------------------------------------------------------- 
struct usb_endpoint_descriptor {
        cyg_uint8      bLength;
        cyg_uint8      bDescriptorType;

        cyg_uint8      bEndpointAddress;
        cyg_uint8      bmAttributes;
        //cyg_uint16     wMaxPacketSize;
        cyg_uint8	   wMaxPacketSize_L;
        cyg_uint8      wMaxPacketSize_H;
        cyg_uint8      bInterval;
}; //__attribute__ ((aligned (32)));

#define USB_DT_ENDPOINT_SIZE            7

#define USB_ENDPOINT_NUMBER_MASK        0x0F    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80

#define USB_ENDPOINT_XFERTYPE_MASK      0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3


// --------------------------------------------------------------------------
struct usb_otg_descriptor {
        cyg_uint8      bLength;
        cyg_uint8      bDescriptorType;

        cyg_uint8      bmAttributes;           /* support for HNP, SRP, etc */
};


// Commands
// --------------------------------------------------------------------------
#define USB_OTG_HNP                     (1 << 1)
#define USB_OTG_SRP                     (1 << 0)

#define USB_DEVICE_A_ALT_HNP_SUPPORT    5       /* (otg) other RH port does */
#define USB_DEVICE_A_HNP_SUPPORT        4       /* (otg) RH port supports HNP */
#define USB_DEVICE_B_HNP_ENABLE         3       /* (otg) dev may initiate HNP */

#define USB_RQ_SET_ADDRESS              0x05 | ( (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE	)	<<8 )
#define USB_RQ_GET_DESCRIPTOR           0x06 | ( (USB_DIR_IN  | USB_TYPE_STANDARD | USB_RECIP_DEVICE	)	<<8 )
#define USB_RQ_GET_CONFIGURATION        0x08 | ( (USB_DIR_IN  | USB_TYPE_STANDARD | USB_RECIP_DEVICE	)	<<8 )
#define USB_RQ_SET_CONFIGURATION        0x09 | ( (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE	)	<<8 )
#define USB_RQ_SET_INTERFACE            0x11 | ( (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE )	<<8 )
#define USB_RQ_SET_FEATURE              0x03 | ( (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE	)	<<8 )
                                                                                             
#define USB_RQ_GET_CLASS_DESCRIPTOR     0x06 | ( (USB_DIR_IN  | USB_TYPE_CLASS | USB_RECIP_DEVICE     )	<<8 )
#define HID_RQ_SET_PROTOCOL             0x11 | ( (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE  )	<<8 )
#define HID_RQ_SET_REPORT               0x09 | ( (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE  )	<<8 )
#define HID_RQ_SET_IDLE                 0x0A | ( (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE  )	<<8 )
#define HUB_RQ_GET_PORT_STATUS          0x00 | ( (USB_DIR_IN  | USB_TYPE_CLASS | USB_RECIP_OTHER      )	<<8 )
#define HUB_RQ_SET_PORT_FEATURE         0x03 | ( (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER      )	<<8 )
#define HUB_RQ_CLEAR_PORT_FEATURE       0x01 | ( (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER      )	<<8 )
//BOT
#define USB_RQ_BOT_GET_MAX_LUN					0xFE | ( (USB_DIR_IN  | USB_TYPE_CLASS | USB_RECIP_INTERFACE  )	<<8 )
//CBI
#define ADSC_OUT_REQUEST_TYPE           0x00 | ( (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE  )	<<8 )
#define ADSC_IN_REQUEST_TYPE            0x00 | ( (USB_DIR_IN  | USB_TYPE_CLASS | USB_RECIP_INTERFACE  )	<<8 )

#define ENDPOINT_CLEAR_PORT_FEATURE     0x01 | ( (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT

#define ENDPOINT_HALT                   0x00
//Bulk-only Mass storage
#define BOT_RESET_REQUEST_CODE          0xFF    /* Reset Request code */
#define BOT_GET_MAX_LUN_REQUEST_CODE    0xFE    /* Get Max Lun Request code */

//----------------------------------------------------------------------------------------------
//USB ERROR
#define USB_GET_DESCRIPTOR_ERROR		0x02
#define USB_GET_CONFIGURATION_ERROR		0x03
#define USB_SET_ADDRESS_ERROR			0x04
#define USB_SET_CONFIGURATION_ERROR		0x05

#define USB_BULK_TRANSFER_ERROR			0x10

//************************************************************
// Function Prototype
//************************************************************
Cyg_ErrNo	usb_write(cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos);
Cyg_ErrNo	usb_read(cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos);
Cyg_ErrNo	usb_select(int select);
Cyg_ErrNo	usb_get_config(void);
Cyg_ErrNo	usb_set_config(cyg_uint32 key, const void *buf, cyg_uint32 *len);
void 		usb_enable_hub(int hub, int port);
void 		usb_disable_hub(int hub, int port);
void 		usb_host_init(void);
void 		usb_host_interrupt(void);
cyg_uint8 	get_usb_device(void);
struct hc_driver 	get_usb_hcd(void);

#endif
