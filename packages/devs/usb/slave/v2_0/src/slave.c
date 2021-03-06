//******************************************
// slave.c
// The slave core layer
//******************************************
//#include <pkgconf/hal.h>

#include <cyg/io/slave.h>
#include <cyg/io/otg.h>
#include <cyg/io/usb.h>
#include <cyg/io/slave_udc.h>
#include <cyg/io/usberr.h>			//usb err / debug
#include <cyg/io/mass.h>
/************************************************************
* Local variables
*************************************************************/
//extern cyg_uint8 bOTG;
//cyg_uint32 dSlaveBase;
static cyg_devio_table_t slave_table_handler;
//=== Descriptor definitions ===
//Device
//struct usb_device_descriptor device_d = {
//	bLength = 0x12;
//	bDescriptorType = 0x01;
//
//	bcdUSB = 0x0002;
//	bDeviceClass = 0x00;
//	bDeviceSubClass = 0x00;
//	bDeviceProtocol = 0x00;
//	bMaxPacketSize0 = 0x40;
//	idVendor = 0x00;
//	idProduct = 0x00;
//	bcdDevice = 0x00;
//	iManufacturer = 0x00;
//	iProduct = 0x00;
//	iSerialNumber = 0x00;
//	bNumConfigurations = 0x00;
//};
////Config
//struct usb_config_descriptor config_d = {
//	bLength = 0x09;
//	bDescriptorType = 0x02;
//
//	wTotalLength = 0x0020;
//	bNumInterfaces = 0x01;
//	bConfigurationValue = 0x01;
//	iConfiguration = 0x00;
//	bmAttributes = 0x80;
//	bMaxPower = 0xFA;
//};
////Interface
//struct usb_interface_descriptor interface_d = {
//	bLength = 0x09;
//	bDescriptorType = 0x04	;
//
//	bInterfaceNumber = 0x00;
//	bAlternateSetting = 0x00;
//	bNumEndpoints = 0x02;
//	bInterfaceClass = 0x08;
//	bInterfaceSubClass = 0x06;
//	bInterfaceProtocol = 0x50;
//	iInterface = 0x04;
//};
////Endpoint
//struct usb_endpoint_descriptor endpoint1_d = {
//	bLength = 0x07;
//	bDescriptorType = 0x05;
//
//	bEndpointAddress = 0x81;
//	bmAttributes = 0x02;
//	wMaxPacketSize = 0x0200;
//	wMaxPacketSize_L = 0x00;
//	wMaxPacketSize_H = 0x02;
//	bInterval = 0x00;
//};
//struct usb_endpoint_descriptor endpoint2_d = {
//	bLength = 0x07;
//	bDescriptorType = 0x05;
//
//	bEndpointAddress = 0x02;
//	bmAttributes = 0x02;
//	wMaxPacketSize = 0x0200;
//	wMaxPacketSize_L = 0x00;
//	wMaxPacketSize_H = 0x02;
//	bInterval = 0x00;
//};
////OTG
//struct usb_otg_descriptor otg_d = {
//	bLength = 0x00;
//	bDescriptorType = 0x00;
//
//	bmAttributes = 0x00;			/* support for HNP, SRP, etc */
//};

//Device Response
	static char vendor_id[] 	= "SIS CORP";
	static char product_id[] 	= "Mass Stor Device";
	static cyg_uint16 release	= 0x0910;	// Use controller chip type


/************************************************************
* Local Function Prototype
*************************************************************/
static void	slave_init(void);
static int 	slave_read(struct SLAVE_DEVICE *sdev, cyg_uint32 len);
static int 	slave_write(struct SLAVE_DEVICE *sdev, cyg_uint32 dataResidue_len);
static int handle_command(struct SLAVE_DEVICE *sdev, cyg_uint32 dataResidue_len);
static int handle_status(struct SLAVE_DEVICE *sdev, cyg_uint32 dataResidue_len);
static int parse_cbw(cyg_uint8 *data, struct CBW *cbw);

cyg_uint32 get_slave_space(cyg_uint8 type);
/************************************************************
* Local Variables
*************************************************************/
//static	struct	SLAVE_DEVICE slave_device[3]={{0},{0}};	//Max Slave Devices
//temp
cyg_uint8 port=0;

#define MAX_PKT_SIZE_11		64

