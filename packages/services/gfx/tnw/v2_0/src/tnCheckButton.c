#include "tnWidgets.h"
/** Create Check Button widget 
@param parent, the parent of the check button to be created
@param flags, the typesof the check button including the visible, enable and transparent
@param rect, the rect of the check button to be created
@param caption, the caption string to show on the check button
@param imageid[], it's 4 dimensions, the first unit is the normal checked status image, the second is the focus-in checked status, the third is the normal unchecked status image, the fourth is the focus-in unchecked status
@retval TN_CHECKBUTTON
*/
TN_WIDGET *
CreateCheckButton (TN_WIDGET * parent, int flags, GR_RECT *rect, char *caption, GR_IMAGE_ID imageid[], int keymap[])
{
	TN_WIDGET	*widget = NULL;
	char		*tmp;

	// create widget for checkbutton
	widget = CreateWidget(parent, rect, GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_FOCUS_IN
		| GR_EVENT_MASK_FOCUS_OUT | GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMER, flags, 0);
	if (widget == NULL) return NULL;

	// initialize widget.
	widget->type = TN_CHECKBUTTON;
	widget->WSpec.checkbutton.bFocus = GR_FALSE;
	widget->WSpec.checkbutton.imageid[0] = imageid[0];
	widget->WSpec.checkbutton.imageid[1] = imageid[1];
	widget->WSpec.checkbutton.imageid[2] = imageid[2];
	widget->WSpec.checkbutton.imageid[3] = imageid[3];
	
	widget->WSpec.checkbutton.keymap[0] = keymap[0];
	widget->WSpec.checkbutton.keymap[1] = keymap[1];
	widget->WSpec.checkbutton.keymap[2] = keymap[2];
	widget->WSpec.checkbutton.keymap[3] = keymap[3];

	widget->WSpec.checkbutton.bFocus = GR_FALSE;
	//widget->WSpec.button.FGColor = fclr;	
	widget->WSpec.checkbutton.st_down = GR_FALSE;
    
    tmp = malloc(strlen(caption) + 1);
    if (tmp == NULL) { free(widget); return NULL;}	
    memcpy(tmp, caption, strlen(caption) + 1);
    widget->WSpec.checkbutton.caption = tmp;



	
	return widget;	
}

