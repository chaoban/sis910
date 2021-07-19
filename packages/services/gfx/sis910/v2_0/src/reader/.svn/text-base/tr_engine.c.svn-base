/*Header file for the Text Reader widget
    * This file is part of `TinyWidgets', a widget set for the nano-X GUI which 
    * is a part of the Microwindows project (www.microwindows.org).
    * Copyright C 2000
    * Sunil Soman <sunil_soman@vsnl.com>
    * Amit Kulkarni <amms@vsnl.net>
    * Navin Thadani <navs@vsnl.net>
    *
    * This library is free software; you can redistribute it and/or modify it
    * under the terms of the GNU Lesser General Public License as published by 
    * the Free Software Foundation; either version 2.1 of the License, 
    * or (at your option) any later version.
    *
    * This library is distributed in the hope that it will be useful, but 
    * WITHOUT ANY WARRANTY; without even the implied warranty of 
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
    * General Public License for more details.
    *
    * You should have received a copy of the GNU Lesser General Public License 
    * along with this library; if not, write to the Free Software Foundation, 
    * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
  */

#include "tnWidgets.h"
#include<string.h>
#include<ctype.h>
#include <assert.h>
#include "tr_engine.h"
#include <sys/stat.h>
#include <unistd.h>

//static char * gtrbuffer;   //the buffer to store the file context

