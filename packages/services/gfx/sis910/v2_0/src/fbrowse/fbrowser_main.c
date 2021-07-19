#include<stdio.h>
#include "tinywidget/tnWidgets.h"
#include "../share/global.h"
#include "../share/browser.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "fbrowser_main.h"
//#include "fbrowser.h"


//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>

#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <cyg/fileio/fileio.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>            // HAL polled output
#include <cyg/fs/fatfs.h>

#include "../share/ecos_config.h"
//==========================================================================



#ifdef __ECOS
//#include "global.h"
#endif

#define FBROWSER_DEBUG 1

#ifdef FBROWSER_DEBUG
#define EPRINTF(n) printf n
#define DPRINTF(n) printf n
#define ASSERT(n) do{  \
                    if (n == 0) \
                        printf("ASSERT: file %s, line %d \n", __FILE__, __LINE__); \
                  }while(0)
#else
#define EPRINTF
#define DPRINTF
#define ASSERT(n)
#endif // end of AUDIO_DEBUG

#define WINDOW2_HEIGHT  400
#define WINDOW2_WIDTH  300

#define music_path	"/testcopy/"
#define pic_path	"/testcopy/"
#define txt_path	"/testcopy/"

//static GR_BOOL gbFBrowserOpen = GR_FALSE;
TN_WIDGET *FB_MainWindow, *file_oper=NULL;
TN_WIDGET *file_copy, *file_del, *file_open, *file_attr;
TN_WIDGET *listbox;

TN_WIDGET *yesno_window, *copyto_window, *attr_window;
TN_WIDGET *btn_yes, *btn_no;
TN_WIDGET *btn_music, *btn_pic, *btn_txt;
TN_WIDGET *bar_state;
TN_WIDGET *btn_attrOK;	
TN_WIDGET *lbl_issure;
TN_WIDGET *lbl_FileSize, *lbl_FileType;

//static char** filelist;
//static char path[128]="";

//static char cur_path[128] = "/romdisk/resource";
static char cur_path[128] = "/";

//static int cur_item;
//static char cur_file[32]="";

extern GR_IMAGE_ID selectAllType(char *str);
	 
RCINFO rcinfo;

void SelectionExecuted(TN_WIDGET *widget, DATA_POINTER data)
{
  
    
        

}

void SelectionInfo(TN_WIDGET *widget, DATA_POINTER data)
{
    char ***filename;
    int nCount = 0, i = 0;
    tnGetSelectedListItems(widget, filename, &nCount);
    
    for(i = 0; i < nCount; i++)
        printf("BrowserKeydown File %d: %s\n", i, (*filename)[i]);
    
    FreeStrList( *filename,nCount);
}

