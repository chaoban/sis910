//==========================================================================
//
//      format.c
//
//      Provide a FAT format utility to format SD card
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2006 eCosCentric Limited
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author:       Sidney Shih
// Date:         2008-10-22
//
//####DESCRIPTIONEND####
//==========================================================================
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <cyg/io/disk.h>
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>
#include <cyg/fs/fatfs.h>
#include <cyg/fileio/fileio.h>

// SuperH is Big Endian
// Set CPU endianness and Macros
#define WORDS_BIGENDIAN
#include "endian.h"

// Definition for hard-coded FAT size caculation
#define MEGA                        (1024UL * 1024UL)
#define GIGA                        (1024UL * 1024UL * 1024UL)
#define DEFAULT_BLOCK_SIZE          512
#define NO_BLKS_8MB                 (( 8 * MEGA) / DEFAULT_BLOCK_SIZE)
#define NO_BLKS_64MB                ((64 * MEGA) / DEFAULT_BLOCK_SIZE)
#define NO_BLKS_1GB                 (( 1 * GIGA) / DEFAULT_BLOCK_SIZE)
#define NO_BLKS_2GB                 (( 2 * GIGA) / DEFAULT_BLOCK_SIZE)

#define FAT_TYPE_FAT12              12
#define FAT_TYPE_FAT16              16
#define FAT_TYPE_FAT32              32

#define PARTITION_FAT12             0x01
#define PARTITION_FAT16_SM          0x04
#define PARTITION_FAT16             0x06
#define PARTITION_FAT32             0x0B
#define PARTITION_FAT32_LBA         0x0C
#define PARTITION_FAT16_LBA         0x0E

// Definition of hard-coded CHS value caulation
#define DEFAULT_HEADS_NO            255
#define DEFAULT_SECTORS_PER_TRACK   63

#define MAX_CHS_CYLINDER            1021

// Definition of some magic numbers
#define DEFAULT_MEDIA_TYPE          0xF8
#define DEFAULT_DRIVER_NUMBER       0x80
#define FAT12_EOC                   0x0FFF
#define FAT16_EOC                   0xFFFF
#define FAT32_EOC                   0x0FFFFFFF
#define FAT12_MEDIA_TYPE_EOC        0x0FF8
#define FAT16_MEDIA_TYPE_EOC        0xFFF8
#define FAT32_MEDIA_TYPE_EOC        0x0FFFFFF8
#define FAT12_EOC_MAGIC             ((FAT12_EOC << 12) | FAT12_MEDIA_TYPE_EOC)
#define FAT16_EOC_MAGIC             ((FAT16_EOC << 16) | FAT16_MEDIA_TYPE_EOC)
#define FAT32_EOC_MAGIC_0           FAT32_MEDIA_TYPE_EOC
#define FAT32_EOC_MAGIC_1           FAT32_EOC

// Some signature values
#define MBR_SIGNATURE               0xAA55
#define BPB_SIGNATURE               0xAA55
#define EXTENDED_BOOT_SIGNATURE     0x29
#define FSINFO_SIGNATURE            0xAA550000
#define FSINFO_LEAD_SIGNATURE       0x41615252
#define FSINFO_STRUCT_SIGNATURE     0x61417272

// Some other default values
#define DEFAULT_OEM_NAME            "SiS     "
#define DEFAULT_VOLUME_ID           "SIS_DPF    "
#define DEFAULT_FAT12_RSVD_SEC_CNT  1
#define DEFAULT_FAT16_RSVD_SEC_CNT  DEFAULT_FAT12_RSVD_SEC_CNT
#define DEFAULT_FAT32_RSVD_SEC_CNT  32
#define DEFAULT_NUMBER_OF_FATS      2
#define DEFAULT_FAT12_ROOT_ENT_CNT  512
#define DEFAULT_FAT16_ROOT_ENT_CNT  DEFAULT_FAT12_ROOT_ENT_CNT
#define DEFAULT_FAT32_ROOT_ENT_CNT  0
#define DEFAULT_FAT32_ROOT_CLUSTER  2
#define DEFAULT_FAT32_FSINFO        1
#define DEFAULT_FAT32_BK_BOOT_SEC   6

