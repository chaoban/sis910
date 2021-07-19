#ifndef _mass_H_
#define _mass_H_

#include <stdio.h>
#include <cyg/infra/cyg_type.h> 	// base types
#include <cyg/io/hcd.h>
#include <cyg/io/usb.h>
#include <cyg/io/urb.h>


// Command Block Wrapper Signature 'USBC'

#define CBW_SIGNATURE                   0x55534243
#define CBW_FLAGS_DATA_IN               0x80
#define CBW_FLAGS_DATA_OUT              0x00

#define CBW_SIZE						31

#define CBW_SIZE_ERROR					0x10

// Command Status Wrapper Signature 'USBS'

#define CSW_SIGNATURE                   0x55534253

#define CBW_CBWCB_SIZE                  16

#define CSW_STATUS_GOOD                 0x00
#define CSW_STATUS_FAILED               0x01
#define CSW_STATUS_PHASE_ERROR          0x02
#define CSW_DATA_ERROR					0x05
#define CSW_DATA_STATUS_FAILED			0x06

#define USBMSC_INTERFACE_CLASS          0x08
#define USBMSC_SUBCLASS_RBC             0x01
#define USBMSC_SUBCLASS_SFF8020i        0x02
#define USBMSC_SUBCLASS_QIC157          0x03
#define USBMSC_SUBCLASS_UFI             0x04
#define USBMSC_SUBCLASS_SFF8070i        0x05
#define USBMSC_SUBCLASS_SCSI            0x06

#define USBMSC_INTERFACE_PROTOCOL_CBIT  0x00
#define USBMSC_INTERFACE_PROTOCOL_CBT   0x01
#define USBMSC_INTERFACE_PROTOCOL_BOT   0x50

#define USBMSC_DEFAULT_LUN_NUMBER		0x00

#define MAX_USB_BULK_LENGTH             0x2000L

// Command Block Wrapper

struct CBW {
	cyg_uint32	dCBWSignature;          // 0-3
	cyg_uint32	dCBWTag;                // 4-7
	cyg_uint32	dCBWDataTransferLength; // 8-11
	cyg_uint8 	bmCBWFlags;             // 12
	cyg_uint8 	bCBWLUN;                // 13
	cyg_uint8 	bCBWCBLength;           // 14
	cyg_uint8 	CBWCB[CBW_CBWCB_SIZE];  // 15-30
};

// Command Status Wrapper

struct CSW {
	cyg_uint32	dCSWSignature;          // 0-3
	cyg_uint32	dCSWTag;                // 4-7
	cyg_uint32	dCSWDataResidue;        // 8-11
	cyg_uint8	bCSWStatus;             // 12
};


// SCSI commands that we recognize
#define SC_TEST_UNIT_READY					0x00
#define SC_REQUEST_SENSE					0x03
#define SC_INQUIRY							0x12
#define SC_READ_10							0x28
#define SC_READ_12							0xa8
#define SC_READ_FORMAT_CAPACITIES			0x23
#define SC_READ_CAPACITY					0x25
#define SC_WRITE_10							0x2a
#define SC_WRITE_12							0xaa

// SCSI commands data length
#define SC_INQUIRY_LENGTH					0x24
#define SC_TEST_UNIT_READY_LENGTH			0x00
#define SC_REQUEST_SENSE_LENGTH				0x12
#define SC_READ_CAPACITY_LENGTH				0x08
#define SC_READ_FORMAT_CAPACITIES_LENGTH	0x0c


// Public Function Prototype
int usb_mass_bulk_transport(struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos);
int usb_mass_CB_transport(struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos);
int usb_mass_storage_transport(struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos);

int usb_mass_bulk_cmd(cyg_uint8 command, struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos);

//pack commands in cbwcb
cyg_uint8 command_packing(cyg_uint8 command, cyg_uint8 *data, cyg_uint32 blockNum, cyg_uint32 pos, cyg_uint8 lun);

//pack the complete package of cbw to send out
cyg_uint32 usb_bulk_setup(	cyg_uint8 command, int direction, struct CBW *cbw, \
									cyg_uint32 data_len, cyg_uint32 data_pos, \
									cyg_uint8 maxlun, cyg_uint8 curlun);
									
//parse data content and check the returned status in csw
cyg_uint32 usb_bulk_status(cyg_uint8 command, cyg_uint8 *data, struct CSW csw);
#endif
