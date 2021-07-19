//******************************************
// mass.c  
// Fill the scsi commands for "Mass Storage"
// in usb
//******************************************
#include <stdio.h>                      /* printf */
#include <stdlib.h>                      /* printf */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <errno.h>	// Cyg_ErrNo
#include <cyg/infra/cyg_type.h> // base types

#include <cyg/io/usb/host/io_usb_host.h>
#include <pkgconf/devs_usb_host.h>
//=========================================
//Locals
//=========================================
extern char Getch(void);
#define DAFAULT_BUFFER_SIZE 8192

//=========================================
//Function Prototype
//=========================================

//=========================================
//Main Functions
//=========================================
void cyg_user_start(void)
{
	Cyg_ErrNo err = ENOERR;
	cyg_io_handle_t handle_usb;
	char *buf = NULL;
//	cyg_uint8 buf[512] = {0};
//	cyg_uint32 blen=0, bpos=0;
    cyg_bool disk_mounted = false;	
    int ret = 0;
    FILE *fp = NULL, *copy = NULL;
	cyg_uint32 lun, len=0;
	
	diag_printf("Prepare to test USB.\n");
	
	//USB mount disk test
	//USB init in lookup
	diag_printf("cyg_io_lookup prepare\n");
//	err = cyg_io_lookup(CYGDAT_DEVS_DISK_USB_DISK_NAME, &handle_usb);
//	if (err != ENOERR)
//		diag_printf("lookup fails\n");
//	
//	if (err == ENOERR)
// 	{

//    diag_printf("Prepare to mount USB on partition 0.\n");
//    err = mount( "/dev/usbblk0/", "/", "fatfs" );
//    if (err)
//    {
//        diag_printf(" Mount fails!; err = %d\n", err);
//		diag_printf("cyg_io_set_config - set current lun.\n");
//		len = sizeof(cyg_uint32);
//		lun = 0;
//		cyg_io_set_config(handle_usb, 8, (void*) &lun, &len);		//assign 0 current lun
			
        diag_printf("Prepare to mount USB on partition 1.\n");
        err = mount( "/dev/usbblk0/1", "/", "fatfs" );
        if (err)
        {
            diag_printf(" Mount fails!\n");
        }
        else
        {
            diag_printf(" Mount success!\n");
            disk_mounted = true;
        }
//    }
//    else
//    {
//        diag_printf(" Mount success!\n");
//        disk_mounted = true;
//    }
    
    diag_printf("FILE COPY TEST.\n");
    if (disk_mounted)
    {       
        //wait_for_tickup();

        //for (count = 0;  count < 10; count++)
        {
//            diag_printf("[%02d] ", count);
//            t1 = cyg_current_time();
    
            diag_printf("Prepare to open input file.\n");
            fp = fopen("IN_FILE.JPG", "rb");
            if (fp == NULL)
            {
                diag_printf("Cannot open file for test.\n");
                goto FINISHED;
            }
            //diag_printf("Prepare to open output file.\n");
            copy = fopen("OUT.JPG", "wb");
            buf = (char *)malloc(DAFAULT_BUFFER_SIZE);
            if (buf)
            {
            	diag_printf("fread \n");
                ret = fread(buf, 1, 4096, fp);
                diag_printf("Writing file\n");
                //while(ret > 0)
                //{   
                	diag_printf("fwrite \n");
                    ret = fwrite(buf, 1, ret, copy);
                    //diag_printf("fwrite: ret %x \n", ret);
                    //if (ret > 0)
                    //{
                    //	diag_printf("fread in write\n");
                    //    ret = fread(buf, 1, 4096, fp);
//                  //      diag_printf(".");
                    //}
                //}
                diag_printf("fclose fp\n");
                fclose(fp);
                diag_printf("fclose copy\n");
                fclose(copy);
                //cyg_uint32 config_val = 0;
                //cyg_uint32 config_val_len = sizeof(cyg_uint32);
                //cyg_io_set_config(handle_usb, USB_CTRL_SET_CONFIG, buf, &len);
                //cyg_io_set_config(hSD, 6, &config_val, &config_val_len);
                
                diag_printf("free buf \n");
                free(buf);
//                diag_printf("\n");
            }
//            t2 = cyg_current_time();
//            ticks = t2 - t1;
//            cyg_tick2time(ticks);
        }
        
    }
FINISHED:    
    diag_printf("COPY TEST FINISHED.\n");
Getch();

    	
	//USB init in lookup
//	err = cyg_io_lookup("/dev/usb/ehci", &handle_usb);
//	
//	if (err == ENOERR)
// 	{			 				
//		diag_printf("USB Single READ (from device) TEST.\n");		
//		
//		//READ IN a block thro IO
//		blen = 1;
//		bpos = 0;	
//		//memset(buf, 0xFF, 512);
//		diag_printf("usb test - cyg_io_bread before\n");
//		err = cyg_io_bread(handle_usb, buf, &blen, bpos);
//		diag_dump_buf_with_offset(buf, 512, buf);
//		diag_printf("block io read Finished.\n");
//		free(buf);	
//Getch();		
//		diag_printf("USB Single WRITE (from host) TEST.\n");	
//		//WRITE OUT a block thro IO
//		blen = 1;
//		bpos = 933;
//		memset(buf, 0xFF, 512);
//		err = cyg_io_bwrite(handle_usb, buf, &blen, bpos);
//		diag_dump_buf_with_offset(buf, 512, buf);
//		diag_printf("block io write Finished.\n");
//		free(buf);					
//Getch();	
//	}
}
