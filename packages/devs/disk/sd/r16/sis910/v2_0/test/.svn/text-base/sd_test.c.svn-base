#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
// To run SD test
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>

// To test FAT
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>
#include <cyg/fs/fatfs.h>
#include <cyg/fileio/fileio.h>
// For performance evaluation
#include <pkgconf/system.h>
#include <cyg/infra/testcase.h>
#include <pkgconf/kernel.h>
#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_if.h>             // delays

// Gateway definition
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
#define gw_wt3_init0        0x90001430
#define gw_wt3_init1        0x90001434
#define gw_rd3_init0        0x900014b0
#define gw_rd3_init1        0x900014b4
#define gw_irq              0x90001700
#define dev_peri_irqst      0x90003614
#define gw_wff_flush        0x90001714
#define padreg1				0x90004204
#define cat2(i,j)           i##j
#define cat3(i,j,k)         i##j##k
#define DIR_SD2G            (1<<12)

// SD card type
#define UNUSED           0
#define SD_1_x           1
#define SD_2_00          2
#define SDHC             3

// LED debug
#define LED_DEBUG(val) HAL_WRITE_UINT32(0x9000A000, val);

// Test mode select
//#define DIRECT_MMIO_TEST 1
#define DAFAULT_BUFFER_SIZE 4096

// For performance evaluation
#define NTHREADS 		1
#define STACKSIZE 		4096
#define TICK_DELAY 		(1500000 / CYGNUM_HAL_RTC_DENOMINATOR)// This is (tick time * 1.5) 
//static cyg_handle_t thread[NTHREADS];
//static cyg_thread thread_obj[NTHREADS];
//static char stack[NTHREADS][STACKSIZE];
void cyg_tick2time(cyg_tick_count_t t);

#define THREAD_STACK_SIZE	( 8192 / sizeof(int) )
#define THREAD_PRIORITY		12
#define TICKS_PER_SECOND	100

int thread_a_stack[ THREAD_STACK_SIZE ];
cyg_thread thread_a_obj;
cyg_handle_t thread_a_hdl;
//static void thread_a( cyg_addrword_t data );
//cyg_sem_t sem_signal_thread;
void wait_for_tickup(void);
int thread_a_count;

extern char Getch(void);

#ifdef DIRECT_MMIO_TEST
static cyg_uint8 card_type = UNUSED;

static Cyg_ErrNo
send_cmd(cyg_uint16 cmd, cyg_uint16 argh, cyg_uint16 argl, cyg_uint16 cmd_dat_cont)
{
    cyg_uint16 val = 0x0;
    Cyg_ErrNo code = ENOERR;

    diag_printf("Prepare to send CMD %d.\n", cmd);
    diag_printf(" argh = %x.\n", argh);
    diag_printf(" argl = %x.\n", argl);
    diag_printf(" cmd_dat_cont = %x.\n", cmd_dat_cont);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_0, cmd);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_1, argh);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_2, argl);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_DAT, cmd_dat_cont);

    // Check busy or not
    do {
        HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
        diag_printf(" REG_SIS910_SD_STATUS = %X\n", val);
        diag_printf(" busy...\n");
    } while (!(val & STATUS_END_CMD_RESPONSE));

    // Check response timeout or not
    HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
    while (val & STATUS_RESPONSE_TIMEOUT)
    {
        // No response return!!
        diag_printf(" REG_SIS910_SD_STATUS = %X\n", val);
        diag_printf(" Response Timeout!!\n");
        return -EIO;
    }

    return code;
}

void send_cmd_no_resp(cyg_uint16 cmd, cyg_uint16 argh, cyg_uint16 argl, cyg_uint16 cmd_dat_cont)
{
    cyg_uint16 val = 0x0;

    diag_printf("Prepare to send CMD %d.\n", cmd);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_0, cmd);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_1, argh);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_2, argl);
    HAL_WRITE_UINT32(REG_SIS910_SD_CMD_DAT, cmd_dat_cont);

    // Check busy or not
    do {
        HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
        diag_printf(" REG_SIS910_SD_STATUS = %X\n", val);
        diag_printf(" busy...\n");
    } while (!(val & STATUS_END_CMD_RESPONSE));
}

