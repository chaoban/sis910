#include "tnWidgets.h"
/** Create Radio Button widget 
@param parent, the parent of the radio button to be created
@param flags, the typesof the radio button including the visible, enable and transparent
@param rect, the rect of the radio button to be created
@param caption, the caption string to show on the radio button
@param imageid[], it's 4 dimensions, the first unit is the normal checked status image, the second is the focus-in checked status, the third is the normal unchecked status image, the fourth is the focus-in unchecked status
*/
TN_WIDGET *
CreateRadioButton (TN_WIDGET *parent, int flags, GR_RECT *rect,
	char *caption, GR_IMAGE_ID imageid[], PRDBGROUP group, int keymap[])
{
	TN_WIDGET	*widget = NULL;
	char		*tmp;

	// create widget for radiobutton
	widget = CreateWidget(parent, rect, GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_FOCUS_IN
		| GR_EVENT_MASK_FOCUS_OUT  | GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMER, flags, 0);
	if (widget == NULL) return NULL;

	// initialize widget.
	widget->type = TN_RADIOBUTTON;
	widget->WSpec.radiobutton.st_down = GR_FALSE;
	widget->WSpec.radiobutton.imageid[0] = imageid[0];
	widget->WSpec.radiobutton.imageid[1] = imageid[1];
	widget->WSpec.radiobutton.imageid[2] = imageid[2];
	widget->WSpec.radiobutton.imageid[3] = imageid[3];
	widget->WSpec.radiobutton.keymap[0] = keymap[0];
	widget->WSpec.radiobutton.keymap[1] = keymap[1];
	widget->WSpec.radiobutton.keymap[2] = keymap[2];
	widget->WSpec.radiobutton.keymap[3] = keymap[3];
	
	tmp = malloc(strlen(caption) + 1);
	if (tmp == NULL) { free(widget); return NULL;}	
	memcpy(tmp, caption, strlen(caption) + 1);
	widget->WSpec.radiobutton.caption = tmp;
	widget->WSpec.radiobutton.bFocus = GR_FALSE;
	widget->WSpec.radiobutton.group = group;
	AddRadioButton(group, widget);	

	return widget;	
}

