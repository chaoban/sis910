/*****************************************************
*file name:Setting.c 
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
// include the header file produced by designer tool.
#include "setting.h"
#include "../config/config.h"


#ifdef __ECOS
#include "../desktop/global.h"
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


#define AP_NAME       "Setting"
#define EPRINTF(a) printf a    

static DPF_EVENT_SUBJECT gSubjectEvent;

static GR_BOOL gbSettingOpen = GR_FALSE;
static RCINFO gSettingRcinfo;
static struct configFile *gpcfgFile=NULL;
static TN_WIDGET	*gpwndSetting,*gpwndBk; 
static TN_WIDGET	*gpbtnLan,*gpbtnBri,*gpbtnCon,*gpbtnSub,*gpbtnAut,*gpbtnSta,*gpbtnRst;
static TN_WIDGET	*gpwndLan,*gpwndBri,*gpwndCon,*gpwndSub,*gpwndAut,*gpwndSta,*gpwmdRst;

/******************************************************************
*path: the path mount in the filesystem
*return:the size(MB) of free memery
*******************************************************************/
static long GetFreeSize(char* path)
{
    long size=0;
#ifndef __ECOS     
    struct statfs fsbuf;

    if (statfs(path,&fsbuf)==0)
    {
        printf("StatusSetting disk info---> total block:%d,free block:%d,block size:%d\n",
            fsbuf.f_blocks,fsbuf.f_bfree,fsbuf.f_bsize);
        size=fsbuf.f_bfree*(fsbuf.f_bsize/1024);
        printf("size:%d\n",size);
        size=size/1024;
        //sprintf(temp,"%d MB",size);
        //tnSetLabelCaption(label[4], temp);        
        return size;
    }
#else
    struct cyg_fs_disk_usage disk_usage;
    
    cyg_fs_getinfo(path,FS_INFO_DISK_USAGE,&disk_usage,sizeof(disk_usage));
    printf("disk total_block:%ld\n",disk_usage.total_blocks);
    printf("disk free block:%ld\n",disk_usage.free_blocks);
    printf("disk block size:%d\n", disk_usage.block_size);	
    size=disk_usage.free_blocks*(disk_usage.block_size/1024);
    size=size/1024;
    //sprintf(temp,"%d MB",size);
    //tnSetLabelCaption(label[4], temp);       
    return size;
    
#endif
}
/******************************************************************************
*add a string to list box , seperate by flag, example:"English,Chinese"
***************************************************************************/
static void AddStringToList(TN_WIDGET *list, const char *str, const char *flag)
{   
    char *tmp;
    char *p=NULL;
    if(str==NULL)
    {
        printf("AddStringToList:--error----\n");
        return;
    }
    tmp=(char *)malloc((strlen(str)+1)*sizeof(char));
    strcpy(tmp,str);
    p=strtok(tmp,flag);
    while(p!=NULL)
    {
        tnAddItemToListBox(list,p,0);
        p=strtok(NULL,",");
    }
    free(tmp);
}
//static void tmBtnDisable(TN_WIDGET *widget)
//{
//    widget->enabled=GR_FALSE;
//}
//static void tmBtnEnable(TN_WIDGET *widget)
//{
//    widget->enabled=GR_TRUE;
//}
/*************************************************************************
*Message box
**************************************************************************/
//void tmMessageTimer(TN_WIDGET *widget,DATA_POINTER dptr)
//{
//    tnDestroyWidget(widget);
//}
//static TN_WIDGET * CreateMessage(TN_WIDGET *widget,char *msg)
//{
//    TN_WIDGET *window;
//    TN_WIDGET *lbl;
//    
//    window=CreateWindowFromRC(widget,0,ID_WND_MESSAGE,&gSettingRcinfo);
//    lbl=CreateLabelFromRC(window, 0, ID_LBL_MES, &gSettingRcinfo);
//    tnSetLabelCaption(lbl, msg);
//    GrRaiseWindow(window->wid);
//    //tnRegisterCallBack(window, WIDGET_TIMER, tmMessageTimer, NULL);                                
//    //GrCreateTimer(window->wid,500);
//    GrSetFocus(window->wid);
//    GrMapWindow(window->wid);
//    return window;
//        
//}
/***************************************************************************
*distroy current window, set focus to the dptr
***************************************************************************/
//static void WndEscaped(TN_WIDGET *widget, DATA_POINTER dptr)
//{
//    TN_WIDGET *focus,*wnd;
//    focus=(TN_WIDGET *)dptr;
//    if (widget->type==TN_WINDOW)
//        wnd=widget;
//    else
//        wnd=GetParent(widget);
//    tnDestroyWidget(wnd);
//    if ((focus->type==TN_BUTTON) &&(!focus->enabled)) 
//        tmBtnEnable(focus);
//        
//    GrSetFocus(focus->wid);
//}
/*****************************************************************************
	button focus exchange in main window
******************************************************************************/	
//static void BtnExchange(TN_WIDGET *widget, DATA_POINTER dptr)
//{
//    TN_WIDGET *focus=widget;
//    GR_KEY ch;
//    ch=(long)widget->data;
//    switch (ch)
//    {
//        case MWKEY_UP:
//            do
//                focus=GetPrevSibling(focus);
//            while(focus->type!=TN_BUTTON);
//            GrSetFocus(focus->wid);                   
//            break;
//        case MWKEY_DOWN:
//            do
//                focus=GetNextSibling(focus);
//            while (focus->type!=TN_BUTTON);
//            GrSetFocus(focus->wid);
//            break;                      
//    }
//}
/****************************************************************************
*Language Setting
*****************************************************************************/
void LanguageSwitch(TN_WIDGET *widget, DATA_POINTER dptr)
{
    char **lang=NULL;
    char *subject;
    char rcfilepath[200];
    int count;
    //set the selected subject to cfg file	
    //msgWnd=CreateMessage(widget,"Switching...");
    if(tnGetSelectedListItems(widget,&lang,&count)<0) return;

    subject=getConfigString(gpcfgFile,"setting", "subject");
    if(subject==NULL)
    {
        printf("not exist this key:subject\n");
        return;
    } 
    
    getRcFilePath(rcfilepath, RESOURCE_PATH, subject, AP_NAME);
    EPRINTF(("LanguageSwitch:rcfilepath--%s\n",rcfilepath));
	//change language
	if(switch_lang(&gSettingRcinfo,rcfilepath,*lang)<0)
	{
        printf("change language failue\n");
        return;
	}
    else
    {
        if(setConfigString(gpcfgFile,"setting","language",*lang)==NULL)
        {
            printf("this key not existed\n");      
            return;
        }
        saveConfigFile(gpcfgFile,CFG_FILE_PATH);
        printf("change language succesfully\n");	
    }
    if (lang!=NULL)  free(*lang);
    //
    //tmMessage(widget,"Switch successful!");
    OnSubMenuEscape(widget,dptr);
}

