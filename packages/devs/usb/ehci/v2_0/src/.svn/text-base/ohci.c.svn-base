//ohci.c
#include <cyg/infra/diag.h>
#include <cyg/io/ohci.h>
#include <cyg/io/usb.h>				//for debug mode
#include <cyg/io/usberr.h>			//usb err / debug


static cyg_uint32 OHCI_CheckTD(cyg_uint32 td_idx, cyg_uint32 wait, cyg_uint8 *src);
static void OHCI_SkipED(cyg_uint32 ed, cyg_uint32 skip);
static cyg_uint32 OHCI_BOT_Cmd(cyg_uint32 dev_idx);
static cyg_uint32 OHCI_CommandTransport(cyg_uint32 type, cyg_uint32 dev_idx);
static cyg_uint32 OHCI_DataOut(cyg_uint32 dev_idx, cyg_uint32 src_addr, cyg_uint32 len);
static cyg_uint32 OHCI_DataIn(cyg_uint32 dev_idx, cyg_uint32 tar_addr, cyg_uint32 len);
static cyg_uint32 OHCI_BOT_Status(cyg_uint32 dev_idx);
static cyg_uint32 OHCI_ADSC_Cmd(cyg_uint32 dev_idx);


void ohci_irq(void)
{
	int i=0;
	i++;
}

//--setup bus setting, such as pci..
bool ohci_bus_register(void)
{
	return 1;
}

