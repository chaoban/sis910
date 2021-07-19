#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
// To run SPI test
#include <cyg/io/spi.h>
#include <cyg/io/spi_sis910.h>

// ST M25P64 Serial Flash memory instruction
#define INST_WREN      0x06
#define INST_WRDI      0x04
#define INST_RDID      0x9F
#define INST_RDSR      0x05
#define INST_WRSR      0x01
#define INST_READ      0x03
#define INST_FAST_READ 0x0B
#define INST_PP        0x02
#define INST_SE        0xD8
#define INST_BE        0xC7
#define INST_RES       0xAB

// LED debug
#define LED_DEBUG(val) SPI_WRITE_UINT32(0x9000A000, val);

// Test mode select
//#define DIRECT_MMIO_TEST 1

#define SPI_WRITE_UINT32(reg, val) HAL_WRITE_UINT32(reg, val);

#define SPI_READ_UINT32(reg, val)  HAL_READ_UINT32(reg, val);

extern char Getch(void);

// Serial Flash SPI device struct
static cyg_spi_sis910_device_t spi_dataflash_dev0 CYG_SPI_DEVICE_ON_BUS(0) =
{
    .spi_device.spi_bus = &cyg_spi_sis910_bus.spi_bus,

    .dev_num     = 2,       // Device number
    .cl_pol      = 0,       // Clock polarity (0 or 1)
    .cl_pha      = 0,       // Clock phase (0 or 1)
    .cl_brate    = 12000000, // Clock baud rate
    .ass         = 1,       // Automatic drop CS
    .cs_up_udly  = 1,       // Delay in usec between CS up and transfer start
    .cs_dw_udly  = 1,       // Delay in usec between transfer end and CS down
    .tr_bt_udly  = 1        // Delay in usec between two transfers
};

cyg_spi_device *cyg_spi_dataflash_dev0 = &spi_dataflash_dev0.spi_device;

void donop(cyg_uint32 number)
{
    cyg_uint32 i;

    for (i = 0; i < number; i++)
    {
        //diag_printf("Do NOP.\n");
        asm("nop");
    }
}

void reg_dump(void)
{
    cyg_uint32 val;

    // Registers Dump
    diag_printf("======== Registers Dump ========\n");
    SPI_READ_UINT32(REG_SIS910_SPI_Rx0, val);
    diag_printf("0x90002420 Tx0/Rx0 = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_Rx1, val);
    diag_printf("0x90002424 Tx1/Rx1 = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_Rx2, val);
    diag_printf("0x90002428 Tx2/Rx2 = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_Rx3, val);
    diag_printf("0x9000242C Tx3/Rx3 = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    diag_printf("0x90002430 CTRL    = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_DIVIDER, val);
    diag_printf("0x90002434 DIVIDER = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_SS, val);
    diag_printf("0x90002438 SS      = 0x%08X\n", val);
    SPI_READ_UINT32(REG_SIS910_SPI_MODE, val);
    diag_printf("0x9000243C MODE    = 0x%08X\n", val);
    diag_printf("================================\n");
}

void send_RDID(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, (INST_RDID << 24));
    diag_printf("Set INST_RDID to Tx.\n");
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_4_BYTES);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_RDID.\n");
    tx_data[0] = INST_RDID;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 4, &tx_data[0], &rx_data[0]);
    // Print out data received
    diag_printf("rx_data[0] = 0x%02X\n", rx_data[0]);
    diag_printf("rx_data[1] = 0x%02X\n", rx_data[1]);
    diag_printf("rx_data[2] = 0x%02X\n", rx_data[2]);
    diag_printf("rx_data[3] = 0x%02X\n", rx_data[3]);
#endif
}

void send_RDSR(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, (INST_RDSR << 16));
    diag_printf("Set INST_RDSR to Tx.\n");
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_3_BYTES);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_RDSR.\n");
    tx_data[0] = INST_RDSR;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 3, &tx_data[0], &rx_data[0]);
    // Print out data received
    diag_printf("rx_data[0] = 0x%02X\n", rx_data[0]);
    diag_printf("rx_data[1] = 0x%02X\n", rx_data[1]);
    diag_printf("rx_data[2] = 0x%02X\n", rx_data[2]);
    diag_printf("rx_data[3] = 0x%02X\n", rx_data[3]);
#endif
}

void send_WREN(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, INST_WREN);
    diag_printf("Set INST_WREN to Tx.\n");
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_1_BYTE);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_WREN.\n");
    tx_data[0] = INST_WREN;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 1, &tx_data[0], &rx_data[0]);
#endif
}

void send_WRDI(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, INST_WRDI);
    diag_printf("Set INST_WRDI to Tx.\n");
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_1_BYTE);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_WRDI.\n");
    tx_data[0] = INST_WRDI;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 1, &tx_data[0], &rx_data[0]);
#endif
}

