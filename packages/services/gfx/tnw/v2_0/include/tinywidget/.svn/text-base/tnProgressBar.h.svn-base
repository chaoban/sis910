#ifndef _TNPROGRESSBAR_H_
#define _TNPROGRESSBAR_H_
#include "tnBase.h"
#include<microwin/nano-X.h>
#define TN_PROGRESSBAR_HEIGHT 15	/*Default height & width */
#define TN_PROGRESSBAR_WIDTH 100
#define PROGRESSBAR_CALLBACKS 1

typedef struct
{
//	GR_COLOR FGColor; /*Fill color for progress bar*/
	int value; /*Current percentage value*/
	GR_SIZE stepsize;
	int nType;
	GR_IMAGE_ID imageid[2];
}
TN_STRUCT_PROGRESSBAR;

TN_WIDGET *
CreateProgressBar(TN_WIDGET * parent, int flags, GR_RECT *rect, int ntype, 
					GR_SIZE stepsize, GR_IMAGE_ID imageid[]);

void DrawProgressBar (TN_WIDGET *);
void ProgressBarEventHandler (GR_EVENT *, TN_WIDGET *);
void tnProgressBarUpdate(TN_WIDGET *,int);
void DestroyProgressBar(TN_WIDGET *);
int tnGetProgressBarValue(TN_WIDGET *);
int tnGetProgressBarStepSize(TN_WIDGET *);
int tnSetProgressBarStepSize(TN_WIDGET *,int);
//TN_COLOR tnGetProgressBarFillColor(TN_WIDGET *);
//int tnSetProgressBarFillColor(TN_WIDGET *,TN_COLOR);
int tnSetProgressBarStyle(TN_WIDGET *,int ntype);
#endif /*_TNPROGRESSBAR_H_*/
