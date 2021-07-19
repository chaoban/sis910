#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>
#include <cyg/fileio/fileio.h>
#include <cyg/fs/fatfs.h>
#include <dirent.h>
#include "global.h"
#include "browser.h"
#include "device.h"
//#include "../files/rc_mount.h"

#ifdef CYGPKG_DEVS_DISK_SD_SIS910
#define  USE_SDDISK
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>
#endif


static GR_IMAGE_ID device_icon, nodevice_icon, folder_icon, pp_icon, txt_icon, unknown_icon, music_icon;
struct menuReturnData closedata;
RCINFO gfbrowserRcinfo;
char * gCurrentPath;

TN_WIDGET *gwinFileOpertion;

static tFileBrowser gpFileBrowser;

int(* gSelector)(char *);

/*****************************************************************************
	Return Filename lists from directory.
******************************************************************************/	
int GetFileList(char *path,char ***fi, int(* selector)(char *))
{
    //fi = (FileInfo*)malloc(10*sizeof(FileInfo));
#ifdef LONG_NAME_SUPPORT
    struct vfat_descriptor tr,*ptr;
    int i = 0, j =0;
    
    int num_files = listdir(path,&tr);
    
    printf("GetFileList %s %d\n",path, num_files);
    
    if(num_files == 0) return -1;
    
    ptr = &tr;
    (*fi)=(char**)malloc(num_files*sizeof(char*));
    
    if(fi==NULL)
    {
        printf("In global.c not enough memory\n");
        return -1;	
    }
    j = 0;    
    for(i = 0;i<num_files;i++)
    {	
        //printf("Files %s %d\n",tr.filename, i);
        if(tr.filename)
        {
            if(selector== NULL || (*selector) (tr.filename))
            {
                (*fi)[j++] = strdup(tr.filename);
            }            
            tr = *tr.next;
        }                       
        else
            break;
    }
                            
        
    close_listdir(ptr);
    return j;
#else		
    struct dirent *pd;
    int i=0;
    DIR *pdir = opendir(path);
    int num_files=0;
    
    if(!pdir)
    {
        printf("open dir failed!!\n");
        return -1;	
    }			
    
    while(  (pd = readdir(pdir))!=NULL)
        num_files++;
    
    (*fi)=(char**)malloc(num_files*sizeof(char*));
    if(fi==NULL)
    {
        printf("In fb.c not enough memory\n");
        return -1;	
    }
    
    closedir(pdir);
    pdir=opendir(path);
    
    for(i=0;i<num_files;i++)
    {	
        pd = readdir(pdir);
        int l = strlen(pd->d_name);
        (*fi)[i] = (char*)malloc((l+1)*sizeof(char));
        if((*fi)[i] == NULL)
        {
            printf("can't alloc!!!!\n");
            closedir(pdir);
            return -1;
        }
        strcpy( (*fi)[i],pd->d_name); 				
    }
    //end LONG_NAME_SUPPORT
    closedir(pdir);
    return num_files;
#endif
}

int FreeFileList(char **fi,int n)
{
    int i=0;
    for(;i<n;i++)
        free(	fi[i]	);
    
    free(fi);    
    return 0;
}

int IsDIR(char *path)
{
    DIR *pdir = opendir(path);

    if(pdir)
    {
        closedir(pdir);
        return 1;
    }
    return 0;
		
}

/**
 *\brief get full name  of a gave name. 
 *\author 
 *\param name :the file name[input]
 *\param fullname:the full name of the file[output] 
 *\date 2008-04-07
 *\return
 */
void GetFullFilename(char *dir,char *filename, char *fullname)
{
    memset(fullname,0,strlen(fullname));
    
    if( !strcmp(filename,"..") ) 
        //strcpy(fullname,dir);
        GetPath(dir, fullname);               
    else if( !strcmp(dir,"/") )
    {        
        strcpy(fullname,filename);
    }    
    else
    {        
        strcpy(fullname,dir);
        strcat(fullname,"/");
        strcat(fullname,filename);
    }
    
    //printf("GetFullFilename %d:%s\n",strlen(fullname), fullname);
    
    return;
}

