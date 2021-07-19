/**
 *\file handle.c
 *\brief It is the event handle module.
 *\author yifj
 *\date 2008-04-07
 *
 *$Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "display.h"
#include "engine.h"
#include "handle.h"
#include "assert.h"
#include "decoder.h"
#include "../desktop/global.h"
#ifdef __ECOS
#include <cyg/kernel/kapi.h>
extern cyg_handle_t dpfDesktopMboxHandle;
cyg_handle_t dpfAudMboxHandle;

#endif //__ECOS

static int HandChangeCur(tHandEvent Event);

/////// don't use it unless it is needed.///
/**
 *\brief get the point refering to global variable PlayInfoConfig
 *\author yifj

 *\date 2008-04-07
 *\return the point
 */
tPlayInfoConfig *HandGetInfoConfig()
{
    return EngGetInfoConfig();
}

/**
 *\brief get the point refering to global variable PlayInfoCur
 *\author yifj

 *\date 2008-04-07
 *\return the point
 */
tPlayInfoCur *HandGetInfoCur()
{
    return EngGetInfoCur();
}

/////////////  //////////////

////// message box ////////////
#ifdef __ECOS
/**
 *\brief Initial the Message Box which is used to comminucate with the desktop
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void HandIniMbox()
{
    static cyg_mbox AudMbox;
    cyg_mbox_create(&dpfAudMboxHandle, &AudMbox);
}

/**
 *\brief Send message to desktop by message box
 *\author yifj
 *\param Msg this param is used to indicate which kind of message to send. 
 *\date 2008-04-07
 *\return AUDIO_FALSE: failure, AUDIO_TRUE:success.
 */
int HandMsgSend(tHandMsg Msg)
{
    //cyg_handle_t mbox_handle;
    char MsgInfo[MAX_MSG_LEN];
    if (Msg == HAND_MSG_BG) {
        cyg_mbox_put(dpfDesktopMboxHandle, (void *) MAIL_APLAYER_BACKGROUND);
    } else if (Msg == HAND_MSG_EXIT) {
        cyg_mbox_put(dpfDesktopMboxHandle, (void *) MAIL_APLAYER_RETURN);
    } else {
        ASSERT(0);
        return AUDIO_FALSE;
    }
    return AUDIO_TRUE;
}

/**
 *\brief receive a message, and get the type of message and the param of message if get any. 
 *\author yifj
 *\param pMsg a point refering to tHandMsg type variable which is used to store the message type.
 *\param pValue a point refering to a string which is used to store the additional message parameter
 *\date 2008-04-07
 *\return AUDIO_TRUE: got a message and the pMsg and pValue are avialable, AUDIO_FALSE: didn't get any message and pMsg and pValue are inavialable.
 */
int HandMsgRecv(tHandMsg * pMsg, char *pValue)
{
    char *p = NULL;
    int i;
    char Type[MAX_MSG_LEN];
    enum MAIL_TYPE mail;

    mail = (enum MAIL_TYPE) cyg_mbox_timed_get(dpfAudMboxHandle, 3);
    if (mail == 0) {
        return AUDIO_FALSE;
    }

    printf("aplayer receive mail is %s\n", (int) mail);
    switch (mail) {
        case MAIL_SUBJECT_GREEN:
            DispSwitchSub("Green");
            break;
        case MAIL_SUBJECT_BLUE:
            DispSwitchSub("Blue");
            break;
        case MAIL_APLAYER_FOREGROUND:
            *pMsg = HAND_MSG_FG;
            break;
        default:
            assert(0);
    }
    return AUDIO_TRUE;
}
#endif  //__ECOS
///////// over ////////////////

//////////////////////////////////
////use to change volume by step. flag==1 volume up; flag==0 volume down
//////////////////////////////////

/**
 *\brief this function is used to initial the Play Mode.
 *\author yifj
 *\param PlayMode
 *\date 2008-04-07
 *\return
 */
