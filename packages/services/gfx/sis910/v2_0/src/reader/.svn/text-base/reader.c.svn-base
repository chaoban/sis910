/**
*@AP:Text Reader
*@author:amboo
*/
#include<stdio.h>
//#include<nano-X.h>
#include "tinywidget/tnWidgets.h"
#include "../share/ecos_config.h"
#include "../share/global.h"
#include "reader.h"
#include "common.h"
#include "recent.h"
#include "bookmarker.h"
#include "dirent.h"
#include "tr_engine.h"
#include <assert.h>

#ifdef __ECOS
#include "../desktop/global.h"
#include <cyg/fileio/fileio.h>
#include <cyg/fs/fatfs.h>
#endif

#define TBWIDTH  800
#define TBHEIGHT 560

#ifndef __ECOS
#define CFG_FILE_PATH  "./resource/config.ini"
#define RESOURCE_PATH  "./resource"
#endif

#define AP_NAME       "Reader"

#define __DEBUG
#ifdef __DEBUG
#define EPRINTF(a) printf a    
#else
#define EPRINTF(a)
#endif

//#define  TX_USE_ENGINE

static TN_WIDGET *window_reader=NULL,*window_menu=NULL,*gWnd_bk=NULL;
static TN_WIDGET*rd_lbl_filename,*gLblBkTime,*gLblMuTime,*gLblScale;
static TN_WIDGET *textreader=NULL;
static RCINFO reader_rcinfo;
static GR_TIMER_ID  gAutoTimer;
static GR_IMAGE_ID gIcon1,gIcon2;

static  struct configFile *cfgfile=NULL;
#define PATH_MAX_LENGTH  100
static char path[PATH_MAX_LENGTH];           //used for File Browser
//
static char * recent[RECENT_NUM];
static int    recentindex;
static BMarker mybm[BOOKMARKER_NUM];
static int mybmindex;
//#ifdef TX_USE_ENGINE

static tEngineInfo gEgInfo;
//#endif
//
void exitreader();
void openContext(char * file,int line);

//void addToRecentList(char * file);
//static void tmBtnDisable(TN_WIDGET *widget)
//{
//    if (widget->type!=TN_BUTTON)
//        return;
//    widget->enabled=GR_FALSE;
//}
//static void tmBtnEnable(TN_WIDGET *widget)
//{
//    if (widget->type!=TN_BUTTON)
//        return;
//    widget->enabled=GR_TRUE;
//}
//

//static void wnd_escaped(TN_WIDGET *widget, DATA_POINTER dptr)
//{
//    TN_WIDGET *focus,*wnd;
//    focus=(TN_WIDGET *)dptr;
//    if (widget->type==TN_WINDOW)
//        wnd=widget;
//    else
//        wnd=GetParent(widget);
//	
//    tnDestroyWidget(wnd);
//    if (!focus->enabled)
//        tmBtnEnable(focus);
//    GrSetFocus(focus->wid);
//}
/**
*@return from menu window to context, if no context window then exit
*/
static void menuReturn(TN_WIDGET * widget, DATA_POINTER dptr)
{
    TN_WIDGET *parent;
    parent=GetParent(widget);
    tnDestroyWidget(parent);
        
#ifdef _pao_shu
    assert(gWnd_bk);
    tnDestroyWidget(gWnd_bk);
#endif        
    if (window_reader==NULL)
        exitreader();
}
/**
*
*/
static LoadIconImg()
{
    gIcon1=GrLoadImageFromFile("/home/amboo/work/dpf/resource/Blue/Reader/icon0.bmp",0);
    gIcon2=GrLoadImageFromFile("/home/amboo/work/dpf/resource/Blue/Reader/txt.bmp",0);
}
static void freelist(char **str,int m)
{
    int i;
    for(i=0;i<m;i++)
        free(str[i]);
    free(str);
    return;
}
 static int filter(const struct dirent *dirp)
{       
    DIR *dir;
    char tmp[200];
    int ret;
    if ((!strcmp(dirp->d_name,".")) ||(!strcmp(dirp->d_name,"..")) )
        return 1;
    else if (strstr(dirp->d_name,".txt") ||strstr(dirp->d_name,".TXT"))
        return 1;
    else
    {
        strcpy(tmp,path);
        strcat(tmp,"/");
        strcat(tmp,dirp->d_name);
        dir=opendir(tmp);
        if (dir!=NULL)
        {
            ret= 1;
            closedir(dir);                              
        }
        else
            ret= 0;
        return ret;        
    }

}
 /**
 *@remove the space of left and right
 */
