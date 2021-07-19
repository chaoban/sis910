//fake ehci.c
#include <cyg/io/ehci.h>
#include <cyg/infra/diag.h>
#include <cyg/io/usb.h>				//for debug mode
#include <cyg/io/usberr.h>			//usb err / debug

void ehci_irq(void)
{
	int i=0;
	i++;
}

//--setup bus setting, such as pci..
bool ehci_bus_register(void)
{
	return true;
}

//initialize HC
cyg_uint32 ehci_start(cyg_uint32 port)
{

	cyg_uint32 data, ret=IS_LOW_SPEED;
	cyg_uint32 wrt_addr, buf_addr, i;

	HD(("ehci.c: ehci_start :%x!!\n", ret));

	iiow(gw_fifo_reset, 0xffffffff);
	iiow(gw_fifo_reset, 0x0);
	iiow(gw_wff_val0, 0x06000608);
	iiow(gw_wff_val1, 0x06100618);
	iiow(gw_wff_val2, 0x06200628);
	iiow(gw_wff_val3, 0x06300638);
	iiow(gw_wff_val4, 0x06400648);
	iiow(gw_wff_val5, 0x06500658);
	iiow(gw_wff_val6, 0x06600668);
	iiow(gw_wff_val7, 0x06700678);
	iiow(gw_rff_val0, 0x08000808);
	iiow(gw_rff_val1, 0x08100818);
	iiow(gw_rff_val2, 0x08200828);
	iiow(gw_rff_val3, 0x08300838);
	iiow(gw_rff_val4, 0x08400848);
	iiow(gw_rff_val5, 0x08500858);
	iiow(gw_lpc0,  0);
	iiow(gw_lpc1,  0);
	iiow(gw_lpc2,  0);
	iiow(gw_lpc3,  ehci_dram_base_addr);
	iiow(gw_lpc4,  ehci_dram_base_addr);
	iiow(gw_lpc5,  ehci_dram_base_addr);
	iiow(gw_lpc6,  ehci_dram_base_addr);
	iiow(gw_lpc7,  0);
	iiow(gw_lpc8,  0);
	iiow(gw_lpc9,  0);
	iiow(gw_lpc10, 0);
	iiow(gw_lpc11, 0);
	iiow(gw_lpc12, 0);
	iiow(gw_lpc13, 0);
	iiow(gw_lpc14, 0);
	iiow(gw_lpc15, 0);

	iiow(0x90002700,0x00000000);
	iiow(0x90004000,0x00000800);	//set tn=1
///	iiow(0x90002700,0x00a80001);	//set resetj=1,osc_sel=1,cmppwdb=1,sel_hc_dev=1
	iiow(0x90002700,0x00a50001);	//set resetj=1,osc_sel=1,cmppwdb=1,sel_hc_dev=1
	
	iiow(0x9000270c,0x00000000);	//clear OTG interrupt
	iiow(0x90002610,0x00800000);	//enable R16 ISR(PCI)

	//clear periodic list
	wrt_addr = EHCI_DRAM_ADDR(EHCI_PERIODIC_LIST);
	for(i=0x0; i<0xfff; i+=4)
	{
		iiow(wrt_addr+i,0xc0013fff);
	}
//HD(("clear periodic list done!\n"));

//Queue Head
	wrt_addr = DRAM_ADDR(QHD_CTRL_OFFSET_0);
	buf_addr = PCI_ADDR(QTD_BUF);
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_CTRL_OFFSET_1)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x7008E000);	//01. MPL=8, 1st Head, HS, Endpt=0, addr=0
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	wrt_addr = DRAM_ADDR(QHD_CTRL_OFFSET_1);
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_CTRL_OFFSET_2)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x70086001);	//01. MPL=8, HS, Endpt=0, addr=1
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	wrt_addr = DRAM_ADDR(QHD_CTRL_OFFSET_2);
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_BULKIN_OFFSET_1)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x70086002);	//01. MPL=8, HS, Endpt=0, addr=2
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	wrt_addr = DRAM_ADDR(QHD_BULKIN_OFFSET_1);
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_BULKIN_OFFSET_2)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x70082001);	//01. MPL=8, HS, Endpt=0, addr=1
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	wrt_addr = DRAM_ADDR(QHD_BULKIN_OFFSET_2);
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_BULKOUT_OFFSET_1)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x70082002);	//01. MPL=8, HS, Endpt=0, addr=2
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	wrt_addr = DRAM_ADDR(QHD_BULKOUT_OFFSET_1);
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_BULKOUT_OFFSET_2)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x70082001);	//01. MPL=8, HS, Endpt=0, addr=1, T=AUTO
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	wrt_addr = DRAM_ADDR(QHD_BULKOUT_OFFSET_2);
	//iiow(wrt_addr+0x00, PCI_ADDR(QHD_INT_OFFSET_1)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x00, PCI_ADDR(QHD_CTRL_OFFSET_0)+0x02);	//00. Queue Head Horizontal Link Pointer, QH
	iiow(wrt_addr+0x04, 0x70082002);	//01. MPL=8, HS, Endpt=0, addr=2
	iiow(wrt_addr+0x08, 0x40000000);	//02. Endpoint Capabilities
	iiow(wrt_addr+0x0C, PCI_ADDR(QHD_BUFFER_OFFSET));	//03. curr qTD pointer, set to 0
	iiow(wrt_addr+0x10, buf_addr+1);	//04. next qTD pointer, pointer to 1st qTD
	for(i=0x14; i<=0x2C; i+=4)
		iiow(wrt_addr+i, 0x00000000);

	//printT("#%d, 0xBE010348=%08X\n",__LINE__,iior(0xBE010348));
	//iiow(0xBE010348,0x00003209);
	iiow(0xBE010348,0x00003212);
	//printT("#%d, 0xBE010348=%08X\n",__LINE__,iior(0xBE010348));

	iiow(0xBE010304,0x000001FF);	//enable pci cmd reg
	iiow(0xBE01030C,0x000020FF);	//cache line size, Latency Timer
	iiow(0xBE010310,0x00000000);	//set base address to 0xC000_0000

	//enable edb interface
	iiow(0xBE010340,0x88000018);	//operational mode enable register

	//break_point(0x01);