//initialize HC
cyg_uint32 ohci_start(cyg_uint32 port)
{
	HD(("ohci: ohci_start\n"));
	
	port =0;
	
	cyg_uint32 wrt_addr, buf_addr, i;
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
	iiow(gw_lpc3,  dram_base_addr);
	iiow(gw_lpc4,  dram_base_addr);
	iiow(gw_lpc5,  dram_base_addr);
	iiow(gw_lpc6,  dram_base_addr);
	iiow(gw_lpc7,  0);
	iiow(gw_lpc8,  0);
	iiow(gw_lpc9,  0);
	iiow(gw_lpc10, 0);
	iiow(gw_lpc11, 0);
	iiow(gw_lpc12, 0);
	iiow(gw_lpc13, 0);
	iiow(gw_lpc14, 0);
	iiow(gw_lpc15, 0);

	iiow(0xC0000060,0x00000000);	//set configure flag
	
	//For real chip, change default clk66 to clk33
	iiow(0x90004014,iior(0x90004014) & ~(1<<31));	//clk=33

	iiow(0x90004000,0x00000800);	//set tn=1			//unnecessary
	//iiow(0x90002700,0x00a80001);	//set resetj=1,osc_sel=1,cmppwdb=1,sel_hc_dev=1		//0x00a50001
iiow(0x90002700,0x00a50001);	//set resetj=1,osc_sel=1,cmppwdb=1,sel_hc_dev=1
iiow(0x90002610,0x00800000);   //enable R16 ISR(PCI)

	iiow(0xbe010044,0x00002100);
	iiow(0xbe010010,0x10000000);
	iiow(0xbe010004,0x00000146);
	iiow(0xbe01000c,0x0000ff00);
	iiow(0xBE010348,0x00003209);
//	break_point(0x02);

//HcControlHeadED
//Dword 0 ¡X MPS F K S D EN FA
//Dword 1 TD Queue Tail Pointer (TailP) ¡X
//Dword 2 TD Queue Head Pointer (HeadP) 0 C H
//Dword 3 Next Endpoint Descriptor (NextED) ¡X
	wrt_addr = DRAM_ADDR(ED_CTRL_OFFSET_0);
	buf_addr = PCI_ADDR(TD_BUF);
//diag_printf("line=%d addr=%08X %08X\n",__LINE__,wrt_addr,buf_addr);
	//iiow(wrt_addr+0x00, 0x00080000);   //pkt size = 8, EN=0 FA=0
	iiow(wrt_addr+0x00, 0x00084000);   //pkt size = 8, SKIP, EN=0 FA=0
	iiow(wrt_addr+0x04, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x08, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x0C, PCI_ADDR(ED_CTRL_OFFSET_0+0x10));   //Next Endpoint Descriptor
//Endpoint Descriptor, exec later
//CTRL_1
	iiow(wrt_addr+0x10, 0x00104001);	//pkt size=16, SKIP, EN=0, FA=1
	iiow(wrt_addr+0x14, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x18, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x1C, 0);   //Next Endpoint Descriptor
	//iiow(wrt_addr+0x1C, PCI_ADDR(ED_CTRL_OFFSET_0+0x20));   //Next Endpoint Descriptor
//CTRL_2
	iiow(wrt_addr+0x20, 0x00104002);   //pkt size=16, SKIP, EN=0, FA=2
	iiow(wrt_addr+0x24, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x28, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x2C, 0x00000000);


//BULK
	wrt_addr = DRAM_ADDR(ED_BULKIN_OFFSET_1);
	buf_addr = PCI_ADDR(TD_BUF);
//BULK_IN
	iiow(wrt_addr+0x00, 0x00404001);   //pkt size=64, SKIP, EN=?, FA=1
	iiow(wrt_addr+0x04, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x08, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x0C, PCI_ADDR(ED_BULKIN_OFFSET_1+0x10));   //Next Endpoint Descriptor
	iiow(wrt_addr+0x10, 0x00404002);   //pkt size=64, SKIP, EN=?, FA=2
	iiow(wrt_addr+0x14, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x18, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x1C, PCI_ADDR(ED_BULKIN_OFFSET_1+0x20));   //Next Endpoint Descriptor
//BULK_OUT
	iiow(wrt_addr+0x20, 0x00404001);   //pkt size=64, SKIP, EN=?, FA=2
	iiow(wrt_addr+0x24, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x28, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x2C, PCI_ADDR(ED_BULKIN_OFFSET_1+0x30));   //Next Endpoint Descriptor
	iiow(wrt_addr+0x30, 0x00404002);   //pkt size=64, SKIP, EN=?, FA=2
	iiow(wrt_addr+0x34, buf_addr+0x30);   //TD Queue Tail Pointer
	iiow(wrt_addr+0x38, buf_addr+0x00);   //TD Queue Head Pointer
	iiow(wrt_addr+0x3C, 0x00000000);	//END
//diag_printf("line=%d\n",__LINE__);
/*
Host Controller Communications Area Format
Off  Size Name               R/W Description
==== ==== ================== == ==================================
0x00 128  HccaInterrruptTable R  These 32 Dwords are pointers to interrupt EDs.
0x80   2  HccaFrameNumber     W  Contains the current frame number.
								 This value is updated by the HC before it
								 begins processing the periodic lists for
								 the frame.
0x82   2  HccaPad1            W  When the HC updates HccaFrameNumber,
								 it sets this word to 0.
0x84   4  HccaDoneHead        W  When the HC reaches the end of a frame and its
								 deferred interrupt register is 0, it writes
								 the current value of its HcDoneHead to this
								 location and generates an interrupt if interrupts
								 are enabled. This location is not written by the
								 HC again until software clears the WD bit in the
								 HcInterruptStatus register. The LSb of this
								 entry is set to 1 to indicate whether an unmasked
								 HcInterruptStatus was set when HccaDoneHead was
								 written.
0x88 116  reserved           R/W Reserved for use by HC
*/

//unknown
	iiow(0x40413620,0x00000000);
	iiow(0x40413624,0x00000000);
	iiow(0x40413640,0x00000000);
//diag_printf("line=%d\n",__LINE__);
	iiow(0x40413644,0x00000000);
	iiow(0x40414620,0x01000008);
	iiow(0x40414624,0x00000241);
	iiow(0x40414630,0x00000000);
//diag_printf("line=%d\n",__LINE__);
	iiow(0x40414634,0x00000000);
	iiow(0x40414638,0x00000000);
	iiow(0x4041463c,0x00000000);
//diag_printf("line=%d\n",__LINE__);
//unknown
	//iiow(0x40415600,0x01000680);
	//iiow(0x40415604,0x00120000);

//HccaInterrruptTable
	wrt_addr = DRAM_ADDR(0);
	for(i=0; i<256; i+=4)
	{
//diag_printf("line=%d, index= %d\n",__LINE__, i);
		iiow(wrt_addr+i,0x00000000);
	}
//diag_printf("line=%d\n",__LINE__);
	//PCI configuration
	//USB Operation
	iiow(0xd0000008,0x00000006);   //HcCommandStatus
	iiow(0xd0000034,0x04001000);   //HcFmInterval
	iiow(0xd0000048,0x00001200);   //HcRhDescriptorA
	iiow(0xd0000050,0x00010000);   //HcRhStatus
	iiow(0xd0000004,0x0000009f);   //HcControl
	iiow(0xd0000010,0xc000007b);   //HcInterruptEnable
	iiow(0xd0000018,PCI_ADDR(0));	//HcHCCA, 256 bytes alignment
//diag_printf("line=%d\n",__LINE__);
	iiow(0xd0000020,PCI_ADDR(ED_CTRL_OFFSET_0));   //HcControlHeadED
	iiow(0xd0000024,PCI_ADDR(ED_CTRL_OFFSET_0));   //HcControlCurrentED
	iiow(0xd0000028,PCI_ADDR(ED_BULKIN_OFFSET_1));   //HcBulkHeadED
	iiow(0xd000002c,PCI_ADDR(ED_BULKIN_OFFSET_1));   //HcBulkCurrentED
	iiow(0xd0000034,0x04001000);   //HcFmInterval,max=1024bits

	diag_printf("line=%d\n",__LINE__);	//Real Chip has to wait before

	iiow(0xd0000040,0x00000258);   //HcPeriodicStart
	iiow(0xd0000054,0x00000002);   //HcRhPortStatus[1]
	iiow(0xd0000058,0x00000002);   //HcRhPortStatus[2]


	HD(("OHCI start FINISH!!\n"));

	return true;
}

//--start HCD running
bool ohci_run(void)
{
	return 1;
}

//--reset HC
bool ohci_reset(void)
{
	return 1;
}

//--called before resuming the hub
bool ohci_resume(void)
{
	return 1;
}

//--cleanly make HCD stop
bool ohci_stop(void)
{
	return 1;
}

