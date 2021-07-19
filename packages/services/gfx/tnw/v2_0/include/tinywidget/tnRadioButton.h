#ifndef _TNRADIOBUTTON_H_
#define _TNRADIOBUTTON_H_

#include "tnBase.h"
#include "tnRadioButtonGroup.h"
#include <microwin/nano-X.h>
		
#define RADIOBUTTON_CALLBACKS   0
#define TN_RADIOBUTTON_HEIGHT  13
#define TN_RADIOBUTTON_WIDTH    1

/* Macro to chaeck if the Radio Button is Active */
#define TN_RADIOBUTTONACTIVE(widget) widget->WSpec.radiobutton.st_down 

/* Structure to store data for a Radiobutton Object */
typedef struct
{
	  char *caption;
	  GR_COLOR FGColor;
	  GR_BOOL st_down;
	  GR_BOOL bFocus;
	  GR_IMAGE_ID imageid[4];
	  PRDBGROUP	group;
	  int keymap[4];
}
TN_STRUCT_RADIOBUTTON;


TN_WIDGET* CreateRadioButton (TN_WIDGET *parent, int flags, GR_RECT *rect,
	char *caption, GR_IMAGE_ID imageid[], PRDBGROUP group, int keymap[]);
void RadioButtonEventHandler(GR_EVENT *,TN_WIDGET *);
void DrawRadioButton (TN_WIDGET *);
void DestroyRadioButton(TN_WIDGET *);
int tnSetRadioButtonCaption(TN_WIDGET *,char *);
int tnGetRadioButtonCaption(TN_WIDGET *, char *);
int tnSetRadioButtonStatus(TN_WIDGET *,GR_BOOL);
int tnGetRadioButtonStatus(TN_WIDGET *widget,GR_BOOL *status);
#endif /*_TNRADIOBUTTON_H_*/