//local buffer for data transport
cyg_uint8 data_buf[1024]={0}; //twice 512, in case
cyg_uint8 csw_buf[13]={0};
/************************************************************
* Main functions
*************************************************************/
void sis910_slave_init(void)
{
	D(("slave.c: sis910_slave_init\n"));

	//slave_init();
}

static Cyg_ErrNo  sis910_slave_lookup(struct cyg_devtab_entry **tab, struct cyg_devtab_entry *st, const char *name)
{
	D(("slave.c: sis910_slave_lookup\n"));

	slave_init();

	return ENOERR;
}

static Cyg_ErrNo sis910_slave_write(cyg_io_handle_t handle, const void* buffer, cyg_uint32* len, cyg_uint32 pos)
{
	cyg_uint32 totallen = *len;			//how many undone data
//	cyg_uint32 dataResidue_len = 0;
	cyg_uint32 hpost = pos;				//current lba
	cyg_uint32 count = 0;
	cyg_uint32 wait_c = 0;
	cyg_uint8  result = 0;
	cyg_uint32 pkt_size = 0;
	cyg_uint8  status_in=0;
	cyg_uint32	timeout = 0;
	cyg_uint32	ack_timeout = 0;

	D(("slave.c: sis910_slave_write: len: %x, pos: %x\n", totallen, hpost));

	if(totallen <64)
	{
diag_printf("R1: ");
		ack_timeout = 0;
		do
		{
			ack_timeout++;
			timeout=0;
			do
			{
				timeout++;
				for(wait_c=0;wait_c<100; wait_c++) ; // -->wait

				status_in  = slave_udc_bin_getstatus();

			}while( (status_in != USB_SLAVE_BULK_IN) && (timeout<30) );


			if(status_in == USB_SLAVE_BULK_IN)
			{
				result = slave_write((struct SLAVE_DEVICE *)buffer, totallen);
				if(result==6)
					diag_printf("Write1 ACK000 FAILs*** ");
				else
					ack_timeout=0;
			}
			else
			{
				diag_printf("Write1:STS FAILs:ack_timeout:%x*** ", ack_timeout);
				result = 6;
			}
		}while((result==6) && (ack_timeout<15));
	}
	else
	{
		pkt_size = MAX_PKT_SIZE_11;
diag_printf("Rs:");
		ack_timeout=0;
		for(count=0; count< totallen; count+=pkt_size)
		{
diag_printf("=>%x ", count);
				timeout=0;
				do
				{
					timeout++;
					for(wait_c=0;wait_c<100; wait_c++) ; // -->wait

					status_in  = slave_udc_bin_getstatus();

				}while( (status_in != USB_SLAVE_BULK_IN) && (timeout<30) );


				if(status_in == USB_SLAVE_BULK_IN)
				{
					result = slave_write((struct SLAVE_DEVICE *)buffer, count);
					if(result == 6)
						diag_printf("WriteN ACK 000 FAIL*** ");
					else
						ack_timeout=0;
				}
				else
				{
					diag_printf("WriteN STS FAILs count:%x *** ", count);
					result = 6;

					status_in  = slave_udc_bout_getstatus();
					if(status_in)
					{
						diag_printf("Write B-OUT ERR*** ");
						break;
					}

					//break;
				}

			if(result == 6)
			{
				count-=pkt_size;
				ack_timeout++;
			}
			if(ack_timeout>15)
			{
				diag_printf("Write retry > 15 FAIL*** ");
				break;
			}

		}//for-end
	}


	return result;
}

