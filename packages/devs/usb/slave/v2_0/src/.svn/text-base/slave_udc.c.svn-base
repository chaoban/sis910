#include <cyg/infra/cyg_type.h> // base types
#include <cyg/io/slave_udc.h>
#include <cyg/io/usberr.h>			//usb err / debug

int mpkt_size=64;
static int h277c=0, h2790=0;

static int BulkInAckWait(void);
static cyg_uint32 GetBulkOutLen(void);
static int WaitDMADone(void);

int slave_udc_start(void)
{
SD(("slave_udc_start - START\n"));
	#if	DP_HIGH==0
	iiow(0x90002774, 0);	//disable D+
	#endif
	
	h277c = iior(0x9000277C);
	SD(("Init ACK=%d\n", h277c));

	iiow(0x90001710, 0xffffffff);
	iiow(0x90001710, 0x0);
	iiow(0x90001740, 0x06000608);
	iiow(0x90001744, 0x06100618);
	
//n_c
	#if	NEW_CODE
	iiow(0x90002784,1);	//data toggle, chg to 0 for 2.0
	#if	1	//Dev_ack_cnt 由 0x278c 改為 0x4018 的 bit 31-26, 這六個bit與別人共用
	iiow(0x9000401C,iior(0x9000401C) | (1<<31));	//但是需要先將 401c的bit 31 設為1
	#else
	iiow(0x9000401C,iior(0x9000401C) & ~(1<<31));	//(下一個版本設0)
	#endif
	#endif
//n_c-end

	h277c = iior(0x9000277C);
	SD(("Init ACK=%d\n", h277c));

	iiow(0x90001760, 0x08000808);
	iiow(0x90001764, 0x08100818);

	h277c = iior(0x9000277C);
	SD(("Init ACK=%d\n", h277c));

	iiow(0x90001500,  0);
	iiow(0x90001504,  0);
	iiow(0x90001508,  0);

//	h277c = iior(0x9000277C);
//	diag_printf("Init ACK=%d\n", h277c);

	iiow(0x9000150c,  0x400000);
	iiow(0x90001510,  0x400000);
	iiow(0x90001514,  0x400000);
	iiow(0x90001518,  0x400000);
	iiow(0x9000151c,  0);

	//host ack counts
	h277c = iior(0x9000277C);
	SD(("Init ACK=%d\n", h277c));

 	//iiow(0x9000276C, 32);	//max pkt size
 	//iiow(0x90002724, __LINE__);		//debug
 	iiow(0x90002714, 0x00000003);				// enable IRQ

	#if	DP_HIGH==0
	iiow(0x90002774, 1);	//enable D+
	while(iior(0x90002774)==0)
		diag_printf ("2774=%x\n",iior(0x90001714)); 		
	#endif

//n_c
	#if	NEW_CODE
	SD(("ack=%d ",iior(0x90004018) >> 26));
	#endif
//n_c_end
SD(("slave_udc_start - End\n"));
	
	return true;	
}

//int slave_udc_ctrl_end(void)
//{
//	return true;
//}