static char * TrTrim(char * in)
{
    int len,i;
    len=strlen(in);
    if (len==0) return NULL;
    len-=1;
    while (in[len]==' ')
    {
        in[len]='\0';
        len--;
    }
    i=0;
    while (in[i]==' ') i++;
    return &in[i];
}
#ifdef __ECOS
static void  AddDirToList(TN_WIDGET *list, char * path)
{
    int n=0;
    struct vfat_descriptor vfatinfo,*ptr1,*ptr2;
    n=listdir(path,&vfatinfo);
    ptr2=&vfatinfo;
    if (n==0)
    {
        EPRINTF(("no entry\n"));
        close_listdir(ptr2);
        return;
    }
    else
    {
        tnDeleteAllItemsFromListBox(list);                    
        do
        {
            vfatinfo=*vfatinfo.next;
            if (((vfatinfo.attr &0x10)==0x10)||((vfatinfo.attr &0x20)==0x20))
            {
                tnAddItemToListBox(list, vfatinfo.filename, 0);
                //printf("%s\n",vfatinfo.filename);
            }
        //pvfatinfo1=pvfatinfo1->next;
        }while(vfatinfo.next!=NULL);
        
        tnSetSelectedListItemAt(list,0,GR_TRUE);        
        close_listdir(ptr2);
    }
}
#endif
/**
*
*/
static int IsDir(char * path)
{
    int ret;
#if 0        
    struct vfat_descriptor vfatinfo,*ptr;
    ret=listdir(path,&vfatinfo);
    ptr=&vfatinfo;        
    close_listdir(ptr);
    if (ret<0) return 0;
    else return 1;        
#else
    DIR *dir;
    dir=opendir(path);
    if (dir!=NULL) ret=1;
    else ret=0;
    closedir(dir);
    return ret;
#endif  //end of #if 0
}