//	diag_printf("line%d status = %08X-%08X\n", __LINE__, iior(0xC0000064),iior(0xC0000068));

///	iiow(0xC000002C,0x0FFF1FFD);	//set frame index and length
	iiow(0xC000002C,0x08881FFD);	//set frame index and length
	
    iiow(0xC0000034,PCI_ADDR(EHCI_PERIODIC_LIST));	//set periodic list base address
    iiow(0xC0000038,PCI_ADDR(QHD_CTRL_OFFSET_0));	//set async list base address

	iiow(0xC0000020,0x00010031);	//USB run and schedule enable
    iiow(0xC0000028,0x0000007f);	//interrupt enable
    iiow(0xC0000060,0x00000001);	//set configure flag

	iiow(0xC0000064+port*4,0x00000100);	//port Reset begin

	//while(1);	
#if	CHIRP_JK || TEST_PACKET	//reset start
	data = 0;
	while(1)	
	{
		for(i=0; i<0x00FFFFFF; i++);
		print("T_%04d  ",data++);
	}
#endif

	iiow(0xC0000064+port*4,0x00000000);	//port0 Reset End

	#if	1
	for(i=0; i<40000; i++) {	//NG:500~100, 150=LOW, 500~200=OK, 2000~1000=FULL
		data = (iior(0xC0000064+port*4) >> 10) & 0x03;
	}
	#else
	while(iior(0xC0000064+port*4) & 0x00000100);	//wait for port1 Reset complete
	#endif

	data = iior(0xC0000064+port*4);
	//diag_printf("line%d data=%08X status=[64]_%08X [68]_%08X\n", __LINE__, data,iior(0xC0000064),iior(0xC0000068));
	if(data & 0x01) {
		diag_printf("device is present on port!\n");
	}

	if(data & 0x02) {
		diag_printf("connect status change\n");
	}
	//diag_printf("line%d data=%08X status=[64]_%08X [68]_%08X\n", __LINE__, data,iior(0xC0000064),iior(0xC0000068));
	//diag_printf("%d 2700=%08X\n", __LINE__,iior(0x90002700));		
	
	data &= ~(1<<13);	//clear port owner
	#if	0
	if(data & (1<<11)) {
		printT("D+\n");
	#else
	i = (data >> 10) & 0x03;
	if(i != 0x01) {
		if(i==0x00)
			diag_printf("00_SE0\n");
		if(i==0x01)
			diag_printf("10_J-State\n");
		if(i==0x03)
			diag_printf("11_Undefined\n");
	#endif	
	
		data &= ~(1<<2);	//port disable
		data |= (1<<8);	//reset
		iiow(0xC0000064+port*4,data);
//	printT("line%d data=%08X status=[64]_%08X [68]_%08X\n", __LINE__, data,iior(0xC0000064),iior(0xC0000068));
		for(i=0; i<5000; i++);	//delay
		//for(i=0; i<500000; i++);	//delay
		data &= ~(1<<8);	//reset end
		iiow(0xC0000064+port*4,data);
		for(i=0; i<600; i++);	//delay
//	printT("line%d data=%08X status=[64]_%08X [68]_%08X\n", __LINE__, data,iior(0xC0000064),iior(0xC0000068));
		data = iior(0xC0000064+port*4);
//	printT("line%d data=%08X status=[64]_%08X [68]_%08X\n", __LINE__, data,iior(0xC0000064),iior(0xC0000068));
		if(data & 0x04) {
			diag_printf("HI-SPEED\n");
			ret = IS_HI_SPEED;
		}
		else {
			diag_printf("FULL SPEED\n");
			ret = IS_FULL_SPEED;
		}
	}
	else {
		if(data & 0x01)
		{
			diag_printf("LOW SPEED!\n");
			ret = IS_LOW_SPEED;
		}
		else
		{
			diag_printf("NO DEIVCE!\n");
			ret = NO_DEIVCE;
		}
	}		

	HD(("ehci.c: ehci_start END: ret = %x\n", ret));
	return ret;
}

