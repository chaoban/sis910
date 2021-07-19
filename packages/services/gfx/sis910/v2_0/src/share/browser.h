#ifndef __BROWSER_H
#define __BROWSER_H
#include "tinywidget/tnWidgets.h"


//button.
#define ID_BTN_FILEOPEN 0
#define ID_BTN_FILECOPY 1
#define ID_BTN_FILEDELETE 2
#define ID_BTN_FILEATTRIBUTE 3
#define ID_BTN_OP_YES 4
#define ID_BTN_OP_NO 5
#define ID_BTN_MUSIC 6
#define ID_BTN_PICTURE 7
#define ID_BTN_BOOK 8
#define ID_BTN_OK 9

//check button.

//radio button.

//label.
#define ID_LBL_YES_NO 0
#define ID_LBL_FILE_SIZE 1
#define ID_LBL_FILE_SIZE1 2
#define ID_LBL_FILE_TYPE 3
#define ID_LBL_FILE_TYPE1 4

//list box.
#define ID_LBX_FILE 0

//progress bar.
#define ID_PIC_DEMO 0

//window.
#define ID_WND_FBROWSER 0
#define ID_WND_FILEOP 1
#define ID_WND_YESNO 2
#define ID_WND_FILE_ATTRIBUTE 3
#define ID_WND_MARK 4
#define ID_WND_PICVIEWER 5

//#define TYPE_IMAGE 1
//#define TYPE_ICON 2
#define BROWSER_NAME  "FBrowse"


typedef struct
{
    TN_WIDGET *pwndBrowser;
    TN_WIDGET *plsbFiles;
}tFileBrowser;

int GetFileList(char *path,char ***fi, int(* selector)(char *));
int FreeFileList(char **fi,int n);
int IsDIR(char *path);
void GetFullFilename(char *dir,char *filename, char *fullname);
int GetPath(char *fullname,char *path);
GR_IMAGE_ID selectImage(char *str);
GR_IMAGE_ID selectAudio(char *str);
GR_IMAGE_ID selectAllType(char *str);

tFileBrowser * OpenFileBrowser(TN_WIDGET *pwidget, char *rcfilepath, char *currentDir,
     int(* selector)(char *),void(*clicked)(TN_WIDGET *,DATA_POINTER),
     void (*keydown)(TN_WIDGET *,DATA_POINTER),void (*escape)(TN_WIDGET *,DATA_POINTER));
     
void OnBrowserClosed(TN_WIDGET* pwidget,DATA_POINTER p);
void OnBrowserClicked(TN_WIDGET *widget, DATA_POINTER data);
void OnFileOpen(TN_WIDGET *widget, DATA_POINTER data);
void OnImageZoom(TN_WIDGET *widget, DATA_POINTER data);
     

#endif //__BROWSER_H