/**
 *\brief get the path name from a file's full name
 *\author xy
 *\param  fullname:[input]the fullname of a file
 *\param  path:[output]the path of a file
 *\date 2008-04-07
 *\return
 */
int GetPath(char *fullname,char *path)
{
    int length;
    char *p;
    int i = 0;
    
    length=strlen(fullname);
    memset(path,0,strlen(path));
    
    if(length <= 0)
        return 0;
        
    p = &fullname[length-1];
    while( (length!=0)&&(*p)!='/')
    {
        length--;
        p--;
    }
    for( i = 0; i < length - 1; i++)
        path[i] = fullname[i];
    
    path[i] = 0;
        
    return length;
    
}

GR_IMAGE_ID selectAudio(char *str)
{
    if( strstr(str,".MP3") || strstr(str,".WAV") || strstr(str,".mp3") || strstr(str,".wav"))
	    return music_icon;	    
	else
	    return unknown_icon;
}

GR_IMAGE_ID selectImage(char *str)
{
    if( strstr(str,".BMP") || strstr(str,".JPG") || strstr(str,".bmp") || strstr(str,".jpg"))
    {
       // diag_mallocinfo();
	    return GrLoadImageFromFile(str,0);	    
	}
	    
	return 0;
}					    
			
GR_IMAGE_ID selectAllType(char *str)
{   
    if( strstr(str,".BMP") || strstr(str,".JPG") || strstr(str,".bmp") || strstr(str,".jpg"))
    {
        return GrLoadImageFromFile(str,0);		        
    }
    else if( strstr(str,".TXT") || strstr(str,".txt"))
        return txt_icon;
    else if(strstr(str,"MP3") || strstr(str,"mp3"))
        return music_icon;        
    else
        return unknown_icon;	    
}					    

//extern STORAGEDEVICE gStorageDevice[4];

