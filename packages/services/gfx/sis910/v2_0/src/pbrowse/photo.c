/*****************************************************
*file name:photo.c 
*descripe:Implement of Setting Function
*author:amboo
*last changed:2008/04/11
***************************************************/
#include<stdio.h>
#include <pthread.h>
#include "assert.h"
#include "string.h"
#include "tinywidget/tnWidgets.h"
#include "../share/ecos_config.h"
#include "../share/global.h"
#include "../share/browser.h"
// include the header file produced by designer tool.
#include "photo.h"
#include "../config/config.h"


#ifdef __ECOS
//#include "global.h"
//extern cyg_handle_t  dthread_handle;//the handle of desktop thread
//extern TN_WIDGET  *desktop_window;
extern cyg_handle_t dpfDesktopMboxHandle;
#include <pkgconf/fs_fat.h>
#include <cyg/fileio/fileio.h>
#else
#include <sys/vfs.h>
#define CFG_FILE_PATH  "./resource/config.ini"
#define RESOURCE_PATH  "./resource"
#endif  //end if __ECOS


#define AP_NAME       "Photo"

#define __DEBUG
#ifdef __DEBUG
#define EPRINTF(a) printf a    
#else
#define EPRINTF(a)
#endif

static GR_BOOL gbPhotoOpen = GR_FALSE;
static RCINFO gphotoRcinfo;
static struct configFile *gpcfgFile=NULL;
static TN_WIDGET	*gpwndPhoto; 
static TN_WIDGET	*gpbtnSlideShow, *gpbtnMode, *gpbtnEffect, *gpbtnTime;
int gPlayMode  = 0, gEffectMode = 1, gInterval = 5;

static char gCurrentDir[128]="/romdisk/resource";
char grcfileDir[128];
static char *gSubject=NULL;
static char *gLang=NULL;

GR_BOOL gbShowClock = false;
static TN_WIDGET *gppicYear[4], *gppicMonth[2], *gppicTimeH[2], *gppicTimM[2], *gppicPP, *gppicCalendar;
GR_IMAGE_ID LED_NUM[10], LED_pp;
static LIST gImportantDays;

#if 0
enum EFFECT_TYPE {
    EFFECT_9,
    EFFECT_10,
    EFFECT_11,
    EFFECT_12,
    EFFECT_13,
    EFFECT_14,
    EFFECT_15,
    EFFECT_1, 
    EFFECT_2, 
    EFFECT_3,
    EFFECT_4,
    EFFECT_5,
    EFFECT_6,
    EFFECT_7,
    EFFECT_8,    
    EFFECT_TOTAL
};
#else
enum EFFECT_TYPE {
    EFFECT_1, 
    EFFECT_2, 
    EFFECT_3,
    EFFECT_4,
    EFFECT_5,
    EFFECT_6,
    EFFECT_7,
    EFFECT_8,
    EFFECT_9,
    EFFECT_10,
    EFFECT_11,
    EFFECT_12,
    EFFECT_13,
    //EFFECT_14,
    //EFFECT_15,
    EFFECT_TOTAL
};
#endif
char **gSlideShowFiles;
int gnFilesCount = 0, gnCurPic = 0;
int nSlideShowWidth = 548, nSlideShowHeight = 411;
int nSlideShowX = 0, nSlideShowY = 16;
GR_WINDOW_ID gSlideShowWindPmap, gSlideShowWindAlphaPmap;
int nCurCount = 0;
int gCurEffectCount = EFFECT_1;
int nTimerCount = 0;
int gcalendar_year, gcalendar_month, gcalendar_day, gCurDay;
extern GR_IMAGE_ID selectImage(char *str);



