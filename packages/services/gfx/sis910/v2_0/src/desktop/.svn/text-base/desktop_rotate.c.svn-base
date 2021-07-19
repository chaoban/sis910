#include<stdio.h>
#include <time.h>
#include "tinywidget/tnWidgets.h"
#include "config.h"
#include "ecos_config.h"
#include "desktop.h"
#include "nxThread.h"
#include "global.h"
#include <assert.h>
#include <math.h>
#include <pthread.h>
#define MWINCLUDECOLORS
#ifdef __ECOS
#include <cyg/kernel/kapi.h>

cyg_handle_t dpfDesktopMboxHandle;
char dpfFbOpenedFileName[512];
//extern cyg_handle_t dpfAudMboxHandle;


extern void Setting_thread(CYG_ADDRWORD data);
extern void Clock_thread(CYG_ADDRWORD data);
extern void photo_thread(CYG_ADDRWORD data);
extern void FB_thread(CYG_ADDRWORD data);
extern void Aplayer_thread(CYG_ADDRWORD data);
extern void Reader_thread(CYG_ADDRWORD data);
//all nano_thread will use the same slot,which is created in template.c
#else
#define CFG_FILE_PATH  "./resource/config.ini"
#define RESOURCE_PATH  "./resource"
#endif                          //end of __ECOS

TN_WIDGET *dpfDesktopWindow;

#ifdef __ECOS
#define PHOTO_FILE "/romdisk/resource/800x480/Desktop/photo_y.jpg"
#define AUDIO_FILE "/romdisk/resource/800x480/Desktop/mp3_y.jpg"
#define SETTING_FILE "/romdisk/resource/800x480/Desktop/setup_y.jpg"
#define READER_FILE "/romdisk/resource/800x480/Desktop/reader_y.jpg"
#define FB_FILE "/romdisk/resource/800x480/Desktop/fb_y.jpg"
#define CLOCK_FILE "/romdisk/resource/800x480/Desktop/clock_y.jpg"

//#define PHOTO_FILE "/resource/240x320/Desktop/photo_y.bmp"
//#define AUDIO_FILE "/resource/240x320/Desktop/mp3_y.bmp"
//#define SETTING_FILE "/resource/240x320/Desktop/setup_y.bmp"
//#define READER_FILE "/resource/240x320/Desktop/reader_y.bmp"
//#define FB_FILE "/resource/240x320/Desktop/fb_y.bmp"
//#define CLOCK_FILE "/resource/240x320/Desktop/clock_y.bmp"
#else
#define PHOTO_FILE "/home/nf/repos/control/demo/photo_y.bmp"
#define AUDIO_FILE "/home/nf/repos/control/demo/mp3_y.bmp"
#define SETTING_FILE "/home/nf/repos/control/demo/setting_y.bmp"
#define READER_FILE "/home/nf/repos/control/demo/reader_y.bmp"
#define FB_FILE "/home/nf/repos/control/demo/fb_y.bmp"
#define CLOCK_FILE "/home/nf/repos/control/demo/clock_y.bmp"

#endif                          //end of __ECOS

#define PI 3.1415
#define MENU_NUM   6

struct tButtonInfo {
    char *picPath;
    GR_GC_ID imgId;
    GR_RECT rect;
    float angleToX;
};

const char gPicList[MENU_NUM][80] = {
    PHOTO_FILE,
    CLOCK_FILE,
    FB_FILE,
    READER_FILE,
    SETTING_FILE,
    AUDIO_FILE,
};

static int gMainWinWidth = 0;
static int gMainWinHeight = 0;
static int gCenterXpos = 0;
static int gCenterYpos = 0;
static int gLongRadiu = 0;
static int gShortRadiu = 0;

static const int gPicWidth = 100;
static const int gPicHeight = 100;
static const int gSteps = 3;

static const float gBeginAngle = 1.0 / 2 * PI + 1.0 / 9 * PI;

static struct tButtonInfo gButtonList[MENU_NUM];
static GR_TIMER_ID gRotateTimer = 0;

static GR_BOOL gIsLeftPressed = GR_FALSE;
static GR_BOOL gIsRightPressed = GR_FALSE;
static GR_BOOL gIsMoveStart = GR_FALSE;

static void InitButtons();
static float RatioFromAngle(float angle);
static ButtonsRotate(float angle);
static void OnDraw(void *p);
static void OnKeyDown(TN_WIDGET * widget, DATA_POINTER data);

