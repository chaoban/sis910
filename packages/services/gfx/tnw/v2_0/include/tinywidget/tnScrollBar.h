#ifndef _TNSCROLLBAR_H_
#define _TNSCROLLBAR_H_
#define TN_SCROLLBAR_PAGESTEP 5
#define TN_SCROLLBAR_LINESTEP 1
#define TN_SCROLLBAR_MINVAL 0
#define TN_SCROLLBAR_MAXVAL 100
#define TN_SCROLLBAR_THUMBSIZE 10
#define TN_SCROLLBAR_VERTICAL_HEIGHT 160
#define TN_SCROLLBAR_VERTICAL_WIDTH 20
#define TN_SCROLLBAR_HORIZONTAL_HEIGHT 20
#define TN_SCROLLBAR_HORIZONTAL_WIDTH 160
#define TN_SCROLLBAR_COMPONENT_SIZE 20
#define TN_VERTICAL 0
#define TN_HORIZONTAL 1

#define TN_SCROLL_NOSCROLL	0
#define TN_SCROLL_LINEUP 	1
#define TN_SCROLL_LINEDOWN 	2
#define TN_SCROLL_PAGEUP 	3
#define TN_SCROLL_PAGEDOWN	4
#define TN_SCROLL_THUMBMOVE	5

#define TN_MIN_THUMBSIZE 	5

#include "tnBase.h"
#include <microwin/nano-X.h>

typedef struct
{
  int orientation;
  GR_SIZE thumbsize;
  int minval;
  int maxval;
  int pagestep;
  int linestep;
  int thumbpos;
  int LastScrollEvent;
  //GR_WINDOW_ID upleft;
  //GR_WINDOW_ID downright;
  //GR_WINDOW_ID thumb; 
  //GR_BOOL st_upleft_down;	
  //GR_BOOL st_downright_down;
  GR_BOOL st_thumb_down;	 
  GR_IMAGE_ID imageid[2];
}
TN_STRUCT_SCROLLBAR;

TN_WIDGET *CreateScrollBar(TN_WIDGET * parent, int flags, GR_RECT *rect, int orientation,
	      int minval, int maxval, int linestep, int pagestep, GR_IMAGE_ID imageid[]);
void ScrollBarEventHandler (GR_EVENT *, TN_WIDGET *);
void DrawScrollBar (TN_WIDGET *);
/*void DrawUpLeft(TN_WIDGET * );
void DrawDownRight(TN_WIDGET *);
void DrawThumb(TN_WIDGET *);*/
int tnGetLastScrollEvent(TN_WIDGET *);
int tnSetLastScrollEvent(TN_WIDGET *widget, int lastevent);
int tnGetThumbPosition(TN_WIDGET *);
int tnSetScrollRange(TN_WIDGET *,int,int);
int tnSetThumbPosition(TN_WIDGET *,int);
void DestroyScrollBar(TN_WIDGET *);
int tnGetScrollBarOrientation(TN_WIDGET*, int*);
int tnGetScrollRange(TN_WIDGET *,int *,int *);
int tnGetScrollStepSizes(TN_WIDGET *,int *,int *);
int tnSetScrollStepSizes(TN_WIDGET *,int ,int );
int tnSetScrollBarOrientation(TN_WIDGET*,int);

#endif /*_TNSCROLLBAR_H_*/