#ifdef __ECOS
void FB_thread(CYG_ADDRWORD data)
{
    struct menuReturnData returndata;
    struct THREAD_PARAMETER *paraData = (struct THREAD_PARAMETER *) data;
    
    INIT_PER_THREAD_DATA(paraData->thread_data_index);
                
    returndata.wFocus = (TN_WIDGET *)paraData->others;                          
    returndata.prcinfo = &rcinfo;                                                    
  
#else
int main(int argc,char **argv)
{   
#endif //end of __ECOS
    struct configFile *pConfig = NULL;
    char *pLang = NULL, *pSub = NULL;
    char RcFilePath[200];
   
    	
#ifdef __ECOS	
    //tnControlLibInitialize();
    tnAppInitialize();	 

#else
    tnAppInitialize();
    
#endif 	// end __ECOS 		       
    pConfig = (struct configFile *) readConfigFile(CFG_FILE_PATH);
    if(!pConfig)
    {
        EPRINTF(("can't open %s\n", CFG_FILE_PATH));
        return;
    }
 
    pLang = (char *) getConfigString(pConfig, "setting","language");
    pSub = (char *) getConfigString(pConfig, "setting", "subject");
    if ((pSub == NULL) || (pLang == NULL))
    {
        DPRINTF(("not exist this key\n"));
        unloadConfigFile(pConfig);
        return;
    }
    
    getRcFilePath(RcFilePath, RESOURCE_PATH, pSub, BROWSER_NAME);
    unloadConfigFile(pConfig);
    OpenFileBrowser((TN_WIDGET *)paraData->others, RcFilePath, cur_path, selectAllType, NULL,SelectionInfo, OnMenuEscape);
    
  
    tnMainLoop();
    
    return;
       
}

//
//
//void endapp(TN_WIDGET *widget,DATA_POINTER d)
//{
//    tnDestroyWidget(widget);
//    
//    if(FB_MainWindow)
//        tnDestroyWidget(FB_MainWindow);
//    tnEndApp(&rcinfo);
//}
//
//int CreateFileMenu()
//{
////    if(file_oper)
////    {
////        printf("CreateFileMenu NULL return\n");
////        return 0;
////    }
//			
//    //GR_RECT fileop_rect={420,230,100,100} ;	
//    file_oper = CreateWindowFromRC(NULL,0,ID_WND_FILEOP,&rcinfo);
//    
//    file_open = CreateButtonFromRC(file_oper,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILEOPEN,&rcinfo);
//    file_copy = CreateButtonFromRC(file_oper,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILECOPY,&rcinfo);
//    file_del  = CreateButtonFromRC(file_oper,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILEDELETE,&rcinfo);
//    file_attr = CreateButtonFromRC(file_oper,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILEATTRIBUTE,&rcinfo);
//    
//    tnRegisterCallBack(file_open,WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
//    tnRegisterCallBack(file_copy,WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
//    tnRegisterCallBack(file_del, WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
//    tnRegisterCallBack(file_attr,WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
//    
//    tnRegisterCallBack(file_open,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,listbox);
//    tnRegisterCallBack(file_copy,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,listbox);
//    tnRegisterCallBack(file_del, WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,listbox);
//    tnRegisterCallBack(file_attr,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,listbox);
//    	
//    tnRegisterCallBack(file_open,WIDGET_CLICKED,click_open,NULL);	
//    tnRegisterCallBack(file_del,  WIDGET_CLICKED,click_del,NULL);
//    tnRegisterCallBack(file_copy, WIDGET_CLICKED,click_copy,NULL);
//    tnRegisterCallBack(file_attr,WIDGET_CLICKED, click_attr,NULL);
//    
//    GrSetFocus(file_open->wid);
//}
//
//void keydown_fileop(TN_WIDGET* pwidget,DATA_POINTER p)
//{	
//    long ch;
//    TN_WIDGET*	pw;
//    ch = (long)pwidget->data;
//    if (ch == MWKEY_LEFT || ch==MWKEY_UP)
//    {
//        pw = GetPrevSibling(pwidget);
//        GrSetFocus(pw->wid);	
//        return;
//    }
//    if (ch == MWKEY_RIGHT || ch==MWKEY_DOWN)
//    {
//        pw = GetNextSibling(pwidget);
//        GrSetFocus(pw->wid);	
//        return;
//    }	
//    
//    if(ch == MWKEY_BACKSPACE)
//    {
//        DestroyFileMenu();
//        GrSetFocus(listbox->wid);
//        tnSetSelectedListItemAt(listbox,cur_item,1);
//    }
//}
//
//void click_open()
//{	
//#ifdef __ECOS
//      cyg_mbox_put(dpfDesktopMboxHandle,(void*)MAIL_FB_OPEN);
//    //  sprintf(dpfFbOpenedFileName,"%s/%s",cur_path,cur_file);
//            		
//#endif
//    endapp(FB_MainWindow,0);
//}
//
//void click_del()
//{
//    DestroyFileMenu();
//    CreateYesNo();
//    GrSetFocus(btn_yes->wid);
//}
//
//void click_copy()
//{	
//    DestroyFileMenu();
//    //GR_RECT r={250,250,320,100};
//    copyto_window = CreateWindowFromRC(NULL,TN_CONTROL_FLAGS_DEFAULT,3,&rcinfo);
//    btn_music = CreateButtonFromRC(copyto_window,TN_CONTROL_FLAGS_DEFAULT,6,&rcinfo);
//    btn_pic	 = CreateButtonFromRC(copyto_window,TN_CONTROL_FLAGS_DEFAULT,7,&rcinfo);
//    btn_txt	 = CreateButtonFromRC(copyto_window,TN_CONTROL_FLAGS_DEFAULT,8,&rcinfo);
//    bar_state = CreateProgressBarFromRC(copyto_window,TN_CONTROL_FLAGS_DEFAULT,0,&rcinfo);
//    
//    tnRegisterCallBack(btn_music, WIDGET_KEYDOWN,keydown_copytoXXX,NULL);
//    tnRegisterCallBack(btn_pic, WIDGET_KEYDOWN,keydown_copytoXXX,NULL);
//    tnRegisterCallBack(btn_txt, WIDGET_KEYDOWN,keydown_copytoXXX,NULL);
//    
//    tnRegisterCallBack(btn_music, WIDGET_CLICKED,click_copytoXXX,(DATA_POINTER)0);
//    tnRegisterCallBack(btn_pic, WIDGET_CLICKED,click_copytoXXX,(DATA_POINTER)1);
//    tnRegisterCallBack(btn_txt, WIDGET_CLICKED,click_copytoXXX,(DATA_POINTER)2);
//    
//    //GrUnmapWindow(bar_state->wid);
//    GrSetFocus(btn_music->wid);
//}
//
//void click_attr()
//{
//    DestroyFileMenu();
//    //GR_RECT r={250,250,320,100};
//    attr_window = CreateWindowFromRC(NULL,TN_CONTROL_FLAGS_DEFAULT,4,&rcinfo);
//    lbl_FileSize =CreateLabelFromRC(attr_window,TN_CONTROL_FLAGS_DEFAULT,1,&rcinfo);
//    lbl_FileType =CreateLabelFromRC(attr_window,TN_CONTROL_FLAGS_DEFAULT,2,&rcinfo);
//    
//    tnSetLabelCaption(lbl_FileSize,"File Size:");
//    tnSetLabelCaption(lbl_FileType,"File Type:");
//    
//    btn_attrOK = CreateButtonFromRC(attr_window,TN_CONTROL_FLAGS_DEFAULT,9,&rcinfo);
//    GrSetFocus(btn_attrOK->wid);
//    
//    tnRegisterCallBack(btn_attrOK, WIDGET_CLICKED,click_attrOK,NULL);
//}
//
//
//void click_attrOK(TN_WIDGET* pwidget,DATA_POINTER p)
//{	
//    tnDestroyWidget(attr_window);
//    GrSetFocus(listbox->wid);
//    tnSetSelectedListItemAt(listbox,cur_item,1);
//}
//
//
//void click_copytoXXX(TN_WIDGET* pwidget,DATA_POINTER p)
//{	
//    char target_path[256],source_path[256];
//    char t[256]="/";
//    strcpy(source_path,path);
//    strcat(source_path,strcat(t,cur_file));	
//    //strcat(target_path,strcat(music_path,cur_file));
//    
//    switch((int)p)
//    {
//        case 0:
//            strcpy(target_path,music_path);
//            strcat(target_path,cur_file);
//            copyfile(source_path,target_path);
//            break;
//        case  1:
//            strcpy(target_path,pic_path);
//            strcat(target_path,cur_file);
//            copyfile(source_path,target_path);
//            break;
//        case	2:
//            strcpy(target_path,txt_path);
//            strcat(target_path,cur_file);
//            copyfile(source_path,target_path);
//            break;
//    }
//    	
//    tnDestroyWidget(copyto_window);
//    GrSetFocus(listbox->wid);
//    tnSetSelectedListItemAt(listbox,cur_item,1);
//}
//
//
//int DestroyFileMenu()
//{
//    tnDestroyWidget(file_oper);
//    file_oper=NULL;
//    return 0;
//}
//
//int CreateYesNo()
//{
//    //GR_RECT r={300,250,200,100};
//    yesno_window = CreateWindowFromRC(NULL,TN_CONTROL_FLAGS_DEFAULT,2,&rcinfo);
//    btn_yes = CreateButtonFromRC(yesno_window,TN_CONTROL_FLAGS_DEFAULT,4,&rcinfo);
//    btn_no = CreateButtonFromRC(yesno_window,TN_CONTROL_FLAGS_DEFAULT,5,&rcinfo);
//    
//    lbl_issure=CreateLabelFromRC(yesno_window,TN_CONTROL_FLAGS_DEFAULT,0,&rcinfo);
//    
//    tnRegisterCallBack(btn_yes, WIDGET_KEYDOWN,keydown_yesno,NULL);
//    tnRegisterCallBack(btn_no,  WIDGET_KEYDOWN,keydown_yesno,NULL);
//    tnRegisterCallBack(btn_yes, WIDGET_CLICKED,click_yes,NULL);
//    tnRegisterCallBack(btn_no,  WIDGET_CLICKED,click_no,NULL);	
//    //GrText(yesno_window->wid,yesno_window->gc,10,30,"Be sure to delete this file ??",32,0);
//}
//
//void keydown_copytoXXX(TN_WIDGET* pwidget,DATA_POINTER p)
//{
//    long ch;
//    TN_WIDGET*	pw;
//    ch = (long)pwidget->data;
//    
//    if (ch == MWKEY_LEFT || ch==MWKEY_UP)
//    {
//        if(pwidget == btn_music)
//            pw = btn_txt;
//        else
//            pw = GetPrevSibling(pwidget);
//        GrSetFocus(pw->wid);	
//        return;
//    }
//    if (ch == MWKEY_RIGHT || ch==MWKEY_DOWN)
//    {
//        if(pwidget == btn_txt)
//            pw = btn_music;
//        else
//            pw = GetNextSibling(pwidget);
//        GrSetFocus(pw->wid);	
//        return;
//    }	
//    
//}
//
//void keydown_yesno(TN_WIDGET* pwidget,DATA_POINTER p)
//{	
//    long ch;
//    ch = (long)pwidget->data;
//    if (ch == MWKEY_LEFT || ch==MWKEY_UP ||ch == MWKEY_RIGHT || ch==MWKEY_DOWN)
//    {
//        if(pwidget == btn_yes)
//            GrSetFocus(btn_no->wid);
//        else
//            GrSetFocus(btn_yes->wid);
//        return;
//    }	
//    
//}
//
//
//void click_yes()
//{
//    DestroyYesNo();
//    char **str;
//    char t[128]="/";
//    int m;
//    char file_path[128]="";
//    strcpy(file_path,path);
//    
//    strcat(file_path,strcat(t,cur_file));	
//    delfile(file_path);
//        
//    GrSetFocus(listbox->wid);
//    tnSetSelectedListItemAt(listbox,0,1);
//    //listfunc(listbox,NULL);
//    
//}
//
//void click_no()
//{
//    DestroyYesNo();
//    GrSetFocus(listbox->wid);
//    tnSetSelectedListItemAt(listbox,cur_item,1);
//}
//
//
//
//int DestroyYesNo()
//{
//    tnDestroyWidget(yesno_window);
//    yesno_window=NULL;
//    return 0;
//}