//--start HCD running
bool ehci_run(void)
{
	return true;
}

//--reset HC
bool ehci_reset(void)
{
	return true;
}

//--called before resuming the hub
bool ehci_resume(void)
{
	return true;
}

//--cleanly make HCD stop
bool ehci_stop(void)
{
	return true;
}

//--manage control/bulk transation
//parameter:	transfer the structure data to raw data for EHCI/OHCI
//return: 		actual transfer size
//						the sent-back message or data is returned in "data" (csw is in "status" param)
int	ehci_control_transfer(int address, cyg_uint8 *setup_data, cyg_uint8 *data, cyg_uint16 data_len)
{
	cyg_uint32 wrt_addr, buf_addr, tmp, i;
	cyg_uint8 *ptr8;
//CD(("ehci_control_transfer START\n"));
	if(address > 2)
	{
//		printT("address=%d > 2, ERROR!\n", address);
		return 0;
	}
//for(i=0;i<8;i++)
//	CD(("echi.c %02x\n", setup_data[i]));
//TD
	wrt_addr = EHCI_DRAM_ADDR(QTD_BUF);
	buf_addr = PCI_ADDR(QTD_BUF+0x60);
	if(setup_data[0] & 0x80)	//IN
	{
		//printT("%s %d wrt_addr=%08X\n", __FUNCTION__, __LINE__, wrt_addr);
		iiow(wrt_addr+0x00, PCI_ADDR(QTD_BUF+0x20));	//Next qTD Pointer
		iiow(wrt_addr+0x04, PCI_ADDR(QTD_BUF+0x40) | 0x4E);	//Alternate Next qTD Pointer
		iiow(wrt_addr+0x08, 0x00080E80);	//8 bytes, C_Page=0, PID=SETUP, ACT
		iiow(wrt_addr+0x0C, buf_addr);	//buffer pointer (page 0)
		iiow(wrt_addr+0x10, 0x00000000);
		iiow(wrt_addr+0x14, 0x00000000);
		iiow(wrt_addr+0x18, 0x00000000);
		iiow(wrt_addr+0x1C, 0x00000000);

		iiow(wrt_addr+0x20, PCI_ADDR(QTD_BUF+0x40));	//Next qTD Pointer
		iiow(wrt_addr+0x24, PCI_ADDR(QTD_BUF+0x40) | 0x4E);	//Alternate Next qTD Pointer
		iiow(wrt_addr+0x28, 0x80000D80+(data_len<<16));	//data_len bytes, C_Page=0, PID=IN, ACT
		iiow(wrt_addr+0x2C, buf_addr+0x20);	//OUT buffer pointer (page 0)
		iiow(wrt_addr+0x30, 0x00000000);
		iiow(wrt_addr+0x34, 0x00000000);
		iiow(wrt_addr+0x38, 0x00000000);
		iiow(wrt_addr+0x3C, 0x00000000);

		iiow(wrt_addr+0x40, 0xDEAD0001);	//Next qTD Pointer
		iiow(wrt_addr+0x44, 0xDEAD0001 | 0x4E);	//Alternate Next qTD Pointer
		iiow(wrt_addr+0x48, 0x80000C80);	//0 bytes, C_Page=0, PID=OUT, ACT
		iiow(wrt_addr+0x4C, buf_addr+0x20);	//IN buffer pointer (page 0)
		iiow(wrt_addr+0x50, 0x00000000);
		iiow(wrt_addr+0x54, 0x00000000);
		iiow(wrt_addr+0x58, 0x00000000);
		iiow(wrt_addr+0x5C, 0x00000000);
    }
    else	//OUT
    {
		//printT("%s %d wrt_addr=%08X\n", __FUNCTION__, __LINE__, wrt_addr);
		iiow(wrt_addr+0x00, PCI_ADDR(QTD_BUF+0x40));	//Next qTD Pointer
		iiow(wrt_addr+0x04, PCI_ADDR(QTD_BUF+0x40) | 0x4E);	//Alternate Next qTD Pointer
		iiow(wrt_addr+0x08, 0x00080E80);	//8 bytes, C_Page=0, PID=SETUP, ACT
		iiow(wrt_addr+0x0C, buf_addr);	//buffer pointer (page 0)
		iiow(wrt_addr+0x10, 0x00000000);
		iiow(wrt_addr+0x14, 0x00000000);
		iiow(wrt_addr+0x18, 0x00000000);
		iiow(wrt_addr+0x1C, 0x00000000);

		iiow(wrt_addr+0x40, 0xDEAD0001);			//Next qTD Pointer
		iiow(wrt_addr+0x44, 0xDEAD0001 | 0x4E);		//Alternate Next qTD Pointer
		iiow(wrt_addr+0x48, 0x80000D80+(0<<16));	//0 bytes, C_Page=0, PID=IN, ACT
		iiow(wrt_addr+0x4C, buf_addr+0x20);			//IN buffer pointer (page 0)
		iiow(wrt_addr+0x50, 0x00000000);
		iiow(wrt_addr+0x54, 0x00000000);
		iiow(wrt_addr+0x58, 0x00000000);
		iiow(wrt_addr+0x5C, 0x00000000);
    }

//setup data
	wrt_addr = EHCI_DRAM_ADDR(QTD_BUF+0x60);
	iiow(wrt_addr+0x00,(setup_data[3]<<24)+(setup_data[2]<<16)+(setup_data[1]<<8)+setup_data[0]);
	iiow(wrt_addr+0x04,(setup_data[7]<<24)+(setup_data[6]<<16)+(setup_data[5]<<8)+setup_data[4]);

//qHD
	wrt_addr = EHCI_DRAM_ADDR(QHD_CTRL_OFFSET_0+address*QHD_LEN) + 0x10;
	iiow(wrt_addr+4, 0x4E);
	iiow(wrt_addr, PCI_ADDR(QTD_BUF));			//TD Queue Head Pointer (HeadP)

	tmp  = iior(0xC0000020);	//USB run and schedule enable
	tmp |= 1 << 5;	//Asynchronous Schedule enable
	iiow(0xC0000020, tmp);

	EHCI_WaitComplete(__FUNCTION__, EHCI_DRAM_ADDR(QTD_BUF)+0x48, EHCI_TIMEOUT);

	tmp  = iior(0xC0000020);
	tmp &= ~(1 << 5);	//Asynchronous Schedule disable
	iiow(0xC0000020, tmp);

	if(setup_data[0] & 0x80)	//IN
	{
		ptr8 = (cyg_uint8 *)EHCI_DRAM_ADDR(QTD_BUF+0x80);
		for(i=0; i<data_len; i+=4)
		{
			*data++ = ptr8[3];
			*data++ = ptr8[2];
			*data++ = ptr8[1];
			*data++ = ptr8[0];
			ptr8 += 4;
		}
	}
	return	data_len;
}