/**
Event Handler for the Check Button 
@param evnet, the event structure
@param widget, the widget generate the event
*/
void
CheckButtonEventHandler (GR_EVENT * event, TN_WIDGET * widget)
{

#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "CheckButtonEventHandler");
#endif
	
  	switch (event->type)
    	{
    	case GR_EVENT_TYPE_FOCUS_IN:
		if(!widget->visible || !widget->enabled)
			break; 
		widget->WSpec.checkbutton.bFocus = GR_TRUE;
		GrClearWindow(widget->wid, GR_FALSE);
		DrawCheckButton(widget);
		break;

	case GR_EVENT_TYPE_FOCUS_OUT:
		if(!widget->visible || !widget->enabled)
			break; 
		widget->WSpec.checkbutton.bFocus = GR_FALSE;
		GrClearWindow(widget->wid, GR_FALSE);
		DrawCheckButton(widget);
		break;

	case GR_EVENT_TYPE_KEY_DOWN:
		if(!widget->visible || !widget->enabled)
			break; 
		switch (event->keystroke.ch) {
			case MWKEY_ENTER:
				widget->WSpec.checkbutton.st_down = !(widget->WSpec.checkbutton.st_down);
				if (widget->CallBack[WIDGET_CLICKED].fp)
					(*(widget->CallBack[WIDGET_CLICKED].fp))(widget, widget->CallBack[WIDGET_CLICKED].dptr);
				DrawCheckButton(widget);
				break;
			case MWKEY_ESCAPE:
				if (widget->CallBack[WIDGET_ESCAPED].fp)	
					(*(widget->CallBack[WIDGET_ESCAPED].fp))(widget, widget->CallBack[WIDGET_ESCAPED].dptr);
				break;
			default:
				if (widget->CallBack[WIDGET_KEYDOWN].fp) {
					if((long)(event->keystroke.ch) > MWKEY_DOWN)
					    widget->data = (void *)((long)(event->keystroke.ch));
					else
					    widget->data = (void *)((long)(widget->WSpec.checkbutton.keymap[event->keystroke.ch - 0xF800] + 0xF800));
					(*(widget->CallBack[WIDGET_KEYDOWN].fp))(widget, widget->CallBack[WIDGET_KEYDOWN].dptr);
				}
				break;
		}
		break;	
    	
	case GR_EVENT_TYPE_EXPOSURE:
	      if(widget->visible)	
	          DrawCheckButton (widget);
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

/** Check button draw routine 
@param checkbutton, the Check Button to DRAW!
*/
void
DrawCheckButton (TN_WIDGET * checkbutton)
{
	GR_WINDOW_INFO winfo;
	GR_WINDOW_ID	pid;
	int	op;
	GR_SIZE captionx, captiony, base;
	//GR_COORD boxx, boxy;

	GrGetWindowInfo(checkbutton->wid, &winfo);
	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
	// draw button borders
	if (!checkbutton->WSpec.checkbutton.st_down) {
		if(!checkbutton->WSpec.checkbutton.bFocus)
			GrDrawImageToFit(pid, checkbutton->gc, 0, 0, 
				winfo.width, winfo.height, checkbutton->WSpec.checkbutton.imageid[0]);
		else
			GrDrawImageToFit(pid, checkbutton->gc, 0, 0, 
				winfo.width, winfo.height, checkbutton->WSpec.checkbutton.imageid[1]);
	} else {
		if(!checkbutton->WSpec.checkbutton.bFocus)
			GrDrawImageToFit(pid, checkbutton->gc, 0, 0, 
				winfo.width, winfo.height, checkbutton->WSpec.checkbutton.imageid[2]);
		else
			GrDrawImageToFit(pid, checkbutton->gc, 0, 0, 
				winfo.width, winfo.height, checkbutton->WSpec.checkbutton.imageid[3]);
	}

	if (checkbutton->WSpec.checkbutton.caption) {
		GrGetGCTextSize(checkbutton->gc, checkbutton->WSpec.checkbutton.caption, -1, GR_TFASCII, &captionx, &captiony, &base);
		//captionx = winfo.width - 40;		//distance reserve to display the checkable
		captionx = (winfo.width - captionx) / 2;
		captiony = (winfo.height - captiony) / 2;
		if (captiony < 0) captiony = 0; 
		
		GrSetGCForeground(checkbutton->gc, checkbutton->WSpec.checkbutton.clr_caption);
		tnDrawText(pid, checkbutton->gc, captionx, captiony, 
			checkbutton->WSpec.checkbutton.caption, -1, GR_TFASCII | GR_TFTOP);
	}

	if (checkbutton->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;

	GrCopyArea(checkbutton->wid, checkbutton->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);

}

/** Check button destroy routine 
@param widget, the Check Button to DESTROY!
*/
void DestroyCheckButton(TN_WIDGET *widget)
{
	int i;
	free(widget->WSpec.checkbutton.caption);
	for (i = 0; i < 4; i++)
		if (widget->WSpec.checkbutton.imageid[i])
			GrFreeImage(widget->WSpec.checkbutton.imageid[i]);
	
	DeleteFromRegistry(widget);
	return;
}

/** Set the check button caption
@param widget, the CheckButton to set;
@param caption, the caption string to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnSetCheckButtonCaption(TN_WIDGET *widget, char *caption)
{
	if(widget->type!=TN_CHECKBUTTON) 
		return -1;
	if(caption!=NULL)
	{
		free(widget->WSpec.checkbutton.caption);
		widget->WSpec.checkbutton.caption= (char *)strdup(caption);
		GrClearWindow(widget->wid,GR_TRUE);
	}
	
	return 1;
}

/** Set the check button caption
@param widget, the CheckButton to get;
@param caption, the caption string to get.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnGetCheckButtonCaption(TN_WIDGET *widget, char *caption)
{
	if(widget->type!=TN_CHECKBUTTON) 
		return -1;
	strcpy(caption,widget->WSpec.checkbutton.caption);
	return 1;
}

/** Set the radio button status
@param widget, the RadioButton to set;
@param status, the status to set, TRUE for Selected & FLASE for Not-Selected.
@return -1 for FAIL & 1 for SUCCESS
*/	
int tnSetCheckButtonStatus(TN_WIDGET *widget,GR_BOOL status)
{
	if(widget->type!=TN_CHECKBUTTON)
		return -1;
	if(widget->WSpec.checkbutton.st_down!=status)
	{
		widget->WSpec.checkbutton.st_down=status;
		GrClearWindow(widget->wid,GR_TRUE);
	}
	return 1;
}

/** Get the radio button status
@param widget, the RadioButton to get;
@param status, the status to get.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnGetCheckButtonStatus(TN_WIDGET *widget, GR_BOOL *status)
{
	if(widget == NULL || widget->type!=TN_CHECKBUTTON)
		return -1;
	
	*status = widget->WSpec.checkbutton.st_down;
	
	return 1;
}

void tnSetCheckButtonCaptionColor(TN_WIDGET *widget,GR_COLOR color)
{             
    widget->WSpec.checkbutton.clr_caption = color;
}

int tnGetCheckButtonCaptionColor(TN_WIDGET *widget,GR_COLOR *color)
{
    if(widget == NULL || widget->type!=TN_CHECKBUTTON)
		return -1;
		             
    *color = widget->WSpec.checkbutton.clr_caption;
    
    return 1;
}

void SetCheckButtonImageID (TN_WIDGET * widget,GR_IMAGE_ID imageid1,
     GR_IMAGE_ID imageid2,GR_IMAGE_ID imageid3,GR_IMAGE_ID imageid4)
{
   	widget->WSpec.checkbutton.imageid[0] = imageid1;
	widget->WSpec.checkbutton.imageid[1] = imageid2;
	widget->WSpec.checkbutton.imageid[2] = imageid3;
	widget->WSpec.checkbutton.imageid[3] = imageid4;
}
