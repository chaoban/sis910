#include "tnWidgets.h"

/** Create Progress Bar with specified params
@param parent, the parent of the progress bar to be created
@param flags, the types of the progress bar including the visible, enable and transparent
@param rect, the rect of the progress bar to be created
@param ntype, deside the drawing type with horizontal, vertical, or discrete.
@param stepsize, the progress step size
@param imageid[], it's 2 dimensions, the first unit is the background image, and the second is the foreground image
*/
TN_WIDGET *
CreateProgressBar(TN_WIDGET * parent, int flags, GR_RECT *rect, int ntype, 
					GR_SIZE stepsize, GR_IMAGE_ID imageid[])
{
	TN_WIDGET	*widget = NULL;

	// create widget for progressbar
	widget = CreateWidget(parent, rect, GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMER, flags, 0);
	if (widget == NULL) return NULL;

	// initialize widget.
	widget->type = TN_PROGRESSBAR;
	widget->WSpec.progressbar.value=0;
	widget->WSpec.progressbar.nType = ntype;
	if(stepsize > 0 && stepsize <= 100)
		widget->WSpec.progressbar.stepsize = stepsize;
	else
		widget->WSpec.progressbar.stepsize = 10;
	widget->WSpec.progressbar.imageid[0] = imageid[0];
	widget->WSpec.progressbar.imageid[1] = imageid[1];
	//widget->WSpec.progressbar.FGColor = 0;//rc_info.fclr;

	return widget;	
}

/**
Event Handler for the Progress Bar 
@param evnet, the event structure
@param widget, the widget generate the event
*/
void
ProgressBarEventHandler (GR_EVENT * event, TN_WIDGET * widget)
{
    
#ifdef WIDGET_KEY_DEBUG
  check_key_event(event, "ProgressBarEventHandler");
#endif
    
  switch (event->type)
  {	
    case GR_EVENT_TYPE_KEY_DOWN:
	if(!widget->visible || !widget->enabled)
		break; 
	switch (event->keystroke.ch) {
		case MWKEY_ENTER:
			if (widget->CallBack[WIDGET_CLICKED].fp)
				(*(widget->CallBack[WIDGET_CLICKED].fp))(widget, widget->CallBack[WIDGET_CLICKED].dptr);
		break;
		case MWKEY_ESCAPE:
			if (widget->CallBack[WIDGET_ESCAPED].fp)	
				(*(widget->CallBack[WIDGET_ESCAPED].fp))(widget, widget->CallBack[WIDGET_ESCAPED].dptr);
		break;
		default:
			if (widget->CallBack[WIDGET_KEYDOWN].fp) {
				widget->data = (void *)((long)(event->keystroke.ch));
					(*(widget->CallBack[WIDGET_KEYDOWN].fp))(widget, widget->CallBack[WIDGET_KEYDOWN].dptr);
			}
		break;
		}
	break;
    case GR_EVENT_TYPE_EXPOSURE:
        printf("%s - GR_EVENT_TYPE_EXPOSURE\n");
        if(widget->visible)	
        {
            printf("%s - GR_EVENT_TYPE_EXPOSURE  visible\n");
	        DrawProgressBar (widget);
	    }
        break;
    case GR_EVENT_TYPE_TIMER:
	if(!widget->enabled)
			break; 
	if (widget->CallBack[WIDGET_TIMER].fp) 
		widget->CallBack[WIDGET_TIMER].fp(widget, 
			widget->CallBack[WIDGET_TIMER].dptr);
	break;
    }
}

