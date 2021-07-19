/**
 *\file audioplayer.c
 *\brief audio player entry
 *\author yifj
 *\date 2008-04-07
 *
 *$Id$
 */
#include<stdio.h>
#include <pthread.h>
#include "assert.h"
#include "string.h"
#include "tinywidget/tnWidgets.h"
#include "../share/ecos_config.h"
#include "../share/global.h"
#include "../share/browser.h"
// include the header file produced by designer tool.
#include "audio.h"
#include "../config/config.h"
#define MWINCLUDECOLORS
#ifndef __ECOS
char dpfFbOpenedFileName[512] = {0};
#endif // __ECOS
#include "engine.h"
#include "common.h"

//#define audio_test 1
#ifdef audio_test
#include <microwin/audio_io.h>
//#include <cyg/io/iis/io_iis.h>
#endif

#define AP_NAME       "APlayer"
#define __DEBUG
#ifdef __DEBUG
#define EPRINTF(a) printf a    
#else
#define EPRINTF(a)
#endif

#define PLI_MAX_NUM 300

extern cyg_handle_t dpfDesktopMboxHandle;
cyg_handle_t dpfAudMboxHandle;
DPF_EVENT gAudioEvent;
TN_WIDGET *gMinimizeFocus;

RCINFO	gAudioRcinfo;
char grcfileDir[128];
static struct configFile *gpcfgFile=NULL;
char grcfileDir[128];
static char *gSubject=NULL;
static char *gLang=NULL;
TN_WIDGET  *gpwndAudio, *gplblCurTime;
TN_WIDGET *gplblCurFile, *gplblSong, *gplblAlbum, *gplblSinger, *gplblPlayTime, *gplblAllTime , *gpprgbPlay;
TN_WIDGET *gpckbPlay, *gpbtnStop, *gpbtnPrev, *gpbtnNext, *gprdbPlayMode, *gpbtnBrowse, *gpbtnPlayList, *gpbtnVolume, *gpprgbVolume;
TN_WIDGET *gpbtnMinimize;
tFileBrowser *gptAudioFileBrowser = NULL;

#ifdef CYGPKG_HAL_R16
static char gCurrentDir[128]="/sd";
//static char gCurrentDir[128]="/romdisk/resource";
#else
static char gCurrentDir[128]="/romdisk/resource";
#endif

typedef struct
{
    char filename[MAX_FILE_PATH];
    int  total_min;
    int  total_sec;
    int  cur_min;
    int  cur_sec;
    float offsetrun;
    float currun;
    GR_BOOL bplay;
    
}SONG_INFO;

tLink gSongList;
int gnCurSongIndex;
char gCurSong[MAX_FILE_PATH];
SONG_INFO gCurSongInfo;

GR_BOOL gboolStart = GR_FALSE;
char **gSongFiles;

extern GR_IMAGE_ID selectImage(char *str);
extern GR_IMAGE_ID selectAudio(char *str);

static int LinkAdd(tpLink pLink, char *Content)
{
    tpLinkNode p = NULL, pbefore = NULL, puse = NULL;
    char *TempContent = NULL;
    int len;

    if ((pLink == NULL) || (Content == NULL)) {
        EPRINTF(("error: input of ComLinkAdd ! \n"));
        return GR_FALSE;
    }
    if ((pLink->Length) >= PLI_MAX_NUM) {
        DPRINTF(("You add too much songs into the favorite library! \n"));
        return GR_FALSE;
    }

    pbefore = p = pLink->pHeader;
    while (p) {
#ifdef __ECOS
        char temp1[MAX_FILE_PATH], temp2[MAX_FILE_PATH];
        strcpy(temp1, Content);
        strcpy(temp2, p->content);
        StrUpr(temp1);
        StrUpr(temp2);
        if (strcmp(temp1, temp2) == 0)
            return GR_TRUE;
#else
        if (strcmp(Content, p->content) == 0)
            return GR_TRUE;
#endif //__ECOS
        pbefore = p;
        p = p->next;
    }

    puse = (tLinkNode *) malloc(sizeof(tLinkNode));
    if (puse == NULL) {
        EPRINTF(("malloc error happened !\n"));
        ASSERT(0);
        return GR_FALSE;
    }
    puse->next = NULL;
    len = strlen(Content);
    if (len >= MAX_FILE_PATH) {
        DPRINTF(("the file path is too long! \n"));
        return GR_FALSE;
    }
    TempContent = (char *) malloc(len + 1);
    if (TempContent == NULL) {
        EPRINTF(("malloc error happened! \n"));
        ASSERT(0);
        return GR_FALSE;
    }
    strcpy(TempContent, Content);
    puse->content = TempContent;
    if (pbefore == NULL)
        pLink->pHeader = puse;
    else
        pbefore->next = puse;


    pLink->Length++;
    return GR_TRUE;
}