void send_SE(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    diag_printf("Set INST_SE to Tx.\n");
    val = (INST_SE << 24) | 0x1000;
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, val);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_4_BYTES);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_SE.\n");
    tx_data[0] = INST_SE;
    tx_data[2] = 0x10;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 4, &tx_data[0], &rx_data[0]);
#endif
}

void send_BE(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    diag_printf("Set INST_BE to Tx.\n");
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, INST_BE);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_1_BYTE);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_BE.\n");
    tx_data[0] = INST_BE;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 1, &tx_data[0], &rx_data[0]);
#endif
}

void send_PP(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    diag_printf("Set INST_PP to Tx.\n");
    val = (INST_PP << 16) | 0x10;
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx3, val);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx3, val);
    diag_printf("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);
    val = 0x00113355;
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx2, val);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx2, val);
    diag_printf("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
    val = 0x22446688;
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx1, val);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx1, val);
    diag_printf("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
    val = 0x0F0F0F0F;
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, val);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_15_BYTES);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    diag_printf("Send INST_PP.\n");
    // Inst
    tx_data[0] = INST_PP;
    // Addr
    tx_data[1] = 0x00;
    tx_data[2] = 0x10;
    tx_data[3] = 0x00;
    // Data Byte 1
    tx_data[4] = 0x11;
    // Data Byte 2
    tx_data[5] = 0x22;
    // Data Byte 3
    tx_data[6] = 0x33;
    // Data Bytes ...
    tx_data[7] = 0x44;
    tx_data[8] = 0x55;
    tx_data[9] = 0x66;
    tx_data[10] = 0x77;
    tx_data[11] = 0x88;
    tx_data[12] = 0x99;
    tx_data[13] = 0xAA;
    tx_data[14] = 0xBB;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 15, &tx_data[0], &rx_data[0]);
#endif
}

void send_READ(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    diag_printf("Set INST_READ to Tx.\n");
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx3, (INST_READ << 24)|0x1000);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx3, val);
    diag_printf("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx2, 0x0);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx2, val);
    diag_printf("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx1, 0x0);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx1, val);
    diag_printf("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, 0x0);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_16_BYTES);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);
    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[32] = {0}, rx_data[32] = {0};
    cyg_uint8 i;

    diag_printf("Send INST_READ.\n");
    tx_data[0] = INST_READ;

    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 16, &tx_data[0], &rx_data[0]);
    // Print out data received
    for (i = 0; i < 16; i++)
    {
        diag_printf("rx_data[%d] = 0x%02X\n", i, rx_data[i]);
    }
#endif
}

void send_FAST_READ(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    // Set the data to transfer
    diag_printf("Set INST_FAST_READ to Tx.\n");
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx3, (INST_FAST_READ << 24)|0x1000);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx3, val);
    diag_printf("REG_SIS910_SPI_Tx3 = 0x%08X\n", val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx2, 0x0);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx2, val);
    diag_printf("REG_SIS910_SPI_Tx2 = 0x%08X\n", val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx1, 0x0);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx1, val);
    diag_printf("REG_SIS910_SPI_Tx1 = 0x%08X\n", val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, 0x0);
    SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
    diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);


    // Set the data size is 8 bits and Interrupt Enable
    ctrl_val = 0x0;
    ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                 SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_16_BYTES);

    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

    // Set Chip Select (Slave Select) to 1
    SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

    // Start the data transfer
    ctrl_val |= CTRL_SIS910_SPI_GO;
    diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
    SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
    // Polling busy bit
    do {
        SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
    } while (val & CTRL_SIS910_SPI_BUSY);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};
    cyg_uint8 i;

    diag_printf("Send INST_FAST_READ.\n");
    tx_data[0] = INST_FAST_READ;
    tx_data[1] = 0x00;
    tx_data[2] = 0x10;
    tx_data[3] = 0x00;
    cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 16, &tx_data[0], &rx_data[0]);
    // Print out data received
    for (i = 0; i < 16; i++)
    {
        diag_printf("rx_data[%d] = 0x%02X\n", i, rx_data[i]);
    }
#endif
}

void check_WIP(void)
{
#ifdef DIRECT_MMIO_TEST
    cyg_uint32 val = 0xFFFFFFFF;
    cyg_uint32 ctrl_val = 0x0;

    do {
        // Do some NOPs
        //donop(200);

        // Set the data to transfer
        SPI_WRITE_UINT32(REG_SIS910_SPI_Tx0, (INST_RDSR << 8));
        //diag_printf("Set INST_RDSR to Tx.\n");
        SPI_READ_UINT32(REG_SIS910_SPI_Tx0, val);
        //diag_printf("REG_SIS910_SPI_Tx0 = 0x%08X\n", val);

        // Set the data size is 8 bits and Interrupt Enable
        ctrl_val = 0x0;
        ctrl_val |= (CTRL_SIS910_SPI_ASS | CTRL_SIS910_SPI_IE |
                     SPI_MODE_ZERO | CTRL_SIS910_SPI_CHAR_LEN_2_BYTES);

        SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);

        // Set Chip Select (Slave Select) to 1
        SPI_WRITE_UINT32(REG_SIS910_SPI_SS, 0x2);

        // Start the data transfer
        ctrl_val |= CTRL_SIS910_SPI_GO;
        //diag_printf("REG_SIS910_SPI_CTRL = 0x%08X\n", ctrl_val);
        SPI_WRITE_UINT32(REG_SIS910_SPI_CTRL, ctrl_val);
        // Polling busy bit
        do {
            SPI_READ_UINT32(REG_SIS910_SPI_CTRL, val);
        } while (val & CTRL_SIS910_SPI_BUSY);

        // Registers Dump
        diag_printf("Check status for memory programming...\n");
        SPI_READ_UINT32(REG_SIS910_SPI_Rx0, val);
        diag_printf("Status Register = 0x%02X\n", val);
    } while (val & 0x1);