// stolen from GNU libparted
static const char MBR_BOOT_CODE[] = {
    0xfa, 0xb8, 0x00, 0x10, 0x8e, 0xd0, 0xbc, 0x00,
    0xb0, 0xb8, 0x00, 0x00, 0x8e, 0xd8, 0x8e, 0xc0,
    0xfb, 0xbe, 0x00, 0x7c, 0xbf, 0x00, 0x06, 0xb9,
    0x00, 0x02, 0xf3, 0xa4, 0xea, 0x21, 0x06, 0x00,
    0x00, 0xbe, 0xbe, 0x07, 0x38, 0x04, 0x75, 0x0b,
    0x83, 0xc6, 0x10, 0x81, 0xfe, 0xfe, 0x07, 0x75,
    0xf3, 0xeb, 0x16, 0xb4, 0x02, 0xb0, 0x01, 0xbb,
    0x00, 0x7c, 0xb2, 0x80, 0x8a, 0x74, 0x01, 0x8b,
    0x4c, 0x02, 0xcd, 0x13, 0xea, 0x00, 0x7c, 0x00,
    0x00, 0xeb, 0xfe
};

// stolen from mkdosfs, by Dave Hudson
#define FAT_BOOT_MESSAGE    \
"This partition does not have an operating system loader installed on it.\n\r"\
"Press a key to reboot..."

#define FAT_BOOT_JUMP    "\xeb\x58\x90"    /* jmp+5a */

#define FAT_BOOT_CODE    "\x0e"        /* push cs */    \
    "\x1f"          /* pop ds */                        \
    "\xbe\x74\x7e"  /* mov si, offset message */        \
        /* write_msg_loop: */                           \
    "\xac"          /* lodsb */                         \
    "\x22\xc0"      /* and al, al */                    \
    "\x74\x06"      /* jz done (+8) */                  \
    "\xb4\x0e"      /* mov ah, 0x0e */                  \
    "\xcd\x10"      /* int 0x10 */                      \
    "\xeb\xf5"      /* jmp write_msg_loop */            \
        /* done: */                                     \
    "\xb4\x00"      /* mov ah, 0x00 */                  \
    "\xcd\x16"      /* int 0x16 */                      \
    "\xb4\x00"      /* mov ah, 0x00 */                  \
    "\xcd\x19"      /* int 0x19 */                      \
    "\xeb\xfe"      /* jmp +0 - in case int 0x19 */     \
        /* doesn't work */                              \
        /* message: */                                  \
    FAT_BOOT_MESSAGE

#define FAT_BOOT_CODE_LENGTH 128

typedef struct __attribute__ ((packed)) RawCHS {
    cyg_uint8   head;
    cyg_uint8   sector;
    cyg_uint8   cylinder;
} RawCHS;

typedef struct __attribute__ ((packed)) PartitionTable {
    cyg_uint8   boot_indicator;     // Boot indicator
    RawCHS      chs_start;
    cyg_uint8   partition_type;     // System ID
    RawCHS      chs_end;
    cyg_uint32  start;              // Starting sector counting from 0
    cyg_uint32  length;             // Number of sectors in partition
} PartitionTable;

typedef struct __attribute__ ((packed)) MasterBootRecord {
    cyg_uint8       boot_code[440];
    cyg_uint32      disk_signature;
    cyg_uint16      dummy;
    PartitionTable  partitions[4];
    cyg_uint16      mbr_signature;
} MasterBootRecord;

