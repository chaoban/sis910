//******************************************
// hcd.c  
//The interface between usb core and 
//host controller driver for EHCI and OHCI
//******************************************
#include <cyg/io/hcd.h>
#include <cyg/io/ehci.h>
#include <cyg/io/ohci.h>
#include <cyg/infra/diag.h>
#include <cyg/io/usberr.h>			//usb err / debug
//=========================================
//Locals
//=========================================
			struct hc_driver sis_ehci_hc_driver = {
			// generic hardware linkage
//			.irq =			ehci_irq,
//		
//			// basic lifecycle operations
//			.bus_register = ehci_bus_register,
			.start =		ehci_start,
//			.run =			ehci_run,
//			.stop =			ehci_stop,
//			.reset =		ehci_reset,
//			.resume =   ehci_resume,
		
			// managing i/o requests and associated device resources
			.control_transfer =		ehci_control_transfer,			           
			.bulk_transfer =		ehci_bulk_transfer,
			
			.ctrl_init = 	EHCI_control_set_packetsize,
			.bulk_init = 	EHCI_bulk_set_packetsize,			
			//Release the current
//			.release_endpoint =		ehci_release_endpoint,
		
			// root hub support		
//			.rh_enable =			ehci_rh_enable,
//			.rh_disable =			ehci_rh_disable,
//			.rh_state =				ehci_rh_state,
		};
		
	struct hc_driver sis_ohci_hc_driver = {
			// generic hardware linkage
//			.irq =			ohci_irq,
//		
//			// basic lifecycle operations
//			.bus_register = ohci_bus_register,
			.start =		ohci_start,
//			.run =			ohci_run,
//			.stop =			ohci_stop,
//			.reset =		ohci_reset,
//			.resume =   ohci_resume,
		
			// managing i/o requests and associated device resources
			.ctrl_init = 	ohci_control_set_packetsize,
			.bulk_init = 	ohci_bulk_set_packetsize,
			.control_transfer =		ohci_control_transfer,
			.bulk_transfer =		ohci_bulk_transfer,
			
			//Release the current
//			.release_endpoint =		ohci_release_endpoint,
//		
//			// root hub support		
//			.rh_enable =			ohci_rh_enable,
//			.rh_disable =			ohci_rh_disable,
//			.rh_state =				ohci_rh_state,
		};
//=========================================
//Function Prototype
//=========================================

//=========================================
//Main Functions
//=========================================

struct hc_driver hcd_init(int ehci)
{	
	HD(("hcd.c : hcd_init: start\n"));
	if(ehci)//ehci or ohci??
	{
		HD(("hcd.c : hcd_init: ehci\n"));
		return sis_ehci_hc_driver;
	}
	else		
	{
		HD(("hcd.c : hcd_init: ohci\n"));
		return sis_ohci_hc_driver;		
	}
}