int	ehci_bulk_transfer(int address, int dir_in, cyg_uint8 *setup_data , cyg_uint8 *data, cyg_uint16 data_len, cyg_uint8 *status)
{
	cyg_uint32 rw_addr, tmp, no;
	cyg_uint8 *ptr8;
	int ret;
//	int i=0;

	if(address > 2)
	{
//		diag_printf("address=%d > 2, ERROR!\n", address);
		return 0;
	}

	if(setup_data != NULL)	//BOT CMD
	{
		rw_addr = EHCI_DRAM_ADDR(EHCI_BOT_OFFSET);
		for(tmp=0; tmp<31; tmp+=4)
		{
			iiow(rw_addr+tmp,(setup_data[tmp+3]<<24)+(setup_data[tmp+2]<<16)+(setup_data[tmp+1]<<8)+setup_data[tmp+0]);
		}
		EHCI_CommandTransport(BOT_CMD, address);
	}

	ret = data_len;
	while(data_len)
	{
		no = (data_len > 512) ? 512 : data_len;
		if(dir_in)
		{
			EHCI_DataIn(address, (cyg_uint32)data, no);
		}
		else
		{
			EHCI_DataOut(address, (cyg_uint32)data, no);
		}
		data += 512;
		data_len  -= no;
	}

	if(setup_data != NULL)	//BOT CMD
	{
		rw_addr = EHCI_BOT_Status(address);
		ptr8 = (cyg_uint8 *)rw_addr;
		for(tmp=0; tmp<13; tmp+=4)
		{
			*status++ = ptr8[3];
			*status++ = ptr8[2];
			*status++ = ptr8[1];
			*status++ = ptr8[0];
			ptr8 += 4;
		}
	}
//BD(("ehci_bulk_transfer end\n"));
	return ret;
}

