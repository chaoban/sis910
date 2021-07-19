#include<stdio.h>
#include <time.h>
#include <tinywidget/tnWidgets.h>
#include "../share/global.h"
#include "../config/config.h"
#include "../share/ecos_config.h"
#include "desktop.h"
#include "nxThread.h"
#include <pkgconf/kernel.h>
#include <cyg/kernel/kapi.h> 
#include "global.h"
#include "../share/device.h"
#include <assert.h>
#define MWINCLUDECOLORS

#ifdef __ECOS
#include <cyg/kernel/kapi.h>

struct configFile *gcfgfile;
cyg_handle_t dpfDesktopMboxHandle;
extern cyg_handle_t dpfAudMboxHandle;

extern void Aplayer_thread(CYG_ADDRWORD data);
extern void Audio_thread(CYG_ADDRWORD data);
extern void Photo_thread(CYG_ADDRWORD data);
extern void FB_thread(CYG_ADDRWORD data);
extern void Clock_thread(CYG_ADDRWORD data);
extern void Setting_thread(CYG_ADDRWORD data);
//extern void Reader_thread(CYG_ADDRWORD data);


//all nano_thread will use the same slot,which is created in template.c
#else
#define CFG_FILE_PATH  "./resource/config.ini"
#define RESOURCE_PATH  "./resource"
#endif //end of __ECOS

TN_WIDGET *dpfDesktopWindow;

TN_WIDGET *gDiskHintWindow;
TN_WIDGET *gHintLabel;

TN_WIDGET *gAlarmHintWindow;
TN_WIDGET *gAlarmHintLabel;

static TN_WIDGET *gDesktopButton[6];
//static TN_WIDGET *gpFocusWidget;
static TN_WIDGET *gTitleLabel;
static TN_WIDGET *gDateLabel;
static TN_WIDGET *gTimeLabel;
static RCINFO gDesktopRcinfo;
static char gDateBuf[20];
static char gTimeBuf[20];
static GR_BOOL gAplayerRunning = GR_FALSE;

PBTNGROUP gbtngrpTest;
int     gnAnimation;
int     gAnimationCount = -1;

DPF_EVENT gAudioEvent;

bool gBoolAlarm = false;
int gAlarmState = 0;
int gAlarmHour;
int gAlarmMin;

static void OnTimer(TN_WIDGET * widget, void *p);
static void OnClicked(TN_WIDGET * pWidget, void *p);
static void GetDate(char *date_b);
void GetTime(char *time_b);
static GR_BOOL IsAudioFormat(char *fileName);
static void InitMbox();
static void SubjectSwitch(char *subject_name, char *lang_name);
static int DesktopMain(int argc, char **argv);

#ifdef __ECOS

static struct tNxThread gApThreads[] = {
    {"mp3",Aplayer_thread, 17, 0, 0},
    //{"fb", Audio_thread, 14, 0, 0},
    {"fb", FB_thread, 14, 0, 0},
    {"setting", Setting_thread, 14, 0, 0},
    //{"reader", Reader_thread, 14, 0},
    {"clock", Clock_thread, 14, 0, 0},
    {"photo", Photo_thread, 14, 0 ,0},
    {NULL, NULL, 0},
};

static struct tNxThread *GetEntry(char *name)
{
    int i;
    for (i = 0; gApThreads[i].name != NULL; i++) {
        if (strcmp(name, gApThreads[i].name) == 0) {
            return &gApThreads[i];
        }
    }
    return NULL;
}
#endif //end of __ECOS

static void SubjectSwitch(char *subject_name, char *lang_name)
{
    char rcfilepath[200];

    getRcFilePath(rcfilepath, RESOURCE_PATH, subject_name, "Desktop");
    //change subject
    if (switch_subject(&gDesktopRcinfo, rcfilepath, lang_name) < 0) {
        printf("change subject failue\n");
        return;
    } else {
        printf("change subject ok\n");
    }
}

static GR_BOOL IsAudioFormat(char *fileName)
{
    char format[5];
    strcpy(format,fileName + strlen(fileName)-4);
    format[4]='\0';
    printf ("format is %s\n",format);
    if (strcasecmp(format,".mp3") == 0){
        return GR_TRUE;
    }
    else{
        return GR_FALSE;
    }
   
}

