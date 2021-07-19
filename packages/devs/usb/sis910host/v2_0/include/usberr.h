#ifndef _usberr_H_
#define _usberr_H_

#include <cyg/io/devtab.h>
#include <cyg/io/io.h>
#include <cyg/infra/cyg_type.h> // base types
#include <errno.h>				// Cyg_ErrNo


//#ifdef CYGDBG_USB_DEBUG
#define USB_DEBUG 1
//#endif

#ifdef CYGDBG_USB_DEBUG_HCD
#define USB_DEBUG_HCD 1
#endif

#ifdef CYGDBG_USB_DEBUG_CTRL
#define USB_DEBUG_CTRL 1
#endif

#ifdef CYGDBG_USB_DEBUG_BULK
#define USB_DEBUG_BULK 1
#endif

//#define USB_DEBUG		1
//#define USB_DEBUG_HCD 	2
//#define USB_DEBUG_CTRL	3
//#define USB_DEBUG_BULK	4

//#ifdef CYGDBG_IO_USB_DEBUG
//#define USB_DEBUG 1
//#endif

//common function start/end
#ifdef USB_DEBUG
	#define D(_args_) diag_printf _args_
#else
	#define D(_args_)
#endif

//ehci/ohci only
#ifdef USB_DEBUG_HCD
	#define HD(_args_) diag_printf _args_
#else
	#define HD(_args_)
#endif

//ctrl only
#ifdef USB_DEBUG_CTRL
	#define CD(_args_) diag_printf _args_
#else
	#define CD(_args_)
#endif

//bulk only
#ifdef USB_DEBUG_BULK
	#define BD(_args_) diag_printf _args_
#else
	#define BD(_args_)
#endif


//----------------------------------------------------------------------------------------------
//USB ERROR
//------------------ OHCI
#define USB_OHCI_CRC_ERR				1
#define USB_OHCI_BITSTUFFING_ERR		2
#define USB_OHCI_DATATOGGLEMISMATCH_ERR	3
#define USB_OHCI_STALL					4
#define USB_OHCI_DEVICE_NO_RESPONDING	5
#define USB_OHCI_PID_CHECK_FAILURE		6
#define USB_OHCI_UNEXPECTED_PID			7
#define USB_OHCI_DATA_OVERRUN			8
#define USB_OHCI_DATA_UNDERRUN			9
#define USB_OHCI_BUFFER_OVERRUN			12
#define USB_OHCI_BUFFER_UNDERRUN		13
#define USB_OHCI_INIT_ERROR				16
#define USB_OHCI_CTRL_PASSIN_ADDR_ERR	17
#define USB_OHCI_CTRL_CHECK_TD_FAIL		18
#define USB_OHCI_BULK_PASSIN_ADDR_ERR	19
#define USB_PORT_DISABLE				20

//------------------- USB CORE
#define USB_INIT_FAIL					21
#define USB_GET_DESCRIPTOR_ERROR		22
#define USB_GET_CONFIGURATION_ERROR		23
#define USB_SET_ADDRESS_ERROR			24
#define USB_SET_CONFIGURATION_ERROR		25
#define USB_NO_DEVICE					26
#define USB_NOT_MASS_STORAGE_CLASS_ERR	27
#define USB_PROTOCOL_ERROR				28

//------------------- MASS BULK
#define USB_BULK_TRANSFER_ERROR			30
#define USB_GET_MAX_LUN_ERROR			31
#define USB_SET_CURRENT_LUN_ERROR		32
#define USB_CHECK_CURRENT_LUN_ERROR		33
#define USB_DISK_READ_CAPACITY_ERROR	34
#define USB_RW_DATA_ERROR				35

//#define USB_PORT_CHANGED				21
//#define USB_ENUMERATION_FAILED			22

#endif         
