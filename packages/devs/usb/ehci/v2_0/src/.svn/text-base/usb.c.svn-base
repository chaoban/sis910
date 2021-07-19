//******************************************
// usb.c  
// The USB core layer
// PS. only one device in current code
//******************************************
#include <stdio.h> 
#include <errno.h>	// Cyg_ErrNo
#include <cyg/infra/diag.h>
//#include <cyg/devs/usb/ehci/otg.h>
#include <cyg/io/usb.h>
#include <cyg/io/urb.h>
#include <cyg/io/mass.h>
#include <cyg/io/hcd.h>
#include <cyg/io/usberr.h>			//usb err / debug
#include <cyg/io/ohci.h>
/************************************************************
* Local variables
*************************************************************/
static cyg_uint8	dev_select_port=0;
static	struct	hc_driver hcd;
static	struct	USB_DEVICE usb_device[MaxDevice];
//static cyg_uint8	dev_select_hcd=OHCI;	//default
/************************************************************
* Local Function Prototype
*************************************************************/
static cyg_uint8 	init_device(int hub, int port);
//static void			get_usb_address(int port);
cyg_uint8 			get_usb_device(void);
//static void 		usb_disconnect(int hub, int port);
static cyg_uint8 	detect_device(int hub, int port, cyg_uint8 status);
static cyg_uint8 	get_hub_state(int hub, int port);
//static void 		check_port_change(int hub, int port);

//DEBUG
static void read_usb_device(struct USB_DEVICE usb_device);
static void read_usb_device_descriptor(struct usb_device_descriptor *devptr);
static void read_usb_configure_descriptor(struct usb_config_descriptor *conptr);
static void read_usb_interface_descriptor(struct usb_interface_descriptor *intptr);
static void read_usb_endpoint_descriptor(struct usb_endpoint_descriptor *endptr);
/************************************************************
* Main functions
*************************************************************/
void usb_host_init(void)
{
//	int	port=0;
	cyg_uint8 retval = 0;
	cyg_uint32 ret=0;

D(("usb.c: host init; active: %x\n",usb_device[dev_select_port].bActive));

	if(!usb_device[dev_select_port].bActive)	
	{

	D(("usb_host_init: hcd_init\n"));

  
		//init struct
		memset(&usb_device,0,sizeof(struct USB_DEVICE)*MaxDevice);      
		memset(&hcd,0,sizeof(struct hc_driver) );
			
		//hcd functions ready
		hcd = hcd_init(EHCI);
    	
		//EHCI init
		ret = hcd.start(0);
    	
	D(("usb_host_init: hcd.start=> ret= %x\n", ret));

	
	if(ret != IS_HI_SPEED)	//if EHCI fail
	{
		hcd = hcd_init(OHCI);	//assign OHCI driver
	
		//OHCI init
		hcd.start(0);
	}

		dev_select_port = 0;

	D(("usb_host_init: detect_device\n"));	
	//check_port_change(ROOTHUB, PORT); //not sure about roothub init
	//do enumeration only without roothub init
		retval = detect_device(0, dev_select_port, 0);	//hub, status --> not defined
 		if(retval)
 		{
			usb_device[dev_select_port].bActive = true;
		}
		else
		{
			usb_device[dev_select_port].bActive = false;
			usb_device[dev_select_port].bDeviceAddress = 0;
		}
			
#ifdef USB_DEBUG
	read_usb_device(usb_device[dev_select_port]);		
#endif
	
//	for(port=1; port<=MaxDevice; port++)
//	{
//		if(usb_device[dev_select_port].bActive)
//		{
//			dev_select_port = port;
//			break;
//		}
//	}
	
	////HC starts to run     
	//hcd.run();//ehci_run();	
	D(("usb_host_init: END!!\n"));
	}
}


Cyg_ErrNo usb_write(cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos)
{
	Cyg_ErrNo ret=ENOERR;
D(("usb.c: usb_write: data: %x; data_len: %x; data_pos: %x\n", data, data_len, data_pos));	
	diag_printf("usb write\n");

#if	HW_SWAP_EN
	//diag_printf("HW_SWAP_EN\n");
#else
	//reverse data -> wait for the hcd big/little endian change
	cyg_uint32 count, tmp1, tmp2=0;
	for(count=0; count<512; count+=4)	
	{
		tmp1 = data[count];
		tmp2 = data[count+1];
		data[count+0] = data[count+3];
		data[count+1] = data[count+2];
		data[count+2] = tmp2;
		data[count+3] = tmp1;

//diag_printf("swap-%d: %x, %x, %x, %x\n",count, data[count], data[count+1], data[count+2], data[count+3]);		
	}	
#endif	
	cyg_drv_dsr_lock();

	ret = usb_mass_storage_transport( usb_device, hcd, WRITE, data, data_len, data_pos);
	
	cyg_drv_dsr_unlock();
		
	return ret;
}