//--Release the current
int ehci_release_endpoint(int endpoint)
{
	return 1;
}

//--root hub support
//Enable/disable/get status -->root hub
int ehci_rh_enable(void)
{
	return 1;
}
int ehci_rh_disable(void)
{
	return 1;
}
int ehci_rh_state(void)
{
	return 1;
}

void EHCI_control_set_packetsize(cyg_uint8 dev_idx, cyg_uint16 pkt_size)
{
	cyg_uint32 wrt_addr, tmp;
	wrt_addr = EHCI_DRAM_ADDR(QHD_CTRL_OFFSET_0+dev_idx*QHD_LEN) + 4;
	tmp  = iior(wrt_addr) & 0xF800FF80;	//update max pkt size & addr
	tmp |= (pkt_size << 16) | dev_idx;
	iiow(wrt_addr, tmp);
//CD(("  EHCI_control_set_packetsize\n"));
}

/*
Endpoint Characteristics:
11:8  Endpoint Number(Endpt).
   7  Inactivate on Next Transaction (I).
 6:0  Device Address.
*/
void EHCI_bulk_set_packetsize(cyg_uint8 dev_idx, cyg_uint8 InNum, cyg_uint16 InSize, cyg_uint8 OutNum, cyg_uint16 OutSize)
{
	cyg_uint32 wrt_addr, tmp;
//BULK IN
	wrt_addr = EHCI_DRAM_ADDR(QHD_BULKIN_OFFSET_1+(dev_idx-1)*QHD_LEN) + 4;
	tmp  = iior(wrt_addr) & 0xF800F080;
	tmp |= (InSize<<16) | (InNum<<8) | dev_idx;
	iiow(wrt_addr+0x00, tmp);
//BULK OUT
	wrt_addr = EHCI_DRAM_ADDR(QHD_BULKOUT_OFFSET_1+(dev_idx-1)*QHD_LEN) + 4;
	tmp  = iior(wrt_addr) & 0xF800F080;
	tmp |= (OutSize<<16) | (OutNum<<8) | dev_idx;
	iiow(wrt_addr+0x00, tmp);
//BD(("  EHCI_bulk_set_packetsize\n"));
}


