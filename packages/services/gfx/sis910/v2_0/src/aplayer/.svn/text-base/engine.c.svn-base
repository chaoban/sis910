/**
 *\file engine.c
 *\brief it is data engine module. It controls the data of audio player.
 *\author yifj
 *\date 2008-04-07
 *
 *$Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "engine.h"
#include "handle.h"
#include "decoder.h"
#include "assert.h"
#include "../config/config.h"
#include "../desktop/global.h"

#include"dirent.h"
#include"sys/stat.h"
//#include"signal.h"
#include"unistd.h"

static tPlayerState gPlayState;
static tPlayInfoConfig gPlayInfoConfig;
static tPlayInfoCur gPlayInfoCur;

//// for dir and file operations////
/**
 *\brief get the items of the specific directory, including files and folers
 *\author yifj
 *\param Dir input parameter, the specific directory which need to be checked.
 *\param pNameList output parameter, *pNameList is use to store the items get from the directory.
 *\param pNum output parameter, it is used to store the total number of found items.
 *\date 2008-04-07
 *\return AUDIO_FALSE:false , AUDIO_TRUE;success
 */
#ifndef LONG_FILENAME
int EngGetDir(char *Dir, struct dirent ***pNameList, int *pNum)
#else
int EngGetDir(char * Dir, char *** pNameList, int * pNum)
#endif //LONG_FILENAME
{
    *pNum = ApScanDir(Dir, pNameList, 0, NULL);
    if (*pNum < 0)
        return AUDIO_FALSE;
    else
        return AUDIO_TRUE;
}

/**
 *\brief free the memory alloted for store the name list under a specific dir by EngGetDir. It should be used in pair with EngGetDir
 *\author yifj
 *\param namelist the memory alloted by EngGetDir, which stores the name of files and dirs under a specific dir.
 *\param n the number of names in the namelist
 *\date 2008-04-07
 *\return
 */
#ifndef LONG_FILENAME
void EngFreeDir(struct dirent **ppNameList, int Num)
#else 
void EngFreeDir(char ** ppNameList, int Num)
#endif //LONG_FILENAME
{
    int i;
    if (ppNameList == NULL)
        return;
    for (i = 0; i < Num; i++)
        free(ppNameList[i]);
    free(ppNameList);
    return;
}


/**
 *\brief analyse a file path, then get in which dir the file is and how many audio file (mp3 or MP3) in  this dir and the order of the file in this dir.
 it is used when the desktop assigns a parameter (specific audio file) to the audio player. The engine module identify a file by the dir in which the file
 exist and the order of this file in the dir, so the file name need to a interpretation to be recongnized by engine module. 
 *\author yifj
 *\param data a string, the name of the input file name.
 *\param SongNum the order of the input file in the dir in which it is now existing.
 *\param TotleNum   the totle audio file name of the dir in which the file is now existing.
 *\param FileDir a string, the dir name in which the file is now existing.
 *\date 2008-04-07
 *\return AUDIO_FALSE:can't find this file, AUDIO_TRUE: success.
 */