Cyg_ErrNo usb_read(cyg_uint8 *data, cyg_uint32 data_len, cyg_uint32 data_pos)
{
	Cyg_ErrNo ret=ENOERR;
	cyg_drv_dsr_lock();

//#ifdef USB_DEBUG		
D(("usb.c: usb_read: data: %x; data_len: %x; data_pos: %x\n", data, data_len, data_pos));
//	read_usb_device(usb_device[dev_select_port]);
//#endif	
	
	ret = usb_mass_storage_transport( usb_device, hcd, READ, data, data_len, data_pos);
	
//	diag_printf("usb.c -> data block print:swap before \n");
//	diag_dump_buf_with_offset(data, 512, data);			
	cyg_drv_dsr_unlock();

	diag_printf("usb read\n");
#if	HW_SWAP_EN
	//diag_printf("HW_SWAP_EN\n");
#else
	//reverse data -> wait for the hcd big/little endian change
	cyg_uint32 count, tmp1, tmp2=0;
	for(count=0; count<512; count+=4)	
	{
		tmp1 = data[count];
		tmp2 = data[count+1];
		data[count+0] = data[count+3];
		data[count+1] = data[count+2];
		data[count+2] = tmp2;
		data[count+3] = tmp1;

//diag_printf("swap-%d: %x, %x, %x, %x\n",count, data[count], data[count+1], data[count+2], data[count+3]);		
	}	
#endif	
//	diag_dump_buf_with_offset(data, 512, data);	
	
	return ret;
}

Cyg_ErrNo usb_select(int select)
{
	dev_select_port = select;
	
	return ENOERR;
}
                    
Cyg_ErrNo usb_get_config(void)
{
	return ENOERR;
}