void SlideShowTimer(TN_WIDGET *widget, DATA_POINTER data)
{
    TN_WIDGET * mainwin;    
    GR_IMAGE_ID tempImge;
    char curFilename[128];
    GR_GC_ID gc=GrNewGC();
    int nBlockWidth, nBlockHeight;
    int i, value;
    int x1, x2, x3, x4, x5, x6;
    mainwin = widget;    
        
    if(nCurCount == 0)
    {  
        do
        {  
            GetFullFilename(gCurrentDir,gSlideShowFiles[gnCurPic],curFilename);
            
            gnCurPic = (gnCurPic == gnFilesCount - 1) ? 0 : gnCurPic + 1;
            
        }while (! ( strstr(curFilename,".BMP") || strstr(curFilename,".JPG")
                 || strstr(curFilename,".bmp") || strstr(curFilename,".jpg")));
        
        tempImge = GrLoadImageFromFile(curFilename,0);                
        GrDrawImageToFit(gSlideShowWindPmap,gc,0,0,nSlideShowWidth,nSlideShowHeight,tempImge);
        GrFreeImage(tempImge);
        
        gCurEffectCount = (gCurEffectCount == EFFECT_TOTAL - 1) ? EFFECT_1 : gCurEffectCount + 1;
    }
                
    printf(" SlideShowTimer %d, %d, Effect %d\n",nTimerCount, nCurCount, gCurEffectCount);
    //diag_mallocinfo();    
    nTimerCount++;
    
    switch(gCurEffectCount)
    {
        case EFFECT_1: // None
            if( nCurCount < 2)
                GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX, nSlideShowY, nSlideShowWidth, nSlideShowHeight, gSlideShowWindPmap, 0, 0,MWROP_SRCCOPY);
            break;
        case EFFECT_2: //Horizon Cross
            nBlockWidth = nSlideShowWidth / gInterval;
            nBlockHeight = nSlideShowHeight / 20;
            for(i=0;i<10;i++)//Horizon Cross
	        {   
	            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + nCurCount*nBlockWidth,nSlideShowY + (i*2)*nBlockHeight,
	                                                nBlockWidth + nSlideShowWidth%gInterval,nBlockHeight+10, gSlideShowWindPmap,
	                                                nCurCount*nBlockWidth, (i*2)*nBlockHeight, MWROP_SRCCOPY);
	            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX +(gInterval-nCurCount - 1)*nBlockWidth, nSlideShowY + (i*2+1)*nBlockHeight,
	                                                nBlockWidth+nSlideShowWidth%gInterval,nBlockHeight+10,gSlideShowWindPmap,
	                                                (gInterval-nCurCount - 1)*nBlockWidth,(i*2+1)*nBlockHeight, MWROP_SRCCOPY);
	        } 
            break;
        case EFFECT_3://Vertical Cross
            nBlockWidth = nSlideShowWidth / 20;
            nBlockHeight = nSlideShowHeight / gInterval;
             for(i=0;i<10;i++)
	        { 
	            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + (i*2)*nBlockWidth, nSlideShowY + (gInterval-nCurCount - 1)*nBlockHeight,
	                                                nBlockWidth,nBlockHeight+nSlideShowHeight%gInterval,gSlideShowWindPmap,
	                                                (i*2)*nBlockWidth,(gInterval-nCurCount - 1)*nBlockHeight,MWROP_SRCCOPY);
	            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + (i*2+1)*nBlockWidth, nSlideShowY + nCurCount*nBlockHeight,
	                                                nBlockWidth,nBlockHeight+nSlideShowHeight%gInterval,gSlideShowWindPmap,
	                                                (i*2+1)*nBlockWidth,nCurCount*nBlockHeight,MWROP_SRCCOPY);
	        }
            break;
        case EFFECT_4://Horizon Louver            
            nBlockHeight = nSlideShowHeight / 4;
            for(i=0;i<4;i++)
	        {
	            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX ,nSlideShowY + i*nBlockHeight,
	                                                nSlideShowWidth, (nCurCount+1)*nBlockHeight/gInterval ,gSlideShowWindPmap,
	                                                0,i*nBlockHeight,MWROP_SRCCOPY);
	        }
            break;
        case EFFECT_5://Vertical Louver
            nBlockWidth = nSlideShowWidth / 4;
            for(i=0;i<4;i++)
	        {	
	            GrCopyArea(mainwin->wid, mainwin->gc,nSlideShowX + i*nBlockWidth ,nSlideShowY, 
	                                                (nCurCount+1) * nBlockWidth/gInterval,nSlideShowHeight,gSlideShowWindPmap,
	                                                i*nBlockWidth,0,MWROP_SRCCOPY);
	        }
            break;
        case EFFECT_6:
            nBlockWidth = nSlideShowWidth / 4;
            nBlockHeight = nSlideShowHeight / 4;
            
            for(i=0;i<16;i++)
	        {	
	            GrCopyArea(mainwin->wid, mainwin->gc,nSlideShowX + (i/4)*nBlockWidth , nSlideShowY + (i%4)*nBlockHeight,
	                                                (nCurCount+1)*nBlockWidth/gInterval, (nCurCount+1)*nBlockHeight /gInterval,gSlideShowWindPmap,
	                                                (i/4)*nBlockWidth,(i%4)*nBlockHeight, MWROP_SRCCOPY);
	        }
            break;