/**
*
*/
int TrInitEgInfo(tEngineInfo* pEginfo,TN_WIDGET *textreader)
{
    GR_GC_INFO  gcinfo;
    GR_FONT_ID font;
    GR_FONT_INFO fontinfo;
    GR_WINDOW_INFO winfo;  
    GR_SIZE charheight,height;
    if (pEginfo==NULL) return -1;
    if (textreader==NULL)  return -1;
    
    pEginfo->line = NULL;
    pEginfo->fp=NULL;
    pEginfo->curline=0;
    GrGetGCInfo(textreader->gc,&gcinfo);
    font=gcinfo.font;
    GrGetFontInfo (font, &fontinfo);
    charheight = fontinfo.height;
    GrGetWindowInfo(textreader->wid,&winfo);   
    height=winfo.height;
    pEginfo->lines = (height - 4) / charheight;  
    pEginfo->lineheight=charheight;
    //init buffer
    pEginfo->buffer=NULL;
    pEginfo->index=0;
    return 0;
}
/**
*@return: 1, if get one line
*@            0, error
*/
static int  getOneLine(char *buffer,TN_WIDGET *textreader, tEngineInfo* pEginfo,int *pos)	
{
    GR_WINDOW_INFO winfo;	
    int textwidth,textheight,textbase;
    if (buffer==NULL) return 0;
    if (pEginfo==NULL) return 0;
    //if (pEginfo->fp==NULL)  return 0;    
    GrGetWindowInfo(textreader->wid,&winfo);   
    int ret=0;
    int i=0;
    textwidth=0;
    printf("getOneLine: width %d margine %d\n",winfo.width,MARGINE);
    while(textwidth<(winfo.width-2*MARGINE))
    {
        if((pEginfo->index)>=(pEginfo->size)) break;  //the end if file
        buffer[i]=pEginfo->buffer[pEginfo->index];
        pEginfo->index++;
        if (pos) (*pos)++;                    
        //the '\r' and '\n' may be the first character
        if ((buffer[i]=='\n') || (buffer[i]=='\r'))
        {
            if (buffer[i]=='\r')
            {
               	buffer[i]=pEginfo->buffer[pEginfo->index];
                pEginfo->index++;
                if (pos) (*pos)++;                                    
            }
            buffer[i]='\0';      
            break;   
        }
        buffer[i+1]='\0';	
        GrGetGCTextSize(textreader->gc,buffer,-1,GR_TFASCII|GR_TFBOTTOM,&textwidth,&textheight,&textbase);
        printf("getOneLine while: width %d textheight %d\n",textwidth,textheight);
        i++;	                
    }
    return 1;
    //while(textwidth<winfo.width-2*MARGINE);
}
//Get the total lines of current txt file
/******************************************************************
*
********************************************************************/
void TrGetTextLines(TN_WIDGET *textreader,tEngineInfo* pEginfo)
{
    int curpos;
    int lines=0;
    int i=0;
    if (pEginfo->buffer==NULL) return;
    char*context=(char *)malloc(TEXT_BUFFER_SIZE*sizeof(char));     //store one line
    assert(context);
    if (context==NULL)  return;
    if (pEginfo->buffer==NULL)
    {	  
        printf("file hasn't been loaded\n");
        return;
    }
    
    printf("GetTextline-- filename:%s\n",pEginfo->filepath);
    printf("GetTextline-- the max lines of each page:%d\n",pEginfo->lines);
    //get the total lines at first 
    printf("GetTextline-- the max lines of each page:%d\n",pEginfo->size);
    while ((pEginfo->index)<(pEginfo->size))
    {
        printf("GetTextline-- index:%d, lines:%d\n",pEginfo->index,lines);
        getOneLine(context, textreader,pEginfo,NULL);
        lines++;
        //if (lines>1000) assert(0);
     }
    printf("GetTextLines---Total lines of the file:%d\n",lines);
    pEginfo->linecount=lines;
    //alloc the buffer, note the size, avoid exceed the boundry
    pEginfo->line=(int *)malloc((lines+4)*sizeof(int));
    assert(pEginfo->line);
    //store the index of each line
    lines=0;
    curpos=0;
    pEginfo->index=0;  //offset to beginning of the buffer
    pEginfo->line[0]=0;            
    lines++;
    while((pEginfo->index)<(pEginfo->size))
    {
        getOneLine(context, textreader,pEginfo,&curpos);
        pEginfo->line[lines]=curpos;        
        lines++;
        // assert(lines!=500);
    }   
    free(context);
   
}	
/****************************************************************
*draw a page Down
*****************************************************************/
void TrDrawPageDown(TN_WIDGET *textreader,tEngineInfo* pEginfo,int position)
{
    int lines;
    char* context=(char *)malloc(TEXT_BUFFER_SIZE*sizeof(char));     //store one line
    assert(context);
    if (context==NULL) return;
    //char *context=textreader->WSpec.textreader.buffer;
    if (pEginfo->buffer==NULL)
    {	  
        printf("file hasn't been loaded\n");
        return;
    }
    //fseek(pEginfo->fp,position,SEEK_SET);
    pEginfo->index=position;
    lines=0;
    while((pEginfo->index<pEginfo->size)&&(lines<pEginfo->lines))
    {	
        getOneLine(context,textreader,pEginfo,NULL);
        GrText(textreader->WSpec.window.pid, textreader->gc, MARGINE,(lines+1)*pEginfo->lineheight,
               context, -1, GR_TFASCII | GR_TFBOTTOM);	
        lines++;
    }            
    free(context);     
}
/*****************************************************************
* load the txt file 
*******************************************************************/
int TrLoadFile(TN_WIDGET *textreader,const char *filename,int curline,tEngineInfo* pEginfo)
{
    int size;
    FILE *fp;
    struct stat fileinfo;
    if (stat(filename,&fileinfo)!=0)  return 0;
    if (fileinfo.st_size>MAX_FILE_SIZE) return 0;
    fp=fopen(filename,"rb");
    if (fp==NULL)
    {
    	printf("---can't open the file\n");
    	return 0;
    }
    else
    printf("---File has been open\n");
    
    TrResetEngine(pEginfo);
    //allocate the buffer
    pEginfo->buffer=(char *)malloc(fileinfo.st_size*sizeof(char));
    assert(pEginfo->buffer);
    if (pEginfo->buffer==NULL) return 0;
    //load the file
    size=fread(pEginfo->buffer,1,fileinfo.st_size,fp);
    fclose(fp);
    assert(size==fileinfo.st_size);
    if (size!=fileinfo.st_size)  return 0;
    //set FILE ptr and lines
    strcpy(pEginfo->filepath,filename);
    //pEginfo->fp=fp;
    pEginfo->size=size;
    
    pEginfo->curline=0;
    pEginfo->index=0;
    
    
    TrGetTextLines(textreader,pEginfo);    
    pEginfo->curline=curline;
    //textreader->WSpec.textreader.buffer=(char *)malloc(TEXT_BUFFER_SIZE*sizeof(char));
    printf("load file success:%s\n",filename);
    return 1;
}
/*********************************************************************
*
**********************************************************************/
void TrCloseFile(tEngineInfo* pEginfo)
{
    TrResetEngine(pEginfo);
}

void TrResetEngine(tEngineInfo* pEginfo)
{
    pEginfo->index=0;
    if(pEginfo->line!=NULL)
    {
	    free(pEginfo->line);
	    pEginfo->line=NULL;
    }
    if (pEginfo->buffer!=NULL)
    {
	    free(pEginfo->buffer);	  
	    pEginfo->buffer=NULL;
    }    
}
/*
void
DestroyTextReader (TN_WIDGET * widget)
{
    ResetTextReader(widget);
    DeleteFromRegistry (widget);
    return;
}*/

/*
void strrev(char *s)
{
	int i,j;
	char temp;
	for(i=0,j=strlen(s)-1;i<j;i++,j--)
	{
		temp=s[j];
		s[j]=s[i];
		s[i]=temp;
	}
}*/
