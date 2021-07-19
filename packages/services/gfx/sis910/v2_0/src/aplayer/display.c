/**
 *\file display.c
 *\brief the display module. This moudle use the wedgets offered by control to display the GUI.
 *\author yifj
 *\date 2008-04-07
 *
 *$Id$
 */
#define MWINCLUDECOLORS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tnWidgets.h"
#include "nano-X.h"
#include "common.h"
#include "display.h"
#include "handle.h"
#include "assert.h"
#include "resource.h"
#include "engine.h"
#include "../config/config.h"
#include "../desktop/global.h"

///// function declare ////
static void DispEndWindow();
static void DispHideWindow();
static void DispShowWindow();
static void DispSubject(char *SubjectName, char *LangName);
static int DispPPBtnAble();
static void DispGetTime(char *time_b);
static void DispIconInit();
static void DispIconDestroy();
/////   over /////

static RCINFO APlayerRC;
static TN_WIDGET *gpMainWindow = NULL, *gpWid = NULL, *gpNavBtn[DISP_NAV_BTN_NUM] = { NULL }, *gpExtBtn[DISP_EXT_BTN_NUM] = {
NULL},
    *gpPlayProgBar = NULL, *gpVolProgBar = NULL,
    *gpPlayListWid = NULL, *gpPlayListBox = NULL,
    *gpPlayItemWid = NULL, *gpPlayItemPlay = NULL, *gpPlayItemDel = NULL, *gpPlayItemUp = NULL, *gpPlayItemDown = NULL, *gpPlayItemDelALL = NULL,
    *gpPlayMode = NULL,
    *gpId3Song = NULL, *gpId3Singer = NULL, *gpId3Album = NULL, *gpPosLabel = NULL, *gpTimeLabel = NULL,
    *gpSongLabel = NULL, *gpFavLabel = NULL, *gpFbLabel = NULL, *gpGlobalTime = NULL,
    *gpFbWid = NULL, *gpFbList = NULL, *gpFbItemWid = NULL, *gpFbItemPlay = NULL, *gpFbItemAdd = NULL;
static GR_TIMER_ID gTimerId = 0;
static GR_IMAGE_ID gImgFolder = 0, gImgMusic = 0;

/**
 *\brief this function is default callback function for direction keys. It makes the the focus swith among buttons and check buttons
 *\author yifj
 *\param widget the widget for which this callback function is registed.
 *\param ptr a pointer to input data, no use in this function.
 *\date 2008-04-07
 *\return
 */
static void DispKeyMove(TN_WIDGET * widget, DATA_POINTER ptr)
{
    TN_WIDGET *pWidget = NULL;
    long ch;
    TN_WIDGET_TYPE type;
    ch = (long) widget->data;

    if (ch == MWKEY_LEFT) {
        pWidget = GetPrevSibling(widget);
        type = pWidget->type;
        if ((type != TN_BUTTON) && (type != TN_CHECKBUTTON)) {
            pWidget == widget;
            do {
                widget = pWidget;
                pWidget = GetNextSibling(widget);
                type = pWidget->type;
            }
            while ((type == TN_BUTTON) || (type == TN_CHECKBUTTON));
            pWidget = widget;
        }
        GrSetFocus(pWidget->wid);
    }
    if (ch == MWKEY_RIGHT) {
        pWidget = GetNextSibling(widget);
        type = pWidget->type;
        if ((type != TN_BUTTON) && (type != TN_CHECKBUTTON)) {
            pWidget == widget;
            do {
                widget = pWidget;
                pWidget = GetPrevSibling(widget);
                type = pWidget->type;
            }
            while ((type == TN_BUTTON) || (type == TN_CHECKBUTTON));
            pWidget = widget;
        }
        GrSetFocus(pWidget->wid);
    }
}

/**
 *\brief this function is default callback function for esc keys. It makes the the focus swith among buttons and check buttons
 *\author yifj
 *\param widget the widget for which this callback function is registed.
 *\param ptr a pointer to input data, no use in this function.
 *\date 2008-04-07
 *\return
 */
static void DispKeyEsc(TN_WIDGET * widget, DATA_POINTER ptr)
{
    //HandEventProc(HAND_EVENT_FB);
    TN_WIDGET *pParentWidget, *pRetWidget;
    TN_WIDGET_TYPE type;

    pRetWidget = (TN_WIDGET *) ptr;
    pParentWidget = GetParent(widget);
    type = pParentWidget->type;
    if (type == TN_WINDOW) {
        if (pParentWidget == gpWid) {
#ifdef __ECOS
            if (EngGetPlayState() == PLAYER_STOP) {
                HandMsgSend(HAND_MSG_EXIT);
                tnDestroyWidget(pParentWidget);
                DispEndWindow();
            } else {
                GrSetFocus(gpNavBtn[NAV_BTN_PLAY_PAUSE]->wid);
                DispHideWindow();
                HandMsgSend(HAND_MSG_BG);
            }
#else
            if (EngGetPlayState() == PLAYER_STOP) {
                //HandMsgSend(HAND_MSG_EXIT);
                tnDestroyWidget(pParentWidget);
                DispEndWindow();
            } else {
                GrSetFocus(gpNavBtn[NAV_BTN_PLAY_PAUSE]->wid);
                DispHideWindow();
                //HandMsgSend(HAND_MSG_BG);
            }
#endif //__ECOS
            return;
        }

        tnDestroyWidget(pParentWidget);
        if (pRetWidget != NULL)
            GrSetFocus(pRetWidget->wid);
    }

    else
        assert(0);
}

///////  pic //////

/**
 *\brief create the play mode picture 
 *\author yifj
 *\param parent the point to the parent widget
 *\param leftx the X-coord of left up point of the picture 
 *\param topy the Y-coord of left up point of the picture
 *\param width the width of the picture
 *\param height the height of the picture
 *\param fname the file name of displing picture.
 *\date 2008-04-07
 *\return
 */