typedef struct __attribute__ ((packed)) FatBootSector {
    cyg_uint8   boot_jump[3];   // 00: Boot strap short or near jump
    cyg_uint8   system_id[8];   // 03: system name
    cyg_uint16  sector_size;    // 0b: bytes per logical sector
    cyg_uint8   cluster_size;   // 0d: sectors/cluster
    cyg_uint16  reserved;       // 0e: reserved sectors
    cyg_uint8   fats;           // 10: number of FATs
    cyg_uint16  dir_entries;    // 11: number of root directory entries
    cyg_uint16  sectors;        // 13: if 0, total_sect supersedes
    cyg_uint8   media;          // 15: media code
    cyg_uint16  fat_length;     // 16: sectors/FAT for FAT12/16
    cyg_uint16  secs_track;     // 18: sectors per track
    cyg_uint16  heads;          // 1a: number of heads
    cyg_uint32  hidden;         // 1c: hidden sectors (partition start)
    cyg_uint32  sector_count;   // 20: no. of sectors (if sectors == 0)

    union {
        // FAT12 & FAT16 fields
        struct __attribute__ ((packed)) {
            cyg_uint8   drive_num;          // 24:
            cyg_uint8   empty_1;            // 25:
            cyg_uint8   ext_signature;      // 26: always 0x29
            cyg_uint32  serial_number;      // 27:
            cyg_uint8   volume_name[11];    // 2b:
            cyg_uint8   fat_name[8];        // 36:
            cyg_uint8   boot_code[448];     // 3f: Boot code (or message)
        } fat16;
        // FAT32 fields
        struct __attribute__ ((packed)) {
            cyg_uint32  fat_length;         // 24: size of FAT in sectors
            cyg_uint16  flags;              // 28: bit8: fat mirroring, low4: active fat
            cyg_uint16  version;            // 2a: minor * 256 + major
            cyg_uint32  root_dir_cluster;   // 2c:
            cyg_uint16  info_sector;        // 30:
            cyg_uint16  backup_sector;      // 32:
            cyg_uint8   empty[12];          // 34:
            cyg_uint8   drive_num;          // 40:
            cyg_uint8   empty_1;            // 41:
            cyg_uint8   ext_signature;      // 42: always 0x29
            cyg_uint32  serial_number;      // 43:
            cyg_uint8   volume_name[11];    // 47:
            cyg_uint8   fat_name[8];        // 52:
            cyg_uint8   boot_code[420];     // 5a: Boot code (or message)
        } fat32;
    } u;

    cyg_uint16  boot_sign;      // 1fe: always 0xAA55
} FatBootSector;

typedef struct __attribute__ ((packed)) FatInfoSector {
    cyg_uint32  signature_1;    // should be 0x41615252
    cyg_uint8   unused[480];
    cyg_uint32  signature_2;    // should be 0x61417272
    cyg_uint32  free_clusters;
    cyg_uint32  next_cluster;   // most recently allocated cluster
    cyg_uint8   unused2[12];
    cyg_uint32  signature_3;    // should be 0xAA55
} FatInfoSector;

typedef struct __attribute__ ((packed)) FatDirEntry {
    cyg_uint8   name[8];
    cyg_uint8   extension[3];
    cyg_uint8   attributes;
    cyg_uint8   is_upper_case_name;
    cyg_uint8   creation_time_low;
    cyg_uint16  creation_time_high;
    cyg_uint16  creation_date;
    cyg_uint16  access_date;
    cyg_uint16  first_cluster_high;
    cyg_uint16  time;
    cyg_uint16  date;
    cyg_uint16  first_cluster;
    cyg_uint16  length;
} FatDirEntry;

typedef struct __attribute__ ((packed)) fs_info_t {
    cyg_uint32          blocks_num;         // number of blocks on disk
    cyg_uint8           fat_type;
    cyg_uint8           cluster_size;
    cyg_uint32          fat_size;
} fs_info_t;

// Struct for private data carried by set_config/get_config
typedef struct cyg_mmc_disk_config_priv_t {
    cyg_uint32          erase_disk_start;
    cyg_uint32          erase_disk_end;
    cyg_bool            format_disk;
} cyg_mmc_disk_config_priv_t;

#define SD_CONFIG_DISK_FORMAT    0x55AA0000
#define SD_CONFIG_DISK_ERASE     0x55AA0001