/**
Event Handler for the Radio Button
@param evnet, the event structure
@param widget, the widget generate the event
*/
void 
RadioButtonEventHandler(GR_EVENT *event,TN_WIDGET *widget)
{

#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "RadioButtonEventHandler");
#endif

	switch(event->type)
	{
		case GR_EVENT_TYPE_FOCUS_IN:
			if(!widget->visible || !widget->enabled)
				break; 
			widget->WSpec.radiobutton.bFocus = GR_TRUE;
			GrClearWindow(widget->wid, GR_FALSE);
			DrawRadioButton(widget);
			break;

		case GR_EVENT_TYPE_FOCUS_OUT:
			if(!widget->visible || !widget->enabled)
				break; 
			widget->WSpec.radiobutton.bFocus = GR_FALSE;
			GrClearWindow(widget->wid, GR_FALSE);
			DrawRadioButton(widget);
			break;

		case GR_EVENT_TYPE_KEY_DOWN:
			if(!widget->visible || !widget->enabled)
				break; 
			switch (event->keystroke.ch) {
			case MWKEY_ENTER:
				SwitchSelect(widget->WSpec.radiobutton.group, widget);
				break;
			case MWKEY_ESCAPE:
				if (widget->CallBack[WIDGET_ESCAPED].fp)	
					(*(widget->CallBack[WIDGET_ESCAPED].fp))(widget, widget->CallBack[WIDGET_ESCAPED].dptr);
				break;
			default:
				if (widget->CallBack[WIDGET_KEYDOWN].fp) {
					//widget->data = (void *)((long)(event->keystroke.ch));
					widget->data = (void *)((long)(widget->WSpec.radiobutton.keymap[event->keystroke.ch - 0xF800] + 0xF800));
					(*(widget->CallBack[WIDGET_KEYDOWN].fp))(widget, widget->CallBack[WIDGET_KEYDOWN].dptr);
				}
				break;
			}
			break;	
		case GR_EVENT_TYPE_EXPOSURE:
			if(widget->visible)	
				DrawRadioButton(widget);
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


/** Draw routine for Radio Button 
@param radiobutton, the Radio Button to DRAW!
*/
void
DrawRadioButton (TN_WIDGET * radiobutton)
{
	GR_WINDOW_INFO winfo,pwinfo;
	GR_SIZE captionwidth,captionheight,base; 
	TN_WIDGET	*parent;
	GR_WINDOW_ID	pid;
	int		op;
	  
	GrGetGCTextSize(radiobutton->gc,radiobutton->WSpec.radiobutton.caption,-1,GR_TFASCII,&captionwidth,&captionheight,&base);
	captionheight=captionheight<13?13:captionheight;
	  
	GrGetWindowInfo (radiobutton->wid,&winfo);
	  
	if(winfo.width==TN_RADIOBUTTON_WIDTH)             
		GrResizeWindow(radiobutton->wid,captionwidth + 18,captionheight);

	parent = GetParent(radiobutton);
	if(parent->type == TN_RADIOBUTTONGROUP){
		  GrGetWindowInfo (parent->wid,&pwinfo);
		  if(winfo.width != pwinfo.width)
		  	  GrResizeWindow(radiobutton->wid,pwinfo.width, winfo.height);
	}

	GrGetWindowInfo (radiobutton->wid,&winfo);			                  

	pid = GrNewPixmap(winfo.width, winfo.height, NULL);

	if(radiobutton->WSpec.radiobutton.st_down==GR_TRUE)	
	{
	  	if(radiobutton->WSpec.radiobutton.bFocus == GR_TRUE)
			GrDrawImageToFit(pid, radiobutton->gc, 0, 0, 
				winfo.width, winfo.height, radiobutton->WSpec.radiobutton.imageid[3]);
		else
			GrDrawImageToFit(pid, radiobutton->gc, 0, 0, 
				winfo.width, winfo.height, radiobutton->WSpec.radiobutton.imageid[2]);

	}else{
		if(radiobutton->WSpec.radiobutton.bFocus == GR_TRUE)
			GrDrawImageToFit(pid, radiobutton->gc, 0, 0, 
				winfo.width, winfo.height, radiobutton->WSpec.radiobutton.imageid[1]);
		  else
			GrDrawImageToFit(pid, radiobutton->gc, 0, 0, 
				winfo.width, winfo.height, radiobutton->WSpec.radiobutton.imageid[0]);
	}
	  
	if (radiobutton->WSpec.radiobutton.caption) {
		captionwidth = winfo.width - 40;		//distance reserve to display the radioable
		captionheight = (winfo.height - captionheight) / 2;
		if (captionheight < 0) captionheight = 0; 
		
		GrSetGCForeground(radiobutton->gc, 0);
		tnDrawText(pid, radiobutton->gc, 18, captionheight, radiobutton->WSpec.radiobutton.caption, -1, GR_TFASCII | GR_TFTOP);
	}

   	if (radiobutton->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;

	GrCopyArea(radiobutton->wid, radiobutton->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);
}

/** Radio Button destroy routine 
@param widget, the Radio Button to DESTROY!
*/
void DestroyRadioButton(TN_WIDGET *widget)
{
	int i;
	free(widget->WSpec.radiobutton.caption);

	for(i = 0; i < 4; i++)
		if (widget->WSpec.radiobutton.imageid[i])
			GrFreeImage(widget->WSpec.radiobutton.imageid[i]);
	DeleteFromRegistry(widget);
	return;
}

/** Set the radio button caption
@param widget, the RadioButton to set;
@param caption, the caption string to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnSetRadioButtonCaption(TN_WIDGET *widget, char *caption)
{
	if(widget->type!=TN_RADIOBUTTON)
		return -1;
	if(caption!=NULL)
	{
		free(widget->WSpec.radiobutton.caption);
		widget->WSpec.radiobutton.caption= (char *) strdup(caption);
		GrClearWindow(widget->wid,GR_TRUE);
	}
	return 1;
}

/** Set the radio button caption
@param widget, the RadioButton to get;
@param caption, the caption string to get.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnGetRadioButtonCaption(TN_WIDGET *widget, char *caption)
{
	if(widget->type!=TN_RADIOBUTTON)
		return -1;
	strcpy(caption,widget->WSpec.radiobutton.caption);
	return 1;
}

/** Set the radio button status
@param widget, the RadioButton to set;
@param status, the status to set, TRUE for Selected & FLASE for Not-Selected.
@retval -1 for FAIL & 1 for SUCCESS
*/		
int tnSetRadioButtonStatus(TN_WIDGET *widget,GR_BOOL status)
{
	GR_WINDOW_INFO winfo;
	GR_WINDOW_ID siblingwid;
	TN_WIDGET *sibling;

	if(widget->type!=TN_RADIOBUTTON)
		return -1;
	if(widget->WSpec.radiobutton.st_down!=status)
	{
		if(status==GR_TRUE)
		{
			GrGetWindowInfo(widget->wid,&winfo);
			GrGetWindowInfo(winfo.parent,&winfo);
			siblingwid=winfo.child;
			while(siblingwid!=0)
			{
				sibling=GetFromRegistry(siblingwid);
				if(TN_RADIOBUTTONACTIVE(sibling))
				{
					sibling->WSpec.radiobutton.st_down=GR_FALSE;
					GrClearWindow(siblingwid,GR_TRUE);
				}
				GrGetWindowInfo(siblingwid,&winfo);
				siblingwid=winfo.sibling;
			}
		}
		widget->WSpec.radiobutton.st_down=status;
		GrClearWindow(widget->wid,GR_TRUE);
	}
	return 1;
}

/** Get the radio button status
@param widget, the RadioButton to get;
@param status, the status to get.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnGetRadioButtonStatus(TN_WIDGET *widget,GR_BOOL *status)
{
	if(widget->type!=TN_RADIOBUTTON)
		return -1;

	*status = widget->WSpec.radiobutton.st_down;
	
	return 1;
}
