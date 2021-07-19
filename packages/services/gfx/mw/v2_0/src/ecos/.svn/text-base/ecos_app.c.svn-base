/* 
 * Written 1999-03-19 by Jonathan Larmour, Cygnus Solutions
 * Modifed for touchscreen testing by Richard Panton 13-09-00
 * This file is in the public domain and may be used for any purpose
 */

/* CONFIGURATION CHECKS */

#include <pkgconf/system.h>     /* which packages are enabled/disabled */
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif
#ifdef CYGPKG_LIBC
# include <pkgconf/libc.h>
#endif
#ifdef CYGPKG_IO_SERIAL
# include <pkgconf/io_serial.h>
#endif
#ifdef CYGPKG_FS_ROM
#define USE_ROMDISK
#endif
#ifdef CYGPKG_FS_JFFS2
# include <pkgconf/io_flash.h>
#define USE_JFFS2
#undef  USE_ROMDISK
#endif

#ifdef CYGPKG_DEVS_DISK_IDE
#define USE_HARDDISK
#endif
 
#ifdef CYGPKG_DEVS_DISK_SD_SIS910
#define  USE_SDDISK
#endif
 
#ifndef CYGFUN_KERNEL_API_C
# error Kernel API must be enabled to build this application
#endif

#ifndef CYGPKG_LIBC_STDIO
# error C library standard I/O must be enabled to build this application
#endif

#ifndef CYGPKG_IO_SERIAL_HALDIAG
# error I/O HALDIAG pseudo-device driver must be enabled to build this application
#endif

/* INCLUDES */

#include <stdio.h>                      /* printf */
#include <stdlib.h>                      /* printf */
#include <string.h>                     /* strlen */
#include <ctype.h>                      /* tolower */
#include <cyg/kernel/kapi.h>            /* All the kernel specific stuff */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <sys/time.h>
#include <network.h>                    /* init_all_network_interfaces() */
#include <dirent.h>
#define STACKSIZE ( 65536 )


#ifdef USE_ROMDISK
#ifdef CYGPKG_HAL_R16
#include "image_r16.h"
#else
#include "image.h"
#endif
#endif

#ifdef USE_SDDISK
// To run SD test
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>
#endif

static void MountDisk();

extern void ecos_nx_init(CYG_ADDRWORD data);
extern void nanowm_thread(CYG_ADDRWORD data);
extern void nanox_thread(CYG_ADDRWORD data);
extern void imageview_thread(CYG_ADDRWORD data);
extern void clock_thread(CYG_ADDRWORD data);
extern void slideshow_thread(CYG_ADDRWORD data);


#ifdef USE_NXKBD
extern void nxkbd_thread(CYG_ADDRWORD data);
#endif
#ifdef USE_NXSCRIBBLE
extern void nxscribble_thread(CYG_ADDRWORD data);
#endif
#ifdef USE_LANDMINE
extern void landmine_thread(CYG_ADDRWORD data);
#endif
#ifdef USE_NTETRIS
extern void ntetris_thread(CYG_ADDRWORD data);
#endif
#ifdef USE_WORLD
extern void world_thread(CYG_ADDRWORD data);
#endif
#ifdef USE_IMG_DEMO
extern void img_demo_thread(CYG_ADDRWORD data);
#endif
static void startup_thread(CYG_ADDRWORD data);
//==========================================================================

//#ifdef USE_ROMDISK
//MTAB_ENTRY( romfs_mte1,
//                   "/",
//                   "romfs",
//                   "",
//                   (CYG_ADDRWORD) &filedata[0] );
//#endif

//#ifdef USE_ROMDISK
//MTAB_ENTRY( romfs_mte1,
//                   "/",
//                   "romfs",
//                   "",
//                   (CYG_ADDRWORD) 0x80120000);
//#endif 
   
////==========================================================================


typedef void fun(CYG_ADDRWORD);
struct nx_thread {
    char         *name;
    fun          *entry;
    int          prio;
    cyg_handle_t t;
    cyg_thread   t_obj;
    char         stack[STACKSIZE];
};

struct nx_thread _threads[] = {
    { "System startup", startup_thread,    11 },

    { "Nano-X server",  nanox_thread,      12 },

    //{ "Nano-WM",        nanowm_thread,     15 },
    //{ "image view",        imageview_thread,     14 },
    //{ "clock Show",        clock_thread,     13 },
    { "slide show",        slideshow_thread,     14 },
#ifdef USE_NXKBD
    { "Nano-KBD",       nxkbd_thread,      13 },
#endif    
#ifdef USE_IMG_DEMO
    { "Image demo",     img_demo_thread,   20 },
#endif
#ifdef USE_NXSCRIBBLE
    { "Scribble",       nxscribble_thread, 20 },
#endif
#ifdef USE_LANDMINE
    { "Landmine",       landmine_thread,   19 },
#endif
#ifdef USE_NTETRIS
    { "Nano-Tetris",    ntetris_thread,    18 },
#endif
#ifdef USE_WORLD
    { "World Map",      world_thread,      21 },
#endif
};
#define NUM(x) (sizeof(x)/sizeof(x[0]))
             
