#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include "engine.h"
#include "handle.h"

//// for short of function create from RC ///
#define MODE_PIC_X 570
#define MODE_PIC_Y 395
#define MODE_PIC_W 75
#define MODE_PIC_H 46
//////////////// over///////

////////////////////////////
#define DISP_NAV_BTN_NUM 4
typedef enum {
    NAV_BTN_PLAY_PAUSE,         // it is also NAV_BTN_PAUSE
    NAV_BTN_STOP,
    NAV_BTN_PRE,
    NAV_BTN_NEXT,
} tNavBtnType;
////////////////////////////

//////////////////////////////
#define DISP_EXT_BTN_NUM 4
typedef enum {
    EXT_BTN_FB,
    EXT_BTN_MODE,
    EXT_BTN_PL,
    EXT_BTN_SOUND
} tExtBtnType;
//////////////////////////////

int DispModeSet(tPlayMode PlayMode);
int DispID3Set(tId3 Id3);
int DispBtnPPSet(tHandEvent Event);
int DispDelPlayitem(int *pDelNum);
int DispPLIAdd(char FileName[]);
int DispPLISetSel(int Num);
int DispUpPlayitem(int *pSelNum);
int DispDownPlayitem(int *pSelNum);
int DispGetSelPlayitem(int *pSelNum);
int DispCrtPlayList();
int DispAddFBItemAt(char *Name, int Pos, int flag);
int DispFBSetSel(int Num);
int DispFBSelNum(int *pSelNum);
int DispFBDelAll();
int DispFBSelInfo(char *FileName);
int DispCrtFB();
int DispInit();
int DispEnd();
int DispSwitchSub(char *Subject);
void DispPPBtnStop();

#endif // __DISPLAY_H__