static int DispDrawPic(TN_WIDGET * parent, int leftx, int topy, int width, int height, char *fname)
{
    GR_RECT rect;

    rect.x = leftx;
    rect.y = topy;
    rect.width = width;
    rect.height = height;

    gpPlayMode = CreatePicture(parent, 0, &rect, fname, GR_FALSE);
    if (gpPlayMode == NULL) {
        assert(0);
        return AUDIO_FALSE;
    }
    return AUDIO_TRUE;
}

/**
 *\brief change the picture of playmode according to the assigned PlayMode
 *\author yifj
 *\param PlayMode tha play mode need to be set
 *\date 2008-04-07
 *\return
 */
int DispModeSet(tPlayMode PlayMode)
{
    char FilePath[MAX_FILE_PATH];

    switch (PlayMode) {
        case NO_CIRCUS:
            strcpy(FilePath, MODE_PIC_NO_PATH);
            ComPathReltoAbs(FilePath);
            tnSetPicture(gpPlayMode, FilePath);  //temp
            break;
        case REPEAT_CURRENT:
            strcpy(FilePath, MODE_PIC_REP_PATH);
            ComPathReltoAbs(FilePath);
            tnSetPicture(gpPlayMode, FilePath);  //temp
            break;
        case CIRCUS_AROUND:
            strcpy(FilePath, MODE_PIC_CIR_PATH);
            ComPathReltoAbs(FilePath);
            tnSetPicture(gpPlayMode, FilePath);  //temp
            break;
    }
    return AUDIO_TRUE;
}

///////// pic over //////

///////// label////////
/**
 *\brief set the content of the ID3 labels
 *\author yifj
 *\param Id3 the Id3 information need to be set
 *\date 2008-04-07
 *\return
 */
int DispID3Set(tId3 Id3)
{
#if 1
    char temp[40];              //MAX_SINGER_NAME+9
    temp[39] = 0;
    
    //   GrSetGCFont(gpId3Song->gc, GrCreateFont("SystemFixed", 20, NULL));
    //   GrSetGCForeground(gpId3Song->gc, RED);

    strcpy(temp, "Song: ");
    strcat(temp, Id3.SongName);
    //mset(temp+strlen(temp), ' ', 40-1-strlen(temp));
    tnSetLabelCaption(gpId3Song, temp);

    strcpy(temp, "Album: ");
    strcat(temp, Id3.AlbumName);
    //mset(temp+strlen(temp), ' ', 40-1-strlen(temp));
    tnSetLabelCaption(gpId3Album, temp);

    strcpy(temp, "Singer: ");
    strcat(temp, Id3.SingerName);
    //mset(temp+strlen(temp), ' ', 40-1-strlen(temp));
    tnSetLabelCaption(gpId3Singer, temp);
#else
    tnSetLabelCaption(gpId3Song, "song~~~");
    tnSetLabelCaption(gpId3Album, "album~~~");
    tnSetLabelCaption(gpId3Singer, "singer~~~");
#endif
    return AUDIO_TRUE;
}


/**
 *\brief set the label, which indication the order of playing song and the total number of songs in the playlist or browser
 *\author yifj
 *\param Cur
 *\param Sum
 *\date 2008-04-07
 *\return
 */
static int DispSongSet(int Cur, int Sum)
{
    char SongLableStr[20];
    sprintf(SongLableStr, "%d/%d", Cur, Sum);
    tnSetLabelCaption(gpSongLabel, SongLableStr);
    return AUDIO_TRUE;
}

/**
 *\brief set the play_pause_button's states, which is a check button.
 *\author yifj
 *\param event HAND_EVENT_PLAY:button unchecked, HAND_EVENT_PAUSE: button checked.
 *\date 2008-04-07
 *\return
 */
int DispBtnPPSet(tHandEvent Event)
{
    if (Event == HAND_EVENT_PLAY)
        tnSetCheckButtonStatus(gpNavBtn[NAV_BTN_PLAY_PAUSE], GR_FALSE);
    else if (Event == HAND_EVENT_PAUSE)
        tnSetCheckButtonStatus(gpNavBtn[NAV_BTN_PLAY_PAUSE], GR_TRUE);
    else
        assert(0);
    return AUDIO_TRUE;
}

////////label over///////

//////////////////////////////////////////////////////////////////////
/**
 *\brief the callback function of playlist(favorite songs) operations
 *\author yifj
 *\param widget the point to the widget which register the callback function
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispPlayitemHand(TN_WIDGET * widget, DATA_POINTER ptr)
{
    tHandEvent event = (tHandEvent) ptr;
    if ((event == HAND_EVENT_PITEM_PLAY)
        || (event == HAND_EVENT_PITEM_RM)
        || (event == HAND_EVENT_PITEM_RM_ALL)) {
        HandEventProc(event);
        DispKeyEsc(widget, NULL);
    } else if ((event == HAND_EVENT_PITEM_UP) || (event == HAND_EVENT_PITEM_DOWN))
        HandEventProc(event);
    else
        EPRINTF(("playitem get error event! \n"));
}

/**
 *\brief delete a playitem from playlist.
 *\author yifj
 *\param pDelNum the assigned position of playitem which need to be delete
 *\date 2008-04-07
 *\return
 */
int DispDelPlayitem(int *pDelNum)
{
    int count, *pPos = NULL, **ppPos;
    ppPos = &pPos;

    tnGetSelectedListPos(gpPlayListBox, ppPos, &count);
    if (count != 1) {
        assert(0);
        return AUDIO_FALSE;
    }
    *pDelNum = *pPos;
    if (pPos != NULL)
        free(pPos);             //there is a threat for men leak in tnGetSelectedListPos
    tnDeleteItemFromListBoxAt(gpPlayListBox, *pDelNum);
    if (tnSetSelectedListItemAt(gpPlayListBox, *pDelNum, GR_TRUE) == -1)
        tnSetSelectedListItemAt(gpPlayListBox, *pDelNum - 1, GR_TRUE);
    //set focus to his parent or lower layer window.
    return AUDIO_TRUE;
}