static Cyg_ErrNo sis910_slave_read(cyg_io_handle_t handle, void* buffer, cyg_uint32* len, cyg_uint32 pos)
{
	int result=1;
	cyg_uint32 totallen = *len;
//	cyg_uint32 hpost=pos;
	cyg_uint32 count=0;
	cyg_uint32 status_out =0;
	cyg_uint32 pkt_size = 0;
	cyg_uint32 timeout = 0;
	cyg_uint32 wait_c=0;
	cyg_uint32 ack_timeout=0;


	D(("slave.c: sis910_slave_read\n"));

	if(totallen<64)
	{
		ack_timeout = 0;
		status_out = 1;
diag_printf("W1: ");
		do
		{
			ack_timeout++;
			if(status_out)
			{
				result = slave_read((struct SLAVE_DEVICE *)buffer, totallen);
				if(result==8)
					diag_printf("Read1 ACK000 FAILs*** ");
			}
			else
			{
				diag_printf("Read1:STS FAILs:ack_timeout:%x*** ", ack_timeout);
				result = 8;
			}

			if(result)
			{
				timeout=0;
				do
				{
					timeout++;
					for(wait_c=0;wait_c<100; wait_c++) ; // -->wait

					status_out  = slave_udc_bout_getstatus();

				}while( (!status_out) && (timeout<30) );
			}

		}while((result==8) && (ack_timeout<15));

		if(result==8)
		{
			diag_printf("Read1 fail:res:%x actimeout:%x*** ", result,ack_timeout);
		}
	}
	else	//data: >=64
	{
		pkt_size = MAX_PKT_SIZE_11;	//64

		ack_timeout=0;


		((struct SLAVE_DEVICE *)buffer)->data_buf = (cyg_uint8 *)BULK_IO_BUF;

diag_printf("Ws: ");
		for(count=0; count< totallen; count+=pkt_size)
		{
diag_printf("=>%x ", count);
			timeout=0;
			do
			{
				timeout++;
				for(wait_c=0;wait_c<100; wait_c++) ; // -->wait

				status_out  = slave_udc_bout_getstatus();

			}while( (!status_out) && (timeout<30) );

			if(status_out)
			{
				result = slave_read((struct SLAVE_DEVICE *)buffer, count);
				if(result == 8)
					diag_printf("ReadN ACK 000 FAIL*** ");
				else
					ack_timeout=0;
			}
			else
			{
				diag_printf("ReadN STS FAILs count:%x *** ", count);
				result = 8;

				status_out  = slave_udc_bin_getstatus();
				if(status_out)
				{
					diag_printf("ReadN B-IN ERR*** ");
					break;
				}
				//break;
			}

			if(result == 8)
			{
				count-=pkt_size;
				ack_timeout++;
			}
			if(ack_timeout>15)
			{
				diag_printf("ReadN retry > 15 FAIL*** ");
				break;
			}

		}//for-end
	}

	D(("slave.c: sis910_slave_read: result: %x\n", result));

	return result;
}

static Cyg_ErrNo sis910_slave_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buffer, cyg_uint32 *len)
{
	cyg_uint32 status_out =0;
	cyg_uint32 status_in =0;

	switch (key) {
		case USB_SLAVE_GET_CONFIG_DATA_STATUS:
			status_out = slave_udc_bout_getstatus();
			status_in  = slave_udc_bin_getstatus();

			if(status_out)
				*(cyg_uint8 *)buffer = USB_SLAVE_BULK_OUT;
			else if(status_in)
				*(cyg_uint8 *)buffer = USB_SLAVE_BULK_IN;
			else
				*(cyg_uint8 *)buffer = 0;
diag_printf("STATUS: IN: %d OUT: %d... ", status_in, status_out);
			break;
		case USB_SLAVE_GET_CONFIG_IO_BUFFER:
diag_printf("Ret_config: %x \n", get_slave_space(IO_BUF_TYPE));
			*(cyg_uint32 *)buffer = get_slave_space(IO_BUF_TYPE);
diag_printf("Ret_config: %x  %x \n", buffer, *(cyg_uint32 *)buffer);
			break;

		default:
			*(cyg_uint8 *)buffer = 0xff;
			break;
	}

	*len = 0;
	return ENOERR;
}

static Cyg_ErrNo sis910_slave_set_config(cyg_io_handle_t handle, cyg_uint32 key,const void *buffer,cyg_uint32 *len)
{
	D(("slave.c: sis910_slave_set_config\n"));
	return ENOERR;
}

//================================================================================================
static void slave_init(void)
{
	//slave controller init
	D(("slave.c: slave_init\n"));
	slave_udc_start();

}

//slave_write: data from device to host(bulk in)
static int slave_write(struct SLAVE_DEVICE *sdev, cyg_uint32 dataResidue_len)
{
	int err=0;

	D(("slave.c: slave_write\n"));

	if(dataResidue_len == 13)
		err = handle_status(sdev, dataResidue_len);
	else
		err = handle_command(sdev, dataResidue_len);

	return err;
}