int HandModeSet(tPlayMode PlayMode)
{
    DispModeSet(PlayMode);
    return AUDIO_TRUE;
}

/**
 *\brief Set the captions of the ID3 Labels.
 *\author yifj
 *\param Id3
 *\date 2008-04-07
 *\return
 */
int HandID3Set(tId3 Id3)
{
    DispID3Set(Id3);
    return AUDIO_FALSE;
}


/**
 *\brief change the volume
 *\author yifj
 *\param flag if == 1 volume up; if == 0 volume down
 *\date 2008-04-07
 *\return AUDIO_FALSE: inlegal flag value
 */
static int HandSound(int flag)
{
    tSoundValue OldValue;
    EngGetSound(&OldValue);
    if (flag == 0)
        EngSetSound(OldValue - STEP_VOLUME);
    else if (flag == 1)
        EngSetSound(OldValue + STEP_VOLUME);
    else {
        assert(0);
        EPRINTF(("wrong argument! \n"));
        return AUDIO_FALSE;
    }
    return AUDIO_TRUE;
}


//////////////////////////////////////////
///hand the nav event. there is a statemachine to control the state changing.
//////////////////////////////////////////

/**
 *\brief handle the nav event. there is a statemachine to control the state changing.
 *\author yifj
 *\param HandEvent it indicate the type of event which need to be handle.
 *\date 2008-04-07
 *\return AUDIO_TRUE:success  AUDIO_FALSE:failure
 */
static int HandStateMachine(tHandEvent HandEvent)
{
    tPlayerState State;
    tPlayInfoConfig *pInfoConfig = EngGetInfoConfig();
    tPlayInfoCur *pInfoCur = EngGetInfoCur();
    State = EngGetPlayState();
    FILE * fp = NULL;
    int Count = 0;

    if ((pInfoConfig->Song == 0) && (HandEvent != HAND_EVENT_STOP))
        return AUDIO_TRUE;

    if (pInfoConfig->EntryMode == FROM_FILEBROWSER){
        Count = pInfoCur->BrowserList.Length;
    }else{
        Count = pInfoCur->SongList.Length;
    }

    while (((fp = fopen(pInfoCur->CurFilePath, "rb")) == NULL) && (Count != 0)){
        DispPPBtnStop();
        HandChangeCur(HAND_EVENT_NEXT);
        Count--;
        }
    if (Count == 0){
        HandEvent = HAND_EVENT_STOP;
    }
    
    if (fp != NULL)
        fclose(fp);
        
    switch (State) {
        case PLAYER_STOP:
            switch (HandEvent) {
                case HAND_EVENT_PLAY:
                    if (strlen(pInfoCur->CurFilePath) == 0) {
                        EngSetPlayState(PLAYER_STOP);
                        break;
                    }
#ifdef DEC_OK
                    Audio_Start(pInfoCur->CurFilePath);
                    Audio_SetCmd(AUD_CMD_PLAY);
                    while (Get_Status() != AUD_STATUS_PLAY)
                        ApRelinquish();
#endif // DEC_OK
                    EngSetId3(pInfoCur->CurFilePath);
                    HandID3Set(pInfoCur->Id3);
                    EngSetPlayState(PLAYER_PLAY);
#ifdef DEC_OK
                    pInfoCur->CurTime = Get_Timer(AUD_TIME_NOW);
                    pInfoCur->SumTime = Get_Timer(AUD_TIME_TOTAL);
#endif // DEC_OK
                    DispBtnPPSet(HAND_EVENT_PAUSE);
                    break;
                case HAND_EVENT_PAUSE:
                    assert(0);  // when it is stop state, there is not pause btn but play btn;
                    break;
                case HAND_EVENT_STOP:
                    break;
                case HAND_EVENT_PRE:
                    HandChangeCur(HAND_EVENT_PRE);
                    break;
                case HAND_EVENT_NEXT:
                    HandChangeCur(HAND_EVENT_NEXT);
                    break;
            }
            break;
        case PLAYER_PLAY:
            switch (HandEvent) {
                case HAND_EVENT_PLAY:
                    assert(0);  // when it is play state, there is not play btn but pause btn;
                    break;
                case HAND_EVENT_PAUSE:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_PAUSE);
                    while (Get_Status() != AUD_STATUS_PAUSE)
                        ApRelinquish();
#endif // DEC_OK
                    EngSetPlayState(PLAYER_PAUSE);
                    DispBtnPPSet(HAND_EVENT_PLAY);
                    break;
                case HAND_EVENT_STOP:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_STOP);
                    while (Get_Status() != AUD_STATUS_STOP)
                        ApRelinquish();
                    Audio_Clear();
#endif // DEC_OK
                    EngSetPlayState(PLAYER_STOP);
                    pInfoCur->CurTime = CUR_TIME_INIT;
                    DispBtnPPSet(HAND_EVENT_PLAY);
                    break;
                case HAND_EVENT_PRE:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_STOP);
                    while (Get_Status() != AUD_STATUS_STOP)
                        ApRelinquish();
                    Audio_Clear();