void UpdateAlarmState()
{
    char *state;
    
    state = getConfigString(gcfgfile, "CLOCK", "AlarmState");
    
    if( !strcmp(state,"ON") )
    {
        gBoolAlarm = true;
        gAlarmState = 1;
    }
    else
    {        
        gBoolAlarm = false;
        gAlarmState = 0;
    }
        
    gAlarmHour = getConfigInt(gcfgfile, "CLOCK", "AlarmHour");
    gAlarmMin = getConfigInt(gcfgfile, "CLOCK", "AlarmMinute");
    
}
int nalarmcount = 0;

static void OnAlarmKeyDown(TN_WIDGET * pWidget, void *p)
{
    TN_WIDGET *focus=pWidget;
    GR_KEY ch;
    ch=(long)pWidget->data;
    dprintf("%s - clicked\n",__FUNCTION__);
    switch (ch)
    {
        case DPFKEY_PLAY:
            dprintf("%s - DPFKEY_PLAY clicked\n",__FUNCTION__);            
            GrUnmapWindow(gAlarmHintWindow->wid);            
            GrUngrabKey(gAlarmHintWindow, DPFKEY_PLAY);
            tnRegisterCallBack(gAlarmHintWindow, WIDGET_KEYDOWN, NULL, (DATA_POINTER) NULL);            
            break;
    }    
}

void DetectAlarm(struct tm *t)
{       
    switch(gAlarmState)
    {
        case 1:
            if((t->tm_hour == gAlarmHour)&&(t->tm_min == gAlarmMin))
            {
                printf("DetectAlarm ON: %d %d\n", gAlarmHour, gAlarmMin);
                gAlarmState = 2;
                GrGrabKey(gAlarmHintWindow->wid, DPFKEY_PLAY, GR_GRAB_HOTKEY_EXCLUSIVE);                
                nalarmcount = 0;                
                tnRegisterCallBack(gAlarmHintWindow, WIDGET_KEYDOWN, (CallBackFuncPtr)OnAlarmKeyDown/*OnControlFocusExchange*/, (DATA_POINTER) NULL);
                GrRaiseWindow(gAlarmHintWindow->wid);
                GrMapWindow(gAlarmHintWindow->wid);
                                
            }
            break;
         case 2:
            dprintf("%s - %d\n",__FUNCTION__, nalarmcount);
            nalarmcount ++;
            if(nalarmcount == 60)
            {
                GrUnmapWindow(gAlarmHintWindow->wid);
                GrUngrabKey(gAlarmHintWindow, DPFKEY_PLAY);
                tnRegisterCallBack(gAlarmHintWindow, WIDGET_KEYDOWN, NULL, (DATA_POINTER) NULL);
                nalarmcount = 0;
                gAlarmState = 1;
                UpdateAlarmState();
            }
            break;
    }

}

void HandlePDFevent(DPF_EVENT * event)
{
    switch(event->type)
    {
        case DPF_SUBJECT_UPDATE:
            printf("HandlePDFevent DPF_SUBJECT_UPDATE: %s\n", (event->subject).subjecttitle);
            SubjectSwitch((event->subject).subjecttitle,"en");
            break;
        
        case DPF_LANGUAGE_UPDATE:
            break;
        case DPF_APLAYER_STATE:
            printf("HandlePDFevent DPF_APLAYER_STATE :%d\n", event->aplayer.state);
            break;
        case DPF_ALARM_UPDATE:
            printf("HandlePDFevent DPF_ALARM_UPDATE\n");
            UpdateAlarmState();
            break;
    }
}


static void OnBtnGrpKeyDown(TN_WIDGET * pWidget, void *p)
{
    TN_WIDGET *focus=pWidget;
    GR_KEY ch;
    ch=(long)pWidget->data;
    
    switch (ch)
    {
        case DPFKEY_PREV:
        //case DPFKEY_VALDOWN:
            tnSelectPrevBtnGroupItem( gbtngrpTest);
            break;
        case DPFKEY_NEXT:
        //case DPFKEY_VALUP:
            tnSelectNextBtnGroupItem( gbtngrpTest);
            break;
        case DPFKEY_PLAY:
            dprintf("%s - DPFKEY_PLAY clicked\n",__FUNCTION__);
            break;
    }    
}