//--manage control/bulk transation
//parameter:	transfer the structure data to raw data for EHCI/OHCI
//return: 		actual transfer size
//						the sent-back message or data is returned in "data" (csw is in "status" param)
int	ohci_control_transfer(int address, cyg_uint8 *setup_data, cyg_uint8 *data, cyg_uint16 data_len)
{
	cyg_uint32 wrt_addr, buf_addr, tmp;
	cyg_uint8 *ptr8;
//	cyg_uint8 *ptr8_print;
	cyg_uint8 i=0;

	if(address > 2)
	{
		diag_printf("ohci: ohci_control_transfer - dev_idx=%d > 2, ERROR!\n", address);
		return 0;
	}


//CD(("ohci.c: ohci_control_transfer : \n"));
//diag_printf("ohci.c: ohci_control_transfer : \n");
//for(i=0; i<8;i++)
//{
//	//CD(("setup %d= %x \n", i, setup_data[i]));
//	diag_printf("setup %d= %x", i, setup_data[i]);
//	//if((i%3)==0)	CD(("\n"));
//}
//diag_printf("\n");
//CD(("\n"));


//TD
	wrt_addr = DRAM_ADDR(TD_BUF);
	buf_addr = PCI_ADDR(TD_BUF+0x30);
	if(setup_data[0] & 0x80)	//IN
	{
		iiow(wrt_addr+0x00, 0xF2E7FFFF);	//SETUP
		iiow(wrt_addr+0x04, buf_addr+0);		//head of buf
		iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x10));
		iiow(wrt_addr+0x0C, buf_addr+8-1);	//tail of buf
		iiow(wrt_addr+0x10, 0xF3F7FFFF);	//IN
		iiow(wrt_addr+0x14, buf_addr+0x10);
		iiow(wrt_addr+0x18, PCI_ADDR(TD_BUF+0x20));
		iiow(wrt_addr+0x1C, buf_addr+0x10+data_len-1);
		iiow(wrt_addr+0x20, 0xF3EFFFFF);	//OUT
		iiow(wrt_addr+0x24, 0x00000000);	//zero length
		iiow(wrt_addr+0x28, PCI_ADDR(TD_BUF+0x30));
		iiow(wrt_addr+0x2C, 0x00000000);
    }
    else	//OUT
    {
		iiow(wrt_addr+0x00, 0xF2E7FFFF);   			//SETUP,DATA0
		iiow(wrt_addr+0x04, buf_addr);   				//Current Buffer Pointer (CBP)
		iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x20));   	//Next TD (NextTD)
		iiow(wrt_addr+0x0C, buf_addr+7);   				//Buffer End (BE)
		iiow(wrt_addr+0x20, 0xF3F7FFFF);		//IN,DATA1
		iiow(wrt_addr+0x24, 0x00000000);   			//Current Buffer Pointer --> ZERO length
	    iiow(wrt_addr+0x28, PCI_ADDR(TD_BUF+0x30));	//Next TD (NextTD)
		iiow(wrt_addr+0x2C, 0x00000000);   			//Buffer End (BE)
		//data=? (data_len bytes)
		wrt_addr = DRAM_ADDR(TD_BUF+0x40);
		//for(tmp=0; tmp<data_len; tmp+=4)
		//{
		//	iiow(wrt_addr+tmp,(data[tmp+3]<<24)+(data[tmp+2]<<16)+(data[tmp+1]<<8)+data[tmp+0]);
		//}
    }

//setup data
	wrt_addr = DRAM_ADDR(TD_BUF+0x30);
	iiow(wrt_addr+0x00,(setup_data[3]<<24)+(setup_data[2]<<16)+(setup_data[1]<<8)+setup_data[0]);
	iiow(wrt_addr+0x04,(setup_data[7]<<24)+(setup_data[6]<<16)+(setup_data[5]<<8)+setup_data[4]);
	tmp = iior(0xd0000008) | (1<<1);	//control list filled
	iiow(0xd0000008,tmp);

	i=0;
	while(i<100){i++;}
//diag_printf("  CTRL2: %08x\n", ((setup_data[3]<<24)+(setup_data[2]<<16)+(setup_data[1]<<8)+setup_data[0]));
//diag_printf("  CTRL2: %08x\n", ((setup_data[7]<<24)+(setup_data[6]<<16)+(setup_data[5]<<8)+setup_data[4]));

//ED
	wrt_addr = DRAM_ADDR(ED_CTRL_OFFSET_0+address*16);
	//iiow(wrt_addr+0x04, PCI_ADDR(TD_BUF+0x30));	//TD Queue Tail Pointer (TailP)
	iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF));			//TD Queue Head Pointer (HeadP)
	//iiow(wrt_addr+0x0C, 0x00000000);
	tmp = iior(wrt_addr+0x00) & ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);   //pkt size=16, SKIP, EN=0, FA=?

