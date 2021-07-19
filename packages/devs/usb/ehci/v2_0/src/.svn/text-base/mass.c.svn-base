//******************************************
// mass.c  
// Fill the scsi commands for "Mass Storage"
// in usb
//******************************************
#include <stdio.h> 
#include <cyg/infra/cyg_type.h> // base types
#include <cyg/io/mass.h>
#include <cyg/infra/diag.h>
//#include <cyg/io/hc_init.h>
#include <cyg/io/usb.h>
#include <cyg/io/usberr.h>			//usb err / debug
//=========================================
//Locals
//=========================================
static cyg_uint32 CurTag=0;

//=========================================
//Function Prototype
//=========================================


//=========================================
//Main Functions
//=========================================
cyg_uint32 usb_bulk_setup(	cyg_uint8 command, int direction, struct CBW *cbw, \
									cyg_uint32 data_len, cyg_uint32 data_pos, \
									cyg_uint8 maxlun, cyg_uint8 curlun)
{
	//struct CBW      *cbw = NULL;
	//cyg_uint32 retval = 0;
	int i = 0;
	cyg_uint16 setup_len = 0;
	cyg_uint8 setup_data[CBW_CBWCB_SIZE]={0};
	
	//init CBWCB in cbw
	for(i=0; i<CBW_CBWCB_SIZE; i++)
	        cbw->CBWCB[i] = 0;	
		
	//Packing CBWCB(block data + scsi command(in CBWCB))
	setup_len = command_packing(command, setup_data, data_len, data_pos, maxlun+1);	
	
	//Packing CBW
	cbw->dCBWSignature = CBW_SIGNATURE;
	CurTag++;
	cbw->dCBWTag =	(CurTag&0x000000ff)<<24 | (CurTag&0x0000ff00)<<8 | 
					(CurTag&0x00ff0000)>>8  | (CurTag&0xff000000)>>24;

	cbw->dCBWDataTransferLength =	(data_len&0x000000ff)<<24 | (data_len&0x0000ff00)<<8 | 
									(data_len&0x00ff0000)>>8  | (data_len&0xff000000)>>24;	//block size //data_len;
							
	cbw->bmCBWFlags = (direction==READ)?0x80:0x00;	//data-out: 0; data-in: 1
	cbw->bCBWLUN = curlun; 							//which command block is being sent
	cbw->bCBWCBLength = setup_len;
	for(i=0; i<setup_len; i++)
	        cbw->CBWCB[i] = setup_data[i];

BD(("mass.c - usb_bulk_setup:\n"));
BD(("  cbw.dCBWSignature: %x ;cbw.dCBWTag: %x  \n", cbw->dCBWSignature, cbw->dCBWTag));
BD(("  cbw.dCBWDataTransferLength: %x ; cbw.bmCBWFlags: %x\n", cbw->dCBWDataTransferLength, cbw->bmCBWFlags));
BD(("  cbw.bCBWLUN: %x ; cbw.bCBWCBLength: %x\n", cbw->bCBWLUN, cbw->bCBWCBLength));
for(i=0; i<setup_len; i++)
{
	BD(("  CBWCB[%d]= %x  ", i, cbw->CBWCB[i]));
	if((i%3)==0)	BD(("\n"));
}


	return setup_len;
	
}