tFileBrowser* OpenFileBrowser(TN_WIDGET *pwidget, char *rcfilepath, char *currentDir,
     int(* selector)(char *),void(*clicked)(TN_WIDGET *,DATA_POINTER),
     void (*keydown)(TN_WIDGET *,DATA_POINTER),void (*escape)(TN_WIDGET *,DATA_POINTER))
{
    
    char ** filelist;
    char tempStr[128];
    char rcfileDir[128];
    int i = 0;
    int num_file = 0;
        
    printf("OpenFileBrowser focus id\n");
    if((gpFileBrowser.pwndBrowser != NULL)&&GsFindWindow(gpFileBrowser.pwndBrowser->wid))
    {
        printf("double clicking BrowserClicked()%d\n",gpFileBrowser.pwndBrowser->wid);
        return NULL;
    }
    
    if(load_rc(&gfbrowserRcinfo,rcfilepath, "en")<0)
    //if(load_rc(&gsettingRcinfo,rcfilepath, lang)<0)
    {
        printf("Load rc file failure\n");
        return NULL;
    }
    printf("OpenFileBrowser focus id %x\n",pwidget->wid);
    closedata.wFocus = pwidget;          
    closedata.prcinfo = &gfbrowserRcinfo;
    gpFileBrowser.pwndBrowser = CreateWindowFromRC(NULL,TN_CONTROL_FLAGS_DEFAULT, ID_WND_FBROWSER,&gfbrowserRcinfo);
	gpFileBrowser.plsbFiles = CreateListBoxFromRC(gpFileBrowser.pwndBrowser,TN_CONTROL_FLAGS_DEFAULT, ID_LBX_FILE,&gfbrowserRcinfo);
	
	if(GetPath(rcfilepath,rcfileDir) < 0) 
	{
	    printf("fail to get rcfileDir from %s\n",rcfilepath);
	    return NULL;
	}
	
	GetFullFilename(rcfileDir,"device.jpg",tempStr);
	device_icon = GrLoadImageFromFile(tempStr,0);
    GetFullFilename(rcfileDir,"nodevice.jpg",tempStr);
	nodevice_icon = GrLoadImageFromFile(tempStr,0);	
	GetFullFilename(rcfileDir,"folder.jpg",tempStr);
	folder_icon = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(rcfileDir,"music.jpg",tempStr);
	music_icon	= GrLoadImageFromFile(tempStr,0);
	GetFullFilename(rcfileDir,"txt.jpg",tempStr);
	txt_icon	= GrLoadImageFromFile(tempStr,0);
	GetFullFilename(rcfileDir,"unknown.jpg",tempStr);
	unknown_icon = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(rcfileDir,"pp.jpg",tempStr);
	pp_icon	= GrLoadImageFromFile(tempStr,0);
	
	gCurrentPath = currentDir;
	gSelector = selector;		
	if( !strcmp(currentDir,"/") )
	{
	    checkDevice();
	
	    for(i = 0; i < 4; i++)
	    {   
	        if(gStorageDevice[i].status) 
	            tnAddItemToListBox(gpFileBrowser.plsbFiles, gStorageDevice[i].devicename,device_icon);
	        else
	            tnAddItemToListBox(gpFileBrowser.plsbFiles, gStorageDevice[i].devicename,nodevice_icon);	    
	    }
	}
	else	
	{
	    //MountSDisk();
        printf("OpenFileBrowser GetFileList()\n");
	    int i = 0;
	    GR_IMAGE_ID tempID;	    
	    num_file = GetFileList( currentDir, &filelist, NULL);	    
	    printf("OpenFileBrowser GetFileList() end\n");

	    for(i = 0; i < num_file; i++)
	    {
	        char p[128];
		    GetFullFilename(currentDir,filelist[i],p);
 
            if(!IsDIR(p) )
			{
			    tempID = gSelector(p);
			    if(tempID)
			        tnAddItemToListBox(gpFileBrowser.plsbFiles,filelist[i],tempID);			            
			}
			else
			{
			    if(( strcmp(filelist[i],".")) &&( strcmp(filelist[i],"..")))
			        tnAddItemToListBox(gpFileBrowser.plsbFiles,filelist[i],folder_icon);
			}
			               

	    }
	    
	    tnAddItemToListBoxAt(gpFileBrowser.plsbFiles,"..",0,folder_icon);
	    
	    FreeStrList(filelist,num_file);
	}	

	
	GrSetFocus(gpFileBrowser.plsbFiles->wid);
	tnSetSelectedListItemAt(gpFileBrowser.plsbFiles,0,GR_TRUE);
    
	tnRegisterCallBack(gpFileBrowser.plsbFiles,WIDGET_CLICKED,OnBrowserClicked,(DATA_POINTER)(*clicked));
	tnRegisterCallBack(gpFileBrowser.plsbFiles,WIDGET_KEYDOWN,(CallBackFuncPtr)(*keydown),NULL);

    if(escape != NULL)
    {
	    tnRegisterCallBack(gpFileBrowser.plsbFiles,WIDGET_ESCAPED,(CallBackFuncPtr)(*escape),(DATA_POINTER) &closedata);	
	    tnRegisterCallBack(gpFileBrowser.pwndBrowser,WIDGET_ESCAPED,(CallBackFuncPtr)(*escape),(DATA_POINTER) &closedata);
    }
    else
    {
        tnRegisterCallBack(gpFileBrowser.plsbFiles,WIDGET_ESCAPED,(CallBackFuncPtr)OnBrowserClosed,(DATA_POINTER) &closedata);	
	    tnRegisterCallBack(gpFileBrowser.pwndBrowser,WIDGET_ESCAPED,(CallBackFuncPtr)OnBrowserClosed,(DATA_POINTER) &closedata);
    }

	GrSetFocus(gpFileBrowser.plsbFiles->wid);
	
	return &gpFileBrowser;
}