//slave_read: data from host to device(bulk out)
static int slave_read(struct SLAVE_DEVICE *sdev, cyg_uint32 len)
{
	int err=0;
	cyg_uint32	count=0;
	cyg_uint32	dlen=len;
	cyg_uint8 *cbw_buf=NULL;
	cyg_uint8 *data_buf=NULL;

//	if(!sdev[port].bCBWReceived)	//get cbw
	if(len == 31)	//command
	{
D(("slave.c: slave_read - get cbw\n"));
		//1. Get CBW
		err = slave_udc_bulkout(&dlen, (cyg_uint8 *)BULK_CMD_BUF);
		if(8 == err)
		{
			diag_printf("slave_read: bulkout err***\n");
			return 8;
		}

//D(("slave.c: slave_read - cbw_buf\n"));
//cbw_buf = (cyg_uint8 *)BULK_CMD_BUF;
//for(count=0; count<31; count++)
//{
//D(("[%02x]:%02x  ", count,*cbw_buf));
//	if((count%4)==3)
//		D(("\n"));
//	cbw_buf++;
//}

		if(dlen > CBW_SIZE)
		{
			diag_printf("CBW len not correct%d *** ", dlen);
			//return CBW_SIZE_ERROR;
		}

		//2. Handle CBW command
		err = parse_cbw(cbw_buf, &(sdev->cbw));
D(("slave.c: slave_read: sdev->dCBWSignature: %x\n", sdev->cbw.dCBWSignature));
		//3. init slave device data info
		sdev->bHost_Command = sdev->cbw.CBWCB[0];
		sdev->bHost_MaxLun	= sdev->cbw.CBWCB[1];
		sdev->bHost_CurLun  = sdev->cbw.bCBWLUN;

		//4. Check valid MaxLun
		if(MAX_LUN < sdev->bHost_MaxLun)
		{
			diag_printf("MLUN not correct*** ");
			return USB_SLAVE_MAXLUN_INCORRECT;
		}
		//5. Get the total blocks and LBA for READ/WRITE
		if( (sdev->cbw.CBWCB[0] == SC_READ_10) || (sdev->cbw.CBWCB[0] == SC_WRITE_10) )
		{
			sdev->dHost_LBA	= (sdev->cbw.CBWCB[2]<<24 | sdev->cbw.CBWCB[3]<<16 | sdev->cbw.CBWCB[4]<<8 | sdev->cbw.CBWCB[5] );
			sdev->dHost_blocks	= (sdev->cbw.CBWCB[7]<<8 | sdev->cbw.CBWCB[8]);
D(("slave.c: slave_read: dHost_LBA: %x, dHost_blocks: %x\n", sdev->dHost_LBA, sdev->dHost_blocks));
		}

		sdev->bCBWReceived = true;
	}
	else	//get data for read/write
	{
D(("slave.c: slave_read - get data\n"));

		if(sdev->cbw.CBWCB[0] == SC_WRITE_10)
		{
			//data_buf = (cyg_uint8 *)BULK_IO_BUF;
			data_buf = (sdev->data_buf)+len;
			dlen = 64;
			err = slave_udc_bulkout(&dlen, data_buf);
			if(err== 8)
			{
				diag_printf("slave_read:bulkout FAIL*** ");
				return 8;
			}

			sdev->data_len +=64;
		}
	}

	return 0;
}

//parsing received data to cbw
static int parse_cbw(cyg_uint8 *data, struct CBW *cbw)
{
	cyg_uint8 count=0;
	cyg_uint8 *pcbw=NULL;
	cyg_uint8 *temp=NULL;

D(("slave.c: parse_cbw\n"));

	temp = (cyg_uint8 *)BULK_CMD_BUF;
	cbw->dCBWSignature = *(cyg_uint32 *)temp;
D(("dCBWSignature: %x\n", cbw->dCBWSignature));
	temp+=4;
	//cbw->dCBWTag = (*(cyg_uint32 *)temp&0x000000ff)<<24 | (*(cyg_uint32 *)temp&0x0000ff00)<<8 |
	//				(*(cyg_uint32 *)temp&0x00ff0000)>>8  | (*(cyg_uint32 *)temp&0xff000000)>>24;
	cbw->dCBWTag = *(cyg_uint32 *)temp;
D(("dCBWTag: %x\n", cbw->dCBWTag));
	temp+=4;
	cbw->dCBWDataTransferLength = (*(cyg_uint32 *)temp&0x000000ff)<<24 | (*(cyg_uint32 *)temp&0x0000ff00)<<8 |
									(*(cyg_uint32 *)temp&0x00ff0000)>>8  | (*(cyg_uint32 *)temp&0xff000000)>>24;
D(("dCBWDataTransferLength: %x\n", cbw->dCBWDataTransferLength));
	temp+=4;
	cbw->bmCBWFlags = *temp;
D(("bmCBWFlags: %x\n", cbw->bmCBWFlags));
    temp++;
	cbw->bCBWLUN = *temp;
D(("bCBWLUN: %x\n", cbw->bCBWLUN));
	temp++;
	cbw->bCBWCBLength = *temp;
D(("bCBWCBLength: %x\n", cbw->bCBWCBLength));

	//checking if cbw is valid
	if(cbw->dCBWSignature != CBW_SIGNATURE)
	{
		//add in current lun checking ...
		diag_printf("cbw invalid!.. ");
		diag_printf("%x ", *(cyg_uint32 *)BULK_CMD_BUF);
		diag_printf("%x ", *((cyg_uint32 *)BULK_CMD_BUF+1));
		return 1;
	}

	//cbwcb length checking
	//...

	temp++;
	pcbw = temp;
	for(count=0; count<CBW_CBWCB_SIZE; count++)
	{
		cbw->CBWCB[count] = pcbw[count];
	}

	return 0;
}

