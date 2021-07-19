#ifndef _RECENT_H_
#define _RECENT_H_
 

#include<stdio.h>
//#include<nano-X.h>
#include "tinywidget/tnWidgets.h"
#include "../share/ecos_config.h"

#define RECENT_NUM  4


int initRecent(char *recent[],char * filepath);
int addToRecent(int index,char *recent[],char * path);
void addRecentToListBox(int index,const char *recent[],TN_WIDGET *widget);
void SaveRecentFile(char * recent[],int index,char * file);

void freeRecent(char *recent[]);


#endif  //_RECENT_H_
