#ifndef _TNCHECKBUTTON_H_
#define _TNCHECKBUTTON_H_

#include "tnBase.h"
#include <microwin/nano-X.h>
#define TN_CHECKBUTTONACTIVE(widget) widget->WSpec.checkbutton.st_down	/*Returns status of checkbutton */

typedef struct
{
  	char 		*caption;	/*caption string for Checkbutton */
  	GR_BOOL 	st_down;	/*Checkbutton status */
  	GR_BOOL 	bFocus;
  	GR_COLOR    clr_caption;
  	GR_IMAGE_ID 	imageid[4];
  	int keymap[4];
}
TN_STRUCT_CHECKBUTTON;

TN_WIDGET *CreateCheckButton (TN_WIDGET * parent, int flags, GR_RECT *rect,
	char *caption, GR_IMAGE_ID imageid[], int keymap[]);
void CheckButtonEventHandler (GR_EVENT *, TN_WIDGET *);
void DrawCheckButton (TN_WIDGET *);
void DestroyCheckButton(TN_WIDGET *);
int tnSetCheckButtonCaption(TN_WIDGET *,char *);
int tnGetCheckButtonCaption(TN_WIDGET *,char *);
int tnSetCheckButtonStatus(TN_WIDGET *, GR_BOOL);
int tnGetCheckButtonStatus(TN_WIDGET *widget, GR_BOOL *status); 
void tnSetCheckButtonCaptionColor(TN_WIDGET *widget,GR_COLOR color);
int tnGetCheckButtonCaptionColor(TN_WIDGET *widget,GR_COLOR *color);
void SetCheckButtonImageID (TN_WIDGET * widget,GR_IMAGE_ID imageid1,
     GR_IMAGE_ID imageid2,GR_IMAGE_ID imageid3,GR_IMAGE_ID imageid4);                                 

#endif /*_TNCHECKBUTTON_H_*/