//according command to fill in data and status, then send them out
static int handle_command(struct SLAVE_DEVICE *sdev, cyg_uint32 dataResidue_len)
{
	cyg_uint32 count1 = 0;
	cyg_uint32 length = 0;
	cyg_uint8 *data   = NULL;
	cyg_uint8 *status = NULL;
	int ack=0;


	D(("slave.c: handle_command - START: cmd: %x\n", sdev[port].bHost_Command));
	data   = (cyg_uint8 *)BULK_IO_BUF;
	status = (cyg_uint8 *)BULK_STS_BUF;

	*(cyg_uint32 *)status = CSW_SIGNATURE;
	status+=4;
	*(cyg_uint32 *)status = sdev[port].cbw.dCBWTag;
	D(("slave.c: handle_command 3: status: %x\n", *(cyg_uint32 *)status));
	status+=4;
	*(cyg_uint32 *)status = 0x00;
	status+=4;
	*(cyg_uint8 *)status = 0x00;


	switch(sdev[port].bHost_Command)
	{
		case SC_INQUIRY:
diag_printf("INQUIRY.. ");
			//sdev[port].csw.bCSWStatus = 0;

			//returned data
			data[0] = 0;		// Non-removable, direct-access device
			data[1] = 0x80;		// RMB
			data[2] = 0x02;		// ISO/ECMA/ANSI
			data[3] = 0x02;		// Response Data Format
			data[4] = 0x1F;		// Additional length
								// Reserve: 5~7
			sprintf(data + 8, "%-8s%-16s%04x", vendor_id, product_id, release);

			length = SC_INQUIRY_LENGTH;		//length = 36
			break;

		case SC_MODE_SENSE_6:
diag_printf("MSENSE6.. ");
			data[0] = 0x03;
			data[1] = 0x00;
			data[2] = 0x00;
			data[3] = 0x00;

			length = SC_MODE_SENSE_6_LENGTH;	//4
			break;

		case SC_TEST_UNIT_READY:
diag_printf("T_UNIT.. ");
			if(!sdev[port].bDevice_Connected)
				*status = 0x01;//sdev[port].csw.bCSWStatus = 1;

			length = SC_TEST_UNIT_READY_LENGTH;	//0
			break;

		case SC_READ_CAPACITY:
diag_printf("READ_CAP.. ");
			//returned data --
			// Max logical block from device
			data[0] = sdev[port].dDevice_TotalBlocks>>24;
			data[1] = sdev[port].dDevice_TotalBlocks>>16;
			data[2] = sdev[port].dDevice_TotalBlocks>>8;
			data[3] = sdev[port].dDevice_TotalBlocks;

			// Block length: 512
			data[4] = data[5] = data[7] = 0;
			data[6] = 0x02;

			length = SC_READ_CAPACITY_LENGTH; //8
			break;

		case SC_READ_FORMAT_CAPACITIES:
diag_printf("READ_F_CAP.. ");
			//returned data
			data[0] = data[1] = data[2] = 0;
			data[3] = 8;// Only the Current/Maximum Capacity Descriptor
			// Number of blocks
    		data[4] = sdev[port].dDevice_TotalBlocks>>24;
    		data[5] = sdev[port].dDevice_TotalBlocks>>16;
    		data[6] = sdev[port].dDevice_TotalBlocks>>8;
    		data[7] = sdev[port].dDevice_TotalBlocks;
			// Current capacity
			data[8] = 0x02;
			// Block length
			data[9] = data[11] = 0;
			data[10] = 0x02;

			length = SC_READ_FORMAT_CAPACITIES_LENGTH; //0x0c
			break;

		case SC_READ_10:
			data   = (sdev[port].data_buf)+dataResidue_len;

			length = 64;

			break;

		case SC_WRITE_10:
diag_printf("WRITE.. ");
			//read data from host and write to disk; only need to write csw to host
			length = dataResidue_len; //0

			break;
		default:
			length = 0;
			break;
	}

	//send data to host
	if(length)
	{
		ack = slave_udc_bulkin(&length, data);
		if(!ack)
		{
			diag_printf("handle_cmd: data in FAIL*** ");
			return 6;
		}
	}

	if( ( (sdev[port].bHost_Command != SC_READ_10) && (sdev[port].bHost_Command != SC_WRITE_10) ) )
	{
		length = 13;
		ack = handle_status(sdev, length);
		if(ack == 6)
		{
			diag_printf("handle_cmd: STS FAIL*** ");
			return ack;
		}
	}

	return 0;
}