/** ProgressBar drawing routine
@param progressbar, the Progress Bar to DRAW!
*/
void
DrawProgressBar (TN_WIDGET * progressbar)
{
 	GR_WINDOW_INFO winfo;
	GR_WINDOW_ID	pid;
 	GR_GC_INFO gcinfo;
 	GR_FONT_INFO finfo;
 	GR_COORD boxx,boxy;
 	GR_SIZE maxwidth, maxheight;
	int count,op,startx, boxlength;

	GrGetWindowInfo (progressbar->wid, &winfo);
	GrGetGCInfo(progressbar->gc,&gcinfo);
	GrGetFontInfo(gcinfo.font,&finfo);
  
	if(winfo.height<=finfo.height){
		GrResizeWindow(progressbar->wid,winfo.width,finfo.height+4);
		GrGetWindowInfo (progressbar->wid, &winfo);
	}
	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
	
	//boxx=1;
	//boxy=1;
	boxx=0;
	boxy=0;
	//maxwidth=winfo.width-2;
	//maxheight=winfo.height-2;
	maxwidth=winfo.width;
	maxheight=winfo.height;
	//Draw border
  	GrDrawImageToFit(pid, progressbar->gc, 0, 0, 
			maxwidth, maxheight, progressbar->WSpec.progressbar.imageid[0]);
    //GrFillRect(pid, progressbar->gc, 0, 0, winfo.width, winfo.height);			
  
  	if(progressbar->WSpec.progressbar.nType==2)//discrete
 	{
	 	startx=0.15*winfo.width;
		 
		boxy = progressbar->WSpec.progressbar.value * winfo.height / 100;
		boxx = winfo.height / 7;
		count = boxy/boxx;
		boxy = (6-count) * boxx;
		GrDrawImageToFit(pid, progressbar->gc, startx,boxy,winfo.width*0.7,boxx-1,
		 						progressbar->WSpec.progressbar.imageid[1]);
	}
	else if(progressbar->WSpec.progressbar.nType == 1)
	{
	    boxlength = (progressbar->WSpec.progressbar.value*maxheight)/100;
	    boxy = winfo.height - 1 - boxlength;
	    if(progressbar->WSpec.progressbar.value != 0)
			GrDrawImageToFit(pid, progressbar->gc, boxx, boxy, maxwidth, boxlength, 
			                      progressbar->WSpec.progressbar.imageid[1]);	 
	}
	else
    { 
        boxlength = (progressbar->WSpec.progressbar.value*maxwidth)/100;
       	if(progressbar->WSpec.progressbar.value != 0)
			GrDrawImageToFit(pid, progressbar->gc, boxx, boxy, boxlength, 
			                      maxheight, progressbar->WSpec.progressbar.imageid[1]);	 
		
	}

	if (progressbar->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;

	GrCopyArea(progressbar->wid, progressbar->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);
	//Draw progress text
	/*
	if(progressbar->WSpec.progressbar.discrete!=GR_TRUE){
		GrSetGCForeground (progressbar->gc, GR_RGB(0,0,0));
		if(progressbar->WSpec.progressbar.value>=100) 
			progressbar->WSpec.progressbar.value=100;
		 
		sprintf(s,"%d",progressbar->WSpec.progressbar.value);
		strcat(s,"%");
	 
		captiony=(winfo.height-finfo.height)/2;
		captionx=(winfo.width-3*finfo.maxwidth)/2;
		GrSetGCForeground(progressbar->gc, GR_RGB(255, 255, 255));
		GrSetGCBackground(progressbar->gc, GR_RGB(0, 0, 0));
		GrSetGCMode(progressbar->gc, GR_MODE_OR);
		GrText(progressbar->wid, progressbar->gc, captionx, captiony, s, -1, GR_TFASCII | GR_TFTOP);
		GrSetGCForeground(progressbar->gc, 0);
		GrSetGCBackground(progressbar->gc, GR_RGB(255, 255, 255));
		GrSetGCMode(progressbar->gc, GR_MODE_AND);
		GrText(progressbar->wid, progressbar->gc, captionx, captiony, s, -1, GR_TFASCII | GR_TFTOP);
		GrSetGCMode(progressbar->gc, GR_MODE_COPY);
		//GrText (progressbar->wid, progressbar->gc, captionx, captiony, s, -1, GR_TFASCII | GR_TFTOP);
	}*/
}

/** Progress the value of the Progress Bar.
@param widget, the Progress Bar to progress the value.
@param percval, the percentage integer number as the value to set.
*/
void tnProgressBarUpdate(TN_WIDGET *widget, int percval)
{
	if(widget->type==TN_PROGRESSBAR && widget->enabled)
	{
		widget->WSpec.progressbar.value=percval;
		GrClearWindow(widget->wid,GR_FALSE);
		DrawProgressBar(widget);
	}
	return;
}

/** Progress Bar destroy routine 
@param widget, the Progress Bar to DESTROY!
*/
void DestroyProgressBar(TN_WIDGET *widget)
{
	if (widget->WSpec.progressbar.imageid[0])
		GrFreeImage(widget->WSpec.progressbar.imageid[1]);
	DeleteFromRegistry(widget);
	return;
}

/** Get the current value of the Progress Bar.
@param widget, the Progress Bar to get the value.
@retval The value of the Progress Bar.
*/
int tnGetProgressBarValue(TN_WIDGET *widget)
{
	if(widget->type!=TN_PROGRESSBAR)
		return -1;
	else 
		return widget->WSpec.progressbar.value;	
}

/** Get the step size of the Progress Bar.
@param widget, the Progress Bar to get the step size.
@retval The step size of the Progress Bar. 
*/
int tnGetProgressBarStepSize(TN_WIDGET *widget)
{
	if(widget->type!=TN_PROGRESSBAR)
		return -1;
	else 
		return widget->WSpec.progressbar.stepsize;
}

/** Set the step size of the Progress Bar.
@param widget, the Progress Bar to set the step size.
@retval -1 for FAIL & 1 for SUCCESS! 
*/
int tnSetProgressBarStepSize(TN_WIDGET *widget, int step)
{
	if(widget->type!=TN_PROGRESSBAR || step <= 0 || step > 100 )
		return -1;
	widget->WSpec.progressbar.stepsize = step;
	GrClearWindow(widget->wid,GR_TRUE);
	return 1;
}
	

/*TN_COLOR tnGetProgressBarFillColor(TN_WIDGET *widget)
{
	if(widget->type!=TN_PROGRESSBAR)
		return -1;
	else
		return widget->WSpec.progressbar.FGColor;
}

int tnSetProgressBarFillColor(TN_WIDGET *widget, TN_COLOR color)
{
	if(widget->type!=TN_PROGRESSBAR || color < 0)
		return -1;
	widget->WSpec.progressbar.FGColor = color;
	GrClearWindow(widget->wid,GR_TRUE);
	return 1;
}*/

/** Set the style of the Progress Bar.
@param widget, the Progress Bar to set the style.
@param ntype, set the drawing type with horizontal, vertical, or discrete.
@retval -1 for FAIL & 1 for SUCCESS! 
*/
int tnSetProgressBarStyle(TN_WIDGET *widget,int ntype)
{
	if(widget->type!=TN_PROGRESSBAR)
		return -1;
	widget->WSpec.progressbar.nType = ntype;
	GrClearWindow(widget->wid,GR_TRUE);
	return 1;
}
									
