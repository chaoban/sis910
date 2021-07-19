/**
 *\file common.c
 *\brief define some popular functions which have no relationship with a special
   module
 *\author yifj
 *\date 2008-04-07
 *
 *$Id$
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include "common.h"
#include "display.h"
#include "handle.h"
#include "engine.h"
#include "assert.h"
#include "../desktop/global.h"

#include"dirent.h"
#include"sys/stat.h"
//#include"signal.h"
#include"unistd.h"
#ifdef __ECOS
#include "cyg/kernel/kapi.h"
#include "pkgconf/io_fileio.h"
#include "pkgconf/fs_fat.h"
#include "cyg/fileio/fileio.h"
#include "cyg/fs/fatfs.h"

#endif 

#define INITIAL_SIZE        4096

/**
 *\brief give up the cpu time to other threads. It is usually used when waitting in the loops
 *\author yifj
 *\date 2008-04-07
 *\return
 */
void ApRelinquish()
{
#ifdef __ECOS
    cyg_thread_yield();
    //cyg_thread_delay(10);
#else
    struct timespec req;
    int ret;
    req.tv_sec = 0;
    req.tv_nsec = 50 * 1000;
    ret = nanosleep(&req, NULL);
    if (-1 == ret) {
        EPRINTF(("\t nanosleep not support\n"));
    }
    //sleep(1);
#endif // end of __ECOS
}


/**
 *\brief scan the special dir and get the dirs and files in it. It is just like the function scandir in linux
 *\author yifj
 *\param name the special dir name which is need to scan.
 *\param list the stucture to store the got dirs and files
 *\param selector to filte the files with file type.
 *\param sorter the sorting agorithm.
 *\date 2008-04-07
 *\return -1:faluire   i>0:the got items in list
 */
#ifndef LONG_FILENAME
int ApScanDir(char *name, struct dirent ***list, int (*selector) (struct dirent *),
              int (*sorter) (const void *, const void *))
#else
int ApScanDir(char * name, char *** list, int(* selector)(struct dirent *),
              int(* sorter)(const void *, const void *))
#endif //LONG_FILENAME
{
#ifdef __ECOS

#ifndef LONG_FILENAME
    struct dirent **names;
    struct dirent *entp;
    DIR *dirp;
    int i;
    int size;
    int len = 0, flag = 0;
    int dirent_size = sizeof(struct dirent);

    // Get initial list space and open directory. 
    size = INITIAL_SIZE;
    names = (struct dirent **) malloc(size * sizeof names[0]);
    if (names == NULL)
        return -1;

    /////// because the opendir in ecos cannot open the path with '/' at the end.
    len = strlen(name);
    if (name[len - 1] == '/') {
        if (name[len - 2] != '.') {
            name[len - 1] = 0;
            flag = 1;
        } else
            strcpy(name, "/");
    }

    dirp = opendir(name);

    if (flag == 1) {
        name[len - 1] = '/';
        flag = 0;
    }
    ////// '/'

    if (dirp == NULL)
        return -1;
    i = 0;
    // Read entries in the directory. 
    while ((entp = readdir(dirp)) != NULL)
        if (selector == NULL || (*selector) (entp)) {
            // User wants them all, or he wants this one. 
            if (++i >= size) {
                size <<= 1;
                names = (struct dirent **)
                    realloc((char *) names, size * sizeof names[0]);
                if (names == NULL) {
                    closedir(dirp);
                    return -1;
                }
            }

            // Copy the entry. 
            names[i - 1] = (struct dirent *) malloc(sizeof(struct dirent)
                                                    + MAX_FILE_PATH + 1);       //hacked by nifei
            if (names[i - 1] == NULL) {
                closedir(dirp);
                return -1;
            }
            memcpy(names[i - 1], entp, dirent_size);
            memcpy(names[i - 1]->d_name, entp->d_name, MAX_FILE_PATH);
            names[i - 1]->d_name[MAX_FILE_PATH] = '\0';
        }

    // Close things off. 
    names[i] = NULL;
    *list = names;
    closedir(dirp);

    // Sort? 
    if (i && sorter)
        qsort((char *) names, i, sizeof names[0], sorter);

    return i;

#else
    int ret = 0, counter = 0, len = 0, flag = 0;
    struct vfat_descriptor fd,*pfd;

    len = strlen(name);
    if (name[len - 1] == '/') {
        if (name[len - 2] != '.') {
            name[len - 1] = 0;
            flag = 1;
        } else
            strcpy(name, "/");
    }
    
    ret = listdir(name, &fd );
    strcpy(fd.next->filename, ".");
    fd.next->ext_name[0] = 0;
    fd.next->attr = 0x10;
    if (flag == 1) {
        name[len - 1] = '/';
        flag = 0;
    }
    
    if ( ret == 0 ){
        DPRINTF(("no entry in dir! \n"));
        return 0;
        }
    if ( ret < 0){
        EPRINTF(("error when listdir! \n"));
        return -1;
    }
    pfd = &fd;
    *list = (char **)malloc(ret * sizeof((*list)[0]));
    do{
        fd = *fd.next;
        (*list)[counter] = (char *)malloc(strlen(fd.filename)+1);
        if ((*list)[counter] == NULL){
            EPRINTF(("cannot malloc! \n"));
            break;
        }
        strcpy((*list)[counter], fd.filename);
        counter++;
    }while(fd.next != NULL);
    
    close_listdir(pfd);
/*
    if (counter != ret){
        while(counter--){
            free((*list)[counter]);
        }
        free(*list);
        return 0;
    }
*/
    return counter;
#endif  //LONG_FILENAME

#else

    return scandir(name, list, 0, alphasort);

#endif // end of __ECOS

}


/**
 *\brief translate the relative path to the absolute path.
 *\author yifj
 *\param fpath the string to discribe the path which is need to be translated,
   and after calling this function, the content of this string will be replaced 
   with the translated absolute path.
 *\date 2008-04-07
 *\return AUDIO_TRUE when succed
 */
int ComPathReltoAbs(char *pFilePath)
{
    char AbsPath[MAX_FILE_PATH];

    getcwd(AbsPath, MAX_FILE_PATH);
    //strcat(AbsPath, "/");
    strcat(AbsPath, pFilePath);
    strcpy(pFilePath, AbsPath);
    return AUDIO_TRUE;
}

/**
 *\brief transform all the character of a string to upper case. This operation should be done when open or write a file with a 
 string to indicate the file name when under linux platform, because of the ECOS ingoring the case of file name. The strings which
 are common in character but different in case should be treated as the identical file.
 *\author yifj
 *\param str
 *\date 2008-04-07
 *\return -1:failure, 0:succed
 */
int StrUpr(char *pStr)
{
    int len = 0, i;
    if (pStr == NULL)
        return -1;
    len = strlen(pStr);
    if (len <= 0)
        return -1;
    for (i = 0; i < len; i++)
        pStr[i] = toupper(pStr[i]);
    return 0;
}

/**
 *\brief free the memory alloted for store the list items. It should be used after calling tnGetSelectedListItems
 *\author yifj
 *\param str the memory alloted by tnGetSelectedListItems, which stores the items of a listbox.
 *\param m the number of items contained in str
 *\date 2008-04-07
 *\return
 */
void ComFreeList(char **ppStr, int Num)
{
    int i;
    for (i = 0; i < Num; i++)
        free(ppStr[i]);
    free(ppStr);
    return;
}