#else
    cyg_uint8 tx_data[16] = {0}, rx_data[16] = {0};

    do {
        tx_data[0] = INST_RDSR;
        cyg_spi_transfer(cyg_spi_dataflash_dev0, false, 3, &tx_data[0], &rx_data[0]);
        // Print out data received
        diag_printf("rx_data[0] = 0x%02X\n", rx_data[0]);
        diag_printf("rx_data[1] = 0x%02X\n", rx_data[1]);
        diag_printf("rx_data[2] = 0x%02X\n", rx_data[2]);
        diag_printf("rx_data[3] = 0x%02X\n", rx_data[3]);

        diag_printf("Check status for memory programming...\n");
        diag_printf("Status Register = 0x%02X\n", rx_data[1]);
    } while (rx_data[1] & 0x1);
#endif
}

//
// Main starting point for the application.
//
void cyg_user_start( void )
{
#ifdef DIRECT_MMIO_TEST
    //LED_DEBUG(0x0);

    diag_printf("It's MMIO SPI test.\n");

    // Select general SPI path
    SPI_WRITE_UINT32(REG_SIS910_SPI_MODE, 0x1);

    // Set the DIVIDER of frequence
    SPI_WRITE_UINT32(REG_SIS910_SPI_DIVIDER, 0x0);

    // Send RDID
    send_RDID();

    // Registers Dump
    reg_dump();

    // Send RDID
    send_RDID();

    // Registers Dump
    reg_dump();

    // Send RDID
    send_RDID();

    // Registers Dump
    reg_dump();

    // Send RDID
    send_RDID();

    // Registers Dump
    reg_dump();

    // Pause for a while
    //Getch();

    // Send RDSR
    send_RDSR();

    // Registers Dump
    reg_dump();

    // Send WREN
    send_WREN();

    // Registers Dump
    reg_dump();

    // Send RDSR
    send_RDSR();

    // Registers Dump
    reg_dump();

    // Pause for a while
    //Getch();

    // send SE
    //send_SE();

    // send BE
    send_BE();

    // Registers Dump
    reg_dump();

    // Wait program done
    check_WIP();

    // send READ
    send_READ();

    // Registers Dump
    reg_dump();

    // Pause for a while
    //Getch();

    // Send RDSR
    send_RDSR();

    // Registers Dump
    reg_dump();

    // Send WREN
    send_WREN();

    // Registers Dump
    reg_dump();

    // Send RDSR
    send_RDSR();

    // Registers Dump
    reg_dump();

    // Pause for a while
    //Getch();

    // send PP
    send_PP();

    // Registers Dump
    reg_dump();

    // Wait program done
    check_WIP();

    // Send WRDI
    send_WRDI();

    // Registers Dump
    reg_dump();

    // Pause for a while
    //Getch();

    // send READ
    send_READ();

    // Registers Dump
    reg_dump();

    // Send FAST_READ
    //send_FAST_READ();
#else
    diag_printf("It's SPI driver test.\n");

    // Send RDID
    send_RDID();

    // Pause for a while
    diag_printf("Press any key to continue.\n");
    Getch();

    // Send WREN
    send_WREN();

    // Pause for a while
    diag_printf("Press any key to continue.\n");
    Getch();

    // Send RDSR
    send_RDSR();

    // Pause for a while
    diag_printf("Press any key to continue.\n");
    Getch();

    // send BE
    send_BE();

    // Wait program done
    check_WIP();

    // send READ
    send_READ();

    // Registers Dump
    reg_dump();

    // Pause for a while
    diag_printf("Press any key to continue.\n");
    Getch();

    // Send WREN
    send_WREN();

    // Pause for a while
    diag_printf("Press any key to continue.\n");
    Getch();

    // Send RDSR
    send_RDSR();

    // Pause for a while
    diag_printf("Press any key to continue.\n");
    Getch();

    // send PP
    send_PP();

    // Wait program done
    check_WIP();

    // send READ
    send_READ();

    // Registers Dump
    reg_dump();
#endif
}