int DelAtPos(tpLink pLink, int Pos)
{
    int i = 1;
    tpLinkNode p = pLink->pHeader, pbef = NULL;
    if (pLink->Length < Pos) {
        assert(0);
        return GR_FALSE;
    }
    if (Pos == 1) {
        pLink->pHeader = p->next;
        (pLink->Length)--;
        free(p->content);
        free(p);
        return GR_TRUE;
    }
    while (i != Pos)            // if Pos == 1????
    {
        pbef = p;
        p = p->next;
        i++;
    }
    pbef->next = p->next;
    (pLink->Length)--;
    free(p->content);
    free(p);
    return GR_TRUE;
}

int LinkRead(tpLink pLink, int Pos, char Content[])
{
    int i = 0;
    tpLinkNode p = pLink->pHeader;
    if ((Content == NULL) || (Pos > pLink->Length) || (Pos < 0)) {
        DPRINTF(("exceed the end ! \n"));
        return GR_FALSE;
    }
    while (i < Pos) {
        p = p->next;
        i++;
    }

    strcpy(Content, p->content);
    return GR_TRUE;
}
int LinkDestroy(tpLink pLink)
{
    tpLinkNode p = pLink->pHeader, pnext;

    if (pLink == NULL) {
        assert(0);
        return GR_FALSE;
    }

    while (p) {
        pnext = p->next;
        free(p->content);
        free(p);
        p = pnext;
    }
    pLink->pHeader = NULL;
    pLink->Length = 0;
    return GR_TRUE;
}

static void IniAudioMbox()
{
    static cyg_mbox AudMbox;
    cyg_mbox_create(&dpfAudMboxHandle, &AudMbox);
}

int PlayListSave(tpLink pLink, char FileName[])
{
    FILE *fp = NULL;
    tpLinkNode p = pLink->pHeader;
    int i = 1;
    fp = fopen(FileName, "w+");
    if (fp == NULL) {
        EPRINTF(("error! \n"));
        return GR_FALSE;
    }

    fprintf(fp, "[PLAY_LIST]\n");
    while ((i <= pLink->Length) && (p != NULL)) {        
        fprintf(fp, "MUSIC%d=%s\n", i, p->content);
        printf("PlayListSave MUSIC%d=%s\n", i, p->content);
        i++;
        p = p->next;
    }
    fclose(fp);
    return GR_TRUE;
}

static char gTimeBuf[16];
void UpdateTimeLabel( TN_WIDGET *widget, int min, int sec)
{   
    struct tm tm1;
    time_t t;
    tm1.tm_sec = sec; 
    tm1.tm_min = min;            
    strftime(gTimeBuf, 15, "%M:%S", &tm1);
    t = mktime(&tm1);

    tnUpdateLabelCaption(widget, gTimeBuf);
}

void initialNewSong(GR_BOOL bplay)
{
    gCurSongInfo.total_min = 1;
    gCurSongInfo.total_sec = 10;
    gCurSongInfo.cur_min = 0;
    gCurSongInfo.cur_sec = 0;
//    gCurSongInfo.bplay = GR_TRUE;
    gCurSongInfo.offsetrun  = (float)100 / (1*60+10);
    gCurSongInfo.currun = 0;

    gboolStart = bplay;
    tnSetCheckButtonStatus(gpckbPlay, gboolStart);
    tnUpdateLabelCaption(gplblCurFile, gCurSongInfo.filename);
    UpdateTimeLabel( gplblPlayTime, gCurSongInfo.cur_min, gCurSongInfo.cur_sec);
    UpdateTimeLabel( gplblAllTime, gCurSongInfo.total_min, gCurSongInfo.total_sec);
    tnProgressBarUpdate(gpprgbPlay, gCurSongInfo.currun);            
}