// Receive host data and move to usb io
int slave_udc_bulkout(cyg_uint32 *data_len, cyg_uint8 *data)
{
	//mpkt_size
	int flush=1;
	cyg_uint32	i=0, len=0;
//	cyg_uint8 *	tBuf=NULL;
	cyg_uint32  wrt_ack_cnt=0;
	cyg_uint32  len_count=0;
	int err=0;

	
SD(("slave_udc_bulkout - START\n"));		

//n_c ????????????????????????
//	iiow(0x90004008,0x1000000);
//	iiow(0x90002734,0x000000);
//n_c-end

//n_c	
	#if	NEW_CODE
	wrt_ack_cnt = iior(0x90004018) >> 26;
	#else
	wrt_ack_cnt = iior(0x9000278c);
	#endif
//n_c-end

	if(*data_len == mpkt_size)
		gw_xfer(wt2, (cyg_uint32)data, (mpkt_size/32), 0x1, 0x4);  // program gateway, get 64 bytes
	else
		gw_xfer(wt2, (cyg_uint32)data, 0x1, 0x1, 0x4);   // program gateway, get 31 bytes

	iiow(0x90004010, ACK_EN);			// open gate for data in(reply ACK)
//diag_printf("4010=%08X ", iior(0x90004010));
	iiow(0x90004010, 0x00000000);			// open gate for data in(reply ACK)
	while(iior(0x90004010)) {
		diag_printf("@");
	}
//diag_printf("slave_udc_bulkout - 2760_2: %08x\n", iior(0x90002760));		

	len_count = GetBulkOutLen();

//n_c
#if	NEW_CODE
	i = iior(0x90004020) & ~(1<<31);
	iiow(0x90004020,i | (1<<31));	//clr bulk out len
	iiow(0x90004020,i);
#else
	iiow(0x90002794,1);	//clr bulk out len	
	iiow(0x90002794,0);
#endif
//n_c-end
	
SD(("slave_udc_bulkout - len: %d*** ", len));		

	//if(len_count != mpkt_size)	//bulk-out cmd  (<64)
	if(*data_len < 32)
	{
			flush = 1;	

		// if data amount is less than 32, then the data needs to be flush out in gateway
		if(flush) {
//diag_printf("-flush-");			
	//		if(((iior(0x90001784) >>21) & 0x1F)==0)
	//			return(SCSI_CMD_INVALID);
			if(((iior(0x90001784) >>21) & 0x1F)) {
				iiow(0x90001714, 0x00000082);	//flush data to DRAM
				for(i=0; i<100; i++) {
					if(iior(0x90001700) & 0x004)	//flush end
						break;
				}
				if(i==100)
					diag_printf("#d, flush data to gateway, timeout*** ", __LINE__);
				//iiow(0x90001714, 0x00000000);	//flush end
			}
		}				
	}
	else if(*data_len == mpkt_size)		//bulk-out data
	{
		if(len_count != mpkt_size)
		{
			diag_printf("UBOUT:len%d != 64*** ", len_count);
			//return 8;
			err = 8;
		}
		else
		{
			for(i=0; i<10; i++);	//==>wait
			err = WaitDMADone();
			if(!err)
			{
				diag_printf("WAITDMA fails*** ");
				diag_printf(" UBOUT:data_add:%x \n", data);
				for(i=0; i<64; i++)
				{
					diag_printf("[%d]%x ",i, data[i]);
					if((i%10) == 9)
						diag_printf("\n");
				}
				diag_printf("\n");
				//return 8;
				err = 8;
			}
			if(h2790 == iior(0x90002790)) {
				//diag_printf("2790(%d->%d),i=%d ", h2790, iior(0x90002790), i);
			}
			#if	NEW_CODE
			if(wrt_ack_cnt != (iior(0x90004018) >> 10) & 0x3F)
			{
				if(err != 1)
				 diag_printf("wrt_ack_cnt(%x->%x) ", wrt_ack_cnt, (iior(0x90004018) >> 10) & 0x3F);			
				wrt_ack_cnt = (iior(0x90004018) >> 10) & 0x3F;			
			}
			#else
			if(wrt_ack_cnt != iior(0x9000278c))
				wrt_ack_cnt = iior(0x9000278c);
			#endif
			else 
			{
				//iiow(0x90002724, 0x1234);		//debug
				if(h2790 != iior(0x90002790)) {
					diag_printf("2790(%d->%d) ", h2790, iior(0x90002790));
				}
				
				#if	0
				while(1);
				#else
				//diag_printf("(%d)",wrt_ack_cnt);
				//i--;  
				diag_printf("UBOUT: Wack Fail:%x*** ", wrt_ack_cnt);
				err = 8;//return 8;
				#endif
			}	
		}	
	}
	else
	{
		diag_printf("UBOUT:32 < len%d < 64*** ", len_count);
		err = 8;//return 8;
	}
	
	h2790 = iior(0x90002790);

	/* clear bulk out sts */
	iiow(0x90002768, 2);
	iiow(0x90002768, 0);

SD(("slave_udc_bulkout - data\n"));
		
//diag_printf("BOUT:%x ", data);
//if((len_count==64)&&(data == 0x40400000))
//{
//	diag_printf("\n");
//	for(i=0; i<len_count; i++)
//	{
//		diag_printf("[%02d]:%02x  ", i,data[i]);
//		if((i%10)==9)
//			diag_printf("\n");
//	}
//	diag_printf("END%%%%%% ");
//}
	if(8 == err)
		return err;
		
	if(len_count==-1 || len_count==0)
	{
		diag_printf("UBOUT:len: %d*** ", len_count);
		return 8;
	}
	else if(len_count==31)	//"USBC"		
	{
		if(*((cyg_uint32 *)data)==0x55534243)			//"USBC"
		{
			SD(("slave_udc_bulkout: usb tag OK\n"));
		}
		else
		{
			diag_printf("UBOUT:usb tag fails, len:%d, %x*** ", len_count, *((cyg_uint32 *)data));
			return 1;
		}
			
	}
	else if(len_count == 64)
	{
		SD(("data OK .. \n"));
//		diag_printf("Dok ");
	}
	else
	{
		diag_printf("UBOUT: len fails:%d*** ", len_count);
//		return 1;
	}

	*data_len = len_count;	
//n_c	
	#if	NEW_CODE
	//diag_printf("ack=%d ", (iior(0x90004018) >> 10) & 0x3F);
	#endif		
//n_c-end	
	SD(("slave_udc_bulkout-END: len=%d ", len));	
	return 0;				
	
}

