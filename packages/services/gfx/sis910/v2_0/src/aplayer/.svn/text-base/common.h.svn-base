#ifndef __COMMON_H__
#define __COMMON_H__

#include"dirent.h"
#include"sys/stat.h"
#include"unistd.h"

#define AUDIO_DEBUG
#define UINT unsigned int

#ifdef AUDIO_DEBUG
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


#define DEC_OK
#ifdef __ECOS
#define LONG_FILENAME
#endif

#include "../share/ecos_config.h"

#define AUDIO_TRUE 0
#define AUDIO_FALSE -1

///////////////////////////////////
//#define CONFIG_PATH "resource/config.ini"
//#define RC_PATH "resource"
#if  0
#define MODE_PIC_NO_PATH "resource/Blue/APlayer/mode_cur.bmp"
#define MODE_PIC_REP_PATH "resource/Blue/APlayer/mode_repeat.bmp"
#define MODE_PIC_CIR_PATH "resource/Blue/APlayer/round_mode.bmp"
#define MODE_PIC_DEF_PATH "resource/Blue/APlayer/default_pic.bmp"
#else
#define MODE_PIC_NO_PATH "romdisk/resource/800x480/APlayer/201.JPG"
#define MODE_PIC_REP_PATH "romdisk/resource/800x480/APlayer/202.JPG"
#define MODE_PIC_CIR_PATH "romdisk/resource/800x480/APlayer/203.JPG"
#define MODE_PIC_DEF_PATH "romdisk/resource/800x480/APlayer/204.JPG"
#endif  //short file name to fit

#define ICON_FOLDER_PATH "romdisk/resource/800x480/APlayer/FolderI.JPG"
#define ICON_MUSIC_PATH "romdisk/resource/800x480/APlayer/MusicI.JPG"

#define APP_NAME "APlayer"
/////////////////////////////////

#define MAX_PLI_NUM 300

///////////special   need to fix
//#define ADDITION_ITEM 0
//////

/////functions declare ///
void ApRelinquish();
//////over ////

#ifndef __ECOS
extern char dpfFbOpenedFileName[512];
#endif //__ECOS

void ApRelinquish();
#ifndef LONG_FILENAME
int ApScanDir(char *name, struct dirent ***list, int (*selector) (struct dirent *),
              int (*sorter) (const void *, const void *));
#else
int ApScanDir(char * name, char *** list, int(* selector)(struct dirent *),
              int(* sorter)(const void *, const void *));
#endif //LONG_FILENAME
int ComPathReltoAbs(char *pFilePath);
int StrUpr(char *pStr);

#endif //end of __COMMON_H__