void HandleAudioEvent(DPF_EVENT * event)
{
    switch(event->aplayer.state)
    {
        case APLAYER_MAXMIZE:
            printf("HandleAudioEvent: APLAYER_MAXMIZE\n");           
            GrMapWindow(gpwndAudio->wid);
            GrSetFocus(gpckbPlay->wid);
            break;
        case APLAYER_MINIMIZE:
            printf("HandleAudioEvent: APLAYER_MINIMIZE\n");
            GrUnmapWindow(gpwndAudio->wid);
            break;
        case APLAYER_EXIT:
            break;
        case APLAYER_NEW:
            initialNewSong( GR_TRUE);                       
            
            #ifdef audio_test
            printf("Play %s\n", gCurSongInfo.filename);
            MediaDecoder_Start(gCurSongInfo.filename);
            MediaDecoder_SetCmd(MEDIA_CMD_PLAY);
            
            //wav_play();
            #endif                        
    
            break;            
        case APLAYER_PLAY:
            printf("HandleAudioEvent: APLAYER_PLAY %s\n", gCurSongInfo.filename);
            gboolStart = GR_TRUE;
            break;
        case APLAYER_PAUSE:            
            printf("HandleAudioEvent: APLAYER_PAUSE\n");
            gboolStart = GR_FALSE;
            break;
        case APLAYER_STOP:                     
            printf("HandleAudioEvent: APLAYER_STOP\n");            
            initialNewSong( GR_FALSE);      
            
            break;
        case APLAYER_NEXT:
            gnCurSongIndex = (gnCurSongIndex == gSongList.Length -1) ? 0: gnCurSongIndex+1;
            LinkRead(&gSongList, gnCurSongIndex, gCurSongInfo.filename);            
            initialNewSong( gboolStart);            
            printf("HandleAudioEvent: APLAYER_NEXT\n");
            break;
        case APLAYER_PREV:
            gnCurSongIndex = (gnCurSongIndex == 0) ? gSongList.Length -1: gnCurSongIndex - 1;
            LinkRead(&gSongList, gnCurSongIndex, gCurSongInfo.filename);            
            initialNewSong( gboolStart);
            printf("HandleAudioEvent: APLAYER_PREV\n");
            break;
        case APLAYER_VOLUP:
            printf("HandleAudioEvent: APLAYER_VOLUP\n");
            break;
        case APLAYER_VOLDWN:
            printf("HandleAudioEvent: APLAYER_VOLDWN\n");
            break;
    }
}

void OnAudioTimer(TN_WIDGET *widget, DATA_POINTER dptr)
{
    DPF_EVENT *event;
    event = (DPF_EVENT *) cyg_mbox_timed_get(dpfAudMboxHandle,10);
    if(event->type == DPF_APLAYER_STATE)
        HandleAudioEvent(event);        
    
    {        
        time_t ticks;
        struct tm *t;
        char time_b[16];

        ticks = time(NULL);
        t = localtime(&ticks);
        strftime(time_b, 15, "%H:%M", t);
        tnUpdateLabelCaption(gplblCurTime, time_b);
    }
    
    if(gboolStart)
    {
        if(gCurSongInfo.cur_sec == 59)
        {
            gCurSongInfo.cur_min++;
            gCurSongInfo.cur_sec = 0;
        }
        else            
            gCurSongInfo.cur_sec ++;
        
        UpdateTimeLabel( gplblPlayTime, gCurSongInfo.cur_min, gCurSongInfo.cur_sec);
        
        if((gCurSongInfo.cur_min == gCurSongInfo.total_min)&&
            (gCurSongInfo.cur_sec >= gCurSongInfo.total_sec))            
        {                                    
            gAudioEvent.aplayer.state = APLAYER_NEXT;
	        cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);
        }
        else
        {                
            gCurSongInfo.currun = gCurSongInfo.currun + gCurSongInfo.offsetrun;            
            tnProgressBarUpdate(gpprgbPlay, gCurSongInfo.currun);
        }
           
    }
}

void OnStartPauseClick(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_BOOL status;
    
    tnGetCheckButtonStatus( widget, &status);
    
    if(status)
    {
        printf("OnStartPauseKeyDown onstart\n");
        gAudioEvent.aplayer.state = APLAYER_PLAY;
	    cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);
    }
    else
    {
        printf("OnStartPauseKeyDown onpause\n");
        gAudioEvent.aplayer.state = APLAYER_PAUSE;
	    cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);
    }
}

void OnStopClick(TN_WIDGET *widget, DATA_POINTER dptr)
{    
    gAudioEvent.aplayer.state = APLAYER_STOP;
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);    
}

void OnPrevClick(TN_WIDGET *widget, DATA_POINTER dptr)
{
    gAudioEvent.aplayer.state = APLAYER_PREV;
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);    
}

void OnNextClick(TN_WIDGET *widget, DATA_POINTER dptr)
{
    gAudioEvent.aplayer.state = APLAYER_NEXT;
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);    
}

void OnFileAdd(TN_WIDGET *widget, DATA_POINTER dptr)
{
    char **files;
    int nCount;
    int *nSelects, nSelectCount = 0;
    int nCurSong, i;
    char curFilename[MAX_FILE_PATH];
        
    tnGetAllListItems(gptAudioFileBrowser->plsbFiles, &files, &nCount);
    tnGetSelectedListPos(gptAudioFileBrowser->plsbFiles, &nSelects,&nSelectCount);
    
    if(nSelectCount > 0)
        GetFullFilename(gCurrentDir,files[nSelects[0]],curFilename);
       
    LinkAdd( &gSongList, curFilename);
    
    for(i = 0 ; i <gSongList.Length; i++)
    {
        LinkRead(&gSongList, i, curFilename);
        printf("Song %d: %s \n", i, curFilename);
    }
}       

