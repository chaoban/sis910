//******************************************
// usb_test.c
// usb device test code  
//******************************************
#include <stdio.h>                      /* printf */
#include <stdlib.h>                      /* printf */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <pkgconf/system.h>
#include <cyg/infra/diag.h>
#include <errno.h>	// Cyg_ErrNo
#include <cyg/infra/cyg_type.h> // base types
#include <pkgconf/io_usb_host.h>
#include <pkgconf/devs_usb_host.h>
#include <cyg/io/slave.h>
#include <cyg/io/mass.h>
#include <cyg/io/usb.h>
#include <cyg/io/disk.h>
#include <cyg/io/diskio.h>

//=========================================
//Locals
//=========================================
extern char Getch(void);

#define DAFAULT_BUFFER_SIZE 	8192
#define THREAD_STACK_SIZE		(65536 / sizeof(int))
#define THREAD_PRIORITY			12
#define MAX_PKT_SIZE_11			64

cyg_io_handle_t handle_usb;
cyg_io_handle_t hD;
cyg_disk_info_t *diskInfo 	= NULL;
struct SLAVE_DEVICE	*buf	= NULL;
cyg_uint8 * diskBuf 		= NULL;
//=========================================
//Function Prototype
//=========================================
cyg_uint8 fakedisk[] = {
	0x52, 0x52, 0x61, 0x41, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x72, 0x72, 0x41, 0x61, 
	0xc4, 0xc3, 0x01, 0x00, 0xf2, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa, 
};


//=========================================
//Main Functions
//=========================================
static cyg_uint32 bulk_inout_ready(int inout, cyg_io_handle_t handle, int config)
{
	cyg_uint32	timeout = 0;
	cyg_uint32	count = 0;
	cyg_uint32	slave_config = 1;
	cyg_uint8 	slave_result = 0;
		
	do{//wait until the data packet is received
		timeout++;	
		for(count=0;count<100; count++) ; // -->wait	
		
		slave_config = 1;
		cyg_io_get_config(handle, config, &slave_result, &slave_config);
		
		if(timeout > 500)		break;
			
	}while(slave_result != inout);	
	
diag_printf("\nTEST: bulk_inout_ready: result: %d; timeout: %d\n", slave_result, timeout);	
	
	return slave_result;
}

static void debug_printbuf(struct SLAVE_DEVICE *buf)
{
	diag_printf("buf->bCBWReceived:%x\n", buf->bCBWReceived);
	diag_printf("buf->bHost_MaxLun:%x\n", buf->bHost_MaxLun);
	diag_printf("buf->bHost_CurLun:%x\n", buf->bHost_CurLun);
	diag_printf("buf->bHost_Command:%x\n", buf->bHost_Command);
	diag_printf("buf->dHost_LBA:%x\n", buf->dHost_LBA);
	diag_printf("buf->dHost_blocks:%x\n", buf->dHost_blocks);
	
	diag_printf("buf->bDevice_MaxLun:%x\n", buf->bDevice_MaxLun);
	diag_printf("buf->bDevice_CurLun:%x\n", buf->bDevice_CurLun);
	diag_printf("buf->bDevice_Connected:%x\n", buf->bDevice_Connected);
	diag_printf("buf->bDevice_Reserve:%x\n", buf->bDevice_Reserve);
	diag_printf("buf->dDevice_TotalBlocks:%x\n", buf->dDevice_TotalBlocks);
	diag_printf("buf->data_len:%x\n", buf->data_len);
	diag_printf("buf->data_buf:%x\n",buf->data_buf[0]);		
}