/*
qhd dword_1, bit14
		dtc=0	dtc=1
qtd	dt=0	1		1
qtd	dt=1	1		2(data toggle err)
*/
//BOT only
cyg_uint32 EHCI_BOT_Cmd(cyg_uint32 dev_idx)
{
	cyg_uint32 wrt_addr, buf_addr, tmp;
//	cyg_uint32 i;
//QTD
	buf_addr = PCI_ADDR(EHCI_BOT_OFFSET);
	wrt_addr = EHCI_DRAM_ADDR(QTD_BUF);

	iiow(wrt_addr+0x00, 0xDEAD0001);	//Next qTD Pointer
	iiow(wrt_addr+0x04, 0xDEAD0001 | 0x4E);	//Alternate Next qTD Pointer
	iiow(wrt_addr+0x08, 0x001F0C80);	//31 bytes, C_Page=0, PID=OUT, ACT, T=AUTO
	iiow(wrt_addr+0x0C, buf_addr);	//buffer pointer (page 0)
	iiow(wrt_addr+0x10, 0x00000000);
	iiow(wrt_addr+0x14, 0x00000000);
	iiow(wrt_addr+0x18, 0x00000000);
	iiow(wrt_addr+0x1C, 0x00000000);

//QHD
	wrt_addr = EHCI_DRAM_ADDR(QHD_BULKOUT_OFFSET_1+(dev_idx-1)*QHD_LEN) + 0x10;
	iiow(wrt_addr+4, 0x4E);
	iiow(wrt_addr, PCI_ADDR(QTD_BUF));			//TD Queue Head Pointer (HeadP)

//#if	0
//diag_printf("-----QHD-----\n");
	//wrt_addr = EHCI_DRAM_ADDR(QHD_BULKOUT_OFFSET_1+(dev_idx-1)*QHD_LEN) + 0x00;
//	diag_printf("#%d (%08X)=%08X %08X %08X %08X\n", __LINE__,wrt_addr,iior(wrt_addr+0),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));
	//wrt_addr = EHCI_DRAM_ADDR(QHD_BULKOUT_OFFSET_1+(dev_idx-1)*QHD_LEN) + 0x10;
//	diag_printf("#%d (%08X)=%08X %08X %08X %08X\n", __LINE__,wrt_addr,iior(wrt_addr+0),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));
//#endif

	tmp  = iior(0xC0000020);	//USB run and schedule enable
	tmp |= 1 << 5;	//Asynchronous Schedule enable
	iiow(0xC0000020, tmp);

	EHCI_WaitComplete(__FUNCTION__, EHCI_DRAM_ADDR(QTD_BUF)+8, EHCI_TIMEOUT);

	tmp  = iior(0xC0000020);
	tmp &= ~(1 << 5);	//Asynchronous Schedule disable
	iiow(0xC0000020, tmp);

	return 0;
}

cyg_uint32 EHCI_CommandTransport(cyg_uint32 type, cyg_uint32 dev_idx)
{
//BD(("EHCI_CommandTransport start\n"));
	cyg_uint32 ret=0;
	if(type==BOT_CMD)
		ret = EHCI_BOT_Cmd(dev_idx);
	return ret;
}