void OnLanguageSetting(TN_WIDGET *widget,DATA_POINTER dptr)
{    
    TN_WIDGET *listbox;
    char *language_option=NULL;
    char *language=NULL;     
    
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndLan->wid))
    {
        return;
    }
    
    gpwndLan=CreateWindowFromRC(NULL,0,ID_WND_LANGUAGE,&gSettingRcinfo);
    listbox=CreateListBoxFromRC(gpwndLan,0,0,&gSettingRcinfo);
    //label=CreateLabelFromRC(window, 0, ID_LBL_LANG, &gSettingRcinfo);
    //add the language list to listbox from CFG file
    language_option=getConfigString(gpcfgFile,"setting", "language_option");

    if(language_option==NULL)
    {
        printf("not exist this key\n");
        return;
    }
    AddStringToList(listbox, language_option, ",");
    language=getConfigString(gpcfgFile,"setting", "language");
    if(language==NULL)
    {
        printf("not exist this key\n");
        return;
    }        
    tnSetSelectedListItem(listbox,language, GR_TRUE);
    tnRegisterCallBack(listbox, WIDGET_CLICKED, LanguageSwitch, (DATA_POINTER)gpbtnLan);                                
    tnRegisterCallBack(listbox, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnLan);        
    GrSetFocus(listbox->wid);
    
    return;    	
}
/**************************************************************************
*Brightness Setting, left/right ---->changed imediately
***************************************************************************/
void BrightKeydown(TN_WIDGET *widget, DATA_POINTER data)
{       
    int brightness;
    GR_KEY  ch;
    char tmp[4];
    TN_WIDGET * progress_bright=widget;
    TN_WIDGET * lbl_brig=(TN_WIDGET *)data;
    brightness=tnGetProgressBarValue(progress_bright);
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_VALDOWN:
            if(brightness>=5)
            {
                brightness-=5;
                tnProgressBarUpdate(progress_bright, brightness);
                sprintf(tmp,"%d",brightness);
                tnSetLabelCaption(lbl_brig, tmp);                                
            }
            if(setConfigInt(gpcfgFile,"setting","brightness",brightness)==NULL)
            {
                printf("this key not existed\n");      
                return;
            }
            break;
        case DPFKEY_VALUP:
            if(brightness<=95)
            {
                brightness+=5;
                tnProgressBarUpdate(progress_bright, brightness);
                sprintf(tmp,"%d",brightness);
                tnSetLabelCaption(lbl_brig, tmp);                                      
            }
            if(setConfigInt(gpcfgFile,"setting","brightness",brightness)==NULL)
            {
            	printf("this key not existed\n");      
            	return;
            }
            break;
            
        default:
        	break;
    }
    //saveConfigFile(gpcfgFile,CFG_FILE_PATH);
}
void OnBrightSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *progress_bright;
    TN_WIDGET *button;
    TN_WIDGET *label[2],*lbl_brig;	
    char tmp[4];
    int brightness;
        
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndBri->wid))
    {
        return;
    }
    
    gpwndBri=CreateWindowFromRC(NULL,0,ID_WND_BRIGHT,&gSettingRcinfo);
    label[0]=CreateLabelFromRC(gpwndBri, 0, ID_LBL_BR0, &gSettingRcinfo);    
    label[1]=CreateLabelFromRC(gpwndBri, 0, ID_LBL_BR100, &gSettingRcinfo);    
    lbl_brig=CreateLabelFromRC(gpwndBri, 0, ID_LBL_BRIGVALUE, &gSettingRcinfo);            
    button = CreateButtonFromRC(gpwndBri, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_BRIGHTNESS,&gSettingRcinfo);
    progress_bright=CreateProgressBarFromRC(gpwndBri, 0, ID_PGB_BRIGHT,&gSettingRcinfo);
    //setConfigInt(gpcfgFile,"setting","brightness",45);
    brightness=getConfigInt(gpcfgFile, "setting", "brightness");
    tnProgressBarUpdate(progress_bright, brightness);
    sprintf(tmp,"%d",brightness);
    tnSetLabelCaption(lbl_brig, tmp);
    tnRegisterCallBack(progress_bright, WIDGET_KEYDOWN, BrightKeydown, (DATA_POINTER)lbl_brig);
    tnRegisterCallBack(progress_bright, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnBri);
    tnRegisterCallBack(progress_bright, WIDGET_CLICKED, OnSubMenuEscape, (DATA_POINTER)gpbtnBri);
    GrSetFocus(progress_bright->wid);
    return;
}
/**************************************************************************
*Contrast Setting, left/right ---->changed imediately
***************************************************************************/
void ContrastKeydown(TN_WIDGET *widget, DATA_POINTER data)
{       
    int contrast;
    GR_KEY  ch;
    char tmp[4];
    TN_WIDGET * progress_con=widget;
    TN_WIDGET * lbl_con=(TN_WIDGET *)data;
    contrast=tnGetProgressBarValue(progress_con);
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_VALDOWN:
            if(contrast>=5)
            {
                contrast-=5;
                tnProgressBarUpdate(progress_con, contrast);
                sprintf(tmp,"%d",contrast);
                tnSetLabelCaption(lbl_con, tmp);                                
            }
            if(setConfigInt(gpcfgFile,"setting","contrast",contrast)==NULL)
            {
                printf("this key not existed\n");      
                return;
            }
        	break;
        case DPFKEY_VALUP:
            if(contrast<=95)
            {
                contrast+=5;
                tnProgressBarUpdate(progress_con, contrast);
                sprintf(tmp,"%d",contrast);
                tnSetLabelCaption(lbl_con, tmp);                                      
            }
            if(setConfigInt(gpcfgFile,"setting","contrast",contrast)==NULL)
            {
                printf("this key not existed\n");      
                return;
            }
            break;
            
        default:
        	break;
    }
    // saveConfigFile(gpcfgFile,CFG_FILE_PATH);
}
void OnContrastSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *progress_cont;
    TN_WIDGET *button;
    TN_WIDGET *label[2],*lbl_cont;	
    char tmp[4];
    int contrast;
        
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndCon->wid))
    {
        return;
    }
    
    gpwndCon=CreateWindowFromRC(NULL,0,ID_WND_CONT,&gSettingRcinfo);
    label[0]=CreateLabelFromRC(gpwndCon, 0, ID_LBL_CON0, &gSettingRcinfo);    
    label[1]=CreateLabelFromRC(gpwndCon, 0, ID_LBL_CON100, &gSettingRcinfo);    
    lbl_cont=CreateLabelFromRC(gpwndCon, 0, ID_LBL_CONTVALUE, &gSettingRcinfo);            
    button = CreateButtonFromRC(gpwndCon, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CONFLAG,&gSettingRcinfo);
    progress_cont=CreateProgressBarFromRC(gpwndCon, 0, ID_PGB_CONT,&gSettingRcinfo);
    //setConfigInt(gpcfgFile,"setting","brightness",45);
    contrast=getConfigInt(gpcfgFile, "setting", "contrast");
    tnProgressBarUpdate(progress_cont, contrast);
    sprintf(tmp,"%d",contrast);
    tnSetLabelCaption(lbl_cont, tmp);
    tnRegisterCallBack(progress_cont, WIDGET_KEYDOWN, ContrastKeydown, (DATA_POINTER)lbl_cont);
    tnRegisterCallBack(progress_cont, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnCon);
    tnRegisterCallBack(progress_cont, WIDGET_CLICKED, OnSubMenuEscape, (DATA_POINTER)gpbtnCon);
    GrSetFocus(progress_cont->wid);
    return;
}
/****************************************************************************
*subject setting
****************************************************************************/
void SubjectSwitch(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *msgWnd;
    char **subject11=NULL;
    char *lang=NULL;
    char rcfilepath[200];
    int count;
    GR_EVENT event;
    //set the selected subject to cfg file	
    //msgWnd=CreateMessage(widget,"Switching...");
    //initialize
    if(gSubjectEvent.subjecttitle)
        free(gSubjectEvent.subjecttitle);        
    
    if(tnGetSelectedListItems(widget,&subject11,&count) <0 ) return;
    
    if(count <= 0) return;
    
    gSubjectEvent.type = DPF_SUBJECT_UPDATE;
    gSubjectEvent.subjecttitle = strdup(subject11[0]);
    FreeStrList(subject11, count);
    
    getRcFilePath(rcfilepath, RESOURCE_PATH, gSubjectEvent.subjecttitle, AP_NAME);
    EPRINTF(("subject_clicked:rcfilepath--%s\n",rcfilepath));
    lang=getConfigString(gpcfgFile,"setting", "language");
    if(lang==NULL)
    {
        printf("not exist this key\n");
        return;
    }  
    //change subject
    if(switch_subject(&gSettingRcinfo,rcfilepath,"en")<0)
    {
        printf("change subject failue\n");
        return;
    }
    else
    {
        //if(setConfigString(gpcfgFile,"setting","subject",*subject11)==NULL)
        //{
        //    printf("this key not existed\n");      
        //    return;
        //}
        //saveConfigFile(gpcfgFile,CFG_FILE_PATH);
        printf("change subject succesfully\n");	
    }
    
#ifdef __ECOS
    cyg_mbox_put( dpfDesktopMboxHandle, (DPF_EVENT *)&gSubjectEvent);
//    if(strcmp(*subject11,"Green") == 0)
//    {
//        printf("send subject:Green is %d\n",MAIL_SUBJECT_GREEN);
//        //pao-shu
//        cyg_mbox_put( dpfDesktopMboxHandle, (void *)MAIL_SUBJECT_GREEN);
//    }
//    else if(strcmp(*subject11,"Blue") == 0)
//    {
//        printf("send subject:Blue is %d\n",MAIL_SUBJECT_BLUE);
//        //pao-shu
//        cyg_mbox_put( dpfDesktopMboxHandle, (void *)MAIL_SUBJECT_BLUE);
//    }
#endif
    //

   // OnSubMenuEscape(widget,dptr);
    
    event.type = GR_EVENT_TYPE_KEY_DOWN;
    event.keystroke.ch = MWKEY_ESCAPE;  
    //ProcessEvent(&event, pwidget);    
    ProcessEvent(&event, widget);


}
void OnSubjectSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *listbox=NULL;
    //TN_WIDGET *label;	
    char *subject=NULL;
    char *subject_option=NULL;
               
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndSub->wid))
    {
        return;
    }
    
    gpwndSub=CreateWindowFromRC(NULL,0,ID_WND_SUBJECT,&gSettingRcinfo);
    //label=CreateLabelFromRC(window, 0, ID_LBL_SUBJ, &gSettingRcinfo);        
    listbox=CreateListBoxFromRC(gpwndSub,0,ID_LBX_SUBJECT,&gSettingRcinfo);
    //add the language list to listbox from CFG file
    subject_option=getConfigString(gpcfgFile,"setting", "subject_option");
    if(subject_option==NULL)
    {
        printf("not exist this key:subject_option\n");
        return;
    }
    AddStringToList(listbox, subject_option, ",");
    //show the current selected
    subject=getConfigString(gpcfgFile,"setting", "subject");
    if(subject==NULL)
    {
        printf("not exist this key:subject\n");
        return;
    }        
    tnSetSelectedListItem(listbox,subject, GR_TRUE);
    tnRegisterCallBack(listbox, WIDGET_CLICKED, SubjectSwitch, (DATA_POINTER)gpbtnSub);                        
    tnRegisterCallBack(listbox, WIDGET_ESCAPED, OnSubMenuEscape, (DATA_POINTER)gpbtnSub);                
    GrSetFocus(listbox->wid);
    
}
/****************************************************************************
*Auto On/Off Setting
*****************************************************************************/
static TN_WIDGET * gpbtnHH,*gpbtnMM;
static TN_WIDGET *gpbtnOn,*gpbtnOff;
//static int findchar(char * p, char c)
//{
//    while (p)
//    {
//        if (*p==c)  return 1;
//        p++;
//    }
//    return 0;
//}

void AutoSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_KEY ch;
    int nPlus = 0, nCurrent;
    char str[4];
    ch=(long)widget->data;
    
    switch(ch)
    {
        case DPFKEY_PREV:
        case DPFKEY_NEXT:            
            OnControlFocusExchange( widget, dptr);
            return;
        case DPFKEY_VALUP:
            nPlus = 1;
            break;
        case DPFKEY_VALDOWN:
            nPlus = -1;
            break;        
    }
    
    int n = (int)dptr;
    
    tnGetButtonCaption(widget,str);    
                   
    switch(n)
    {            
        case 0://Hour            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 0, 23);
            break;
            
        case 1://Minute            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 0, 59);
            break;
                        
        default:
            return;
    }
    
    tnSetButtonCaption(widget,str);
}

//void AutoSettimeHH(TN_WIDGET *widget, DATA_POINTER dptr)
//{
//    GR_KEY  ch;
//    TN_WIDGET *focus=(TN_WIDGET *)dptr; //another btn
//    TN_WIDGET *window=GetParent(widget);
//    char tmp[3];
//    int hh;
//    tnGetButtonCaption(widget, tmp);
//    hh=atoi(tmp);
//    ch=(long)widget->data;       
//    switch(ch)
//    {
//        case MWKEY_LEFT:
//        case MWKEY_RIGHT:
//            GrSetFocus(focus->wid);
//            break;
//        case MWKEY_UP:
//            hh++;
//            if (hh>23) hh=0;
//            sprintf(tmp,"%d",hh);
//            tnSetButtonCaption(widget, tmp);
//            break;
//        case MWKEY_DOWN:
//            hh--;
//            if (hh<0) hh=23;
//            sprintf(tmp,"%d",hh);
//            tnSetButtonCaption(widget, tmp);                       
//            break;
//    }
//}
//void AutoSettimeMM(TN_WIDGET *widget, DATA_POINTER dptr)
//{
//    GR_KEY  ch;
//    TN_WIDGET *focus=(TN_WIDGET *)dptr; //another btn
//    TN_WIDGET *window=GetParent(widget);
//    char tmp[3];
//    int mm;        
//    tnGetButtonCaption(widget, tmp);
//    mm=atoi(tmp);        
//    ch=(long)widget->data;       
//    switch(ch)
//    {
//        case MWKEY_LEFT:
//        case MWKEY_RIGHT:
//            GrSetFocus(focus->wid);
//            break;
//        case MWKEY_UP:
//            mm++;
//            if (mm>59) mm=0;
//            sprintf(tmp,"%d",mm);
//            tnSetButtonCaption(widget, tmp);
//            break;
//        case MWKEY_DOWN:
//            mm--;
//            if (mm<0) mm=59;
//            sprintf(tmp,"%d",mm);
//            tnSetButtonCaption(widget, tmp);                       
//            break;
//    }
//}
void AutoUpdateTime(TN_WIDGET *widget, DATA_POINTER *dptr)
{
    char time[10];
    char tmp[3];
    TN_WIDGET * focus=(TN_WIDGET *)dptr;
    tnGetButtonCaption(gpbtnHH,tmp);
    strcpy(time,tmp);
    strcat(time,":");
    tnGetButtonCaption(gpbtnMM,tmp);
    strcat(time,tmp);
    tnSetButtonCaption(focus, time);
    OnSubMenuEscape(widget, dptr);
       
}
void AutoSettime(TN_WIDGET * widget, TN_WIDGET *button)
{
    TN_WIDGET * window;
    TN_WIDGET *lbl[2];
    window=CreateWindowFromRC(widget,0, ID_WND_SETTIME, &gSettingRcinfo);
    lbl[0]=CreateLabelFromRC(window, 0, ID_LBL_SETTIME, &gSettingRcinfo);
    lbl[1]=CreateLabelFromRC(window, 0, ID_LBL_TIME_TS, &gSettingRcinfo);
    gpbtnHH=CreateButtonFromRC(window, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_HH, &gSettingRcinfo);
    gpbtnMM=CreateButtonFromRC(window, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MM, &gSettingRcinfo);
    tnRegisterCallBack(gpbtnHH, WIDGET_ESCAPED, OnSubMenuEscape,(DATA_POINTER)button);          
    tnRegisterCallBack(gpbtnMM, WIDGET_ESCAPED, OnSubMenuEscape,(DATA_POINTER)button);    
    tnRegisterCallBack(gpbtnHH, WIDGET_KEYDOWN, (CallBackFuncPtr)AutoSetting,(DATA_POINTER )0);        
    tnRegisterCallBack(gpbtnMM, WIDGET_KEYDOWN, (CallBackFuncPtr)AutoSetting,(DATA_POINTER )1); 
    tnRegisterCallBack(gpbtnHH, WIDGET_CLICKED, (CallBackFuncPtr)AutoUpdateTime,(DATA_POINTER )button);        
    tnRegisterCallBack(gpbtnMM, WIDGET_CLICKED, (CallBackFuncPtr)AutoUpdateTime,(DATA_POINTER )button);             
           
    GrSetFocus(gpbtnHH->wid);
    
}
void AutoClick(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_KEY  ch;
    char time[10];
    TN_WIDGET *focus=(TN_WIDGET *)dptr;
    TN_WIDGET *window=GetParent(widget);
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_PREV:
        case DPFKEY_NEXT:
            GrSetFocus(focus->wid);                              
            break;
        case DPFKEY_VALUP:
        case DPFKEY_VALDOWN:
            tnGetButtonCaption(widget, time);
            if (strcmp(time,"disable")!=0)
            {      
                EPRINTF(("set to disable\n"));
                tnSetButtonCaption(widget,"disable");
            }
            else
            {
                EPRINTF(("set time\n"));                                    
                AutoSettime(window,widget);    
            }
            break;
    }
}
void AutoEscape(TN_WIDGET *widget, DATA_POINTER dptr)
{
    char tmp[10];
    tnGetButtonCaption(gpbtnOn, tmp);
    setConfigString(gpcfgFile, "setting", "autoon", tmp);
    tnGetButtonCaption(gpbtnOff, tmp);
    setConfigString(gpcfgFile, "setting", "autooff", tmp);
    OnSubMenuEscape(widget, dptr);
}
void OnAutoSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    char *autoon=NULL;
    char *autooff=NULL;
    TN_WIDGET *label[3];	
        
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndAut->wid))
    {
        return;
    }
    
    gpwndAut=CreateWindowFromRC(NULL,0,ID_WND_AUTO,&gSettingRcinfo);
    //label[0]=CreateLabelFromRC(window, 0, ID_LBL_AUTO, &gSettingRcinfo);    
    label[1]=CreateLabelFromRC(gpwndAut, 0, ID_LBL_AUTO_1, &gSettingRcinfo);            
    label[2]=CreateLabelFromRC(gpwndAut, 0, ID_LBL_AUTO_2, &gSettingRcinfo);                    
    gpbtnOn = CreateButtonFromRC(gpwndAut, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_AUTO_ON,&gSettingRcinfo);
    gpbtnOff = CreateButtonFromRC(gpwndAut, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_AUTO_OFF,&gSettingRcinfo);    
    autoon=getConfigString(gpcfgFile,"setting", "autoon");
    if(autoon==NULL)
    {
        printf("not exist this key:autoon\n");
        return;
    }
    autooff=getConfigString(gpcfgFile,"setting", "autooff");
    if(autoon==NULL)
    {
        printf("not exist this key:autooff\n");
        return;
    }   
    tnSetButtonCaption(gpbtnOn, autoon);
    tnSetButtonCaption(gpbtnOff, autooff);
    tnRegisterCallBack(gpbtnOn, WIDGET_KEYDOWN, (CallBackFuncPtr)AutoClick,(DATA_POINTER )gpbtnOff);        
    tnRegisterCallBack(gpbtnOff, WIDGET_KEYDOWN, (CallBackFuncPtr)AutoClick,(DATA_POINTER )gpbtnOn);         
    tnRegisterCallBack(gpbtnOn, WIDGET_CLICKED, AutoEscape, (DATA_POINTER)gpbtnAut);    
    tnRegisterCallBack(gpbtnOff, WIDGET_CLICKED, AutoEscape, (DATA_POINTER)gpbtnAut);        
    tnRegisterCallBack(gpbtnOn, WIDGET_ESCAPED, AutoEscape, (DATA_POINTER)gpbtnAut);    
    tnRegisterCallBack(gpbtnOff, WIDGET_ESCAPED, AutoEscape, (DATA_POINTER)gpbtnAut);        
    GrSetFocus(gpbtnOn->wid);
    return;
    
}
/************************************************************************
*Status Setting
*************************************************************************/
void OnStatusSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *label[4];
    char * vision=NULL;
    long size;
    char temp[10];
    
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndSta->wid))
    {
        return;
    }
    
    gpwndSta=CreateWindowFromRC(NULL,0,ID_WND_STATUS,&gSettingRcinfo);
    //label[0]=CreateLabelFromRC(window, 0, ID_LBL_STAT, &gSettingRcinfo);     
    label[0]=CreateLabelFromRC(gpwndSta, 0, ID_LBL_FMVISION, &gSettingRcinfo);            
    label[1]=CreateLabelFromRC(gpwndSta, 0, ID_LBL_FREEMEM, &gSettingRcinfo);            
    label[2]=CreateLabelFromRC(gpwndSta, 0, ID_LBL_ST_SV, &gSettingRcinfo);            
    label[3]=CreateLabelFromRC(gpwndSta, 0, ID_LBL_ST_FM, &gSettingRcinfo); 
    //get software vision information
    vision=getConfigString(gpcfgFile,"setting", "vision");
    if(vision!=NULL)
    {
        tnSetLabelCaption(label[2], vision);
    }        
    //get the disk information
    size=GetFreeSize("/");
    if (size<1024)
        sprintf(temp,"%d MB",size);
    else
        sprintf(temp,"%8.2f GB",size/1024.0);                
    tnSetLabelCaption(label[3], temp);       
    
    tnRegisterCallBack(gpwndSta, WIDGET_ESCAPED, OnSubMenuEscape,(DATA_POINTER)gpbtnSta);  
    GrSetFocus(gpwndSta->wid);
    return;

}
/*************************************************************************
*Reset Setting
**************************************************************************/
void ResetClicked(TN_WIDGET *widget, DATA_POINTER dptr)
{
    char * def_subj=NULL, *def_lang=NULL;
    int def_brig,def_cont;
    char rcfilepath[50];
    def_subj=getConfigString(gpcfgFile, "DEFAULT", "subject");
    if (def_subj==NULL) 
    {
        EPRINTF(("not exist this key:DEFAULT->subject\n"));
        return;
    }
    def_lang=getConfigString(gpcfgFile, "DEFAULT", "language");
    if (def_lang==NULL)
    {
        EPRINTF(("not exist this key:DEFAULT->subject\n"));
        return;
    }
    getRcFilePath(rcfilepath, RESOURCE_PATH, def_subj, AP_NAME);
    //switch subject
	if(switch_subject(&gSettingRcinfo,rcfilepath,def_lang)<0)
	{
	    EPRINTF(("change subject failue\n"));
	    return;
	}
    else
    {
        if(setConfigString(gpcfgFile,"setting","subject",def_subj)==NULL)
        {
	        printf("this key not existed\n");      
	        return;
	    }
    }
    //switch language
    if(switch_lang(&gSettingRcinfo,rcfilepath,def_lang)<0)
    {
	    EPRINTF(("change language failue\n"));
	    return;
	}
    else
    {
        if(setConfigString(gpcfgFile,"setting","language",def_lang)==NULL)
        {
	        printf("this key not existed\n");      
	        return;
	    }
    }
    //bightness
    def_brig=getConfigInt(gpcfgFile, "DEFAULT", "brightness");
    if(setConfigInt(gpcfgFile,"setting","brightness",def_brig)==NULL)
    {
        printf("this key not existed\n");      
        return;
    }
    //contrast
    def_cont=getConfigInt(gpcfgFile,"DEFAULT","contrast");
    if(setConfigInt(gpcfgFile,"setting","contrast",def_cont)==NULL)
    { 
        printf("this key not existed\n");      
        return;
    }       
    //save config file
    //saveConfigFile(gpcfgFile,CFG_FILE_PATH);
    OnSubMenuEscape(widget,dptr);
}
void OnResetSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *label;	
        
    //Check for reduplicate clicking
    if(GsFindWindow(gpwmdRst->wid))
    {
        return;
    }
        
    gpwmdRst=CreateWindowFromRC(NULL,0,ID_WND_RESET,&gSettingRcinfo);
    //label[0]=CreateLabelFromRC(window, 0, ID_LBL_REST, &gSettingRcinfo);                
    label = CreateLabelFromRC(gpwmdRst, 0, ID_LBL_REST_MES, &gSettingRcinfo);                        
    tnRegisterCallBack(gpwmdRst, WIDGET_ESCAPED, OnSubMenuEscape,(DATA_POINTER)gpbtnRst);
    tnRegisterCallBack(gpwmdRst, WIDGET_CLICKED, ResetClicked,(DATA_POINTER)gpbtnRst);        
    GrSetFocus(gpwmdRst->wid);
    return;

}
/**************************************************************************
*
***************************************************************************/
void ExitSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
   
	printf("ExitSetting \n");
    saveConfigFile(gpcfgFile,CFG_FILE_PATH);
    unloadConfigFile(gpcfgFile);
