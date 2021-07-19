#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
// To run MS test
#include <cyg/io/disk.h>
#include <cyg/io/memorystick_protocol.h>
#include <cyg/io/memorystick_sis910.h>

// To test FAT
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>
#include <cyg/fs/fatfs.h>
#include <cyg/fileio/fileio.h>

// LED debug
#define LED_DEBUG(val) HAL_WRITE_UINT32(0x9000A000, val);

// Test mode select
//#define DIRECT_MMIO_TEST 1
#define DAFAULT_BUFFER_SIZE 4096

extern char Getch(void);

#ifdef DIRECT_MMIO_TEST
#define CMD_SET_TPC(arg)            (arg << 12)

// Definitions for gateway related registers
#define gw_fifo_reset       0x90001710
#define gw_wff_val0         0x90001740
#define gw_wff_val1         0x90001744
#define gw_rff_val0         0x90001760
#define gw_rff_val1         0x90001764
#define gw_lpc0             0x90001500
#define gw_lpc1             0x90001504
#define gw_lpc2             0x90001508
#define gw_lpc3             0x9000150C
#define gw_lpc4             0x90001510
#define gw_lpc5             0x90001514
#define gw_lpc6             0x90001518
#define gw_lpc7             0x9000151C
#define gw_rff3_dir         0x900017d0
#define dev_warb_ctrl0      0x90003620
#define dev_warb_ctrl1      0x90003624
#define dev_rarb_ctrl       0x90003628
#define dev_dir_ctrl        0x90003604
#define dev_fifo_en         0x9000362c
#define dev_peri_irqen      0x90003610
#define gw_wt1_init0        0x90001410
#define gw_wt1_init1        0x90001414
#define gw_rd3_init0        0x900014b0
#define gw_rd3_init1        0x900014b4
#define gw_irq              0x90001700
#define dev_peri_irqst      0x90003614
#define cat2(i,j)           i##j
#define cat3(i,j,k)         i##j##k

#define gw_xfer(ch, addr, dx, dy, ctl) do {                               \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init0), (dy << 9) | dx << 2);       \
        HAL_WRITE_UINT32(cat3(gw_,ch,_init1), (addr & 0xfffffff0) | ctl); \
} while(0)

