 /*Header file for the Text Box widget
 * This file is part of `TinyWidgets', a widget set for the nano-X GUI which is  * a part of the Microwindows project (www.microwindows.org).
 * Copyright C 2000
 * Sunil Soman <sunil_soman@vsnl.com>
 * Amit Kulkarni <amms@vsnl.net>
 * Navin Thadani <navs@vsnl.net>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; either version 2.1 of the License, 
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License 
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *callback:CLICKED,CLOSED
 *
 */

#ifndef _TR_ENGINE_H_
#define _TR_ENGINE_H_
#include "tnBase.h"
#include<nano-X.h>

#define MAX_FILE_SIZE (2*1024*1024)      //2//2M
#define MARGINE   30
#define TEXT_BUFFER_SIZE 256
#define FILEPATH_MAX_LENGTH 128
typedef struct
{
  //GR_COLOR FGColor;
  int lineheight;
  int size;    //file size,in byte
  int lines;  //the lines of per page
  char filepath[FILEPATH_MAX_LENGTH];
  FILE *fp;               // the relative txt file pointer
  int *line;               //the array of the position of every pages
  int linecount;        //total lines of the file 
  int curline;           //current line number
  int index;   //the current position in the buffer 
  char *buffer;       //buffer to store the file context
  GR_KEY keystroke;
}tEngineInfo;
/*
TN_WIDGET *
CreateTextReader (TN_WIDGET * ,
              int ,
              int ,
	    //  char *,
              GR_SIZE ,
              GR_SIZE ,
              char * ,
              GR_SIZE ,
              GR_COLOR ,
              GR_COLOR 
	      );*/
int TrInitEgInfo(tEngineInfo* pEginfo,TN_WIDGET *textreader);
//void TextReaderEventHandler(GR_EVENT *,TN_WIDGET *);
void TrDrawPageDown(TN_WIDGET *textreader,tEngineInfo* pEginfo,int position);
void TrGetTextLines(TN_WIDGET *textreader,tEngineInfo* pEginfo);
int TrLoadFile(TN_WIDGET *textreader,const char *filename,int curline,tEngineInfo* pEginfo);
void TrResetEngine(tEngineInfo* pEginfo);
void TrCloseFile(tEngineInfo* pEginfo);
//void DestroyTextReader(TN_WIDGET *);

 
#endif /*_TR_ENGINE_H_*/