Cyg_ErrNo usb_set_config(cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
	struct CBW	cbw;
	struct CSW  csw;
	struct usb_ctrlrequest dr;
	cyg_uint8 *setup_data = NULL;
	cyg_uint8 *temp_buf = NULL;
	int i=0;
//	int port = 1;
	struct usb_device_descriptor *devptr = NULL;
	struct usb_config_descriptor *conptr = NULL;	
	struct usb_interface_descriptor *intptr = NULL;
	struct usb_endpoint_descriptor *endptr = NULL;
	cyg_uint8 *ptr = NULL;
	cyg_uint8 maxlun, curlun=0;
	cyg_uint16 setup_len, tmp = 0;
	int result = 0;
	cyg_uint8 *clun = NULL;
		
	D(("usb_set_config start\n"));
			
	switch (key)
	{
		//case 0~7-> for test code
		case 0:
			//init struct
			memset(&usb_device,0,sizeof(struct USB_DEVICE)*MaxDevice);      
			memset(&hcd,0,sizeof(struct hc_driver) );			

			usb_device[dev_select_port].bHubDeviceNumber = 0;
			usb_device[dev_select_port].bHubPortNumber = 1;
			usb_device[dev_select_port].wEndp0MaxPacket = 8;
			usb_device[dev_select_port].bEndpointSpeed = 0 & USB_PORT_STAT_DEV_SPEED_MASK;
			usb_device[dev_select_port].bMaxLUN = 0xFF;
			usb_device[dev_select_port].bDeviceAddress = 0;
	    	
			hcd = hcd_init(EHCI);	//hcd functions ready        				       
			result = hcd.start(0);	//hcd init ready
			diag_printf("TEST ITEM: INIT HC READY: result= %d\n", result);
			break;
		case 1:	//from hcd - control command(--device descriptor--)
			usb_device[dev_select_port].bHubDeviceNumber = 0;
			usb_device[dev_select_port].bHubPortNumber = 1;
			usb_device[dev_select_port].wEndp0MaxPacket = 8;
			usb_device[dev_select_port].bEndpointSpeed = 0 & USB_PORT_STAT_DEV_SPEED_MASK;
			usb_device[dev_select_port].bMaxLUN = 0xFF;
			usb_device[dev_select_port].bDeviceAddress = 0;		
						        
			dr.wRequest = USB_RQ_GET_DESCRIPTOR; //(requestType + request)
			dr.wValue 	= USB_DT_DEVICE;	
			dr.wIndex 	= 0;
			dr.wLength 	= 0x08 << 8;	//length should be the same as data length!!
			setup_data 	= (cyg_uint8 *) &dr; 
			      	       	
			hcd.control_transfer(	0,					//address: set-address has been done!!
									setup_data,			//setup data
									(cyg_uint8 *) buf, 	//buffer from FS
									0x08); 				//data length
									
			devptr = (struct usb_device_descriptor *) buf;
			usb_device[dev_select_port].wEndp0MaxPacket = devptr->bMaxPacketSize0;	
			usb_device[dev_select_port].bNumConfigs = devptr->bNumConfigurations;
			usb_device[dev_select_port].bConfigNum = 0;	
												
			temp_buf = (cyg_uint8 *) buf;
			diag_printf("TEST ITEM: GET DEVICE DESCRIPTOR\n");
			for(i=0; i<8; i++)
			{
				diag_printf("  returned data[%d]: %02x  ",i, temp_buf[i]);
				if((i%3)==0)	diag_printf("\n");
			}
			diag_printf("\n");
			
			//set packet size for control init
			hcd.ctrl_init(usb_device[dev_select_port].bDeviceAddress, usb_device[dev_select_port].wEndp0MaxPacket);		
        	break;
        	
        case 2:	//from urb - control command(--configuration descriptor--)
			usb_control_msg(	usb_device[dev_select_port],
								hcd,
								IN,
								USB_RQ_GET_DESCRIPTOR,
								USB_DT_CONFIG,
								0,
								(cyg_uint8 *) buf,
								0x20);  
								  
			ptr = (cyg_uint8 *)buf;
			conptr = (struct usb_config_descriptor *) ptr;
			
			diag_printf("TEST ITEM: GET CONFIGURATION\n");
		
			//Get configuration
			if(conptr->bDescriptorType == USB_DT_CONFIG)
			{	
				//print out all configuration field.		
				read_usb_configure_descriptor(conptr);
		
				usb_device[dev_select_port].bNumInterfaces = conptr->bNumInterfaces;
				usb_device[dev_select_port].bConfigNum = conptr->bConfigurationValue;
				ptr += conptr->bLength;
			}	
			
			//Get Interface
			intptr = (struct usb_interface_descriptor *) ptr;
			if(intptr->bDescriptorType == USB_DT_INTERFACE)
			{
				read_usb_interface_descriptor(intptr);
			
				usb_device[dev_select_port].bInterfaceNum = intptr->bInterfaceNumber;
				usb_device[dev_select_port].bAltSettingNum = intptr->bAlternateSetting;
    			usb_device[dev_select_port].bBaseClass = intptr->bInterfaceClass;
    			usb_device[dev_select_port].bSubClass = intptr->bInterfaceSubClass;
    			usb_device[dev_select_port].bProtocol = intptr->bInterfaceProtocol;
    		
				if(usb_device[dev_select_port].bBaseClass !=BASE_CLASS_MASS_STORAGE)
				{			
					diag_printf("ERROR!! NOT MASS STORAGE CLASS\n");
    		    	return false;
    		    }    			
				ptr += intptr->bLength;
			}		
				
			//Get Endpoint
			endptr = (struct usb_endpoint_descriptor *) ptr;
			while (endptr->bDescriptorType != USB_DT_ENDPOINT)
			{
				ptr += intptr->bLength;
				endptr = (struct usb_endpoint_descriptor *) ptr;
			}	
								
			do
			{
				read_usb_endpoint_descriptor(endptr);	

				if(endptr->bmAttributes == USB_ENDPOINT_XFER_BULK)	//endpoing is bulk
				{										
					if(endptr->bEndpointAddress & USB_ENDPOINT_DIR_MASK)	//bulk in
					{						
						//tmp  = (endptr->wMaxPacketSize&0x00ff)<<8;
						//tmp1 = (endptr->wMaxPacketSize&0xff00)>>8;
						usb_device[dev_select_port].bBulkInEndpoint = endptr->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
						usb_device[dev_select_port].wBulkInMaxPkt =  endptr->wMaxPacketSize_H << 8 | endptr->wMaxPacketSize_L;//0x200;//tmp | tmp1;
//diag_printf("TEST ITEM: bBulkInEndpoint: %x ; wBulkInMaxPkt: %x\n", usb_device[dev_select_port].bBulkInEndpoint, usb_device[dev_select_port].wBulkInMaxPkt);
					}
					else	//bulk out
					{

diag_printf("endptr->wMaxPacketSize: %x %x\n", endptr->wMaxPacketSize_L, endptr->wMaxPacketSize_H);

						//tmp  = (endptr->wMaxPacketSize&0x00ff)<<8;
						//tmp1 = (endptr->wMaxPacketSize&0xff00)>>8;
						usb_device[dev_select_port].bBulkOutEndpoint = endptr->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
						usb_device[dev_select_port].wBulkOutMaxPkt =  endptr->wMaxPacketSize_H << 8 | endptr->wMaxPacketSize_L;//0x200;//tmp | tmp1;
						
diag_printf("TEST ITEM: bBulkOutEndpoint: %x ; wBulkOutMaxPkt: %x\n", usb_device[dev_select_port].bBulkOutEndpoint, usb_device[dev_select_port].wBulkOutMaxPkt);						
					}				
				}
				else if(endptr->bmAttributes == USB_ENDPOINT_XFER_INT)	//endpoint is interrupt
				{
					//not handle interrupt yet
				}
				
				ptr += endptr->bLength;
				endptr = (struct usb_endpoint_descriptor *) ptr;				
			} while (endptr->bDescriptorType == USB_DT_ENDPOINT);
				
diag_printf("Get Configuration END\n");	
					    	
        	break;
        	
		case 3: //hcd - control command(--set address--)
			dr.wRequest = USB_RQ_SET_ADDRESS;
			dr.wValue 	= 0x01 << 8;
			dr.wIndex 	= 0; 
			dr.wLength 	= 0 << 8; 
			setup_data 	= (cyg_uint8 *) &dr;
	            	
			hcd.control_transfer(	0,					//address: set-address has been done!!
									setup_data,			//setup data
									(cyg_uint8 *) buf, 	//buffer from FS
									0); 				//data length
			temp_buf = (cyg_uint8 *) buf;
			diag_printf("TEST ITEM: SET ADDRESS\n");
			for(i=0; i<8; i++)
			{
				diag_printf("  returned data[%d]: %02x  ",i, temp_buf[i]);
				if((i%3)==0)	diag_printf("\n");
			}
			diag_printf("\n");		
			
			break; 
			
		case 4: //urb - control command(--set configuration--)
			usb_device[dev_select_port].bDeviceAddress = 1;	//After setting address, the device address is 1.
			usb_control_msg(	usb_device[dev_select_port],		//USB_DEVICE
								hcd,
								OUT,
								USB_RQ_SET_CONFIGURATION,
								usb_device[dev_select_port].bConfigNum<<8,
								0,
								0,
								0);	
			diag_printf("TEST ITEM: SET CONFIGURATION\n");
			break;       	
        case 5:	//hcd: bulk command (--READ10--)
        	//Set bulk init for HC
			hcd.bulk_init(usb_device[dev_select_port].bDeviceAddress, 
									usb_device[dev_select_port].bBulkInEndpoint, usb_device[dev_select_port].wBulkInMaxPkt,
									usb_device[dev_select_port].bBulkOutEndpoint, usb_device[dev_select_port].wBulkOutMaxPkt);	        	
			diag_printf("bDeviceAddress: %x\n", usb_device[dev_select_port].bDeviceAddress);
			diag_printf("TEST ITEM: bBulkInEndpoint: %x, wBulkInMaxPkt:%x\n", usb_device[dev_select_port].bBulkInEndpoint, usb_device[dev_select_port].wBulkInMaxPkt);
			diag_printf("TEST ITEM: bBulkOutEndpoint: %x, wBulkOutMaxPkt:%x\n", usb_device[dev_select_port].bBulkOutEndpoint, usb_device[dev_select_port].wBulkOutMaxPkt);
			
			for(tmp=0; tmp<16; tmp++)
				cbw.CBWCB[tmp] = 0;
				
        	cbw.dCBWSignature = CBW_SIGNATURE;
        	cbw.dCBWTag = 0;
        	cbw.dCBWDataTransferLength = 0x200<<8;	//1 block
        	cbw.bmCBWFlags = 0x80;
			cbw.bCBWLUN = 0x00;			//*****ATTENTION!! Using PQI card reader which are 2 lun number for current use.
			cbw.bCBWCBLength = 0x0a;
	       	cbw.CBWCB[0] = 0x28;
	       	cbw.CBWCB[1] = 0x00 << 4; 	//*****ATTENTION!! for PQI card reader which are 4 lun for total amount
	       	cbw.CBWCB[2] = 0;			//block position 0
	       	cbw.CBWCB[3] = 0;
	       	cbw.CBWCB[4] = 0;
	       	cbw.CBWCB[5] = 0;
	       	cbw.CBWCB[6] = 0;
	       	cbw.CBWCB[7] = 0;
	       	cbw.CBWCB[8] = 1;			//1-block length
	       	cbw.CBWCB[9] = 0;
	       	
			hcd.bulk_transfer(	1,							//address: set-address has been done!!							
								IN, 						//direction
								(cyg_uint8 *) &cbw, 		//cbw: BOT-setup data
								(cyg_uint8 *) buf,
								0x200, 
								(cyg_uint8 *)&csw );		//csw: BOT-status data
			diag_printf("TEST ITEM: BULK READ10\n");
			diag_printf("  csw.dCSWSignature: %x ;csw.dCSWTag: %x\n", csw.dCSWSignature, csw.dCSWTag);
			diag_printf("  csw.dCSWDataResidue: %x; csw.bCSWStatus: %x\n", csw.dCSWDataResidue, csw.bCSWStatus);
			diag_printf("\n");
			
        	break;
        	
        case 6: //hcd: bulk command (--WRITE10--)
        	cbw.dCBWSignature = CBW_SIGNATURE;
        	cbw.dCBWTag = 0;
        	cbw.dCBWDataTransferLength = 0x200<<8;	//1 block
        	cbw.bmCBWFlags = 0x00;
			cbw.bCBWLUN = 0x00;			//Using PQI card reader which are 2 lun number for current use.
			cbw.bCBWCBLength = 0x0a;			
	       	cbw.CBWCB[0] = 0x2a;
	       	cbw.CBWCB[1] = 0x01 << 4; 	//for PQI card reader which are 4 lun for total amount
	       	cbw.CBWCB[2] = 0;			//block position 0
	       	cbw.CBWCB[3] = 0;
	       	cbw.CBWCB[4] = 0x07;
	       	cbw.CBWCB[5] = 0xB0;
	       	cbw.CBWCB[6] = 0;
	       	cbw.CBWCB[7] = 0;
	       	cbw.CBWCB[8] = 0x01;			//1-block length
	       	cbw.CBWCB[9] = 0;
	       	
			hcd.bulk_transfer(	1,							//address: set-address has been done!!						
								OUT, 						//direction
								(cyg_uint8 *) &cbw, 		//cbw: BOT-setup data
								(cyg_uint8 *) buf,
								0x200, 
								(cyg_uint8 *)&csw );		//csw: BOT-status data
			diag_printf("TEST ITEM: BULK WRITE10\n");								
			diag_printf("  csw.dCSWSignature: %x ;csw.dCSWTag: %x\n", csw.dCSWSignature, csw.dCSWTag);
			diag_printf("  csw.dCSWDataResidue: %x; csw.bCSWStatus: %x\n", csw.dCSWDataResidue, csw.bCSWStatus);
			diag_printf("\n");  			      	
        	break;
        	
        case 7:	//urb: bulk command (--READ10--)
        	//Set bulk init for HC
        	maxlun = 3;	//the max lun is 4
        	curlun = 2;	//the current lun is 2 for card reader
        	
			hcd.bulk_init(usb_device[dev_select_port].bDeviceAddress, 
									usb_device[dev_select_port].bBulkInEndpoint, usb_device[dev_select_port].wBulkInMaxPkt,
									usb_device[dev_select_port].bBulkOutEndpoint, usb_device[dev_select_port].wBulkOutMaxPkt);			
									
			setup_len = usb_bulk_setup(	SC_READ_10, IN ,&cbw, (*len * 0x200), 0, maxlun, curlun);
											
			usb_bulk_msg(	usb_device[dev_select_port],		//USB_DEVICE
			                hcd, 
			                IN, 
			                (cyg_uint8 *)&cbw, 
			                setup_len, 
			                (cyg_uint8 *) buf, 
			                (*len * 0x200),
			                (cyg_uint8 *)&csw);
    		
			result = usb_bulk_status(0, (cyg_uint8 *) buf, csw);	
			
			if(result)
			{
				diag_printf("mass.c - usb_mass_bulk_transport: READ - csw not passed!!\n");
				for(i=1000; i>=0; i--) {}
			}									
									
        	break;
        case 8:
        	clun = (cyg_uint8 *) buf;
        	usb_device[dev_select_port].bLUN = *clun;	//LUN for read assigned
        	diag_printf("TEST ITEM 8: cyg_io_set_config: bLUN= %d\n", usb_device[dev_select_port].bLUN);
        	break;
        //SELECT OHCI / EHCI
//        case 9:
//        	selHcd = (cyg_uint8 *) buf;
//			dev_select_hcd = *selHcd;	//assign EHCI/OHCI
//			diag_printf("TEST ITEM 9: select hcd: %d", dev_select_hcd);
//			break;        	        	
        default:
        	break;
	}	
	return ENOERR;
}

