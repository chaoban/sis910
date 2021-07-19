
#include "interface.h"

/*******  Photo SlideShow  *******/
//  JPEG decoder
int JPEG_Decoder(void * src, PMWIMAGEHDR pimage, PSD psd, MWBOOL fast_grayscale)
{
//    return DecodeJPEG_SW(src, pimage, psd, fast_grayscale);
    return 0;
}

/******* Audio Player *******/
//play
int MediaDecoder_Start(char* filepath)
{
    UBuff *inbuf;
    FILE *fp;
    AUD_tType type;
    unsigned int AbvStart, AbvEnd ,AbvSize;
    int size = 1024;
    
    //Read file
    fp = fopen(filepath,"r");
    if(fp ==NULL)
    {
        return ERROR;
    }
    
    //Create input buffer and copy data from file
    AbvStart = (unsigned int)malloc(size);
    AbvSize = fread(AbvStart, size, 1, fp);
    while(AbvSize < size)
    {
        free(AbvStart);
        AbvSize = 0;
        size = size*2;
        AbvStart = (unsigned int)malloc(size);
        AbvSize = fread(AbvStart, size, 1, fp);
    }
        
    //Create Audio decoder
    AUD_Create(AbvStart, AbvSize, type);
    return OKAY;
}

void MediaDecoder_SetCmd (tMediaCmdType MediaCmd)
{
    tMsg msg;
    
    if( !MsgQ_Create(AUD_PLAYER_CTRL))
    {
        ;
    }
    
    switch(MediaCmd)
    {
        case MEDIA_CMD_PLAY:
        {
            msg.MsgID = MSG_PRES_START;
            break;
        }
        case MEDIA_CMD_STOP:
        {
            msg.MsgID = MSG_PRES_STOP;
            break;
        }
        case MEDIA_CMD_PAUSE:
        {
            msg.MsgID = MSG_PRES_PAUSE;
            break;
        }
        case MEDIA_CMD_CONTINUE:
        {
            msg.MsgID = MSG_PRES_START;
            break;
        }
        default:
        {
            return ERROR;
            break;
        }
    }
  
    MsgQ_Send(AUD_PLAYER_CTRL, msg);
    //MsgQ_Rcb(AUD_PLAYER_CTRL, &msg);
   
}
void MediaDecoder_Clear ()
{
    MsgQ_Delete(AUD_PLAYER_CTRL);
    AUD_Cleanup(1);//(Tid);
}
//Volume
int Set_Volume(int value)
{
}
//status
tMediaSatusType Get_Status()
{
    tMediaSatusType status;
    
    return status;
}
char* Get_Timer(tTimeQueryType Type)
{
}

/*******  Setting  *******/
int Set_Brightness(int value)
{
}
int Get_Brightness()
{
}

int Set_Default()
{
}

int Set_Shutdown(char* Time)
{
}
char* Get_Shutdown()
{
}
