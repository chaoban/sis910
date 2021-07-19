#ifndef __HANDLE_H__
#define __HANDLE_H__
#include "common.h"
#include "engine.h"

typedef enum {
    HAND_EVENT_PLAY,
    HAND_EVENT_PAUSE,
    HAND_EVENT_STOP,
    HAND_EVENT_PRE,
    HAND_EVENT_NEXT,
    HAND_EVENT_FB,
    HAND_EVENT_MODE,
    HAND_EVENT_PL,
    HAND_EVENT_SOUND,
    HAND_EVENT_SOUND_UP,
    HAND_EVENT_SOUND_DOWN,
    HABD_EVENT_FB_ENTER,
    HAND_EVENT_FB_PLAY,
    HAND_EVENT_FB_ADD,
    HAND_EVENT_FB_ADD_ALL,
    HAND_EVENT_PITEM_PLAY,
    HAND_EVENT_PITEM_RM,
    HAND_EVENT_PITEM_RM_ALL,
    HAND_EVENT_PITEM_UP,
    HAND_EVENT_PITEM_DOWN,
    HAND_EVENT_TIMER
} tHandEvent;

//////message /////
typedef enum {
    HAND_MSG_SUB,               // recieve 
    HAND_MSG_FG,                // recieve 
    HAND_MSG_BG,                // send 
    HAND_MSG_EXIT               // send 
} tHandMsg;
#define MAX_MSG_LEN 50
/////over /////////

tPlayInfoCur *HandGetInfoCur();
tPlayInfoConfig *HandGetInfoConfig();
int HandMsgSend(tHandMsg Msg);
int HandMsgRecv(tHandMsg * pMsg, char *pValue);
int HandModeSet(tPlayMode PlayMode);
int HandID3Set(tId3 Id3);
int HandInitPLI(tpLink pPLink);
int HandEventProc(tHandEvent HandEvent);
int HandEnterDir(char *FolderName);
int HandGetFolderNum();
void HandInit();
void HandEnd();

#endif // __HANDLE_H__
