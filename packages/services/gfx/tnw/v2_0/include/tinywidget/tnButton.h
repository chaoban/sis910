#ifndef _TNBUTTON_H_
#define _TNBUTTON_H_

#include "tnBase.h"
#include <microwin/nano-X.h>

typedef struct
{
	char		*caption;
	GR_IMAGE_ID	imageid[2];
	GR_BOOL 	bFocus;
	int keymap[4];
}
TN_STRUCT_BUTTON;

/** create push button widget */
TN_WIDGET *CreateButton(TN_WIDGET *parent, int flags, GR_RECT *rect, char *caption, GR_IMAGE_ID iid[], int keymap[]);
void ButtonEventHandler (GR_EVENT *, TN_WIDGET *);
void DrawButton (TN_WIDGET *);
void DestroyButton(TN_WIDGET *);
int tnGetButtonPressed(TN_WIDGET *);
int tnSetButtonPixmap(TN_WIDGET *,char *);
int tnRemoveButtonPixmap(TN_WIDGET *);
int tnSetButtonCaption(TN_WIDGET *,char *);
int tnGetButtonCaption(TN_WIDGET *,char *);
int tnSetButtonImage(TN_WIDGET *widget, GR_IMAGE_ID iid[]);

#endif /*_TNBUTTON_H_*/