#endif // DEC_OK
                    HandChangeCur(HAND_EVENT_PRE);
                    if (strlen(pInfoCur->CurFilePath) == 0) {
                        EngSetPlayState(PLAYER_STOP);
                        break;
                    }
#ifdef DEC_OK
                    Audio_Start(pInfoCur->CurFilePath);
                    Audio_SetCmd(AUD_CMD_PLAY);
                    while (Get_Status() != AUD_STATUS_PLAY)
                        ApRelinquish();
                    EngSetId3(pInfoCur->CurFilePath);
                    HandID3Set(pInfoCur->Id3);
                    pInfoCur->CurTime = Get_Timer(AUD_TIME_NOW);
                    pInfoCur->SumTime = Get_Timer(AUD_TIME_TOTAL);
#endif // DEC_OK
                    break;
                case HAND_EVENT_NEXT:
                    //ADecStop();
                    //change global variable
                    //ADecPlay();
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_STOP);
                    while (Get_Status() != AUD_STATUS_STOP)
                        ApRelinquish();
                    Audio_Clear();
#endif // DEC_OK
                    HandChangeCur(HAND_EVENT_NEXT);
                    if (strlen(pInfoCur->CurFilePath) == 0) {
                        EngSetPlayState(PLAYER_STOP);
                        break;
                    }
#ifdef DEC_OK
                    Audio_Start(pInfoCur->CurFilePath);
                    Audio_SetCmd(AUD_CMD_PLAY);
                    while (Get_Status() != AUD_STATUS_PLAY)
                        ApRelinquish();
                    EngSetId3(pInfoCur->CurFilePath);
                    HandID3Set(pInfoCur->Id3);
                    pInfoCur->CurTime = Get_Timer(AUD_TIME_NOW);
                    pInfoCur->SumTime = Get_Timer(AUD_TIME_TOTAL);
#endif // DEC_OK
                    break;
            }
            break;
        case PLAYER_PAUSE:
            switch (HandEvent) {
                case HAND_EVENT_PLAY:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_CONTINUE);
                    while (Get_Status() != AUD_STATUS_PLAY)
                        ApRelinquish();
#endif // DEC_OK
                    EngSetPlayState(PLAYER_PLAY);
                    DispBtnPPSet(HAND_EVENT_PAUSE);
                    break;
                case HAND_EVENT_PAUSE:
                    assert(0);  // when it is pause state, there is not pause btn but play btn;
                    break;
                case HAND_EVENT_STOP:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_STOP);
                    while (Get_Status() != AUD_STATUS_STOP)
                        ApRelinquish();
                    Audio_Clear();
