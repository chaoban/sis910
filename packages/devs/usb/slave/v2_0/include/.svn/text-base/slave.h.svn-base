#ifndef _slave_H_
#define _slave_H_

// ------------ include path -------------------
#include <cyg/infra/cyg_type.h>
#include <errno.h>	// Cyg_ErrNo
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/mass.h>

#define	BULK_IO_BUF		0x40400000	//512*2 bytes -> 65536 bytes (128 blocks)
#define	BULK_CMD_BUF	0x40410000	//0x40400400	//31 bytes
#define	BULK_STS_BUF	0x40410020	//0x40400420	//13 bytes

#define IO_BUF_TYPE		0x01
#define CMD_BUF_TYPE	0x02
#define BULK_BUF_BUF	0x03

#define MAX_IO_BLOCKS	128
// ----------- Get Config -------------------
#define USB_SLAVE_GET_CONFIG_DATA_STATUS	1
#define USB_SLAVE_GET_CONFIG_IO_BUFFER		2

#define USB_SLAVE_BULK_IN	1
#define USB_SLAVE_BULK_OUT	2

// ----------- DEVICE SETTING ----------------
#define MAX_LUN	2

#define CLUN_SD	0
#define CLUN_MS	1


// ---------------- ERROR MESSAGE ---------------
#define USB_SLAVE_MAXLUN_INCORRECT		1


// ------------ slave device structure ---------
struct SLAVE_DEVICE
{
	cyg_uint8	bCBWReceived;
	cyg_uint8	bHost_MaxLun;	//host
	cyg_uint8	bHost_CurLun;
	cyg_uint8 	bHost_Command;
	cyg_uint32	dHost_LBA;
	cyg_uint32	dHost_blocks;

	cyg_uint8	bDevice_MaxLun;//slave
	cyg_uint8	bDevice_CurLun;
	cyg_uint8	bDevice_Connected;
	cyg_uint8	bDevice_Reserve;
	cyg_uint32	dDevice_TotalBlocks;
	cyg_uint32	data_len;		//data transfer
	//cyg_uint8	data_buf[512];
	cyg_uint8  *data_buf;
	
	struct CBW	cbw;
	struct CSW	csw;		
} __attribute__ ((aligned (16)));

#endif
