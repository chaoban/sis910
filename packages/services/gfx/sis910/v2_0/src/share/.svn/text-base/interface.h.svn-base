/*
*   Interface of AP and driver
*   AP:Photo, Audio, Setting
*/

///*******  Photo SlideShow  *******/
////  JPEG decoder
//BOOL Image_Decoder(DWORD ImgType, DWORD *InBuf, DWORD *OutBuf, int Width, int Height, int Size);
////  2D engine
//BOOL Blt(void *pDst, int xDst, int yDst, void *pSrc, int xSrc, int ySrc, int width, int height);
//BOOL Scale(void *pDst, int wDst, int hDst, void *pSrc, int wSrc, int hSrc);
//BOOL Blend(int BlendType, void *pDst, int Dfactor, void *pSrc, int Sfactor);

/******* Audio Player *******/
typedef enum{
    AUD_CMD_PLAY,
    AUD_CMD_STOP,
    AUD_CMD_PAUSE,
    AUD_CMD_CONTINUE,
}tAudCmdType;

typedef enum{
    AUD_STATUS_PLAY,
    AUD_STATUS_STOP,
    AUD_STATUS_PAUSE,
}tAudSatusType;

typedef enum{
    AUD_TIME_NOW,
    AUD_TIME_TOTAL,
}tTimeQueryType;

//play
BOOL Audio_Start(char* path);           //Return success/fail
void Audio_SetCmd (tAudCmdType AudioCmd);
BOOL Audio_Clear();                     //Return success/fail
//Volume
BOOL Set_Volume(int value);             //Return success/fail
//status
tAudSatusType Get_Status(); 
char* Get_Timer(tTimeQueryType Type);

/*******  Setting  *******/
BOOL Set_Brightness(int value);         //Return success/fail
int Get_Brightness();

BOOL Set_Default();                     //Return success/fail

BOOL Set_Shutdown(char* Time);          //Return success/fail
char* Get_Shutdown();