#ifdef __ECOS
    printf("send MAIL_RETURN is %d\n",MAIL_RETURN);
    //pao-shu
    cyg_mbox_put(dpfDesktopMboxHandle, (void *)MAIL_RETURN);
#endif

    OnMenuEscape(widget,dptr);
	
#ifdef _pao_shu	
    tnDestroyWidget(gpwndBk);
#endif

}
/**************************************************************************
*Setting main
****************************************************************************/
//void OpenSetting(CYG_ADDRWORD data)
//{       
//    char rcfilepath[200];
//	char *subject=NULL;
//    char *lang=NULL;
//	TN_WIDGET *title[2];
//	struct menuReturnData *returndata;
//	
//    returndata = (struct menuReturnData *) data;
//	
//	//loadconfigfile
//    gpcfgFile=readConfigFile(CFG_FILE_PATH);
//    if(!gpcfgFile)
//    {
//        printf("can't load the configure file\n");
//        return;
//    }
//	subject=getConfigString(gpcfgFile,"setting","subject");
//	if (subject==NULL) return;
//    
//    lang=getConfigString(gpcfgFile,"setting","language");
//	
//	if (lang==NULL) return;
//#ifdef __ECOS	    
//	//tnControlLibInitialize();
//	tnAppInitialize();
//#else	
//	tnAppInitialize();
//#endif
//    	//select the rc file according to the subject in configfile
//	getRcFilePath(rcfilepath, RESOURCE_PATH, subject, AP_NAME);
//    EPRINTF(("rcfilepath---:%s\n",rcfilepath));
//    
//    if(load_rc(&gSettingRcinfo,rcfilepath, "en")<0)
//    //if(load_rc(&gSettingRcinfo,rcfilepath, lang)<0)
//    {
//        EPRINTF(("Load rc file failure\n"));
//        return;
//    }
//	//create background window
//	//pao-shu
//#ifdef _pao_shu	
//	printf("OpenSetting gpwndBk\n");
//	gpwndBk = CreateWindowFromRC(NULL, 0, ID_WND_SETTING_BK,&gSettingRcinfo);
//	title[0]=CreateLabelFromRC(gpwndBk, 0, ID_LBL_TITLE_1, &gSettingRcinfo);                
//    
//    printf("OpenSetting gpwndSetting\n");
//	// create main window.
//	gpwndSetting = CreateWindowFromRC(gpwndBk, 0, ID_WND_SETTING,&gSettingRcinfo);
//#else
//    gpwndSetting = CreateWindowFromRC(NULL, 0, ID_WND_SETTING,&gSettingRcinfo);
//#endif	
//	title[1]=CreateLabelFromRC(gpwndSetting, 0, ID_LBL_TITLE_2, &gSettingRcinfo);                
//	
//	printf("OpenSetting gpbtnLan\n");
//	// create buttons
//	gpbtnLan= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_LANGUAGE,&gSettingRcinfo);
//	gpbtnBri= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_BRIGHT,&gSettingRcinfo);
//	gpbtncon= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CONTRAST,&gSettingRcinfo);    
//	gpbtnSub= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_SUBJECT,&gSettingRcinfo);
//	gpbtnAut= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_AUTO,&gSettingRcinfo);
//	gpbtnSta= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_STATUS,&gSettingRcinfo);
//    gpbtnRst= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_RESET,&gSettingRcinfo);
//	// register callback function
//	tnRegisterCallBack(gpbtnLan, WIDGET_CLICKED, (CallBackFuncPtr)LanguageSetting, NULL);
//	tnRegisterCallBack(gpbtnBri, WIDGET_CLICKED, (CallBackFuncPtr)BrightSetting,NULL);
//	tnRegisterCallBack(gpbtncon, WIDGET_CLICKED, (CallBackFuncPtr)ContrastSetting,NULL);    
//	tnRegisterCallBack(gpbtnSub, WIDGET_CLICKED, (CallBackFuncPtr)SubjectSetting, NULL);
//	tnRegisterCallBack(gpbtnAut, WIDGET_CLICKED, (CallBackFuncPtr)AutoSetting, NULL);
//	tnRegisterCallBack(gpbtnSta, WIDGET_CLICKED, (CallBackFuncPtr)StatusSetting, NULL);
//	tnRegisterCallBack(gpbtnRst, WIDGET_CLICKED, (CallBackFuncPtr)ResetSetting, NULL);
//
//	tnRegisterCallBack(gpbtnLan, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//	tnRegisterCallBack(gpbtnBri, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//    tnRegisterCallBack(gpbtncon, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//	tnRegisterCallBack(gpbtnSub, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//	tnRegisterCallBack(gpbtnAut, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//	tnRegisterCallBack(gpbtnSta, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//	tnRegisterCallBack(gpbtnRst, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
//
//    returndata->prcinfo = &gSettingRcinfo;
//    
//	tnRegisterCallBack(gpbtnLan, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);
//	tnRegisterCallBack(gpbtnBri, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);
//	tnRegisterCallBack(gpbtncon, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);    
//	tnRegisterCallBack(gpbtnSub, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);
//	tnRegisterCallBack(gpbtnAut, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);
//	tnRegisterCallBack(gpbtnSta, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);
//	tnRegisterCallBack(gpbtnRst, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER)returndata);
//	// set focus on the first button.
//
//	GrSetFocus(gpbtnLan->wid);
//	tnMainLoop();
//	return;
//}
/***********************************************************************
*
*************************************************************************/