CD(("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12)));

	tmp = iior(0xD0000004) & ~(1<<4);	//control list disable
	iiow(0xD0000004,tmp);
	iiow(0xD0000024,PCI_ADDR(ED_CTRL_OFFSET_0+address*16));   //HcControlCurrentED
	tmp |= 1 << 4;	//control list enable
	iiow(0xD0000004,tmp);

CD(("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12)));

	OHCI_CheckTD(0,0x800,__FUNCTION__);
	OHCI_CheckTD(1,0x800,__FUNCTION__);
	OHCI_CheckTD(2,0x800,__FUNCTION__);
	OHCI_SkipED(address, 1);	//disable control ED

	if(setup_data[0] & 0x80)	//IN
	{
		ptr8 = (cyg_uint8 *)DRAM_ADDR(TD_BUF+0x40);
		for(i=0; i<data_len; i+=4)
		{
			*data++ = ptr8[3];
			*data++ = ptr8[2];
			*data++ = ptr8[1];
			*data++ = ptr8[0];
			ptr8 += 4;
		}
	}

//diag_printf("ohci: ohci_control_transfer - END\n");

	return	data_len;
}


int	ohci_bulk_transfer(int address, int dir_in, cyg_uint8 *setup_data , cyg_uint8 *data, cyg_uint16 data_len, cyg_uint8 *status)
{
	cyg_uint32 rw_addr, tmp, no;
	cyg_uint8 *ptr8;
	int ret;
//	int i=0;


CD(("ohci.c: ohci_bulk_transfer: START\n"));

	if(address > 2)
	{
//		printT("dev_idx=%d > 2, ERROR!\n", address);
		return 0;
	}

	if(setup_data != NULL)	//BOT CMD
	{
		rw_addr = DRAM_ADDR(BOT_OFFSET);
		for(tmp=0; tmp<31; tmp+=4)
		{
			iiow(rw_addr+tmp,(setup_data[tmp+3]<<24)+(setup_data[tmp+2]<<16)+(setup_data[tmp+1]<<8)+setup_data[tmp+0]);
		}
		OHCI_CommandTransport(BOT_CMD, address);
		//OHCI_DataIn(dev_idx, buf, num << 9);
		//diag_printf("line=%d\n", __LINE__);
		//OHCI_BOT_Status(address);
	}

	ret = data_len;
	while(data_len)
	{
		no = (data_len > 512) ? 512 : data_len;
		if(dir_in)
		{
			OHCI_DataIn(address, (cyg_uint32)data, no);
		}
		else
		{
			OHCI_DataOut(address, (cyg_uint32)data, no);
		}
		data += 512;
		data_len  -= no;
	}

	if(setup_data != NULL)	//BOT CMD
	{
		rw_addr = OHCI_BOT_Status(address);
		//rw_addr = OHCI_BOT_Status(address);
		ptr8 = (cyg_uint8 *)rw_addr;
		//for(tmp=0, tmp1=0; tmp<16; tmp++, *ptr8++)
		//{
		//	if(tmp<12 || tmp == 15)
		//	{
		//		status[tmp1] = *ptr8;//*ptr8++;
		//		diag_printf("ohci_bulk_transfer: status = %x\n", status[tmp1]);
		//		tmp1++;
		//	}
		//}
		for(tmp=0; tmp<13; tmp+=4)
		{
			*status++ = ptr8[3];
			*status++ = ptr8[2];
			*status++ = ptr8[1];
			*status++ = ptr8[0];
//BD(("ohci_bulk_transfer: status = %x, %x, %x, %x\n", ptr8[3], ptr8[2], ptr8[1], ptr8[0]));
			ptr8 += 4;
		}
	}
	return ret;
}

//--Release the current
int ohci_release_endpoint(int endpoint)
{
	return 1;
}

//--root hub support
//Enable/disable/get status -->root hub
int ohci_rh_enable(void)
{
	return 1;
}
int ohci_rh_disable(void)
{
	return 1;
}
int ohci_rh_state(void)
{
	return 1;
}

static cyg_uint32 OHCI_CheckTD(cyg_uint32 td_idx, cyg_uint32 wait, cyg_uint8 *src)
{
	cyg_uint32 read_addr, i, cc, ec;
	read_addr = DRAM_ADDR(TD_BUF+td_idx*0x10);
	if(wait==0) wait=CTRL_TIMEOUT;
	for(i=0; i<wait; i++)
	{
		if(iior(read_addr+0x04)==0)
			break;	//head of buf
//		for(ec=0; ec++; ec<0x80);
		//printT("%s #%d, i=%d\n", __FUNCTION__, __LINE__, i);
	}
	for(ec=0; ec<wait; ec++)
	{
		cc = iior(read_addr) >> 28;
		if(cc <= 13)	break;
		//printT("idx=%d %08X\n", td_idx, iior(read_addr));
		//if(ec==0x400) break;
		//for(i=0; i<0x400; i++);
	}

//printT("line=%d\n", __LINE__);
	i = iior(read_addr);
	cc = i >> 28;	//[31:28]=cc, [27:26]=ec
	ec = (i >> 26) & 0x03;
	if((src != NULL) && cc)
	//if((src != NULL) && i)
	{
		diag_printf("#%d %s: TD(%d)=%08X %08X %08X %08X cc=%d ec=%d\n", __LINE__,src, td_idx, \
			iior(read_addr), iior(read_addr+4),iior(read_addr+8),iior(read_addr+12),\
			cc, ec);
	}
//diag_printf	("line=%d\n", __LINE__);
	return cc;
}

static void OHCI_SkipED(cyg_uint32 ed, cyg_uint32 skip)
{
	cyg_uint32 wrt_addr, tmp;
	wrt_addr = DRAM_ADDR(ED_CTRL_OFFSET_0+ed*16);
//printT("line=%d ED=%08X %08X %08X %08X\n",__LINE__,iior(wrt_addr+0),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));
	tmp = iior(wrt_addr+0x00);
	if(skip)
		tmp |=  (1<<14);	// SKIP
	else
		tmp &= ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);
//printT("line=%d ED=%08X %08X %08X %08X\n",__LINE__,iior(wrt_addr+0),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));
}

static cyg_uint32 OHCI_CommandTransport(cyg_uint32 type, cyg_uint32 dev_idx)
{
	cyg_uint32 ret;
	if(type==ADSC_CMD)
		ret = OHCI_ADSC_Cmd(dev_idx);
	else
		ret = OHCI_BOT_Cmd(dev_idx);
	return ret;
}


//test OK!
static cyg_uint32 OHCI_BOT_Cmd(cyg_uint32 dev_idx)
{
	cyg_uint32 wrt_addr, buf_addr, tmp;
	cyg_uint32 i=0; 

	//OHCI_SkipED(ED_BULKOUT_1+dev_idx-1, 1);	//disable BULKOUT ED

//TD
	wrt_addr = DRAM_ADDR(TD_BUF);
	buf_addr = PCI_ADDR(BOT_OFFSET);
//printT("line=%d addr=%08X %08X\n",__LINE__,wrt_addr,buf_addr);
	iiow(wrt_addr+0x00, 0xF0EFFFFF);	//OUT, DATAx toggle automatic
    iiow(wrt_addr+0x04, buf_addr+0);		//head of buf
    iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x30));	//next TD
    iiow(wrt_addr+0x0C, buf_addr+31-1);	//tail of buf

//printT("line=%d, OHCI_BOT_Cmd wrt_addr+0x00=%08X\n",__LINE__,iior(wrt_addr+0x00));

	tmp = iior(0xd0000008) | (1<<2);	//bulk list filled
	iiow(0xd0000008,tmp);
		
	while(i<100){i++;}
//	diag_printf("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));
//printT("line=%d OHCI_BOT_Cmd\n",__LINE__);

//ED
	wrt_addr = DRAM_ADDR(ED_BULKOUT_OFFSET_1+(dev_idx-1)*16);
//printT("line=%d addr=%08X %08X\n",__LINE__,wrt_addr,ED_BULKOUT_OFFSET_1);
	//iiow(wrt_addr+0x04, PCI_ADDR(TD_BUF+0x30));	//TD Queue Tail Pointer (TailP)
	tmp = iior(wrt_addr+0x08) & 0x0F;
	iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF)+tmp);			//TD Queue Head Pointer (HeadP)
	//iiow(wrt_addr+0x0C, 0x00000000);
	tmp = iior(wrt_addr+0x00) & ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);