//static TN_WIDGET *gDesktopButton[5];
//static TN_WIDGET *gpFocusWidget;
static RCINFO gDesktopRcinfo;
static char gDateBuf[20];
static char gTimeBuf[20];
static GR_BOOL gAplayerRunning = GR_FALSE;
static int gFocusIndex = 0;

static void OnTimer(TN_WIDGET * widget, void *p);
static void OnClicked(TN_WIDGET * pWidget, void *p);

static void HandleMail(enum MAIL_TYPE mail);
static void GetDate(char *date_b);
static void GetTime(char *time_b);
static GR_BOOL IsAudioFormat(char *fileName);
static void InitMbox();
static void SubjectSwitch(char *subject_name, char *lang_name);
static int DesktopMain(int argc, char **argv);
#ifdef __ECOS
static struct tNxThread gApThreads[] = {
    {"mp3", Aplayer_thread, 17},
    {"photo", photo_thread, 17},
    {"fb", FB_thread, 17},
    {"setting", Setting_thread, 17},
    {"reader", Reader_thread, 17},
    {"clock", Clock_thread, 17},    
    {NULL, NULL, 0},
};

static char gMenuList[MENU_NUM] [10]={
    "photo",
    "mp3",
    "setting",
    "reader",
    "fb",
    "clock"
};

#endif                          //end of __ECOS
static void InitValues()
{
    gMainWinWidth = 800;
    gMainWinHeight = 480;
    gCenterXpos = gMainWinWidth / 2 -20;
    gCenterYpos = gMainWinHeight / 2;
    gLongRadiu = gMainWinWidth / 4;
    gShortRadiu = gMainWinHeight / 4;
}
static void InitButtons()
{
    float offsetAngel = 2 * PI / MENU_NUM;
    int i;
    for (i = 0; i < MENU_NUM; ++i) {
        float angleToX = gBeginAngle + i * offsetAngel;
        gButtonList[i].rect.x = (int) floor(gCenterXpos + gLongRadiu * cos(angleToX));
        gButtonList[i].rect.y = (int) floor(gCenterYpos + gShortRadiu * sin(angleToX));
        gButtonList[i].picPath = (char *) gPicList[i];
	    gButtonList[i].imgId = GrLoadImageFromFile(gButtonList[i].picPath,0);
        gButtonList[i].angleToX = angleToX;
        float ratio = RatioFromAngle(gButtonList[i].angleToX);
        printf("%d - %f - %f\n",i,gButtonList[i].angleToX * 180/PI, ratio);
        gButtonList[i].rect.width = (int) floor(gPicWidth * ratio);
        gButtonList[i].rect.height = (int) floor(gPicHeight * ratio);

        //printf("InitButtons %d rect.x = %d, rect.y =%d, rect.width = %d, rect.height = %d \n",
        //   i,gButtonList[i].rect.x,
		//	 gButtonList[i].rect.y,
		//	 gButtonList[i].rect.width,
		//	 gButtonList[i].rect.height);
        //printf("%d, x =%d,y=%d,angelToX = %f\n",i,gButtonList[i].rect.x,gButtonList[i].rect.y,gButtonList[i].angleToX *180/PI); 
        // gButtonList[i].pButton = CreatePicture(wnd, 0, &gButtonList[i].rect, gButtonList[i].picPath, GR_FALSE);
    }

}
static float RatioFromAngle(float angle)
{
    if (fabs(gBeginAngle - angle) < 0.01) {
	//printf("angle %f is clear to gBeginAngle\n",angle * 180/3.1415);
        angle = gBeginAngle;
    }
    
    //printf("angle = %f\n",angle * 180/3.1415);
    if (angle < gBeginAngle) {
        angle = PI - angle;
    }

    if (angle > PI + gBeginAngle) {
        angle = PI * 3 - angle;
    }

//This is the simplest algorithm
#if 0
//     if (fabs(angle - gBeginAngle) < 0.01) {
//         return 2.0;
//     } else {
//         return 1.0;
//     }
#endif

    float index = -(fabs(angle - gBeginAngle)/ PI);
    float ratio = pow(3,index);
    return ratio;
   
}
static ButtonsRotate(float angle)
{
    int i;
    for (i = 0; i < MENU_NUM; ++i) {
        gButtonList[i].angleToX += angle;
        gButtonList[i].rect.x = (int) floor(gCenterXpos + gLongRadiu * cos(gButtonList[i].angleToX));
        gButtonList[i].rect.y = (int) floor(gCenterYpos + gShortRadiu * sin(gButtonList[i].angleToX));
        float ratio = RatioFromAngle(gButtonList[i].angleToX);
        //printf("%d - %f - %f\n",i,gButtonList[i].angleToX * 180/PI, ratio);
        gButtonList[i].rect.width = (int) floor(gPicWidth * ratio);
        gButtonList[i].rect.height = (int) floor(gPicHeight * ratio);
        //printf("%d, x =%d,y=%d,angelToX = %f\n",i,gButtonList[i].rect.x,gButtonList[i].rect.y,gButtonList[i].angleToX *180/PI); 

        if (gButtonList[i].angleToX > PI * 2) {
            gButtonList[i].angleToX -= PI * 2;
        }

        if (gButtonList[i].angleToX < 0) {
            gButtonList[i].angleToX += PI * 2;
        }
        //GrMoveWindow(gButtonList[i].pButton->wid,gButtonList[i].rect.x,gButtonList[i].rect.y);

    }


}
int nTimePass = 0;
int nAnimation  = 0;
int nRight = 0;
int nCurAnimation = 2;
int bClick = 0;