int slave_udc_bulkin(cyg_uint32 *data_len, cyg_uint8 *data)
{
	cyg_uint32 len=0;
	cyg_uint32 count=0;
	int ack=0;
	int err=0;

SD(("slave_udc_bulkin - START\n"));

int i=0;		
// d for(i=0; i<*data_len; i++)
// d {
// d 	diag_printf("[%02d]:%02x  ", i,data[i]);
// d 	if((i%10)==9)
// d 		diag_printf("\n");
// d }
// d diag_printf("\n");

	h277c = iior(0x9000277C);			//host ack counts
	//iiow(0x90002734,64);				//n_c	
SD(("...%x\n", h277c));

//	iiow(0x90004008,0x1000000);					// use auto tx data mode (gw_irq)
	len = (*data_len > mpkt_size) ? mpkt_size : *data_len;
SD(("slave_udc_bulkin - len: %x\n", len));		

	count = len & 0x03;		
	if(count) {	//!= 4N
		if(*data_len == 13)
		{
			iiow(0x90002734,0x0C);							// if tx data less than 32 byte, 8 bytes
			iiow(0x90002738,0x0C);
		}
		else
		{
			iiow(0x90002734, (len + 3) & ~3);				// if tx data more than 32 byte 20->20+4 bytes (20+4)=36 byte
			iiow(0x90002738, (len + 3) & ~3);
		}
			
		if(count==1)
			iiow(0x9000275c,0xE);	//e:1110 ->1 byte more. ex: c: 1100(2 byre); 8:1000(3 byte)
		else if(count==2)
			iiow(0x9000275c,0xC);	
		else		
			iiow(0x9000275c,0x8);	
	}
	else {
		iiow(0x90002734, len-4);				// if tx data more than 32 byte 20->20+4 bytes (20+4)=36 byte
		iiow(0x9000275c,0x0);	//e:1110 ->1 byte more. ex: c: 1100(2 byre); 8:1000(3 byte)
	}

iiow(0x90004008,0x1000000);					// use auto tx data mode (gw_irq)
//diag_printf("data add: %x, data[2]: %x\n", data, data[1]);	
	count = len%32;
	if(count>0)
		count = (len/32)+1;
	else count = len/32;
	gw_xfer(rd2, (cyg_uint32)data, 0x1, count, 0x2);   // start get data
//	gw_xfer(rd2, (cyg_uint32)data, 0x1, (len+32-1)/32, 0x2);   // start get data	
//diag_printf("UBIN%x ", data);

	for(count=0; count<10; count++);	//==>wait
	err = WaitDMADone();
	if(!err)
	{		
		diag_printf("UBIN:DMA WAIT NG*** ");
		//return 0;
	}
	
	iiow(0x90004008,0x3000000);					// use auto tx data mode (gw_irq)
	
	ack = BulkInAckWait();
	if(!ack) {
		diag_printf("(A%d=%d)", __LINE__,iior(0x9000277C));
		diag_printf("NO ACK?*** ");
//					diag_printf("2710=%04X 2718=%04X ack=%d/%d ", \
//						iior(0x90002710),iior(0x90002718),\
//						iior(0x9000277C),iior(0x90002790));		
//		for(i=0; i<10; i++)		diag_printf("==");
		//while(1);
	}
		
SD(("\nslave_udc_bulkin - END\n"));	

	return ack;		
}