cyg_uint32 EHCI_DataIn(cyg_uint32 dev_idx, cyg_uint32 tar_addr, cyg_uint32 len)
{
//D(("EHCI_DataIn start\n"));
	cyg_uint32 wrt_addr, buf_addr, tmp, i;
	cyg_uint8 *pSrc8, *pTar8;
	cyg_uint16 *pSrc16, *pTar16;
	cyg_uint32 *pSrc32, *pTar32;

//QTD
	buf_addr = PCI_ADDR(QTD_BUF+QHD_LEN*4);
	wrt_addr = EHCI_DRAM_ADDR(QTD_BUF);
	iiow(wrt_addr+0x00, 0xDEAD0001);	//Next qTD Pointer
	iiow(wrt_addr+0x04, 0xDEAD0001 | 0x4E);	//Alternate Next qTD Pointer
	iiow(wrt_addr+0x08, 0x80000D80+(len<<16));	//len bytes, C_Page=0, PID=IN, ACT
    #if	HW_SWAP_EN
	iiow(wrt_addr+0x0C, buf_addr&0x7FFFFFFF);	//buffer pointer (page 0)
	#else
	iiow(wrt_addr+0x0C, buf_addr);	//buffer pointer (page 0)
	#endif
	iiow(wrt_addr+0x10, 0x00000000);
	iiow(wrt_addr+0x14, 0x00000000);
	iiow(wrt_addr+0x18, 0x00000000);
	iiow(wrt_addr+0x1C, 0x00000000);

//QHD
	wrt_addr = EHCI_DRAM_ADDR(QHD_BULKIN_OFFSET_1+(dev_idx-1)*QHD_LEN) + 0x10;
	iiow(wrt_addr+4, 0x4E);
	iiow(wrt_addr, PCI_ADDR(QTD_BUF));			//TD Queue Head Pointer (HeadP)

	tmp  = iior(0xC0000020);	//USB run and schedule enable
	tmp |= 1 << 5;	//Asynchronous Schedule enable
	iiow(0xC0000020, tmp);

	EHCI_WaitComplete(__FUNCTION__, EHCI_DRAM_ADDR(QTD_BUF)+8, EHCI_TIMEOUT);

	tmp  = iior(0xC0000020);
	tmp &= ~(1 << 5);	//Asynchronous Schedule disable
	iiow(0xC0000020, tmp);

	if((tar_addr & 0x1F)==0)	//32 bits alignment addr
	{
		pSrc32 = (cyg_uint32 *)EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN*4);
		pTar32 = (cyg_uint32 *)tar_addr;
		for(i=0; i<len; i+=4)
		{
			*pTar32++ = *pSrc32++;
		}
	}
	else if((tar_addr & 0x0F)==0)	//16 bits alignment addr
	{
		pSrc16 = (cyg_uint16 *)EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN*4);
		pTar16 = (cyg_uint16 *)tar_addr;
		for(i=0; i<len; i+=2)
		{
			*pTar16++ = *pSrc16++;
		}
	}
	else //8 bits alignment addr
	{
		pSrc8 = (cyg_uint8 *)EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN*4);
		pTar8 = (cyg_uint8 *)tar_addr;
		for(i=0; i<len; i++)
		{
			*pTar8++ = *pSrc8++;
		}
	}

	return 0;
}

cyg_uint32 EHCI_DataOut(cyg_uint32 dev_idx, cyg_uint32 src_addr, cyg_uint32 len)
{
//D(("EHCI_DataOut start\n"));
	cyg_uint32 wrt_addr, buf_addr, tmp, i;
	cyg_uint8 *pSrc8, *pTar8;
	cyg_uint16 *pSrc16, *pTar16;
	cyg_uint32 *pSrc32, *pTar32;

	if((src_addr & 0x1F)==0)	//32 bits alignment addr
	{
		pTar32 = (cyg_uint32 *)EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN*4);
		pSrc32 = (cyg_uint32 *)src_addr;
		for(i=0; i<len; i+=4)
		{
			*pTar32++ = *pSrc32++;
		}
	}
	else if((src_addr & 0x0F)==0)	//16 bits alignment addr
	{
		pTar16 = (cyg_uint16 *)EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN*4);
		pSrc16 = (cyg_uint16 *)src_addr;
		for(i=0; i<len; i+=2)
		{
			*pTar16++ = *pSrc16++;
		}
	}
	else //8 bits alignment addr
	{
		pTar8 = (cyg_uint8 *)EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN*4);
		pSrc8 = (cyg_uint8 *)src_addr;
		for(i=0; i<len; i++)
		{
			*pTar8++ = *pSrc8++;
		}
	}

//QTD
	buf_addr = PCI_ADDR(QTD_BUF+QHD_LEN*4);
	wrt_addr = EHCI_DRAM_ADDR(QTD_BUF);
	iiow(wrt_addr+0x00, 0xDEAD0001);	//Next qTD Pointer
	iiow(wrt_addr+0x04, 0xDEAD0001 | 0x4E);	//Alternate Next qTD Pointer
	iiow(wrt_addr+0x08, 0x00000C80+(len<<16));	//13 bytes, C_Page=0, PID=OUT, ACT
    #if	HW_SWAP_EN
	iiow(wrt_addr+0x0C, buf_addr&0x7FFFFFFF);	//buffer pointer (page 0)
	#else
	iiow(wrt_addr+0x0C, buf_addr);	//buffer pointer (page 0)
	#endif
	iiow(wrt_addr+0x10, 0x00000000);
	iiow(wrt_addr+0x14, 0x00000000);
	iiow(wrt_addr+0x18, 0x00000000);
	iiow(wrt_addr+0x1C, 0x00000000);