void myBook_Clicked(TN_WIDGET * widget, DATA_POINTER dptr)
{
    char ** str=NULL;
    char *ptr;
    int count,n,i;
    struct dirent **namelist;
    tnGetSelectedListItems(widget, &str, &count);
    if (!count) return;
    ptr=TrTrim(str[0]);
    //
    if(strcmp(ptr,".")==0)
	{
        //tnSetSelectedListItem(listbox,str[0],GR_FALSE);
        return;
	}
    if(strcmp(ptr,"..") == 0)
    {
        if(strcmp(path,".")==0) return;
            tnDeleteAllItemsFromListBox(widget);
        i = strlen(path)-1;
        while(i && path[i] != '/')
	        i--;
     	
     	path[i] = '\0';
#ifndef __ECOS            
        n=ap_scandir(path,&namelist,filter,NULL);
        
        if (n<0)
            EPRINTF(("error\n"));
        else
        {
            i=0;
            while(i<n)
            {
                LoadIconImg();
                tnAddItemToListBox(widget, namelist[i]->d_name,gIcon1);  
                free(namelist[i]);
                i++;
            }
            free(namelist);
            tnSetSelectedListItemAt(widget,0,GR_TRUE);
        }
#else
        AddDirToList(widget, path);
#endif
	}
	else
	{
#ifndef __ECOS             
	
        DIR *dir;
        strcat(path,"/");	
        strcat(path,ptr);
        if ((dir=opendir(path))!=NULL)
        {
            //open dir
            tnDeleteAllItemsFromListBox(widget);
            n=ap_scandir(path,&namelist,filter,NULL);
        
            if (n<0)
                EPRINTF(("error\n"));
            else
            {
                i=0;
                while(i<n)
                {
                    LoadIconImg();
                    tnAddItemToListBox(widget, namelist[i]->d_name,gIcon1);  
                    free(namelist[i]);
                    i++;
                }
                free(namelist);
                tnSetSelectedListItemAt(widget,0,GR_TRUE);
            }
            closedir(dir);

        }
#else
        strcat(path,"/");	
        strcat(path,ptr);
        if (IsDir(path))
        {
            AddDirToList(widget, path);
        }
#endif
        //open txt file
        else
        {
            if (window_reader==NULL)
            {
                openContext(path,0);
            }
            else
            {
                char name[20];
                if (TrLoadFile(textreader, path, 0, &gEgInfo))
                {
                    //add to recent list
                    recentindex=addToRecent(recentindex,recent,path);
                    // EPRINTF(("openContext---index:%d\n ",index));            
            		getFileName(name,path);
                    tnSetLabelCaption(rd_lbl_filename, name);				  
                }
                else
                {
                    EPRINTF(("load txt file failure\n"));
                    return ;
                }
            
            }
            
		    menuReturn(widget,dptr);
        }
	}
	
	freelist(str,count);
    EPRINTF(("path=%s\n",path));

}
void myBook(TN_WIDGET * widget, DATA_POINTER dptr)
{
    TN_WIDGET *list,*window,*lbl;
    struct dirent **namelist;
    int n,i;
    //tmBtnDisable((TN_WIDGET *)dptr);
#ifdef _pao_shu         
    window=CreateWindowFromRC(gWnd_bk, 0, ID_WND_MYBOOK, &reader_rcinfo);
#else
    window=CreateWindowFromRC(window_menu, 0, ID_WND_MYBOOK, &reader_rcinfo);
#endif        
    // lbl=CreateLabelFromRC(window, 0, ID_LBL_MYBOOK, &reader_rcinfo);   
    //tnSetLabelCaption(lbl, char * caption)
    list=CreateListBoxFromRC(window, 0, ID_LBX_MYBOOK, &reader_rcinfo);
    strcpy(path,RESOURCE_PATH);
    printf("path:%s\n",path);
#ifdef __ECOS
    AddDirToList(list,path);
#else
    n=ap_scandir(path,&namelist,filter,NULL);  //amboo
    if (n<0)
            EPRINTF(("error\n"));
    else
    {
        i=0;
        while(i<n)
        {
            LoadIconImg();
            char * tmp=(char *)malloc((strlen(path)+strlen(namelist[i]->d_name)+4)*sizeof(char));
            assert(tmp);
            if (tmp==NULL) return;
            strcpy(tmp,path);
            strcat(tmp,"/");
            strcat(tmp,namelist[i]->d_name);
            if (IsDir(tmp))
                tnAddItemToListBox(list, namelist[i]->d_name,gIcon1);  
            else
                tnAddItemToListBox(list, namelist[i]->d_name,gIcon2);  
            //free(namelist[i]);
            free(tmp);
            i++;
        }
    }
    freelist(namelist,n);            
#endif  
    tnSetSelectedListItemAt(list,0,GR_TRUE);
    tnRegisterCallBack(list, WIDGET_ESCAPED, OnSubMenuEscape, dptr);
    tnRegisterCallBack(list, WIDGET_CLICKED, myBook_Clicked, dptr); 
    GrSetFocus(list->wid);

}
void Recently_Clicked(TN_WIDGET * widget, DATA_POINTER dptr)
{
    // char ** str=NULL;
    char *p;
    int count,n,i;
    int *pos;
    //tnGetSelectedListItems(widget, &str, &count);
    tnGetSelectedListPos(widget,&pos, &count);
    if (count==0) return;
    printf("pos[0]=%d\n",pos[0]);
    i=recentindex-pos[0];
    if (i<0) i+=RECENT_NUM;         
    p=recent[i];
    while((*p!='.')&&(*p!='/')) p++;
    strcpy(path,p);
    printf("Recently_Clicked:path=%s\n",path);
    if (window_reader==NULL)
    {
        openContext(path,0);
    }
    else
    {
        char name[20];
        if (TrLoadFile(textreader, path, 0, &gEgInfo))
        {
            //add to recent list
            recentindex=addToRecent(recentindex,recent,path);
	        getFileName(name,path);
	        tnSetLabelCaption(rd_lbl_filename, name);
            // EPRINTF(("openContext---index:%d\n ",index));            
        }
        else
        {
            EPRINTF(("load txt file failure\n"));
            return ;
        }
        //destroy menu window and to text
        //wnd_escaped(window_menu,dptr);
        menuReturn(widget,dptr);
    }       
    
    //freelist(str,count);  
    assert(pos);
    free(pos);
}

