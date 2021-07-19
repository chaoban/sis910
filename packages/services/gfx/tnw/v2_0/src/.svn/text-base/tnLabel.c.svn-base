#include "tnWidgets.h"
/**Create Label with specified params
@param parent, the parent of the label to be created
@param flags, the types of the label including the visible, enable and transparent
@param rect, the rect of the label to be created
@param caption, the caption string to show on the label
@param img, the image id of the label background
@param wnd, the window id of the label background if the label is transparent
*/
TN_WIDGET *CreateLabel(TN_WIDGET *parent, int flags, GR_RECT *rect, char *caption, GR_IMAGE_ID img, GR_WINDOW_ID wnd)
{
	TN_WIDGET	*pWidget;
	char		*tmp;

	pWidget = CreateWidget(parent, rect, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMER, flags, 0);

	if (pWidget == NULL) return NULL;
	// initialize label related information.
	pWidget->type = TN_LABEL;
	pWidget->WSpec.label.iBack = img;
	pWidget->WSpec.label.wBack = wnd;	
	tmp = malloc(strlen(caption) + 1);
	if (tmp == NULL) { 
		tnDestroyWidget(pWidget);
		free(pWidget);
		return NULL;
	}	
	memcpy(tmp, caption, strlen(caption) + 1);
	pWidget->WSpec.label.caption = tmp;

	return pWidget;
}

/**
Event Handler for the Label 
@param evnet, the event structure
@param widget, the widget generate the event
*/
void
LabelEventHandler (GR_EVENT * event, TN_WIDGET * widget)
{
    
#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "LabelEventHandler");
#endif

  	switch (event->type)
  	{
    	case GR_EVENT_TYPE_EXPOSURE:
			if(widget->visible)	
	      			DrawLabel (widget);
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

/**Label drawing routine
@param label, the Label to DRAW!
*/
void
DrawLabel (TN_WIDGET * label)
{
  	GR_WINDOW_INFO 	winfo;
  	GR_WINDOW_ID	pid;	
  	GR_SIZE captionwidth, captionheight, base;
  	GR_SIZE captionx, captiony;
	int	op;
	TN_WIDGET *parent;

  	GrGetWindowInfo (label->wid, &winfo);
  	//GrSetGCForeground (label->gc, 0); //later will replaced by config file.
  	GrSetGCForeground (label->gc, label->fgcolor); //later will replaced by config file.
  	GrGetGCTextSize (label->gc, label->WSpec.label.caption, -1, GR_TFASCII,
		&captionwidth, &captionheight, &base);
    
    //GrSetGCForeground(label->gc, GR_RGB(255, 255, 255));     
    //GrDrawImageToFit(label->wid, label->gc, 0, 0, winfo.width, winfo.height, label->WSpec.label.iBack);  
    parent=GetParent(label);
    
	/*Resize the window size to caption height & width  else use the values given by the user */
  	if (winfo.height < captionheight || winfo.width < captionwidth)
    {
      	GrResizeWindow (label->wid, captionwidth, captionheight);
		GrGetWindowInfo (label->wid, &winfo);
    }		
	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
		
	
  	if((int)label->WSpec.label.iBack > 0)
  	{  	    
  	    GrDrawImageToFit(pid, label->gc, 0, 0, 
			winfo.width, winfo.height, label->WSpec.label.iBack);
	}
	else
	{	    
	    //GrCopyArea(pid, label->gc, 0, 0, winfo.width, winfo.height, label->WSpec.label.wBack, 0, 0, MWROP_COPY);
	    GrCopyArea(pid, label->gc, 0, 0, winfo.width, winfo.height, parent->background, winfo.x, winfo.y, MWROP_COPY);
	}
    
   	captionx = (winfo.width - captionwidth) / 2;
  	captiony = (winfo.height - captionheight) / 2;
    
  	if (captionx < 0)
    		captionx = 0;		/*If caption height or width < window height or */
  	if (captiony < 0)
    		captiony = 0;		/*width - reset to 0 */
    
	/*Draw caption*/	
  	tnDrawText(pid, label->gc, captionx, captiony,
	  	label->WSpec.label.caption, -1,
	  	GR_TFASCII | GR_TFTOP);
    
	if (label->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;
    
	GrCopyArea(label->wid, label->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);
  	return;
}

/** Label destroy routine 
@param widget, the Label to DESTROY!
*/
void DestroyLabel(TN_WIDGET *widget)
{
	free(widget->WSpec.label.caption);
	if (widget->WSpec.label.iBack)
		GrFreeImage(widget->WSpec.label.iBack);
		
	if (widget->WSpec.label.wBack)
	    GrDestroyWindow(widget->WSpec.label.wBack);
	    
	DeleteFromRegistry(widget);
	return;
}
/** Update the label caption
@param widget, the Label to set;
@param caption, the caption string to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnUpdateLabelCaption(TN_WIDGET *widget, char *caption)
{
	if(widget->type!=TN_LABEL) 
		return -1;
	if(caption!=NULL)
	{
		free(widget->WSpec.label.caption);
		widget->WSpec.label.caption= (char *) strdup(caption);
	}
	DrawLabel (widget);
	return 1;
}

/** Set the label caption
@param widget, the Label to set;
@param caption, the caption string to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnSetLabelCaption(TN_WIDGET *widget, char *caption)
{
	if(widget->type!=TN_LABEL) 
		return -1;
	if(caption!=NULL)
	{
		free(widget->WSpec.label.caption);
		widget->WSpec.label.caption= (char *) strdup(caption);
	}
	GrClearWindow(widget->wid,GR_TRUE);
	return 1;
}

/*
int tnSetLabelCaptionColor(TN_WIDGET *widget, TN_COLOR color)
{
	if(widget->type!=TN_LABEL)
		return -1;
	widget->WSpec.label.FGColor=color;
	return 1;
}

TN_COLOR tnGetLabelCaptionColor(TN_WIDGET *widget)
{
        if(widget->type!=TN_LABEL)
	                return -1;
	else
		return widget->WSpec.label.FGColor;
}
*/

/** Get the Label caption
@param widget, the Label to set;
@param caption, the caption string to get.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnGetLabelCaption(TN_WIDGET *widget,char *caption)
{
        if(widget->type != TN_LABEL)
                return -1;
        strcpy(caption,widget->WSpec.label.caption);
	return 1;
}

/** Set the Label image
@param widget, the Label to set;
@param image, the image id to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnGetLabelImage(TN_WIDGET *widget,GR_IMAGE_ID iid)
{
    if(widget->type != TN_LABEL)
        return -1;    
    
    if (widget->WSpec.label.iBack)
		GrFreeImage(widget->WSpec.label.iBack);
	
	widget->WSpec.label.iBack = iid;	
	return 1;
}