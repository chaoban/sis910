#ifndef _BOOKMARKER_H_
#define _BOOKMARKER_H_
#include "stdio.h"
#include "tinywidget/tnWidgets.h"

#define BOOKMARKER_NUM  8

typedef struct{
    char *text;	
    char *path;
    int   posi;
}BMarker;

int initBookMarker(BMarker bm[],char * filepath);
int addToBM( BMarker bm[], int index,const char * text,const char * path, int posi);
void addBMToListBox(int index,BMarker bm[],TN_WIDGET *widget);
void saveBookmarker(BMarker bm[],int index,char * file);
void freeBM(BMarker bm[]);

#endif //_BOOKMARKER_H_