// stolen from GNU libparted
static void
sector_to_chs(cyg_uint32 sector, RawCHS* chs)
{
    cyg_uint32  real_c, real_h, real_s;

    real_c = sector / (DEFAULT_HEADS_NO * DEFAULT_SECTORS_PER_TRACK);
    real_h = (sector / DEFAULT_SECTORS_PER_TRACK) % DEFAULT_HEADS_NO;
    real_s = sector % DEFAULT_SECTORS_PER_TRACK;

    if (real_c > MAX_CHS_CYLINDER) {
    real_c = 1023;
    real_h = DEFAULT_HEADS_NO - 1;
    real_s = DEFAULT_SECTORS_PER_TRACK - 1;
    }

    chs->cylinder = real_c % 0x100;
    chs->head = real_h;
    chs->sector = real_s + 1 + (real_c >> 8 << 6);
}

// This is the Microsoft calculation from FATGEN
static void
calc_fat_size(cyg_uint8 fat_type, cyg_uint32 DskSize, cyg_uint32 SecPerClus, cyg_uint32 *FatSz)
{
    cyg_uint32 NumFATs = DEFAULT_NUMBER_OF_FATS;
    cyg_uint32 RootDirSectors;
    cyg_uint32 ReservedSecCnt;
    cyg_uint32 TmpVal1, TmpVal2;

    if (fat_type == FAT_TYPE_FAT32)
    {
        RootDirSectors = 0;
        ReservedSecCnt = DEFAULT_FAT32_RSVD_SEC_CNT;
    }
    else
    {
        // RootDirSectors = ((RootEntCnt * 32) + (BytesPerSec - 1)) / BytesPerSec;
        // RootEntCnt is 512 and BytesPerSec is 512
        RootDirSectors = 32;
        ReservedSecCnt = DEFAULT_FAT16_RSVD_SEC_CNT;
    }

    TmpVal1 = DskSize - (ReservedSecCnt + RootDirSectors);
    TmpVal2 = (256 * SecPerClus) + NumFATs;
    if (fat_type == FAT_TYPE_FAT32)
    {
        TmpVal2 /= 2;
        *FatSz = (TmpVal1 + (TmpVal2 - 1)) / TmpVal2;
    }
    else
    {
        *FatSz = ((TmpVal1 + (TmpVal2 - 1)) / TmpVal2) & 0xFFFF;
    }
}

Cyg_ErrNo
erase_disk(cyg_io_handle_t handle, fs_info_t *fs_info)
{
    cyg_mmc_disk_config_priv_t disk_config_priv = {0};
    cyg_uint32 len = sizeof(cyg_mmc_disk_config_priv_t);
    Cyg_ErrNo err;

    // Erase start sector
    disk_config_priv.erase_disk_start = 0;
    // Erase end sector
    disk_config_priv.erase_disk_end = fs_info->blocks_num - 1;

    // XXX: Special hack to erase sectors
    err = cyg_io_set_config(handle, SD_CONFIG_DISK_ERASE, &disk_config_priv, &len);
    
    return err;
}

Cyg_ErrNo
get_disk_geometry(cyg_io_handle_t handle, fs_info_t *fs_info)
{
    cyg_disk_info_t disk_info = {0};
    cyg_uint32 len = sizeof(cyg_disk_info_t);
    Cyg_ErrNo err;

    // Get disk the geometry
    err = cyg_io_get_config(handle, CYG_IO_GET_CONFIG_DISK_INFO, &disk_info, &len);
    if (err != ENOERR)
    {
        diag_printf("err = %d\n", err);
        return -EIO;
    }

    fs_info->blocks_num = disk_info.blocks_num;
    return ENOERR;
}