#endif // DEC_OK

                    EngSetPlayState(PLAYER_STOP);
                    pInfoCur->CurTime = CUR_TIME_INIT;
                    DispBtnPPSet(HAND_EVENT_PLAY);
                    break;
                case HAND_EVENT_PRE:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_STOP);
                    while (Get_Status() != AUD_STATUS_STOP)
                        ApRelinquish();
                    Audio_Clear();
#endif // DEC_OK
                    EngSetPlayState(PLAYER_STOP);
                    pInfoCur->CurTime = CUR_TIME_INIT;
                    HandChangeCur(HAND_EVENT_PRE);
                    break;
                case HAND_EVENT_NEXT:
#ifdef DEC_OK
                    Audio_SetCmd(AUD_CMD_STOP);
                    while (Get_Status() != AUD_STATUS_STOP)
                        ApRelinquish();
                    Audio_Clear();
#endif // DEC_OK
                    EngSetPlayState(PLAYER_STOP);
                    pInfoCur->CurTime = CUR_TIME_INIT;
                    HandChangeCur(HAND_EVENT_NEXT);

                    break;
            }
            break;
    }
    return AUDIO_TRUE;
}


/**
 *\brief this function initial the playlist(favorites) listbox. It adds list box items to list box according to the special list link.
 *\author yifj
 *\param pPLink point to a link list struct which stores the items need to display in the list box
 *\date 2008-04-07
 *\return
 */
int HandInitPLI(tpLink pPLink)
{
    int i = 0;
    char FilePath[MAX_FILE_PATH], FileName[MAX_FILENAME_LEN];

    while (i < pPLink->Length) {
        EngLinkRead(pPLink, i + 1, FilePath);
        EngPathToName(FilePath, FileName);
        DispPLIAdd(FileName);
        i++;
    }
    DispPLISetSel(0);
    return AUDIO_TRUE;
}

/**
 *\brief this function update the FileBrowser list box items according to the specific path which is express by pInfoCur->FileBrower
 only the file which have the specific extension like "MP3" or "mp3" will be pick out to display on list box.
 *\author yifj

 *\date 2008-04-07
 *\return AUDIO_FALSE:failure, AUDIO_TRUE: success
 */
static int HandUpdateFB()
{
#ifndef LONG_FILENAME
    struct dirent **NameList = NULL;
#else
    char **NameList = NULL;
#endif //LONG_FILENAME

    int AllNum = 0;
    int i, result = AUDIO_TRUE;
    char TempPath[MAX_DIR_LEN];
    DIR *fdir = NULL;

    tPlayInfoConfig *pInfoConfig = NULL;
    pInfoConfig = (tPlayInfoConfig *) EngGetInfoConfig();
    tPlayInfoCur *pInfoCur = NULL;
    pInfoCur = (tPlayInfoCur *) EngGetInfoCur();
    result = EngGetDir(pInfoCur->FileBrower, &NameList, &AllNum);
    if (result == AUDIO_FALSE) {
        EngFreeDir(NameList, AllNum);
        assert(0);
        return AUDIO_FALSE;
    }

    strcpy(TempPath, pInfoCur->FileBrower);
    DispFBDelAll();
    pInfoCur->FBFolderNum = 0;
    pInfoCur->FBFileNum = 0;
    for (i = 0; i < AllNum; i++) {
#ifndef LONG_FILENAME
        strcat(TempPath, NameList[i]->d_name);
#else
        strcat(TempPath, NameList[i]);
#endif //LONG_FILENAME
        if ((fdir = opendir(TempPath)) != NULL) {
            closedir(fdir);
#ifndef LONG_FILENAME
            DispAddFBItemAt(NameList[i]->d_name, pInfoCur->FBFolderNum, 0);
#else
            DispAddFBItemAt(NameList[i], pInfoCur->FBFolderNum, 0);
#endif //LONG_FILENAME
            pInfoCur->FBFolderNum++;
#ifndef LONG_FILENAME
        } else if (strstr(NameList[i]->d_name, "mp3") || strstr(NameList[i]->d_name, "MP3")) {
            DispAddFBItemAt(NameList[i]->d_name, pInfoCur->FBFolderNum + pInfoCur->FBFileNum, 1);
#else
        } else if (strstr(NameList[i], "mp3") || strstr(NameList[i], "MP3")) {
            DispAddFBItemAt(NameList[i], pInfoCur->FBFolderNum + pInfoCur->FBFileNum, 1);
#endif //LONG_FILENAME
            pInfoCur->FBFileNum++;
        }
        strcpy(TempPath, pInfoCur->FileBrower);
    }
    EngFreeDir(NameList, AllNum);
    return AUDIO_TRUE;
}