void OnFilePlay(TN_WIDGET *widget, DATA_POINTER dptr)
{ 
    char **files;
    int nCount;
    int *nSelects, nSelectCount = 0;
    GR_EVENT event;
    int nCurSong, i;
    char curFilename[MAX_FILE_PATH];
        
    tnGetAllListItems(gptAudioFileBrowser->plsbFiles, &files, &nCount);
    tnGetSelectedListPos(gptAudioFileBrowser->plsbFiles, &nSelects,&nSelectCount);
    
    if(nSelectCount > 0)
        gnCurSongIndex = nSelects[0];
    else
        gnCurSongIndex = 0;
    
    gAudioEvent.aplayer.state = APLAYER_STOP;
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);    
    LinkDestroy( &gSongList);
    
    for( i = 0; i < nCount; i++)
    {
        GetFullFilename(gCurrentDir,files[i],curFilename);
        LinkAdd( &gSongList, curFilename);        
        free( files[i] );
    }
    
    free(files);
    
    LinkRead(&gSongList, gnCurSongIndex, gCurSongInfo.filename);        

    free(nSelects);                                                                             
           
    event.type = GR_EVENT_TYPE_KEY_DOWN;
    event.keystroke.ch = MWKEY_ESCAPE;
    ProcessEvent(&event, widget);
    ProcessEvent(&event, gptAudioFileBrowser->pwndBrowser);

    //Post to main player to play new audio    	                    
    gAudioEvent.aplayer.state = APLAYER_NEW;               
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);  
        
}