#ifdef __ECOS
void Setting_thread(CYG_ADDRWORD data)
{    
    struct menuReturnData returndata;
    struct THREAD_PARAMETER *paraData = (struct THREAD_PARAMETER *) data;

    INIT_PER_THREAD_DATA(paraData->thread_data_index);
         
    returndata.wFocus = (TN_WIDGET *)paraData->others;
    returndata.prcinfo = &gSettingRcinfo;    
    
#else
void main(void *argv)
{
#endif
    char rcfilepath[200];
	char *subject=NULL;
    char *lang=NULL;
	TN_WIDGET *title[2];
		    
	//loadconfigfile
    gpcfgFile=readConfigFile(CFG_FILE_PATH);
    if(!gpcfgFile)
    {
        printf("can't load the configure file\n");
        return;
    }
	subject=getConfigString(gpcfgFile,"setting","subject");
	if (subject==NULL) return;
    
    lang=getConfigString(gpcfgFile,"setting","language");
	
	if (lang==NULL) return;
#ifdef __ECOS	    
	//tnControlLibInitialize();
	tnAppInitialize();
#else	
	tnAppInitialize();
#endif
    	//select the rc file according to the subject in configfile
	getRcFilePath(rcfilepath, RESOURCE_PATH, subject, AP_NAME);
    EPRINTF(("rcfilepath---:%s\n",rcfilepath));
    
    if(load_rc(&gSettingRcinfo,rcfilepath, "en")<0)
    //if(load_rc(&gSettingRcinfo,rcfilepath, lang)<0)
    {
        EPRINTF(("Load rc file failure\n"));
        return;
    }
	//create background window
	//pao-shu
#ifdef _pao_shu	
	printf("OpenSetting gpwndBk\n");
	gpwndBk = CreateWindowFromRC(NULL, 0, ID_WND_SETTING_BK,&gSettingRcinfo);
	//title[0]=CreateLabelFromRC(gpwndBk, 0, ID_LBL_TITLE_1, &gSettingRcinfo);                
    
    printf("OpenSetting gpwndSetting\n");
	// create main window.
	gpwndSetting = CreateWindowFromRC(gpwndBk, 0, ID_WND_SETTING,&gSettingRcinfo);
#else
    gpwndSetting = CreateWindowFromRC(NULL, 0, ID_WND_SETTING,&gSettingRcinfo);
#endif	
	title[1]=CreateLabelFromRC(gpwndSetting, 0, ID_LBL_TITLE_2, &gSettingRcinfo);                
		
	// create buttons
	gpbtnLan= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_LANGUAGE,&gSettingRcinfo);
	gpbtnBri= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_BRIGHT,&gSettingRcinfo);
	gpbtnCon= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CONTRAST,&gSettingRcinfo);    
	gpbtnSub= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_SUBJECT,&gSettingRcinfo);
	gpbtnAut= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_AUTO,&gSettingRcinfo);
	gpbtnSta= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_STATUS,&gSettingRcinfo);
    gpbtnRst= CreateButtonFromRC(gpwndSetting, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_RESET,&gSettingRcinfo);
	// register callback function
	tnRegisterCallBack(gpbtnLan, WIDGET_CLICKED, (CallBackFuncPtr)OnLanguageSetting, NULL);
	tnRegisterCallBack(gpbtnBri, WIDGET_CLICKED, (CallBackFuncPtr)OnBrightSetting,NULL);
	tnRegisterCallBack(gpbtnCon, WIDGET_CLICKED, (CallBackFuncPtr)OnContrastSetting,NULL);    
	tnRegisterCallBack(gpbtnSub, WIDGET_CLICKED, (CallBackFuncPtr)OnSubjectSetting, NULL);
	tnRegisterCallBack(gpbtnAut, WIDGET_CLICKED, (CallBackFuncPtr)OnAutoSetting, NULL);
	tnRegisterCallBack(gpbtnSta, WIDGET_CLICKED, (CallBackFuncPtr)OnStatusSetting, NULL);
	tnRegisterCallBack(gpbtnRst, WIDGET_CLICKED, (CallBackFuncPtr)OnResetSetting, NULL);

	tnRegisterCallBack(gpbtnLan, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
	tnRegisterCallBack(gpbtnBri, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
    tnRegisterCallBack(gpbtnCon, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
	tnRegisterCallBack(gpbtnSub, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
	tnRegisterCallBack(gpbtnAut, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
	tnRegisterCallBack(gpbtnSta, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
	tnRegisterCallBack(gpbtnRst, WIDGET_KEYDOWN, OnControlFocusExchange, NULL);
   
	tnRegisterCallBack(gpbtnLan, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);
	tnRegisterCallBack(gpbtnBri, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);
	tnRegisterCallBack(gpbtnCon, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);    
	tnRegisterCallBack(gpbtnSub, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);
	tnRegisterCallBack(gpbtnAut, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);
	tnRegisterCallBack(gpbtnSta, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);
	tnRegisterCallBack(gpbtnRst, WIDGET_ESCAPED, ExitSetting, (DATA_POINTER) &returndata);
	// set focus on the first button.

	GrSetFocus(gpbtnLan->wid);
	
	tnMainLoop();
	return;
}



