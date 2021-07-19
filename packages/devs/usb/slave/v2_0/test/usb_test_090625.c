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
	
diag_printf("BOUT WAIT: ret: %d; timeOut: %d ..", slave_result, timeout);	
	
	return slave_result;
}

static void debug_printbuf(struct SLAVE_DEVICE *buf)
{
//	diag_printf("buf->bCBWReceived:%x\n", buf->bCBWReceived);
//	diag_printf("buf->bHost_MaxLun:%x\n", buf->bHost_MaxLun);
//	diag_printf("buf->bHost_CurLun:%x\n", buf->bHost_CurLun);
	diag_printf("TEST: bHost_Command:%x  ", buf->bHost_Command);
	diag_printf("dHost_LBA:%x  ", buf->dHost_LBA);
	diag_printf("dHost_blocks:%x ", buf->dHost_blocks);
	
//	diag_printf("buf->bDevice_MaxLun:%x\n", buf->bDevice_MaxLun);
//	diag_printf("buf->bDevice_CurLun:%x\n", buf->bDevice_CurLun);
//	diag_printf("buf->bDevice_Connected:%x\n", buf->bDevice_Connected);
//	diag_printf("buf->bDevice_Reserve:%x\n", buf->bDevice_Reserve);
//	diag_printf("buf->dDevice_TotalBlocks:%x\n", buf->dDevice_TotalBlocks);
//	diag_printf("buf->data_len:%x\n", buf->data_len);
//	diag_printf("buf->data_buf:%x\n",buf->data_buf[0]);		
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
		diag_printf("TEST:usb slave lookup fails:err!\n", err);
		goto fail;
	}

	// force to free buffers
	if(diskInfo)	{free(diskInfo);	diag_printf("free1\n");}
	if(diskBuf)		{free(diskBuf);		diag_printf("free2\n");}
	if(buf)			{free(buf);			diag_printf("free3\n");}

	//allocate necessary memory
	diskInfo	= (cyg_disk_info_t *)malloc( sizeof(cyg_disk_info_t) );
	buf 		= (struct SLAVE_DEVICE *)malloc( sizeof(struct SLAVE_DEVICE) );	
//	diskBuf		= (cyg_uint8 *)malloc_align(512, 32);//1 block
				
	memset(buf,0,sizeof(struct SLAVE_DEVICE));
	
	tempLen=1;
	cyg_io_get_config(handle_usb, USB_SLAVE_GET_CONFIG_IO_BUFFER, &tempPos, &tempLen);

	//point to the same buf address
	diskBuf = (cyg_uint8 *)tempPos;
	buf->data_buf = (cyg_uint8 *)tempPos;
