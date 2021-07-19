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
#include <network.h>                    /* InitAllNetWorkInterfaces() */

#include "../files/rc_mount.h"

#include "nxThread.h"



//static void MountDisk();

extern void ecos_nx_init(CYG_ADDRWORD data);
extern void nanox_thread(CYG_ADDRWORD data);
//extern void nanowm_thread(CYG_ADDRWORD data);

extern void DpfDesktopThread(CYG_ADDRWORD data);
//extern void Aplayer_thread(CYG_ADDRWORD data);
//extern void Reader_thread(CYG_ADDRWORD data);
//extern void photo_thread(CYG_ADDRWORD data);
//extern void photo_test_thread(CYG_ADDRWORD data);
//
//extern void Clock_thread(CYG_ADDRWORD data);
//extern void Setting_thread(CYG_ADDRWORD data);
//extern void FB_thread(CYG_ADDRWORD data);


static void StartupThread(CYG_ADDRWORD data);


//#define SHOW_RESULT( _fn, _res ) \
//  diag_printf("<FAIL>: " #_fn "() returned %d %s\n", (int)_res, _res<0?strerror(errno):"");
//
//#define CHKFAIL_TYPE( _fn, _res, _type ) { \
//if ( _res != -1 ) \
//  diag_printf("<FAIL>: " #_fn "() returned %d (expected -1)\n", (int)_res); \
//else if ( errno != _type ) \
//    diag_printf("<FAIL>: " #_fn "() failed with errno %d (%s),\n    expected %d (%s)\n", errno, strerror(errno), _type, strerror(_type) ); \
//}

//==========================================================================
//==========================================================================

cyg_ucount32 dpfgNanoxDataIndex;
static struct tNxThread gThreads[] = {
    { "System startup", StartupThread,    11 },

    { "Nano-X server",  nanox_thread,      12 },    
    
    //{ "Nano-WM",        nanowm_thread,     15 },

    //{ "Application",       photo_test_thread,      17 },    
    { "Application",       DpfDesktopThread,      16 },
  

};
#define NUM(x) (sizeof(x)/sizeof(x[0]))

// Functions not provided in eCos by standard...
//char *
//strdup(char *string) {
//    char *newbit = malloc(strlen(string)+1);
//    strcpy(newbit,string);
//    return newbit;
//}

//int 
//gettimeofday(struct timeval *tv, struct timezone *tz)
//{
//     tv->tv_usec = 0;
//     tv->tv_sec = time(NULL);
//     return(0);
//}

int gettimeofday(struct timeval* tv, struct timezone* tz)
{

    //int ticks_per_second = 1000000000/
	//    (CYGNUM_HAL_RTC_NUMERATOR/CYGNUM_HAL_RTC_DENOMINATOR);
    //
	//cyg_tick_count_t cur_time = cyg_current_time();
    //
	//int tix = cur_time % ticks_per_second;
    //
	//tv->tv_sec = cur_time / ticks_per_second;
    //
	//tv->tv_usec = (tix * 1000000)/ticks_per_second;
	
	//ecos
	tv->tv_usec = 0;
    tv->tv_sec = time(NULL);

	return 0;

}

//int
//strcasecmp(const char *s1, const char *s2)
//{
//    char c1, c2;
//
//    while ((c1 = tolower(*s1++)) == (c2 = tolower(*s2++)))
//        if (c1 == 0)
//            return (0);
//    return ((unsigned char)c1 - (unsigned char)c2);
//}
void
InitAllNetWorkInterfaces_(void)
{
   /* static volatile int in_InitAllNetWorkInterfaces = 0;
    cyg_scheduler_lock();
    while ( in_InitAllNetWorkInterfaces ) {
        // Another thread is doing this...
        cyg_scheduler_unlock();
        cyg_thread_delay( 10 );
        cyg_scheduler_lock();
    }
    in_InitAllNetWorkInterfaces = 1;
    cyg_scheduler_unlock();*/

#ifdef CYGPKG_NET_NLOOP
#if 0 < CYGPKG_NET_NLOOP
    {
        static int loop_init = 0;
        int i;
        if ( 0 == loop_init++ )
            for ( i = 0; i < CYGPKG_NET_NLOOP; i++ )
                init_loopback_interface( i );
    }
#endif
#endif


    // Open the monitor to other threads.
  //  in_InitAllNetWorkInterfaces = 0;

}

//==========================================================================

#ifndef CYGINT_ISO_STRING_STRFUNCS

char *strcat( char *s1, const char *s2 )
{
    char *s = s1;
    while( *s1 ) s1++;
    while( (*s1++ = *s2++) != 0);
    return s;
}

#endif

//==========================================================================

//static void listdir_( char *name, int statp )
//{
//    int err;
//    DIR *dirp;
//    
//    diag_printf("<INFO>: reading directory %s\n",name);
//    
//    dirp = opendir( name );
//    if( dirp == NULL ) SHOW_RESULT( opendir, -1 );
//
//    for(;;)
//    {
//        struct dirent *entry = readdir( dirp );
//        
//        if( entry == NULL )
//            break;
//
//        diag_printf("<INFO>: entry %14s",entry->d_name);
//        if( statp )
//        {
//            char fullname[PATH_MAX];
//            struct stat sbuf;
//
//            if( name[0] )
//            {
//                strcpy(fullname, name );
//                if( !(name[0] == '/' && name[1] == 0 ) )
//                    strcat(fullname, "/" );
//            }
//            else fullname[0] = 0;
//            
//            strcat(fullname, entry->d_name );
//            
//            err = stat( fullname, &sbuf );
//            if( err < 0 )
//            {
//                if( errno == ENOSYS )
//                    diag_printf(" <no status available>");
//                else SHOW_RESULT( stat, err );
//            }
//            else
//            {
//                diag_printf(" [mode %08x ino %08x nlink %d size %ld]",
//                            sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,sbuf.st_size);
//            }
//        }
//
//        diag_printf("\n");
//    }
//
//    err = closedir( dirp );
//    if( err < 0 ) SHOW_RESULT( stat, err );
//}