static void OnTimer(TN_WIDGET * widget, void *p)
{
    static int counter = 0;
    int steps = 6;

//Pao-shu
//    printf("TimePass count = %d\n", nTimePass);
//{   
//    if(nCurAnimation != nAnimation)
//    {
//        if(nTimePass > 7)
//        {
//            //if(nAnimation == 0)
//            {    
//                if(nRight == 0)
//                {
//                    gIsRightPressed = GR_FALSE;
//                    gIsLeftPressed = GR_TRUE;
//	                gIsMoveStart = GR_TRUE;
//	                ChangeFocus(MWKEY_LEFT);
//	            }
//	            else
//	            {
//	                gIsRightPressed = GR_TRUE;
//	                gIsLeftPressed = GR_FALSE;
//	                gIsMoveStart = GR_TRUE;
//	                ChangeFocus(MWKEY_RIGHT);
//	            }
//                if(gRotateTimer == 0)
//                {
//                    gRotateTimer = GrCreateTimer(dpfDesktopWindow->wid, 50);
//                }	      
//            }
//            //else
//            //{
//            
//            
//            //}
//        
//	    }
//	    else
//	        nTimePass ++;
//	}
//	else
//	{
//	    if(!bClick)
//	    {
//	        OnClicked(NULL, NULL);
//	        bClick = 1;
//	    }
//	}
//}




#ifdef __ECOS
    enum MAIL_TYPE mail = (enum MAIL_TYPE) cyg_mbox_timed_get(dpfDesktopMboxHandle,10);
    if (mail) {
        HandleMail(mail);
    }
#endif //end of __ECOS
    if (counter == steps)
    {
        gIsMoveStart = GR_FALSE;
        gIsRightPressed = GR_FALSE;
        gIsLeftPressed = GR_FALSE;
        counter = 0;
	    GrDestroyTimer(gRotateTimer);	    
	    gRotateTimer = 0;
	    nTimePass = 0;
	    nAnimation++;                            
	    printf("Key nAnimation %d\n",nAnimation);
    }

    if (gIsMoveStart) {
        if (gIsLeftPressed) {
            float angle = -(2 * PI) / (steps * MENU_NUM);
            ButtonsRotate(angle);
        }

        if (gIsRightPressed) {
            float angle = (2 * PI) / (steps * MENU_NUM);
            ButtonsRotate(angle);
        }

        //printf("couner = %d\n", counter);
        counter++;
    }
    GrClearWindow(widget->wid, GR_TRUE);
}

static void ChangeFocus(long keyType)
{
    if (keyType == MWKEY_LEFT){
        if(gFocusIndex == 0){
            gFocusIndex = MENU_NUM -1;
        }else{
            gFocusIndex -= 1;
        }
    }
    else if(keyType == MWKEY_RIGHT){
        if(gFocusIndex == MENU_NUM -1){
            gFocusIndex = 0;
        }else{
            gFocusIndex +=1;
        }
    }
}
static void OnKeyDown(TN_WIDGET * widget, DATA_POINTER data)
{
    int type = (int) data;
    long ch;

    TN_WIDGET *pWidget;

    ch = (long) widget->data;
    if (type == WIDGET_KEYDOWN)
    {
        switch (ch)
        {
	        case MWKEY_LEFT:
	            gIsRightPressed = GR_FALSE;
	            gIsLeftPressed = GR_TRUE;
	            gIsMoveStart = GR_TRUE;
	            ChangeFocus(ch);
	            printf("key down MWKEY_LEFT\n");
	            break;
	        case MWKEY_RIGHT:
	            gIsRightPressed = GR_TRUE;
	            gIsLeftPressed = GR_FALSE;
	            gIsMoveStart = GR_TRUE;
	            ChangeFocus(ch);
	            printf("key down MWKEY_RIGHT\n");
	            break;
	        default:
	            ;               //do nothing
        }

	    if(gRotateTimer == 0)
	    {
	        gRotateTimer = GrCreateTimer(dpfDesktopWindow->wid, 50);
	    }
    }

    //printf("focus is %s\n",gMenuList[gFocusIndex]);
}


