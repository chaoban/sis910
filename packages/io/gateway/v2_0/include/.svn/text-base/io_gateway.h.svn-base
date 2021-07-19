#ifndef __SIS_IO_OR__
#define	__SIS_IO_OR__

#define MAX_READ_CH_NUM                     5
#define MAX_WRITE_CH_NUM                    3
#define MAX_CH_NUM                          8
#define MUTEX_CH_SHT                        4                    
cyg_mutex_t ch_mutex_lock[MAX_CH_NUM];

static cyg_uint32 baseR[5] = {0,6,12,18,24},baseW[4]={0,0,10,20};
static cyg_uint32 sizeR[5] = {6,6,6,8,6},sizeW[4] = {0,10,10,12};  
#define CH_READ_VDEC                        0
#define CH_READ_I2S                         1
#define CH_READ_USB                         2
#define CH_READ_SD                          3
#define CH_READ_MS                          4

#define CH_WRITE_MS                         1
#define CH_WRITE_USB                        2
#define CH_WRITE_SD                         3

#define DEVICE_VDEC                         0
#define DEVICE_I2S                          1
#define DEVICE_USB                          2
#define DEVICE_SD                           3
#define DEVICE_MS                           4
#define QUERY_CHANNEL_STATUS                11
#define SET_FIFOs_START_SIZE                10

#define QUERY_CHANNEL_STATUS_WRITE          20
#define QUERY_CHANNEL_STATUS_READ           21
#define GATEWAY_RESET_CHANNEL_READ          22
#define GATEWAY_RESET_CHANNEL_WRITE         23
#define GATEWAY_STOP_CHANNEL_READ           24
#define GATEWAY_STOP_CHANNEL_WRITE          25

struct CYG_GW_INFO;
typedef struct CYG_GW_INFO cyg_gw_info, *cyg_gw_info_ptr;

//=============================================================================
// IO vector descriptors

struct CYG_GW_INFO
{
    void           *base;             // Base address
    unsigned int    size;             // Length.
    unsigned int    devid;
};

bool sis_gateway_read(void* ,int ,int );
bool sis_gateway_write(void* ,int ,int );
bool gateway_fifo_reset(int ,int );
bool gateway_dma_stop(int ,int );


static int getchannel(int devid,int RW);
#endif