static Cyg_ErrNo
cpu_read_singleblk(cyg_uint32 RCA, cyg_uint16 blk_length, cyg_uint32 addr)
{
    cyg_uint16 argh = 0x0, argl = 0x0;
    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;
    Cyg_ErrNo rtn_code;

    diag_printf("cpu_read_singleblk()\n");

    // Read 'blk_length' bytes through CPU
    // Set block length to 'blk_length' bytes
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, blk_length);
    // Set read through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x10);

    // Send CMD 16
    // Set BLOCKLEN to 'blk_length' bytes
    argh = 0x0;
    argl = blk_length;
    rtn_code = send_cmd(MMC_REQUEST_SET_BLOCKLEN, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
    diag_printf("CMD 16 sent.\n");

    if (rtn_code == ENOERR)
    {
        ;
    }
    // Send CMD 17
    // Read single block form 'addr'
    if ((card_type == SD_1_x) || (card_type == SD_2_00))
    {
        argh = (addr * 512) >> 16;
        argl = (addr * 512);
    }
    else
    {   // SDHC
        argh = addr >> 16;
        argl = addr;
    }

    rtn_code = send_cmd(MMC_REQUEST_READ_SINGLE_BLOCK, argh, argl,
                       (CMD_DAT_CONT_DATA_ENABLE | CMD_DAT_CONT_R1_RESPONSE));
    if (rtn_code == ENOERR)
    {
        cyg_uint32 i;
        cyg_uint16 val = 0x0;

        // Read data from SD_DATA_BUFFERS register
        diag_printf("=======================================================\n");
        diag_printf("                     DATA READ BACK                    \n");
        diag_printf("=======================================================\n");

        for (i = 0; i < (blk_length / 2); i++)
        {
            //diag_printf("  i = %d\n", i);
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
                //diag_printf("  REG_SIS910_SD_CLK_RATE = %X\n", val);
            } while (!(val & 0x40)); // Xport ready
            // Read data
            HAL_READ_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);

            if ((i % 8) == 7)
            {
                diag_printf("0x%04X\n", val);
            }
            else
            {
                diag_printf("0x%04X ", val);
            }
        }
        diag_printf("=======================================================\n");

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            diag_printf("  REG_SIS910_SD_STATUS = %X\n", val);
        } while (!(val & 0x800));

        do {
            // End command response
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            diag_printf("  REG_SIS910_SD_STATUS = %X\n", val);
        } while (!(val & 0x2000));

        // Wait SD card go back to tran state
        argh = RCA;
        argl = 0x0;
        rtn_code = send_cmd(MMC_REQUEST_SEND_STATUS, argh, argl, CMD_DAT_CONT_R1_RESPONSE);

        if (rtn_code == ENOERR)
        {
            cyg_uint32 card_status;
            cyg_uint32 current_state;

            do {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
                current_state = card_status >> 9;
                diag_printf(" current_state = %x\n", current_state);
            } while (current_state != 4);
        }
    }
    return ENOERR;
}