/* -------------------------------------------------------------------------- */
void usb_enable_hub(int hub, int port)
{
	if(hub & ISROOTHUB)
	{                
//		hcd.rh_enable();//ehci_enable_roothub(port);
	}
}



void usb_disable_hub(int hub, int port)
{
	if(hub & ISROOTHUB)
	{                
//		hcd.rh_disable();//ehci_disable_roothub(port);
	}
}

static cyg_uint8 get_hub_state(int hub, int port)
{
    cyg_uint8 result=0;
	
	if(hub & ISROOTHUB)
	{
//		result = hcd.rh_state();//ehci_roothub_state(port);
	}
	return result;
}

//void usb_stop_device(int hub, int port)
//{
//
//}
//void usb_disconnect(int hub, int port)
//{
//        usb_disable_hub(hub, port);
//        //usb_stop_device(hub, port);
//}
/* -------------------------------------------------------------------------- */
//static void get_usb_address(int port)
//{
//	int addr = 0, search = 0, i = 0;
//	
//	search = 1;
//	do {
//		addr = search;
//		
//		for(i=1; i<=MaxDevice; i++)
//		{
//			if(usb_device[i-1].bDeviceAddress == search)
//			{
//				search++;
//				break;
//			}
//		}
//		if(addr == search)
//		{
//			usb_device[port-1].bDeviceAddress = addr;
//		}
//	} while (search != addr);
//}

