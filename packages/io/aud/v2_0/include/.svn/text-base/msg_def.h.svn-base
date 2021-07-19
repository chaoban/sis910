///////////////////////////////////////////////////////////////////////////////////////////////
//
//	MSG_DEF.H
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef MSG_DEF_H
#define MSG_DEF_H

#include <pkgconf/io_aud.h>
#include <cyg/kernel/kapi.h>

typedef enum {
    MSG_ABV_UPDATE = 0,
    MSG_ABV_DRAIN,
    MSG_ABV_FILL,
    MSG_PRES_START,
    MSG_PRES_STOP,
    MSG_PRES_PAUSE,
    MSG_PRES_OPEN,
    MSG_PRES_CLOSE,
    MSG_PRES_INFLUSH,
    MSG_PRES_OUTFLUSH,
    MSG_PRES_SET_TIMEBASE,
    MSG_PRES_SET_SYS_PARAM,
    MSG_READY,
    MSG_LAST
} MSG_tType;

/// Data structure for message paramters
typedef union {
    void          *Ptr;
    unsigned int   Dword;
    unsigned short Word[2];
    unsigned char  Byte[4];
} tMsgParam;

/// Data structure for each message (16 bytes/msg)
typedef struct {
    unsigned  MsgID;    ///< Message ID (defined in msg_type.h)
    tMsgParam Para1;
    tMsgParam Para2;
    tMsgParam Para3;
} tMsg;

#ifdef ECOS_MSG_API
typedef struct {
	cyg_handle_t	handle;
	cyg_mbox	mbox;
} tMsg_Queue;
#else
typedef struct {
	tMsg *msg;
	int fullness;
	int empty;
	int sem;
} tMsg_Queue;
#endif

// Message queue ID
typedef enum {
	AUD_PLAYER_CTRL = 0,
	AUD_PLAYER_ATTR,
	ERROR_MSG_Q = MAX_QUEUE_NUM,
} Msg_Queue_t;


#endif /* MSG_DEF_H */