//-----------------------------------                        
        case EFFECT_7://zhong xing fangda 
            
            //GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + (gInterval-nCurCount-1 )*nSlideShowWidth/(2*gInterval),nSlideShowY + (gInterval-nCurCount -1)*nSlideShowHeight/(2*gInterval),
            //                                    (nCurCount + 1)*nSlideShowWidth/(gInterval),(nCurCount+1)*nSlideShowHeight/(gInterval),gSlideShowWindPmap,
            //                                    (gInterval-nCurCount-1)*nSlideShowWidth/(2*gInterval),(gInterval-nCurCount -1)*nSlideShowHeight/(2*gInterval) ,MWROP_SRCCOPY);
            break;
         case EFFECT_8:
            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + (gInterval-nCurCount -1)*nSlideShowWidth/(2*gInterval), nSlideShowY,
                                                (nCurCount +1)*nSlideShowWidth/(gInterval),nSlideShowHeight,gSlideShowWindPmap,
                                                (gInterval-nCurCount -1)*nSlideShowWidth/(2*gInterval),(gInterval-nCurCount -1)*nSlideShowHeight/(2*gInterval) ,MWROP_SRCCOPY);            
                                                               
            break;
         case EFFECT_9://up ok
            
            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX,nSlideShowY + (gInterval-nCurCount-1)*(nSlideShowHeight)/gInterval,
                                                nSlideShowWidth,(nSlideShowHeight)/gInterval+nSlideShowHeight%gInterval,gSlideShowWindPmap,
                                                0,(gInterval-nCurCount-1)*(nSlideShowHeight)/gInterval,MWROP_SRCCOPY);
            break;
         case EFFECT_10://down ok
            
            GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX,nSlideShowY + nCurCount*nSlideShowHeight/gInterval,
                                                nSlideShowWidth,(nSlideShowHeight)/gInterval+nSlideShowHeight%gInterval,gSlideShowWindPmap,
                                                0,nCurCount*(nSlideShowHeight)/gInterval,MWROP_SRCCOPY);
            break;
        
        case EFFECT_11:

            value=(nCurCount*50>255)?255:(nCurCount*50);   //fade
            //printf("fade: %d\n",value);
            GrCopyArea( mainwin->wid , mainwin->gc, nSlideShowX, nSlideShowY,
                                                  nSlideShowWidth,nSlideShowHeight,gSlideShowWindPmap,
                                                  0, 0, MWROP_BLENDCONSTANT | value);
            break;
        
        case EFFECT_12: //right ok
            
            GrCopyArea(mainwin->wid, mainwin->gc,nSlideShowX + nCurCount*(nSlideShowWidth)/gInterval, nSlideShowY,
                                                nSlideShowWidth/gInterval+nSlideShowWidth%gInterval,nSlideShowHeight,gSlideShowWindPmap,
                                                nCurCount*(nSlideShowWidth)/gInterval ,0,MWROP_SRCCOPY);
            break;
        
       //case EFFECT_13: //up left
       //    x1 = nSlideShowX + (gInterval-nCurCount - 1)*(nSlideShowWidth)/gInterval;
       //    x2 = nSlideShowY + (gInterval-nCurCount - 1)*(nSlideShowHeight)/gInterval;
       //    x3 = (nCurCount + 1)*(nSlideShowWidth)/gInterval;
       //    x4 = (nSlideShowHeight)/gInterval+nSlideShowHeight%gInterval;
       //    x5 = (gInterval-nCurCount - 1)*(nSlideShowWidth)/gInterval;
       //    x6 = (gInterval-nCurCount - 1)*(nSlideShowHeight)/gInterval;
       //    printf("x1=%d, x2=%d, x3=%d, x4=%d, x5=%d, x6=%d\n", x1, x2, x3, x4, x5,x6);
       //    GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + (gInterval-nCurCount - 1)*(nSlideShowWidth)/gInterval , nSlideShowY + (gInterval-nCurCount - 1)*(nSlideShowHeight)/gInterval,
       //                                        (nCurCount + 1)*(nSlideShowWidth)/gInterval,(nSlideShowHeight)/gInterval+nSlideShowHeight%gInterval,gSlideShowWindPmap,
       //                                        (gInterval-nCurCount - 1)*(nSlideShowWidth)/gInterval ,(gInterval-nCurCount - 1)*(nSlideShowHeight)/gInterval,MWROP_SRCCOPY);
       //    printf("end\n");                                                
       //    //GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX + (gInterval-nCurCount -1)*(nSlideShowWidth)/gInterval , nSlideShowY + (gInterval-nCurCount -1)*(nSlideShowHeight)/gInterval,
       //    //                                    (nSlideShowWidth)/gInterval+nSlideShowWidth%gInterval,(nCurCount + 1)*(nSlideShowHeight)/gInterval,gSlideShowWindPmap,
       //    //                                    (gInterval-nCurCount -1)*(nSlideShowWidth)/gInterval ,(gInterval-nCurCount - 1)*(nSlideShowHeight)/gInterval,MWROP_SRCCOPY);
       //    break;
       //
       //case EFFECT_14://right down
       //    GrCopyArea(mainwin->wid, mainwin->gc,nSlideShowX + nCurCount*(nSlideShowWidth)/gInterval,nSlideShowY,
       //                                        nSlideShowWidth/gInterval+10,(nCurCount + 1)*(nSlideShowHeight)/gInterval,gSlideShowWindPmap,
       //                                        nCurCount*(nSlideShowWidth)/gInterval,0, MWROP_SRCCOPY);
       //    GrCopyArea(mainwin->wid, mainwin->gc, nSlideShowX,nSlideShowY +nCurCount*(nSlideShowHeight)/gInterval,
       //                                        (nCurCount +1)*(nSlideShowWidth)/gInterval+10, nSlideShowHeight/gInterval+10,gSlideShowWindPmap,
       //                                        0,nCurCount*(nSlideShowHeight)/gInterval,MWROP_SRCCOPY);
       //    break;
    }
    
    nCurCount = (nCurCount == gInterval - 1) ? 0 : nCurCount + 1;
        
    GrDestroyGC(gc);
}