cyg_uint32 usb_bulk_status(cyg_uint8 command, cyg_uint8 *data, struct CSW csw)
{
//	cyg_uint16 length = 0;	
	cyg_uint8	ret=CSW_STATUS_FAILED;
	cyg_uint8	data_ret=CSW_STATUS_GOOD;
	cyg_uint32	tag=0;


if(data != NULL)
{
BD(("mass.c - usb_bulk_status: \n"));
BD((" data[0]: %x   data[1]: %x  \n", data[0], data[1]));
BD((" data[2]: %x   data[3]: %x  \n", data[2], data[3]));
}

		
	if(data!=NULL)
	{        
		BD(("usb_bulk_status: %x", command));
		switch(command)
		{
			case SC_INQUIRY:
				data_ret = CSW_STATUS_FAILED;
				if(data[0] == 0x00) 			//direct-access device			
					if(data[1] == 0x80)			//removable media				
						if(data[4] == 0x1F)		//the rest length is 1F
							data_ret = CSW_STATUS_GOOD;							
				break;
			case SC_READ_CAPACITY:	//Returned LBA and Block length in bytes -> no use yet!						
				break;
			case SC_READ_FORMAT_CAPACITIES:			//no use
				break;
			case SC_REQUEST_SENSE:
BD(("SC_REQUEST_SENSE: data[0]: %x   data[12]: %x  \n", data[0], data[12]));
				data_ret = CSW_STATUS_FAILED;
				if(data[0] == 0x70)					//Current error
				{
					//Additional Sense Code
					if(data[12] == 0x28)			//"Not ready to ready change, medium may have changed"
						data_ret = CSW_STATUS_GOOD;	
				}
				break;
			default:
				data_ret=CSW_STATUS_GOOD;
				break;
		}
	}
	

BD(("mass.c - usb_bulk_status: \n"));
BD(("  csw.dCSWSignature: %x ;csw.dCSWTag: %x\n", csw.dCSWSignature, csw.dCSWTag));
BD(("  csw.dCSWDataResidue: %x; csw.bCSWStatus: %x\n", csw.dCSWDataResidue, csw.bCSWStatus));


	if(csw.dCSWSignature == CSW_SIGNATURE)
	{		
BD(("usb_bulk_status: CSW_SIGNATURE\n"));
BD(("usb_bulk_status: CurTag: %x\n", CurTag));
BD(("usb_bulk_status: tag: %x\n", tag));
		tag =	(csw.dCSWTag&0xff000000)>>24 | (csw.dCSWTag&0x00ff0000)>>8 |
				(csw.dCSWTag&0x0000ff00)<<8 | (csw.dCSWTag&0x000000ff)<<24;
				
BD(("usb_bulk_status: change tag: %x\n", tag));
		
		if(tag == CurTag)
		{		
BD(("usb_bulk_status: CurTag; bCSWStatus: %x\n", csw.bCSWStatus));		
	  		if(csw.bCSWStatus == CSW_STATUS_GOOD)			
	  	   		ret = CSW_STATUS_GOOD;	//status 0 is command passed
	  	   	else
	  	   		diag_printf("usb_bulk_status: !!! NOT CSW_STATUS_GOOD\n");
	  	   		
	  	}
	}


BD(("mass.c - usb_bulk_status: ret: %x ; data_ret: %x\n", ret, data_ret));
	
	//RETURN error message handle
	if(CSW_STATUS_GOOD == ret)
	{
		if(CSW_STATUS_GOOD == data_ret)
			ret = CSW_STATUS_GOOD;
		else
			ret = CSW_DATA_ERROR; 
	}
	else
	{
		if(CSW_STATUS_GOOD == data_ret)
			ret = CSW_STATUS_FAILED;
		else
			ret = CSW_DATA_STATUS_FAILED;
	}
	
BD(("mass.c - usb_bulk_status: FINAL ret: %x\n", ret));	
	
	//usb_reset(port);
	return ret;
}

//command_packing
//pack the command and data in the cbwcb for the setup data.
//Return: the total length of "cbwcb"
cyg_uint8 command_packing(cyg_uint8 command, cyg_uint8 *data, cyg_uint32 blockNum, cyg_uint32 pos, cyg_uint8 lun)
{//BOT only, according to SCSI spec
	cyg_uint8 length = 0;
	int i=0;

	//Initialize
	for(i=0; i<CBW_CBWCB_SIZE; i++)
		data[i]=0;

	if((command == SC_TEST_UNIT_READY) || (command == SC_INQUIRY) || (command == SC_REQUEST_SENSE))
	{
		lun = 0;
		pos = 0;
		blockNum=0;
	}

	
	data[0]  = command; 					//operation code
	data[1]  = lun << 4;					//LUN/DPO/FUA/EBP/Reserved/RelAdr
	data[2]  = pos >> 24;					//2-5 LBA
	data[3]  = pos >> 16;			
	data[4]  = pos >> 8;
	data[5]  = pos;	
				
	//Command Length		
	switch(command)
	{
		case SC_TEST_UNIT_READY:
			length=6;
			break;
		case SC_WRITE_10:
		case SC_READ_10:			
			length=10;
			
			//the passed in blockNum is still byte-length.
			blockNum = blockNum/0x200;			
			data[6]  = 0x00;			//Reserved
			data[7]  = blockNum >>8;	//7-8 Transfer length
			data[8]  = blockNum;
			data[9]  = 0;				//Control			
			break;
			
		case SC_WRITE_12:
		case SC_READ_12:
			length = 12;

			//the passed in blockNum is still byte-length.
			blockNum = blockNum/0x200;			
			data[6]  = blockNum >> 24;	//6-9 Transfer length
			data[7]  = blockNum >> 16;	 
			data[8]  = blockNum >> 8; 
			data[9]  = blockNum;							
			data[10] = 0x00;			//Reserved
			data[11] = 0;				//Control			
			break;
		case SC_READ_FORMAT_CAPACITIES:
			break;
		case SC_READ_CAPACITY:
			length = 10;
			
			data[6]  = 0x00;			//6-8 Reserved
			data[7]  = 0x00;	 
			data[8]  = 0x00; 
			data[9]  = 0x00;							
			break;
		case SC_INQUIRY:
			length = 6;	
					
			data[4] = 0x24;				//Allocation length:36							
			break;
		case SC_REQUEST_SENSE:
			length = 18;
			
			 data[4] = 0x12;
			break;
		default://not supported !!
			return length;
			break;
	}	

	return length;
}

