#ifndef CYG_DEVS_DISK_MEMORYSTICK_PROTOCOL_H
#define CYG_DEVS_DISK_MEMORYSTICK_PROTOCOL_H

// ========================================================
//   TPC, CMD and REGISTER Settings for Memory Stick Card
// ========================================================
// Transfer Protocol Command (TPC)
#define	MS_TPC_RESERVED             0x1
#define	MS_TPC_READ_PAGE_DATA       0x2
#define	MS_TPC_READ_REG             0x4
#define	MS_TPC_GET_INT              0x7
#define	MS_TPC_SET_RW_REG_ADRS      0x8
#define	MS_TPC_WRITE_REG            0xB
#define	MS_TPC_WRITE_PAGE_DATA      0xD
#define	MS_TPC_SET_CMD              0xE

// Memory Access Command (FLASH Command)
#define	MS_CMD_BLOCK_READ           0xAA
#define	MS_CMD_BLOCK_WRITE          0x55
#define	MS_CMD_BLOCK_END            0x33
#define	MS_CMD_BLOCK_ERASE          0x99
#define	MS_CMD_FLASH_STOP           0xCC

// Function Command
#define	MS_CMD_SLEEP                0x5A
#define	MS_CMD_CLEAR_BUF            0xC3
#define	MS_CMD_RESET                0x3C

// Status Registers
#define MS_INT_REG                  0x01
#define MS_STATUS_REG_0             0x02
#define MS_STATUS_REG_1             0x03
#define MS_TYPE_REG                 0x04
#define MS_CATEGORY_REG             0x06
#define MS_CLASS_REG                0x07

// Parameter Registers
#define MS_SYSTEM_PARAMETER_REG     0x10
#define MS_BLOCK_ADDRESS_REG_2      0x11
#define MS_BLOCK_ADDRESS_REG_1      0x12
#define MS_BLOCK_ADDRESS_REG_0      0x13
#define MS_CMD_PRRAMETER_REG        0x14
#define MS_PAGE_ADDRESS             0x15

// Extra Data Registers
#define MS_OVERWRITE_FLAG_REG       0x16
#define MS_MANAGEMENT_FLAG_REG      0x17
#define MS_LOGICAL_ADDRESS_REG_1    0x18
#define MS_LOGICAL_ADDRESS_REG_0    0x19


// ============================================================
//   TPC, CMD and REGISTER Settings for Memory Stick PRO Card
// ============================================================
// Transfer Protocol Command (TPC)
#define	MSPRO_TPC_READ_LONG_DATA    MS_TPC_READ_PAGE_DATA
#define	MSPRO_TPC_READ_SHORT_DATA   0x3
#define	MSPRO_TPC_READ_REG          MS_TPC_READ_REG
#define	MSPRO_TPC_GET_INT           MS_TPC_GET_INT
#define	MSPRO_TPC_SET_RW_REG_ADRS   MS_TPC_SET_RW_REG_ADRS
#define	MSPRO_TPC_EX_SET_CMD        0x9
#define	MSPRO_TPC_WRITE_REG         MS_TPC_WRITE_REG
#define	MSPRO_TPC_WRITE_LONG_DATA   MS_TPC_WRITE_PAGE_DATA
#define	MSPRO_TPC_WRITE_SHORT_DATA  0xC
#define	MSPRO_TPC_SET_CMD           MS_TPC_SET_CMD


// Memory Access Command
#define	MSPRO_CMD_READ_DATA         0x20
#define	MSPRO_CMD_WRITE_DATA        0x21
#define	MSPRO_CMD_READ_INFO         0x22
#define	MSPRO_CMD_WRITE_INFO        0x23
#define	MSPRO_CMD_READ_ATRB         0x24
#define	MSPRO_CMD_STOP              0x25
#define	MSPRO_CMD_ERASE             0x26
#define	MSPRO_CMD_CHG_CLASS         0x27

// Function Command
#define	MSPRO_CMD_FORMAT            0x10
#define	MSPRO_CMD_SLEEP             0x11

// Status Registers
#define MSPRO_INT_REG               MS_INT_REG
#define MSPRO_STATUS_REG            MS_STATUS_REG0
#define MSPRO_TYPE_REG              MS_TYPE_REG
#define MSPRO_CATEGORY_REG          MS_CATEGORY_REG
#define MSPRO_CLASS_REG             MS_CLASS_REG

// Parameter Registers
#define MSPRO_SYSTEM_PARAMETER_REG  MS_SYSTEM_PARAMETER_REG
#define MSPRO_DATA_COUNT_REG_1      0x11
#define MSPRO_DATA_COUNT_REG_0      0x12
#define MSPRO_DATA_ADDRESS_REG_3    0x13
#define MSPRO_DATA_ADDRESS_REG_2    0x14
#define MSPRO_DATA_ADDRESS_REG_1    0x15
#define MSPRO_DATA_ADDRESS_REG_0    0x16
#define MSPRO_TPC_PARAMETER_REG     0x17
#define MSPRO_CMD_PARAMETER_REG     0x18

// Status Registers
// INT Register Specification (0x01, Read Only)
// Interruption factors
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | reg | CED | ERR | BREQ|     |     |     |     |CMDNK|
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | def |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
#define MS_INT_CMD_NORMAL_END       0x80
#define MS_INT_CMD_ERR_END          0xC0
#define MS_INT_DATA_REQUEST         0x20
#define MS_INT_DATA_REQUEST_ERR     0x60
#define MS_INT_EXECUTING_CMD        0x00
#define MS_INT_CMD_NON_EXECUTABLE   0x81
#define MS_INT_HAS_ERROR            0x40
// Status Register Specification (0x02, Read Only)
// Status of the system
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | reg | ES  |     |     |     |     |     | SL  | WP  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | def |  0  |  0  |  0  |  0  |  0  |  0  |  0  | 0/1 |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
#define MS_STATUS_ERASED            0x80
#define MS_STATUS_NOT_ERASED        0x00
#define MS_STATUS_SLEEP             0x02
#define MS_STATUS_WAKEUP            0x00
#define MS_STATUS_WRITE_PROTECTED   0x01
#define MS_STATUS_WRITE_PERMITTED   0x00
// Type Register Specification (0x04, Read Only)
// Media type
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | def |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  1  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
#define MS_TYPE_ONLY_SERIAL_MODE    0x00 // 0xFF is also valid for Memory Stick
#define MS_TYPE_HAS_PARALLEL_MODE   0x01
// Category Register Specification (0x06, Read Only)
// Media category
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | def |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
#define MS_CATEGORY_MEMORY          0x00 // 0xFF is also valid for Memory Stick
// Class Register Specification (0x07, Read Only)
// Media class
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | def |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
#define MS_CLASS_RAM                0x00
#define MS_CLASS_ROM                0x01
#define MS_CLASS_R                  0x02
#define MS_CLASS_OTHER_ROM          0x03

// Parameter Registers
// System Parameter Register Specification (0x10, Write Only)
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
// | reg | SRAC|     |     |     |     |     |     |     |
// +-----+-----+-----+-----+-----+-----+-----+-----+-----+
#define MS_PARAM_SERIAL_IF          0x80
#define MS_PARAM_PARALLEL_IF        0x00

#define UNUSED           0
#define MS               1
#define MSPRO            2

#endif
