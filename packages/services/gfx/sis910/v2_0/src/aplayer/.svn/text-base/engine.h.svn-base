#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "common.h"
#include "dirent.h"


/// need to change!!
#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define STEP_VOLUME 10

typedef UINT tSoundValue;
typedef UINT tPlayerTime;
typedef UINT tSongId;
typedef UINT tListId;

typedef enum {
    PLAYER_STOP,
    PLAYER_PLAY,
    PLAYER_PAUSE
} tPlayerState;

typedef enum {
    NO_CIRCUS,
    REPEAT_CURRENT,
    CIRCUS_AROUND
} tPlayMode;

typedef enum {
    FROM_FILEBROWSER,
    FROM_PLAYLIST
} tEntryMode;


//////Link////////
struct Node {
    char *content;
    struct Node *next;
};
typedef struct Node tLinkNode;
typedef struct Node *tpLinkNode;

typedef struct {
    int Length;
    tpLinkNode pHeader;
} tLink;
typedef tLink *tpLink;
/////over//////////


////inital value ////
#define CUR_TIME_INIT 0
#define SUM_TIME_INIT 100
///over /////


#define MAX_SONG_NAME 100
#define MAX_SINGER_NAME 100
#define MAX_ALBUM_NAME 100

typedef struct {
    char SongName[MAX_SONG_NAME];
    char SingerName[MAX_SINGER_NAME];
    char AlbumName[MAX_ALBUM_NAME];
} tId3;                         // support only ID3v1

#define MAX_PL_PATH 100
#define MAX_FILE_PATH 100
#define MAX_DIR_LEN 100
#define MAX_FILENAME_LEN 50

typedef struct {
    ////need to record
    tSoundValue Sound;
    tPlayMode PlayMode;
    tEntryMode EntryMode;
    // available when MODE in CURRENT_LIST_FILE, CIRCUS_LIST
    char PlayList[MAX_PL_PATH];
    // available when MODE in CURRENT_FOLDER_FILE, CIRCUS_FOLDER
    char DirPath[MAX_DIR_LEN];
    tSongId Song;          // it indicates the order in FB or playlist. player engine finds audio files by it
} tPlayInfoConfig;

typedef struct {
    ////temp
    tId3 Id3;
    tPlayerTime CurTime;
    tPlayerTime SumTime;
    tLink SongList;             // playItem list
    tLink BrowserList;          // maybe don't need it, besause it may be changed out the control of the playing AUDIO player
    char CurFilePath[MAX_FILE_PATH];
    DIR *pCurDir;               // need it or delete it?
    FILE *pCurFile;             // need it or delete it?
    char FileBrower[MAX_DIR_LEN];
    int FBFileNum;              // describe the File number of the current file browser path
    int FBFolderNum;            // describe the folder number of the current file browser path
} tPlayInfoCur;

//// for id3 ////
typedef struct{
    char a  :   1;
    char b  :   1;
    char c  :   1;
    char d  :   1;
}tId3Flags;

typedef struct{
    char a  :   1;
    char b  :   1;
    // ignore other flags
}tFrameFlags;
/////end ////////

#ifndef LONG_FILENAME
int EngGetDir(char *Dir, struct dirent ***pNameList, int *pNum);
void EngFreeDir(struct dirent **ppNameList, int Num);
#else
int EngGetDir(char * Dir, char *** pNameList, int * pNum);
void EngFreeDir(char ** ppNameList, int Num);
#endif //LONG_FILENAME

int EngLinkRead(tpLink pLink, int Pos, char Content[]);
int EngLinkSave(tpLink pLink, char FileName[]);
int EngLinkDestroy(tpLink pLink);
int EngAddPlayitem(char *Content);
int EngDelPlayitem(int DelNum);
int EngUpPlayitem(tpLink pLink, int SelNum);
int EngDownPlayitem(tpLink pLink, int SelNum);
void EngSetPlayState(tPlayerState State);
tPlayerState EngGetPlayState();
tPlayInfoConfig *EngGetInfoConfig();
tPlayInfoCur *EngGetInfoCur();
int EngSetSound(tSoundValue Value);
int EngGetSound(tSoundValue * pValue);
int EngSetId3(char FileName[]);
int EngReadBrowserList();
int EngPathToName(char Path[], char FileName[]);
int EngChMode(tPlayMode * PlayMode);
void EngInit();
void EngEnd();

#endif // __ENGINE_H__