int usb_mass_bulk_transport(struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos)
{
	int result = 0;
	//int i=0, j=0;
	cyg_uint8 counter=0;
	cyg_uint8 maxlun=USBMSC_DEFAULT_LUN_NUMBER;
//	cyg_uint8 curlun=0;
	cyg_uint16 setup_len = 0;	
	cyg_uint8 ctrl_data[USB_MAX_CONTROL_DATA_SIZE] = {0};	
	struct CBW      cbw;
	struct CSW      csw;
	cyg_uint8	port=0;	

	port = get_usb_device();
	
	BD(("mass.c-usb_mass_bulk_transport: bMaxLUN: %x, bLUN: %x\n", udev[port].bMaxLUN,  udev[port].bLUN));
	
	//1. Get MAX LUN before write (Bulk-only)	
	if(0xFF == udev[port].bMaxLUN)					//FIRST-TIME,need to get the max lun
	{		
		usb_control_msg(	udev[port],				//USB_DEVICE
							hcd,					//hc_driver
							IN,						//Direction
		                	USB_RQ_BOT_GET_MAX_LUN,	//Request
		                	(cyg_uint16)0,			//value
		                	(cyg_uint16)0,			//index
		                	ctrl_data,				//data
		                	1);	 					//data_len
		                	
		//if LUN is NULL, MaxLun sets to default and continue
		if(NULL != ctrl_data)
			maxlun = *ctrl_data;
			
		udev[port].bMaxLUN = maxlun;
		
		BD(("mass.c - usb_mass_bulk_transport: Getting MAXLUN!! \n"));
		BD(("  maxlun(ctrl): %02x  maxlun: %02x, udev.bMaxLUN: %02x\n", *ctrl_data, maxlun, udev[port].bMaxLUN));
	}
	
	//2. Set bulk init setting before bulk transfer
	hcd.bulk_init(	udev[port].bDeviceAddress, 
					udev[port].bBulkInEndpoint, udev[port].wBulkInMaxPkt,
					udev[port].bBulkOutEndpoint, udev[port].wBulkOutMaxPkt);	

	//3. Check if the assigned current LUN is valid
	if(udev[port].bLUN > udev[port].bMaxLUN)
	{
		diag_printf("mass.c - current LUN is larger than MAX LUN\n");
		udev[port].bLUN = USBMSC_DEFAULT_LUN_NUMBER;
		//return -EINVAL;
	}
								
	//	Use Test_Unit_Ready to check if the current lun is valid
	counter = 3;				//20090119,retry counter
	do
	{	
		--counter;
		//Prepare command package
		setup_len = usb_bulk_setup(	SC_TEST_UNIT_READY, READ, &cbw, SC_TEST_UNIT_READY_LENGTH, data_pos, udev[port].bMaxLUN, udev[port].bLUN);

		//DEBUG
		BD(("mass.c - usb_mass_bulk_transport: SENDing Bulk - TEST UNIT READY Sending!! \n"));
		BD(("  setup_len: %x \n", setup_len));
		BD(("  cbw.dCBWSignature: %x ;cbw.dCBWTag: %x \n", cbw.dCBWSignature, cbw.dCBWTag));
		BD(("  cbw.dCBWDataTransferLength: %x ;cbw.bmCBWFlags: %x\n", cbw.dCBWDataTransferLength, cbw.bmCBWFlags));
		BD(("  cbw.bCBWLUN: %x ;cbw.bCBWCBLength: %x \n", cbw.bCBWLUN, cbw.bCBWCBLength));

		//Send TEST_UNIT_READY command																						
		usb_bulk_msg(	udev[port],					//USB_DEVICE
		                hcd, 
		                IN,  
		                (cyg_uint8 *)&cbw, 
		                setup_len, 
		                data, 
		                SC_TEST_UNIT_READY_LENGTH,	//0x00
		                (cyg_uint8 *)&csw);			//test unit ready only needs one byte
			                  		
		result = usb_bulk_status(0, data, csw);		//check status value
		
		if(result == CSW_STATUS_GOOD)
			break;									//test unit ready OK!!
		else
		{		    		
			//Send REQUEST SENSE to check what error condition happened!!
			setup_len = usb_bulk_setup(	SC_REQUEST_SENSE, READ, &cbw, SC_REQUEST_SENSE_LENGTH, data_pos, udev[port].bMaxLUN, udev[port].bLUN);
			usb_bulk_msg(	udev[port],				//USB_DEVICE
			                hcd, 
			                IN,  
			                (cyg_uint8 *)&cbw, 
			                setup_len, 
			                data, 
			                SC_REQUEST_SENSE_LENGTH,//0x12
			                (cyg_uint8 *)&csw);		//test unit ready only needs one byte		    		

			result = usb_bulk_status(SC_REQUEST_SENSE, data, csw);	//check data and status
			
			if(result == CSW_STATUS_GOOD)
			{
				++result;	//Request sense OK, do again test_unit_ready
				diag_printf("mass.c - usb_mass_bulk_transport: REQUEST SENSE OK, test unit again: %x\n", udev[port].bLUN);
			}			
		}   
	}while(result && counter);	
		
BD(("mass.c - usb_mass_bulk_transport: After TEST_UNIT_READY!!"));
BD(("  udev.bLUN: %02x, udev.bMaxLUN: %02x \n", udev[port].bLUN, udev[port].bMaxLUN));

	//3. INQUIRY to check peripheral device type and removable medium
	//counter = 3;	//retry counter
	//do
	//{
	//	setup_len = usb_bulk_setup(	SC_INQUIRY, direction, &cbw, SC_INQUIRY_LENGTH, data_pos, 0,0);
	//	usb_bulk_msg(	udev[port],		//USB_DEVICE
	//	                hcd, 
	//	                IN,  
	//	                (cyg_uint8 *)&cbw, 
	//	                setup_len, 
	//	                data, 
	//	                SC_INQUIRY_LENGTH,		//0x24
	//	                (cyg_uint8 *)&csw);
    //	
	//	result = usb_bulk_status(SC_INQUIRY, data, csw);
	//	
	//	if(CSW_STATUS_FAILED == result)
	//		diag_printf("mass.c - usb_mass_bulk_transport: ERROR!! INQUIRY:csw failed!!\n");
	//	else if(CSW_DATA_ERROR == result)
	//		diag_printf("mass.c - usb_mass_bulk_transport: ERROR!! INQUIRY:csw passed, data failed!!\n");
	//	else if(CSW_DATA_STATUS_FAILED == result)
	//		diag_printf("mass.c - usb_mass_bulk_transport: ERROR!! INQUIRY:csw, data all failed!!\n");
	//		
	//}while(result && counter);

	
	//4. Use Bulk Only to send command and data
	counter = 3;	//retry counter
	do
	{
		counter--;		
		setup_len = usb_bulk_setup(	(direction==READ)?SC_READ_10:SC_WRITE_10, \
									direction ,&cbw, data_len, data_pos, udev[port].bMaxLUN, udev[port].bLUN);
//diag_printf("mass.c R/W\n");										
		usb_bulk_msg(	udev[port],		//USB_DEVICE
		                hcd, 
		                (direction==READ)?IN:OUT, 
		                (cyg_uint8 *)&cbw, 
		                setup_len, 
		                data, 
		                data_len,
		                (cyg_uint8 *)&csw);
    	
		result = usb_bulk_status(0, data, csw);		//check status
		
		if(result)
		{
			diag_printf("mass.c - usb_mass_bulk_transport: csw not passed!!\n");			
			if(direction==READ)
				BD(("READ\n"));
			else
				BD(("WRITE\n"));
		}
	}while(result && counter);
		
		return result;
}