/**
 *\brief add a playitem to playlist.
 *\author yifj
 *\param FileName[] a file name of playitem which need to be added
 *\date 2008-04-07
 *\return
 */
int DispPLIAdd(char FileName[])
{
    tnAddItemToListBox(gpPlayListBox, FileName, gImgMusic);
    return AUDIO_TRUE;
}

/**
 *\brief select a specific playitem
 *\author yifj
 *\param num the assined number of playitem, which need to be selected
 *\date 2008-04-07
 *\return
 */
int DispPLISetSel(int Num)
{
    tnSetSelectedListItemAt(gpPlayListBox, Num, GR_TRUE);
    return AUDIO_TRUE;
}

/**
 *\brief make a playitem  step foreward in order.
 *\author yifj
 *\param pSelNum the item number which need to be operated.
 *\date 2008-04-07
 *\return
 */
int DispUpPlayitem(int *pSelNum)
{
    int count, *pPos = NULL, **ppPos;
    char *pListInfo = NULL, **ppListInfo, ***pppListInfo;
    ppPos = &pPos;
    ppListInfo = &pListInfo;
    pppListInfo = &ppListInfo;

    tnGetSelectedListPos(gpPlayListBox, ppPos, &count);
    if (count != 1) {
        assert(0);
        return AUDIO_FALSE;
    }
    tnGetSelectedListItems(gpPlayListBox, pppListInfo, &count);
    *pSelNum = *pPos;
    if (pPos != NULL)
        free(pPos);
    if (*pSelNum <= 0) {
        return AUDIO_FALSE;
    }
#if 0
    tnDeleteItemFromListBoxAt(gpPlayListBox, *pSelNum);
    tnAddItemToListBoxAt(gpPlayListBox, *ppListInfo, *pSelNum - 1);
#else
    tnExchangeItems(gpPlayListBox, *pSelNum, *pSelNum - 1);
#endif
    ComFreeList(*pppListInfo, count);
    return AUDIO_TRUE;
}

/**
 *\brief make a playitem  step backward in order.
 *\author yifj
 *\param pSelNum the item number which need to be operated.
 *\date 2008-04-07
 *\return
 */
int DispDownPlayitem(int *pSelNum)
{
    int count, *pPos = NULL, **ppPos;
    char *pListInfo = NULL, **ppListInfo, ***pppListInfo;
    ppPos = &pPos;
    ppListInfo = &pListInfo;
    pppListInfo = &ppListInfo;

    tnGetSelectedListPos(gpPlayListBox, ppPos, &count);
    if (count != 1) {
        assert(0);
        return AUDIO_FALSE;
    }
    tnGetSelectedListItems(gpPlayListBox, pppListInfo, &count);
    *pSelNum = *pPos;
    if (pPos != NULL)
        free(pPos);
    if (*pSelNum >= (gpPlayListBox->WSpec.listbox.numitems - 1)) {
        return AUDIO_FALSE;
    }
#if 0
    tnDeleteItemFromListBoxAt(gpPlayListBox, *pSelNum);
    tnAddItemToListBoxAt(gpPlayListBox, *ppListInfo, *pSelNum + 1);
#else
    tnExchangeItems(gpPlayListBox, *pSelNum, *pSelNum + 1);
#endif
    ComFreeList(*pppListInfo, count);
    return AUDIO_TRUE;
}

/**
 *\brief get the item number of which is selected.
 *\author yifj
 *\param pSelNum a pointer to store the number
 *\date 2008-04-07
 *\return
 */
int DispGetSelPlayitem(int *pSelNum)
{
    int count, *pPos = NULL, **ppPos;
    ppPos = &pPos;

    tnGetSelectedListPos(gpPlayListBox, ppPos, &count);
    if (count != 1) {
        assert(0);
        return AUDIO_FALSE;
    }
    *pSelNum = *pPos;
    if (pPos != NULL)
        free(pPos);
    if (*pSelNum > (gpPlayListBox->WSpec.listbox.numitems - 1)) {
        return AUDIO_FALSE;
    }
    return AUDIO_TRUE;
}

/**
 *\brief the callback function of enter operation on a playitem.
 *\author yifj
 *\param widget the point to the widget which register the callback function 
 *\param ptr not used
 *\date 2008-04-07
 *\return
 */