GR_BOOL gbool2PP = GR_FALSE;
TN_WIDGET *gwMain = NULL, *gwSlideShow = NULL, *gwCalendar = NULL;
struct tm gtmCurDay;

void CalendarTimer(TN_WIDGET *widget, DATA_POINTER data)
{    
    SetTime();
    SetCalendar(gppicCalendar);
    
}

void OnSlideShowTimer(TN_WIDGET *widget, DATA_POINTER data)
{
    SlideShowTimer( gwSlideShow, data);
    
    if(gbShowClock)
        CalendarTimer( gppicCalendar, data);
    
}

void OnSlideShowEscape(TN_WIDGET *widget, DATA_POINTER data)
{
    TN_WIDGET *wnd;
    int i = 0;
    
    if(gSlideShowWindPmap)
        GrDestroyWindow(gSlideShowWindPmap);
    
    if(gSlideShowWindAlphaPmap)
        GrDestroyWindow(gSlideShowWindAlphaPmap);
        
    for( i = 0; i< 10; i++)
        GrFreeImage(LED_NUM[i]);
    
    
    wnd=GetParent(widget);
    if(wnd)
    {
        OnSubMenuEscape( wnd, data);
    }
    else
    {
        OnSubMenuEscape( widget, data);
    }
    
    gwMain = NULL;
    listFree( &gImportantDays);
    listDump(&gImportantDays);
}

void SetTime()
{
    gbool2PP = !gbool2PP;
    tnSetVisible(gppicPP, gbool2PP);   
    
    gGetCurrDay(&gtmCurDay);
    
    {
        int year1 = 0, year2 = 0, year3 = 0, year4 = 0;
        int month1 = 0, month2 = 0;
        int day1 = 0, day2 = 0;
        int hour1 = 0, hour2 = 0;
        int minute1 = 0, minute2 = 0;
                        
        gcalendar_year = 1900 + gtmCurDay.tm_year;
        gcalendar_month = gtmCurDay.tm_mon + 1;
        gcalendar_day = gtmCurDay.tm_mday;
        
        year4 = gcalendar_year %10;
        year3 = (gcalendar_year / 10) %10;
        year2 = (gcalendar_year / 100) %10;
        year1 = (gcalendar_year) / 1000;
        
        month2 = gcalendar_month%10;
        month1 = gcalendar_month / 10;
        
        day2 = gtmCurDay.tm_mday %10;
        day1 = gtmCurDay.tm_mday / 10;
        
        hour2 = gtmCurDay.tm_hour % 10;
        hour1 = gtmCurDay.tm_hour / 10;
        
        minute2 = gtmCurDay.tm_min % 10;
        minute1 = gtmCurDay.tm_min / 10;
        
        tnSetPictureID(gppicYear[0],LED_NUM[year1]);
        tnSetPictureID(gppicYear[1],LED_NUM[year2]);
        tnSetPictureID(gppicYear[2],LED_NUM[year3]);
        tnSetPictureID(gppicYear[3],LED_NUM[year4]);
        
        tnSetPictureID(gppicMonth[0],LED_NUM[month1]);
        tnSetPictureID(gppicMonth[1],LED_NUM[month2]);
        
        tnSetPictureID(gppicTimeH[0],LED_NUM[hour1]);
        tnSetPictureID(gppicTimeH[1],LED_NUM[hour2]);
        
        tnSetPictureID(gppicTimM[0],LED_NUM[minute1]);
        tnSetPictureID(gppicTimM[1],LED_NUM[minute2]);
    }
}