static cyg_uint8 init_device(int hub, int port)
{
	//cyg_uint8 ret = 0;		
	//int i=0;
	cyg_uint8 addr_to_be_set = 0x01;
	
#ifdef USB_DEBUG_CTRL
	diag_printf("usb.c: init_device-setAddr: device address: %02x, %02x\n", usb_device[dev_select_port].bDeviceAddress, port);	
#endif                   
                 
	//The device address is still default until SET-ADDRESS is done!
	//SET ADDRESS
	usb_control_msg( 	usb_device[dev_select_port],	//USB_DEVICE
						hcd,
						OUT,
						USB_RQ_SET_ADDRESS,
						addr_to_be_set<<8,				//SET DEVICE ADDRESS
						0,
						0,
						0);

	//Set the device address
	//get_usb_address(port);	
	usb_device[dev_select_port].bDeviceAddress = (dev_select_port+1);

#ifdef USB_DEBUG_CTRL
	diag_printf("usb.c: init_device-setConfig: device address: %d, %d\n", usb_device[dev_select_port].bDeviceAddress, port);	
#endif


	//CYGACC_CALL_IF_DELAY_US(10000); //delay_ms(10);
	
	//SET CONFIGURATION
	usb_control_msg(	usb_device[dev_select_port],	//USB_DEVICE
						hcd,
						OUT,
						USB_RQ_SET_CONFIGURATION,
						usb_device[dev_select_port].bConfigNum<<8,
						0,
						0,
						0);

	return true;
}