/**
 *\brief this function handles all kinds of events dispatched by display module
 *\author yifj
 *\param HandEvent the event need to be deal with
 *\date 2008-04-07
 *\return AUDIO_FALSE:failure, AUDIO_TRUE: success
 */
int HandEventProc(tHandEvent HandEvent)
{
    int result, i;
    tPlayerState State;
    State = EngGetPlayState();
    tPlayInfoConfig *pInfoConfig = NULL;
    pInfoConfig = (tPlayInfoConfig *) EngGetInfoConfig();
    tPlayInfoCur *pInfoCur = NULL;
    pInfoCur = (tPlayInfoCur *) EngGetInfoCur();
    result = AUDIO_TRUE;
    tPlayMode PlayMode;
    int DelNum, SelNum;
    char SelFilePath[MAX_FILE_PATH];
    char SelFileName[MAX_FILENAME_LEN];

#ifdef DEC_OK
    tAudSatusType Status;
#endif // DEC_OK

    switch (HandEvent) {
        case HAND_EVENT_PLAY:
        case HAND_EVENT_PAUSE:
        case HAND_EVENT_PRE:
        case HAND_EVENT_NEXT:
        case HAND_EVENT_STOP:
            DPRINTF(("got the event%d ! \n", HandEvent));
            result = HandStateMachine(HandEvent);
            break;
        case HAND_EVENT_FB:
            EngReadBrowserList();
            if (pInfoCur->BrowserList.Length == 0)
                pInfoConfig->Song = 0;
            else if (pInfoConfig->Song > pInfoCur->BrowserList.Length){
                pInfoConfig->Song = 1;
            }
            result = DispCrtFB();
            result = HandUpdateFB();
            DispFBSetSel(0);
            break;
        case HAND_EVENT_MODE:
            result = EngChMode(&PlayMode);
            result = DispModeSet(PlayMode);
            break;
        case HAND_EVENT_PL:
            result = DispCrtPlayList();
            if (result == AUDIO_FALSE)
                break;
            result = HandInitPLI(&(pInfoCur->SongList));
            if (result == AUDIO_FALSE)
                break;
            //DispNeedFix(); /// need fixed, for the additional item
            //result = EngRelPL(PlayListItems);
            if (result == AUDIO_FALSE)
                break;
            break;
        case HAND_EVENT_SOUND:
            //result = DispSound(***);
            break;
        case HAND_EVENT_SOUND_UP:
            result = HandSound(1);      // change disp
            break;
        case HAND_EVENT_SOUND_DOWN:
            result = HandSound(0);
            break;

        case HABD_EVENT_FB_ENTER:
            EngReadBrowserList();
            HandUpdateFB();
            DispFBSetSel(0);
            break;
        case HAND_EVENT_FB_PLAY:
            HandStateMachine(HAND_EVENT_STOP);
            result = DispFBSelNum(&SelNum);     //DelNum range 0,1.....
            if (result == AUDIO_FALSE)
                break;
            pInfoConfig->EntryMode = FROM_FILEBROWSER;
            strcpy(pInfoConfig->DirPath, pInfoCur->FileBrower);
            pInfoConfig->Song = SelNum + 1 - pInfoCur->FBFolderNum;
            result = EngLinkRead(&(pInfoCur->BrowserList), pInfoConfig->Song, pInfoCur->CurFilePath);
            if (result == AUDIO_FALSE) {
                assert(0);
                break;
            }
            HandStateMachine(HAND_EVENT_PLAY);
            // call the brower api made by tsing.store info (is it the duty of disp?)
            break;
        case HAND_EVENT_FB_ADD:
            result = DispFBSelInfo(SelFileName);
            if (result == AUDIO_FALSE)
                break;
            if (strcmp(SelFileName, ".") == 0) {
                int i;
                char FilePath[MAX_FILE_PATH];
                for (i = 0; i < pInfoCur->FBFileNum; i++) {
                    EngLinkRead(&(pInfoCur->BrowserList), i + 1, FilePath);
                    EngAddPlayitem(FilePath);
                }
                EngLinkSave(&(pInfoCur->SongList), pInfoConfig->PlayList);
                break;
            } else {
                strcpy(SelFilePath, pInfoCur->FileBrower);
                strcat(SelFilePath, SelFileName);
                result = EngAddPlayitem(SelFilePath);
                result = EngLinkSave(&(pInfoCur->SongList), pInfoConfig->PlayList);
                break;
            }
        case HAND_EVENT_PITEM_PLAY:
            HandStateMachine(HAND_EVENT_STOP);
            result = DispGetSelPlayitem(&SelNum);       //DelNum range 0,1.....
            if (result == AUDIO_FALSE)
                break;
            pInfoConfig->EntryMode = FROM_PLAYLIST;
            pInfoConfig->Song = SelNum + 1;
            result = EngLinkRead(&(pInfoCur->SongList), pInfoConfig->Song, pInfoCur->CurFilePath);
            if (result == AUDIO_FALSE) {
                assert(0);
                break;
            }
            HandStateMachine(HAND_EVENT_PLAY);
            // call DispPlOn()
            //(is it the duty of disp?)
            break;
        case HAND_EVENT_PITEM_RM:
            result = DispDelPlayitem(&DelNum);  //DelNum range 0,1.....
            if (result == AUDIO_FALSE)
                break;
            result = EngDelPlayitem(DelNum + 1);
            if (pInfoConfig->EntryMode == FROM_PLAYLIST){
                if ((DelNum+1) <= pInfoConfig->Song)
                    pInfoConfig->Song--;
            }
            result = EngLinkSave(&(pInfoCur->SongList), pInfoConfig->PlayList);
            if (result == AUDIO_FALSE)
                break;
            break;
        case HAND_EVENT_PITEM_RM_ALL:
            for (i = 0; i < pInfoCur->SongList.Length; i++) {
                //DispPLISetSel(i);
                DispDelPlayitem(&DelNum);
            }
            EngLinkDestroy(&(pInfoCur->SongList));
            if (pInfoConfig->EntryMode == FROM_PLAYLIST){
                pInfoConfig->Song = 0;
            }
            result = EngLinkSave(&(pInfoCur->SongList), pInfoConfig->PlayList);
            if (result == AUDIO_FALSE) {
                EPRINTF(("error happend when save the platlist ! \n"));
                break;
            }
            break;
        case HAND_EVENT_PITEM_UP:
            result = DispUpPlayitem(&SelNum);
            if (result == AUDIO_FALSE)
                break;
            DispPLISetSel(SelNum - 1);
            result = EngUpPlayitem(&(pInfoCur->SongList), SelNum);
            result = EngLinkSave(&(pInfoCur->SongList), pInfoConfig->PlayList);
            break;
        case HAND_EVENT_PITEM_DOWN:
            result = DispDownPlayitem(&SelNum);
            if (result == AUDIO_FALSE)
                break;
            DispPLISetSel(SelNum + 1);
            result = EngDownPlayitem(&(pInfoCur->SongList), SelNum);
            result = EngLinkSave(&(pInfoCur->SongList), pInfoConfig->PlayList);
            break;
        case HAND_EVENT_TIMER:
#ifdef DEC_OK
            Status = Get_Status();
            switch (State) {
                case PLAYER_PLAY:
                    switch (Status) {
                        case AUD_STATUS_PLAY:
                            pInfoCur->CurTime = Get_Timer(AUD_TIME_NOW);
                            pInfoCur->SumTime = Get_Timer(AUD_TIME_TOTAL);
                            result = AUDIO_TRUE;
                            break;
                        case AUD_STATUS_PAUSE:
                            assert(0);
                            result = AUDIO_FALSE;
                            break;
                        case AUD_STATUS_STOP:
                            if ((pInfoConfig->EntryMode == FROM_FILEBROWSER) && (pInfoCur->BrowserList.Length == 0)){
                                DispPPBtnStop();
                                HandEventProc(HAND_EVENT_STOP);
                                break;
                            }
                            if ((pInfoConfig->EntryMode == FROM_PLAYLIST) && (pInfoCur->SongList.Length == 0)){
                                DispPPBtnStop();
                                HandEventProc(HAND_EVENT_STOP);
                                break;
                            }
                            if (pInfoConfig->PlayMode == NO_CIRCUS) {
                                HandEventProc(HAND_EVENT_STOP);
                            } else if (pInfoConfig->PlayMode == REPEAT_CURRENT) {
                                HandEventProc(HAND_EVENT_STOP);
                                HandEventProc(HAND_EVENT_PLAY);
                            } else if (pInfoConfig->PlayMode == CIRCUS_AROUND) {
                                HandEventProc(HAND_EVENT_STOP);
                                HandEventProc(HAND_EVENT_NEXT);
                                HandEventProc(HAND_EVENT_PLAY);
                            } else {
                                assert(0);
                                result = AUDIO_FALSE;
                            }
                            break;
                    }
                    break;
                case PLAYER_PAUSE:
                    switch (Status) {
                        case AUD_STATUS_PLAY:
                            assert(0);
                            result = AUDIO_FALSE;
                            break;
                        case AUD_STATUS_PAUSE:
                            break;
                        case AUD_STATUS_STOP:
                            HandEventProc(HAND_EVENT_STOP);
                            break;
                    }
                    break;
                case PLAYER_STOP:
                    switch (Status) {
                        case AUD_STATUS_PLAY:
                            assert(0);
                            result = AUDIO_FALSE;
                            break;
                        case AUD_STATUS_PAUSE:
                            assert(0);
                            result = AUDIO_FALSE;
                            break;
                        case AUD_STATUS_STOP:
                            break;
                    }
                    break;
                default:
                    assert(0);
                    result = AUDIO_FALSE;
            }
#endif // DEC_OK
            break;

        default:
            assert(0);
            result = AUDIO_FALSE;
            break;
    }
    return result;
}