static void OnClicked(TN_WIDGET * pWidget, void *p)
{
    dprintf("button %s was clicked\n", (char *) p);

#ifdef __ECOS
    char *cmd = (char *) p;
    struct tNxThread *p_thread = GetEntry(cmd);
    if (p_thread)
    {
        cyg_uint16 id = 0;
        cyg_uint32 state = 0;
        cyg_handle_t test = 0;
        GR_BOOL bCreate = GR_FALSE;           
        {                
             id = (p_thread->t_obj).unique_id;
             state = (p_thread->t_obj).state;
             //self->state = EXITED;
             printf("before thread id = %d state = %d\n",id, state);
             test = cyg_thread_find( id );
             if((test !=NULL)&&(state!=16))
             {
                printf("found\n");
                bCreate = GR_FALSE;
             }
             else
             {
                bCreate = GR_TRUE;
                printf("not found\n");
             }
        }
        //if audioplayer is runnning in background and you click audioplayer button       
        if (p_thread == &gApThreads[0] && bCreate == GR_FALSE)
        {
            printf("send APLAYER_MAXMIZE mail to aplayer\n");
            gAudioEvent.type = DPF_APLAYER_STATE;
            gAudioEvent.aplayer.state = APLAYER_MAXMIZE;
            cyg_mbox_put(dpfAudMboxHandle, (void *)&gAudioEvent);
        }
        //else
        {
            p_thread->parameters.thread_data_index = dpfgNanoxDataIndex;
            p_thread->parameters.others = (void *)pWidget;
            //p_thread->parameters.handle = &(p_thread->t);
                                            
            //if(p_thread->t == 0)
            if(bCreate)
            {
                printf("thread create %x\n",((TN_WIDGET *)p_thread->parameters.others)->wid);
                cyg_thread_create(p_thread->prio,
                          p_thread->entry,
                          //(cyg_addrword_t) dpfgNanoxDataIndex,
                          (cyg_addrword_t) &p_thread->parameters,                                             
                          p_thread->name, (void *) p_thread->stack, STACKSIZE, &p_thread->t, &p_thread->t_obj);
                cyg_thread_resume(p_thread->t);
            }
            {                
                 id = (p_thread->t_obj).unique_id;
                 state = (p_thread->t_obj).state;
                 //self->state = EXITED;
                 printf("after thread id = %d state = %d\n",id, state);
                 test = cyg_thread_find( id );
                 if(test !=NULL)
                    printf("found\n");
                 else
                    printf("not found\n");
            }
        
        }

        if(p_thread == &gApThreads[0])
        {
            gAplayerRunning = GR_TRUE;
        }
    } 
    else 
    {
        printf("now we don't assign a function for %s\n", cmd);
    }
#else
    SubjectSwitch("Blue", "en");
#endif //end of __ECOS

}

static void GetDate(char *date_b)
{
    time_t ticks;
    struct tm *t;

    ticks = time(NULL);
    t = localtime(&ticks);
    strftime(date_b, 127, "%Y-%m-%d", t);
    //dprintf("todate date =%s\n", date_b);
}

void GetTime(char *time_b)
{
    time_t ticks;
    struct tm *t;

    ticks = time(NULL);
    t = localtime(&ticks);
    strftime(time_b, 127, "%Y-%m-%d  %H:%M:%S", t);
    
    DetectAlarm(t);
    //dprintf("Now Time =%s\n", time_b);
} 


#ifdef CYGPKG_DEVS_DISK_SD_SIS910
//#define  USE_SDDISK
//#include <cyg/io/disk.h>
//#include <cyg/io/sd_sis910.h>
//#include <cyg/io/mmc_protocol.h>
//#include <cyg/io/sd_protocol.h>

GR_BOOL bShow = GR_FALSE;
//
//static int  CheckSDisk(char *name)
//{
//    int ret = 0;
//    ret = mount( "/dev/sdblk0/", name, "fatfs" );
//    
//    if (ret)
//    {
//        //diag_printf(" Mount 0 fails!; err = %d\n", ret);
//        ret = mount( "/dev/sdblk0/1", name, "fatfs" );
//        if (ret)
//        {
//            //diag_printf(" SD Mount fails!\n");
//            return 0;
//        }
//        else
//        {
//            diag_printf(" SD Mount success!\n");
//                        
//        }
//    }
//    else
//    {
//        diag_printf(" SD Mount success!\n");        
//    }
//    
//    return 1;
//}
//