void Recently(TN_WIDGET * widget, DATA_POINTER dptr)
{
    TN_WIDGET *list,*window,*lbl;
    //tmBtnDisable((TN_WIDGET *)dptr);  
#ifdef _pao_shu              
    window=CreateWindowFromRC(gWnd_bk, 0, ID_WND_RECENT, &reader_rcinfo);
#else
    window=CreateWindowFromRC(window_menu, 0, ID_WND_RECENT, &reader_rcinfo);
#endif        
        
    //lbl=CreateLabelFromRC(window, 0, ID_LBL_RECENT, &reader_rcinfo);                
    list=CreateListBoxFromRC(window, 0, ID_LBX_RECENT, &reader_rcinfo);
    addRecentToListBox(recentindex,(const char *)recent,list);
    tnSetSelectedListItemAt(list,0,GR_TRUE);
    tnRegisterCallBack(list, WIDGET_ESCAPED, OnSubMenuEscape, dptr);    
    tnRegisterCallBack(list, WIDGET_CLICKED, Recently_Clicked, dptr);         
    GrSetFocus(list->wid);        
}
void bookmarker_selected(TN_WIDGET * widget, DATA_POINTER dptr)
{
    char *file;
    int curline,count,i;
    int *pos;
    tnGetSelectedListPos(widget,&pos, &count);
    if (count==0) return;
    printf("pos[0]=%d\n",pos[0]);
    i=mybmindex-pos[0];
    free(pos);
    if (i<0) i+=BOOKMARKER_NUM;
    curline=mybm[i].posi;
    file=mybm[i].path;        
    if (textreader!=NULL)
    {
    
        if (TrLoadFile(textreader,file, curline, &gEgInfo))
        {
            char name[20];
            recentindex=addToRecent(recentindex,recent,file);
            getFileName(name,file);
            tnSetLabelCaption(rd_lbl_filename, name);
        }
        else
            return;
    }
    else
    {
        openContext(file,curline);                    
    }
    menuReturn(widget,dptr);

}
void myBookMarker(TN_WIDGET * widget, DATA_POINTER dptr)
{
    TN_WIDGET *list,*window,*lbl;
    //tmBtnDisable((TN_WIDGET *)dptr);
#ifdef _pao_shu        
    window=CreateWindowFromRC(gWnd_bk, 0, ID_WND_MYBM, &reader_rcinfo);
#else
    window=CreateWindowFromRC(window_menu, 0, ID_WND_MYBM, &reader_rcinfo);
#endif        
    //lbl=CreateLabelFromRC(window, 0, ID_LBL_BOOKMARKER, &reader_rcinfo);                
    list=CreateListBoxFromRC(window, 0, ID_LBX_MYBM, &reader_rcinfo);
    addBMToListBox(mybmindex,mybm,list);
    tnSetSelectedListItemAt(list,0,GR_TRUE);
    tnRegisterCallBack(list, WIDGET_ESCAPED, OnSubMenuEscape, dptr);    
    tnRegisterCallBack(list, WIDGET_CLICKED, bookmarker_selected, dptr);         
    GrSetFocus(list->wid);                 
}
void addBookMarker(TN_WIDGET * widget, DATA_POINTER dptr)
{       
    int curline,i;
    FILE *fp;
    int len,posi;
    char *p;
    if (textreader==NULL) return;        
    if (gEgInfo.buffer!=NULL)
    {
        char *text=(char *)malloc(32*sizeof(char));
        assert(text);
        if (text==NULL) return;            
        curline=gEgInfo.curline;                    
        posi=gEgInfo.line[curline];
        //get some string of first line 
        memcpy(text,&gEgInfo.buffer[posi],20);
        printf("---addBookMarker text:%s\n",text);
        //remove the char of '\n' or '\r'
        p=text;
        while (*p)
        {
            if (((*p)=='\n')||((*p)=='\r'))
                (*p)=' ';
            p++;
        }                    
        if (strlen(text)==0) return;
        //memcpy(text,p,(strlen(p)<50)?strlen(p):50);
        //add to bookmarker
        mybmindex=addToBM(mybm,mybmindex,text,gEgInfo.filepath,curline);
        free(text);
        //wnd_escaped(widget,dptr);
        menuReturn(widget,dptr);                    
    }
}
/**
*@
*/
void autoBrowser(TN_WIDGET * widget, DATA_POINTER dptr)
{
    printf("Auto Browser\n");
    if (textreader==NULL) return;
    gAutoTimer=GrCreateTimer(textreader->wid,1000);
    //wnd_escaped(widget,dptr);        
    menuReturn(widget,dptr);
}