static cyg_uint8 detect_device(int hub, int port, cyg_uint8 status)
{
	cyg_uint8 data[USB_MAX_CONTROL_DATA_SIZE] = {0};
	struct usb_device_descriptor *devptr = NULL;
	struct usb_config_descriptor *conptr = NULL;
	struct usb_interface_descriptor *intptr = NULL;
	struct usb_endpoint_descriptor *endptr = NULL;
	cyg_uint8 *ptr = NULL;
	//int size, state;
	cyg_uint8 ret = 0;
	cyg_uint8 addr_ret = 0;
	cyg_uint8 conf_ret = 0;
	cyg_uint8 err_counter = 1;
	cyg_uint16 length =0;

D(("usb.c: detect_device: START\n"));

	usb_device[dev_select_port].bHubPortNumber = port;
	usb_device[dev_select_port].wEndp0MaxPacket = 8;
	usb_device[dev_select_port].bEndpointSpeed = status & USB_PORT_STAT_DEV_SPEED_MASK;
	usb_device[dev_select_port].bMaxLUN = 0xFF;			//init value
	usb_device[dev_select_port].bDeviceAddress = 0;	
	
	//for testing: ohci card reader
	//usb_device[dev_select_port].bMaxLUN = 0x03;
	//usb_device[dev_select_port].bLUN	= 0x02;
	
//	if(hub & ISROOTHUB)
//	{
//		usb_enable_hub(hub, port);
//	}

	//1. Get Descriptor (Device) with default size	
	usb_control_msg(	usb_device[dev_select_port],
						hcd,
						IN,
						USB_RQ_GET_DESCRIPTOR,
						USB_DT_DEVICE,
						0,
						data,
						0x08);	//don't know the length yet, send the smallest.
				
	
	devptr = (struct usb_device_descriptor *) data;
	usb_device[dev_select_port].wEndp0MaxPacket = devptr->bMaxPacketSize0;	
	length = devptr->bLength; //the exact data length from device
	
	//set packet size for control init
	hcd.ctrl_init(usb_device[dev_select_port].bDeviceAddress, usb_device[dev_select_port].wEndp0MaxPacket);	

//	memset(&data, 0, USB_MAX_CONTROL_DATA_SIZE);//reset data
//	for(i=0; i<USB_MAX_CONTROL_DATA_SIZE; i++)
//		data[i]=0;
	//CYGACC_CALL_IF_DELAY_US(10000);//delay_ms(10);
			
	//1.1 Get Descriptor (Device) with exact data size
	err_counter = 0;
	do
	{
		addr_ret = 0;
		err_counter++;
		usb_control_msg(	usb_device[dev_select_port],
							hcd,
							IN,
							USB_RQ_GET_DESCRIPTOR,
							USB_DT_DEVICE,
							0,
							data,
							length); 
		
		devptr = (struct usb_device_descriptor *) data;

#ifdef USB_DEBUG_CTRL
	read_usb_device_descriptor(devptr);
#endif		

		if(devptr->bLength!=0x12) //device descriptor size
		{
			diag_printf("ERROR!! Get DEVICE Descriptor incorrect Length \n");
		    addr_ret = USB_GET_DESCRIPTOR_ERROR;//return false;		    
		}
	}while((addr_ret)&&(err_counter<3));
		
	usb_device[dev_select_port].bNumConfigs = devptr->bNumConfigurations;
	usb_device[dev_select_port].bConfigNum = 0;
		
//	memset(&data, 0, USB_MAX_CONTROL_DATA_SIZE); //is it necessary?
	
	//2. Get Descriptor (Configuration) with max size
	usb_control_msg(	usb_device[dev_select_port],
						hcd,
						IN,
						USB_RQ_GET_DESCRIPTOR,
						USB_DT_CONFIG,
						0,
						data,
						usb_device[dev_select_port].wEndp0MaxPacket);	//don't know exact length, use max
	
	
	ptr = data;
	conptr = (struct usb_config_descriptor *) ptr;
	length = ((conptr->wTotalLength>>8) + (conptr->wTotalLength<<8) *0x100); //0x4000 -> 0x0040

	//2.1 Get Descriptor (Configuration) with exact size
	err_counter = 0;
	do
	{
		conf_ret = 0;
		err_counter++;
		usb_control_msg(	usb_device[dev_select_port],
							hcd,
							IN,
							USB_RQ_GET_DESCRIPTOR,
							USB_DT_CONFIG,
							0,
							data,
							length);
    	
		ptr = data;
		conptr = (struct usb_config_descriptor *) ptr;				
			
		if((conptr->bLength != USB_DT_CONFIG_SIZE)||(conptr->bDescriptorType != USB_DT_CONFIG))
		{
			diag_printf("ERROR!! Get CONFIG Descriptor ERROR \n");
		    conf_ret = USB_GET_CONFIGURATION_ERROR;//return false;				
		}				
	}while((conf_ret)&&(err_counter<3));

	//ERROR handle
	if( (addr_ret == USB_GET_DESCRIPTOR_ERROR) || (conf_ret == USB_GET_CONFIGURATION_ERROR) )
		return USB_GET_DESCRIPTOR_ERROR;

		
#ifdef USB_DEBUG_CTRL	
	read_usb_configure_descriptor(conptr);
#endif

	if(conptr->bDescriptorType == USB_DT_CONFIG)		
	{
		usb_device[dev_select_port].bNumInterfaces = conptr->bNumInterfaces;
		usb_device[dev_select_port].bConfigNum = conptr->bConfigurationValue;
		
		ptr += conptr->bLength;
	}

	intptr = (struct usb_interface_descriptor *) ptr;
	if(intptr->bDescriptorType == USB_DT_INTERFACE)
	{
#ifdef USB_DEBUG_CTRL
diag_printf("usb.c: detect-device:  in the USB_DT_INTERFACE\n");
	read_usb_interface_descriptor(intptr);
#endif
	
		usb_device[dev_select_port].bInterfaceNum = intptr->bInterfaceNumber;
		usb_device[dev_select_port].bAltSettingNum = intptr->bAlternateSetting;
    	usb_device[dev_select_port].bBaseClass = intptr->bInterfaceClass;
    	usb_device[dev_select_port].bSubClass = intptr->bInterfaceSubClass;
    	usb_device[dev_select_port].bProtocol = intptr->bInterfaceProtocol;

		if(usb_device[dev_select_port].bBaseClass !=BASE_CLASS_MASS_STORAGE)
		{			
			diag_printf("ERROR!! NOT MASS STORAGE CLASS\n");
        	return false;
        }

		switch(usb_device[dev_select_port].bProtocol)
		{
        	//case PROTOCOL_CBI:
        	case PROTOCOL_CB:
        	case PROTOCOL_BOT:
        		break;
        	default:
        		diag_printf("ERROR!! NOT CB or BOT\n");
        		return false;
		}

		ptr += intptr->bLength;
	}

	endptr = (struct usb_endpoint_descriptor *) ptr;
	while (endptr->bDescriptorType != USB_DT_ENDPOINT)
	{
		ptr += intptr->bLength;
		endptr = (struct usb_endpoint_descriptor *) ptr;
	}
	
	do
	{
#ifdef USB_DEBUG_CTRL
	read_usb_endpoint_descriptor(endptr);	
#endif

		if(endptr->bmAttributes == USB_ENDPOINT_XFER_BULK)	//endpoing is bulk
		{
			if(endptr->bEndpointAddress & USB_ENDPOINT_DIR_MASK)	//bulk in
			{
				usb_device[dev_select_port].bBulkInEndpoint = endptr->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
				usb_device[dev_select_port].wBulkInMaxPkt = endptr->wMaxPacketSize_H << 8 | endptr->wMaxPacketSize_L;;
			}
			else	//bulk out
			{
				usb_device[dev_select_port].bBulkOutEndpoint = endptr->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
				usb_device[dev_select_port].wBulkOutMaxPkt = endptr->wMaxPacketSize_H << 8 | endptr->wMaxPacketSize_L;;
			}
		}
		else if(endptr->bmAttributes == USB_ENDPOINT_XFER_INT)	//endpoint is interrupt
		{
			//not handle interrupt yet
		}
		
		ptr += endptr->bLength;
		endptr = (struct usb_endpoint_descriptor *) ptr;
	} while (endptr->bDescriptorType == USB_DT_ENDPOINT);

#ifdef USB_DEBUG_CTRL
diag_printf("usb.c: usb_bulk_set_packetsize DONE!! ");
diag_printf("  bDeviceAddress: %x\n", usb_device[dev_select_port].bDeviceAddress);
diag_printf("  bBulkInEndpoint: %x, wBulkInMaxPkt: %x\n", usb_device[dev_select_port].bBulkInEndpoint, usb_device[dev_select_port].wBulkInMaxPkt);
diag_printf("  bBulkOutEndpoint: %x, wBulkOutMaxPkt: %x\n", usb_device[dev_select_port].bBulkOutEndpoint, usb_device[dev_select_port].wBulkOutMaxPkt);
#endif
		
//	if(hub & ISROOTHUB && usb_device[port-1].bEndpointSpeed != USB_EPSPEED_HISPEED)
//	{
//		if(hcd.release_endpoint)//if the function is ready
//		{
//#ifdef USB_DEBUG
//	diag_printf("Error!! HC release endpoint!!\n");
//#endif		
			//hcd.release_endpoint(port);//hcd_release_port(port);//ehci_release_port(port);
//		}
//	}

	//CYGACC_CALL_IF_DELAY_US(10000);//delay_ms(10);
	
	//3. INIT Device
	ret = init_device(hub, port);
	return ret;
}

