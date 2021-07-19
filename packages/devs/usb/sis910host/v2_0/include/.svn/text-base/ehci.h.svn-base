#ifndef _ehci_H_
#define _ehci_H_
//fake ehci.h
#include <stdio.h>
#include <cyg/infra/cyg_type.h> 	// base types
#include <cyg/io/ohci.h>
//#define	NULL	0
//#define	UINT32 unsigned long
//#define	UINT16 unsigned short
//#define	UINT8  unsigned char
//#define	DRAM_ADDR(x)	(0x40000000+dram_base_addr+x)
//#define	PCI_ADDR(x)		(0xC0000000+x)
#define	QHD_LEN	64
#define	EHCI_TIMEOUT	0x8000
//#define	CTRL_TIMEOUT	0x2000
//#define __FUNCTION__ 	0
#define	HW_SWAP_EN		1

#define iiow(a,b)  *(volatile cyg_uint32 *)(a)=b
#define iior(a)    *(volatile cyg_uint32 *)(a)

//cyg_uint32 dram_base_addr = 0x400000;
#define ehci_dram_base_addr	0x400000
#define	EHCI_DRAM_ADDR(x)	(0x40000000+ehci_dram_base_addr+x)
#define	PCI_ADDR(x)		(0xC0000000+x)

/* total 1KB */
#define	EHCI_PERIODIC_LIST		(0)			//0000
#define	QHD_CTRL_OFFSET_0		(EHCI_PERIODIC_LIST+4096+64)//x040
#define	QHD_CTRL_OFFSET_1		(QHD_CTRL_OFFSET_0+QHD_LEN)		//x080
#define	QHD_CTRL_OFFSET_2		(QHD_CTRL_OFFSET_1+QHD_LEN)		//x0C0
#define	QHD_BULKIN_OFFSET_1		(QHD_CTRL_OFFSET_2+QHD_LEN)		//x100
#define	QHD_BULKIN_OFFSET_2		(QHD_BULKIN_OFFSET_1+QHD_LEN)	//x140
#define	QHD_BULKOUT_OFFSET_1	(QHD_BULKIN_OFFSET_2+QHD_LEN)	//x180
#define	QHD_BULKOUT_OFFSET_2	(QHD_BULKOUT_OFFSET_1+QHD_LEN)	//x1C0
#define	QHD_INT_OFFSET_1		(QHD_BULKOUT_OFFSET_2+QHD_LEN)	//x200
#define	QHD_INT_OFFSET_2		(QHD_INT_OFFSET_1+QHD_LEN)		//x240
#define	QHD_BUFFER_OFFSET		(QHD_INT_OFFSET_2+QHD_LEN)		//x280
//#define	QHD_BUFFER_OFFSET	(QHD_INT_OFFSET_2+1024)		//x280
#define	EHCI_BOT_OFFSET			(QHD_BUFFER_OFFSET+QHD_LEN)		//x2C0, 31 bytes
#define	EHCI_CBI_OFFSET			(EHCI_BOT_OFFSET+32)				//x2E0, 12 bytes
#define	QTD_BUF					(EHCI_CBI_OFFSET+32)				//x300
#define QTD_BUF_END				(QTD_BUF+1024)

/* total 1KB */
#define HCCA_ADDR				(QTD_BUF_END+0)//dram_base_addr+0
#define	ED_CTRL_OFFSET_0		(QTD_BUF_END+256)//(0+256)						//100
#define	ED_CTRL_OFFSET_1		(ED_CTRL_OFFSET_0+16)		//110
#define	ED_CTRL_OFFSET_2		(ED_CTRL_OFFSET_1+16)		//120
#define	ED_BULKIN_OFFSET_1		(ED_CTRL_OFFSET_2+16)		//130
#define	ED_BULKIN_OFFSET_2		(ED_BULKIN_OFFSET_1+16)		//140
#define	ED_BULKOUT_OFFSET_1		(ED_BULKIN_OFFSET_2+16)		//150
#define	ED_BULKOUT_OFFSET_2		(ED_BULKOUT_OFFSET_1+16)	//160
#define	ED_INT_OFFSET_1			(ED_BULKOUT_OFFSET_2+16)	//170
#define	ED_INT_OFFSET_2			(ED_INT_OFFSET_1+16)		//180
#define	BOT_OFFSET				(ED_INT_OFFSET_2+16)		//190, 31 bytes
#define	CBI_OFFSET				(BOT_OFFSET+32)				//1B0, 12 bytes
#define	TD_BUF					(CBI_OFFSET+16)				//1C0


#define	TEST_PACKET	0
#define	TEST_K	0
#define	TEST_J	0
#define	CHIRP_JK	0
//enum {
//	CTRL=0,
//	BULKIN,
//	BULKOUT
//};
//
//enum {
//	ADSC_CMD,
//	BOT_CMD
//};

enum {
	NO_DEIVCE		=  6,
	IS_HI_SPEED		=  0,
	IS_FULL_SPEED	=  1,
	IS_LOW_SPEED	=  2
};

enum {
	QHD_CTRL_0,
	QHD_CTRL_1,
	QHD_CTRL_2,
	QHD_BULKIN_1,
	QHD_BULKIN_2,
	QHD_BULKOUT_1,
	QHD_BULKOUT_2,
	QHD_INT_1,
	QHD_INT_2,
	QHD_ISO_1,	//NULL
	QHD_ISO_2	//NULL
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


void EHCI_control_set_packetsize(cyg_uint8 dev_idx, cyg_uint16 pkt_size);
void EHCI_bulk_set_packetsize(cyg_uint8 dev_idx, cyg_uint8 InNum, cyg_uint16 InSize, cyg_uint8 OutNum, cyg_uint16 OutSize);

cyg_uint32 EHCI_DataIn(cyg_uint32 dev_idx, cyg_uint32 tar_addr, cyg_uint32 len);
cyg_uint32 EHCI_DataOut(cyg_uint32 dev_idx, cyg_uint32 src_addr, cyg_uint32 len);

cyg_uint32 EHCI_WaitComplete(cyg_uint8 *src, cyg_uint32 addr, cyg_uint32 timeout);
cyg_uint32 EHCI_CommandTransport(cyg_uint32 type, cyg_uint32 dev_idx);

cyg_uint32 EHCI_BOT_Status(cyg_uint32 dev_idx);

void ehci_irq(void);

//--setup bus setting, such as pci..
bool ehci_bus_register(void);

//initialize HC
cyg_uint32 ehci_init (void);
cyg_uint32 ehci_start(cyg_uint32 port);


//--start HCD running
bool ehci_run(void);


//--reset HC
bool ehci_reset(cyg_uint32 port);


//--called before resuming the hub
bool ehci_resume(void);


//--cleanly make HCD stop
bool ehci_stop(void);


//--manage control/bulk transation
//parameter:	transfer the structure data to raw data for EHCI/OHCI
//return: 		actual transfer size
//						the sent-back message or data is returned in "data" (csw is in "status" param)
int	ehci_control_transfer(int address, cyg_uint8 *setup_data, cyg_uint8 *data, cyg_uint16 data_len);

int	ehci_bulk_transfer(int address, int dir_in, cyg_uint8 *setup_data , cyg_uint8 *data, cyg_uint16 data_len, cyg_uint8 *status);


//--Release the current
int ehci_release_endpoint(int endpoint);

cyg_uint8 ehci_check_portstatus(cyg_uint32 port);

//--root hub support
//Enable/disable/get status -->root hub
int ehci_rh_enable(void);

int ehci_rh_disable(void);

int ehci_rh_state(void);

#endif