static int handle_status(struct SLAVE_DEVICE *sdev, cyg_uint32 dataResidue_len)
{
//	int i=0;
	cyg_uint32 count1 = 0;
	cyg_uint32 count2 = 0;
	cyg_uint32 ack_count = 0;
	cyg_uint8 *status = NULL;
	cyg_uint32 bulkin_sts = 0;
	int ack=0;

	//Send csw to host
	if(CSW_SIZE != dataResidue_len)
		return 0xff;

	D(("slave.c: handle_status - START\n"));
	status = (cyg_uint8 *)BULK_STS_BUF;
	*(cyg_uint32 *)status = CSW_SIGNATURE;
	status+=4;
	*(cyg_uint32 *)status = sdev[port].cbw.dCBWTag;
	D(("slave.c: handle_status 3: status: %x\n", *(cyg_uint32 *)status));
	status+=4;
	*(cyg_uint32 *)status = 0x00;
	status+=4;
	*(cyg_uint8 *)status = 0x00;


	bulkin_sts = USB_SLAVE_BULK_IN;
	ack_count = 0;
	do
	{
		ack_count++;
		count1=0;
		do
		{
			count1++;
			for(count2=0;count2<100; count2++) ; // -->wait
			bulkin_sts = slave_udc_bin_getstatus();

			if(count1 > 100)		break;
		}while(bulkin_sts != USB_SLAVE_BULK_IN);


		if(bulkin_sts == USB_SLAVE_BULK_IN)
		{
			ack = slave_udc_bulkin(&dataResidue_len, (cyg_uint8 *)BULK_STS_BUF);
			sdev[port].bCBWReceived = false;
		}
	}while( (!ack) && (ack_count<20));

	if(!ack)
	{
		diag_printf("handle_STS:RETRY FAIL*** ");
		return 6;
	}

	return 0;
}

cyg_uint32 get_slave_space(cyg_uint8 type)
{
	cyg_uint32 buf=0;

	switch(type)
	{
		case IO_BUF_TYPE:
			buf = BULK_IO_BUF;
			break;
		case CMD_BUF_TYPE:
			buf = BULK_CMD_BUF;
			break;
		case BULK_BUF_BUF:
			buf = BULK_STS_BUF;
			break;
		default:
			break;
	}

	return buf;
}

#ifdef ALIGN_USE_FUNC_PTR
void* cyg_slave_buf_alloc(cyg_uint32 sizes)
{
//    FUNC_DBG_PTR;

    return malloc_align(sizes, 32); // 32 bytes alignment
}
#endif

// eCos Device Table Entries
static BLOCK_DEVIO_TABLE(slave_table_handler,
                        sis910_slave_write,
                        sis910_slave_read,
                        NULL,
                        sis910_slave_get_config,
                        sis910_slave_set_config);

#ifdef ALIGN_USE_FUNC_PTR
static BLOCK_DEVTAB_ENTRY(sis910_slave_devtab_entry,
                         "/dev/slave/",
                         0,
                         &slave_table_handler,
                         sis910_slave_init,
                         sis910_slave_lookup,
                         NULL,
                         cyg_slave_buf_alloc);
#else
static BLOCK_DEVTAB_ENTRY(sis910_slave_devtab_entry,
                         "/dev/slave/",
                         0,
                         &slave_table_handler,
                         sis910_slave_init,
                         sis910_slave_lookup,
                         NULL);
#endif