//static void check_port_change(int hub, int port)
//{
//	cyg_uint8 status = 0, retval = 0;
//	
//	status = get_hub_state(hub, port);
//	
//	if(status & USB_PORT_STAT_DEV_CONNECT_CHANGED)
//	{
//		if(status & USB_PORT_STAT_DEV_CONNECTED)
//		{
//			retval = detect_device(hub, port, status);
//			if(retval)
//			{
//				usb_device[port-1].bActive = true;
//			}
//			else
//			{
//			        usb_device[port-1].bDeviceAddress = 0;
//			        usb_disable_hub(hub, port);
//			}
//		}
//		else
//		{
//			usb_device[port-1].bDeviceAddress = 0;
//			usb_disable_hub(hub, port);
//		}
//	}
//}

/* -------------------------------------------------------------------------- */


void usb_host_interrupt(void)
{
	hcd.irq();
}

cyg_uint8 get_usb_device(void)
{
	return dev_select_port;
}

struct hc_driver get_usb_hcd(void)
{
	return hcd;
}

static void read_usb_device(struct USB_DEVICE usb_device)
{
	diag_printf("=============  usb_device  =================\n");
	diag_printf("usb_device.bActive: %02x\n", usb_device.bActive);
	diag_printf("usb_device.bHubDeviceNumber: %02x\n", usb_device.bHubDeviceNumber);
	diag_printf("usb_device.bHubPortNumber: %02x\n", usb_device.bHubPortNumber);
	diag_printf("usb_device.bDeviceAddress: %02x\n", usb_device.bDeviceAddress);
	diag_printf("usb_device.bEndpointSpeed: %02x\n", usb_device.bEndpointSpeed);
	diag_printf("usb_device.bEndpointNum: %02x\n", usb_device.bEndpointNum);	
	diag_printf("usb_device.wEndp0MaxPacket: %x\n", usb_device.wEndp0MaxPacket);
	diag_printf("usb_device.bBulkInEndpoint: %02x\n", usb_device.bBulkInEndpoint);
	diag_printf("usb_device.wBulkInMaxPkt: %x\n", usb_device.wBulkInMaxPkt);
	diag_printf("usb_device.bBulkOutEndpoint: %02x\n", usb_device.bBulkOutEndpoint);
	diag_printf("usb_device.wBulkOutMaxPkt: %x\n", usb_device.wBulkOutMaxPkt);
	diag_printf("usb_device.bDataSync: %02x\n", usb_device.bDataSync);
	diag_printf("usb_device.bNumConfigs: %02x\n", usb_device.bNumConfigs);
	diag_printf("usb_device.bNumInterfaces: %02x\n", usb_device.bNumInterfaces);
	diag_printf("usb_device.bConfigNum: %02x\n", usb_device.bConfigNum);
	diag_printf("usb_device.bInterfaceNum: %02x\n", usb_device.bInterfaceNum);
	diag_printf("usb_device.bAltSettingNum: %02x\n", usb_device.bAltSettingNum);
	diag_printf("usb_device.bBaseClass: %02x\n", usb_device.bBaseClass);
	diag_printf("usb_device.bSubClass: %02x\n", usb_device.bSubClass);
	diag_printf("usb_device.bProtocol: %02x\n", usb_device.bProtocol);
	diag_printf("usb_device.bMaxLUN: %02x\n", usb_device.bMaxLUN);
	diag_printf("usb_device.bLUN: %02x\n", usb_device.bLUN);	
	diag_printf("============= End ! usb_device End ! ===============\n");	
}