//diag_printf("TEST: buf_add: %x %x\n", diskBuf, buf->data_buf);	
	
	//loop to get/send the cmd/data
	while(1)
	{
		//get ctrl-end from slave
		slave_result = bulk_inout_ready(USB_SLAVE_BULK_OUT, handle_usb, USB_SLAVE_GET_CONFIG_DATA_STATUS);				

		if(USB_SLAVE_BULK_OUT == slave_result)//bulk out-data in 
		{		
			//get USB INFO from slave
			tempLen = 31;
			if(err = cyg_io_bread(handle_usb, buf, &tempLen, tempPos))
			{
				diag_printf("TEST:USB cyg_io_bread fails!: %d", err);
				goto fail;
			}
			
			//debug log
			if( (buf->bHost_Command==SC_READ_10) || (buf->bHost_Command==SC_WRITE_10))
            	debug_printbuf(buf);
			
 			//Devices lookup
 			if(CLUN_MS == buf->bHost_CurLun)
 				sprintf(diskStr, "/dev/msblk0/");
 			else if(CLUN_SD == buf->bHost_CurLun)
 				sprintf(diskStr, "/dev/sdblk0/");
 			else ; //others: usb device
//diag_printf("slave_getData: bulk-out: deviceString: %s\n", diskStr);
 		
 			//Disk Init
 			if(err = cyg_io_lookup(diskStr, &hD))
			{
				buf->bDevice_Connected   = false;
				diag_printf("TEST:%s  lookup fails!: %d", diskStr, err);
				//goto fail;
			}
			else
			{
				buf->bDevice_Connected   = true;
			}

			// Get DISK INFO - DISK get config
			if(!err)
			{
				err = cyg_io_get_config(hD, CYG_IO_GET_CONFIG_DISK_INFO, (void *)diskInfo, &diskInfo_len);
				if(err)		diag_printf("TEST:DISK_get_config fails!: %x", err);
			}
				
			buf->dDevice_TotalBlocks = diskInfo->blocks_num-1;
			//buf->bDevice_Connected   = true;		

			// Handle host commands
			if(buf->bHost_Command == SC_WRITE_10)//WRITE COMMAND: write data to device
			{
				diag_printf("TEST: WRITE ");
				cyg_uint32 blockNum = buf->dHost_blocks;
				diskPos = buf->dHost_LBA;

				do
				{
					//get data packet from host
					diag_printf("BlockNum:%x diskPos:%x .. ", blockNum, diskPos);
					
					//get USB INFO(data packet) from slave
        			if(blockNum<=MAX_IO_BLOCKS)
        				diskLen = blockNum;
        			else
        				diskLen = MAX_IO_BLOCKS;
        				
					for(count=0; count<2; count++)
					{
						tempLen = 512*diskLen;
						tempPos = diskPos;
												
						err = cyg_io_bread(handle_usb, buf, &tempLen, tempPos);        	    	
						if(!err)
							break;
						else
							diag_printf("TEST WRITE: RETRY%x FAIL:%x***** ",count, err);
					}
					
					if( (err != ENOERR)	&& (!(count%3)) )
					{
						diag_printf("TEST WRITE STOPs***** ");
						break;
					}

					//write blocks to disk
					diag_printf("TEST:diskBuf_add:%x ", diskBuf	);
					err = cyg_io_bwrite(hD, diskBuf, &diskLen, diskPos);
        	    	
					if(err != ENOERR)
					{
						diag_printf("TEST:disk io write FAILs:%x***** ", err);  
						err = cyg_io_bwrite(hD, diskBuf, &diskLen, diskPos);
						if(err !=ENOERR)
						{
							diag_printf("TEST:disk io write RETRY FAILs:%x***** ", err);
							goto fail;
						}
					}
															
					blockNum = (blockNum<=MAX_IO_BLOCKS)?0:(blockNum-MAX_IO_BLOCKS);
					if(!blockNum)	diskPos += MAX_IO_BLOCKS;
						
        		}while( (!diskLen) && ( diskPos <= (buf->dHost_LBA+buf->dHost_blocks) ) );							

				///if( (buf->dHost_LBA==0)||(buf->dHost_LBA==buf->dDevice_TotalBlocks))
				///{
				///	diag_printf("<LBA End>\n");
				///	for(count=0; count<512 ; count++) //memcpy     
				///	{
				///	//sdev->data_buf[count] = data_buf[count];   
				///		diag_printf("[%03d]%02x  ",count, buf->data_buf[count]);
				///		if((count%10)==9)
				///			diag_printf("\n");				
				///	}																										
				///}
        					
				//csw response
				tempLen = CSW_SIZE;
				if(err != ENOERR)
					buf->csw.bCSWStatus = 0x01;
				else
					buf->csw.bCSWStatus = 0x00;
				err = cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);
				if(err != ENOERR)	
					diag_printf("TEST:WRITE CMD status error: %x***** ", err);				
			}//write-cmd-end
			else if(buf->bHost_Command == SC_READ_10)//READ COMMAND: read data from device
			{			
				//diag_printf("TEST READ: host_blocks: %x, Host_LBA: %x .. ", buf->dHost_blocks, buf->dHost_LBA);
				cyg_uint32 blockNum = buf->dHost_blocks;
				diskPos = buf->dHost_LBA;
				//diskLen = 1; //always 1 block at a time
        		
        		//128blocks at a time
        		do
        		{
        			//read blocks from disk
        			diag_printf(" TEST READ: blockNum: %x, diskPos: %x .. ", blockNum, diskPos);
        			if(blockNum<=MAX_IO_BLOCKS)
        				diskLen = blockNum;
        			else
        				diskLen = MAX_IO_BLOCKS;
 diag_printf("TEST-READ:%x ", blockNum);       				
        			if(err = cyg_io_bread(hD, diskBuf, &diskLen, diskPos))
					{
						diag_printf("TEST:disk io write fails!err: %d***** ", err);
						goto fail;
					} 
					else
					{
						//write data to host with 3 retry
						for(count=0; count<3; count++)
						{
diag_printf("TEST-READ:Retry:%x ", count);																					
							tempLen = diskLen*512; //residue data length
							tempPos = diskPos; //current lba
							
							err = cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);
							
							if(!err)
								break;
							else
								diag_printf("TEST READ: RETRY FAIL:%x***** ", err);
						}//for-end
						
						if( (err != ENOERR)	&& (!(count%3)) )
						{
							diag_printf("TEST READ STOPs***** ");
							break;
						}						
																																							
					}//disk read if-end
					
					blockNum = (blockNum<=MAX_IO_BLOCKS)?0:(blockNum-MAX_IO_BLOCKS);
					if(!blockNum)	diskPos += MAX_IO_BLOCKS;
						
        		}while( (!diskLen) && ( diskPos <= (buf->dHost_LBA+buf->dHost_blocks) ) );        	        	        		
				
				//CSW response
				tempLen = CSW_SIZE;
				if(err != ENOERR)
					buf->csw.bCSWStatus = 0x01;
				else
					buf->csw.bCSWStatus = 0x00;
				err = cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);
				if(err != ENOERR)	
					diag_printf("TEST: READ CMD status error: %x***** ", err);
			}//read-cmd-end
			else//OTHER COMMAND
			{							
				tempLen = 1; //length at least one but no need accurate
				cyg_io_bwrite(handle_usb, buf, &tempLen, tempPos);	//write all to usb host	
			}//other-command-end 																						
		}//bulk-out-end
	}//while-end
	
fail:
	if(diskInfo)	{free(diskInfo);	diag_printf("free11\n");}
	if(diskBuf)		{free(diskBuf);		diag_printf("free22\n");}
	if(buf)			{free(buf);			diag_printf("free33\n");}	
}


void cyg_user_start(void)
{
	int i=0;
	int 		 thread_a_stack[ THREAD_STACK_SIZE ];
	cyg_thread 	 thread_a_obj;
	cyg_handle_t thread_a_hdl;
	
	//----- test code starts! -----
	diag_printf("=== Prepare to test USB DEVICE. ===\n");

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
