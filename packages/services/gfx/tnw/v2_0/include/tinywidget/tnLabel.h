#ifndef _TNLABEL_H_
#define _TNLABEL_H_

#include "tnBase.h"
#include <microwin/nano-X.h>

typedef struct
{
  	char 		*caption;		/*Label caption */
  	GR_IMAGE_ID 	iBack;		/*background color */
  	GR_WINDOW_ID 	wBack;		/*background color  fron parent windows*/
}
TN_STRUCT_LABEL;

TN_WIDGET *CreateLabel(TN_WIDGET *parent, int flags, GR_RECT *rect, char *caption, GR_IMAGE_ID img, GR_WINDOW_ID wnd);

void DrawLabel (TN_WIDGET *);
int tnUpdateLabelCaption(TN_WIDGET *widget, char *caption);
void LabelEventHandler (GR_EVENT *, TN_WIDGET *);
void DestroyLabel(TN_WIDGET *);
int tnSetLabelCaption(TN_WIDGET *, char *);
int tnGetLabelCaption(TN_WIDGET *, char *);
int tnSetLabelCaptionColor(TN_WIDGET *,TN_COLOR);
TN_COLOR tnGetLabelCaptionColor(TN_WIDGET *);
int tnGetLabelImage(TN_WIDGET *widget,GR_IMAGE_ID iid);
#endif /*_TNLABEL_H_*/
