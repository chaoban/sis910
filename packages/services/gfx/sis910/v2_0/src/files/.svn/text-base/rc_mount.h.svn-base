#ifndef _RC_MOUNT_H
#define _RC_MOUNT_H

#include <pkgconf/system.h>     /* which packages are enabled/disabled */
#include <dirent.h>

#ifdef CYGPKG_FS_ROM
#define USE_ROMDISK
#ifdef CYGPKG_HAL_R16
#include "resource_r16.h"
#else
#include "resource.h"
#endif
#endif

#ifdef CYGPKG_FS_JFFS2
#include <pkgconf/io_flash.h>
#define USE_JFFS2
#undef  USE_ROMDISK
#endif

#ifdef CYGPKG_DEVS_DISK_IDE
#define USE_HARDDISK
#endif
 
#ifdef CYGPKG_DEVS_DISK_SD_SIS910
#define  USE_SDDISK
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>
#endif



#ifdef USE_ROMDISK
MTAB_ENTRY( romfs_mte1,
                   "/romdisk",
                   "romfs",
                   "",
                   (CYG_ADDRWORD) &filedata[0] );
#endif


#define SHOW_RESULT( _fn, _res ) \
  diag_printf("<FAIL>: " #_fn "() returned %d %s\n", (int)_res, _res<0?strerror(errno):"");

#define CHKFAIL_TYPE( _fn, _res, _type ) { \
if ( _res != -1 ) \
  diag_printf("<FAIL>: " #_fn "() returned %d (expected -1)\n", (int)_res); \
else if ( errno != _type ) \
    diag_printf("<FAIL>: " #_fn "() failed with errno %d (%s),\n    expected %d (%s)\n", errno, strerror(errno), _type, strerror(_type) ); \
}




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
    
    //listdir_( "/romdisk", true ); 
#ifdef USE_HARDDISK
    err = mount("/dev/hda/1", "/hd", "fatfs");
    if (err < 0) {
        printf("mount err\n");
        //exit(-1);
    }
    err = chdir("/hd");
    if (err < 0) {
        printf("chdir err\n");
        //exit(-1);
    }
    //printf("mount ide disk ok!\n");    
#endif
    

//#ifdef USE_SDDISK
//    err = mount( "/dev/sdblk0/", "/sd", "fatfs" );
//    if (err)
//    {
//        diag_printf(" Mount fails!; err = %d\n", err);
//        diag_printf("Prepare to mount SD disk on partition 1.\n");
//        mount( "/dev/sdblk0/1", "/sd", "fatfs" );
//        if (err)
//        {
//            diag_printf(" SD Mount fails!\n");
//        }
//    }
//    else
//    {
//        diag_printf(" SD Mount success!\n");
//    }
//    //listdir_( "/sd/pic_sd", true );
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


#endif //_RC_MOUNT_H