//diag_printf("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	tmp = iior(0xD0000004) & ~(1<<5);	//bulk list disable
	iiow(0xD0000004,tmp);
		//printT("line=%d [0xD000002C]=%08X\n", __LINE__,iior(0xD000002C));
	iiow(0xD000002C,PCI_ADDR(ED_BULKOUT_OFFSET_1+(dev_idx-1)*16));   //HcBulkCurrentED
//diag_printf("line=%d [0xD000002C]=%08X\n", __LINE__,iior(0xD000002C));
	tmp |= 1 << 5;	//bulk list enable
	iiow(0xD0000004,tmp);
	
//diag_printf("line=%d [0xD0000004]=%08X\n", __LINE__,iior(0xD0000004));

	OHCI_CheckTD(0,0x800,__FUNCTION__);
	OHCI_SkipED(ED_BULKOUT_1+dev_idx-1, 1);	//disable BULKOUT ED

	//wrt_addr = DRAM_ADDR(TD_BUF);
	//printT("line=%d, wrt_addr+0x00=%08X\n",__LINE__,iior(wrt_addr+0x00));
	//wrt_addr = DRAM_ADDR(ED_BULKOUT_OFFSET_1+(dev_idx-1)*16);
	//printT("line=%d, ED_BULKOUT+0x08=%08X\n",__LINE__,iior(wrt_addr+0x08));

	return 0;
}

static cyg_uint32 OHCI_DataIn(cyg_uint32 dev_idx, cyg_uint32 tar_addr, cyg_uint32 len)
{
	cyg_uint32 wrt_addr, buf_addr, tmp, i;
	cyg_uint8 *pSrc8, *pTar8;
	cyg_uint8 *pSrc16, *pTar16;
	cyg_uint32 *pSrc32, *pTar32;
	//OHCI_SkipED(ED_BULKIN_1+dev_idx-1, 1);	//disable BULKIN ED
//TD
	wrt_addr = DRAM_ADDR(TD_BUF);
	buf_addr = PCI_ADDR(TD_BUF+0x30);
//diag_printf("OHCI_DataIn: line=%d len=%d\n",__LINE__,len);
    //iiow(wrt_addr+0x00, 0xF3F7FFFF);	//IN, len bytes
    iiow(wrt_addr+0x00, 0xF0F7FFFF);	//IN, len bytes
    #if	HW_SWAP_EN
    iiow(wrt_addr+0x04, (buf_addr+0)&0x7FFFFFFF);		//head of buf
    iiow(wrt_addr+0x0C, ((buf_addr+0)&0x7FFFFFFF)+len-1);		//tail of buf
    #else
    iiow(wrt_addr+0x04, buf_addr+0);		//head of buf
    iiow(wrt_addr+0x0C, buf_addr+len-1);		//tail of buf
    #endif
    iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x30));	//next TD

	//wrt_addr = DRAM_ADDR(TD_BUF);
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	tmp = iior(0xd0000008) | (1<<2);	//bulk list filled
	iiow(0xd0000008,tmp);
	
	i=0;
	while(i<100){i++;}