void SetCalendar(TN_WIDGET *widget)
{
    GR_WINDOW_INFO 	winfo;
    int offsetx = 0, offsety = 0;
    int startx = 0, starty = 0;
    int ndrawx = 0, ndrawy =0;
    int startweekday = 0, curday = 0, days = 0;
    int keybase;
    char str[8];
    int i;
    GR_COLOR temp1, temp2;
    
    GrGetWindowInfo (widget->wid, &winfo);  	
  	offsetx = winfo.width / 7;
  	offsety = winfo.height / 6;  	
  	startx = offsetx/2 - 4;
  	starty = offsety/2 - 4;
  	
  	ndrawx = startx;
  	ndrawy = offsety + starty;
    
    if(gCurDay == gcalendar_day)
        return;
    else
        gCurDay = gcalendar_day;
    
    //Clear Calendar String
    DrawPicture (widget);
    
    //Draw Previous Month
    startweekday = gGetWeekDay(gcalendar_year,gcalendar_month,1);
    printf("%s - gcalendar_day %d, gCurDay %d\n", __FUNCTION__,gcalendar_day, gCurDay);
    if(gcalendar_month == 1)
    {
        keybase = 1200;
        days = 31;
    }
    else
    {
        keybase = (gcalendar_month - 1) * 100;
        days = gGetNumDaysOfMonth(gcalendar_year,gcalendar_month - 1);
    }
    curday = days -  startweekday + 1;
    
    for(i=0; i < startweekday; i++)
    {
        if(listSearch( &gImportantDays, keybase + curday))
        {
            GrSetGCForeground(widget->gc, GR_RGB(255, 255, 255));
            GrFillRect(widget->wid, widget->gc, ndrawx - (offsetx/2) + 4, ndrawy - (offsety/2) + 5, offsetx, offsety);             
            GrSetGCForeground(widget->gc, MWRGB(1, 128, 255));
  	    }
  	    else if(ndrawx == startx) 
  	        GrSetGCForeground(widget->gc, MWRGB(229, 101, 104));  	    
  	    else
  	        GrSetGCForeground(widget->gc, MWRGB(255, 255, 255));
  	        
  	    GrSetGCBackground(widget->gc, GR_ARGB(250,255, 255, 255));
  	    
  	    sprintf(str,"%d",curday);
  	    GrText(widget->wid, widget->gc, ndrawx, ndrawy, str, -1, GR_TFASCII | GR_TFTOP);
  	    
  	    curday++;
  	    
  	    ndrawx += offsetx;
  	    if(ndrawx > winfo.width)
  	    {
  	         ndrawx = startx;
  	         ndrawy += offsety;
  	    }
    }
    
    days = gGetNumDaysOfMonth(gcalendar_year,gcalendar_month);  
    keybase = gcalendar_month * 100;
    curday = 1;
    
    for( i=startweekday; i < 36; i++)
    {
        if(listSearch( &gImportantDays, keybase + curday))
        {
            GrSetGCForeground(widget->gc, GR_RGB(255, 255, 255));
            GrFillRect(widget->wid, widget->gc, ndrawx - (offsetx/2) + 4, ndrawy - (offsety/2) + 5, offsetx, offsety);
            temp1 = MWRGB(1, 128, 255);            
  	    }
  	    else if(ndrawx == startx)
  	        temp1 = MWRGB(229, 101, 104);
  	         	    
  	    else
  	        temp1 = MWRGB(255, 255, 255);  	        
  	    
  	    if(gcalendar_day == curday)
  	    {
  	        GrSetGCForeground( widget->gc, GR_ARGB(250,255, 0, 0));  	
  	        GrRect(widget->wid, widget->gc, ndrawx - (offsetx/2) + 4, ndrawy - (offsety/2) + 5, offsetx, offsety);
  	    }
  	    
  	    GrSetGCForeground(widget->gc, temp1);
  	    GrSetGCBackground(widget->gc, GR_ARGB(250,255, 255, 255));
  	    
  	    sprintf(str,"%d",curday);
  	    GrText(widget->wid, widget->gc, ndrawx, ndrawy, str, -1, GR_TFASCII | GR_TFTOP);
  	    
  	    curday++;
  	    //If next month
        if( curday > days)
        {
            keybase = (gcalendar_month == 12) ? 100 : (gcalendar_month + 1) * 100;                    
            curday = 1;
        }
  	    
  	    ndrawx += offsetx;
  	    if(ndrawx > winfo.width)
  	    {
  	         ndrawx = startx;
  	         ndrawy += offsety;
  	    }
    }
    
}