static void slave_getData(void)
{ 
	cyg_uint8 	slave_result = 0;	
	cyg_uint32	count = 0;
	cyg_uint32	tempPos = 0;
	cyg_uint32	tempLen = 1;
    cyg_uint32	diskPos = 0;
    cyg_uint32	diskLen = 0;     
	Cyg_ErrNo 	err	= ENOERR;
    cyg_uint32 	diskInfo_len = sizeof(cyg_disk_info_t);	
    char diskStr [50]={0};	    
    
	diag_printf("slave_getData - START!\n");    	
	
	//usb device loopup
	err = cyg_io_lookup("/dev/slave/", &handle_usb);
	
	if (err != ENOERR)
	{
		diag_printf("usb slave lookup fails!\n");
		goto fail;
	}

	// force to free buffers
	if(diskInfo)	{free(diskInfo);	diag_printf("free1\n");}
	if(diskBuf)		{free(diskBuf);		diag_printf("free2\n");}
	if(buf)			{free(buf);			diag_printf("free3\n");}

	//allocate necessary memory
	diskInfo	= (cyg_disk_info_t *)malloc( sizeof(cyg_disk_info_t) );
	buf 		= (struct SLAVE_DEVICE *)malloc( sizeof(struct SLAVE_DEVICE) );	
	diskBuf		= (cyg_uint8 *)malloc_align(512, 32);//1 block
				
	memset(buf,0,sizeof(struct SLAVE_DEVICE));
diag_printf("TEST: buf: %x\n", buf);
	
	//loop to get/send the cmd/data
	while(1)
	{
		//get ctrl-end from slave
diag_printf("TEST: bulk-out WAIT:\n");

		slave_result = bulk_inout_ready(USB_SLAVE_BULK_OUT, handle_usb, USB_SLAVE_GET_CONFIG_DATA_STATUS);				

		if(USB_SLAVE_BULK_OUT == slave_result)//bulk out-data in 
		{
			//get USB INFO from slave
			if(err = cyg_io_bread(handle_usb, buf, &tempLen, tempPos))
			{
				diag_printf("USB cyg_io_bread fails!: %d\n", err);
				goto fail;			
			}
			
			//debug log
            debug_printbuf(buf);
			
 			//Devices lookup
 			if(CLUN_MS == buf->bHost_CurLun)		
 				sprintf(diskStr, "/dev/msblk0/");
 			else if(CLUN_SD == buf->bHost_CurLun)
 				sprintf(diskStr, "/dev/sdblk0/");
 			else ; //others: usb device
diag_printf("slave_getData: bulk-out: deviceString: %s\n", diskStr);
 		
 			//Disk Init
 			if(err = cyg_io_lookup(diskStr, &hD))
			{
				buf->bDevice_Connected   = false;
				diag_printf("%s  lookup fails!: %d\n", diskStr, err);
				goto fail;
			}
			else
			{
				buf->bDevice_Connected   = true;
			}

			// Get DISK INFO - DISK get config
			err = cyg_io_get_config(hD, CYG_IO_GET_CONFIG_DISK_INFO, (void *)diskInfo, &diskInfo_len);
				
			buf->dDevice_TotalBlocks = diskInfo->blocks_num;
			buf->bDevice_Connected   = true;		
diag_printf("TEST: disk totalB: %x; command: %x\n", buf->dDevice_TotalBlocks, buf->bHost_Command);	


			// Handle host commands
			if(buf->bHost_Command == SC_WRITE_10)//WRITE COMMAND: write data to device
			{
				diag_printf("TEST: WRITE\n");
				cyg_uint32 blockNum = buf->dHost_blocks;
				diskPos = buf->dHost_LBA;
				
				//data writing
				for(; blockNum>0; blockNum--,diskPos++)
				{						
					//get data packet from host
					diag_printf("TEST: WRITE-CMD\n");
					slave_result = bulk_inout_ready(USB_SLAVE_BULK_OUT, handle_usb, USB_SLAVE_GET_CONFIG_DATA_STATUS);
				
					//get USB INFO(data packet) from slave
					cyg_io_bread(handle_usb, buf, &tempLen, tempPos);        	    	
				
					for(count=0; count<512; count++)  //memcpy
						diskBuf[count] = buf->data_buf[count];
        	    	
					//write blocks to disk
					diskLen = 1;			//always 1 block at a time
					err = cyg_io_bwrite(hD, diskBuf, &diskLen, diskPos);
        	    	
					if(err != ENOERR)
					{
						diag_printf("disk io write block fails!\n");  
						goto fail;
					}
        	
				}//for-end
        	
				//write CSW to host	
				tempLen=0;	//no residue length
				cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);		
			}//write-cmd-end
			else if(buf->bHost_Command == SC_READ_10)//READ COMMAND: read data from device
			{
				diag_printf("TEST: READ: host_blocks: %x, buf->dHost_LBA: %x\n", buf->dHost_blocks, buf->dHost_LBA);
				cyg_uint32 blockNum = buf->dHost_blocks;
				diskPos = buf->dHost_LBA;
				diskLen = 1; //always 1 block at a time
        	
				for(; blockNum>0; blockNum--,diskPos++)
				{
					//read blocks from disk
					diag_printf("TEST: READ: read from disk: blockNum: %x, diskPos: %x\n", blockNum, diskPos);
					////if(err = cyg_io_bread(hD, diskBuf, &diskLen, diskPos))
					////{
					////	diag_printf("disk io write block fails!err: %d\n", err);
					////	//retry??
					////}
					////else
					{
						diag_printf("TEST: READ: disk data print: \n");
						for(count=0; count<512; count++)  //memcpy
						{
							buf->data_buf[count] = fakedisk[count];
							diag_printf("[%03d]%02x  ",count, buf->data_buf[count]);
							if((count%8)== 7)
								diag_printf("\n");
						}		
						
						//write data to host	
						
						tempLen =512; //residue data length
						tempPos = buf->dHost_blocks; //current lba
						diag_printf("TEST: READ: read from slave \n");
																	
						err = cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);
						if(err != ENOERR)
						{
							tempLen =512; //residue data length
							tempPos = buf->dHost_blocks; //current lba
							diag_printf("TEST: READ: read from slave - RETRY \n");
																		
							err = cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);							
							if(err != ENOERR)
							{
								diag_printf("TEST READ cmd: retry bulk-in fail\n");
								break;
							}
						}					
					}//disk read if-end
				}//for-end
				
				tempLen = CSW_SIZE;
				if(err != ENOERR)
					buf->csw.bCSWStatus = 0x01;
				err = cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);
				if(err != ENOERR)	
					diag_printf("TEST: READ CMD status error\n");			
			}//read-cmd-end
			else//OTHER COMMAND
			{			
				diag_printf("TEST: OTHER CMDs wait:\n");
				//slave_result = bulk_inout_ready(USB_SLAVE_BULK_IN, handle_usb, USB_SLAVE_GET_CONFIG_DATA_STATUS);
	
				//diag_printf("\nTEST: bulk-in: result: %d; count: %d\n", slave_result, count);	

				//if(USB_SLAVE_BULK_IN == slave_result)//bulk out-data in 
				{
					tempLen = 1;									
					cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);	//write all to usb host	
				}
			}//other-command-end 																						
		}//bulk-out-end
	}//while-end
	
fail:
	if(diskInfo)	{free(diskInfo);	diag_printf("free1\n");}
	if(diskBuf)		{free(diskBuf);		diag_printf("free2\n");}
	if(buf)			{free(buf);			diag_printf("free3\n");}	
}


void cyg_user_start(void)
{
	int i=0;
	int 		 thread_a_stack[ THREAD_STACK_SIZE ];
	cyg_thread 	 thread_a_obj;
	cyg_handle_t thread_a_hdl;
	
	//----- test code starts! -----
	diag_printf("== Prepare to test USB DEVICE. ==\n");

	while(1)
	{
		slave_getData();
		for(i=0; i<100; i++)
			diag_printf(".");
	}
//	cyg_thread_create(	THREAD_PRIORITY,
//						(cyg_thread_entry_t*)slave_getData,
//						(cyg_addrword_t)0,
//						"USB Slave Thread",
//						(void *)thread_a_stack,
//						THREAD_STACK_SIZE,
//		    			&thread_a_hdl,
//		    			&thread_a_obj	);
//		    			
//	//Cases a thread to continue execution
//	cyg_thread_resume(thread_a_hdl);
//	
//	cyg_scheduler_start();
	
}//cyg_user_start-end