static void DispPlayitemEnter(TN_WIDGET * widget, DATA_POINTER ptr)
{
    //gpPlayItemWid = CreateWindowFromRC(widget, 0, IDW_PIW, &APlayerRC);
    gpPlayItemWid = CreateWindowFromRC(NULL, 0, IDW_PIW, &APlayerRC);
    gpPlayItemPlay = CreateButtonFromRC(gpPlayItemWid, 0, IDB_PI_PLAY, &APlayerRC);
    gpPlayItemDel = CreateButtonFromRC(gpPlayItemWid, 0, IDB_PI_DEL, &APlayerRC);
    gpPlayItemUp = CreateButtonFromRC(gpPlayItemWid, 0, IDB_PI_UP, &APlayerRC);
    gpPlayItemDown = CreateButtonFromRC(gpPlayItemWid, 0, IDB_PI_DOWN, &APlayerRC);
    gpPlayItemDelALL = CreateButtonFromRC(gpPlayItemWid, 0, IDB_PI_ALL_DEL, &APlayerRC);

    tnRegisterCallBack(gpPlayItemPlay, WIDGET_CLICKED, DispPlayitemHand, (DATA_POINTER) HAND_EVENT_PITEM_PLAY);
    tnRegisterCallBack(gpPlayItemPlay, WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpPlayItemPlay, WIDGET_ESCAPED, DispKeyEsc, gpPlayListWid);
    tnRegisterCallBack(gpPlayItemDel, WIDGET_CLICKED, DispPlayitemHand, (DATA_POINTER) HAND_EVENT_PITEM_RM);
    tnRegisterCallBack(gpPlayItemDel, WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpPlayItemDel, WIDGET_ESCAPED, DispKeyEsc, gpPlayListWid);
    tnRegisterCallBack(gpPlayItemDelALL, WIDGET_CLICKED, DispPlayitemHand, (DATA_POINTER) HAND_EVENT_PITEM_RM_ALL);
    tnRegisterCallBack(gpPlayItemDelALL, WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpPlayItemDelALL, WIDGET_ESCAPED, DispKeyEsc, gpPlayListWid);

    tnRegisterCallBack(gpPlayItemDown, WIDGET_CLICKED, DispPlayitemHand, (DATA_POINTER) HAND_EVENT_PITEM_DOWN);
    tnRegisterCallBack(gpPlayItemDown, WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpPlayItemDown, WIDGET_ESCAPED, DispKeyEsc, gpPlayListWid);

    tnRegisterCallBack(gpPlayItemUp, WIDGET_CLICKED, DispPlayitemHand, (DATA_POINTER) HAND_EVENT_PITEM_UP);
    tnRegisterCallBack(gpPlayItemUp, WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpPlayItemUp, WIDGET_ESCAPED, DispKeyEsc, gpPlayListWid);

    GrSetFocus(gpPlayItemPlay->wid);
    //GrClearWindow(gpPlayItemWid->wid, GR_TRUE);
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
int DispCrtPlayList()
{
    gpPlayListWid = CreateWindowFromRC(NULL, 0, IDW_PLW, &APlayerRC);
    if (gpPlayListWid == NULL)
        return AUDIO_FALSE;
    gpFavLabel = (TN_WIDGET *) CreateLabelFromRC(gpPlayListWid, 0, IDL_FAV, &APlayerRC);
    gpPlayListBox = (TN_WIDGET *) CreateListBoxFromRC(gpPlayListWid, 0, IDPL_BOX, &APlayerRC);
    if (gpPlayListBox == NULL)
        return AUDIO_FALSE;
    // tnDeleteItemFromListBoxAt(gpPlayListBox, 0);
    GrSetFocus(gpPlayListBox->wid);
    tnRegisterCallBack(gpPlayListBox, WIDGET_CLICKED, DispPlayitemEnter, NULL);
    tnRegisterCallBack(gpPlayListBox, WIDGET_ESCAPED, DispKeyEsc, gpExtBtn[EXT_BTN_PL]);
    return AUDIO_TRUE;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/////// file brower/////
/**
 *\brief the callback function of the buttons belonging to file browser item.
 *\author yifj
 *\param widget the point to the widget which register the callback function. It must be the listbox of file browser
 *\param ptr HAND_EVENT_FB_PLAY:play operation,  HAND_EVENT_FB_ADD:add operation
 *\date 2008-04-07
 *\return
 */
static void DispFBitemHand(TN_WIDGET * widget, DATA_POINTER ptr)
{
    HandEventProc((tHandEvent) ptr);
    DispKeyEsc(widget, gpFbList);
}

/**
 *\brief add an item to File browser
 *\author yifj
 *\param Name a string, which contain the caption of the item
 *\param Pos the position to add to
 *\date 2008-04-07
 *\return
 */
int DispAddFBItemAt(char *Name, int Pos, int flag)
{
    if (flag == 0)
        tnAddItemToListBoxAt(gpFbList, Name, Pos, gImgFolder);
    else
        tnAddItemToListBoxAt(gpFbList, Name, Pos, gImgMusic);
}

/**
 *\brief not commented
 *\author yifj
 *\param num
 *\date 2008-04-07
 *\return
 */
int DispFBSetSel(int Num)
{
    tPlayInfoCur *pInfoCur = NULL;
    pInfoCur = HandGetInfoCur();
    tnSetLabelCaption(gpFbLabel, pInfoCur->FileBrower);
    tnSetSelectedListItemAt(gpFbList, Num, GR_TRUE);
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj
 *\param pSelNum
 *\date 2008-04-07
 *\return
 */
int DispFBSelNum(int *pSelNum)
{
    int count, *pPos = NULL, **ppPos;
    ppPos = &pPos;

    tnGetSelectedListPos(gpFbList, ppPos, &count);
    if (count != 1) {
        assert(0);
        return AUDIO_FALSE;
    }
    *pSelNum = *pPos;
    if (pPos != NULL)
        free(pPos);
    if (*pSelNum > (gpFbList->WSpec.listbox.numitems - 1)) {
        return AUDIO_FALSE;
    }
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return 0:not a folder, 1:current folder, 2:parent folder, 3: children folder
 */
static int DispSelIsFolder()
{
    int FolderNum;
    int count, result, *pPos = NULL, **ppPos;
    char *pListInfo = NULL, **ppListInfo, ***pppListInfo;
    ppPos = &pPos;
    ppListInfo = &pListInfo;
    pppListInfo = &ppListInfo;

    FolderNum = HandGetFolderNum();
    tnGetSelectedListPos(gpFbList, ppPos, &count);

    result = 0;
    if (*pPos <= FolderNum - 1) {
        tnGetSelectedListItems(gpFbList, pppListInfo, &count);
        if (strcmp(*ppListInfo, ".") == 0) {
            result = 1;
        } else if (strcmp(*ppListInfo, "..") == 0) {
            result = 2;
        } else {
            result = 3;
        }
        ComFreeList(*pppListInfo, count);
    };
    if (pPos != NULL)
        free(pPos);

    return result;
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispFBItemEnter(TN_WIDGET * widget, DATA_POINTER ptr)
{
    char FolderName[MAX_FILENAME_LEN];
    int count, FolederFlag;
    char *pListInfo = NULL, **ppListInfo, ***pppListInfo;
    ppListInfo = &pListInfo;
    pppListInfo = &ppListInfo;

    FolederFlag = DispSelIsFolder();
    if ((FolederFlag == 2) || (FolederFlag == 3)) {
        tnGetSelectedListItems(gpFbList, pppListInfo, &count);
        HandEnterDir(*ppListInfo);
        HandEventProc(HABD_EVENT_FB_ENTER);
        ComFreeList(*pppListInfo, count);
        return;
    } else if (FolederFlag == 1) {
        gpFbItemWid = CreateWindowFromRC(NULL, 0, IDW_FBIW, &APlayerRC);
        gpFbItemAdd = CreateButtonFromRC(gpFbItemWid, 0, IDB_FBI_ADD, &APlayerRC);;

        tnRegisterCallBack(gpFbItemAdd, WIDGET_CLICKED, DispFBitemHand, (DATA_POINTER) HAND_EVENT_FB_ADD);
        tnRegisterCallBack(gpFbItemAdd, WIDGET_KEYDOWN, DispKeyMove, NULL);
        tnRegisterCallBack(gpFbItemAdd, WIDGET_ESCAPED, DispKeyEsc, gpFbList);

        GrSetFocus(gpFbItemAdd->wid);
        return;
    } else {
        gpFbItemWid = CreateWindowFromRC(NULL, 0, IDW_FBIW, &APlayerRC);
        gpFbItemPlay = CreateButtonFromRC(gpFbItemWid, 0, IDB_FBI_PLAY, &APlayerRC);
        gpFbItemAdd = CreateButtonFromRC(gpFbItemWid, 0, IDB_FBI_ADD, &APlayerRC);;

        tnRegisterCallBack(gpFbItemPlay, WIDGET_CLICKED, DispFBitemHand, (DATA_POINTER) HAND_EVENT_FB_PLAY);
        tnRegisterCallBack(gpFbItemPlay, WIDGET_KEYDOWN, DispKeyMove, NULL);
        tnRegisterCallBack(gpFbItemPlay, WIDGET_ESCAPED, DispKeyEsc, gpFbList);

        tnRegisterCallBack(gpFbItemAdd, WIDGET_CLICKED, DispFBitemHand, (DATA_POINTER) HAND_EVENT_FB_ADD);
        tnRegisterCallBack(gpFbItemAdd, WIDGET_KEYDOWN, DispKeyMove, NULL);
        tnRegisterCallBack(gpFbItemAdd, WIDGET_ESCAPED, DispKeyEsc, gpFbList);

        GrSetFocus(gpFbItemPlay->wid);
        return;
    }
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
int DispFBDelAll()
{
    if (gpFbList == NULL)
        return AUDIO_FALSE;
    tnDeleteAllItemsFromListBox(gpFbList);
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj
 *\param FileName
 *\param FolderNum
 *\date 2008-04-07
 *\return
 */
int DispFBSelInfo(char *FileName)
{
    int count;
    char *pListInfo = NULL, **ppListInfo, ***pppListInfo;
    ppListInfo = &pListInfo;
    pppListInfo = &ppListInfo;

    tnGetSelectedListItems(gpFbList, pppListInfo, &count);
    strcpy(FileName, (*pppListInfo)[0]);
    ComFreeList(*pppListInfo, count);
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
int DispCrtFB()
{
    gpFbWid = CreateWindowFromRC(NULL, 0, IDW_FBW, &APlayerRC);  /// need to change rc
    if (gpFbWid == NULL)
        return AUDIO_FALSE;
    gpFbLabel = (TN_WIDGET *) CreateLabelFromRC(gpFbWid, 0, IDL_FOLDER, &APlayerRC);
    gpFbList = (TN_WIDGET *) CreateListBoxFromRC(gpFbWid, 0, IDFB_BOX, &APlayerRC);
    if (gpFbList == NULL)
        return AUDIO_FALSE;
    // tnDeleteItemFromListBoxAt(gpPlayListBox, 0);
    GrSetFocus(gpFbList->wid);
    tnRegisterCallBack(gpFbList, WIDGET_CLICKED, DispFBItemEnter, NULL);
    tnRegisterCallBack(gpFbList, WIDGET_ESCAPED, DispKeyEsc, gpExtBtn[EXT_BTN_FB]);
    return AUDIO_TRUE;
}

////// browser over////


////// progress bar///////
/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispTimerFunc(TN_WIDGET * widget, DATA_POINTER ptr)
{
    tPlayInfoConfig *pInfoConfig = NULL;
    tPlayInfoCur *pInfoCur = NULL;

    tHandMsg Msg;
    char Value[MAX_MSG_LEN];

#ifdef __ECOS
    if (HandMsgRecv(&Msg, Value) == AUDIO_TRUE) {
        if (Msg == HAND_MSG_FG) {
            if (strlen(dpfFbOpenedFileName) != 0){
                HandEventProc(HAND_EVENT_STOP);
                HandEnd();
                EngInit();
                HandEventProc(HAND_EVENT_PLAY);
                dpfFbOpenedFileName[0] = 0;
            }    
            DispShowWindow();
            GrSetFocus(gpNavBtn[NAV_BTN_PLAY_PAUSE]->wid);
        } else if (Msg == HAND_MSG_SUB)
            DispSubject(Value, "en");
        else
            EPRINTF(("got the wrong msg \n"));
    }
#endif // end of __ECOS


    pInfoConfig = HandGetInfoConfig();
    pInfoCur = HandGetInfoCur();
    if (pInfoConfig->EntryMode == FROM_FILEBROWSER) {
        DispSongSet(pInfoConfig->Song, pInfoCur->BrowserList.Length);
        tnSetLabelCaption(gpPosLabel, pInfoConfig->DirPath);
    } else {
        DispSongSet(pInfoConfig->Song, pInfoCur->SongList.Length);
        tnSetLabelCaption(gpPosLabel, "My Favorite Songs");
    }

    HandEventProc(HAND_EVENT_TIMER);
    char TimeStr[20];
    sprintf(TimeStr, "%d/%d", pInfoCur->CurTime, pInfoCur->SumTime);
    tnSetLabelCaption(gpTimeLabel, TimeStr);
    tnProgressBarUpdate(gpPlayProgBar, (pInfoCur->CurTime) * 100 / (pInfoCur->SumTime));
    char TimeBuf[20];
    DispGetTime(TimeBuf);
    tnSetLabelCaption(gpGlobalTime, TimeBuf);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispVolEsc(TN_WIDGET * widget, DATA_POINTER ptr)
{
    TN_WIDGET *pRetWidget;
    pRetWidget = (TN_WIDGET *) ptr;
    if (pRetWidget != NULL)
        GrSetFocus(pRetWidget->wid);
    tnDestroyWidget(widget);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispVolEnter(TN_WIDGET * widget, DATA_POINTER ptr)
{
    TN_WIDGET *pRetWidget;
    pRetWidget = (TN_WIDGET *) ptr;
    if (pRetWidget != NULL)
        GrSetFocus(pRetWidget->wid);
    tnDestroyWidget(widget);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispVolMove(TN_WIDGET * widget, DATA_POINTER ptr)
{
    long ch;
    int value;
    ch = (long) widget->data;

    if (ch == MWKEY_UP) {
        if (HandEventProc(HAND_EVENT_SOUND_UP) == AUDIO_TRUE) {
            value = tnGetProgressBarValue(gpVolProgBar);
            tnProgressBarUpdate(gpVolProgBar, value + STEP_VOLUME);
        }
    } else if (ch == MWKEY_DOWN) {
        if (HandEventProc(HAND_EVENT_SOUND_DOWN) == AUDIO_TRUE) {
            value = tnGetProgressBarValue(gpVolProgBar);
            tnProgressBarUpdate(gpVolProgBar, value - STEP_VOLUME);
        }
    } else
        DPRINTF(("not recieve this key! \n"));

}

//////progress over/////

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispNavBtnPlayPause(TN_WIDGET * widget, DATA_POINTER ptr)
{
    int result;
    GR_BOOL status;             // true: down   ;  false: up
    //// I don't certain about it!!! 
    tnGetCheckButtonStatus(gpNavBtn[NAV_BTN_PLAY_PAUSE], &status);

    if (DispPPBtnAble() == AUDIO_FALSE) {
        if (status == GR_TRUE)
            tnSetCheckButtonStatus(gpNavBtn[NAV_BTN_PLAY_PAUSE], GR_FALSE);
        else
            tnSetCheckButtonStatus(gpNavBtn[NAV_BTN_PLAY_PAUSE], GR_TRUE);
        return;
    }

    if (status == GR_TRUE) {
        result = HandEventProc(HAND_EVENT_PLAY);
    } else if (status == GR_FALSE) {
        result = HandEventProc(HAND_EVENT_PAUSE);
    } else
        assert(0);

    if (result == AUDIO_FALSE)
        EPRINTF(("why? \n"));
  
      // if (Play_or_Pause == 0)
      // tnSetButtonPixmap(widget, bmpfilename[0]);
      // else 
      // tnSetButtonPixmap(widget, bmpfilename[DISP_NAV_BTN_NUM]);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispNavBtnPre(TN_WIDGET * widget, DATA_POINTER ptr)
{
    HandEventProc(HAND_EVENT_PRE);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispNavBtnNext(TN_WIDGET * widget, DATA_POINTER ptr)
{
    HandEventProc(HAND_EVENT_NEXT);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispNavBtnStop(TN_WIDGET * widget, DATA_POINTER ptr)
{
    HandEventProc(HAND_EVENT_STOP);
}

static CallBackFuncPtr gNavBtnAct[DISP_NAV_BTN_NUM] = { DispNavBtnPlayPause, DispNavBtnStop, DispNavBtnPre, DispNavBtnNext };

//////////////////////////////////////////////
/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispExtBtnFB(TN_WIDGET * widget, DATA_POINTER ptr)
{
    DispIconDestroy();
    DispIconInit();
    HandEventProc(HAND_EVENT_FB);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispExtBtnMode(TN_WIDGET * widget, DATA_POINTER ptr)
{
    HandEventProc(HAND_EVENT_MODE);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispExtBtnPL(TN_WIDGET * widget, DATA_POINTER ptr)
{
    DispIconDestroy();
    DispIconInit();
    HandEventProc(HAND_EVENT_PL);
}

/**
 *\brief not commented
 *\author yifj
 *\param widget
 *\param ptr
 *\date 2008-04-07
 *\return
 */
static void DispExtBtnSound(TN_WIDGET * widget, DATA_POINTER ptr)
{
    gpVolProgBar = (TN_WIDGET *) CreateProgressBarFromRC(gpWid, 0, IDPB_VOL, &APlayerRC);

    tnRegisterCallBack(gpVolProgBar, WIDGET_CLICKED, DispVolEnter, gpExtBtn[EXT_BTN_SOUND]);
    tnRegisterCallBack(gpVolProgBar, WIDGET_KEYDOWN, DispVolMove, NULL);
    tnRegisterCallBack(gpVolProgBar, WIDGET_ESCAPED, DispVolEsc, gpExtBtn[EXT_BTN_SOUND]);
    GrSetFocus(gpVolProgBar->wid);
}

static CallBackFuncPtr gExtBtnAct[DISP_EXT_BTN_NUM] = { DispExtBtnFB, DispExtBtnMode, DispExtBtnPL, DispExtBtnSound };




/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispCreateBtns()
{
    gpNavBtn[NAV_BTN_PLAY_PAUSE] = (TN_WIDGET *) CreateCheckButtonFromRC(gpWid, 0, IDCB_PP, &APlayerRC);
    gpNavBtn[NAV_BTN_STOP] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_STOP, &APlayerRC);
    gpNavBtn[NAV_BTN_PRE] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_PRE, &APlayerRC);
    gpNavBtn[NAV_BTN_NEXT] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_NEXT, &APlayerRC);
    gpExtBtn[EXT_BTN_FB] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_FB, &APlayerRC);
    gpExtBtn[EXT_BTN_MODE] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_MODE, &APlayerRC);
    gpExtBtn[EXT_BTN_PL] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_PL, &APlayerRC);
    gpExtBtn[EXT_BTN_SOUND] = (TN_WIDGET *) CreateButtonFromRC(gpWid, 0, IDB_VOL, &APlayerRC);


    //CreateLabel(gpTimeLabel, pWid, 663, 449, "00:00/00:00", 100, 30, "", 6, BLUE, RED);



    tnRegisterCallBack(gpNavBtn[NAV_BTN_PLAY_PAUSE], WIDGET_CLICKED, gNavBtnAct[NAV_BTN_PLAY_PAUSE], NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_PLAY_PAUSE], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_PLAY_PAUSE], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpNavBtn[NAV_BTN_STOP], WIDGET_CLICKED, gNavBtnAct[NAV_BTN_STOP], NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_STOP], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_STOP], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpNavBtn[NAV_BTN_PRE], WIDGET_CLICKED, gNavBtnAct[NAV_BTN_PRE], NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_PRE], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_PRE], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpNavBtn[NAV_BTN_NEXT], WIDGET_CLICKED, gNavBtnAct[NAV_BTN_NEXT], NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_NEXT], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpNavBtn[NAV_BTN_NEXT], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpExtBtn[EXT_BTN_FB], WIDGET_CLICKED, gExtBtnAct[EXT_BTN_FB], NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_FB], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_FB], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpExtBtn[EXT_BTN_MODE], WIDGET_CLICKED, gExtBtnAct[EXT_BTN_MODE], NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_MODE], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_MODE], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpExtBtn[EXT_BTN_PL], WIDGET_CLICKED, gExtBtnAct[EXT_BTN_PL], NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_PL], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_PL], WIDGET_ESCAPED, DispKeyEsc, NULL);
    
    tnRegisterCallBack(gpExtBtn[EXT_BTN_SOUND], WIDGET_CLICKED, gExtBtnAct[EXT_BTN_SOUND], NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_SOUND], WIDGET_KEYDOWN, DispKeyMove, NULL);
    tnRegisterCallBack(gpExtBtn[EXT_BTN_SOUND], WIDGET_ESCAPED, DispKeyEsc, NULL);

    GrSetFocus(gpNavBtn[NAV_BTN_PLAY_PAUSE]->wid);
}