static Cyg_ErrNo
cpu_write_singleblk(cyg_uint32 RCA, cyg_uint16 blk_length, cyg_uint32 addr)
{
    cyg_uint16 argh = 0x0, argl = 0x0;
    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;
    cyg_uint8 data[512];
    Cyg_ErrNo rtn_code;
    cyg_uint32 i;

    diag_printf("cpu_write_singleblk()\n");

    // Prepare data for writing test
    for (i = 0; i < 256; i++)
    {
        diag_printf("  i = %d\n", i);
        data[i] = i;
        diag_printf("    data[%d] = 0x%02X; ", i, data[i]);
        data[i + 256] = i;
        diag_printf("    data[%d] = 0x%02X\n", (i + 256), data[i + 256]);
    }

    // Write 'blk_length' bytes through CPU
    // Set block length to 'blk_length' bytes
    HAL_WRITE_UINT32(REG_SIS910_SD_BLK_LEN, blk_length);
    // Set write through CPU
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x20);

    // Send CMD 16
    // Set BLOCKLEN to 'blk_length' bytes
    argh = 0x0;
    argl = blk_length;
    rtn_code = send_cmd(MMC_REQUEST_SET_BLOCKLEN, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
    diag_printf("CMD 16 sent.\n");

    if (rtn_code == ENOERR)
    {
        ;
    }
    // Send CMD 24
    // Read single block form 'addr'
    if ((card_type == SD_1_x) || (card_type == SD_2_00))
    {
        argh = (addr * 512) >> 16;
        argl = (addr * 512);
    }
    else
    {   // SDHC
        argh = addr >> 16;
        argl = addr;
    }

    rtn_code = send_cmd(MMC_REQUEST_WRITE_BLOCK, argh, argl,
                       (CMD_DAT_CONT_DATA_WRITE |
                        CMD_DAT_CONT_DATA_ENABLE |
                        CMD_DAT_CONT_R1_RESPONSE));
    diag_printf("CMD 24 sent.\n");
    if (rtn_code == ENOERR)
    {
        cyg_uint32 i;
        cyg_uint16 val = 0x0;

        // Set buffer ready signal and last buffer
        HAL_WRITE_UINT32(REG_SIS910_SD_BUF_RDY, 0x3);
        diag_printf("  Set buffer ready signal and last buffer.\n");

        for (i = 0; i < (blk_length / 2); i++)
        {
            diag_printf("  i = %d\n", i);
            do {
                HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
                //diag_printf("  REG_SIS910_SD_CLK_RATE = %X\n", val);
            } while (val & 0x80);
            // Write data
            val = (data[i] << 8) | data[511 - i];
            diag_printf("  Data to write is 0x%04X\n", val);
            HAL_WRITE_UINT32(REG_SIS910_SD_DATA_BUFFERS, val);
            diag_printf("  Writing data...\n");
        }

        do {
            // Wait data transfer done
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            diag_printf("  REG_SIS910_SD_STATUS = %X\n", val);
        } while (!(val & 0x800));

        do {
            // End command response
            HAL_READ_UINT32(REG_SIS910_SD_STATUS, val);
            diag_printf("  REG_SIS910_SD_STATUS = %X\n", val);
        } while (!(val & 0x2000));

        // Wait SD card go back to tran state
        argh = RCA;
        argl = 0x0;
        rtn_code = send_cmd(MMC_REQUEST_SEND_STATUS, argh, argl, CMD_DAT_CONT_R1_RESPONSE);

        if (rtn_code == ENOERR)
        {
            cyg_uint32 card_status;
            cyg_uint32 current_state;

            do {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                card_status = (((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8));
                current_state = card_status >> 9;
                diag_printf(" current_state = %x\n", current_state);
            } while (current_state != 4);
        }
    }
    return ENOERR;
}

#else
void gateway_read_test(void)
{
    cyg_uint32 i;
    cyg_io_handle_t hSD;
    cyg_uint8 *buf = NULL, *obuf = NULL;
    cyg_uint32 bread_len;
    cyg_uint32 bread_pos;
    Cyg_ErrNo err;

    diag_printf("Prepare to test read SD card through Gateway.\n");
    err = cyg_io_lookup("/dev/sdblk0/", &hSD);
    if (err == ENOERR)
    {
        buf = malloc(2100);
        if (buf)
        {
            cyg_uint32 offset;
//            cyg_uint32 config_val = 1;
//            cyg_uint32 config_val_len = sizeof(cyg_uint32);

            diag_printf("Original buf address = 0x%08X\n", buf);
            obuf = buf;
            offset = (cyg_uint32)buf % 32;
            buf += (32 - offset);
            diag_printf("Shifted buf address = 0x%08X\n", buf);

            // Set SD driver to use gateway
            diag_printf("Set SD driver to use gateway.\n");
//            cyg_io_set_config(hSD, CYG_IO_SET_CONFIG_SD_GATEWAY_XFER, &config_val, &config_val_len);

            diag_printf("Single Block Read Test.\n");
            bread_len = 0x1;
            bread_pos = 0x0;
            err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
            if (err == ENOERR)
            {
                diag_printf("cyg_io_bread() success.\n");
                diag_printf("=======================================\n");
                diag_printf("             DATA READ BACK            \n");
                diag_printf("=======================================\n");
                for (i = 0; i < 512; i++)
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
            }
            else
            {
                diag_printf("cyg_io_bread() err = %d\n", err);
            }

            diag_printf("Multiple Blocks Read Test.\n");
            bread_len = 0x2;
            bread_pos = 0x0;
            err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
            if (err == ENOERR)
            {
                diag_printf("cyg_io_bread() success.\n");
                diag_printf("=======================================\n");
                diag_printf("             DATA READ BACK            \n");
                diag_printf("=======================================\n");
                for (i = 0; i < (512 * bread_len); i++)
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
            }
            else
            {
                diag_printf("cyg_io_bread() err = %d\n", err);
            }

            free(obuf);
            diag_printf("Test finished.\n");
        }
        else
        {
            diag_printf("Allocate memory fails.\n");
        }
    }
    else
    {
        diag_printf("cyg_io_lookup() err = %d\n", err);
    }
}

void cpu_read_test(void)
{
    cyg_uint32 i;
    cyg_io_handle_t hSD;
    cyg_uint8 *buf = NULL, *obuf = NULL;
    cyg_uint32 bread_len;
    cyg_uint32 bread_pos;
    Cyg_ErrNo err;

    diag_printf("Prepare to test read SD card through CPU.\n");
    err = cyg_io_lookup("/dev/sdblk0/", &hSD);
    if (err == ENOERR)
    {
        buf = malloc(2048);
        if (buf)
        {
//            cyg_uint32 config_val = 0;
//            cyg_uint32 config_val_len = sizeof(cyg_uint32);

            obuf = buf;

            // Set SD driver to use CPU
            diag_printf("Set SD driver to use CPU.\n");
//            cyg_io_set_config(hSD, CYG_IO_SET_CONFIG_SD_GATEWAY_XFER, &config_val, &config_val_len);

            diag_printf("Single Block Read Test.\n");
            bread_len = 0x1;
            bread_pos = 0x0;
            err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
            if (err == ENOERR)
            {
                diag_printf("cyg_io_bread() success.\n");
                diag_printf("=======================================\n");
                diag_printf("             DATA READ BACK            \n");
                diag_printf("=======================================\n");
                for (i = 0; i < 512; i++)
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
            }
            else
            {
                diag_printf("cyg_io_bread() err = %d\n", err);
            }

            diag_printf("Multiple Blocks Read Test.\n");
            bread_len = 0x2;
            bread_pos = 0x0;
            err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
            if (err == ENOERR)
            {
                diag_printf("cyg_io_bread() success.\n");
                diag_printf("=======================================\n");
                diag_printf("             DATA READ BACK            \n");
                diag_printf("=======================================\n");
                for (i = 0; i < (512 * bread_len); i++)
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
            }
            else
            {
                diag_printf("cyg_io_bread() err = %d\n", err);
            }

            free(obuf);
            diag_printf("Test finished.\n");
        }
        else
        {
            diag_printf("Allocate memory fails.\n");
        }
    }
    else
    {
        diag_printf("cyg_io_lookup() err = %d\n", err);
    }
}
#endif

//
// Main starting point for the application.
//
#ifdef DIRECT_MMIO_TEST
void cyg_user_start( void )
{
    LED_DEBUG(0x1);
    cyg_uint16 val = 0x0;
    cyg_uint16 argh = 0x0, argl = 0x0;
    cyg_uint16 reply_1 = 0x0, reply_2 = 0x0, reply_3 = 0x0;//, reply_4 = 0x0, reply_5 = 0x0;
    Cyg_ErrNo rtn_code = ENOERR;
    cyg_uint32 OCR, RCA;
    mmc_csd_register CSD;
    mmc_cid_register CID;

    diag_printf("Hello eCos World.\n");

    HAL_WRITE_UINT32(padreg1, 2); // SD pad enable
    HAL_WRITE_UINT32(dev_rarb_ctrl, 0x32410);    /* ch 6 select sd card (4) */
    HAL_WRITE_UINT32(dev_dir_ctrl, DIR_SD2G);        /* ideh ->gateway arb*/
    HAL_WRITE_UINT32(dev_warb_ctrl1, 0xf6ffff01); // modify for new peri and gateway write arbiter setting

    /* Enable Gateway FIFO: 256-byte per channel */
    HAL_WRITE_UINT32(gw_fifo_reset, 0xffffffff);
    HAL_WRITE_UINT32(gw_fifo_reset, 0x0);
    HAL_WRITE_UINT32(gw_wff_val0, 0x06000608);
    HAL_WRITE_UINT32(gw_wff_val1, 0x06100618);

    HAL_WRITE_UINT32(gw_rff_val0, 0x08000808);
    HAL_WRITE_UINT32(gw_rff_val1, 0x08100818);

    HAL_WRITE_UINT32(gw_lpc0,  0);
    HAL_WRITE_UINT32(gw_lpc1,  0);
    HAL_WRITE_UINT32(gw_lpc2,  0);
    HAL_WRITE_UINT32(gw_lpc3,  0);
    HAL_WRITE_UINT32(gw_lpc4,  0);
    HAL_WRITE_UINT32(gw_lpc5,  0);
    HAL_WRITE_UINT32(gw_lpc6,  0);
    HAL_WRITE_UINT32(gw_lpc7,  0);

    // Set clock rate
    HAL_WRITE_UINT32(REG_SIS910_SD_CLK_RATE, 0x5);
    HAL_READ_UINT32(REG_SIS910_SD_CLK_RATE, val);
    diag_printf(" REG_SIS910_SD_CLK_RATE = %x\n", val);

    // Card initialization and identification
    // ==========================
    //  Card Type Identification
    // ==========================
    // Go to idle state
    // Send CMD 0
    send_cmd_no_resp(MMC_REQUEST_GO_IDLE_STATE, 0, 0, CMD_DAT_CONT_CARD_INIT);
    diag_printf("CMD 0 Sent.\n");

    // Check SD specification version
    // Send CMD 8, and R7 returns
    argh = 0x0;
    argl = (1 << 8) | 0xAA; // VHS = 0x1; check pattern = 0xAA;
    rtn_code = send_cmd(SD_REQUEST_SEND_IF_COND, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
    diag_printf("CMD 8 sent.\n");

    // Card returns response
    // SD 2.00
    if (rtn_code == ENOERR)
    {
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);

        if (((reply_2 & 0x000F) == 0x1) && ((reply_3 & 0xFF00) == 0xAA00))
        {
            diag_printf("Response to CMD8, and check ok.\n");
        }
        else
        {
            diag_printf("Response to CMD8, but check fail.\n");
            goto CARDFAILED;
        }

        // Send ACMD 41 with HCS = 1
        // CMD 55: APP_CMD
        // [31:16] RCA = 0x0000 in IDLE_STATE
        do {
            argh = 0x0;
            argl = 0x0;
            rtn_code = send_cmd(MMC_REQUEST_APP_CMD, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
            argh = 0x40FF;
            argl = 0x8000;
            rtn_code = send_cmd(SD_REQUEST_APP_OP_COND, argh, argl, CMD_DAT_CONT_R3_RESPONSE);
            diag_printf(" ACMD 41 sent.\n");

            if (rtn_code == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);

                OCR = ((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8);
            }
            else
            {
                diag_printf("No response to ACMD 41.\n");
                goto CARDFAILED;
            }
        } while (!(OCR & 0x80000000)); // Repeat until busy_bit in OCR is cleared

        // CSS in response?
        // CSS = 1
        // SD 2.00 High Capacity Card
        if ((OCR >> 30) == 0x3)
        {
            card_type = SDHC;
            diag_printf("It's SD 2.00 High Capacity Card.\n");
        }
        // CSS = 0
        // SD 2.00 Standard Capacity Card
        else if ((OCR >> 30) == 0x2)
        {
            card_type = SD_2_00;
            diag_printf("It's SD 2.00 Standard Capacity Card.\n");
        }
        else
        {
            diag_printf("No CCS in ACMD 41 response.\n");
            goto CARDFAILED;
        }
    }
    // No response
    // SD 1.x or not SD
    // Send ACMD 41 with HCS = 0
    else
    {
        diag_printf("No response to CMD8, and it's not SD 2.00 compatible.\n");
        // Send ACMD 41 with HCS = 0
        // CMD 55: APP_CMD
        // [31:16] RCA = 0x0000 in IDLE_STATE
        do {
            argh = 0x0;
            argl = 0x0;
            rtn_code = send_cmd(MMC_REQUEST_APP_CMD, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
            argh = 0x00FF;
            argl = 0x8000;
            rtn_code = send_cmd(SD_REQUEST_APP_OP_COND, argh, argl, CMD_DAT_CONT_R3_RESPONSE);
            diag_printf(" ACMD 41 sent.\n");

            if (rtn_code == ENOERR)
            {
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
                HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_3);
                OCR = ((reply_1 & 0x00FF) << 24) | (reply_2 << 8) | ((reply_3 & 0xFF00) >> 8);
                diag_printf(" OCR = %X\n", OCR);
            }
            else
            {
                diag_printf("No response to ACMD 41.\n");
                goto CARDFAILED;
            }
        } while (!(OCR & 0x80000000)); // Repeat until busy_bit in OCR is cleared
        card_type = SD_1_x;
        diag_printf("It's SD 1.x Standard Capacity Card.\n");
    }

    // ==========================
    //  Get CID
    // ==========================
    // Send CMD 2
    argh = 0x0;
    argl = 0x0;
    rtn_code = send_cmd(MMC_REQUEST_ALL_SEND_CID, argh, argl, CMD_DAT_CONT_R2_RESPONSE);
    diag_printf("CMD 2 sent.\n");

    if (rtn_code == ENOERR)
    {
        cyg_uint32 i;
        for (i = 0; i < 16; i += 2)
        {
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
            diag_printf("  reply = %X\n", reply_1);
            CID.cid_data[i] = ((reply_1 & 0xFF00) >> 8);
            CID.cid_data[i + 1] = reply_1 & 0x00FF;
        }
    }

    // ==========================
    //  Get RCA
    // ==========================
    // Send CMD 3
    argh = 0x0;
    argl = 0x0;
    rtn_code = send_cmd(SD_REQUEST_SEND_RELATIVE_ADDR, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
    diag_printf("CMD 3 sent.\n");

    if (rtn_code == ENOERR)
    {
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
        HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_2);
        RCA = ((reply_1 & 0x00FF) << 8) | ((reply_2 & 0xFF00) >> 8);
        diag_printf(" RCA = %X\n", RCA);
    }

    // ==========================
    //  Get CSD
    // ==========================
    // Send CMD 9
    argh = RCA;
    argl = 0x0;
    rtn_code = send_cmd(MMC_REQUEST_SEND_CSD, argh, argl, CMD_DAT_CONT_R2_RESPONSE);
    diag_printf("CMD 9 sent.\n");

    if (rtn_code == ENOERR)
    {
        cyg_uint32 i;
        for (i = 0; i < 16; i += 2)
        {
            HAL_READ_UINT32(REG_SIS910_SD_RES_REG, reply_1);
            diag_printf("  reply = %X\n", reply_1);
            CSD.csd_data[i] = ((reply_1 & 0xFF00) >> 8);
            CSD.csd_data[i + 1] = reply_1 & 0x00FF;
        }
    }

    // ==========================
    //  Move to Transfer State
    // ==========================
    // Send CMD 7
    argh = RCA;
    argl = 0x0;
    rtn_code = send_cmd(MMC_REQUEST_SELECT_DESELECT, argh, argl, CMD_DAT_CONT_R1_RESPONSE);
    diag_printf("CMD 7 sent.\n");

    if (rtn_code == ENOERR)
    {
        ;
    }

    // ==========================
//    //         Write Test
//    // ==========================
//    // Write a 512 bytes single block
//    diag_printf("=========================\n");
//    diag_printf(" Test single block Write \n");
//    diag_printf("=========================\n");
//    rtn_code = cpu_write_singleblk(RCA, 0x200, 0x1);
//    if (rtn_code == ENOERR)
//    {
//        // goto FINISHED;
//    }
//    else
//    {
//        goto FAILED;
//    }


    // ==========================
    //          Read Test
    // ==========================
    // Read a 512 bytes single block
    diag_printf("========================\n");
    diag_printf(" Test single block read \n");
    diag_printf("========================\n");
    rtn_code = cpu_read_singleblk(RCA, 0x200, 7868);
    if (rtn_code == ENOERR)
    {
        goto FINISHED;
    }
    else
    {
        goto FAILED;
    }

    // ================================
    //  Unusable Card / Error Handling
    // ================================
CARDFAILED:
FAILED:
    diag_printf("ERROR!!\n");

FINISHED:
    diag_printf("Test finished.\n");

    //LED_DEBUG(0xF);
}
#else
void
wait_for_tickup(void)
{
    cyg_tick_count_t tv0, tv1;
    
    tv0 = cyg_current_time();
    while (true) 
    {
        tv1 = cyg_current_time();
        if (tv1 != tv0) 
        	break;
    }
}

void 
cyg_tick2time(cyg_tick_count_t t)
{
	cyg_uint32 totalseconds = t/TICKS_PER_SECOND;
    cyg_uint32 tempseconds = totalseconds;
    cyg_uint32 day = 0;
    cyg_uint32 hour = 0;
    cyg_uint32 minute = 0;
    cyg_uint32 sec = 0;
    cyg_uint32 msec = 0;

	/* DAY */
    if(tempseconds >= 86400){
		day = (tempseconds / 86400);
		tempseconds -= (day * 86400);
    }
    /* HOUR */
    if(tempseconds >= 3600){
		hour = (tempseconds/3600);
		tempseconds -= (hour*3600);
    }
    /* MINUTE */
    if(tempseconds >= 60){
		minute = (tempseconds/60);
		tempseconds -= (minute*60);
    }
    /* SECOND */
    sec = tempseconds;
    
    /* 1/100 SECOND */
//	msec = (tempseconds*1000);
//	msec = (msec%1000)/10;
    msec = t%TICKS_PER_SECOND;
    
    if(day > 0){
        diag_printf("Elapsed time is %d day %d hour %d min %d.%02d sec.\n", day, hour, minute, sec, msec);
    } 
    else if(hour > 0){
        diag_printf("Elapsed time is %d hour %d min %d.%02d sec.\n", hour, minute, sec, msec);
    }
    else if(minute > 0){
        diag_printf("Elapsed time is %d min %d.%02d sec.\n", minute, sec, msec);
    }
    else{
        diag_printf("Elapsed time is %d.%02d sec.\n", sec, msec);
    }
}

void block_read(void)
{
    Cyg_ErrNo err;
    cyg_uint8 * buf = NULL;
    cyg_io_handle_t hSD;
    cyg_uint32 bread_len;
    cyg_uint32 bread_pos;
    cyg_uint32 i;

    diag_printf("Prepare to test read SecureDigital card.\n");
    err = cyg_io_lookup("/dev/sdblk0/", &hSD);
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
                memset(buf, 0xFF, 512);
                err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
                if (err != ENOERR)
                {
                    diag_printf("Read fails.\n");
                }
                else
                {
                    //diag_printf(".");
                    //diag_dump_buf_with_offset(buf, 512, buf);
                    diag_printf("==============================\n");
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
    cyg_io_handle_t hSD;
    cyg_uint32 bread_len;
    cyg_uint32 bread_pos;
    cyg_uint32 i;

    diag_printf("Prepare to test read SecureDigital card.\n");
    err = cyg_io_lookup("/dev/sdblk0/", &hSD);
    if (err == ENOERR)
    {
        diag_printf("Single Block Read/Write Test.\n");
        buf = malloc_align(512, 32);
        if (buf)
        {
//            for (i = 0; i < 512; i++)
//            {
//                *buf++ = i;
//            }

            for (i = 0; i < 20000; i++)
            {
//                diag_printf("-");
                memset(buf, 0xFF, 512);
                bread_len = 1;
                bread_pos = i;
                err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
                if (err != ENOERR)
                {
                    diag_printf("Read fails.\n");
                }
                else
                {
                    diag_printf("==== Original data ====\n");
                    diag_dump_buf_with_offset(buf, 512, buf);
                }
//                diag_printf("\\");

//                diag_printf("|");
                bread_len = 1;
                bread_pos = i;
                err = cyg_io_bwrite(hSD, buf, &bread_len, bread_pos);
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
                memset(buf, 0xFF, 512);
                bread_len = 1;
                bread_pos = i;
                err = cyg_io_bread(hSD, buf, &bread_len, bread_pos);
                if (err != ENOERR)
                {
                    diag_printf("Read fails.\n");
                }
                else
                {
                    diag_printf("==== Data after write ====\n");
                    diag_dump_buf_with_offset(buf, 512, buf);
                }
                diag_printf("Press any key to continue.\n");
                Getch();
            }
            free(buf);
        }
    }
    diag_printf("Test Finished.\n");
}

static void
file_copy(void)
{
    Cyg_ErrNo err;
    cyg_bool disk_mounted = false;
    char *buf = NULL;
    FILE *fp = NULL, *copy = NULL;
    int ret = 0;
    cyg_tick_count_t t1, t2, ticks;

    diag_printf("Prepare to mount SecureDigital disk on partition 0.\n");
    err = mount( "/dev/sdblk0/", "/", "fatfs" );
    if (err)
    {
        diag_printf(" Mount fails!; err = %d\n", err);
        diag_printf("Prepare to mount SecureDigital disk on partition 1.\n");
        err = mount( "/dev/sdblk0/1", "/", "fatfs" );
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

    diag_printf("File copy test.\n");
    if (disk_mounted)
    {
        cyg_uint32 count;
        wait_for_tickup();

        for (count = 0;  count < 10; count++)
        {
            diag_printf("[%02d] ", count);
            t1 = cyg_current_time();
    
//            diag_printf("Prepare to open input file.\n");
            fp = fopen("IN_FILE.JPG", "rb");
            if (fp == NULL)
            {
                diag_printf("Cannot open file for test.\n");
                goto FINISHED;
            }
//            diag_printf("Prepare to open output file.\n");
            copy = fopen("OUT_FILE.JPG", "wb");
            buf = (char *)malloc(DAFAULT_BUFFER_SIZE);
            if (buf)
            {
                ret = fread(buf, 1, DAFAULT_BUFFER_SIZE, fp);
//                diag_printf("Writing file");
                while(ret > 0)
                {
                    ret = fwrite(buf, 1, ret, copy);
                    if (ret > 0)
                    {
                        ret = fread(buf, 1, DAFAULT_BUFFER_SIZE, fp);
//                        diag_printf(".");
                    }
                }
                fclose(fp);
                fclose(copy);
                free(buf);
//                diag_printf("\n");
            }
            t2 = cyg_current_time();
            ticks = t2 - t1;
            cyg_tick2time(ticks);
        }
        
    }
FINISHED:
    diag_printf("Test Finished.\n");
}

static void
file_read(void)
{
    Cyg_ErrNo err;
    cyg_bool disk_mounted = false;
    char *buf = NULL;
    FILE *fp = NULL;
    int ret = 0;
    cyg_tick_count_t t1, t2, ticks;

    diag_printf("Prepare to mount SecureDigital disk on partition 0.\n");
    err = mount( "/dev/sdblk0/", "/", "fatfs" );
    if (err)
    {
        diag_printf(" Mount fails!; err = %d\n", err);
        diag_printf("Prepare to mount SecureDigital disk on partition 1.\n");
        err = mount( "/dev/sdblk0/1", "/", "fatfs" );
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

    diag_printf("File read test.\n");
    if (disk_mounted)
    {
        cyg_uint32 count;
        wait_for_tickup();

        for (count = 0;  count < 1; count++)
        {
            diag_printf("[%02d] ", (count + 1));
            t1 = cyg_current_time();
    
//            diag_printf("Prepare to open input file.\n");
            fp = fopen("IN_FILE.JPG", "rb");
            if (fp == NULL)
            {
                diag_printf("Cannot open file for test.\n");
                goto FINISHED;
            }
            buf = (char *)malloc(DAFAULT_BUFFER_SIZE);
            if (buf)
            {
                ret = fread(buf, 1, DAFAULT_BUFFER_SIZE, fp);
//                diag_printf("Reading file");
                while(ret > 0)
                {
                    ret = fread(buf, 1, DAFAULT_BUFFER_SIZE, fp);
//                        diag_printf(".");
                }
                free(buf);
//                diag_printf("\n");
            }
            fclose(fp);
            t2 = cyg_current_time();
            ticks = t2 - t1;
            cyg_tick2time(ticks);
        }
        
    }
FINISHED:
    diag_printf("Test Finished.\n");
}

static void
mount_test(void)
{
    Cyg_ErrNo err;
    cyg_tick_count_t t1, t2, ticks;
    cyg_bool part0 = false, part1 = false;

    do {
        err = mount( "/dev/sdblk0/", "/", "fatfs" );
        if (err)
        {
            part0 = false;
//            diag_printf("Partition #0 mount fails!; err = %d\n", err);
            err = mount( "/dev/sdblk0/1", "/", "fatfs" );
            if (err)
            {
                part1 = false;
//                diag_printf("Partition #1 mount fails!; err = %d\n", err);
            }
            else
            {
                part1 = true;
//                diag_printf("Partition #1 mount success!\n");
                err = umount("/");
//                diag_printf("umount err = %d\n", err);
            }
        }
        else
        {
            part0 = true;
//            diag_printf("Partition #0 mount success!\n");
            err = umount("/");
//            diag_printf("umount err = %d\n", err);
        }

        if (part0 || part1)
        {
            diag_printf("We have a card.\n");
        }
        else
        {
            diag_printf("\n");
        }

        // Do some delays
        t1 = cyg_current_time();
        do {
            t2 = cyg_current_time();
            ticks = t2 - t1;
        } while (ticks < (1 * 100));
    } while (true);
}

void cyg_user_start(void)
{
#if 0
	thread_a_count = 0;
	
//	cyg_semaphore_init( &sem_signal_thread, 0 );
		    
	cyg_thread_create(
			THREAD_PRIORITY,
			(cyg_thread_entry_t*)file_read,
			(cyg_addrword_t)0,
			"File Read Thread",
			(void *)thread_a_stack,
			THREAD_STACK_SIZE,
		    &thread_a_hdl,
		    &thread_a_obj );
		      
	cyg_thread_resume(thread_a_hdl);
	
	cyg_scheduler_start();
    //file_copy();
    //block_write();
    //block_read();
#endif
#if 1
	cyg_thread_create(
			THREAD_PRIORITY,
			(cyg_thread_entry_t*)mount_test,
			(cyg_addrword_t)0,
			"Mount Test Thread",
			(void *)thread_a_stack,
			THREAD_STACK_SIZE,
		    &thread_a_hdl,
		    &thread_a_obj );
		      
	cyg_thread_resume(thread_a_hdl);
	
	cyg_scheduler_start();
#endif
}
#endif