//ED
	wrt_addr = DRAM_ADDR(ED_BULKIN_OFFSET_1+(dev_idx-1)*16);
	//iiow(wrt_addr+0x04, PCI_ADDR(TD_BUF+0x30));	//TD Queue Tail Pointer (TailP)
	tmp = iior(wrt_addr+0x08) & 0x0F;
	iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF)+tmp);			//TD Queue Head Pointer (HeadP)
	//iiow(wrt_addr+0x0C, 0x00000000);
	tmp = iior(wrt_addr+0x00) & ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	//wrt_addr = DRAM_ADDR(TD_BUF);
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	tmp = iior(0xD0000004) & ~(1<<5);	//bulk list disable
	iiow(0xD0000004,tmp);
		//printT("line=%d [0xD0000024]=%08X\n", __LINE__,iior(0xD000002C));
	iiow(0xD000002C,PCI_ADDR(ED_BULKIN_OFFSET_1+(dev_idx-1)*16));   //HcBulkCurrentED
		//printT("line=%d [0xD0000024]=%08X\n", __LINE__,iior(0xD000002C));
	tmp |= 1 << 5;	//bulk list enable
	iiow(0xD0000004,tmp);

	//wrt_addr = DRAM_ADDR(TD_BUF);
//diag_printf("#%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	OHCI_CheckTD(0,len*16,__FUNCTION__);

	//wrt_addr = DRAM_ADDR(TD_BUF);
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	OHCI_SkipED(ED_BULKIN_1+dev_idx-1, 1);	//disable BULKIN ED

	if((tar_addr & 0x1F)==0)	//32 bits alignment addr
	{
		pSrc32 = (cyg_uint32 *)DRAM_ADDR(TD_BUF+0x30);
//diag_printf("#%d addr=%08X\n", __LINE__,pSrc32); 	
	//diag_dump_buf_with_offset(pSrc32, 512, pSrc32);
		pTar32 = (cyg_uint32 *)tar_addr;
//diag_printf("OHCI_DataIn: line=%d, %x, %x\n",__LINE__, pSrc32, pTar32);
		for(i=0; i<len; i+=4)
		{
			*pTar32++ = *pSrc32++;
		}
	}
	else if((tar_addr & 0x0F)==0)	//16 bits alignment addr
	{
		pSrc16 = (cyg_uint8 *)DRAM_ADDR(TD_BUF+0x30);
		pTar16 = (cyg_uint8 *)tar_addr;
		for(i=0; i<len; i+=2)
		{
			*pTar16++ = *pSrc16++;
		}
	}
	else //8 bits alignment addr
	{
		pSrc8 = (cyg_uint8 *)DRAM_ADDR(TD_BUF+0x30);
		pTar8 = (cyg_uint8 *)tar_addr;
		for(i=0; i<len; i++)
		{
			*pTar8++ = *pSrc8++;
		}
	}
//	diag_dump_buf_with_offset(tar_addr, 512, tar_addr);
	//return DRAM_ADDR(TD_BUF+0x40);
	return 0;
}

static cyg_uint32 OHCI_DataOut(cyg_uint32 dev_idx, cyg_uint32 src_addr, cyg_uint32 len)
{
	cyg_uint32 wrt_addr, buf_addr, tmp, i;
	cyg_uint8 *pSrc8, *pTar8;
	cyg_uint8 *pSrc16, *pTar16;
	cyg_uint32 *pSrc32, *pTar32;
	//OHCI_SkipED(ED_BULKOUT_1+dev_idx-1, 1);	//disable BULKIN ED

	if((src_addr & 0x1F)==0)	//32 bits alignment addr
	{
		pTar32 = (cyg_uint32 *)DRAM_ADDR(TD_BUF+0x30);
///	diag_printf("#%d addr=%08X\n", __LINE__,pTar32); 	
		pSrc32 = (cyg_uint32 *)src_addr;
		for(i=0; i<len; i+=4)
		{
			*pTar32++ = *pSrc32++;
		}
	}
	else if((src_addr & 0x0F)==0)	//16 bits alignment addr
	{
		pTar16 = (cyg_uint8 *)DRAM_ADDR(TD_BUF+0x30);
		pSrc16 = (cyg_uint8 *)src_addr;
		for(i=0; i<len; i+=2)
		{
			*pTar16++ = *pSrc16++;
		}
	}
	else //8 bits alignment addr
	{
		pTar8 = (cyg_uint8 *)DRAM_ADDR(TD_BUF+0x30);
		pSrc8 = (cyg_uint8 *)src_addr;
		for(i=0; i<len; i++)
		{
			*pTar8++ = *pSrc8++;
		}
	}

//	diag_dump_buf_with_offset(src_addr, 512, src_addr);
		pTar32 = (cyg_uint32 *)DRAM_ADDR(TD_BUF+0x30);
///	diag_printf("#%d addr=%08X\n", __LINE__,pTar32); 	
//	diag_dump_buf_with_offset(pTar32, 512, pTar32);
		pTar32 = (cyg_uint32 *)DRAM_ADDR_C(TD_BUF+0x30);
///	diag_printf("#%d addr=%08X\n", __LINE__,pTar32); 	
//	diag_dump_buf_with_offset(pTar32, 512, pTar32);
//TD
	wrt_addr = DRAM_ADDR(TD_BUF);
	buf_addr = PCI_ADDR(TD_BUF+0x30);
//printT("line=%d dev_idx=%08X %08X\n",__LINE__,wrt_addr,buf_addr);
    //iiow(wrt_addr+0x00, 0xF3EFFFFF);	//OUT
    iiow(wrt_addr+0x00, 0xF0EFFFFF);	//OUT
    #if	HW_SWAP_EN
    iiow(wrt_addr+0x04, (buf_addr+0)&0x7FFFFFFF);		//head of buf
    #else
    iiow(wrt_addr+0x04, buf_addr+0);		//head of buf
    #endif
    iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x30));	//next TD

    #if	HW_SWAP_EN
    iiow(wrt_addr+0x0C, ((buf_addr+0)&0x7FFFFFFF)+len-1);		//tail of buf
	#else
	iiow(wrt_addr+0x0C, buf_addr+len-1);		//tail of buf
	#endif
	 
	tmp = iior(0xd0000008) | (1<<2);	//bulk list filled
	iiow(0xd0000008,tmp);

	i=0;
	while(i<100){i++;}
		