/**
 *\brief this function is called when file browser change current directory. It save the correct path in PlayInfoCur.FileBrower
 *\author yifj
 *\param FolderName
 *\date 2008-04-07
 *\return
 */
int HandEnterDir(char *FolderName)
{
    int i;
    tPlayInfoCur *pPlayInfoCur = EngGetInfoCur();
    if (FolderName == NULL)
        return AUDIO_FALSE;
    if (strcmp(FolderName, ".") == 0) {
        return AUDIO_TRUE;
    }
    if (strcmp(FolderName, "..") == 0) {
        if(strcmp(pPlayInfoCur->FileBrower, "/")==0 || strcmp(pPlayInfoCur->FileBrower, "./")==0)
            return AUDIO_TRUE;
        i = strlen(pPlayInfoCur->FileBrower) - 2;
        while (i && pPlayInfoCur->FileBrower[i] != '/')
            i--;
        pPlayInfoCur->FileBrower[i + 1] = '\0';
    } else {
        strcat(pPlayInfoCur->FileBrower, FolderName);
        strcat(pPlayInfoCur->FileBrower, "/");
    }
    return AUDIO_TRUE;
}

/**
 *\brief change the playing song. It happend if the operation next_song or previous_song need to be executed.
 There are two entry modes:
 1. FROM_FILEBROWSER the player is in this entry mode if user enters the player by active a song in the file browser.
 Then the next or previous relate to songs order in filebrowser.
 2. FROM_PLAYLIST the player is in this entry mode if user enters the player by active a song in favorate songs list.
 Then the next or previous relate to songs order in favorate list.
 *\author yifj
 *\param event HAND_EVENT_PRE or HAND_EVENT_NEXT,
 *\date 2008-04-07
 *\return
 */