cyg_uint32 slave_udc_bout_getstatus(void)
{
	int ret;
	ret = iior(0x90002768) & 0x01;
	if(ret) {
		//iiow(0x90002768, 2);
		//iiow(0x90002768, 0);
		//while(iior(0x90002768) & 0x01) {
		//	diag_printf("?");
		//}
	}
	return (ret);	// detect host bulk out req
}

void slave_udc_bout_clrstatus(void)
{
	iiow(0x90002710,0x02);		// clr irq
	iiow(0x90002710,0x00);
}

cyg_uint32 slave_udc_bin_getstatus(void)
{
	cyg_uint32 ret;
	ret = iior(0x90002718) & 0x01;	//bulk_in req ?
	//diag_printf("\b\b\b\b\b\b\b\b\b 2718=%2d ", iior(0x90002718));
	return ret;
}

void slave_udc_bin_clrstatus(void)
{
	iiow(0x90002710,0x01);		// clr irq
	iiow(0x90002710,0x00);
} 

static int BulkInAckWait(void)
{
	int i=0,ack=0;
	
	
	//for(i=0; i<TIME_OUT*10; i++)
	for(i=0; i<200; i++)
	{
		//if the host ack counts change(increase)
		if(iior(0x9000277C) != h277c) {
			break;
		}		
	}
	//diag_printf("B%02X:", iior(0x90002778));
	if(iior(0x9000277C) != h277c) {
//		diag_printf("(A%d=%d_%d a=%d ", __LINE__,iior(0x9000277C), i, iior(0x90002790));
		h277c = iior(0x9000277C);
		ack = 1;
	}
	else {
//		if(debug)
//			iiow(0x90002724, 0x1234);		//debug
		diag_printf("#%d, no ack(%d/%d)??? ",__LINE__,iior(0x9000277C),iior(0x90002790));
	}
	#if	0
	iiow(0x90002778,0x04);	//CLR ACK
	iiow(0x90002778,0x00);	//CLR ACK
	#endif
	//diag_printf("A%02X ", iior(0x90002778));

	//#if	1
	//iiow(0x90002710,0x01);		// clr irq
	//iiow(0x90002710,0x00);
	//#endif
    slave_udc_bin_clrstatus();
    
	return ack;
}

static cyg_uint32 GetBulkOutLen(void)
{
	cyg_uint32 len=-1, try_time=100;
	//diag_printf("GetBulkOutLen ");
	while(try_time)
	{
		//if(iior(0x90002718) & 3)	//start of bulk_in or last byte of bulk_out
		if(len != iior(0x90002760)) {
			len = iior(0x90002760);
			//try_time--;
		}
		else		
			try_time--;
		if(iior(0x90002718) & 0x01)	//bulk_in req ?
		{
			diag_printf("*Bulk out ERR(%d)*", __LINE__);
			break;
		}
	}
	if(!try_time) {
		SD(("GetBulkOutLen:2718=%d 2768=%d ", iior(0x90002718),iior(0x90002768)));			
	}
//#if	0	
	if((len != 31) && (len != mpkt_size)) {	
		//iiow(0x90002724, 0x1234);		//debug
		diag_printf("len=%d 2718=%d 2760=%d 2710=%d t=%d ", len, iior(0x90002718), iior(0x90002760), iior(0x90002710),try_time);
		//dump((cyg_uint8 *)BULK_CMD_BUF, 31);
		diag_printf("*bulk out len err(%d)*, len=%d", __LINE__, len);
//			while(1);
	}	
//#endif	
//	iiow(0x90002710,0x02);		//bulk_out_last_byte_sts_clr
//	iiow(0x90002710,0x00);
	slave_udc_bout_clrstatus();
	
	return len;
}


static int WaitDMADone(void)
{
	int	j = 0, ret=1, i=0;
	while(!(iior(0x90001700) & ((1<<10) + (1<<2)))) {	//wrt or read done!
		//for(i=0; i<10; i++);	//->wait
		j++;
		if(j == 10000)
			break;
	}
	if(j==10000) {
		diag_printf("<!DMA NG!, 17a4=%05X:%03X>", (iior(0x900017A4) >> 19) & 0x1F, (iior(0x900017A4) >> 16) & 0x7 );
		ret = 0;
	}
	//diag_printf("<17a4=%05X:%03X>", (iior(0x900017A4) >> 19) & 0x1F, (iior(0x900017A4) >> 16) & 0x7 );	//read
	iiow(0x90001700,(1<<10)+(1<<2));
	return ret;
}