// Function prototype
static Cyg_ErrNo mspro_get_int(cyg_uint8 *reg_int);
static Cyg_ErrNo mspro_media_type_identify(void);
static Cyg_ErrNo mspro_read_attribute(cyg_uint8 *attrib_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_parse_attribute(cyg_uint8 *attrib_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_confirm_cpu_startup(void);
static Cyg_ErrNo mspro_switch_interface_mode(cyg_uint8 mode);
static Cyg_ErrNo mspro_cpu_read_long_data(cyg_uint8 *data_buf, cyg_uint16 *blk_no);
static Cyg_ErrNo mspro_gateway_read_long_data(cyg_uint8 *data_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_read_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len);
static Cyg_ErrNo mspro_get_int(cyg_uint8 *reg_int);
static Cyg_ErrNo mspro_cpu_write_long_data(cyg_uint8 *data_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_gateway_write_long_data(cyg_uint8 *data_buf, cyg_uint16 *buf_len);
static Cyg_ErrNo mspro_write_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len);
static Cyg_ErrNo mspro_set_rw_reg_addr(cyg_uint8 READ_REG_ADRS, cyg_uint8 READ_REG_SIZE, cyg_uint8 WRITE_REG_ADRS, cyg_uint8 WRITE_REG_SIZE);
static Cyg_ErrNo mspro_set_cmd(const cyg_uint8 cmd);

void reg_dump(void)
{
    cyg_uint32 val = 0x0;

    diag_printf("======== Register Dump ========\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_CMD, val);
    diag_printf(" REG_SIS910_MSTICK_CMD = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
    diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    diag_printf(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
    diag_printf(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);
    diag_printf("===============================\n", val);
}

void gateway_init(void)
{
    // Config GATEWAY FIFO base and size
    // Enable Gateway FIFO: 256-byte per channel
    HAL_WRITE_UINT32(gw_fifo_reset, 0xffffffff);
    HAL_WRITE_UINT32(gw_fifo_reset, 0x0);
    HAL_WRITE_UINT32(gw_wff_val0, 0x03000308);
    HAL_WRITE_UINT32(gw_wff_val1, 0x03100318);
    HAL_WRITE_UINT32(gw_rff_val0, 0x08000808);
    HAL_WRITE_UINT32(gw_rff_val1, 0x08100818);

    //  Config GATEWAY DMA channel base address
    HAL_WRITE_UINT32(gw_lpc0, 0);
    HAL_WRITE_UINT32(gw_lpc1, 0);
    HAL_WRITE_UINT32(gw_lpc2, 0);
    HAL_WRITE_UINT32(gw_lpc3, 0);
    HAL_WRITE_UINT32(gw_lpc4, 0);
    HAL_WRITE_UINT32(gw_lpc5, 0);
    HAL_WRITE_UINT32(gw_lpc6, 0);
    HAL_WRITE_UINT32(gw_lpc7, 0);

    //  RFF_CH3 is shared by RFF_CH3_0 and RFF_CH3_1,
    //  used register "gw_rff3_dir" to switch.
    //  0: switch to RFF_CH3_0
    //  1: switch to RFF_CH3_1
    HAL_WRITE_UINT32(gw_rff3_dir, 0x1);

    HAL_WRITE_UINT32(dev_rarb_ctrl, 0x23450);
    HAL_WRITE_UINT32(dev_warb_ctrl0, 0x76543210);
    HAL_WRITE_UINT32(dev_warb_ctrl1, 0x76321098);
    HAL_WRITE_UINT32(dev_dir_ctrl, (0x01<<14));
    HAL_WRITE_UINT32(dev_fifo_en, 0xFFFF8000);  // enable WFF[15] & RFF
    HAL_WRITE_UINT32(dev_peri_irqen, 0x1<<12);  // enable ms_irq
}

void hw_init(void)
{
    cyg_uint32 val = 0;

    diag_printf("hw_init()\n");

    // SD & MS Shared PAD Switch to MS, default is SD
    HAL_WRITE_UINT32(0x90004000, 0x200);

    val = CTRL_BYTE_SWAP | CTRL_RISC_MODE | CTRL_CLK_DIVIDER_IS_8| CTRL_MS_ENABLE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_CTL, val);
    diag_printf(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

    val = 0;
    val = SYS_SERIAL_ACCESS_MODE | SYS_FIFO_CLEAR_ENABLE | SYS_FIFO_WRITE_MODE | SYS_MS_INTERRUPT_ENABLE |
          SYS_RISE_EDGE_INPUT_ENABLE | SYS_RISE_EDGE_OUTPUT_DISABLE | SYS_DEFAULT_BUSY_COUNT;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    diag_printf(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);
}

static Cyg_ErrNo
mspro_media_type_identify(void)
{
    cyg_uint8 reg_buf[4] = {0};
    cyg_uint8 reg_len = 0x4;
    Cyg_ErrNo err;

    diag_printf("mspro_media_type_identify()\n");

    // Set continuous four registers to read (0x04, 0x06, 0x07)
    err = mspro_set_rw_reg_addr(MS_TYPE_REG, 0x4, 0x0, 0x0);
    // Read registers back
    err = mspro_read_reg(reg_buf, reg_len);

    // Parse Type Register (0x04)
    if(reg_buf[0] == MS_TYPE_HAS_PARALLEL_MODE)
    {
        diag_printf(" Media Type = 0x%02X\n", reg_buf[0]);
    }
    else
    {
        diag_printf(" Media Type = 0x%02X\n", reg_buf[0]);
        return -EIO;
    }

    // Parse Category Register (0x06)
    if(reg_buf[2] == MS_CATEGORY_MEMORY)
    {
        diag_printf(" Media Category = 0x%02X\n", reg_buf[2]);
    }
    else
    {
        diag_printf(" Media Category = 0x%02X\n", reg_buf[2]);
        return -EIO;
    }

    // Parse Class Register (0x07)
    if(reg_buf[3] == MS_CLASS_RAM)
    {
        diag_printf(" Media Class = 0x%02X\n", reg_buf[3]);
    }
    else if ((reg_buf[3] == MS_CLASS_ROM) || (reg_buf[3] == MS_CLASS_R) || (reg_buf[3] == MS_CLASS_OTHER_ROM))
    {
        diag_printf(" Media Class = 0x%02X\n", reg_buf[3]);
        return -EIO;
    }

    return ENOERR;
}

static Cyg_ErrNo
mspro_confirm_cpu_startup(void)
{
    cyg_uint8 int_val = 0;
    cyg_uint32 i;

    diag_printf("mspro_confirm_cpu_startup()\n");

    // Get INT register value
    for (i = 0; i < 1000; i++)
    {
        mspro_get_int(&int_val);
        if (int_val & MS_INT_CMD_NORMAL_END)
        {
            mspro_get_int(&int_val);
            if (int_val & MS_INT_HAS_ERROR)
            {
                if (int_val & MS_INT_CMD_NON_EXECUTABLE)
                {
                    diag_printf("shifted to Write-Disabled Status.\n");
                    // TODO: Set to read only state
                    return ENOERR;
                }
                else
                {
                    diag_printf("Media Error!!\n");
                    return -EIO;
                }
            }
            else
            {
                diag_printf("CPU Start up is completed.\n");
                return ENOERR;
            }
        }
    }
    // Fails to confirm CPU startup
    diag_printf("Media Error!!\n");
    return -EIO;
}

static Cyg_ErrNo
mspro_read_attribute(cyg_uint8 *attrib_buf, cyg_uint16 *buf_len)
{
    cyg_uint32 val;
    cyg_uint8 reg_val;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 *buf = attrib_buf;
    cyg_uint16 blk_no = 1;
    cyg_uint8 i;
    cyg_uint8 read_count = (*buf_len >> 9);

    diag_printf("mspro_read_attribute()\n");

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Get read_count blocks (512 * read_count bytes)
    reg_buf[0] = 0x00; // Data Count [15:8]
    reg_buf[1] = read_count; // Data Count [7:0]
    // For Data Address
    reg_buf[2] = 0x00; // Data Address [31:24]
    reg_buf[3] = 0x00; // Data Address [23:16]
    reg_buf[4] = 0x00; // Data Address [15:8]
    reg_buf[5] = 0x00; // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set READ_ATRB command
    mspro_set_cmd(MSPRO_CMD_READ_ATRB);

    // Wait until MS_INTERRUPT signal
    diag_printf("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_MS_INTERRUPT));

    for (i = 0; i < read_count; i++)
    {
        mspro_get_int(&reg_val);
        if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
        {
            mspro_cpu_read_long_data(buf, &blk_no);
            buf += 512;
        }
        else
        {
            diag_printf("Read Attribute Error!!\n");
            return -EIO;
        }
    }
    return ENOERR;
}

static Cyg_ErrNo
mspro_parse_attribute(cyg_uint8 *attrib_buf, cyg_uint16 *buf_len)
{
    cyg_uint8 entry_count;
//    cyg_uint8 sys_info_count;
    cyg_uint8 i;

    diag_printf("mspro_parse_attribute()\n");

    diag_printf(" attrib_buf[0] = 0x%02X\n", attrib_buf[0]);
    diag_printf(" attrib_buf[1] = 0x%02X\n", attrib_buf[1]);

    // Check Signature Code
    if ((attrib_buf[0] == 0xC3) && (attrib_buf[1] == 0xA5))
    {
        // Get Device Entry Count
        if ((attrib_buf[5] >= 0x1) && (attrib_buf[5] <= 0xC))
        {
            entry_count = attrib_buf[5];
            for (i = 0; i < entry_count; i++)
            {
                cyg_uint16 entry_base = 16 + (i * 12);
                // Entry 0:
                // Address: 17, 16, 19, 18
                // Size: 21, 20, 23, 22
                // Device Information ID: 25
                diag_printf(" entry_base = %d\n", entry_base);
                diag_printf(" attrib_buf[%d] = 0x%02X\n", (entry_base + 9), attrib_buf[entry_base + 9]);
                if (attrib_buf[entry_base + 9] == 0x10)
                {
                    // It matchs System Information ID
                    cyg_uint32 address, size;

                    diag_printf(" attrib_buf[%d] = 0x%02X\n", (entry_base + 1), attrib_buf[entry_base + 1]);
                    diag_printf(" attrib_buf[%d] = 0x%02X\n", (entry_base), attrib_buf[entry_base]);
                    diag_printf(" attrib_buf[%d] = 0x%02X\n", (entry_base + 3), attrib_buf[entry_base + 3]);
                    diag_printf(" attrib_buf[%d] = 0x%02X\n", (entry_base + 2), attrib_buf[entry_base + 2]);
                    address = (attrib_buf[entry_base + 1] << 24) | (attrib_buf[entry_base] << 16) |
                              (attrib_buf[entry_base + 3] << 8) | (attrib_buf[entry_base + 2]);
                    size = (attrib_buf[entry_base + 5] << 24) | (attrib_buf[entry_base + 4] << 16) |
                           (attrib_buf[entry_base + 7] << 8) | (attrib_buf[entry_base + 6]);
                    diag_printf(" address = 0x%04X\n", address);
                    diag_printf(" size = 0x%04X\n", size);
                    diag_printf(" address + size = 0x%04X\n", address + size);
                    if ((size == 0x60) && (address >= 0x1A0) && ((address + size) <= 0x8000))
                    {
                        // Confirm System Information
                        diag_printf(" attrib_buf[%d] = 0x%02X\n", (address + 1), attrib_buf[address + 1]);
                        if (attrib_buf[address + 1] == 0x2)
                        {

                            switch (attrib_buf[address + 57])
                            {
                                case 0x00:
                                    diag_printf("Memory Stick PRO [Read/Write]\n");
                                    break;
                                case 0x01:
                                case 0x02:
                                case 0x03:
                                    diag_printf("Memory Stick PRO [Read Only]\n");
                                    break;
                                default:
                                    diag_printf("Failure of Obtaining Attribute Information Media Error. [Media breakdown]\n");
                                    return -EIO;
                            }
                            if (attrib_buf[address + 47] & 0xC0)
                            {
                                diag_printf("Shifts to read/write Protected Status.\n");
                            }

                            // Get others information
                            diag_printf("Block size = 0x%04X\n", (attrib_buf[address + 3] << 8) | attrib_buf[address + 2]);
                            diag_printf("Total blocks = 0x%04X\n", (attrib_buf[address + 5] << 8) | attrib_buf[address + 4]);
                            diag_printf("User area blocks = 0x%04X\n", (attrib_buf[address + 7] << 8) | attrib_buf[address + 6]);
                            diag_printf("Page size = 0x%04X\n", (attrib_buf[address + 9] << 8) | attrib_buf[address + 8]);
                            diag_printf("Unit size = 0x%04X\n", (attrib_buf[address + 45] << 8) | attrib_buf[address + 44]);
                            diag_printf("Interface type = 0x%02X\n", attrib_buf[address + 50]);

                            return ENOERR;
                        }
                    }
                }
            }
        }
    }

    diag_printf("Failure of Obtaining Attribute Information Media Error. [Media breakdown]\n");
    return -EIO;
}

static Cyg_ErrNo
mspro_cpu_read_long_data(cyg_uint8 *data_buf, cyg_uint16 *blk_no)
{
    cyg_uint32 val;
    cyg_uint32 i, j;
    cyg_uint8 int_val = 0;
    //cyg_uint16 blk_no = *blk_no;
    cyg_uint32 n;

    diag_printf("mspro_cpu_read_long_data()\n");

    for (n = 0; n < *blk_no; n++)
    {
        // Set TPC to MSPRO_TPC_READ_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_READ_LONG_DATA) | CMD_DATA_SELECT_CPU | 512));

        // Read data back
        for (i = 0; i < (512 / 8); i++)
        {
            // Wait until FIFO_EMPTY signal clear
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
            } while (val & STATUS_FIFO_EMPTY);

            for (j = 0; j < 4; j++)
            {
                HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
                diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
                // Get high byte first, when data came back.
                *data_buf = val & 0xFF;
                data_buf++;
                *data_buf = (val >> 8);
                data_buf++;
            }
        }

        // Wait until MS INTERRUPT signal
        do {
            HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
            diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
        } while (!(val & STATUS_MS_INTERRUPT));
        // Get INT register value
        mspro_get_int(&int_val);
        if (int_val == MS_INT_CMD_NORMAL_END)
        {
            diag_printf("MS_INT_CMD_NORMAL_END\n");
        }
        else if (int_val == MS_INT_DATA_REQUEST)
        {
            diag_printf("MS_INT_DATA_REQUEST\n");
        }
        else
        {
            return -EIO;
        }
    };

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_gateway_read_long_data(cyg_uint8 *data_buf, cyg_uint16 *buf_len)
{
    cyg_uint32 val;
    cyg_uint16 blk_no = *buf_len;
    cyg_uint32 n;

    diag_printf("mspro_gateway_read_long_data()\n");
    for (n = 0; n < blk_no; n++)
    {
        // Reset REG_SIS910_MSTICK_SYS and REG_SIS910_MSTICK_CTL settings for gateway access
        HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
        val |= SYS_INTERRUPT_ENABLE | SYS_DMA_SINGLE_ACCESS_MODE;
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
        diag_printf(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

        val = CTRL_BYTE_SWAP | CTRL_DMA_MODE | CTRL_CLK_DIVIDER_IS_8 | CTRL_MS_ENABLE;
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
        diag_printf(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

        // Reset gateway
        HAL_WRITE_UINT32(gw_fifo_reset, 0x02);  // Reset WFF_CH1
        HAL_WRITE_UINT32(gw_fifo_reset, 0x0);

        // Set TPC to MSPRO_TPC_READ_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_READ_LONG_DATA) | 512));

        // Read data back
        diag_printf("Prepare to read data through gateway.\n");
        gw_xfer(wt1, (cyg_uint32)data_buf, 16, 1, 0x2);
        data_buf += 512;

        do{
            HAL_READ_UINT32(gw_irq, val);
            diag_printf(" gw_irq = 0x%04X\n", val);
        } while (!val);

        // Clear gw_irq
        HAL_READ_UINT32(gw_irq, val);
        HAL_WRITE_UINT32(gw_irq, val);

//        // Wait until MS INTERRUPT signal
//        do {
//            HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
//            diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
//        } while (!(val & STATUS_MS_INTERRUPT));
//        // Get INT register value
//        mspro_get_int(&int_val);
//        if (int_val == MS_INT_CMD_NORMAL_END)
//        {
//            diag_printf("MS_INT_CMD_NORMAL_END\n");
//        }
//        else if (int_val == MS_INT_DATA_REQUEST)
//        {
//            diag_printf("MS_INT_DATA_REQUEST\n");
//        }
//        else
//        {
//            return EIO;
//        }
    }
    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    // Wait dev_peri_irqst.ms_irq
    do {
        HAL_READ_UINT32(dev_peri_irqst, val);
    } while (!(val & 0x1000));

    // Clear IRQ
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    val |= SYS_INTERRUPT_CLEAR_ENABLE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    diag_printf(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_switch_interface_mode(cyg_uint8 mode)
{
    Cyg_ErrNo err;
    cyg_uint8 reg_buf;

    diag_printf("mspro_switch_interface_mode()\n");

    if (mode == MS_PARALLEL_MODE)
    {
        diag_printf("Switch to parallel interface mode.\n");
        // Set system parameter to write
        err = mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_SYSTEM_PARAMETER_REG, 0x1);
        if (err != ENOERR)
        {
            return err;
        }

        // Fills the system parameter
        reg_buf = MS_PARAM_PARALLEL_IF;
        err = mspro_write_reg(&reg_buf, 0x1);
        if (err != ENOERR)
        {
            return err;
        }
    }

    return ENOERR;
}

static Cyg_ErrNo
mspro_read_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len)
{
    cyg_uint32 val;
    cyg_uint8 i;
    cyg_uint16 reg_tmp[4] = {0};
    cyg_uint8 *buf = reg_buf;

    diag_printf("mspro_read_reg()\n");

    // Set TPC to READ_REG, and data bytes is reg_len bytes
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_READ_REG) | reg_len));
    diag_printf(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_READ_REG) | reg_len));

    if (reg_buf == NULL)
    {
        return -EINVAL;
    }

    if (reg_len > 8)
    {
        return -EINVAL;
    }

    // Read data to H/W register
    for (i = 0; i < 4; i++)
    {
        HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, reg_tmp[i]);
        *buf = (reg_tmp[i] >> 8);
        buf++;
        *buf = (reg_tmp[i] & 0xFF);
        buf++;
    }
    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_get_int(cyg_uint8 *reg_int)
{
    cyg_uint32 val;

    diag_printf("mspro_get_int()\n");

    // Set TPC to MSPRO_TPC_GET_INT, and data bytes is one byte
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_GET_INT) | 0x1));
    diag_printf(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_GET_INT) | 0x1));

    // Wait until data send into FIFO
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (val & STATUS_FIFO_EMPTY);

    // Read data back
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    // Get the high byte
    *reg_int = (val >> 8);
    // Don't care dummy read
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);
    HAL_READ_UINT32(REG_SIS910_MSTICK_DATA, val);

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_cpu_write_long_data(cyg_uint8 *data_buf, cyg_uint16 *buf_len)
{
    cyg_uint32 val;
    cyg_uint32 i, j;
    cyg_uint8 int_val = 0;
    cyg_uint16 blk_no = *buf_len;
    cyg_uint32 n;


    diag_printf("mspro_cpu_write_long_data()\n");

    for (n = 0; n < blk_no; n++)
    {
        // Set TPC to MSPRO_TPC_WRITE_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_WRITE_LONG_DATA) | CMD_DATA_SELECT_CPU | 512));
        // Write data back
        for (i = 0; i < (512 / 8); i++)
        {
            // Wait until FIFO_FULL signal clear
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
            } while (val & STATUS_FIFO_FULL);

            for (j = 0; j < 4; j++)
            {
                val = 0;
                val |= *data_buf;
                data_buf++;
                val |= (*data_buf << 8);
                data_buf++;
                diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
                HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, val);
            }
        }

        // Wait until MS INTERRUPT signal
        do {
            HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
            diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
        } while (!(val & STATUS_MS_INTERRUPT));
        // Get INT register value
        mspro_get_int(&int_val);
        if (int_val == MS_INT_CMD_NORMAL_END)
        {
            diag_printf("MS_INT_CMD_NORMAL_END\n");
        }
        else if (int_val == MS_INT_DATA_REQUEST)
        {
            diag_printf("MS_INT_DATA_REQUEST\n");
        }
        else
        {
            return EIO;
        }
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_gateway_write_long_data(cyg_uint8 *data_buf, cyg_uint16 *buf_len)
{
    cyg_uint32 val;
    cyg_uint16 blk_no = *buf_len;
    cyg_uint32 n;

    diag_printf("mspro_gateway_write_long_data()\n");
    for (n = 0; n < blk_no; n++)
    {
        // Reset REG_SIS910_MSTICK_SYS and REG_SIS910_MSTICK_CTL settings for gateway access
        HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
        val |= SYS_INTERRUPT_ENABLE | SYS_DMA_SINGLE_ACCESS_MODE | SYS_FIFO_WRITE_MODE;
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
        diag_printf(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

        val = CTRL_BYTE_SWAP | CTRL_DMA_MODE | CTRL_CLK_DIVIDER_IS_8 | CTRL_MS_ENABLE;
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CTL, val);
        diag_printf(" REG_SIS910_MSTICK_CTL = 0x%04X\n", val);

        // Reset gateway
        HAL_WRITE_UINT32(gw_fifo_reset, 0x80);  // Reset WFF_CH3
        HAL_WRITE_UINT32(gw_fifo_reset, 0x0);

        // Set TPC to MSPRO_TPC_WRITE_LONG_DATA, and data bytes is 512 bytes
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_WRITE_LONG_DATA) | 512));

        // Write data back
        gw_xfer(rd3, (cyg_uint32)data_buf, 16, 1, 0x2);
        data_buf += 512;

        do{
            HAL_READ_UINT32(gw_irq, val);
            diag_printf(" gw_irq = 0x%04X\n", val);
        } while (!val);

        //clear gw_irq
        HAL_READ_UINT32(gw_irq, val);
        HAL_WRITE_UINT32(gw_irq, val);
    }

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    // Wait dev_peri_irqst.ms_irq
    do {
        HAL_READ_UINT32(dev_peri_irqst, val);
    } while (!(val & 0x1000));

    // Clear IRQ
    HAL_READ_UINT32(REG_SIS910_MSTICK_SYS, val);
    val |= SYS_INTERRUPT_CLEAR_ENABLE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_SYS, val);
    diag_printf(" REG_SIS910_MSTICK_SYS = 0x%04X\n", val);

    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_write_reg (cyg_uint8 *reg_buf, const cyg_uint8 reg_len)
{
    cyg_uint32 val;
    cyg_uint8 i, j;
    cyg_uint16 reg_tmp[4] = {0};

    diag_printf("mspro_write_reg()\n");

    // Set TPC to WRITE_REG, and data bytes is reg_len bytes
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_WRITE_REG) | reg_len));
    diag_printf(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_WRITE_REG) | reg_len));

    if (reg_buf == NULL)
    {
        return -EINVAL;
    }

    if (reg_len > 8)
    {
        return -EINVAL;
    }

    // Combine reg_buf into H/W acceptable form
    for (i = 0, j = 0; i < reg_len; i+=2, j++)
    {
        reg_tmp[j] = (reg_buf[i] << 8) |  reg_buf[i + 1];
        diag_printf(" reg_tmp[%d] = 0x%04X\n", j, reg_tmp[j]);
    }

    // Write data to H/W register
    for (i = 0; i < 4; i++)
    {
        HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, reg_tmp[i]);
    }
    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_set_rw_reg_addr(cyg_uint8 READ_REG_ADRS, cyg_uint8 READ_REG_SIZE, cyg_uint8 WRITE_REG_ADRS, cyg_uint8 WRITE_REG_SIZE)
{
    cyg_uint32 val;

    diag_printf("mspro_set_rw_reg_addr()\n");

    // Set TPC to SET_R/W_REG_ADRS, and data bytes is 4 bytes
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_SET_RW_REG_ADRS) | 0x4));
    diag_printf(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_SET_RW_REG_ADRS) | 0x4));

    // Set READ_REG address and size
    val = (READ_REG_ADRS << 8) | READ_REG_SIZE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    // Set WRITE_REG address and size
    val = (WRITE_REG_ADRS << 8) | WRITE_REG_SIZE;
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, val);
    diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", val);
    // Don't care dummy write
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
    HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
    // Wait until READY signal
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_READY));

    return ENOERR;
}