void BrowserClicked(TN_WIDGET *widget, DATA_POINTER data)
{
    
    int *nSelects, nSelectCount = 0;
    GR_WINDOW_ID testid;
    GR_WINDOW_INFO winfo;
    char tempStr[128];
    char *clockState;

    //Check for reduplicate clicking
    if((gwMain != NULL) && GsFindWindow(gwMain->wid))
    {
        printf("double clicking BrowserClicked()");
        return;
    }
        
    tnGetAllListItems(widget, &gSlideShowFiles, &gnFilesCount);
    tnGetSelectedListPos(widget, &nSelects,&nSelectCount);
    
    if(nSelectCount > 0)
        gnCurPic = nSelects[0];
    else
        gnCurPic = 0;
    
    free(nSelects);
    
    clockState = (char *)getConfigString(gpcfgFile, "CLOCK", "ShowClock");
    
    if(!strcmp(clockState, "ON"))
        gbShowClock = true;
    else
        gbShowClock = false;
    
    
    gwMain  = CreateWindowFromRC( NULL, 0, ID_WND_SLIDEMAIN, &gphotoRcinfo);    
    tnRegisterCallBack(gwMain, WIDGET_ESCAPED, OnSlideShowEscape, (DATA_POINTER)widget);
    
    gwSlideShow  = CreateWindowFromRC( gwMain, 0, ID_WND_SLIDESHOW, &gphotoRcinfo); 
    tnRegisterCallBack(gwSlideShow, WIDGET_ESCAPED, OnSlideShowEscape, (DATA_POINTER)widget);
    
    tnRegisterCallBack(gwSlideShow, WIDGET_TIMER, OnSlideShowTimer, (DATA_POINTER)WIDGET_TIMER);
    GrCreateTimer(gwSlideShow->wid, 1000);
     
       
    if(gbShowClock)
    {
        gwCalendar  = CreateWindowFromRC( gwMain, 0, ID_WND_CALENDAR, &gphotoRcinfo);        
        gppicCalendar = CreatePictureFromRC(gwCalendar,GR_WM_PROPS_NOBACKGROUND,ID_PIC_CALENDAR,&gphotoRcinfo);
        //calendar and time initial   	
	    GetFullFilename(grcfileDir,"0.jpg",tempStr);
	    LED_NUM[0] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"1.jpg",tempStr);
	    LED_NUM[1] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"2.jpg",tempStr);
	    LED_NUM[2] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"3.jpg",tempStr);
	    LED_NUM[3] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"4.jpg",tempStr);
	    LED_NUM[4] = GrLoadImageFromFile(tempStr,0);
	    printf("BrowserClicked load num\n");
	    GetFullFilename(grcfileDir,"5.jpg",tempStr);
	    LED_NUM[5] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"6.jpg",tempStr);
	    LED_NUM[6] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"7.jpg",tempStr);
	    LED_NUM[7] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"8.jpg",tempStr);
	    printf("BrowserClicked load num\n");
	    LED_NUM[8] = GrLoadImageFromFile(tempStr,0);
	    GetFullFilename(grcfileDir,"9.jpg",tempStr);
	    LED_NUM[9] = GrLoadImageFromFile(tempStr,0);
        
	    
        gppicYear[0] = CreatePictureFromRC(gwCalendar,0,ID_PIC_Y1,&gphotoRcinfo);
        gppicYear[1] = CreatePictureFromRC(gwCalendar,0,ID_PIC_Y2,&gphotoRcinfo);
        gppicYear[2] = CreatePictureFromRC(gwCalendar,0,ID_PIC_Y3,&gphotoRcinfo);
        gppicYear[3] = CreatePictureFromRC(gwCalendar,0,ID_PIC_Y4,&gphotoRcinfo);
        
        gppicMonth[0] = CreatePictureFromRC(gwCalendar,0,ID_PIC_M1,&gphotoRcinfo);
        gppicMonth[1] = CreatePictureFromRC(gwCalendar,0,ID_PIC_M2,&gphotoRcinfo);
        
        gppicTimeH[0] = CreatePictureFromRC(gwCalendar,0,ID_PIC_H1,&gphotoRcinfo);
        gppicTimeH[1] = CreatePictureFromRC(gwCalendar,0,ID_PIC_H2,&gphotoRcinfo);
        
        gppicTimM[0] = CreatePictureFromRC(gwCalendar,0,ID_PIC_MN1,&gphotoRcinfo);
        gppicTimM[1] = CreatePictureFromRC(gwCalendar,0,ID_PIC_MN2,&gphotoRcinfo);
        
        gppicPP = CreatePictureFromRC(gwCalendar,0,ID_PIC_PP,&gphotoRcinfo);          
            
        tnRegisterCallBack(gwCalendar, WIDGET_ESCAPED, OnSlideShowEscape, (DATA_POINTER)widget);                         
        
        //Initial Clock Location
        clockState = (char *)getConfigString(gpcfgFile, "CLOCK", "ClockLoc");
    
        if(!strcmp(clockState, "Left"))
        {
            GR_WINDOW_INFO 	winfo;                
            GrGetWindowInfo (gwCalendar->wid, &winfo);
            GrMoveWindow(gwCalendar->wid, 0, 0);
            GrMoveWindow(gwSlideShow->wid, winfo.width, 0);
            SetCalendar(gppicCalendar);
            printf("SlideShow Left\n");
        }
        
        //Initial              
        listFree( &gImportantDays);
        gGetImportantDay( gpcfgFile, &gImportantDays);
        gCurDay = 0;
    }

    
    GrSetFocus(gwSlideShow->wid);

    
    //Initialize for slide show
    nCurCount = 0;
    //gCurEffectCount = EFFECT_15;
    gCurEffectCount = EFFECT_13;
    
    GrGetWindowInfo (gwSlideShow->wid, &winfo);
    if(winfo.width < 400)
    {
        nSlideShowWidth = 328;
        nSlideShowHeight = 184;
    }
    gSlideShowWindPmap = GrNewPixmap(nSlideShowWidth,nSlideShowHeight,NULL);//remember free it
    gSlideShowWindAlphaPmap = GrNewPixmap(nSlideShowWidth, nSlideShowHeight, NULL);
 
    

}