//extern STORAGEDEVICE gStorageDevice[4];
void DetectSDCard()
{
    //if(ncount==10)
    //{
        if(bShow)
        {
            if(CheckSDisk( &gStorageDevice[1]))
            {
                //GrUnmapWindow(gDiskHintWindow->wid);
                //umount("/sd1");
                //bShow = GR_FALSE;
            }
            else
            {
                //umount("/sd1");
                UmountSDisk(&gStorageDevice[1]);
//                GrUnmapWindow(gDiskHintWindow->wid);
                bShow = GR_FALSE;
            }
            //if(ncount == 5)
            //{
            //    ncount = 0;
            //    GrUnmapWindow(gDiskHintWindow->wid);
            //    printf("GrUnmapWindow gDiskHintWindow\n"); 
            //}
            //else
            //    ncount ++;            
        }
        else
        {
            if(MountSDisk(&gStorageDevice[1]))
            {
                //GrRaiseWindow(gDiskHintWindow->wid);
                //GrMapWindow(gDiskHintWindow->wid);
                bShow = GR_TRUE;
            }            
        }
    //                
    //}
}
#endif

int ncount=0;

static void OnTimer(TN_WIDGET * widget, void *p)
{

    DPF_EVENT *event;
    
    event = (DPF_EVENT *) cyg_mbox_timed_get(dpfDesktopMboxHandle,10);
    if(event->type)
        HandlePDFevent(event);

    GetTime(gTimeBuf);
    
    tnUpdateLabelCaption(gTimeLabel, gTimeBuf);
    
#ifdef CYGPKG_DEVS_DISK_SD_SIS910
    DetectSDCard();
#endif
    
    
//
////    GrInjectKeyboardEvent(dpfDesktopWindow->wid,'a',0,0,1);//added by nifei
}

#define JPEG_FILE  "/romdisk/resource/480x234/test4.JPG"