static int HandChangeCur(tHandEvent Event)
{
    tpLink pLink = NULL;
    int result, NewNum = 0;
    char Content[MAX_FILE_PATH];
    tPlayInfoCur *pPlayInfoCur = EngGetInfoCur();
    tPlayInfoConfig *pPlayInfoConfig = EngGetInfoConfig();
    if (Event == HAND_EVENT_PRE) {
        if (pPlayInfoConfig->EntryMode == FROM_FILEBROWSER)
            pLink = &(pPlayInfoCur->BrowserList);
        else
            pLink = &(pPlayInfoCur->SongList);

        if (pLink->Length == 0) {
            pPlayInfoCur->CurFilePath[0] = 0;
            pPlayInfoConfig->Song = 0;
            return AUDIO_FALSE;
        }
        if (pPlayInfoConfig->Song > pLink->Length){
            NewNum = 1;
        }else{
            NewNum = (pPlayInfoConfig->Song - 1) % pPlayInfoCur->SongList.Length;
            NewNum ? NewNum : (NewNum = pPlayInfoCur->SongList.Length);
        }
        result = EngLinkRead(pLink, NewNum, Content);
        if (result == AUDIO_TRUE) {
            pPlayInfoConfig->Song = NewNum;
            strcpy(pPlayInfoCur->CurFilePath, Content);
        }
    }

    else if (Event == HAND_EVENT_NEXT) {
        if (pPlayInfoConfig->EntryMode == FROM_FILEBROWSER)
            pLink = &(pPlayInfoCur->BrowserList);
        else
            pLink = &(pPlayInfoCur->SongList);
        if (pLink->Length == 0) {
            pPlayInfoCur->CurFilePath[0] = 0;
            pPlayInfoConfig->Song = 0;
            return AUDIO_FALSE;
        }
        NewNum = pPlayInfoConfig->Song % pLink->Length + 1;
        result = EngLinkRead(pLink, NewNum, Content);
        if (result == AUDIO_TRUE) {
            pPlayInfoConfig->Song = NewNum;
            strcpy(pPlayInfoCur->CurFilePath, Content);
        }
    } else
        assert(0);
    return result;
}

/**
 *\brief it gives the total number of folders in current path
 *\author yifj

 *\date 2008-04-07
 *\return folder number
 */
int HandGetFolderNum()
{
    tPlayInfoCur *pInfoCur = NULL;
    pInfoCur = (tPlayInfoCur *) EngGetInfoCur();
    return pInfoCur->FBFolderNum;
}

/**
 *\brief initial the Handle module at the beginning of audioplayer creating.
 *\author yifj

 *\date 2008-04-07
 *\return
 */
void HandInit()
{
#ifdef __ECOS
    HandIniMbox();
#endif // __ECOS
    DPRINTF(("HandInit over! \n"));
}

/**
 *\brief fullfill the duty of handle module when the audio player exit. 
 *\author yifj

 *\date 2008-04-07
 *\return
 */
void HandEnd()
{
    EngEnd();
}
