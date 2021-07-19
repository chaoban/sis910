#ifndef CYG_DEVS_DISK_MEMORYSTICK_SIS910_H
#define CYG_DEVS_DISK_MEMORYSTICK_SIS910_H

// Registers for H/W Memory Stick controller
#define REG_SIS910_MSTICK_CMD       0x900036C0
#define REG_SIS910_MSTICK_DATA      0x900036C4
#define REG_SIS910_MSTICK_STATUS    0x900036C8
#define REG_SIS910_MSTICK_SYS       0x900036CC
#define REG_SIS910_MSTICK_CTL	    0x900036D0

#define MS_SERIAL_MODE              1
#define MS_PARALLEL_MODE            0

// Definitions for mstick_cmd (0x900036C0)
#define CMD_SET_TPC(arg)            (arg << 12)
#define CMD_DATA_SELECT_CPU         (0x1 << 10)

// Definitions for mstick_status (0x900036C8)
#define STATUS_DMA_REQUEST          (0x1 << 14)
#define STATUS_MS_INTERRUPT         (0x1 << 13)
#define STATUS_READY                (0x1 << 12)
#define STATUS_CRC_ERROR            (0x1 << 9)
#define STATUS_TIMEOUT_ERROR        (0x1 << 8)
#define STATUS_FIFO_EMPTY           (0x1 << 5)
#define STATUS_FIFO_FULL            (0x1 << 4)
#define STATUS_MS_COMMAND_END       (0x1 << 3)
#define STATUS_MS_ERROR             (0x1 << 2)
#define STATUS_MS_BUF_REQUEST       (0x1 << 1)
#define STATUS_MS_CMD_NO_ACK        (0x1 << 0)

// Definitions for mstick_sys (0x900036CC)
#define SYS_SYNC_RESET_ENABLE       (0x1 << 15)
#define SYS_SYNC_RESET_DISABLE      (0x0 << 15)
#define SYS_SERIAL_ACCESS_MODE      (0x1 << 14)
#define SYS_PARALLEL_ACCESS_MODE    (0x0 << 14)
#define SYS_INTERRUPT_ENABLE        (0x1 << 13)
#define SYS_INTERRUPT_DISABLE       (0x0 << 13)
#define SYS_NO_CRC_MODE             (0x1 << 12)
#define SYS_CRC_MODE                (0x0 << 12)
#define SYS_INTERRUPT_CLEAR_ENABLE  (0x1 << 11)
#define SYS_INTERRUPT_CLEAR_DISABLE (0x0 << 11)
#define SYS_MS_INTERRUPT_ENABLE     (0x1 << 10)
#define SYS_MS_INTERRUPT_DISABLE    (0x0 << 10)
#define SYS_FIFO_CLEAR_ENABLE       (0x1 << 9)
#define SYS_FIFO_CLEAR_DISABLE      (0x0 << 9)
#define SYS_FIFO_WRITE_MODE         (0x1 << 8)
#define SYS_FIFO_READ_MODE          (0x0 << 8)
#define SYS_DMA_SINGLE_ACCESS_MODE  (0x1 << 7)
#define SYS_DMA_DUAL_ACCESS_MODE    (0x0 << 7)
#define SYS_DMA_EDGE_REQUEST_MODE   (0x1 << 6)
#define SYS_DMA_LEVEL_REQUEST_MODE  (0x0 << 6)
#define SYS_DMA_INTRURUPT_ENABLE    (0x1 << 5)
#define SYS_DMA_INTERRUPT_DISABLE   (0x0 << 5)
#define SYS_RISE_EDGE_INPUT_ENABLE  (0x1 << 4)
#define SYS_RISE_EDGE_INPUT_DISABLE (0x0 << 4)
#define SYS_RISE_EDGE_OUTPUT_ENABLE (0x1 << 3)
#define SYS_RISE_EDGE_OUTPUT_DISABLE (0x0 << 3)
#define SYS_DEFAULT_BUSY_COUNT      0x5

// Definitions for mstick_ctl (0x900036D0)
#define CTRL_NO_BYTE_SWAP           (0x1 << 4)
#define CTRL_BYTE_SWAP              (0x0 << 4)
#define CTRL_RISC_MODE              (0x1 << 3)
#define CTRL_DMA_MODE               (0x0 << 3)
#define CTRL_CLK_DIVIDER_IS_2       (0x0 << 1)
#define CTRL_CLK_DIVIDER_IS_4       (0x1 << 1)
#define CTRL_CLK_DIVIDER_IS_8       (0x2 << 1)
#define CTRL_CLK_DIVIDER_IS_16      (0x3 << 1)
#define CTRL_MS_ENABLE              (0x1 << 0)
#define CTRL_MS_DISABLE             (0x0 << 0)

// The size of each disk block
#define MS_DEFAULT_BLOCK_LENGTH        512

typedef struct cyg_mspro_device {
	struct cyg_mspro_bus*     mspro_bus;
} cyg_mspro_device;

typedef struct cyg_mspro_bus {
    cyg_drv_mutex_t     mspro_lock;
} cyg_mspro_bus;

typedef struct cyg_mspro_disk_info_t {
	cyg_mspro_device*   mspro_dev;
	cyg_uint32          mspro_block_count;
	cyg_uint32          mspro_block_length;
	cyg_bool            mspro_connected;
	cyg_bool            mspro_read_only;
    cyg_uint32          mspro_heads_per_cylinder;
    cyg_uint32          mspro_sectors_per_head;
    cyg_uint32          mspro_read_block_length;
    cyg_uint32          mspro_write_block_length;
    cyg_uint8           mspro_card_type;
    cyg_uint8           mspro_bus_width;
} cyg_mspro_disk_info_t;

#endif
