#ifndef _usberr_H_
#define _usberr_H_

#include <cyg/io/devtab.h>
#include <cyg/io/io.h>
#include <cyg/infra/cyg_type.h> // base types
#include <errno.h>				// Cyg_ErrNo

#define USB_DEBUG		1
//#define USB_DEBUG_HCD 	2
//#define USB_DEBUG_CTRL	3
#define USB_DEBUG_BULK	4

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


#endif         