//static void MountDisk()
//{
//    int err;
// 
//#ifdef USE_HARDDISK
//    err = mount("/dev/hda/1", "/", "fatfs");
//    if (err < 0) {
//        printf("mount err\n");
//        exit(-1);
//    }
//    err = chdir("/");
//    if (err < 0) {
//        printf("chdir err\n");
//        exit(-1);
//    }
//    printf("mount ide disk ok!\n");
//#endif
//
//#ifdef USE_SDDISK
// err = mount( "/dev/sdblk0/", "/", "fatfs" );
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
// #endif    
//    //#ifdef USE_ROMDISK
////    {
////        char ROM_fs[32];
////        int res;
////
////        printf("Mount ROM file system\n");
////#ifdef CYGPKG_HAL_ARM_SA11X0_IPAQ
////        // Work around hardware anomaly which causes major screen flicker
////        {
////            char *hold_rom_fs;
////            if ((hold_rom_fs = malloc(0x80080)) != 0) {
////                // Note: ROM fs requires 32 byte alignment
////                hold_rom_fs = (char *)(((unsigned long)hold_rom_fs + 31) & ~31);
////                memcpy(hold_rom_fs, 0x50F00000, 0x80000);
////                sprintf(ROM_fs, "0x%08x", hold_rom_fs);
////            } else {
////                printf("Can't allocate memory to hold ROM fs!\n");
////            }
////        }
////#else
////        sprintf(ROM_fs, "0x%08x", 0x50F00000);
////        sprintf(ROM_fs, "0x%08x", 0x61F00000);
////#endif
////        printf("ROM fs at %s\n", ROM_fs);
////        if ((res = mount(ROM_fs, "/", "romfs")) < 0) {
////            printf("... failed\n");
////        }
////        chdir("/");
////    }
////#endif
////
////#ifdef USE_JFFS2
////    {
////        int res;
////        printf("... Mounting JFFS2 on \"/\"\n");
////        res = mount( CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1, "/", "jffs2" );
////        if (res < 0) {
////            printf("Mount \"/\" failed - res: %d\n", res);
////        }
////        chdir("/");
////    }   
////#endif
//     //diag_printf("<INFO>: ROMFS root follows\n");
//    //listdir_( "/", true );
//    //
//    //diag_printf("<INFO>: ROMFS list of /ico follows\n");
//    //listdir_( "/pic", true );
//    //
//    //diag_printf("<INFO>: ROMFS list of /pic follows\n");
//    //listdir_( "/resource", true );
//    //
//    //diag_printf("<INFO>: ROMFS list of /pic follows\n");
//    //listdir_( "/resource/800x480", true );
//    
//}
#if 0
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
#else
static void 
StartupThread(CYG_ADDRESS data)
{
    int i;
    struct tNxThread *nx = NULL;

    printf("SYSTEM INITIALIZATION in progress\n");
    printf("NETWORK:\n");
    InitAllNetWorkInterfaces_();
    // init_all_network_interfaces();
    
    printf("MOUNT DISK\n");
    MountDisk();
    printf("MOUNT DISK end\n");

    // Allocate a free thread data slot
    // Note: all MicroWindows/NanoX threads use this slot for NanoX-private
    // data.  That's why there is only one call here.
    dpfgNanoxDataIndex = cyg_thread_new_data_index();
    printf("data index = %d\n", dpfgNanoxDataIndex);

    printf("Creating system threads\n");
    nx = &gThreads[1];
    for (i = 1;  i < NUM(gThreads);  i++, nx++) {
        cyg_thread_create(nx->prio,
                          nx->entry,
                          (cyg_addrword_t) dpfgNanoxDataIndex,
                          nx->name,
                          (void *)nx->stack, STACKSIZE,
                          &nx->t,
                          &nx->t_obj);
    }

    printf("Starting threads\n");
    nx = &gThreads[1];
    for (i = 1;  i < NUM(gThreads);  i++, nx++) {
        printf("Starting %s\n", nx->name);
        cyg_thread_resume(nx->t);
        // Special case - run additional code, specific to this environment
        // only after the server has had a chance to startup
        //if (i == 2) {
        //    ecos_nx_init(dpfgNanoxDataIndex);
        //}
    }

    printf("SYSTEM THREADS STARTED!\n");
}
#endif
void cyg_user_start(void)
{
    struct tNxThread *nx;

    printf("cyg_user_start!\n");
    nx = &gThreads[0];
    cyg_thread_create(nx->prio,
                      nx->entry,
                      (cyg_addrword_t) 0,
                      nx->name,
                      (void *)nx->stack, CYGNUM_HAL_STACK_SIZE_TYPICAL,
                      &nx->t,
                      &nx->t_obj);
    cyg_thread_resume(nx->t);
}