void BrowserKeydown(TN_WIDGET *widget, DATA_POINTER data)
{
    char ***filename;
    int nCount = 0, i = 0;
    tnGetSelectedListItems(widget, filename, &nCount);
    
    for(i = 0; i < nCount; i++)
        printf("BrowserKeydown File %d: %s\n", i, (*filename)[i]);
    
    FreeStrList( *filename,nCount);
}

tFileBrowser *gptFileBrowser = NULL;
void OnSlideShow(TN_WIDGET* pwidget,DATA_POINTER p)
{    
    
    char rcfilepath[128];
        
    getRcFilePath(rcfilepath, RESOURCE_PATH, gSubject, BROWSER_NAME);
    EPRINTF(("rcfilepath---:%s\n",rcfilepath));
    printf("OnSlideShow\n");
    
    if((gptFileBrowser != NULL) && GsFindWindow(gptFileBrowser->pwndBrowser->wid))
    {
        printf("double clicking OnSlideShow %d\n",gptFileBrowser->pwndBrowser->wid);
        return;
    }    
    gptFileBrowser = OpenFileBrowser(gpbtnSlideShow, rcfilepath, gCurrentDir, selectImage, BrowserClicked,BrowserKeydown, NULL);
    
    
}

void PlayModeSelect(TN_WIDGET* pwidget,DATA_POINTER p)
{
    char **strModes = NULL;
    
    if(tnGetSelectedListItems(pwidget,&strModes,&gPlayMode)<0) return;

    if (strModes!=NULL)  free(*strModes);
            
    OnSubMenuEscape(pwidget,p);
}

static TN_WIDGET *gpwndPlayMode; 

void OnPlayModeSetting(TN_WIDGET* pwidget,DATA_POINTER p)
{
    //TN_WIDGET *window;        
    TN_WIDGET *listbox;
    
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndPlayMode->wid))
    {
        return;
    }
    
    gpwndPlayMode  = CreateWindowFromRC( NULL, 0, ID_WND_MODE, &gphotoRcinfo);
    listbox = CreateListBoxFromRC( gpwndPlayMode, 0, ID_LBX_PLAYMODE, &gphotoRcinfo);
    
    tnSetSelectedListItemAt(listbox, gPlayMode, GR_TRUE);
    
    tnRegisterCallBack(listbox, WIDGET_CLICKED, PlayModeSelect, (DATA_POINTER)gpbtnMode);                                
    tnRegisterCallBack(listbox, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnMode);
    tnRegisterCallBack(gpwndPlayMode, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnMode);
    
    GrSetFocus(listbox->wid);
  
}


void EffectsSelect(TN_WIDGET* pwidget,DATA_POINTER p)
{
    char **strModes = NULL;
    
    if(tnGetSelectedListItems(pwidget,&strModes,&gEffectMode)<0) return;

    if (strModes!=NULL)  free(*strModes);
            
    OnSubMenuEscape(pwidget,p);
}

void OnEffectSetting(TN_WIDGET* pwidget,DATA_POINTER p)
{
    TN_WIDGET *window;        
    TN_WIDGET *listbox;      
    
    window  = CreateWindowFromRC( NULL, 0, ID_WND_EFFECTS, &gphotoRcinfo);
    listbox = CreateListBoxFromRC( window, 0, ID_LBX_EFFECTS, &gphotoRcinfo);
    
    tnSetSelectedListItemAt(listbox, gEffectMode, GR_TRUE);
    
    printf("OnEffectSetting\n");
    tnRegisterCallBack(listbox, WIDGET_CLICKED, EffectsSelect, (DATA_POINTER)gpbtnEffect);                                
    tnRegisterCallBack(listbox, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnEffect);
    tnRegisterCallBack(window, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnEffect);
    
    GrSetFocus(listbox->wid);        
}