//ED
	wrt_addr = DRAM_ADDR(ED_BULKOUT_OFFSET_1+(dev_idx-1)*16);
	//iiow(wrt_addr+0x04, PCI_ADDR(TD_BUF+0x30));	//TD Queue Tail Pointer (TailP)
	tmp = iior(wrt_addr+0x08) & 0x0F;
	iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF)+tmp);			//TD Queue Head Pointer (HeadP)
	//iiow(wrt_addr+0x0C, 0x00000000);
	tmp = iior(wrt_addr+0x00) & ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	tmp = iior(0xD0000004) & ~(1<<5);	//bulk list disable
	iiow(0xD0000004,tmp);
		//printT("line=%d [0xD0000024]=%08X\n", __LINE__,iior(0xD000002C));
	iiow(0xD000002C,PCI_ADDR(ED_BULKOUT_OFFSET_1+(dev_idx-1)*16));   //HcBulkCurrentED
		//printT("line=%d [0xD0000024]=%08X\n", __LINE__,iior(0xD000002C));
	tmp |= 1 << 5;	//bulk list enable
	iiow(0xD0000004,tmp);

	OHCI_CheckTD(0,len*16,__FUNCTION__);
	OHCI_SkipED(ED_BULKOUT_1+dev_idx-1, 1);	//disable BULKOUT ED
	return 0;
}

//13 bytes,
static cyg_uint32 OHCI_BOT_Status(cyg_uint32 dev_idx)
{
	cyg_uint32 wrt_addr, buf_addr, tmp, i;
//	cyg_uint32 i;
	//OHCI_SkipED(ED_BULKIN_1+dev_idx-1, 1);	//disable BULKIN ED
	iiow(DRAM_ADDR(TD_BUF+0x30),0x00000000);
//TD
	wrt_addr = DRAM_ADDR(TD_BUF);
	buf_addr = PCI_ADDR(TD_BUF+0x30);
//diag_printf("line=%d dev_idx=%08X %08X\n",__LINE__,wrt_addr,buf_addr);
    iiow(wrt_addr+0x00, 0xF0F7FFFF);	//IN, DATAx toggle automatic
    iiow(wrt_addr+0x04, buf_addr+0);		//head of buf
    iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x30));	//next TD
    iiow(wrt_addr+0x0C, buf_addr+13-1);		//tail of buf

	tmp = iior(0xd0000008) | (1<<2);	//bulk list filled
	iiow(0xd0000008,tmp);

	i=0;
	while(i<100){i++;}

//ED
	wrt_addr = DRAM_ADDR(ED_BULKIN_OFFSET_1+(dev_idx-1)*16);
	//iiow(wrt_addr+0x04, PCI_ADDR(TD_BUF+0x30));	//TD Queue Tail Pointer (TailP)
	tmp = iior(wrt_addr+0x08) & 0x0F;
	iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF)+tmp);			//TD Queue Head Pointer (HeadP)
	//iiow(wrt_addr+0x0C, 0x00000000);
	tmp = iior(wrt_addr+0x00) & ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	tmp = iior(0xD0000004) & ~(1<<5);	//bulk list disable
	iiow(0xD0000004,tmp);
		//printT("line=%d [0xD000002C]=%08X\n", __LINE__,iior(0xD000002C));
	iiow(0xD000002C,PCI_ADDR(ED_BULKIN_OFFSET_1+(dev_idx-1)*16));   //HcBulkCurrentED
		//printT("line=%d [0xD000002C]=%08X\n", __LINE__,iior(0xD000002C));
	tmp |= 1 << 5;	//bulk list enable
	iiow(0xD0000004,tmp);

	OHCI_CheckTD(0,0x1000,__FUNCTION__);
	OHCI_SkipED(ED_BULKIN_1+dev_idx-1, 1);	//disable BULKIN ED

	buf_addr = DRAM_ADDR(TD_BUF+0x30);

BD(("OHCI_BOT_Status: CSW=%08X %08X %08X %08X\n", iior(buf_addr+0),iior(buf_addr+4),iior(buf_addr+8),iior(buf_addr+12)));

	//return DRAM_ADDR(TD_BUF+0x40);
	return buf_addr;
}

