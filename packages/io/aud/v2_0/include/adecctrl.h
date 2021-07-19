///////////////////////////////////////////////////////////////////////////////////////////////
//
//	ADECCTRL.h
//
///////////////////////////////////////////////////////////////////////////////////////////////


/**
** \file
** \brief Audio decoding control structure and defines
*/

#ifndef ADECCTRL_H
#define ADECCTRL_H
#include "adec.h"
#include "UB.h"
#include "adeccmd.h"
#include "msg_def.h"

 #ifndef ERROR
#define ERROR 0x1
#endif

#ifndef OKAY
#define OKAY  0x0
#endif

//#define DECODER_THREAD_STACK_SIZE (2048/(sizeof(int)))
//#define DEF_ABV_THRESHOLD       4096
//#define DEF_DBV_THRESHOLD       10240
//#define AUD_OUTPUT_IIS

#ifdef AUD_IO_DEBUG 
#define DEBUG_FUNCTION() do { diag_printf("%s\n", __FUNCTION__); } while (0)
#define DEBUG_LINE() do { diag_printf("%d\n", __LINE__); } while (0)
#else
#define DEBUG_FUNCTION() do {} while(0)
#define DEBUG_LINE() do {} while(0)
#endif

#define DIM(xx)		((sizeof(xx))/sizeof((xx)[0]))

typedef enum {
    AUD_STATUS_ERROR           = -1,    ///< Error encountered
    AUD_STATUS_CMD_RECVD       = 0,     ///< ACK for receiving command
    AUD_STATUS_INFLUSH_DONE    = 1,     ///< Input flush is done
    AUD_STATUS_OUTFLUSH_DONE   = 2,     ///< Output flush is done
    AUD_STATUS_TIMEBASE_SYNCED = 3,     ///< Timebase sync is done
    AUD_STATUS_STOPPED         = 4,     ///< Decoding has stopped
} AUD_tStatus;

typedef enum {
    AUD_CTRL_STATE_STOP,
    AUD_CTRL_STATE_IDLE,
    AUD_CTRL_STATE_DEC,
    AUD_CTRL_STATE_ERROR,
    AUD_CTRL_STATE_SWITCH,    
    AUD_CTRL_STATE_RESTORE,   
} AUD_tCtrlState;

typedef enum {
    AUD_FLUSH_STATE_IDLE,
    AUD_FLUSH_STATE_ACTIVE,
    AUD_FLUSH_STATE_DONE,
} AUD_tFlushState;

/// Decoder task control block
typedef struct {
    int             Tid;            ///< Thread id
    void            *pStack;        ///< Stack for decoding task
    UBuff           Abv;            ///< Input buffer
    unsigned        AbvThresh;      ///< Input buffer threshold
    FILE*           AFile;	    ///< Input file
    UBuff           Dbv;            ///< Intermediate buf for decoded samples
    unsigned        DbvAllocAddr;   ///< Allocated DBV address
    void            *pPrivAttr;     ///< Decoder-specific attributes
    int             InitDone;       ///< Whether initialization is done
    AUD_tAttr       Attr;           ///< General stream attributes
    AUD_tCtrlState  CtrlState;      ///< Decoding state
    AUD_tFlushState FlushInState;   ///< Input flush state
    AUD_tFlushState FlushOutState;  ///< Output flush state
    struct AUD_tModule *pModule;    ///< Decoder module
} AUD_tCtrlBlk;

/// Fundamental elements for a decoder module
typedef struct{
    int    Type;                           ///< Audio type
    int   (*Init)(AUD_tCtrlBlk *pAcb);    ///< Initialization routine
    int    (*Decode)(AUD_tCtrlBlk *pAcb);  ///< Decode routine
    int    (*Cleanup)(AUD_tCtrlBlk *pAcb); ///< Cleanup routine
    int    (*GetAttr)(AUD_tCtrlBlk *pAcb); ///< Get stream attribute routine
}AUD_tModule;

/**
** \brief ADEC initialzation - Called during system startup, create semaphore 
**        and obtain DMA channel for ABV.
*/
void   AUD_Init(void);

/**
** \brief Creates a decoding task upon audio play command
**
** Setup decoding parameters, create decoding task and initialize
** post-processing module.
**
** \param Tid Thread id
** \param AbvStart Starting address of ABV
** \param AbvSize Size of ABV
** \param Type Audio type
**
** \note This function is called by the messaging module, when a PRES_Open()
**       command is received.
*/
void   AUD_Create(int Tid, FILE *File, int AbvSize, AUD_tType Type);

/**
** \brief Finalizes and deletes the decoding task
**
** Call the decoder's cleanup function, delete any dynamically allocated
** resources, clean up the post-processing module and finally delete the
** decoding task.
**
** \param Tid Thread ID
** \param CatId Audio category
**
** \note This function is called by the messaging module, when a PRES_Close()
**       command is received.
*/

void   AUD_Cleanup(int Tid);

int AUD_ParseCmd(AUD_tCtrlBlk *pAcb);

void tick2time(cyg_tick_count_t t);

#ifdef AUD_TIME_CONTROL
double Get_Current_Play_time(int sample_num, int sample_rate);
#endif

#endif /* ADECCTRL_H */
