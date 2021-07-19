//******************************************
// usb_test.c  
// usb device test code 
//******************************************
#include <stdio.h>                      /* printf */
#include <stdlib.h>                      /* printf */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <errno.h>	// Cyg_ErrNo
#include <cyg/infra/cyg_type.h> // base types

//#include <cyg/io/usb/sis910host/io_usb_host.h>
#include <pkgconf/devs_usb_host.h>
#include <pkgconf/io_usb_host.h>
//=========================================
//Locals
//=========================================
extern char Getch(void);
#define DAFAULT_BUFFER_SIZE 8192
#define THREAD_STACK_SIZE	( 8192 / sizeof(int) )
#define THREAD_PRIORITY		12

int thread_a_stack[ THREAD_STACK_SIZE ];
cyg_thread thread_a_obj;
cyg_handle_t thread_a_hdl;
//=========================================
//Function Prototype
//=========================================
static void usb_mount_test(void);
static cyg_uint32 lookup_usb(cyg_io_handle_t *handle_usb);
static cyg_bool usbdisk_mount(cyg_io_handle_t *handle_usb);
//=========================================
//Main Functions
//=========================================
void cyg_user_start(void)
{	
	int i=0;


	diag_printf("=== Prepare to test USB HOST. ===\n");

	// thread : usb disk mount
//	cyg_thread_create(
//			THREAD_PRIORITY,
//			(cyg_thread_entry_t*)usb_mount_test,
//			(cyg_addrword_t)0,
//			"USB Mount Thread",
//			(void *)thread_a_stack,
//			THREAD_STACK_SIZE,
//		    &thread_a_hdl,
//		    &thread_a_obj );
//		      
//	cyg_thread_resume(thread_a_hdl);
//	
//	cyg_scheduler_start();
	do
	{
		usb_mount_test();
		for(i=0; i<300;i++)	diag_printf(".");
	}while(true);
	
	diag_printf("\n");
}

static void usb_mount_test(void)
{
	cyg_io_handle_t handle_usb;
	char *buf = NULL;
    int ret = 0;
	cyg_uint32 mlunlen=0;
	cyg_bool disk_mounted = false;


	diag_printf("THREAD START!\n");

	// lookup main usb and return max lun number
 	mlunlen = lookup_usb(&handle_usb);	
    
	if( mlunlen == 0xFF)
	{
		diag_printf("Could not get max lun Error: %x.\n", mlunlen);
	}
	else
	{
		// mount
    	diag_printf("Prepare to mount USB disk\n");
    	
		disk_mounted = usbdisk_mount(&handle_usb);		
	}
	

FINISHED:    
    diag_printf("THREAD FINISHED.\n");
//Getch();
}


static cyg_uint32 lookup_usb(cyg_io_handle_t *handle_usb)
{
	Cyg_ErrNo err = ENOERR;
	cyg_uint32 mlen=0;


	diag_printf("cyg_io_lookup prepare\n");
	//err = cyg_io_lookup(CYGDAT_DEVS_DISK_USB_DISK_NAME, handle_usb);
	err = cyg_io_lookup("/dev/usbblk0/", handle_usb);
	
	if (err != ENOERR)
	{
		diag_printf("lookup fails: err: %d\n", err);
		Getch();
	}
	else
	{
		diag_printf("cyg_io_get_config - get max lun.\n");

		//Get max lun from device
		err = usb_get_max_lun(handle_usb, &mlen);
		
		diag_printf("Max Lun: %x.\n", mlen);  
	} 	
	
	return mlen;
}


static cyg_bool usbdisk_mount(cyg_io_handle_t *handle_usb)
{
	Cyg_ErrNo err = ENOERR;
	cyg_uint8 disknum = 0;
    cyg_bool disk0_mounted, disk1_mounted = false;
    char string [50]={0};


	// set current lun for mounting
	// read/write from current lun 2 card
	disknum = 0;//2;
	
	usb_set_current_lun(handle_usb, disknum);

    diag_printf("Prepare to mount USB disk %d on partition 0.\n", disknum);
    	
	err = sprintf(string, "/dev/hd%d/", disknum);

	err = mount( string, "/", "fatfs" );
	
	if( err )
	{
		diag_printf(" %s: Mount0 fails! err = %d\n", string, err);
		
		diag_printf("Prepare to mount USB disk %d on partition 1.\n", disknum);
		
		err = sprintf(string, "/dev/hd%d/1", disknum);
		
		err = mount( string, "/", "fatfs" );
		
        if ( err )
        {
            diag_printf(" %s: Mount1 fails! err = %d\n", string, err);
            //Getch();
        }
        else
        {
            diag_printf(" %s: Mount1 success!\n", string);
            disk1_mounted = true;
            umount("/");
        }							
	}
	else
	{
		diag_printf(" %s : Mount0 success!\n", string);
        disk0_mounted = true;
        umount("/");
	}
	
	if(disk0_mounted || disk1_mounted)
		diag_printf("disk %d is mounted!.\n", disknum);
	else
		diag_printf(".....");
		
	return (disk0_mounted || disk1_mounted);	
}