//QHD
	wrt_addr = EHCI_DRAM_ADDR(QHD_BULKOUT_OFFSET_1+(dev_idx-1)*QHD_LEN) + 0x10;
	iiow(wrt_addr+4, 0x4E);
	iiow(wrt_addr, PCI_ADDR(QTD_BUF));			//TD Queue Head Pointer (HeadP)

	tmp  = iior(0xC0000020);	//USB run and schedule enable
	tmp |= 1 << 5;	//Asynchronous Schedule enable
	iiow(0xC0000020, tmp);

	EHCI_WaitComplete(__FUNCTION__, EHCI_DRAM_ADDR(QTD_BUF)+8, EHCI_TIMEOUT);

	tmp  = iior(0xC0000020);
	tmp &= ~(1 << 5);	//Asynchronous Schedule disable
	iiow(0xC0000020, tmp);

	return 0;
}


cyg_uint32 EHCI_BOT_Status(cyg_uint32 dev_idx)
{
	cyg_uint32 wrt_addr, buf_addr, tmp;
//	cyg_uint32 i;
//QTD
	buf_addr = PCI_ADDR(QTD_BUF+QHD_LEN);
	wrt_addr = EHCI_DRAM_ADDR(QTD_BUF);
	iiow(wrt_addr+0x00, 0xDEAD0001);	//Next qTD Pointer
	iiow(wrt_addr+0x04, 0xDEAD0001 | 0x4E);	//Alternate Next qTD Pointer
	iiow(wrt_addr+0x08, 0x800D0D80);	//13 bytes, C_Page=0, PID=IN, ACT
	iiow(wrt_addr+0x0C, buf_addr);	//buffer pointer (page 0)
	iiow(wrt_addr+0x10, 0x00000000);
	iiow(wrt_addr+0x14, 0x00000000);
	iiow(wrt_addr+0x18, 0x00000000);
	iiow(wrt_addr+0x1C, 0x00000000);

//QHD
	wrt_addr = EHCI_DRAM_ADDR(QHD_BULKIN_OFFSET_1+(dev_idx-1)*QHD_LEN) + 0x10;
	iiow(wrt_addr+4, 0x4E);
	iiow(wrt_addr, PCI_ADDR(QTD_BUF));			//TD Queue Head Pointer (HeadP)

	tmp  = iior(0xC0000020);	//USB run and schedule enable
	tmp |= 1 << 5;	//Asynchronous Schedule enable
	iiow(0xC0000020, tmp);

	EHCI_WaitComplete(__FUNCTION__, EHCI_DRAM_ADDR(QTD_BUF)+8, EHCI_TIMEOUT);

	tmp  = iior(0xC0000020);
	tmp &= ~(1 << 5);	//Asynchronous Schedule disable
	iiow(0xC0000020, tmp);

	buf_addr = EHCI_DRAM_ADDR(QTD_BUF+QHD_LEN);

BD(("EHCI_BOT_Status: CSW=%08X %08X %08X %08X\n", iior(buf_addr+0),iior(buf_addr+4),iior(buf_addr+8),iior(buf_addr+12)));

	return buf_addr;
}

cyg_uint32 EHCI_WaitComplete(cyg_uint8 *src, cyg_uint32 addr, cyg_uint32 timeout)
{
	cyg_uint32 i, j, ret=0;
	#if	0
	for(i=0; i<timeout; i++)
	{
		if((iior(addr) & 0x80)==0)
			break;
		for(j=0; j<80; j++);
	}
	#else
	for(i=0; i<0xFF; i++)
	{
		if((iior(addr) & 0x80)==0)
		{
			//printT("%s ready=%d\n", src, i);
			break;
		}
		for(j=0; j<80; j++);
	}
	#endif

	if((iior(addr) & 0x40))
	{
		//printT("Halted\n");
		ret = -1;
		//break_point(dec2bcd(__LINE__));
	}

	if(i==timeout)
	{
		//printT("%s time out\n", src);
	}
	return ret;
}