static Cyg_ErrNo
mspro_set_cmd(const cyg_uint8 cmd)
{
    cyg_uint32 val;

    diag_printf("mspro_set_cmd()\n");

    switch(cmd)
    {
        case MSPRO_CMD_READ_DATA:
        case MSPRO_CMD_WRITE_DATA:
        case MSPRO_CMD_READ_INFO:
        case MSPRO_CMD_WRITE_INFO:
        case MSPRO_CMD_READ_ATRB:
        case MSPRO_CMD_STOP:
        case MSPRO_CMD_ERASE:
        case MSPRO_CMD_CHG_CLASS:
        case MSPRO_CMD_FORMAT:
        case MSPRO_CMD_SLEEP:
            // Set TPC to SET_CMD, and data bytes is 1 byte
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_CMD, (CMD_SET_TPC(MSPRO_TPC_SET_CMD) | 0x1));
            diag_printf(" REG_SIS910_MSTICK_CMD = 0x%04X\n", (CMD_SET_TPC(MSPRO_TPC_SET_CMD) | 0x1));
            // Set CMD
            // Set to high byte
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, (cmd << 8));
            diag_printf(" REG_SIS910_MSTICK_DATA = 0x%04X\n", (cmd << 8));
            // Don't care dummy write
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
            HAL_WRITE_UINT32(REG_SIS910_MSTICK_DATA, 0x0);
            // Wait until READY signal
            do {
                HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
                diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
            } while (!(val & STATUS_READY));
            return ENOERR;
        default:
            return -EINVAL;
    }
}