void IntervalSelect(TN_WIDGET* pwidget,DATA_POINTER p)
{
    char **strModes = NULL;
    int count = 0;
    int arrayIntervals[4] = {5,10,30,60};
    
    if(tnGetSelectedListItems(pwidget,&strModes,&count) < 0) return;
        
    gInterval = arrayIntervals[count];

    if (strModes!=NULL)  free(*strModes);
            
    OnSubMenuEscape(pwidget,p);
}

void OnIntervalSetting(TN_WIDGET* pwidget,DATA_POINTER p)
{
    TN_WIDGET *window;        
    TN_WIDGET *listbox;  

    window  = CreateWindowFromRC( NULL, 0, ID_WND_TIME, &gphotoRcinfo);
    listbox = CreateListBoxFromRC( window, 0, ID_LBX_INTERVAL, &gphotoRcinfo);
    
    tnSetSelectedListItemAt(listbox, 0, GR_TRUE);
    
    tnRegisterCallBack(listbox, WIDGET_CLICKED, IntervalSelect, (DATA_POINTER)gpbtnTime);                                
    tnRegisterCallBack(listbox, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnTime);
    tnRegisterCallBack(window, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnTime);    
    
    GrSetFocus(listbox->wid);
}

#ifdef __ECOS
void Photo_thread(CYG_ADDRWORD data)
{    
    struct menuReturnData returndata;
    struct THREAD_PARAMETER *paraData = (struct THREAD_PARAMETER *) data;

    INIT_PER_THREAD_DATA(paraData->thread_data_index);
        
    returndata.wFocus = (TN_WIDGET *)paraData->others;
    returndata.prcinfo = &gphotoRcinfo;         
    
#else
void main(void *argv)
{
#endif
    char rcfilepath[200];
	
	TN_WIDGET *title;
		    
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
    
    if(load_rc(&gphotoRcinfo,rcfilepath, "en")<0)
    //if(load_rc(&gsettingRcinfo,rcfilepath, lang)<0)
    {
        EPRINTF(("Load rc file failure\n"));
        return;
    }
	//create background window
	//pao-shu
    gpwndPhoto = CreateWindowFromRC(NULL, 0, ID_WND_PHOTO,&gphotoRcinfo);
	
	title=CreateLabelFromRC(gpwndPhoto, 0, ID_LBL_PHOTO, &gphotoRcinfo);                
	
	// create buttons
	gpbtnSlideShow = CreateButtonFromRC( gpwndPhoto, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_SLIDESHOW, &gphotoRcinfo);
	gpbtnMode      = CreateButtonFromRC( gpwndPhoto, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MODE, &gphotoRcinfo);
	gpbtnEffect    = CreateButtonFromRC( gpwndPhoto, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_EFFECT, &gphotoRcinfo);    
	gpbtnTime      = CreateButtonFromRC( gpwndPhoto, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_TIME, &gphotoRcinfo);
	
	// register callback function
	tnRegisterCallBack( gpbtnSlideShow, WIDGET_CLICKED, (CallBackFuncPtr)OnSlideShow, NULL);
	tnRegisterCallBack( gpbtnMode,      WIDGET_CLICKED, (CallBackFuncPtr)OnPlayModeSetting,NULL);
	tnRegisterCallBack( gpbtnEffect,    WIDGET_CLICKED, (CallBackFuncPtr)OnEffectSetting,NULL);    
	tnRegisterCallBack( gpbtnTime,      WIDGET_CLICKED, (CallBackFuncPtr)OnIntervalSetting, NULL);

	tnRegisterCallBack( gpbtnSlideShow, WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack( gpbtnMode,      WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
    tnRegisterCallBack( gpbtnEffect,    WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack( gpbtnTime,      WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
   
	tnRegisterCallBack( gpbtnSlideShow, WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape, (DATA_POINTER) &returndata);
	tnRegisterCallBack( gpbtnMode,      WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape, (DATA_POINTER) &returndata);
	tnRegisterCallBack( gpbtnEffect,    WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape, (DATA_POINTER) &returndata);    
	tnRegisterCallBack( gpbtnTime,      WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape, (DATA_POINTER) &returndata);	
	// set focus on the first button.
    diag_mallocinfo();
    
	GrSetFocus(gpbtnMode->wid);
	tnMainLoop();
	return;
}