// Functions not provided in eCos by standard...
char *
strdup(char *string) {
    char *newbit = malloc(strlen(string)+1);
    strcpy(newbit,string);
    return newbit;
}

int 
gettimeofday(struct timeval *tv,
             struct timezone *tz)
{


        /*int ticks_per_second = 1000000000/
	    (CYGNUM_HAL_RTC_NUMERATOR/CYGNUM_HAL_RTC_DENOMINATOR);

	cyg_tick_count_t cur_time = cyg_current_time();
	//cyg_tick_count_t cur_time = 1000;

	int tix = cur_time % ticks_per_second;

	tv->tv_sec = cur_time / ticks_per_second;

	tv->tv_usec = (tix * 1000000)/ticks_per_second;

	return 0;*/
	
    tv->tv_usec = 0;
    tv->tv_sec = time(NULL);
    return(0);
}

int
strcasecmp(const char *s1, const char *s2)
{
    char c1, c2;

    while ((c1 = tolower(*s1++)) == (c2 = tolower(*s2++)))
        if (c1 == 0)
            return (0);
    return ((unsigned char)c1 - (unsigned char)c2);
}

#define SHOW_RESULT( _fn, _res ) \
  diag_printf("<FAIL>: " #_fn "() returned %d %s\n", (int)_res, _res<0?strerror(errno):"");
static void listdir_( char *name, int statp )
{
    int err;
    DIR *dirp;
    
    diag_printf("<INFO>: reading directory %s\n",name);
    
    dirp = opendir( name );
    if( dirp == NULL ) SHOW_RESULT( opendir, -1 );

    for(;;)
    {
        struct dirent *entry = readdir( dirp );
        
        if( entry == NULL )
            break;

        diag_printf("<INFO>: entry %14s",entry->d_name);
        if( statp )
        {
            char fullname[PATH_MAX];
            struct stat sbuf;

            if( name[0] )
            {
                strcpy(fullname, name );
                if( !(name[0] == '/' && name[1] == 0 ) )
                    strcat(fullname, "/" );
            }
            else fullname[0] = 0;
            
            strcat(fullname, entry->d_name );
            diag_printf("file: %s\n", fullname);
            
            err = stat( fullname, &sbuf );
            if( err < 0 )
            {
                if( errno == ENOSYS )
                    diag_printf(" <no status available>");
                else SHOW_RESULT( stat, err );
            }
            else
            {
                diag_printf(" [mode %08x ino %08x nlink %d size %ld]",
                            sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,sbuf.st_size);
            }
        }

        diag_printf("\n");
    }

    err = closedir( dirp );
    if( err < 0 ) SHOW_RESULT( stat, err );
}

static void MountDisk()
{
    int err;
 
 #ifdef USE_HARDDISK
    err = mount("/dev/hda/1", "/", "fatfs");
    if (err < 0) {
        printf("mount err\n");
        exit(-1);
    }
    err = chdir("/");
    if (err < 0) {
        printf("chdir err\n");
        exit(-1);
    }
    printf("mount ide disk ok!\n");
#endif

//#ifdef USE_SDDISK                                                 
//    err = mount( "/dev/sdblk0/", "/", "fatfs" );                  
//    if (err)                                                          
//    {                                                             
//        diag_printf(" Mount fails!; err = %d\n", err);                
//        diag_printf("Prepare to mount SD disk on partition 1.\n");
//        mount( "/dev/sdblk0/1", "/", "fatfs" );                       
//        if (err)                                                      
//        {                                                             
//            diag_printf(" Mount fails!\n");                           
//        }                                                             
//    }                                                                 
//    else                                                          
//    {                                                                 
//        diag_printf(" Mount success!\n");                             
//    }
//    
//    {
//        int fd, id;
//	struct stat s;
//	void *buffer = NULL;
//	//buffer_t src;
//	char path[64]="/pic_sd/1.bmp";
//  //printf("GdLoadImageFromFile: open image: %s\n", path);
//	//fd = open("/pic_sd/1.bmp", O_RDONLY);
//	//if (fd < 0 || fstat(fd, &s) < 0) {
//	//	EPRINTF("GdLoadImageFromFile: can't open image: %s\n", path);
//	//	return 0;
//	//}
//	 fd = open("/pic_sd/1.bmp", O_RDONLY);
//     if( fd < 0 || fstat(fd, &s) < 0)
//       {
//            diag_printf("open in error %d\n",fd);
//            return 0;
//        }
//    }                                                                
//#endif
//#ifdef USE_SDDISK
//    err = mount( "/dev/sdblk0/", "/", "fatfs" );
//    if (err)
//    {
//        diag_printf(" Mount fails!; err = %d\n", err);
//        diag_printf("Prepare to mount SD disk on partition 1.\n");
//        mount( "/dev/sdblk0/1", "/", "fatfs" );
//        if (err)
//        {
//            diag_printf(" Mount fails!\n");
//        }
//    }
//    else
//    {
//        diag_printf(" Mount success!\n");
//    }
//#endif    
//#ifdef USE_ROMDISK
//    {
//        char ROM_fs[32];
//        int res;
//
//        printf("Mount ROM file system\n");
//#ifdef CYGPKG_HAL_ARM_SA11X0_IPAQ
//        // Work around hardware anomaly which causes major screen flicker
//        {
//            char *hold_rom_fs;
//            if ((hold_rom_fs = malloc(0x80080)) != 0) {
//                // Note: ROM fs requires 32 byte alignment
//                hold_rom_fs = (char *)(((unsigned long)hold_rom_fs + 31) & ~31);
//                memcpy(hold_rom_fs, 0x50F00000, 0x80000);
//                sprintf(ROM_fs, "0x%08x", hold_rom_fs);
//            } else {
//                printf("Can't allocate memory to hold ROM fs!\n");
//            }
//        }
//#else
//        sprintf(ROM_fs, "0x%08x", 0x50F00000);
//        sprintf(ROM_fs, "0x%08x", 0x61F00000);
//#endif
//        printf("ROM fs at %s\n", ROM_fs);
//        if ((res = mount(ROM_fs, "/", "romfs")) < 0) {
//            printf("... failed\n");
//        }
//        chdir("/");
//    }
//#endif
//
//#ifdef USE_JFFS2
//    {
//        int res;
//        printf("... Mounting JFFS2 on \"/\"\n");
//        res = mount( CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1, "/", "jffs2" );
//        if (res < 0) {
//            printf("Mount \"/\" failed - res: %d\n", res);
//        }
//        chdir("/");
//    }   
//#endif
     //diag_printf("<INFO>: ROMFS root follows\n");
    //listdir_( "/", true );
    //
    //diag_printf("<INFO>: ROMFS list of /ico follows\n");
    //listdir_( "/pic", true );
    //
    //diag_printf("<INFO>: ROMFS list of /pic follows\n");
    //listdir_( "/resource", true );
    //
    //diag_printf("<INFO>: ROMFS list of /pic follows\n");
    //listdir_( "/resource/800x480", true );
    
}

static void 
startup_thread(CYG_ADDRESS data)
{
    cyg_ucount32 nanox_data_index;
    int i;
    struct nx_thread *nx = NULL;

    printf("SYSTEM INITIALIZATION in progress\n");
    printf("NETWORK:\n");
    init_all_network_interfaces();
    
    printf("MOUNT DISK\n");
    MountDisk();    

    // Allocate a free thread data slot
    // Note: all MicroWindows/NanoX threads use this slot for NanoX-private
    // data.  That's why there is only one call here.
    nanox_data_index = cyg_thread_new_data_index();
    printf("data index = %d\n", nanox_data_index);

    printf("Creating system threads\n");
    nx = &_threads[1];
    for (i = 1;  i < NUM(_threads);  i++, nx++) {
        cyg_thread_create(nx->prio,
                          nx->entry,
                          (cyg_addrword_t) nanox_data_index,
                          nx->name,
                          (void *)nx->stack, STACKSIZE,
                          &nx->t,
                          &nx->t_obj);
    }

    printf("Starting threads\n");
    nx = &_threads[1];
    for (i = 1;  i < NUM(_threads);  i++, nx++) {
        printf("Starting %s\n", nx->name);
        cyg_thread_resume(nx->t);
        // Special case - run additional code, specific to this environment
        // only after the server has had a chance to startup
        if (i == 2) {
            ecos_nx_init(nanox_data_index);
        }
    }

    printf("SYSTEM THREADS STARTED!\n");
}

void cyg_user_start(void)
{
    struct nx_thread *nx;

    nx = &_threads[0];
    cyg_thread_create(nx->prio,
                      nx->entry,
                      (cyg_addrword_t) 0,
                      nx->name,
                      (void *)nx->stack, STACKSIZE,
                      &nx->t,
                      &nx->t_obj);
    cyg_thread_resume(nx->t);
}