static cyg_uint32 OHCI_ADSC_Cmd(cyg_uint32 dev_idx)
{
	cyg_uint32 wrt_addr, buf_addr, tmp;
	//OHCI_SkipED(dev_idx, 1);	//disable control ED
//TD
	wrt_addr = DRAM_ADDR(TD_BUF);
	buf_addr = PCI_ADDR(TD_BUF+0x30);
//printT("line=%d dev_idx=%08X %08X\n",__LINE__,wrt_addr,buf_addr);
    iiow(wrt_addr+0x00, 0xF2E7FFFF);	//SETUP,DATA0
    iiow(wrt_addr+0x04, buf_addr+0);		//head of buf
    iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF+0x10));
    iiow(wrt_addr+0x0C, buf_addr+8-1);	//tail of buf

	buf_addr = PCI_ADDR(CBI_OFFSET);
    iiow(wrt_addr+0x10, 0xF3EFFFFF);	//OUT,DATA1
    iiow(wrt_addr+0x14, buf_addr);
    iiow(wrt_addr+0x18, PCI_ADDR(TD_BUF+0x20));
    iiow(wrt_addr+0x1C, buf_addr+12-1);

    iiow(wrt_addr+0x20, 0xF3F7FFFF);	//IN,DATA1
    iiow(wrt_addr+0x24, 0x00000000);	//zero length
    iiow(wrt_addr+0x28, PCI_ADDR(TD_BUF+0x30));
    iiow(wrt_addr+0x2C, 0x00000000);

	wrt_addr = DRAM_ADDR(TD_BUF+0x30);
//TD_BUF+0x30, setup data=21 00 00 00 -- 00 00 0C 00, AcceptDeviceSpecificCommand
	iiow(wrt_addr+0x00,0x00000021);
	iiow(wrt_addr+0x04,0x000C0000);

	tmp = iior(0xd0000008) | (1<<1);	//control list filled
	iiow(0xd0000008,tmp);

//ED
	wrt_addr = DRAM_ADDR(ED_CTRL_OFFSET_0+dev_idx*16);
	//iiow(wrt_addr+0x04, PCI_ADDR(TD_BUF+0x30));	//TD Queue Tail Pointer (TailP)
	iiow(wrt_addr+0x08, PCI_ADDR(TD_BUF));			//TD Queue Head Pointer (HeadP)
	//iiow(wrt_addr+0x0C, 0x00000000);
	tmp = iior(wrt_addr+0x00) & ~(1<<14);	//!SKIP
	iiow(wrt_addr+0x00, tmp);   //pkt size=16, SKIP, EN=0, FA=?
	//printT("line=%d ED=%08X %08X %08X %08X\n", __LINE__, iior(wrt_addr),iior(wrt_addr+4),iior(wrt_addr+8),iior(wrt_addr+12));

	tmp = iior(0xD0000004) & ~(1<<4);	//control list disable
	iiow(0xD0000004,tmp);
		//printT("line=%d [0xD0000024]=%08X\n", __LINE__,iior(0xD0000024));
	iiow(0xD0000024,PCI_ADDR(ED_CTRL_OFFSET_0+dev_idx*16));   //HcControlCurrentED
		//printT("line=%d [0xD0000024]=%08X\n", __LINE__,iior(0xD0000024));
	tmp |= 1 << 4;	//control list enable
	iiow(0xD0000004,tmp);

	OHCI_CheckTD(0,0,__FUNCTION__);
	OHCI_CheckTD(1,0,__FUNCTION__);
	OHCI_CheckTD(2,0,__FUNCTION__);
	OHCI_SkipED(dev_idx, 1);	//disable control ED
	//return DRAM_ADDR(TD_BUF+0x40);
	return 0;
}


void ohci_control_set_packetsize (cyg_uint8 dev_address, cyg_uint16 maxsize)
{
	cyg_uint32 wrt_addr, tmp;
	wrt_addr = DRAM_ADDR(ED_CTRL_OFFSET_0+dev_address*16);
	tmp  = iior(wrt_addr+0x00) & 0xF800FFFF;	//update max pkt size
	tmp |= maxsize << 16;
	iiow(wrt_addr+0x00, tmp);
}

void ohci_bulk_set_packetsize (	cyg_uint8 dev_address,
								cyg_uint8 bulkInEdNum, cyg_uint16 binmaxsize,
								cyg_uint8 bulkOutEdNum, cyg_uint16 boutmaxsize	)
{
	cyg_uint32 wrt_addr, tmp;

	BD(("hc_init.c - usb_bulk_set_packetsize !!\n"));
//BULK IN
	wrt_addr = DRAM_ADDR(ED_BULKIN_OFFSET_1+(dev_address-1)*16);
	tmp = 0x00004000 | (binmaxsize<<16) | (bulkInEdNum<<7) | dev_address;
	iiow(wrt_addr+0x00, tmp);   //pkt size=?, SKIP, EN=?, FA=?}
//BULK OUT
	wrt_addr = DRAM_ADDR(ED_BULKOUT_OFFSET_1+(dev_address-1)*16);
	tmp = 0x00004000 | (boutmaxsize<<16) | (bulkOutEdNum<<7) | dev_address;
	iiow(wrt_addr+0x00, tmp);   //pkt size=?, SKIP, EN=?, FA=?}
}