static void read_usb_device_descriptor(struct usb_device_descriptor *devptr)
{
	diag_printf("(usb.c:detect_device) =====ENUM-GET_DESCRIPTOR (DEVICE) ====\n");
	diag_printf("  devptr->bLength: %02x ; devptr->bDescriptorType: %02x\n", devptr->bLength, devptr->bDescriptorType);
//	diag_printf("devptr->bcdUSB:  %x\n", (devptr->bcdUSB>>8)+(devptr->bcdUSB&0x00ff)<<8);
	diag_printf("  devptr->bDeviceClass: %02x ; devptr->bDeviceSubClass: %x\n", devptr->bDeviceClass, devptr->bDeviceSubClass);
	diag_printf("  devptr->bDeviceProtocol: %02x ; devptr->bMaxPacketSize0: %02x\n", devptr->bDeviceProtocol, devptr->bMaxPacketSize0);
	diag_printf("  devptr->idVendor: %x\n", devptr->idVendor);
	diag_printf("  devptr->idProduct: %x\n", devptr->idProduct);
	diag_printf("  devptr->bcdDevice: %x\n", devptr->bcdDevice);
	diag_printf("  devptr->iManufacturer: %02xx ; devptr->iProduct: %02x\n", devptr->iManufacturer, devptr->iProduct);
	diag_printf("devptr->iSerialNumber: %02xx ; devptr->bNumConfigurations: %02x\n", devptr->iSerialNumber, devptr->bNumConfigurations);
	diag_printf("======== END! ENUM-GET_DESCRIPTOR END! =========\n");	
}

static void read_usb_configure_descriptor(struct usb_config_descriptor *conptr)
{
	diag_printf("(usb.c:detect_device) ===ENUM-GET_DESCRIPTOR (CONFIG) ====\n");
	diag_printf("  conptr->bLength: %02x ; conptr->bDescriptorType: %02x\n", conptr->bLength, conptr->bDescriptorType);
	diag_printf("  conptr->wTotalLength: %x\n", conptr->wTotalLength>>8);
	diag_printf("  conptr->bNumInterfaces: %02x ; conptr->bConfigurationValue: %02x\n", conptr->bNumInterfaces, conptr->bConfigurationValue);
	diag_printf("  conptr->iConfiguration: %02x ; conptr->bmAttributes: %02x\n", conptr->iConfiguration, conptr->bmAttributes);
	diag_printf("  conptr->bMaxPower: %02x\n", conptr->bMaxPower);	
}

static void read_usb_interface_descriptor(struct usb_interface_descriptor *intptr)
{
	diag_printf("(usb.c:detect_device) ===ENUM-GET_DESCRIPTOR (INTERFACE) ====\n");
	diag_printf(" intptr->bLength: %02x ; intptr->bDescriptorType: %02x\n", intptr->bLength, intptr->bDescriptorType);
	diag_printf(" intptr->bInterfaceNumber: %02x ; intptr->bAlternateSetting: %02x\n", intptr->bInterfaceNumber, intptr->bAlternateSetting);
	diag_printf(" intptr->bNumEndpoints: %02x\n", intptr->bNumEndpoints);
	diag_printf(" intptr->bInterfaceClass: %02x\n", intptr->bInterfaceClass);
	diag_printf(" intptr->bInterfaceSubClass: %02x\n", intptr->bInterfaceSubClass);
	diag_printf(" intptr->bInterfaceProtocol: %02x\n", intptr->bInterfaceProtocol);	
	diag_printf(" intptr->iInterface: %02x\n", intptr->iInterface);	
	diag_printf("\n");	
}

static void read_usb_endpoint_descriptor(struct usb_endpoint_descriptor *endptr)
{
	//cyg_uint16 tmp, tmp1=0;
	diag_printf("===ENUM-GET_DESCRIPTOR (ENDPOINT) ====\n");
	diag_printf("  endptr->bLength: %02x\n", endptr->bLength);
	diag_printf("  endptr->bDescriptorType: %02x\n", endptr->bDescriptorType);
	diag_printf("  endptr->bEndpointAddress: %02x\n", endptr->bEndpointAddress);
	diag_printf("  endptr->bmAttributes: %02x\n", endptr->bmAttributes);
	
//	diag_printf("  endptr->wMaxPacketSize: %04d\n", endptr->wMaxPacketSize);
	diag_printf("  endptr->bInterval: %02x\n", endptr->bInterval);	
	diag_printf("\n");	
}
