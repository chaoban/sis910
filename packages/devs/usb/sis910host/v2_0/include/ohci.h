#ifndef _ohci_H_
#define _ohci_H_
//fake ohci.h
#include <stdio.h>
#include <cyg/infra/cyg_type.h> 	// base types
#include <cyg/io/ehci.h>
//#define WORD cyg_uint32
//#define	CBW_SIGNATURE	0x43425355	//'USBC'
//#define	CBW_TAG			0x53695321	//'SiS!'
//#define	CSW_SIGNATURE	0x53425355	//'USBS'
//#define	CSW_TAG			CBW_TAG

#define	CTRL_TIMEOUT	0x6000
#define	HW_SWAP_EN		1

//#define iiow(a,b)  *(volatile WORD*)(a)=b
//#define iior(a)    *(volatile WORD*)(a)

//cyg_uint32 dram_base_addr = 0x400000;
#define dram_base_addr	0x400000
#define	DRAM_ADDR(x)	(0x40000000+dram_base_addr+x)

#if	HW_SWAP_EN
	#define	DRAM_ADDR_C(x)	(0xC0000000+dram_base_addr+x)
#else
	#define	DRAM_ADDR_C(x)	DRAM_ADDR(x)
#endif

#define	PCI_ADDR(x)		(0xC0000000+x)

/* total 1KB */
//#define HCCA_ADDR				dram_base_addr+0
//#define	ED_CTRL_OFFSET_0		(0+256)						//100
//#define	ED_CTRL_OFFSET_1		(ED_CTRL_OFFSET_0+16)		//110
//#define	ED_CTRL_OFFSET_2		(ED_CTRL_OFFSET_1+16)		//120
//#define	ED_BULKIN_OFFSET_1		(ED_CTRL_OFFSET_2+16)		//130
//#define	ED_BULKIN_OFFSET_2		(ED_BULKIN_OFFSET_1+16)		//140
//#define	ED_BULKOUT_OFFSET_1		(ED_BULKIN_OFFSET_2+16)		//150
//#define	ED_BULKOUT_OFFSET_2		(ED_BULKOUT_OFFSET_1+16)	//160
//#define	ED_INT_OFFSET_1			(ED_BULKOUT_OFFSET_2+16)	//170
//#define	ED_INT_OFFSET_2			(ED_INT_OFFSET_1+16)		//180
//#define	BOT_OFFSET				(ED_INT_OFFSET_2+16)		//190, 31 bytes
//#define	CBI_OFFSET				(BOT_OFFSET+32)				//1B0, 12 bytes
//#define	TD_BUF					(CBI_OFFSET+16)				//1C0

enum {
	CTRL=0,
	BULKIN,
	BULKOUT
};

enum {
	ADSC_CMD,
	BOT_CMD
};

enum {
	ED_CTRL_0,
	ED_CTRL_1,
	ED_CTRL_2,
	ED_BULKIN_1,
	ED_BULKIN_2,
	ED_BULKOUT_1,
	ED_BULKOUT_2,
	ED_INT_1,
	ED_INT_2,
	ED_ISO_1,	//NULL
	ED_ISO_2	//NULL
};

#define gw_fifo_reset                   0x90001710
#define gw_wff_val0                     0x90001740
#define gw_wff_val1                     0x90001744
#define gw_wff_val2                     0x90001748
#define gw_wff_val3                     0x9000174c
#define gw_wff_val4                     0x90001750
#define gw_wff_val5                     0x90001754
#define gw_wff_val6                     0x90001758
#define gw_wff_val7                     0x9000175c
#define gw_rff_val0                     0x90001760
#define gw_rff_val1                     0x90001764
#define gw_rff_val2                     0x90001768
#define gw_rff_val3                     0x9000176c
#define gw_rff_val4                     0x90001770
#define gw_rff_val5                     0x90001774
#define gw_lpc0                         0x90001500
#define gw_lpc1                         0x90001504
#define gw_lpc2                         0x90001508
#define gw_lpc3                         0x9000150c
#define gw_lpc4                         0x90001510
#define gw_lpc5                         0x90001514
#define gw_lpc6                         0x90001518
#define gw_lpc7                         0x9000151c
#define gw_lpc8                         0x90001520
#define gw_lpc9                         0x90001524
#define gw_lpc10                        0x90001528
#define gw_lpc11                        0x9000152c
#define gw_lpc12                        0x90001530
#define gw_lpc13                        0x90001534
#define gw_lpc14                        0x90001538
#define gw_lpc15                        0x9000153c

void ohci_irq(void);

//--setup bus setting, such as pci..
bool ohci_bus_register(void);

//initialize HC
cyg_uint32 ohci_init (void);

cyg_uint32 ohci_start(cyg_uint32 port);

cyg_uint8 ohci_check_portstatus(cyg_uint32 port);

//--start HCD running
bool ohci_run(void);


//--reset HC
bool ohci_reset(cyg_uint32 port);


//--called before resuming the hub
bool ohci_resume(void);


//--cleanly make HCD stop
bool ohci_stop(void);


//--manage control/bulk transation
//parameter:	transfer the structure data to raw data for EHCI/OHCI
//return: 		actual transfer size
//						the sent-back message or data is returned in "data" (csw is in "status" param)
int	ohci_control_transfer(int address, cyg_uint8 *setup_data, cyg_uint8 *data, cyg_uint16 data_len);

int	ohci_bulk_transfer(int address, int dir_in, cyg_uint8 *setup_data , cyg_uint8 *data, cyg_uint16 data_len, cyg_uint8 *status);


//--Release the current
int ohci_release_endpoint(int endpoint);


//--root hub support
//Enable/disable/get status -->root hub
int ohci_rh_enable(void);

int ohci_rh_disable(void);

int ohci_rh_state(void);

void ohci_control_set_packetsize (cyg_uint8 dev_address, cyg_uint16 maxsize);

void ohci_bulk_set_packetsize (	cyg_uint8 dev_address,
								cyg_uint8 bulkInEdNum, cyg_uint16 binmaxsize,
								cyg_uint8 bulkOutEdNum, cyg_uint16 boutmaxsize	);

#endif