static int DesktopMain(int argc, char **argv)
{

    int i;
    char rcfilepath[200];
    char *subject = NULL;
    int test = 0;
    GR_IMAGE_ID   testid;
    
#ifdef __ECOS
    tnControlLibInitialize();
#else
    tnAppInitialize();
#endif //end of __ECOS
    //select the rc file according to the subject in configfile
    gcfgfile = readConfigFile(CFG_FILE_PATH);
    if (!gcfgfile) {
        printf("can't load the configure file %s\n", CFG_FILE_PATH);
        return 0;
    }
    subject = getConfigString(gcfgfile, "setting", "subject");
    if (subject == NULL)
        return 0;
    
    getRcFilePath(rcfilepath, RESOURCE_PATH, subject, "Desktop");
    if (load_rc(&gDesktopRcinfo, rcfilepath, "en") < 0) {
        printf("Failed to load rc file %s!\n", rcfilepath);
        return -1;
    }

    
    dpfDesktopWindow = CreateWindowFromRC(NULL, 0, ID_WND_DESKTOP, &gDesktopRcinfo);    
        
    GrSetFocus(dpfDesktopWindow->wid);
    
    gTitleLabel = CreateLabelFromRC(dpfDesktopWindow, 0,ID_LBL_DESKTOP,&gDesktopRcinfo);
    gDateLabel  = CreateLabelFromRC(dpfDesktopWindow, 0,ID_LBL_DATE,&gDesktopRcinfo);
    gTimeLabel  = CreateLabelFromRC(dpfDesktopWindow, 0,ID_LBL_TIME,&gDesktopRcinfo);
    //
    GetDate(gDateBuf);
    GetTime(gTimeBuf);
    tnSetLabelCaption(gDateLabel,gDateBuf);
    tnSetLabelCaption(gTimeLabel,gTimeBuf);
    //
    GrCreateTimer(dpfDesktopWindow->wid, 1000);
    tnRegisterCallBack(dpfDesktopWindow,WIDGET_TIMER,OnTimer,NULL);   
    //    
    
    // create buttons
    gDesktopButton[0] = CreateButtonFromRC(dpfDesktopWindow, TN_CONTROL_FLAGS_DEFAULT/*|GR_WM_PROPS_TRANSPARENT*/, ID_BTN_AUDIO, &gDesktopRcinfo);
    gDesktopButton[1] = CreateButtonFromRC(dpfDesktopWindow, TN_CONTROL_FLAGS_DEFAULT/*|GR_WM_PROPS_TRANSPARENT*/, ID_BTN_PHOTO, &gDesktopRcinfo);
    gDesktopButton[2] = CreateButtonFromRC(dpfDesktopWindow, TN_CONTROL_FLAGS_DEFAULT/*|GR_WM_PROPS_TRANSPARENT*/, ID_BTN_FILE, &gDesktopRcinfo);
    gDesktopButton[3] = CreateButtonFromRC(dpfDesktopWindow, TN_CONTROL_FLAGS_DEFAULT/*|GR_WM_PROPS_TRANSPARENT*/, ID_BTN_CLOCK, &gDesktopRcinfo);
    gDesktopButton[4] = CreateButtonFromRC(dpfDesktopWindow, TN_CONTROL_FLAGS_DEFAULT/*|GR_WM_PROPS_TRANSPARENT*/, ID_BTN_SETTING, &gDesktopRcinfo);
    //gDesktopButton[5] = CreateButtonFromRC(dpfDesktopWindow, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_READER, &gDesktopRcinfo);
    //
    gbtngrpTest = CreateBtnGroupFromRC( dpfDesktopWindow, ID_BTNGRP_TYPE0, &gDesktopRcinfo);
    tnAddItemToBtnGroup( gbtngrpTest, gDesktopButton[0]);
    tnAddItemToBtnGroup( gbtngrpTest, gDesktopButton[1]);
    tnAddItemToBtnGroup( gbtngrpTest, gDesktopButton[2]);
    tnAddItemToBtnGroup( gbtngrpTest, gDesktopButton[3]);
    tnAddItemToBtnGroup( gbtngrpTest, gDesktopButton[4]);
    
    tnRegisterCallBack(gDesktopButton[0], WIDGET_CLICKED, (CallBackFuncPtr) OnClicked, (DATA_POINTER) "mp3");
    tnRegisterCallBack(gDesktopButton[1], WIDGET_CLICKED, (CallBackFuncPtr) OnClicked, (DATA_POINTER) "photo");
    tnRegisterCallBack(gDesktopButton[2], WIDGET_CLICKED, (CallBackFuncPtr) OnClicked, (DATA_POINTER) "fb");
    tnRegisterCallBack(gDesktopButton[3], WIDGET_CLICKED, (CallBackFuncPtr) OnClicked, (DATA_POINTER) "clock");
    tnRegisterCallBack(gDesktopButton[4], WIDGET_CLICKED, (CallBackFuncPtr) OnClicked, (DATA_POINTER) "setting");
    //tnRegisterCallBack(gDesktopButton[5], WIDGET_CLICKED, (CallBackFuncPtr) OnClicked, (DATA_POINTER) "reader");    
    ////
    for (i = 0; i < 5; ++i) {
        tnRegisterCallBack(gDesktopButton[i], WIDGET_KEYDOWN, (CallBackFuncPtr)OnBtnGrpKeyDown/*OnControlFocusExchange*/, (DATA_POINTER) NULL);
    }
    //GrSetFocus(gDesktopButton[0]->wid); 
    tnSetFocusBtnGroupItem(gbtngrpTest, gDesktopButton[0]); 
    
    //////Initial SD Card Hint Window
    gDiskHintWindow = CreateWindowFromRC(NULL, 0, ID_WND_DISKHINT, &gDesktopRcinfo);
    gHintLabel = CreateLabelFromRC(gDiskHintWindow, 0,ID_LBL_DISKHINT,&gDesktopRcinfo);
    GrUnmapWindow(gDiskHintWindow->wid);
    //
    ////Initial Alarm Hint Window
    gAlarmHintWindow = CreateWindowFromRC(NULL, 0, ID_WND_ALARMHINT, &gDesktopRcinfo);
    gAlarmHintLabel = CreateLabelFromRC(gAlarmHintWindow, 0,ID_LBL_ALARMHINT,&gDesktopRcinfo);
    GrUnmapWindow(gAlarmHintWindow->wid);      
    
    //Initial Alarm State
    UpdateAlarmState();
     diag_mallocinfo();
    
    
    //unloadConfigFile(cfgfile);
#ifdef CYGPKG_HAL_R16      
    GdSetHwUsage(1);
#endif    
    tnMainLoop();
    return 0;
}

#ifdef __ECOS
static void InitMbox()
{
    //mbox = (cyg_mbox *)cyg_mempool_var_try_alloc(var_mempool_h, sizeof(cyg_mbox));

    static cyg_mbox mbox;
    cyg_mbox_create(&dpfDesktopMboxHandle, &mbox);
}

void DpfDesktopThread(CYG_ADDRWORD data)
{
    int argc = 0;
    char **argv;
    INIT_PER_THREAD_DATA(data);
    InitMbox();
    DesktopMain(argc, argv);
}
#else
int main(int argc, char **argv)
{
    DesktopMain(argc, argv);
    
    return 1;
}
#endif //end of __ECOS