int usb_mass_CB_transport(struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos)
{
	int result=0;
	cyg_uint16 setup_len = 0;	
	cyg_uint8 *setup_data=NULL;	
	cyg_uint8	port=0;	

	port = get_usb_device();	
	//Packing CBWCB(block data + scsi command(in CBWCB))
	setup_len = command_packing((direction==READ)?SC_READ_10:SC_WRITE_10, setup_data, data_len, data_pos, 0);	
			
			//1. Send ADSC command in control.(CBI: control - command; bulk - data)
		result = usb_control_msg(	udev[port],					//USB_DEVICE
									hcd,					//hc_driver
									OUT,					//Direction
			           				ADSC_OUT_REQUEST_TYPE,	//Request
			           				0,						//value
			           				0,						//index
			           				setup_data,				//data
			           				setup_len);	 			//data_length
		
		//2. Send data only without command and status
		usb_bulk_msg(	udev[port],					//USB_DEVICE
						hcd, 
						(direction==READ)?IN:OUT, 
						NULL, 
						0, 
						data, 
						data_len,
						NULL);	
		
		return result;
}

int usb_mass_storage_transport(struct USB_DEVICE *udev, struct hc_driver hcd, int direction, cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos)
{
	int result=0;
	cyg_uint8	port=0;	

	port = get_usb_device();


BD(("mass.c - usb_mass_storage_transport 1: udev.bProtocol: %x\n", udev[port].bProtocol));

		
	if(PROTOCOL_BOT == udev[port].bProtocol)
		result = usb_mass_bulk_transport(udev, hcd, direction, data, data_len, data_pos);
	else if(PROTOCOL_CB == udev[port].bProtocol)
		result = usb_mass_CB_transport(udev, hcd, direction, data, data_len, data_pos);
	
	return result;
}