#ifdef __ECOS
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
#endif                          //end of __ECOS

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
    strcpy(format, fileName + strlen(fileName) - 4);
    format[4] = '\0';
    printf("format is %s\n", format);
    if (strcasecmp(format, ".mp3") == 0) {
        return GR_TRUE;
    } else {
        return GR_FALSE;
    }

}
static void HandleMail(enum MAIL_TYPE mail)
{
#ifdef __ECOS
    printf("desktop receive mail is %d\n", (int) mail);
    switch (mail) {
        case MAIL_RETURN:
        case MAIL_APLAYER_BACKGROUND:
	    //           GrSetFocus(gpFocusWidget->wid);
            break;
        case MAIL_APLAYER_RETURN:
            gAplayerRunning = GR_FALSE;
//            GrSetFocus(gpFocusWidget->wid);
            break;
        case MAIL_SUBJECT_GREEN:
            SubjectSwitch("Green", "en");
            if (gAplayerRunning) {
                //pao-shu
                //cyg_mbox_put(dpfAudMboxHandle, (void *) MAIL_SUBJECT_GREEN);
            }
            break;
        case MAIL_SUBJECT_BLUE:
            SubjectSwitch("Blue", "en");
            if (gAplayerRunning) {
                //pao-shu
                //cyg_mbox_put(dpfAudMboxHandle, (void *) MAIL_SUBJECT_BLUE);
            }
            break;
        case MAIL_FB_OPEN:
            printf("the opened file from fbroser is %s\n", dpfFbOpenedFileName);
            if (IsAudioFormat(dpfFbOpenedFileName)) {
                if (gAplayerRunning) {
                    //pao-shu
                    //cyg_mbox_put(dpfAudMboxHandle, (void *) MAIL_APLAYER_FOREGROUND);
                } else {
                    struct tNxThread *p_thread = GetEntry("mp3");
                    if (p_thread) {
                        cyg_thread_create(p_thread->prio,
                                          p_thread->entry,
                                          (cyg_addrword_t) dpfgNanoxDataIndex,
                                          p_thread->name, (void *) p_thread->stack, STACKSIZE, &p_thread->t,
                                          &p_thread->t_obj);
                        cyg_thread_resume(p_thread->t);
                        //gpFocusWidget = gDesktopButton[1];
                    }
                }
            }

            break;
        default:
            break;
    }
#endif                          //end of __ECOS
}
static void OnClicked(TN_WIDGET * pWidget, void *p)
{
#ifdef __ECOS
    char *cmd = gMenuList[gFocusIndex];
    printf("cmd is %s\n",cmd);
    struct tNxThread *p_thread = GetEntry(cmd);
    if (p_thread) {
        //if audioplayer is runnning in background and you click audioplayer button
        if (p_thread == &gApThreads[0] && gAplayerRunning == GR_TRUE) {
            printf("send APLAYER_FOREGROUND mail to aplayer\n");
            //pao-shu
            //cyg_mbox_put(dpfAudMboxHandle, (void *) MAIL_APLAYER_FOREGROUND);
        } else {
            //pao-shu
            printf("Open %s\n",p_thread->name);
            cyg_thread_create(p_thread->prio,
                              p_thread->entry,
                              (cyg_addrword_t) dpfgNanoxDataIndex,
                              p_thread->name, (void *) p_thread->stack, STACKSIZE, &p_thread->t, &p_thread->t_obj);
            cyg_thread_resume(p_thread->t);
//            gpFocusWidget = pWidget;
        }

        if (p_thread == &gApThreads[0]) {
            gAplayerRunning = GR_TRUE;
        }
    } else {
        printf("now we don't assign a function for %s\n", cmd);
    }
#else
    SubjectSwitch("Blue", "en");
#endif                          //end of __ECOS

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

static void GetTime(char *time_b)
{
    time_t ticks;
    struct tm *t;

    ticks = time(NULL);
    t = localtime(&ticks);
    strftime(time_b, 127, "%H:%M:%S", t);
    //dprintf("Now Time =%s\n", time_b);
}

static void DrawMenu()
{
    GR_GC_ID gc = GrNewGC();
    int i;
    for (i = 0; i < MENU_NUM; ++i) {
//         GrDrawImageFromFile(dpfDesktopWindow->WSpec.window.pid, gc, gButtonList[i].rect.x, gButtonList[i].rect.y,
//                             gButtonList[i].rect.width, gButtonList[i].rect.height, gButtonList[i].picPath, GR_FALSE);
    //printf("DrawMenu %d rect.x = %d, rect.y =%d, rect.width = %d, rect.height = %d \n",
    //       i,gButtonList[i].rect.x,
	//		 gButtonList[i].rect.y,
	//		 gButtonList[i].rect.width,
	//		 gButtonList[i].rect.height);
			 
	GrDrawImageToFit(dpfDesktopWindow->WSpec.window.pid,
			 gc,
			 gButtonList[i].rect.x,
			 gButtonList[i].rect.y,
			 gButtonList[i].rect.width,
			 gButtonList[i].rect.height,
			 gButtonList[i].imgId);
    }
}

static void OnDraw(void *p)
{
    TN_WIDGET *pWidget = (TN_WIDGET *) p;

    GR_GC_ID gc = GrNewGC();
    GetDate(gDateBuf);
    GetTime(gTimeBuf);

    GrSetGCForeground(gc, GR_RGB(118, 133, 151));
    GrFillRect(dpfDesktopWindow->WSpec.window.pid, gc, 0, 0, 800, 600);
    DrawMenu();
    
    //printf("in On Draw\n");
#if 1
    GrSetGCBackground(gc, GR_RGB(118, 133, 151));
    GrSetGCForeground(gc, GR_RGB(255,255,255));
    GrText(pWidget->WSpec.window.pid, gc, 50, 60, "Main Menu", 9, GR_TFASCII);
    GrText(pWidget->WSpec.window.pid, gc, gCenterXpos, 60, gDateBuf, strlen(gDateBuf), GR_TFASCII);
    GrText(pWidget->WSpec.window.pid, gc, gMainWinWidth - 110, 60, gTimeBuf, strlen(gTimeBuf), GR_TFASCII);
    GrLine(pWidget->WSpec.window.pid, gc, 50, 70, gMainWinWidth - 50, 70);
    
#endif
    GrDestroyGC(gc);
    
}


static int DesktopMain(int argc, char **argv)
{

    int i;
    char rcfilepath[200];
    char *subject = NULL;

    struct configFile *cfgfile;
#ifdef __ECOS
    tnControlLibInitialize();
#else
    tnAppInitialize();
#endif                          //end of __ECOS
    //select the rc file according to the subject in configfile
    printf("load the configure file %s\n", CFG_FILE_PATH);
    cfgfile = readConfigFile(CFG_FILE_PATH);
    if (!cfgfile) {
        printf("can't load the configure file %s\n", CFG_FILE_PATH);
        return;
    }
    subject = getConfigString(cfgfile, "setting", "subject");
    if (subject == NULL)
        return;

    //tnAppInitialize();
    getRcFilePath(rcfilepath, RESOURCE_PATH, subject, "Desktop");
    if (load_rc(&gDesktopRcinfo, rcfilepath, "en") < 0) {
        printf("Failed to load rc file %s!\n", rcfilepath);
        return -1;
    }
    unloadConfigFile(cfgfile);

    dpfDesktopWindow = CreateWindowFromRC(NULL, 0, IDW_WINDOW, &gDesktopRcinfo);
    InitValues();
    InitButtons();

    GrCreateTimer(dpfDesktopWindow->wid, 500);
    tnRegisterCallBack(dpfDesktopWindow, WIDGET_EXPOSURE, (CallBackFuncPtr) OnDraw, dpfDesktopWindow);
    tnRegisterCallBack(dpfDesktopWindow, WIDGET_TIMER, (CallBackFuncPtr) OnTimer, dpfDesktopWindow);
    tnRegisterCallBack(dpfDesktopWindow, WIDGET_KEYDOWN, OnKeyDown, (DATA_POINTER) WIDGET_KEYDOWN);
    tnRegisterCallBack(dpfDesktopWindow, WIDGET_CLICKED, (CallBackFuncPtr) OnClicked,NULL );

    tnMainLoop();
    return 0;
}

#ifdef __ECOS
static void InitMbox()
{
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
}
#endif                          //end of __ECOS
