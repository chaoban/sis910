#ifndef INTERFACE_H
#define INTERFACE_H
/*
*   Interface of AP and driver
*   AP:Photo, Audio, Setting
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cyg/io/aud/msg_def.h>
#include <cyg/io/aud/adeccmd.h>
#include <cyg/io/aud/adecctrl.h>

#include "device.h"

#ifndef ERROR
#define ERROR 0x1
#endif

#ifndef OKAY
#define OKAY  0x0
#endif

///*******  Photo SlideShow  *******/
////  JPEG decoder
int JPEG_Decoder(void * src, PMWIMAGEHDR pimage, PSD psd, MWBOOL fast_grayscale);
////  2D engine
//BOOL Blt(void *pDst, int xDst, int yDst, void *pSrc, int xSrc, int ySrc, int width, int height);
//BOOL Flip();
//initial multi-fram buffer point
int Scale(void *pDst, int wDst, int hDst, void *pSrc, int wSrc, int hSrc);
int Blend(int BlendType, void *pDst, int Dfactor, void *pSrc, int Sfactor);

/******* Audio and Vedio Player *******/
//typedef enum {
//    AUDTYPE_INVALID = 0,
//    AUDTYPE_AC3,
//    AUDTYPE_EAC3,
//    AUDTYPE_MLP,
//    AUDTYPE_THD,
//    AUDTYPE_DTS,
//    AUDTYPE_LPCM_BD,
//    AUDTYPE_DTS_LBR,
//    AUDTYPE_MPG2,
//    AUDTYPE_MP3,
//    AUDTYPE_AAC,
//    AUDTYPE_WMA,
//    AUDTYPE_OGG,
//    NUM_AUDTYPE,
//} tAudType;

typedef enum{
    MEDIA_CMD_PLAY,
    MEDIA_CMD_STOP,
    MEDIA_CMD_PAUSE,
    MEDIA_CMD_CONTINUE,
}tMediaCmdType;

typedef enum{
    MEDIA_STATUS_PLAY,
    MEDIA_STATUS_STOP,
    MEDIA_STATUS_PAUSE,
}tMediaSatusType;

typedef enum{
    MEDIA_TIME_NOW,
    MEDIA_TIME_TOTAL,
}tTimeQueryType;

//play
int MediaDecoder_Start(char* filepath);           //Return success/fail
void MediaDecoder_SetCmd (tMediaCmdType MediaCmd);
void MediaDecoder_Clear ();
//status
tMediaSatusType Get_Status(); 
char* Get_Timer(tTimeQueryType Type);
//Volume
int Set_Volume(int value);             //Return success/fail

/*******  Setting  *******/
int Set_Brightness(int value);         //Return success/fail
int Get_Brightness();

int Set_Default();                     //Return success/fail

int Set_Shutdown(char* Time);          //Return success/fail
char* Get_Shutdown();

#endif
