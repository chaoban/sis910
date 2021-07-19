#ifndef __DECODER_H__
#define __DECODER_H__

#include "stdio.h"
#include "stdlib.h"

typedef enum {
    AUD_CMD_PLAY,
    AUD_CMD_STOP,
    AUD_CMD_PAUSE,
    AUD_CMD_CONTINUE,
} tAudCmdType;

typedef enum {
    AUD_STATUS_PLAY,
    AUD_STATUS_STOP,
    AUD_STATUS_PAUSE,
} tAudSatusType;

typedef enum {
    AUD_TIME_NOW,
    AUD_TIME_TOTAL,
} tTimeQueryType;

#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
struct nx_thread {
    char *name;
    CYG_ADDRWORD *entry;
    int prio;
    cyg_handle_t t;
    cyg_thread t_obj;
    char stack[STACKSIZE];
};
#endif //__ECOS

//play
int Audio_Start(char *path);    //Return success/fail
void Audio_SetCmd(tAudCmdType AudioCmd);
//Volume
int Set_Volume(int value);      //Return success/fail
//status
tAudSatusType Get_Status();
int Get_Timer(tTimeQueryType Type);
int Audio_Clear();

#endif // __DECODER_H__