static int EngGetOrderDir(char *data, int *SongNum, char *FileDir)
{
    int i = 0, AllNum = 0, result, Num = 0;
    char TempPath[MAX_FILE_PATH];
    DIR *fdir = NULL;

    *SongNum = 0;
#ifndef LONG_FILENAME
    struct dirent **NameList = NULL;
#else
    char **NameList = NULL;
#endif //LONG_FILENAME

    if (data == NULL) {
        return AUDIO_FALSE;
        ASSERT(0);
    }
    i = strlen(data);
    for (; i >= 0; i--) {
        if ((data[i] == '/') || (data[i] == '\\'))
            break;
    }
    if (i < 0)
        strcpy(FileDir, "./");
    else {
        FileDir[i + 1] = 0;
        while (i >= 0) {
            FileDir[i] = data[i];
            i--;
        }
    }

    result = EngGetDir(FileDir, &NameList, &AllNum);
    if (result == AUDIO_FALSE) {
        EngFreeDir(NameList, AllNum);
        assert(0);
        return AUDIO_FALSE;
    }

    strcpy(TempPath, FileDir);
    for (i = 0; i < AllNum; i++) {
#ifndef LONG_FILENAME
        strcat(TempPath, NameList[i]->d_name);
#else
        strcat(TempPath, NameList[i]);
#endif // LONG_FILENAME
        if ((fdir = opendir(TempPath)) != NULL) {
            closedir(fdir);
        } else {
#ifndef LONG_FILENAME
            if (strstr(NameList[i]->d_name, "mp3") || strstr(NameList[i]->d_name, "MP3")) {
#else
            if (strstr(NameList[i], "mp3") || strstr(NameList[i], "MP3")) {
#endif //LONG_FILENAME
                Num++;
                if (strcmp(TempPath, data) == 0) {
                    DPRINTF(("found the param file! \n"));
                    *SongNum = Num;
                }
            }
        }
        strcpy(TempPath, FileDir);
    }
    EngFreeDir(NameList, AllNum);
    if (*SongNum == 0) {
        EPRINTF(("didn't find this file \n"));
        return AUDIO_FALSE;
    }
    return AUDIO_TRUE;
}

////// dir operations over /////

///// Link Operation---- for playlist ////////
/**
 *\brief initial the link structure of engine module, including songlist which contain the file names of favorite songs and BrowserList which contain the audio file names 
 in the current path (where the last play operation happens).
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static int EngLinkInit()
{
    tpLink pPlayLink = &(gPlayInfoCur.SongList);
    tpLink pBrowserLink = &(gPlayInfoCur.BrowserList);
    if ((pPlayLink == NULL) || (pBrowserLink == NULL)) {
        EPRINTF(("error: input of ComLinkInit! \n"));
        return AUDIO_FALSE;
    }
    pPlayLink->Length = 0;
    pPlayLink->pHeader = NULL;
    pBrowserLink->Length = 0;
    pBrowserLink->pHeader = NULL;
    return AUDIO_TRUE;
}

/**
 *\brief add a node at the end of a link
 *\author yifj
 *\param pLink the pointer to the link
 *\param Content the content of the node which need to be added. 
 *\date 2008-04-07
 *\return AUDIO_FALSE:failed, AUDIO_TRUE:success
 */
static int EngLinkAdd(tpLink pLink, char *Content)
{
    tpLinkNode p = NULL, pbefore = NULL, puse = NULL;
    char *TempContent = NULL;
    int len;

    if ((pLink == NULL) || (Content == NULL)) {
        EPRINTF(("error: input of ComLinkAdd ! \n"));
        return AUDIO_FALSE;
    }
    if ((pLink->Length) >= MAX_PLI_NUM) {
        DPRINTF(("You add too much songs into the favorite library! \n"));
        return AUDIO_FALSE;
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
            return AUDIO_TRUE;
#else
        if (strcmp(Content, p->content) == 0)
            return AUDIO_TRUE;
#endif //__ECOS
        pbefore = p;
        p = p->next;
    }

    puse = (tLinkNode *) malloc(sizeof(tLinkNode));
    if (puse == NULL) {
        EPRINTF(("malloc error happened !\n"));
        ASSERT(0);
        return AUDIO_FALSE;
    }
    puse->next = NULL;
    len = strlen(Content);
    if (len >= MAX_FILE_PATH) {
        DPRINTF(("the file path is too long! \n"));
        return AUDIO_FALSE;
    }
    TempContent = (char *) malloc(len + 1);
    if (TempContent == NULL) {
        EPRINTF(("malloc error happened! \n"));
        ASSERT(0);
        return AUDIO_FALSE;
    }
    strcpy(TempContent, Content);
    puse->content = TempContent;
    if (pbefore == NULL)
        pLink->pHeader = puse;
    else
        pbefore->next = puse;


    pLink->Length++;
    return AUDIO_TRUE;
}

/**
 *\brief read the content of the assigned node of the link
 *\author yifj
 *\param pLink the pointer to link 
 *\param Pos the position of the assigned node 
 *\param Content[] a pointer to a string in which the function will write the got content
 *\date 2008-04-07
 *\return
 */
int EngLinkRead(tpLink pLink, int Pos, char Content[])
{
    int i = 1;
    tpLinkNode p = pLink->pHeader;
    if ((Content == NULL) || (Pos > pLink->Length) || (Pos < 1)) {
        DPRINTF(("exceed the end ! \n"));
        return AUDIO_FALSE;
    }
    while (i < Pos) {
        p = p->next;
        i++;
    }
    ///////
    //if (strlen(p->content)+1 > sizeof(Content))
    //    {
    //    assert(0);
    //    return AUDIO_FALSE;
    //    }
    ///////
    strcpy(Content, p->content);
    return AUDIO_TRUE;
}

/**
 *\brief save the link to a file with specific format.
 *\author yifj
 *\param pLink the pointer to link which need to be saved
 *\param filename[] the file name of the file where the link saved
 *\date 2008-04-07
 *\return AUDIO_FALSE or AUDIO_TRUE
 */
int EngLinkSave(tpLink pLink, char FileName[])
{
    FILE *fp = NULL;
    tpLinkNode p = pLink->pHeader;
    int i = 1;
    fp = fopen(FileName, "w+");
    if (fp == NULL) {
        EPRINTF(("error! \n"));
        return AUDIO_FALSE;
    }

    fprintf(fp, "[PLAY_LIST]\n");
    while ((i <= pLink->Length) && (p != NULL)) {
        fprintf(fp, "MUSIC%d=%s\n", i, p->content);
        i++;
        p = p->next;
    }
    fclose(fp);
    return AUDIO_TRUE;
}

/**
 *\brief delete the assigned node of a link
 *\author yifj
 *\param pLink the pointer to the link 
 *\param Pos the order of the assigned node
 *\date 2008-04-07
 *\return
 */
static int EngLinkDelAtPos(tpLink pLink, int Pos)
{
    int i = 1;
    tpLinkNode p = pLink->pHeader, pbef = NULL;
    if (pLink->Length < Pos) {
        assert(0);
        return AUDIO_FALSE;
    }
    if (Pos == 1) {
        pLink->pHeader = p->next;
        (pLink->Length)--;
        free(p->content);
        free(p);
        return AUDIO_TRUE;
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
    return AUDIO_TRUE;
}

/**
 *\brief free the memory when the link is used over
 *\author yifj
 *\param pLink the pointer to the link
 *\date 2008-04-07
 *\return
 */
int EngLinkDestroy(tpLink pLink)
{
    tpLinkNode p = pLink->pHeader, pnext;

    if (pLink == NULL) {
        assert(0);
        return AUDIO_FALSE;
    }

    while (p) {
        pnext = p->next;
        free(p->content);
        free(p);
        p = pnext;
    }
    pLink->pHeader = NULL;
    pLink->Length = 0;
    return AUDIO_TRUE;
}


/** 
 *\brief add a aduio file to favorite songs list.
 *\author yifj
 *\param Content the file name which will be added to favorite song list as a node
 *\date 2008-04-07
 *\return
 */
int EngAddPlayitem(char *Content)
{
    EngLinkAdd(&(gPlayInfoCur.SongList), Content);
    if (strlen(gPlayInfoCur.CurFilePath) == 0) {
        gPlayInfoConfig.Song = 1;
        EngLinkRead(&(gPlayInfoCur.SongList), 1, gPlayInfoCur.CurFilePath);
    }
    return;
}

/**
 *\brief delete a aduio file from favorite songs list.
 *\author yifj
 *\param DelNum the assigned order of the file which will be delete from favorite songs list.
 *\date 2008-04-07
 *\return
 */
int EngDelPlayitem(int DelNum)
{
    return EngLinkDelAtPos(&(gPlayInfoCur.SongList), DelNum);
}

/**
 *\brief make the selected song take one step ahead in favorite songs order 
 *\author yifj
 *\param pLink the pointer to the favorite song list,
 *\param SelNum the assigned song number which need to be changed in order.
 *\date 2008-04-07
 *\return
 */
int EngUpPlayitem(tpLink pLink, int SelNum)
{
    tpLinkNode p = pLink->pHeader, ppre = NULL, pprepre = NULL;
    int i = 0;
    if (SelNum <= 0) {
        return AUDIO_FALSE;
    }
    while (i < SelNum) {
        pprepre = ppre;
        ppre = p;
        p = p->next;
        i++;
    }

    if (SelNum == 1) {
        ppre->next = p->next;
        p->next = ppre;
        pLink->pHeader = p;
    } else {
        ppre->next = p->next;
        p->next = ppre;
        pprepre->next = p;
    }

    return AUDIO_TRUE;
}

/**
 *\brief make the selected song take one step backward in favorite songs order
 *\author yifj
 *\param pLink the pointer to the favorite song list,
 *\param SelNum the assigned song number which need to be changed in order.
 *\date 2008-04-07
 *\return
 */
int EngDownPlayitem(tpLink pLink, int SelNum)
{
    tpLinkNode p = pLink->pHeader, ppre = NULL, pnext = p->next;
    int i = 0;
    /// if (SelNum >= (pPlayListBox->WSpec.listbox.numitems - 1)) // need to know how long the list is.
    while (i < SelNum) {
        ppre = p;
        p = p->next;
        i++;
    }
    pnext = p->next;

    if (SelNum == 0) {
        p->next = pnext->next;
        pnext->next = p;
        pLink->pHeader = pnext;
    } else {
        p->next = pnext->next;
        pnext->next = p;
        ppre->next = pnext;
    }

    return AUDIO_TRUE;
}

//////////////////////////////////over///////////////////////



/**
 *\brief set global variable gPlayState
 *\author yifj
 *\param State the assigned state need to be set to gPlayState
 *\date 2008-04-07
 *\return
 */
void EngSetPlayState(tPlayerState State)
{
    gPlayState = State;
}

/**
 *\brief get global variable gPlayState value
 *\author yifj

 *\date 2008-04-07
 *\return the state value get from gPlayState
 */
tPlayerState EngGetPlayState()
{
    return gPlayState;
}

/**
 *\brief get the pointer to globel variable gPlayInfoConfig
 *\author yifj

 *\date 2008-04-07
 *\return the pointer to gPlayInfoConfig
 */
tPlayInfoConfig *EngGetInfoConfig()
{
    return &gPlayInfoConfig;
}

/**
 *\brief get the pointer to globel variable gPlayInfoCur
 *\author yifj

 *\date 2008-04-07
 *\return the pointer to gPlayInfoCur
 */
tPlayInfoCur *EngGetInfoCur()
{
    return &gPlayInfoCur;
}

/**
 *\brief not commented
 *\author yifj
 *\param Value
 *\date 2008-04-07
 *\return
 */
int EngSetSound(tSoundValue Value)
{
    if ((Value < MIN_VOLUME) || (Value > MAX_VOLUME)) {
        //assert(0);
        return AUDIO_FALSE;
    }
    gPlayInfoConfig.Sound = Value;
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj
 *\param pValue
 *\date 2008-04-07
 *\return
 */
int EngGetSound(tSoundValue * pValue)
{
    if ((gPlayInfoConfig.Sound < MIN_VOLUME) || (gPlayInfoConfig.Sound > MAX_VOLUME)) {
        assert(0);
        return AUDIO_FALSE;
    }
    *pValue = gPlayInfoConfig.Sound;
    return AUDIO_TRUE;
}


////
////disgard of list and file;
////

/**
 *\brief read the configuration file and set the global parameter of the player.
 *\author yifj
 *\param data
 *\date 2008-04-07
 *\return AUDIO_FALSE or AUDIO_TRUE
 */
static int EngLoadConfig()
{
    struct configFile *pConfig = NULL;
    char *pGetString = NULL;
    int GetNum, result;

    ///// it is usefull if data != NULL ////
    /////// over /////

    pConfig = readConfigFile(CFG_FILE_PATH);
    if (!pConfig) {
        EPRINTF(("can't open %s\n", CFG_FILE_PATH));
        return AUDIO_FALSE;
    }

//    if (strlen(dpfFbOpenedFileName) != 0) {
//        result = EngGetOrderDir(dpfFbOpenedFileName, &SongNum, FileDir);
//        if (result == AUDIO_FALSE)
//            ASSERT(0);
//    }

    GetNum = getConfigInt(pConfig, "audio player", "sound");
    gPlayInfoConfig.Sound = GetNum;
    GetNum = getConfigInt(pConfig, "audio player", "playmode");
    gPlayInfoConfig.PlayMode = GetNum;
    pGetString = getConfigString(pConfig, "audio player", "playlistfile");
    //strcpy(gPlayInfoConfig.PlayList, pGetString);
    strcat(gPlayInfoConfig.PlayList,"/romdisk");
    strcat(gPlayInfoConfig.PlayList,pGetString);

    gPlayInfoConfig.DirPath[0] = 0;
    if (strlen(dpfFbOpenedFileName) == 0) {
        GetNum = getConfigInt(pConfig, "audio player", "entrymode");
        gPlayInfoConfig.EntryMode = GetNum;
        pGetString = getConfigString(pConfig, "audio player", "dirpath");
        #ifdef __ECOS
        if (pGetString[0] != '/')
            strcpy(gPlayInfoConfig.DirPath, "/hd/");
        else
            strcpy(gPlayInfoConfig.DirPath, "/hd/");
        #else
        if ((pGetString[0] == '.' && pGetString[1] == '/') == 0)
            strcpy(gPlayInfoConfig.DirPath, "./");
        #endif //__ECOS__
        strcat(gPlayInfoConfig.DirPath, pGetString);  
        GetNum = getConfigInt(pConfig, "audio player", "songid");
        gPlayInfoConfig.Song = GetNum;
    } else {
        int SongNum = 0;        
        gPlayInfoConfig.EntryMode = FROM_FILEBROWSER;
        result = EngGetOrderDir(dpfFbOpenedFileName, &SongNum, gPlayInfoConfig.DirPath);
        if (result == AUDIO_FALSE)
            ASSERT(0);
        gPlayInfoConfig.Song = SongNum;
    }

    unloadConfigFile(pConfig);
    return AUDIO_TRUE;
}

/**
 *\brief save the global parameter of player to configuration file
 *\author yifj

 *\date 2008-04-07
 *\return AUDIO_FALSE or AUDIO_TRUE
 */
static int EngSaveConfig()
{
    struct configFile *pConfig = NULL;
    int Len = 0;

    pConfig = readConfigFile(CFG_FILE_PATH);
    if (!pConfig) {
        EPRINTF(("can't open %s\n", CFG_FILE_PATH));
        return AUDIO_FALSE;
    }

    setConfigInt(pConfig, "audio player", "sound", gPlayInfoConfig.Sound);
    setConfigInt(pConfig, "audio player", "playmode", gPlayInfoConfig.PlayMode);
    setConfigInt(pConfig, "audio player", "entrymode", gPlayInfoConfig.EntryMode);
    setConfigInt(pConfig, "audio player", "songid", gPlayInfoConfig.Song);

    setConfigString(pConfig, "audio player", "playlistfile", gPlayInfoConfig.PlayList);
    Len = strlen(gPlayInfoConfig.DirPath);
    if ((Len == 0) || (gPlayInfoConfig.DirPath[Len-1] != '/'))
        strcat(gPlayInfoConfig.DirPath, "/");    
    setConfigString(pConfig, "audio player", "dirpath", gPlayInfoConfig.DirPath);

    saveConfigFile(pConfig, CFG_FILE_PATH);
    unloadConfigFile(pConfig);
    return AUDIO_TRUE;
}

/**
 *\brief read the file in which the favorate songs list is stored. 
 *\author yifj

 *\date 2008-04-07
 *\return AUDIO_FALSE or AUDIO_TRUE
 */
static int EngReadPlayList()
{
    struct configFile *pConfig = NULL;
    char Key[10] = { 0 };
    char *pGetString = NULL;
    int i = 1;
    int result = AUDIO_TRUE;

    pConfig = readConfigFile(gPlayInfoConfig.PlayList);
    if (!pConfig) {
        EPRINTF(("can't open %s\n", gPlayInfoConfig.PlayList));
        return AUDIO_FALSE;
    }
    //while (1) {
    //    sprintf(Key, "MUSIC%d", i);
    //    pGetString = getConfigString(pConfig, "PLAY_LIST", Key);
    //    if (pGetString == NULL) {
    //        DPRINTF(("read over the playlist file! \n"));
    //        break;
    //    }
    //    result = EngLinkAdd(&(gPlayInfoCur.SongList), pGetString);
    //    assert(result == AUDIO_TRUE);
    //    i++;
    //}
    unloadConfigFile(pConfig);
    return result;
}

/// read the folder--gPlayInfoCur.FileBrower, you need to reassignt the value before!
/**
 *\brief read the audio files itmes of current playing path
 *\author yifj

 *\date 2008-04-07
 *\return AUDIO_FALSE or AUDIO_TRUE
 */
int EngReadBrowserList()
{
#ifndef LONG_FILENAME
    struct dirent **NameList = NULL;
#else
    char ** NameList = NULL;
#endif // LONG_FILENAME

    int AllNum = 0;
    int i, result = AUDIO_TRUE;
    char TempPath[MAX_DIR_LEN];
    DIR *fdir = NULL;

    gPlayInfoCur.FBFolderNum = 0;
    gPlayInfoCur.FBFileNum = 0;
    EngLinkDestroy(&(gPlayInfoCur.BrowserList));
    result = EngGetDir(gPlayInfoCur.FileBrower, &NameList, &AllNum);
    if (result == AUDIO_FALSE) {
        EngFreeDir(NameList, AllNum);
        assert(0);
        return AUDIO_FALSE;
    }

    strcpy(TempPath, gPlayInfoCur.FileBrower);
    for (i = 0; i < AllNum; i++) {
#ifndef LONG_FILENAME
        strcat(TempPath, NameList[i]->d_name);
#else
        strcat(TempPath, NameList[i]);
#endif //LONG_FILENAME
        if ((fdir = opendir(TempPath)) != NULL) {
            closedir(fdir);
            gPlayInfoCur.FBFolderNum++;
#ifndef LONG_FILENAME
        } else if(strstr(NameList[i]->d_name, "mp3") || strstr(NameList[i]->d_name, "MP3")) {
#else
        } else if(strstr(NameList[i], "mp3") || strstr(NameList[i], "MP3")) {
#endif //LONG_FILENAME
            result = EngLinkAdd(&(gPlayInfoCur.BrowserList), TempPath);
            assert(result == AUDIO_TRUE);
            gPlayInfoCur.FBFileNum++;
        }
        strcpy(TempPath, gPlayInfoCur.FileBrower);
    }
    EngFreeDir(NameList, AllNum);

    return AUDIO_TRUE;
}

/**
 *\brief get the file name from the full path of the file
 *\author yifj
 *\param Path[] input param, the string stores the full path
 *\param FileName[] output param, the file name
 *\date 2008-04-07
 *\return
 */
int EngPathToName(char Path[], char FileName[])
{
    int result, len, i;
    len = strlen(Path);
    if ((len >= MAX_FILE_PATH) || (len <= 0)) {
        assert(0);
        return AUDIO_FALSE;
    }
    i = len - 1;
    while (i >= 0) {
        if ((Path[i] == 47) || (Path[i] == 92))
            break;
        i--;
    }
    i++;
    if (((len + 1) - i) > MAX_FILENAME_LEN) {
        assert(0);
        return AUDIO_FALSE;
    }
    strcpy(FileName, &Path[i]);
    return AUDIO_TRUE;
}


/**
 *\brief change playing mode
 *\author yifj
 *\param PlayMode output, it is used to store the current Play mode
 *\date 2008-04-07
 *\return AUDIO_FALSE or AUDIO_TRUE
 */
int EngChMode(tPlayMode * PlayMode)
{
    if (PlayMode == NULL) {
        EPRINTF(("error: ! \n"));
        ASSERT(0);
        return AUDIO_FALSE;
    }
    gPlayInfoConfig.PlayMode += 1;
    gPlayInfoConfig.PlayMode %= 3;
    *PlayMode = gPlayInfoConfig.PlayMode;
    return AUDIO_TRUE;
}

/**
 *\brief set the current file path in player parameter according to the entry mode and the current file order number in file browser or favorate list.
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static int EngInitCurFile()
{
    if (gPlayInfoConfig.EntryMode == FROM_PLAYLIST) {
        if (gPlayInfoCur.SongList.Length >= gPlayInfoConfig.Song){
            EngLinkRead(&(gPlayInfoCur.SongList), gPlayInfoConfig.Song, gPlayInfoCur.CurFilePath);       
        }else{ 
            (gPlayInfoCur.SongList.Length == 0)?(gPlayInfoConfig.Song = 0):(gPlayInfoConfig.Song = 1);
            EngLinkRead(&(gPlayInfoCur.SongList), gPlayInfoConfig.Song, gPlayInfoCur.CurFilePath);
        }
    } else {
        if (gPlayInfoCur.BrowserList.Length >= gPlayInfoConfig.Song){
            EngLinkRead(&(gPlayInfoCur.BrowserList), gPlayInfoConfig.Song, gPlayInfoCur.CurFilePath);       
        }else{ 
            (gPlayInfoCur.BrowserList.Length == 0)?(gPlayInfoConfig.Song = 0):(gPlayInfoConfig.Song = 1);
            EngLinkRead(&(gPlayInfoCur.BrowserList), gPlayInfoConfig.Song, gPlayInfoCur.CurFilePath);
        }
    }
    gPlayInfoCur.pCurDir = NULL; ///
    gPlayInfoCur.pCurFile = NULL;        ///
    return AUDIO_TRUE;
}

/**
 *\brief initial the time parameter of the player. 
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static int EngInitTimes()
{
    gPlayInfoCur.CurTime = CUR_TIME_INIT;
    gPlayInfoCur.SumTime = SUM_TIME_INIT;
    //gPlayInfoCur.SumTime = ADS(); //iris interface
    // no need to set progress bar to the beginning because it is there already!
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj

 *\date 2008-04-07
 *\return
 */
static int EngInitID3()
{
    EngSetId3(gPlayInfoCur.CurFilePath);
    return AUDIO_TRUE;
}

/**
 *\brief not commented
 *\author yifj
 *\param data
 *\date 2008-04-07
 *\return
 */
void EngInit()
{
    int result;
    gPlayState = PLAYER_STOP;

    result = EngLoadConfig();
    if (result == AUDIO_FALSE) {
        EPRINTF(("can not load the configure! \n"));
        exit(0);
    }
    strcpy(gPlayInfoCur.FileBrower, gPlayInfoConfig.DirPath);
    printf("EngInit 0\n");
    result = EngLinkInit();
    assert(result == AUDIO_TRUE);
    printf("EngInit 1\n");
    result = EngReadPlayList();
    assert(result == AUDIO_TRUE);
//pao-shu
    printf("EngInit 2\n");
    result = EngReadBrowserList();
    assert(result == AUDIO_TRUE);
    printf("EngInit 3\n");
    result = EngInitCurFile();
    assert(result == AUDIO_TRUE);
    printf("EngInit 4\n");
    result = EngInitID3();
    assert(result == AUDIO_TRUE);
    printf("EngInit 5\n");
    result = EngInitTimes();
    assert(result == AUDIO_TRUE);
    printf("EngInit 6\n");
    HandModeSet(gPlayInfoConfig.PlayMode);
    printf("EngInit 7\n");
    HandID3Set(gPlayInfoCur.Id3);


    DPRINTF(("EngInit over! \n"));
}

/**
 *\brief end the engine module
 *\author yifj

 *\date 2008-04-07
 *\return
 */
void EngEnd()
{
    strcpy(gPlayInfoConfig.DirPath, gPlayInfoCur.FileBrower);
    EngLinkSave(&(gPlayInfoCur.SongList), gPlayInfoConfig.PlayList);
    EngSaveConfig();
}

//////for id3/////
#define GET_4BYTES(n)		(((*(n)) << 24) | ((*((n)+1)) << 16) | \
				((*((n)+2)) << 8) | ((*((n)+3))))
#define GET_3BYTES(n)		(((*(n)) << 16) | ((*((n)+1)) << 8) | ((*((n)+2))))

static unsigned long EngUnsync(unsigned long b0, unsigned long b1, unsigned long b2, unsigned long b3)
{   
    return (((long)(b0 & 0x7F) << (3*7)) |
            ((long)(b1 & 0x7F) << (2*7)) |
            ((long)(b2 & 0x7F) << (1*7)) | 
            ((long)(b3 & 0x7F) << (0*7)));
}

static int EngId3ReadFrame(FILE* fp, char VersionMain)
{
    char FrameId[5] = {0};
    int FrameSize = 0;
    char ReadBuf[10] = {0}, *Content = NULL;
    tFrameFlags FrameFlag = {0};
    // note: I ingnored the text encoding field
    if (VersionMain == 2){
        fread(ReadBuf, 6, 1, fp);
        memcpy(FrameId, ReadBuf, 3);
        FrameId[3] = 0;
        FrameSize = GET_3BYTES(ReadBuf + 3);
        if (strcmp(FrameId, "TT2") == 0){
            Content = (char *)malloc(FrameSize);
            if (Content == NULL){
                EPRINTF(("memory alloc error! \n"));
                return FrameSize;
            }
            fread(Content, FrameSize, 1, fp);
            strcpy(gPlayInfoCur.Id3.SongName, Content+1); /// FIXME, the MAX_SONG_NAME and ... are not longer fit!
            free(Content);
        }else if(strcmp(FrameId, "TP1") == 0){
            Content = (char *)malloc(FrameSize);
            if (Content == NULL){
                EPRINTF(("memory alloc error! \n"));
                return FrameSize;
            }
            fread(Content, FrameSize, 1, fp);
            strcpy(gPlayInfoCur.Id3.SingerName, Content+1); /// FIXME, the MAX_SONG_NAME and ... are not longer fit!
            free(Content);
        }else if(strcmp(FrameId, "TAL") == 0){
            Content = (char *)malloc(FrameSize);
            if (Content == NULL){
                EPRINTF(("memory alloc error! \n"));
                return FrameSize;
            }
            fread(Content, FrameSize, 1, fp);
            strcpy(gPlayInfoCur.Id3.AlbumName, Content+1); /// FIXME, the MAX_SONG_NAME and ... are not longer fit!
            free(Content);
        }else if(strlen(FrameId) == 0){
            FrameSize = 0x40000000; // if meet the padding, return a big number to exit the loop.
            return FrameSize;
        }else{
            fseek(fp, FrameSize, SEEK_CUR);
            DPRINTF(("i don't care about this frame! \n"));
        }
    }else if((VersionMain == 3) || (VersionMain == 4)){
        fread(ReadBuf, 10, 1, fp);
        memcpy(FrameId, ReadBuf, 4);
        FrameId[4] = 0;
        FrameSize = GET_4BYTES(ReadBuf + 4);
        FrameFlag.a =(ReadBuf[8]>>(8-1))|(ReadBuf[8]<<1);
        FrameFlag.b =(ReadBuf[7]>>(8-2))|(ReadBuf[7]<<2);
        if ((FrameFlag.a != 0) || (FrameFlag.b != 0)){
            EPRINTF(("the frame should be discard! \n"));
            return FrameSize;
        }
        if (strcmp(FrameId, "TIT2") == 0){
            Content = (char *)malloc(FrameSize);
            if (Content == NULL){
                EPRINTF(("memory alloc error! \n"));
                return FrameSize;
            }
            fread(Content, FrameSize, 1, fp);
            strcpy(gPlayInfoCur.Id3.SongName, Content+1); /// FIXME, the MAX_SONG_NAME and ... are not longer fit!
            free(Content);
        }else if(strcmp(FrameId, "TPE1") == 0){
            Content = (char *)malloc(FrameSize);
            if (Content == NULL){
                EPRINTF(("memory alloc error! \n"));
                return FrameSize;
            }
            fread(Content, FrameSize, 1, fp);
            strcpy(gPlayInfoCur.Id3.SingerName, Content+1); /// FIXME, the MAX_SONG_NAME and ... are not longer fit!
            free(Content);
        }else if(strcmp(FrameId, "TALB") == 0){
            Content = (char *)malloc(FrameSize);
            if (Content == NULL){
                EPRINTF(("memory alloc error! \n"));
                return FrameSize;
            }
            fread(Content, FrameSize, 1, fp);
            strcpy(gPlayInfoCur.Id3.AlbumName, Content+1); /// FIXME, the MAX_SONG_NAME and ... are not longer fit!
            free(Content);
        }else if(strlen(FrameId) == 0){
            FrameSize = 0x40000000; // if meet the padding, return a big number to exit the loop.
            return FrameSize;
        }else{
            DPRINTF(("i don't care about this frame! \n"));
        }
    }else
        assert(0);
    return FrameSize;
}

static int EngId3One(char FileName[])
{
    FILE *fp;
    char id3tag[3];
    fp = fopen(FileName, "rb");
    if (fp == NULL) {
        EPRINTF(("can not open the file! \n"));
        return AUDIO_FALSE;
    }
    fseek(fp, -128, SEEK_END);
    fread(id3tag, 3, 1, fp);

    if ((id3tag[0] != 'T') || (id3tag[1] != 'A') || (id3tag[2] != 'G')) {
        fclose(fp);
        gPlayInfoCur.Id3.SongName[0] = 0;
        gPlayInfoCur.Id3.SingerName[0] = 0;
        gPlayInfoCur.Id3.AlbumName[0] = 0;
        return AUDIO_FALSE;
    }
    fread(gPlayInfoCur.Id3.SongName, 30, 1, fp);
    gPlayInfoCur.Id3.SongName[30] = 0;
    fread(gPlayInfoCur.Id3.SingerName, 30 - 1, 1, fp);
    gPlayInfoCur.Id3.SingerName[30] = 0;
    fread(gPlayInfoCur.Id3.AlbumName, 30, 1, fp);
    gPlayInfoCur.Id3.AlbumName[30] = 0;
    fclose(fp);
    return AUDIO_TRUE;
}

static int EngId3Two(char FileName[])
{
    FILE *fp;
    char ReadBuf[10];
    char VersionMain;
    tId3Flags Id3flag = {0,0,0,0};
    int TagSize = 0, FrameSize = 0, i;
    gPlayInfoCur.Id3.SongName[0] = 0;
    gPlayInfoCur.Id3.SingerName[0] = 0;
    gPlayInfoCur.Id3.AlbumName[0] = 0;
    
    fp = fopen(FileName, "rb");
    if (fp == NULL) {
        EPRINTF(("can not open the file! \n"));
        return AUDIO_FALSE;
    }  
    fread(ReadBuf, 10, 1, fp);
    if ((ReadBuf[0] != 'I') || (ReadBuf[1] != 'D') || (ReadBuf[2] != '3')) {
        fclose(fp);
        return AUDIO_FALSE;
    }
    VersionMain = ReadBuf[3];
    Id3flag.a =(ReadBuf[5]>>(8-1))|(ReadBuf[5]<<1);
    Id3flag.b =(ReadBuf[5]>>(8-2))|(ReadBuf[5]<<2);
    Id3flag.c =(ReadBuf[5]>>(8-3))|(ReadBuf[5]<<3);
    Id3flag.d =(ReadBuf[5]>>(8-4))|(ReadBuf[5]<<4);
    DPRINTF(("flag.a = %d, flag.b = %d, flag.c = %d, flag.d = %d \n ",
            Id3flag.a, Id3flag.b, Id3flag.c, Id3flag.d));
    TagSize = EngUnsync(ReadBuf[6], ReadBuf[7], ReadBuf[8], ReadBuf[9]);
            
 // I ignored the flags about EngUnsynchronisation, compressin, experiental and footer for convinient.   
    if (VersionMain == 2){ // version 2.0
        for (i = 0; i < TagSize; i += FrameSize){
            FrameSize = EngId3ReadFrame(fp, VersionMain);
        }            
    }else if (VersionMain == 3){ // version 2.3
        int ExtHeadSize = 0;
        fread(ReadBuf, 4 ,1, fp);
        ExtHeadSize = GET_4BYTES(ReadBuf);
        fseek(fp, ExtHeadSize, SEEK_CUR);
        for (i = 0; i < TagSize; i += FrameSize){
            FrameSize = EngId3ReadFrame(fp, VersionMain);
        }  
    }else if (VersionMain == 4){ // version 2.4
        /// extend header FIXME
        int ExtHeadSize = 0;
        fread(ReadBuf, 4 ,1, fp);
        ExtHeadSize = EngUnsync(ReadBuf[0], ReadBuf[1], ReadBuf[2], ReadBuf[3]);
        fseek(fp, ExtHeadSize - 4, SEEK_CUR);
        for (i = ExtHeadSize; i < TagSize; i += FrameSize){
            FrameSize = EngId3ReadFrame(fp, VersionMain);
        }  
    }else{
        EPRINTF(("the wrong major version ! \n"));
        fclose(fp);
        return AUDIO_FALSE;
    }
    fclose(fp);
    return AUDIO_TRUE;
}


// this function only abstract three fields from the id3 tag. it is a subset.
// Note: not all field handeld here!
int EngSetId3(char FileName[])
{
    if (EngId3One(FileName) == AUDIO_TRUE)
        return AUDIO_TRUE;
    else if (EngId3Two(FileName) == AUDIO_TRUE)
        return AUDIO_TRUE;
    else 
        return AUDIO_FALSE;
}