static void
calc_param(fs_info_t *fs_info)
{
    cyg_uint32 blocks_num = fs_info->blocks_num;
    cyg_uint8 fat_type;
    cyg_uint8 cluster_size;
    cyg_uint32 fat_size;

    // Calculate the FAT type
    // Table lookup (SD Spec. / Part 2. File System Spec. Table C-2)
    if (blocks_num <= NO_BLKS_64MB)
    {
        fat_type = FAT_TYPE_FAT12;
    }
    else if (blocks_num <= NO_BLKS_2GB)
    {
        fat_type = FAT_TYPE_FAT16;
    }
    else
    {
        fat_type = FAT_TYPE_FAT32;
    }

    // Table lookup (SD Spec. / Part 2. File System Spec. Table C-1)
    if (blocks_num <= NO_BLKS_8MB)
    {
        cluster_size = 16;
    }
    else if (blocks_num <= NO_BLKS_1GB)
    {
        cluster_size = 32;
    }
    else
    {
        cluster_size = 64;
    }

    // Caculate the FAT table size
    calc_fat_size(fat_type, blocks_num, cluster_size, &fat_size);

    fs_info->fat_type = fat_type;
    fs_info->cluster_size = cluster_size;
    fs_info->fat_size = fat_size;
}

static void
create_tables(cyg_io_handle_t handle, fs_info_t *fs_info)
{
    // Partition position parameters
    cyg_uint32 part_start_sector;
    cyg_uint32 part_end_sector;
    cyg_uint32 part_sectors;
    // FAT related parameters
    cyg_uint32 blocks_num = fs_info->blocks_num;
    cyg_uint8 fat_type = fs_info->fat_type;
    cyg_uint8 cluster_size = fs_info->cluster_size;
    cyg_uint32 fat_size = fs_info->fat_size;
    // Tables for writing to disk
    MasterBootRecord mbr = {{0}};
    FatBootSector fat_table = {{0}};
    FatInfoSector fat_info_table = {0};
    cyg_uint8 eoc_mark[DEFAULT_BLOCK_SIZE] = {0};
    cyg_uint8 root_dir_block[DEFAULT_BLOCK_SIZE] = {0};
    MasterBootRecord *mbr_table = &mbr;
    FatBootSector *bs = &fat_table;
    FatInfoSector *is = &fat_info_table;
    PartitionTable *part = &mbr_table->partitions[0];
    RawCHS chs_start = {0}, chs_end = {0};
    cyg_uint8 empty[512] = {0};
    // Parameters for data writing position
    cyg_uint32 reserved_region_sectors;
    cyg_uint32 fat_region_sectors;
    cyg_uint32 root_directories_sectors;
    // Others
    cyg_uint32 len = 1;     // One sector for writing
    Cyg_ErrNo err;
    cyg_uint32 count;
    cyg_uint32 empty_len;
    cyg_uint32 pos;
    
    // Setup MBR (Cluster 0, Sector 0)
    memset(mbr_table, 0, DEFAULT_BLOCK_SIZE);
    memcpy(mbr_table, MBR_BOOT_CODE, sizeof(MBR_BOOT_CODE));
    mbr_table->disk_signature = PED_CPU_TO_LE32(0xFFFFFFFF);// XXX: It should be a UID
    mbr_table->mbr_signature = PED_CPU_TO_LE16(MBR_SIGNATURE);
    // Setup partition table
    part->boot_indicator = 0x0;                     // It's non-bootable
    // Caculate the sectors in partition
    part_start_sector = cluster_size;
    part_end_sector = blocks_num;
    part_sectors = part_end_sector - part_start_sector;
    // Fill the CHS value of start sector
    sector_to_chs(part_start_sector, &chs_start);   // Partition starts at cluster 1
    part->chs_start = chs_start;
    // Fill the CHS value of end sector
    sector_to_chs(part_end_sector, &chs_end);       // XXX: It hard-coded to last
    part->chs_end = chs_end;
    part->start = PED_CPU_TO_LE32(part_start_sector);// Partition starts at cluster 1
    part->length = PED_CPU_TO_LE32(part_sectors);
    // Fill the corresponding FAT type
    switch (fat_type) {
        case FAT_TYPE_FAT12:
            part->partition_type = PARTITION_FAT12;
            break;
        case FAT_TYPE_FAT16:
            part->partition_type = PARTITION_FAT16;
            break;
        case FAT_TYPE_FAT32:
        default:
            part->partition_type = PARTITION_FAT32;
    }

    // Setup BPB (Cluster 1, Sector 0 ~ 8)
    memset(bs, 0, DEFAULT_BLOCK_SIZE);
    memcpy(bs->boot_jump, FAT_BOOT_JUMP, 3);
    memcpy(bs->system_id, DEFAULT_OEM_NAME, 8);
    bs->sector_size = PED_CPU_TO_LE16(DEFAULT_BLOCK_SIZE);
    bs->cluster_size = cluster_size;
    bs->fats = DEFAULT_NUMBER_OF_FATS;
    bs->media = DEFAULT_MEDIA_TYPE;
    // No really CHS geometry, use recommend value
    bs->secs_track = PED_CPU_TO_LE16(DEFAULT_SECTORS_PER_TRACK);
    bs->heads = PED_CPU_TO_LE16(DEFAULT_HEADS_NO);
    bs->hidden = PED_CPU_TO_LE32(part_start_sector);// FAT valume starts from Cluster 1

    switch (fat_type) {
        case FAT_TYPE_FAT12:
            bs->reserved = PED_CPU_TO_LE16(DEFAULT_FAT12_RSVD_SEC_CNT);
            bs->dir_entries = PED_CPU_TO_LE16(DEFAULT_FAT12_ROOT_ENT_CNT);
            bs->sectors = PED_CPU_TO_LE16(part_sectors);
            bs->sector_count = PED_CPU_TO_LE32(0);
            bs->fat_length = PED_CPU_TO_LE16(fat_size);
            bs->u.fat16.drive_num = DEFAULT_DRIVER_NUMBER;
            bs->u.fat16.ext_signature = EXTENDED_BOOT_SIGNATURE;
            // It should generated by current time
            bs->u.fat16.serial_number = PED_CPU_TO_LE32(0x12345678);
            memcpy(bs->u.fat16.volume_name, DEFAULT_VOLUME_ID, 11);
            memcpy(bs->u.fat16.fat_name, "FAT12   ", 8);
            memcpy(bs->u.fat16.boot_code, FAT_BOOT_CODE, FAT_BOOT_CODE_LENGTH);
            break;
        case FAT_TYPE_FAT16:
            bs->reserved = PED_CPU_TO_LE16(DEFAULT_FAT16_RSVD_SEC_CNT);
            bs->dir_entries = PED_CPU_TO_LE16(DEFAULT_FAT16_ROOT_ENT_CNT);
            if (blocks_num >= 0x10000)
            {
                bs->sectors = PED_CPU_TO_LE16(0);
                bs->sector_count = PED_CPU_TO_LE32(part_sectors);
            }
            else
            {
                bs->sectors = PED_CPU_TO_LE16(part_sectors);
                bs->sector_count = PED_CPU_TO_LE32(0);
            }
            bs->fat_length = PED_CPU_TO_LE16(fat_size);
            bs->u.fat16.drive_num = DEFAULT_DRIVER_NUMBER;
            bs->u.fat16.ext_signature = EXTENDED_BOOT_SIGNATURE;
            // It should generated by current time
            bs->u.fat16.serial_number = PED_CPU_TO_LE32(0x12345678);
            memcpy(bs->u.fat16.volume_name, DEFAULT_VOLUME_ID, 11);
            memcpy(bs->u.fat16.fat_name, "FAT16   ", 8);
            memcpy(bs->u.fat16.boot_code, FAT_BOOT_CODE, FAT_BOOT_CODE_LENGTH);
            break;
        case FAT_TYPE_FAT32:
        default:
            bs->reserved = PED_CPU_TO_LE16(DEFAULT_FAT32_RSVD_SEC_CNT);
            bs->dir_entries = PED_CPU_TO_LE16(DEFAULT_FAT32_ROOT_ENT_CNT);
            bs->sectors = PED_CPU_TO_LE16(0);
            bs->sector_count = PED_CPU_TO_LE32(part_sectors);
            // This is only for FAT12/16
            bs->fat_length = PED_CPU_TO_LE16(0);
            // This is only for FAT32
            bs->u.fat32.fat_length = PED_CPU_TO_LE32(fat_size);
            bs->u.fat32.flags = PED_CPU_TO_LE16(0);
            bs->u.fat32.version = PED_CPU_TO_LE16(0);
            bs->u.fat32.root_dir_cluster = PED_CPU_TO_LE32(DEFAULT_FAT32_ROOT_CLUSTER);
            bs->u.fat32.info_sector = PED_CPU_TO_LE16(DEFAULT_FAT32_FSINFO);
            bs->u.fat32.backup_sector = PED_CPU_TO_LE16(DEFAULT_FAT32_BK_BOOT_SEC);
            bs->u.fat32.drive_num = DEFAULT_DRIVER_NUMBER;
            bs->u.fat32.ext_signature = EXTENDED_BOOT_SIGNATURE;
            // It should generated by current time
            bs->u.fat32.serial_number = PED_CPU_TO_LE16(0x12345678);
            memcpy(bs->u.fat32.volume_name, DEFAULT_VOLUME_ID, 11);
            memcpy(bs->u.fat32.fat_name, "FAT32   ", 8);
            memcpy(bs->u.fat32.boot_code, FAT_BOOT_CODE, FAT_BOOT_CODE_LENGTH);
    }
    // Fill the signature
    bs->boot_sign = PED_CPU_TO_LE16(BPB_SIGNATURE);

    // Caculte some parameters for data writing position
    if (fat_type == FAT_TYPE_FAT32)
    {
        reserved_region_sectors = DEFAULT_FAT32_RSVD_SEC_CNT;
        // Root directories use at leaset one cluster
        // Free space starts from cluster 3
        root_directories_sectors = cluster_size;
    }
    else
    {
        reserved_region_sectors = DEFAULT_FAT16_RSVD_SEC_CNT;
        root_directories_sectors = 32;  // 512 entries
    }
    fat_region_sectors = fat_size * 2;

    // Zero'd all reserved sectors and root directories
    memset(empty, 0, DEFAULT_BLOCK_SIZE);
    empty_len = part_start_sector           /* Sectors before this partition */
              + reserved_region_sectors     /* Reserved sectors */
              + fat_region_sectors          /* FAT tables */
              + root_directories_sectors;   /* Root directories */

    diag_printf("Cleaning up reserved sectors and root directries (0 - %d)...\n", empty_len);
    for (count = 0; count < empty_len; count++)
    {
        err = cyg_io_bwrite(handle, (void *)empty, &len, count);
        if (err != ENOERR)
        {
            diag_printf("err = %d\n", err);
        }
    }

    // Write Master Boot Record (MBR)
    diag_printf("Writing MBR (0)...\n");
    err = cyg_io_bwrite(handle, (void *)mbr_table, &len, 0);
    if (err != ENOERR)
    {
        diag_printf("err = %d\n", err);
    }

    // Write FAT Boot Sector (BPB)
    diag_printf("Writing BPB (%d)...\n", part_start_sector);
    err = cyg_io_bwrite(handle, (void *)bs, &len, part_start_sector);
    if (err != ENOERR)
    {
        diag_printf("err = %d\n", err);
    }

    // FAT32 only sectors
    if (fat_type == FAT_TYPE_FAT32)
    {
        cyg_uint32 user_area_size = part_sectors            /* Total sectors of partition */
                                  - reserved_region_sectors /* Reserved sectors */
                                  - fat_region_sectors;     /* FAT tables */

        // Setup FSInfo Sector
        // Fill two magic numbers
        is->signature_1 = PED_CPU_TO_LE32(FSINFO_LEAD_SIGNATURE);
        is->signature_2 = PED_CPU_TO_LE32(FSINFO_STRUCT_SIGNATURE);
        // XXX: It's ok. Maybe!
        is->free_clusters = PED_CPU_TO_LE32((user_area_size / cluster_size) - 1);
        // It's hard-coded to second cluster
        is->next_cluster = PED_CPU_TO_LE32(0x2);
        is->signature_3 = PED_CPU_TO_LE32(FSINFO_SIGNATURE);

        // Write FSInfo
        pos = part_start_sector + DEFAULT_FAT32_FSINFO;
        diag_printf("Writing FSInfo (%d)...\n", pos);
        err = cyg_io_bwrite(handle, (void *)is, &len, pos);
        if (err != ENOERR)
        {
            diag_printf("err = %d\n", err);
        }

        // Write backup boot sector
        diag_printf("Writing backup BPB (%d)...\n", pos);
        pos = part_start_sector + DEFAULT_FAT32_BK_BOOT_SEC;
        err = cyg_io_bwrite(handle, (void *)bs, &len, pos);
        if (err != ENOERR)
        {
            diag_printf("err = %d\n", err);
        }

        // Write backup FSInfo
        pos = part_start_sector + DEFAULT_FAT32_BK_BOOT_SEC + DEFAULT_FAT32_FSINFO;
        diag_printf("Writing backup FSInfo (%d)...\n", pos);
        err = cyg_io_bwrite(handle, (void *)is, &len, pos);
        if (err != ENOERR)
        {
            diag_printf("err = %d\n", err);
        }
    }

    // Mark EOC of FAT
    memset(eoc_mark, 0, DEFAULT_BLOCK_SIZE);
    if (fat_type == FAT_TYPE_FAT32)
    {
        cyg_uint32 magic;

        magic = PED_CPU_TO_LE32(FAT32_EOC_MAGIC_0);
        memcpy(eoc_mark, &magic, 4);
        magic = PED_CPU_TO_LE32(FAT32_EOC_MAGIC_1);
        memcpy(&eoc_mark[4], &magic, 4);
        magic = PED_CPU_TO_LE32(FAT32_EOC_MAGIC_1);
        memcpy(&eoc_mark[8], &magic, 4);
    }
    else if (fat_type == FAT_TYPE_FAT16)
    {
        cyg_uint32 magic = PED_CPU_TO_LE32(FAT16_EOC_MAGIC);
        memcpy(eoc_mark, &magic, 4);
    }
    else if (fat_type == FAT_TYPE_FAT12)
    {
        cyg_uint32 magic = PED_CPU_TO_LE32(FAT12_EOC_MAGIC);
        memcpy(eoc_mark, &magic, 4);
    }
 
    for (count = 0; count < 2; count++)
    {
        pos = part_start_sector + reserved_region_sectors + (count * fat_size);
        diag_printf("Writing EOC mark (%d)...\n", pos);
        err = cyg_io_bwrite(handle, (void *)eoc_mark, &len, pos);
        if (err != ENOERR)
        {
            diag_printf("err = %d\n", err);
        }
    }

    // XXX: Special hack for Volume ID
    pos = part_start_sector /* Sectors before this partition */
        + reserved_region_sectors /* Reserved sectors */
        + fat_region_sectors; /* FAT tables */
    memset(root_dir_block, 0, DEFAULT_BLOCK_SIZE);
    memcpy(root_dir_block, DEFAULT_VOLUME_ID, 11);
    root_dir_block[11] = 0x08;

    err = cyg_io_bwrite(handle, (void *)root_dir_block, &len, pos);
    if (err != ENOERR)
    {
        diag_printf("err = %d\n", err);
    }
}

void
format(cyg_io_handle_t handle, cyg_bool full_format)
{
    fs_info_t fs_info = {0};

    // Get disk geometry
    get_disk_geometry(handle, &fs_info);
    // Caculte some parameters for FAT creating
    calc_param(&fs_info);
    // Erase the whole disk, if needed.
    if (full_format)
    {
        erase_disk(handle, &fs_info);
    }
    // Create & write tables to disk
    create_tables(handle, &fs_info);
}

void
cyg_user_start(void)
{
    cyg_io_handle_t handle;
    const char *devname = "/dev/sdblk0/";
    cyg_bool full_format = false;

    // Lookup device
    cyg_io_lookup(devname, &handle);

    // Start format the disk
    diag_printf("Start format the disk.\n");
    format(handle, full_format);
    diag_printf("Disk formated.\n");
}