/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispCreatePics()
{
    char FilePath[MAX_FILE_PATH];
    strcpy(FilePath, MODE_PIC_DEF_PATH);
    ComPathReltoAbs(FilePath);
    DispDrawPic(gpWid, MODE_PIC_X, MODE_PIC_Y, MODE_PIC_W, MODE_PIC_H, FilePath);        //temp
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispCreateProgressBar()
{
    gpPlayProgBar = (TN_WIDGET *) CreateProgressBarFromRC(gpWid, 0, IDPB_TIME, &APlayerRC);
    gTimerId = GrCreateTimer(gpWid->wid, 500);
    if (gTimerId == 0) {
        EPRINTF(("can't create the timer! \n"));
        exit(0);
    }
    tnRegisterCallBack(gpWid, WIDGET_TIMER, DispTimerFunc, NULL);
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispCreateLabel()
{
    gpId3Song = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_SONG, &APlayerRC);
    gpId3Album = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_ALBUM, &APlayerRC);
    gpId3Singer = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_SINGER, &APlayerRC);
    gpPosLabel = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_CUR_POS, &APlayerRC);
    gpTimeLabel = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_TIME, &APlayerRC);
    gpSongLabel = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_SONG_ORDER, &APlayerRC);
    gpGlobalTime = (TN_WIDGET *) CreateLabelFromRC(gpWid, 0, IDL_TIME_DISP, &APlayerRC);
}