void OnBrowserClosed(TN_WIDGET* pwidget,DATA_POINTER p)
{
    
    struct menuReturnData *data = NULL;
    
    TN_WIDGET *parent = NULL, *focus = NULL;
    RCINFO *prcinfo = NULL;    
    
    if(pwidget->type == TN_WINDOW)
        parent = pwidget;
    else
        parent=GetParent(pwidget);

    tnDestroyWidget(parent);
    printf("OnBrowserClosed %d\n", parent->wid);
    gpFileBrowser.pwndBrowser = NULL;
    
     if (p != NULL)
    {
        data = (struct menuReturnData *) p;
        focus = data->wFocus;
        printf("OnMenuEscape focus id %x\n",focus->wid);
        if(focus != NULL)
        {
            
            if ((focus->type==TN_BUTTON) &&(!focus->enabled))
                focus->enabled=GR_TRUE;        
            GrSetFocus(focus->wid);
        }
                 
        prcinfo = data->prcinfo;
        
        if(prcinfo != NULL)
        {            
            destroy_rc(prcinfo);//Must be excuted last.
        }        

    }
}

char gselectfile[128];
void OnBrowserClicked(TN_WIDGET *widget, DATA_POINTER data)
{
    char **filename;
    int nCount = 0, i = 0;    
        
    tnGetSelectedListItems(widget, &filename, &nCount);	
    
    if( nCount < 1) return;
	
	GetFullFilename(gCurrentPath,filename[0],gselectfile);		
	printf("%s - gCurrentPath %s, gselectfile %s\n",__FUNCTION__,gCurrentPath, gselectfile);
    
    if((!strcmp(filename[0],"..")) && (strlen(gselectfile)==0))
    {
        strcpy(gCurrentPath, "/");
        tnDeleteAllItemsFromListBox(widget);
        checkDevice();
	
	    for(i = 0; i < 4; i++)
	    {   
	        if(gStorageDevice[i].status) 
	            tnAddItemToListBox(gpFileBrowser.plsbFiles, gStorageDevice[i].devicename,device_icon);
	        else
	            tnAddItemToListBox(gpFileBrowser.plsbFiles, gStorageDevice[i].devicename,nodevice_icon);	    
	    }
	    
	    tnSetSelectedListItemAt(widget,0,GR_TRUE);
    }
	else if(!IsDIR(gselectfile))
	{	    	    
	    if(data)
	    {
	        EventDispatchFuncPtr pFunc;
	        pFunc = (EventDispatchFuncPtr) data;
	        pFunc( widget, NULL);
	    }
	    else
	    {
	        TN_WIDGET *file_open, *file_copy, *file_del, *file_attr;
            //Check for reduplicate clicking
            if(GsFindWindow(gwinFileOpertion->wid))
            {
                return;
            }
  	        
            gwinFileOpertion = CreateWindowFromRC(NULL,0,ID_WND_FILEOP,&gfbrowserRcinfo);
            
            file_open = CreateButtonFromRC(gwinFileOpertion,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILEOPEN,&gfbrowserRcinfo);
            file_copy = CreateButtonFromRC(gwinFileOpertion,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILECOPY,&gfbrowserRcinfo);
            file_del  = CreateButtonFromRC(gwinFileOpertion,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILEDELETE,&gfbrowserRcinfo);
            file_attr = CreateButtonFromRC(gwinFileOpertion,TN_CONTROL_FLAGS_DEFAULT,ID_BTN_FILEATTRIBUTE,&gfbrowserRcinfo);
            
            tnRegisterCallBack(file_open,WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
            tnRegisterCallBack(file_copy,WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
            tnRegisterCallBack(file_del, WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
            tnRegisterCallBack(file_attr,WIDGET_KEYDOWN,(CallBackFuncPtr)OnControlFocusExchange,NULL);
            
            tnRegisterCallBack(file_open,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,widget);
            tnRegisterCallBack(file_copy,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,widget);
            tnRegisterCallBack(file_del, WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,widget);
            tnRegisterCallBack(file_attr,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,widget);
            	
            tnRegisterCallBack(file_open,WIDGET_CLICKED, OnFileOpen,NULL);	
            //tnRegisterCallBack(file_del,  WIDGET_CLICKED,click_del,NULL);
            //tnRegisterCallBack(file_copy, WIDGET_CLICKED,click_copy,NULL);
            //tnRegisterCallBack(file_attr,WIDGET_CLICKED, click_attr,NULL);
            
            GrSetFocus(file_open->wid);
	    }
	    
	}
	else
	{
	    int num_file = 0;
	    char ** filelist;
	    GR_IMAGE_ID tempID;
	
	    memset(gCurrentPath, 0 , strlen(gCurrentPath));
	    strcpy(gCurrentPath, gselectfile);	    	    
	    printf("%s - first gCurrentPath %s, gselectfile %s\n",__FUNCTION__,gCurrentPath, gselectfile);
	    num_file = GetFileList( gCurrentPath, &filelist, NULL);	    
	    
	    tnDeleteAllItemsFromListBox(widget);

	    for(i = 0; i < num_file; i++)
	    {
	        char p[128];		        		    
            if(( strcmp(filelist[i],".")) &&( strcmp(filelist[i],"..")))
            {
                GetFullFilename(gCurrentPath,filelist[i],p);
                
                if(!IsDIR(p) )
			    {
			        tempID = gSelector(p);
			        if(tempID)
			        {
			            //diag_mallocinfo();
			            tnAddItemToListBox(widget,filelist[i],tempID);			            
			        }
			    }
			    else
			    {
			        tnAddItemToListBox(gpFileBrowser.plsbFiles,filelist[i],folder_icon);
			    }
		    }
			    
	        //if( !strcmp(filelist[i],".") )
			//	tnAddItemToListBoxAt(widget,filelist[i],0,folder_icon);				
			//else if(!strcmp(filelist[i],"..") )
			//	tnAddItemToListBoxAt(widget,filelist[i],1,folder_icon);
			//else
			//{			    
			//    if(!IsDIR(p) )
			//    {
			//        tempID = gSelector(p);
			//        if(tempID)
			//            tnAddItemToListBox(widget,filelist[i],tempID);			            
			//    }
			//    else
			//        tnAddItemToListBoxAt(widget,filelist[i],2,folder_icon);
			//}
	    }
	    tnAddItemToListBoxAt(gpFileBrowser.plsbFiles,"..",0,folder_icon);
	    tnSetSelectedListItemAt(widget,0,GR_TRUE);
	    FreeStrList(filelist,num_file);		    
    }
    
    FreeStrList( filename,nCount);    
}

int nOffsetX = 0, nOffsetY = 0;
TN_WIDGET *gpwndPicViewer;
TN_WIDGET *gppicViewer;

void OnFileOpen(TN_WIDGET *widget, DATA_POINTER data)
{
    printf("OnFileOpen %s\n",gselectfile);    
 
    //Check for reduplicate clicking
    if(GsFindWindow(gpwndPicViewer->wid))
    {
        printf("double clicking OnFileOpen()\n");
        return;
    }
         
    gpwndPicViewer = CreateWindowFromRC(NULL,0,ID_WND_PICVIEWER,&gfbrowserRcinfo);
    gppicViewer = CreatePictureFromRC(gpwndPicViewer,0,ID_PIC_DEMO,&gfbrowserRcinfo); 
    
    tnRegisterCallBack(gppicViewer,WIDGET_KEYDOWN,(CallBackFuncPtr)OnImageZoom,NULL);
    
    tnRegisterCallBack(gpwndPicViewer,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,widget);
    tnRegisterCallBack(gppicViewer,WIDGET_ESCAPED,(CallBackFuncPtr)OnSubMenuEscape,widget);
    
    tnSetPicture(gppicViewer ,gselectfile);
    tnSetPictureStretch(gppicViewer, 1, 100, 0, 0);
    
    GrSetFocus(gpwndPicViewer->wid); 
}

void OnImageZoom(TN_WIDGET *widget, DATA_POINTER data)
{
    TN_WIDGET *focus=widget;
    GR_KEY ch;
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_VALDOWN:
            nOffsetX -=20;
            break;
        case DPFKEY_VALUP:
            nOffsetX +=20;
            break;
        case DPFKEY_PREV:
            nOffsetY -=20;
            break;
        case DPFKEY_NEXT:
            nOffsetY +=20;
            break;
        case MWKEY_ENTER:
            tnSetPictureStretch(gppicViewer, 1, 200, 0, 0);
            break;
        default:
            return;
     }
     
    tnSetPictureStretch(gppicViewer, 1, 200, nOffsetX, nOffsetY);
}