void OnFileOperation(TN_WIDGET* pwidget, DATA_POINTER p)
{
    TN_WIDGET *wndFileOp, *btnOpen, *btnAdd;
    
    wndFileOp = CreateWindowFromRC(NULL,TN_CONTROL_FLAGS_DEFAULT, ID_WND_FILEOP,&gAudioRcinfo);
	btnOpen = CreateButtonFromRC(wndFileOp,TN_CONTROL_FLAGS_DEFAULT, ID_BTN_SONGOPEN,&gAudioRcinfo);
	btnAdd = CreateButtonFromRC(wndFileOp,TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ADDPLAYLIST,&gAudioRcinfo);
	
	tnRegisterCallBack( btnOpen, WIDGET_CLICKED, (CallBackFuncPtr)OnFilePlay, NULL);
	tnRegisterCallBack( btnAdd,  WIDGET_CLICKED, (CallBackFuncPtr)OnFileAdd, NULL);
	
	tnRegisterCallBack( btnOpen, WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack( btnAdd, WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
		
	tnRegisterCallBack(wndFileOp,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    tnRegisterCallBack(btnOpen,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    tnRegisterCallBack(btnAdd,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    
    GrSetFocus(btnOpen->wid);
}
void OnBrowserClick(TN_WIDGET* pwidget,DATA_POINTER p)
{    
       
    char rcfilepath[128];
        
    getRcFilePath(rcfilepath, RESOURCE_PATH, gSubject, BROWSER_NAME);
    EPRINTF(("rcfilepath---:%s\n",rcfilepath));
    printf("OnSlideShow\n");
    
    if(GsFindWindow(gptAudioFileBrowser->pwndBrowser->wid))
    {
        printf("double clicking OnSlideShow %d\n",gptAudioFileBrowser->pwndBrowser->wid);
        return;
    }    
    gptAudioFileBrowser = OpenFileBrowser(gpbtnBrowse, rcfilepath, gCurrentDir, selectAudio, OnFileOperation,NULL, NULL);
    
    
}

TN_WIDGET *gpwndPlaylist, *gplsbPlaylist, *gpwndPlaylistOp;

void OnPlaylistOpen(TN_WIDGET* pwidget,DATA_POINTER p)
{
   
    char **files;
    int nCount;
    int *nSelects, nSelectCount = 0;
    int i=0;
    GR_EVENT event;
        
    tnGetAllListItems(gplsbPlaylist, &files, &nCount);
    tnGetSelectedListPos(gplsbPlaylist, &nSelects,&nSelectCount);
    
    if(nSelectCount > 0)
        gnCurSongIndex = nSelects[0];
    else
        gnCurSongIndex = 0;
    
    gAudioEvent.aplayer.state = APLAYER_STOP;
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);             
    LinkDestroy( &gSongList);
    
    for( i = 0; i < nCount; i++)
    {
        LinkAdd( &gSongList, files[i]);
        printf("Song %d: %s\n",i, files[i]);
        free(	files[i]	);
    }
    
    free(files);
    
    LinkRead(&gSongList, gnCurSongIndex, gCurSongInfo.filename);    
    
    free(nSelects);    
    
    event.type = GR_EVENT_TYPE_KEY_DOWN;
    event.keystroke.ch = MWKEY_ESCAPE;
    //ProcessEvent(&event, pwidget);
    ProcessEvent(&event, gplsbPlaylist);
    
    //Post to main player to play new audio    
    gAudioEvent.aplayer.state = APLAYER_NEW;
	cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);    
}

void OnPlaylistDel(TN_WIDGET* pwidget,DATA_POINTER p)
{
   
    char **files;
    int nCount;
    int *nSelects, nSelectCount = 0;
    int i=0;
    tLink tempLink;
    GR_EVENT event;
    
    tnGetSelectedListPos(gplsbPlaylist, &nSelects,&nSelectCount);
    
    if(nSelectCount > 0)
    {
        gnCurSongIndex = nSelects[0];
        tnDeleteItemFromListBoxAt(gplsbPlaylist,nSelects[0]);
        tnSetSelectedListItemAt(gplsbPlaylist,nSelects[0],GR_TRUE);
    }
    
    free(nSelects);
    
    tnGetAllListItems(gplsbPlaylist, &files, &nCount);
    
    tempLink.Length = 0;
    tempLink.pHeader = NULL;
    for( i = 0; i < nCount; i++)
    {
        LinkAdd( &tempLink, files[i]);
        //printf("Song %d: %s\n",i, files[i]);
        free(	files[i]	);
    }
    
    free(files);
    
    PlayListSave(&tempLink, PLYLST_FILE_PATH);
    LinkDestroy( &tempLink);
    
           
}
void OnPlaylistOperation(TN_WIDGET* pwidget,DATA_POINTER p)
{
    TN_WIDGET *btnOpen, *btnDel;
    
    
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndPlaylistOp->wid))
    {
        printf("double clicking OnPlaylistClick()\n");
        return;
    }
    
    gpwndPlaylistOp = CreateWindowFromRC( gplsbPlaylist,TN_CONTROL_FLAGS_DEFAULT, ID_WND_PLAYLISTOP,&gAudioRcinfo);
	btnOpen = CreateButtonFromRC(gpwndPlaylistOp,TN_CONTROL_FLAGS_DEFAULT, ID_BTN_LISTTOPEN,&gAudioRcinfo);
	btnDel = CreateButtonFromRC(gpwndPlaylistOp,TN_CONTROL_FLAGS_DEFAULT, ID_BTN_DELLIST,&gAudioRcinfo);
	
	tnRegisterCallBack( btnOpen, WIDGET_CLICKED, (CallBackFuncPtr)OnPlaylistOpen, NULL);
	tnRegisterCallBack( btnDel,  WIDGET_CLICKED, (CallBackFuncPtr)OnPlaylistDel, NULL);
	
	tnRegisterCallBack( btnOpen, WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack( btnDel,  WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
		
	tnRegisterCallBack(gpwndPlaylistOp, WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    tnRegisterCallBack(btnOpen,  WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    tnRegisterCallBack(btnDel,   WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    
    GrSetFocus(btnOpen->wid);
}


void OnPlaylistClick(TN_WIDGET* pwidget,DATA_POINTER p)
{
    TN_WIDGET *lblTitle;
    
    //Check for reduplicate clicking
    //if(GsFindWindow(gpwndPlaylist->wid))
    if(GetFromRegistry(gpwndPlaylist->wid) != NULL)
    {
        printf("double clicking OnPlaylistClick()\n");
        return;
    }
    
    gpwndPlaylist = CreateWindowFromRC(NULL,TN_CONTROL_FLAGS_DEFAULT, ID_WND_PLAYLIST,&gAudioRcinfo);
	gplsbPlaylist = CreateListBoxFromRC(gpwndPlaylist,TN_CONTROL_FLAGS_DEFAULT, ID_LBX_PLAYLIST,&gAudioRcinfo);
	lblTitle = CreateLabelFromRC(gpwndPlaylist, TN_CONTROL_FLAGS_DEFAULT,ID_LBL_PLAYLIST,&gAudioRcinfo);
	
	tnRegisterCallBack( gplsbPlaylist, WIDGET_CLICKED, (CallBackFuncPtr)OnPlaylistOperation, NULL);
	
	tnRegisterCallBack( gpwndPlaylist, WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    tnRegisterCallBack( gplsbPlaylist, WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,pwidget);
    
    GrSetFocus(gplsbPlaylist->wid);
    
    //Inititalize listbox and read from playlist.dat
    {
        struct configFile *pConfig = NULL;
        char Key[10] = { 0 };
        char *pGetString = NULL;
        int i = 1;
        
        //pConfig = readConfigFile(PLYLST_FILE_PATH);
        pConfig = readPlayList(PLYLST_FILE_PATH);
        
        if (!pConfig) {
            EPRINTF(("can't open %s\n", PLYLST_FILE_PATH));        
        }
        while (1) {
            sprintf(Key, "MUSIC%d", i);
            pGetString = getConfigString(pConfig, "PLAY_LIST", Key);
            if (pGetString == NULL) {
                EPRINTF(("read over the playlist file! \n"));
                break;
            }
            tnAddItemToListBox( gplsbPlaylist, pGetString, -1);
            i++;
        }
        unloadConfigFile(pConfig);
        
        tnSetSelectedListItemAt(gplsbPlaylist,0,GR_TRUE);
    }
    
    
}

void OnPlaylistExit(TN_WIDGET* pwidget,DATA_POINTER p)
{
    OnSubMenuEscape( pwidget, p);
    gpwndPlaylist = NULL;
    
}

int gnVolValue;

void OnVolumeClick(TN_WIDGET *widget, DATA_POINTER dptr)
{
    tnSetVisible(gpprgbVolume, !(gpprgbVolume->visible));
}

void OnVolumeKeyDown(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_KEY ch;
    ch=(long)widget->data;
    
    switch (ch)
    {
        case DPFKEY_PREV:
        case DPFKEY_NEXT:            
            printf("OnVolumeKeyDown MWKEY_LEFT\n");
            tnSetVisible(gpprgbVolume, GR_FALSE);
            OnAudioFuncKeyDown(widget, dptr);
            break;
        case DPFKEY_VALUP:
            gnVolValue = (gnVolValue == 100) ? 100 : gnVolValue + 10;
            if(gpprgbVolume->visible == GR_FALSE)
                tnSetVisible(gpprgbVolume, GR_TRUE);
            tnProgressBarUpdate(gpprgbVolume, gnVolValue);            
            
            gAudioEvent.aplayer.state = APLAYER_VOLUP;
	        cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent);
            break;
        case DPFKEY_VALDOWN:
            gnVolValue = (gnVolValue == 0) ? 0 : gnVolValue - 10;
            if(gpprgbVolume->visible == GR_FALSE)
                tnSetVisible(gpprgbVolume, GR_TRUE);
            tnProgressBarUpdate(gpprgbVolume, gnVolValue);
            
            gAudioEvent.aplayer.state = APLAYER_VOLDWN;
	        cyg_mbox_put(dpfAudMboxHandle, (void *) &gAudioEvent); 
            break;        
    }
    
}

void OnMinimizeClick(TN_WIDGET *widget, DATA_POINTER dptr)
{    
    GrUnmapWindow(gpwndAudio->wid);
    GrSetFocus(gMinimizeFocus->wid);
    
	gAudioEvent.aplayer.state = APLAYER_MINIMIZE;
	cyg_mbox_put(dpfDesktopMboxHandle, (void *) &gAudioEvent);
}

void OnAudioFuncKeyDown(TN_WIDGET *widget, DATA_POINTER dptr) 
{
    TN_WIDGET *focus=widget;
    int *data = (int *) dptr;
    GR_KEY ch;
    GR_EVENT event;
    ch=(long)widget->data;
    
    switch (ch)
    {
        case DPFKEY_PREV:
        case MWKEY_LEFT:
            do
                focus=GetPrevSibling(focus);
            while((focus->type != TN_BUTTON) && (focus->type != TN_CHECKBUTTON));
            GrSetFocus(focus->wid);                   
            break;
        case DPFKEY_NEXT:
        case MWKEY_RIGHT:
            do
                focus=GetNextSibling(focus);
            while ((focus->type != TN_BUTTON) && (focus->type != TN_CHECKBUTTON));
            GrSetFocus(focus->wid);
            break;
        case DPFKEY_PLAY:
            if(!gboolStart)
            {
                GrSetFocus(gpckbPlay->wid);
                event.type = GR_EVENT_TYPE_KEY_DOWN;
                event.keystroke.ch = MWKEY_ENTER;
                ProcessEvent(&event, gpckbPlay);
            }
            break;
        case DPFKEY_PAUSE:
            if(gboolStart)
            {
                GrSetFocus(gpckbPlay->wid);
                event.type = GR_EVENT_TYPE_KEY_DOWN;
                event.keystroke.ch = MWKEY_ENTER;
                ProcessEvent(&event, gpckbPlay);
            }
            break;
         case DPFKEY_STOP:
            if(gboolStart)
            {
                GrSetFocus(gpbtnStop->wid);
                event.type = GR_EVENT_TYPE_KEY_DOWN;
                event.keystroke.ch = MWKEY_ENTER;
                ProcessEvent(&event, gpbtnStop);
            }
            break;
         case DPFKEY_PREV_SCAN:
            GrSetFocus(gpbtnPrev->wid);
            event.type = GR_EVENT_TYPE_KEY_DOWN;
            event.keystroke.ch = MWKEY_ENTER;
            ProcessEvent(&event, gpbtnPrev);            
            break;
         case DPFKEY_NEXT_SCAN:
            GrSetFocus(gpbtnNext->wid);
            event.type = GR_EVENT_TYPE_KEY_DOWN;
            event.keystroke.ch = MWKEY_ENTER;
            ProcessEvent(&event, gpbtnNext);            
            break;         
    }
}

void OnAudioEscape(TN_WIDGET *widget, DATA_POINTER dptr)
{
    LinkDestroy(&gSongList);
    
    gAudioEvent.aplayer.state = APLAYER_EXIT;
	cyg_mbox_put(dpfDesktopMboxHandle, (void *) &gAudioEvent);
    
    OnMenuEscape(widget, dptr);
}

void Audio_thread(CYG_ADDRWORD data)
{
    printf("wav_play from thread\n");
    FILE * file = NULL;
    int fsize = 0, err = 0, empty = 0, t = 0;
    struct stat *buf = NULL;
    unsigned int in_buf = 0, out_buf = 0;
    char* path = NULL;
    bool bPlay = true;

    path = "/sd/LOVE_1.WAV";        
    //Read file
   // file = fopen( path, "rb+" );
    //file = fopen( gCurSongInfo.filename, "rb+" );
   // buf = (struct stat *)malloc(sizeof(struct stat));
    //err = stat(path, buf);
    //if(err < 0)
    //{
    //    EPRINTF(("stat file error!!!\n"));
    //}
    //else
    //{
    //    EPRINTF(("stat file success!!!\n"));
    //}    
	//fsize = buf->st_size;
	
	//in_buf = (unsigned int)malloc(fsize);
	//in_buf = (unsigned int)malloc_align(fsize, 32);
	//fread(in_buf, 1, fsize, file);
    //iis_set_value(24, 48000); //bitrate = 24, samplerate = 48k
    //out_buf = (unsigned int)malloc_align(GW_FIFO_SIZE, 32);//32 byte align    
	//printf("out_buf = %x\n", out_buf);
	//printf("size = %x\n", fsize);
	bPlay = true;
	while(bPlay){
	    printf("out_buf = test\n");
	    cyg_thread_delay( 100 );
        //iis_send_data(in_buf, GW_FIFO_SIZE);
        
        cyg_uint32 i = 0, j = 0;
        
        for(j = 0; j < 10; j++)
        {
        
             for(i = 0; i < 8000000; i++)
        
             {
                  asm("NOP");
        
             }
        
        }

        //while(1 != (iis_get_status(FLAG_GW_EMPTY) & 0x1))
        //{
        //    int n = -1;
        //    n = iis_get_status(FLAG_GW_EMPTY);
        //    printf(" iis_get_status %d\n", n);
        //}
    }
}

#ifdef __ECOS

void Aplayer_thread(CYG_ADDRWORD data)
{    
    struct menuReturnData returndata;
    struct THREAD_PARAMETER *paraData = (struct THREAD_PARAMETER *) data;

    INIT_PER_THREAD_DATA(paraData->thread_data_index);
        
    returndata.wFocus = (TN_WIDGET *)paraData->others;
    returndata.prcinfo = &gAudioRcinfo;
    gMinimizeFocus = (TN_WIDGET *)paraData->others;
    
    IniAudioMbox();
    
#else
void main(void *argv)
{
#endif
    char rcfilepath[200];
	
		    
	//loadconfigfile
    gpcfgFile=readConfigFile(CFG_FILE_PATH);
    if(!gpcfgFile)
    {
        printf("can't load the configure file\n");
        return;
    }
	gSubject=getConfigString(gpcfgFile,"setting","subject");
	if (gSubject==NULL) return;
    
    gLang=getConfigString(gpcfgFile,"setting","language");
	
	if (gLang==NULL) return;
#ifdef __ECOS	    
	//tnControlLibInitialize();
	tnAppInitialize();
#else	
	tnAppInitialize();
#endif
    	//select the rc file according to the subject in configfile
	getRcFilePath(rcfilepath, RESOURCE_PATH, gSubject, AP_NAME);
    
    if(GetPath(rcfilepath,grcfileDir) < 0) return;
    
    if(load_rc(&gAudioRcinfo,rcfilepath, "en")<0)
    //if(load_rc(&gsettingRcinfo,rcfilepath, lang)<0)
    {
        EPRINTF(("Load rc file failure\n"));
        return;
    }
	//create background window
    gpwndAudio = CreateWindowFromRC(NULL, 0, ID_WND_AUDIO,&gAudioRcinfo);
	                
	
	// create buttons	
	gplblCurFile   = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_FILE, &gAudioRcinfo);
	gplblCurTime   = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_CURTIME, &gAudioRcinfo);
	gplblSong      = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_SONG, &gAudioRcinfo);
	gplblAlbum     = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_Album, &gAudioRcinfo);
	gplblSinger    = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_SINGER, &gAudioRcinfo);	
	gplblPlayTime  = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_PLAYTIME, &gAudioRcinfo);
	gplblAllTime   = CreateLabelFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_LBL_ALLTIME, &gAudioRcinfo);
	
	tnSetWidgetFGColor( gplblCurFile, GR_ARGB( 250, 255, 255, 255));
	
	gpprgbPlay     = CreateProgressBarFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_PGB_PLAY, &gAudioRcinfo);
	
	gpckbPlay      = CreateCheckButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_CKB_PLAY, &gAudioRcinfo);
	gpbtnStop      = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_STOP, &gAudioRcinfo);
	gpbtnPrev      = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_PREV, &gAudioRcinfo);    
	gpbtnNext      = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_NEXT, &gAudioRcinfo);
	
	//gprdbPlayMode  = CreateRadioButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_RDB_PLAYMODE, &gAudioRcinfo);
	gpbtnBrowse    = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_BROWSE, &gAudioRcinfo);
	gpbtnPlayList  = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_PLST, &gAudioRcinfo);    
	gpbtnVolume    = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_VOL, &gAudioRcinfo);
	gpprgbVolume   = CreateProgressBarFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_PGB_VOL, &gAudioRcinfo);
	
	gpbtnMinimize  = CreateButtonFromRC( gpwndAudio, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MIN, &gAudioRcinfo);
	
	GrCreateTimer(gpwndAudio->wid, 400);
    tnRegisterCallBack(gpwndAudio,WIDGET_TIMER,OnAudioTimer,NULL);
    
	// register callback function
	tnRegisterCallBack( gpckbPlay,     WIDGET_CLICKED, (CallBackFuncPtr)OnStartPauseClick, NULL);
	tnRegisterCallBack( gpbtnStop,     WIDGET_CLICKED, (CallBackFuncPtr)OnStopClick,NULL);
	tnRegisterCallBack( gpbtnPrev,     WIDGET_CLICKED, (CallBackFuncPtr)OnPrevClick,NULL);    
	tnRegisterCallBack( gpbtnNext,     WIDGET_CLICKED, (CallBackFuncPtr)OnNextClick, NULL);
	tnRegisterCallBack( gpbtnBrowse,   WIDGET_CLICKED, (CallBackFuncPtr)OnBrowserClick, NULL);
	tnRegisterCallBack( gpbtnPlayList, WIDGET_CLICKED, (CallBackFuncPtr)OnPlaylistClick, NULL);
	tnRegisterCallBack( gpbtnVolume,   WIDGET_CLICKED, (CallBackFuncPtr)OnVolumeClick, NULL);
	tnRegisterCallBack( gpbtnMinimize,   WIDGET_CLICKED, (CallBackFuncPtr)OnMinimizeClick, NULL);					

	tnRegisterCallBack( gpckbPlay,     WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
	tnRegisterCallBack( gpbtnStop,     WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
    tnRegisterCallBack( gpbtnPrev,     WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
	tnRegisterCallBack( gpbtnNext,     WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
	//tnRegisterCallBack( gprdbPlayMode,  WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
	tnRegisterCallBack( gpbtnBrowse,   WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
    tnRegisterCallBack( gpbtnPlayList, WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
	tnRegisterCallBack( gpbtnVolume,   WIDGET_KEYDOWN, (CallBackFuncPtr)OnVolumeKeyDown, NULL);
	tnRegisterCallBack( gpbtnMinimize, WIDGET_KEYDOWN, (CallBackFuncPtr)OnAudioFuncKeyDown, NULL);
   
	tnRegisterCallBack( gpckbPlay,     WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);
	tnRegisterCallBack( gpbtnStop,     WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);
	tnRegisterCallBack( gpbtnPrev,     WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);    
	tnRegisterCallBack( gpbtnNext,     WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);	
	//tnRegisterCallBack( gprdbPlayMode, WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);
	tnRegisterCallBack( gpbtnBrowse,   WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);
	tnRegisterCallBack( gpbtnPlayList, WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);    
	tnRegisterCallBack( gpbtnVolume,   WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);	
	tnRegisterCallBack( gpbtnMinimize, WIDGET_ESCAPED, (CallBackFuncPtr)OnAudioEscape, (DATA_POINTER) &returndata);	
	// set focus on the first button.	
	GrSetFocus(gpckbPlay->wid);
	
	//Initialize global value
	gSongList.Length = 0;
    gSongList.pHeader = NULL;
	gnCurSongIndex = 0;
	gnVolValue = 50;
	tnProgressBarUpdate( gpprgbVolume, gnVolValue);
	tnSetVisible( gpprgbVolume, GR_FALSE);
	
	gAudioEvent.aplayer.type = DPF_APLAYER_STATE;
	gAudioEvent.aplayer.state = APLAYER_MAXMIZE;
	cyg_mbox_put(dpfDesktopMboxHandle, (void *) &gAudioEvent);
	diag_mallocinfo();
	tnMainLoop();
	return;
}