void AutoBrowerTimer(TN_WIDGET *widget, DATA_POINTER dptr)
{
    if (gEgInfo.buffer==NULL) return;
    if(gEgInfo.curline<(gEgInfo.linecount-1))
    {
        gEgInfo.curline++;
        GrClearWindow(widget->wid,GR_TRUE);
    }       
}
/**
*@
*/
static void GetTime(char *time_b)
{
    time_t ticks;
    struct tm *t;

    ticks = time(NULL);
    t = localtime(&ticks);
    strftime(time_b, 127, "%H:%M:%S", t);
    //dprintf("Now Time =%s\n", time_b);
}
void OnTimer(TN_WIDGET *widget, DATA_POINTER dptr)
{
    // char date[20];
    char time[20];
    GetTime(time);
    tnSetLabelCaption(gLblBkTime,time);
}
/////////////////////////////////////////////////////////////////
//button focus exchange in main window
//static void btn_exchange(TN_WIDGET *widget, DATA_POINTER dptr)
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
void openReaderMenu()
{
    TN_WIDGET *button[6];
    TN_WIDGET *lbl[2];
#ifdef _pao_shu   
    gWnd_bk=CreateWindowFromRC(NULL,0, ID_WND_TRBK, &reader_rcinfo);
    lbl[0]=CreateLabelFromRC(gWnd_bk, 0, ID_LBL_MENU, &reader_rcinfo);        
    window_menu=CreateWindowFromRC(gWnd_bk,0, ID_WND_TRMENU, &reader_rcinfo);
#else
    window_menu=CreateWindowFromRC(NULL,0, ID_WND_TRMENU, &reader_rcinfo);
#endif    
    lbl[1]=CreateLabelFromRC(window_menu, 0, ID_LBL_TR, &reader_rcinfo);
    button[0] = CreateButtonFromRC(window_menu, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MYBOOK,&reader_rcinfo);
	button[1] = CreateButtonFromRC(window_menu, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_RF,&reader_rcinfo);
	button[2] = CreateButtonFromRC(window_menu, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MYBM,&reader_rcinfo);
	button[3] = CreateButtonFromRC(window_menu, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ADDBM,&reader_rcinfo);
	button[4] = CreateButtonFromRC(window_menu, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_AUTO,&reader_rcinfo);     
	//button[5] = CreateButtonFromRC(window_menu, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_AUTOSTOP,&reader_rcinfo);         
        //
    tnRegisterCallBack(button[0], WIDGET_CLICKED, (CallBackFuncPtr)myBook, (DATA_POINTER)button[0]);
	tnRegisterCallBack(button[1], WIDGET_CLICKED, (CallBackFuncPtr)Recently, (DATA_POINTER)button[1]);
	tnRegisterCallBack(button[2], WIDGET_CLICKED, (CallBackFuncPtr)myBookMarker, (DATA_POINTER)button[2]);
	tnRegisterCallBack(button[3], WIDGET_CLICKED, (CallBackFuncPtr)addBookMarker, (DATA_POINTER)button[3]);
    tnRegisterCallBack(button[4], WIDGET_CLICKED, (CallBackFuncPtr)autoBrowser, (DATA_POINTER)button[4]);
    //tnRegisterCallBack(button[5], WIDGET_CLICKED, (CallBackFuncPtr)autoBrowserStop, (DATA_POINTER)button[5]);
    tnRegisterCallBack(button[0], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack(button[1], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange,NULL);
	tnRegisterCallBack(button[2], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack(button[3], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
  	tnRegisterCallBack(button[4], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
  	//tnRegisterCallBack(button[5], WIDGET_KEYDOWN, (CallBackFuncPtr)btn_exchange, NULL);
    tnRegisterCallBack(button[0], WIDGET_ESCAPED, (CallBackFuncPtr)menuReturn, NULL);
	tnRegisterCallBack(button[1], WIDGET_ESCAPED, (CallBackFuncPtr)menuReturn,NULL);
	tnRegisterCallBack(button[2], WIDGET_ESCAPED, (CallBackFuncPtr)menuReturn, NULL);
	tnRegisterCallBack(button[3], WIDGET_ESCAPED, (CallBackFuncPtr)menuReturn, NULL);
  	tnRegisterCallBack(button[4], WIDGET_ESCAPED, (CallBackFuncPtr)menuReturn, NULL);
  	//tnRegisterCallBack(button[5], WIDGET_ESCAPED, (CallBackFuncPtr)menuReturn, NULL);
    
    GrSetFocus(button[0]->wid);    
}
///////////////////////////////////////////////////////////////
void showMenu(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GrDestroyTimer(gAutoTimer);
    openReaderMenu();
}

//#ifdef TX_USE_ENGINE
void ReaderKeydown(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_KEY ch;
    int lines=gEgInfo.lines;  
    
    ch=(long)widget->data;
    GrDestroyTimer(gAutoTimer);
    switch(ch)
    {
        case MWKEY_LEFT:
            //page up	
            //printf("keyPress left curline:%d\n",gEgInfo.curline);
            if(gEgInfo.curline>0)
            {
                gEgInfo.curline-=lines;
                if(gEgInfo.curline<0)
                    gEgInfo.curline=0;
                GrClearWindow(widget->wid,GR_TRUE); 
            }
            break;
            
        case MWKEY_RIGHT:
            //page dowm
            //printf("keyPress right\n");
            if((gEgInfo.curline+lines)<(gEgInfo.linecount-1))
            {
                gEgInfo.curline+=lines;
                GrClearWindow(widget->wid,GR_TRUE);
            }            
            break;
            
        case MWKEY_UP:
            //line up
            //printf("keyPress up\n");
            if(gEgInfo.curline>0)
            {
                gEgInfo.curline--;
                GrClearWindow(widget->wid,GR_TRUE);
            }            
            break;
            
    	case MWKEY_DOWN:
            //line down
            //printf("keyPress down\n");
            if(gEgInfo.curline<(gEgInfo.linecount-1))
            {
                gEgInfo.curline++;
                GrClearWindow(widget->wid,GR_TRUE);
            }
            break;    
    }
}
void ReaderExposure(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_WINDOW_INFO winfo;
    int  scale;
    int zs,xs;
    char tmp[10];
    GrGetWindowInfo(widget->wid, &winfo);
    GrSetGCForeground(widget->gc, GR_RGB(0, 0, 0));
    GrSetGCUseBackground(widget->gc,GR_FALSE);
    TrDrawPageDown(widget,&gEgInfo,gEgInfo.line[gEgInfo.curline]);
    //set the scale label
    scale=((gEgInfo.curline*1000)/gEgInfo.linecount);
    zs=scale/10;
    xs=scale%10;
    sprintf(tmp,"%d.%d%%",zs,xs);
    //strcat(tmp,"%");
    tnSetLabelCaption(gLblScale, tmp);
    GrSetFocus(widget->wid);
}
//#endif
void ExitContext(TN_WIDGET *widget, DATA_POINTER dptr)
{
    exitreader();
}
void openContext(char * file, int line)
{
    char name[20];
    GR_WINDOW_INFO winfo;
    window_reader=CreateWindowFromRC(NULL,0,ID_WND_CONTEXT_BK,&reader_rcinfo);
    rd_lbl_filename=CreateLabelFromRC(window_reader, 0, ID_LBL_FILENAME, &reader_rcinfo);
    gLblBkTime=CreateLabelFromRC(window_reader, 0, ID_LBL_TIME_BK, &reader_rcinfo);
    gLblScale=CreateLabelFromRC(window_reader, 0, ID_LBL_SCALE, &reader_rcinfo);
    tnRegisterCallBack(window_reader,WIDGET_TIMER,OnTimer,NULL);        
    GrCreateTimer(window_reader->wid,1000);
    //create context window
    textreader=CreateWindowFromRC(window_reader,0,ID_WND_CONTEXT,&reader_rcinfo); 
    TrInitEgInfo(&gEgInfo,textreader);
    if (TrLoadFile(textreader, file, line, &gEgInfo))
    {
        //add to recent list
        recentindex=addToRecent(recentindex,recent,file);
        // EPRINTF(("openContext---index:%d\n ",index));           
	    getFileName(name,file);
	    tnSetLabelCaption(rd_lbl_filename, name);
    }
    else
    {
        EPRINTF(("load txt file failure\n"));
        //return ;
    }
    tnRegisterCallBack(textreader,WIDGET_CLICKED,showMenu,NULL);
    tnRegisterCallBack(textreader,WIDGET_KEYDOWN,ReaderKeydown,NULL);    
    tnRegisterCallBack(textreader,WIDGET_EXPOSURE,ReaderExposure,NULL);        
    tnRegisterCallBack(textreader,WIDGET_ESCAPED,ExitContext,NULL);
    tnRegisterCallBack(textreader,WIDGET_TIMER,AutoBrowerTimer,NULL);        
    GrSetFocus(textreader->wid);        
}
//////////////////////////////////////////////////////////////////////////
//Reader Entry Function
//file is NULL then show menu else open the file directly
void openReader(char *file)
{       
    char *rcfilepath=NULL;
    char *subject=NULL;
    char *recentfile=NULL;  
    char *bmfile=NULL;
	//select the rc file according to the subject in configfile
    cfgfile= (struct configFile *) readConfigFile(CFG_FILE_PATH);
    if(!cfgfile){
        EPRINTF(("can't load the configure file\n"));
        return;
    }
	subject= (char *) getConfigString(cfgfile,"setting","subject");
	if (subject==NULL) {
        EPRINTF(("can't find this key\n"));
        return;
    }
    //init Recentfile list
	recentfile= (char *) getConfigString(cfgfile,"text reader","recentfile");
	if (recentfile==NULL) {
        EPRINTF(("can't find this key:recentfile\n"));
        return;
    } 
    recentindex=initRecent(recent,recentfile);
    EPRINTF(("---recentindex:%d\n",recentindex));        
    //init BookMarker
	bmfile= (char *) getConfigString(cfgfile,"text reader","bmfile");
	if (bmfile==NULL) {
        EPRINTF(("can't find this key:bmfile\n"));
        return;
    }
    EPRINTF(("---mybmlist:%s\n",bmfile));                        
    //for test
    mybmindex=-1;
    EPRINTF(("---mybmindex:%d\n",mybmindex));                
    mybmindex=initBookMarker(mybm,bmfile);
    EPRINTF(("---mybmindex:%d\n",mybmindex));
            
    //
	tnAppInitialize();
    rcfilepath=(char *)malloc(100*sizeof(char));
    assert(rcfilepath);
    if (rcfilepath==NULL) return;
    getRcFilePath(rcfilepath, RESOURCE_PATH, subject, AP_NAME);
    load_rc(&reader_rcinfo,rcfilepath, "en");
    free(rcfilepath);
    
    if (file==NULL)
        openReaderMenu();
    else
        openContext(file,0);
    tnMainLoop();
        
}

void exitreader()
{       
    TN_WIDGET * wnd;
    //char * filelist=NULL;
    char *bmfile=NULL;
    char *recentfile=NULL;
    GrDestroyTimer(gAutoTimer);
    TrResetEngine(&gEgInfo);
    //save recent file list
    recentfile= (char *) getConfigString(cfgfile,"text reader","recentfile");       
    EPRINTF(("---recentfilelist:%s\n",recentfile));        
    SaveRecentFile(recent, recentindex, recentfile);
    freeRecent(recent);
    //save bookmarker
    bmfile= (char *) getConfigString(cfgfile,"text reader","bmfile");
    saveBookmarker(mybm,mybmindex,bmfile);
    freeBM(mybm); 
    //save configfile
    saveConfigFile(cfgfile, CFG_FILE_PATH);
    unloadConfigFile(cfgfile);
    
    if (window_reader)
        tnDestroyWidget(window_reader);
#ifdef __ECOS
    printf("send MAIL_RETURN is %d\n",MAIL_RETURN);
    cyg_mbox_put(dpfDesktopMboxHandle, (void *)MAIL_RETURN);
#endif
    EPRINTF(("Exitreader--------\n"));
    tnEndApp(&reader_rcinfo);	
    
}

struct menuReturnData returndata;

#ifdef __ECOS
void Reader_thread(CYG_ADDRWORD data)
{
    
   	struct THREAD_PARAMETER *paraData = (struct THREAD_PARAMETER *) data;
    INIT_PER_THREAD_DATA(paraData->thread_data_index);

    returndata.wFocus = (TN_WIDGET *)paraData->others;
  	returndata.prcinfo = &reader_rcinfo;	

    openReader(NULL);
}
#else
int main(int argc,char **argv)
{
    //char file[100]="/home/amboo/work/dpf/reader/test2.txt";
    strcpy(path, "./resource/test2.txt");
    openReader(path);
    return 0;
}
#endif






