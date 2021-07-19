#ifndef _hcd_H_
#define _hcd_H_

#include <stdio.h> 
#include <cyg/infra/cyg_type.h> // base types
#include <cyg/kernel/kapi.h>
#include <cyg/io/devtab.h>

#include <cyg/io/ehci.h>

//HCD functions
struct hc_driver {

	//--irq handler
	void	(*irq)						(void);

	//--setup bus setting, such as pci..
//	int (*bus_register)		(void);
	
	//initialize HC
	cyg_uint32 (*start) 					(cyg_uint32 port);			

	//--start HCD running	
	int (*run)							(void);
	
	//--reset HC
//	int (*reset)						(void);	
//
//	//--called before resuming the hub
//	int (*resume) 					(void);
//
//	//--cleanly make HCD stop
//	int (*stop) 						(void);

	void (*ctrl_init)			(	cyg_uint8 dev_address, cyg_uint16 maxsize);
	
	void (*bulk_init)				(	cyg_uint8 dev_address, 
										cyg_uint8 bulkInEdNum, cyg_uint16 binmaxsize, 
										cyg_uint8 bulkOutEdNum, cyg_uint16 boutmaxsize	);
											
	//--manage control/bulk transation 
	//parameter:	transfer the structure data to raw data for EHCI/OHCI
	//return: 		actual transfer size
	//						the sent-back message or data is returned in "data" (csw is in "status" param)
	int	(*control_transfer)	(int address, cyg_uint8 *setup_data, cyg_uint8 *data, cyg_uint16 data_len);
	int	(*bulk_transfer)	(int address, int dir_in, cyg_uint8 *setup_data , cyg_uint8 *data, cyg_uint16 data_len, cyg_uint8 *status);

	//--Release the current 
	int (*release_endpoint)	(int endpoint);
	
	//--root hub support
	//Enable/disable/get status -->root hub
//	int (*rh_enable)				(void);
//	int (*rh_disable)				(void);
//	int (*rh_state)					(void);
};

struct hc_driver hcd_init(int ehci);

#endif