//////about window //////
/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispHideWindow()
{
    DPRINTF(("go to hind the app! \n"));
    tnSetVisible(gpWid, GR_FALSE);
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispEndWindow()
{
    DPRINTF(("got to End the app!  \n"));
    DispIconDestroy();
    HandEventProc(HAND_EVENT_STOP);
    HandEnd();
    tnEndApp(&APlayerRC);
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static void DispShowWindow()
{
    DPRINTF(("go to show the app! \n"));
    tnSetVisible(gpWid, GR_TRUE);
}

///// window over //////

////// about subject /////
/**
 *\brief not commented
 *\author yifj
 *\param SubjectName
 *\param LangName
 *\date 2008-04-07
 *\return
 */
static void DispSubject(char *SubjectName, char *LangName)
{
    DPRINTF(("got the sub %s, lang %s \n", SubjectName, LangName));
}

////// subject over/////

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
int DispInit()
{
    struct configFile *pConfig = NULL;
    char *pLang = NULL, *pSub = NULL;
    char RcFilePath[MAX_FILE_PATH];

#ifdef __ECOS	    
//	gpMainWindow = tnControlLibInitialize();
    gpMainWindow = tnAppInitialize();
	//tnControlLibInitialize();
#else	
	gpMainWindow = tnAppInitialize();
#endif    

    pConfig = readConfigFile(CFG_FILE_PATH);
    if (!pConfig) {
        EPRINTF(("can't open %s\n", CFG_FILE_PATH));
        return AUDIO_FALSE;
    }
    pSub = getConfigString(pConfig, "setting", "subject");
#if 0
    pLang = getConfigString(pConfig, "setting", "language");
#else
    pLang = "en";
#endif
    if ((pSub == NULL) || (pLang == NULL)) {
        DPRINTF(("not exist this key\n"));
        unloadConfigFile(pConfig);
        return AUDIO_FALSE;
    }

    getRcFilePath(RcFilePath, RESOURCE_PATH, pSub, APP_NAME);
    load_rc(&APlayerRC, RcFilePath, pLang);
    unloadConfigFile(pConfig);
    if (gpMainWindow == NULL) {
        assert(0);
        exit(0);
    }
    
    gpWid = (TN_WIDGET *) CreateWindowFromRC(gpMainWindow, 0, IDW_WINDOW, &APlayerRC);
    //gpWid = (TN_WIDGET *) CreateWindowFromRC(NULL, 0, IDW_WINDOW, &APlayerRC);

    DispCreateBtns();
    DispCreateProgressBar();
    DispCreatePics();
    DispCreateLabel();
    
    //pao-shu
    //DispShowWindow();
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
int DispEnd()
{
    tnEndApp(&APlayerRC);
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
//void DispNeedFix()
//{
//    tnDeleteItemFromListBoxAt(gpPlayListBox, 0);
//}


/**
 *\brief not commented
 *\author yifj
 *\param time_b
 *\date 2008-04-07
 *\return
 */

static void DispGetTime(char *time_b)
{
    time_t ticks;
    struct tm *t;

    ticks = time(NULL);
    t = localtime(&ticks);
    strftime(time_b, 127, "%H:%M:%S", t);
}

/**
 *\brief not commented
 *\author yifj
 *\param Subject
 *\date 2008-04-07
 *\return
 */
int DispSwitchSub(char *Subject)
{
    struct configFile *pConfig = NULL;
    char RcFilePath[MAX_FILE_PATH];
    char *pLang = NULL;

#if 0
    pConfig = readConfigFile(CFG_FILE_PATH);
    if (!pConfig) {
        EPRINTF(("can't open %s\n", CFG_FILE_PATH));
        return AUDIO_FALSE;
    }
    pLang = getConfigString(pConfig, "setting", "language");
#else
    pLang = "en";
#endif

    if (pLang == NULL) {
        DPRINTF(("not exist this key\n"));
        return AUDIO_FALSE;
    }

    getRcFilePath(RcFilePath, RESOURCE_PATH, Subject, APP_NAME);

    //change subject
    if (switch_subject(&APlayerRC, RcFilePath, pLang) < 0) {
        DPRINTF(("change subject failue\n"));
        return AUDIO_FALSE;
    } else {
        DPRINTF(("change subject succesfully\n"));
        return AUDIO_TRUE;
    }
}

/**
 *\brief not commented
 *\author yifj
 *\param Language
 *\date 2008-04-07
 *\return
 */
int DispSwitchLang(char *Language)
{
    struct configFile *pConfig = NULL;
    char RcFilePath[MAX_FILE_PATH];
    char *pSub = NULL;

    pConfig = readConfigFile(CFG_FILE_PATH);
    if (!pConfig) {
        EPRINTF(("can't open %s\n", CFG_FILE_PATH));
        return AUDIO_FALSE;
    }
    pSub = getConfigString(pConfig, "setting", "subject");
    if (pSub == NULL) {
        DPRINTF(("not exist this key\n"));
        unloadConfigFile(pConfig);
        return AUDIO_FALSE;
    }

    getRcFilePath(RcFilePath, RESOURCE_PATH, pSub, APP_NAME);
    unloadConfigFile(pConfig);
    //change subject
    if (switch_lang(&APlayerRC, RcFilePath, Language) < 0) {
        DPRINTF(("change language failue\n"));
        return AUDIO_FALSE;
    } else {
        DPRINTF(("change language succesfully\n"));
        return AUDIO_TRUE;
    }
}

static int DispPPBtnAble()
{
    tPlayInfoCur *pInfoCur = NULL;
    tPlayInfoConfig *pInfoConfig = NULL;
    pInfoCur = HandGetInfoCur();
    pInfoConfig = HandGetInfoConfig();
    if (pInfoConfig->EntryMode == FROM_FILEBROWSER) {
        if ((pInfoCur->BrowserList.Length == 0) || (pInfoConfig->Song == 0))
            return AUDIO_FALSE;
        else if ((pInfoCur->BrowserList.Length > 0) && (pInfoConfig->Song > 0))
            return AUDIO_TRUE;
        else
            assert(0);
    } else {
        if ((pInfoCur->SongList.Length == 0) || (pInfoConfig->Song == 0))
            return AUDIO_FALSE;
        else if ((pInfoCur->SongList.Length > 0)  && (pInfoConfig->Song > 0))
            return AUDIO_TRUE;
        else
            assert(0);
    }
    return AUDIO_FALSE;
}

void DispPPBtnStop()
{
    tnSetCheckButtonStatus(gpNavBtn[NAV_BTN_PLAY_PAUSE], GR_FALSE);
}

static void DispIconInit()
{
    char ImgFilePath[MAX_FILE_PATH];
    
    strcpy(ImgFilePath, ICON_FOLDER_PATH);
    ComPathReltoAbs(ImgFilePath);
    gImgFolder = GrLoadImageFromFile(ImgFilePath,0);
    strcpy(ImgFilePath, ICON_MUSIC_PATH);
    ComPathReltoAbs(ImgFilePath);
    gImgMusic = GrLoadImageFromFile(ImgFilePath,0);
    return;
}

static void DispIconDestroy()
{
    GrFreeImage(gImgFolder);
    GrFreeImage(gImgMusic);
}