// Ref: v1.00 Spec. Section A.3.3.1
void ms_cpu_read_test(void)
{
    cyg_uint8 *data_buf = NULL;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    cyg_uint32 val;
    cyg_uint16 buf_len = 2;
    cyg_uint8 attrib_buf[512] = {0};
    cyg_uint16 attrib_buf_len = 512;
    cyg_uint32 i;

    diag_printf("ms_cpu_read_test()\n");

    // Initialize H/W
    hw_init();

    // Identify media type
    mspro_media_type_identify();

    // Switch interface mode
    mspro_switch_interface_mode(MS_SERIAL_MODE);

    // Confirm CPU startup
    mspro_confirm_cpu_startup();

    // Read Attribute Information
    mspro_read_attribute(attrib_buf, &attrib_buf_len);
    // Print out Attribute Information
    diag_printf("=======================================\n");
    for (i = 0; i < 512; i++)
    {
        diag_printf("attrib_buf[%d] = 0x%02X\n", i, attrib_buf[i]);
    }
    diag_printf("=======================================\n");

    // Parse Attribute Information
    mspro_parse_attribute(attrib_buf, &attrib_buf_len);

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Get 2 blocks (1024 bytes)
    reg_buf[0] = 0x00; // Data Count [15:8]
    reg_buf[1] = 0x02; // Data Count [7:0]
    // For Data Address
    reg_buf[2] = 0x00; // Data Address [31:24]
    reg_buf[3] = 0x00; // Data Address [23:16]
    reg_buf[4] = 0x00; // Data Address [15:8]
    reg_buf[5] = 0x01; // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set READ_DATA command
    mspro_set_cmd(MSPRO_CMD_READ_DATA);

    // Wait until MS_INTERRUPT signal
    diag_printf("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
    {
        data_buf = malloc(512 * buf_len);
        if (data_buf)
        {
            cyg_uint8 *buf = data_buf;

            mspro_cpu_read_long_data(data_buf, &buf_len);

            // Print out data
            diag_printf("=======================================\n");
            diag_printf("             DATA READ BACK            \n");
            diag_printf("=======================================\n");
            for (i = 0; i < (512 * buf_len); i++)
            {
                if ((i % 8) == 7)
                {
                    diag_printf("0x%02X\n", *buf);
                }
                else
                {
                    diag_printf("0x%02X ", *buf);
                }
                buf++;
            }
            diag_printf("=======================================\n");

            free(data_buf);
            diag_printf("Finished!!\n");
        }
    }
    else
    {
        diag_printf("Error!!\n");
    }
}

void ms_gateway_read_test(void)
{
    cyg_uint8 *data_buf = NULL;
    cyg_uint8 *obuf = NULL;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    cyg_uint32 val;
    cyg_uint16 buf_len = 2;

    diag_printf("ms_gateway_read_test()\n");

    // Gateway initialization
    gateway_init();

    // Initialize H/W
    hw_init();

    // Identify media type
    mspro_media_type_identify();

    // Switch interface mode
    mspro_switch_interface_mode(MS_SERIAL_MODE);

    // Confirm CPU startup
    mspro_confirm_cpu_startup();

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Get 2 blocks (1024 bytes)
    reg_buf[0] = 0x00; // Data Count [15:8]
    reg_buf[1] = 0x02; // Data Count [7:0]
    // For Data Address
    reg_buf[2] = 0x00; // Data Address [31:24]
    reg_buf[3] = 0x00; // Data Address [23:16]
    reg_buf[4] = 0x00; // Data Address [15:8]
    reg_buf[5] = 0x01; // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set READ_DATA command
    mspro_set_cmd(MSPRO_CMD_READ_DATA);

    // Wait until MS_INTERRUPT signal
    diag_printf("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
    {
        cyg_uint8 offset;
        data_buf = malloc(512 * buf_len + 40);
        diag_printf("Original buf address = 0x%08X\n", data_buf);
        obuf = data_buf;
        offset = (cyg_uint32)data_buf % 32;
        data_buf += (32 - offset);
        diag_printf("Shifted buf address = 0x%08X\n", data_buf);
        if (data_buf)
        {
            cyg_uint8 *buf = data_buf;
            cyg_uint32 i;

            mspro_gateway_read_long_data(data_buf, &buf_len);

            // Print out data
            diag_printf("=======================================\n");
            diag_printf("             DATA READ BACK            \n");
            diag_printf("=======================================\n");
            for (i = 0; i < (512 * buf_len); i++)
            {
                if ((i % 8) == 7)
                {
                    diag_printf("0x%02X\n", *buf);
                }
                else
                {
                    diag_printf("0x%02X ", *buf);
                }
                buf++;
            }
            diag_printf("=======================================\n");

            free(obuf);
            diag_printf("Finished!!\n");
        }
    }
    else
    {
        diag_printf("Error!!\n");
    }
}

// Ref: v1.00 Spec. Section A.3.3.2
void ms_cpu_write_test(void)
{
    cyg_uint8 *data_buf = NULL;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    cyg_uint32 val;
    cyg_uint16 buf_len = 2;

    diag_printf("ms_cpu_write_test()\n");

    // Initialize H/W
    hw_init();

    // Identify media type
    mspro_media_type_identify();

    // Switch interface mode
    mspro_switch_interface_mode(MS_SERIAL_MODE);

    // Confirm CPU startup
    mspro_confirm_cpu_startup();

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Write 2 blocks (1024 bytes)
    reg_buf[0] = 0x00; // Data Count [15:8]
    reg_buf[1] = 0x02; // Data Count [7:0]
    // For Data Address
    reg_buf[2] = 0x00; // Data Address [31:24]
    reg_buf[3] = 0x00; // Data Address [23:16]
    reg_buf[4] = 0x00; // Data Address [15:8]
    reg_buf[5] = 0x01; // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set WRITE_DATA command
    mspro_set_cmd(MSPRO_CMD_WRITE_DATA);

    // Wait until MS_INTERRUPT signal
    diag_printf("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
    {
        data_buf = malloc(512 * buf_len);
        if (data_buf)
        {
            cyg_uint8 *buf = data_buf;
            cyg_uint32 i;

            // Print out data to write
            diag_printf("=======================================\n");
            diag_printf("            DATA WRITE BACK            \n");
            diag_printf("=======================================\n");
            for (i = 0; i < (512 * buf_len); i++)
            {
                // Fills data to write
                *buf = i;

                if ((i % 8) == 7)
                {
                    diag_printf("0x%02X\n", *buf);
                }
                else
                {
                    diag_printf("0x%02X ", *buf);
                }
                buf++;
            }
            diag_printf("=======================================\n");

            mspro_cpu_write_long_data(data_buf, &buf_len);

            free(data_buf);
            diag_printf("Finished!!\n");
        }
    }
    else
    {
        diag_printf("Error!!\n");
    }
}

void ms_gateway_write_test(void)
{
    cyg_uint8 *data_buf = NULL;
    cyg_uint8 *obuf = NULL;
    cyg_uint8 reg_buf[6] = {0};
    cyg_uint8 reg_val = 0;
    cyg_uint32 val;
    cyg_uint16 buf_len = 2;

    diag_printf("ms_gateway_write_test()\n");

    // Gateway initialization
    gateway_init();

    // Initialize H/W
    hw_init();

    // Identify media type
    mspro_media_type_identify();

    // Switch interface mode
    mspro_switch_interface_mode(MS_SERIAL_MODE);

    // Confirm CPU startup
    mspro_confirm_cpu_startup();

    // Set registers to write
    mspro_set_rw_reg_addr(0x0, 0x0, MSPRO_DATA_COUNT_REG_1, 0x6);

    // Fills the data count and data address
    // For Data Count
    // Get 1 block (512 bytes)
    reg_buf[0] = 0x00; // Data Count [15:8]
    reg_buf[1] = 0x02; // Data Count [7:0]
    // For Data Address
    reg_buf[2] = 0x00; // Data Address [31:24]
    reg_buf[3] = 0x00; // Data Address [23:16]
    reg_buf[4] = 0x00; // Data Address [15:8]
    reg_buf[5] = 0x01; // Data Address [7:0]
    mspro_write_reg(reg_buf, 0x6);

    // Set WRITE_DATA command
    mspro_set_cmd(MSPRO_CMD_WRITE_DATA);

    // Wait until MS_INTERRUPT signal
    diag_printf("Wait until MS INTERRUPT signal.\n");
    do {
        HAL_READ_UINT32(REG_SIS910_MSTICK_STATUS, val);
        diag_printf(" REG_SIS910_MSTICK_STATUS = 0x%04X\n", val);
    } while (!(val & STATUS_MS_INTERRUPT));

    mspro_get_int(&reg_val);
    if ((reg_val == MS_INT_DATA_REQUEST) || (reg_val == MS_INT_DATA_REQUEST_ERR))
    {
        cyg_uint8 offset;
        data_buf = malloc(512 * buf_len + 40);
        diag_printf("Original buf address = 0x%08X\n", data_buf);
        obuf = data_buf;
        offset = (cyg_uint32)data_buf % 32;
        data_buf += (32 - offset);
        data_buf += 0x40000000;
        diag_printf("Shifted buf address = 0x%08X\n", data_buf);
        if (data_buf)
        {
            cyg_uint8 *buf = data_buf;
            cyg_uint32 i;

            // Print out data
            diag_printf("=======================================\n");
            diag_printf("            DATA WRITE BACK            \n");
            diag_printf("=======================================\n");
            for (i = 0; i < (512 * buf_len); i++)
            {
                 // Fills data to write
                *buf = i;

                if ((i % 8) == 7)
                {
                    diag_printf("0x%02X\n", *buf);
                }
                else
                {
                    diag_printf("0x%02X ", *buf);
                }
                buf++;
            }
            diag_printf("=======================================\n");

            mspro_gateway_write_long_data(data_buf, &buf_len);

            free(obuf);
            diag_printf("Finished!!\n");
        }
    }
    else
    {
        diag_printf("Error!!\n");
    }
}

void cyg_user_start(void)
{
    diag_printf("Memory Stick MMIO test.\n");

    //ms_cpu_write_test();

    ms_cpu_read_test();

    //ms_gateway_write_test();

    //ms_gateway_read_test();
}
#else
void file_copy(void)
{
    Cyg_ErrNo err;
    cyg_bool disk_mounted = false;
    char *buf = NULL;
    FILE *fp = NULL, *copy = NULL;
    int ret = 0;

    diag_printf("Prepare to mount Memory Stick Pro disk on partition 0.\n");
    err = mount( "/dev/msblk0/", "/", "fatfs" );
    if (err)
    {
        diag_printf(" Mount fails!; err = %d\n", err);
        diag_printf("Prepare to mount Memory Stick Pro disk on partition 1.\n");
        err = mount( "/dev/msblk0/1", "/", "fatfs" );
        if (err)
        {
            diag_printf(" Mount fails!\n");
        }
        else
        {
            diag_printf(" Mount success!\n");
            disk_mounted = true;
        }
    }
    else
    {
        diag_printf(" Mount success!\n");
        disk_mounted = true;
    }

    diag_printf("Press any key to continue.\n");
    Getch();

    if (disk_mounted)
    {
        diag_printf("Prepare to open input file.\n");
        fp = fopen("IN_FILE.JPG", "rb");
        diag_printf("Prepare to open output file.\n");
        copy = fopen("OUT_FILE.JPG", "wb");
        buf = (char *)malloc(DAFAULT_BUFFER_SIZE);
        if (buf)
        {
            ret = fread(buf, 1, DAFAULT_BUFFER_SIZE, fp);
            diag_printf("Writing file");
            while(ret > 0)
            {
                ret = fwrite(buf, 1, ret, copy);
                if (ret > 0)
                {
                    ret = fread(buf, 1, DAFAULT_BUFFER_SIZE, fp);
                    diag_printf(".");
                }
            }
            fclose(fp);
            fclose(copy);
            free(buf);
            diag_printf("\n");
        }
    }
    diag_printf("Test Finished.\n");
}

void block_read(void)
{
    Cyg_ErrNo err;
    cyg_uint8 * buf = NULL;
    cyg_io_handle_t hMS;
    cyg_uint32 bread_len;
    cyg_uint32 bread_pos;
    cyg_uint32 i;

    diag_printf("Prepare to test read MS card.\n");
    err = cyg_io_lookup("/dev/msblk0/1", &hMS);
    if (err == ENOERR)
    {
        diag_printf("Single Block Read Test.\n");
        buf = malloc_align(512, 32);
        if (buf)
        {
            for (i = 0; i < 20000; i++)
            {
                bread_len = 1;
                bread_pos = i;
                err = cyg_io_bread(hMS, buf, &bread_len, bread_pos);
                if (err != ENOERR)
                {
                    diag_printf("Read fails.\n");
                }
                else
                {
                    diag_printf(".");
                    //diag_dump_buf_with_offset(buf, 512, buf);
                }
            }

            free(buf);
        }
    }
    diag_printf("Test Finished.\n");
}

void block_write(void)
{
    Cyg_ErrNo err;
    cyg_uint8 *buf = NULL;
    cyg_io_handle_t hMS;
    cyg_uint32 bread_len;
    cyg_uint32 bread_pos;
    cyg_uint32 i;

    diag_printf("Prepare to test read MS card.\n");
    err = cyg_io_lookup("/dev/msblk0/1", &hMS);
    if (err == ENOERR)
    {
        diag_printf("Single Block Read/Write Test.\n");
        buf = malloc_align(512, 32);
        if (buf)
        {
            for (i = 0; i < 512; i++)
            {
                *buf++ = i;
            }

            for (i = 0; i < 20000; i++)
            {
//                diag_printf("-");
//                bread_len = 1;
//                bread_pos = i;
//                err = cyg_io_bread(hMS, buf, &bread_len, bread_pos);
//                if (err != ENOERR)
//                {
//                    diag_printf("Read fails.\n");
//                }
//                diag_printf("\\");

//                diag_printf("|");
                bread_len = 1;
                bread_pos = 0;
                err = cyg_io_bwrite(hMS, buf, &bread_len, bread_pos);
                if (err != ENOERR)
                {
                    diag_printf("Write fails.\n");
                    break;
                }
                else
                {
                    diag_printf(".\n");
//                diag_printf("/");
                }
            }
            free(buf);
        }
    }
    diag_printf("Test Finished.\n");
}

void cyg_user_start(void)
{
    file_copy();
}
#endif




