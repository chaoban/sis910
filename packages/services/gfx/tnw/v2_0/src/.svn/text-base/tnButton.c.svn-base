#include "tnWidgets.h"

/** 
Create push button widget 
@param parent, the parent of the button to be created
@param flags, the types of the button including the visible, enable and transparent
@param rect, the rect of the button to be created
@param caption, the caption string to show on the button
@param iid[], it's 2 dimensions, the first unit is the normal status image, and the second is the focus-in status
@retval  TN_BUTTON
*/
TN_WIDGET *CreateButton(TN_WIDGET *parent, int flags, GR_RECT *rect, char *caption, GR_IMAGE_ID iid[], int keymap[])
{
	TN_WIDGET	*pWidget;
	char		*tmp;

	pWidget = CreateWidget(parent, rect, GR_EVENT_MASK_KEY_DOWN
		| GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_FOCUS_IN
		| GR_EVENT_MASK_FOCUS_OUT | GR_EVENT_MASK_TIMER, flags, 0);

	if (pWidget == NULL) return NULL;
	// initialize button related information.
	pWidget->type = TN_BUTTON;
	pWidget->WSpec.button.imageid[0] = iid[0];
	pWidget->WSpec.button.imageid[1] = iid[1];
	
	pWidget->WSpec.button.keymap[0] = keymap[0];
	pWidget->WSpec.button.keymap[1] = keymap[1];
	pWidget->WSpec.button.keymap[2] = keymap[2];
	pWidget->WSpec.button.keymap[3] = keymap[3];
	
	tmp = malloc(strlen(caption) + 1);
	if (tmp == NULL) { 
		tnDestroyWidget(pWidget);
		free(pWidget); 
		return NULL;
	}	
	memcpy(tmp, caption, strlen(caption) + 1);
	pWidget->WSpec.button.caption = tmp;
	pWidget->WSpec.button.bFocus = GR_FALSE;
	return pWidget;
}

/**
Event Handler for the Push Button 
@param evnet, the event structure
@param widget, the widget generate the event
*/
void ButtonEventHandler(GR_EVENT * event, TN_WIDGET * widget)
{

#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "ButtonEventHandler");
#endif
		
  	switch (event->type)
    	{
	case GR_EVENT_TYPE_FOCUS_IN:
		if(!widget->visible || !widget->enabled)
			break; 
		widget->WSpec.button.bFocus = GR_TRUE;
		//GrResizeWindow(widget->wid, rcbtn.rect.width, rcbtn.rect.height);
	    //GrMoveWindow(widget->wid, rcbtn.rect.x, rcbtn.rect.y);
		GrClearWindow(widget->wid, GR_FALSE);
		DrawButton(widget);
		break;

	case GR_EVENT_TYPE_FOCUS_OUT:
		if(!widget->visible || !widget->enabled)
			break; 
		widget->WSpec.button.bFocus = GR_FALSE;
		GrClearWindow(widget->wid, GR_FALSE);
		DrawButton(widget);
		break;

	case GR_EVENT_TYPE_KEY_DOWN:
	    printf("Button GR_EVENT_TYPE_KEY_DOWN %d\n",event->keystroke.ch);
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
				    if((long)(event->keystroke.ch) > MWKEY_DOWN)
					    widget->data = (void *)((long)(event->keystroke.ch));
					else
					    widget->data = (void *)((long)(widget->WSpec.button.keymap[event->keystroke.ch - 0xF800] + 0xF800));
					(*(widget->CallBack[WIDGET_KEYDOWN].fp))(widget, widget->CallBack[WIDGET_KEYDOWN].dptr);
				}
				break;
		}
		break;	
    	case GR_EVENT_TYPE_EXPOSURE:
		if(widget->visible)	
	      		DrawButton (widget);
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

/** Push button draw routine 
@param button, the Button to DRAW!
*/
void DrawButton(TN_WIDGET *button)
{
	GR_WINDOW_INFO	winfo;
	GR_WINDOW_ID	pid;
	int		captionx;
	int		captiony;
	int		base;
	int		op;
	TN_WIDGET *parent;
    
    //if (button->WSpec.button.bFocus) {
	//    GrSetGCForeground(button->gc, GR_RGB(255, 255, 255));
	//	//GrDrawImageToFit(button->wid, button->gc, 0, 0, 
	//	//	winfo.width, winfo.height, button->WSpec.button.imageid[1]);
	//	GrFillRect(button->wid, button->gc, 0, 0, winfo.width, winfo.height);
	//}
	//else
	//{
	//    GrSetGCForeground(button->gc, GR_RGB(0, 0, 0));
	//	//GrDrawImageToFit(button->wid, button->gc, 0, 0, 
	//	//	winfo.width, winfo.height, button->WSpec.button.imageid[0]);
	//	GrFillRect(button->wid, button->gc, 0, 0, winfo.width, winfo.height);
	//}
    
    //pao-shu
    //printf("DrawButton \n");
	GrGetWindowInfo(button->wid, &winfo);
	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
    
    parent=GetParent(button);
    
	// draw button borders
	if (button->WSpec.button.bFocus) {
	    GrSetGCForeground(button->gc, GR_RGB(255, 255, 255));
		GrDrawImageToFit(pid, button->gc, 0, 0, 
			winfo.width, winfo.height, button->WSpec.button.imageid[1]);
		//GrFillRect(pid, button->gc, 0, 0, winfo.width, winfo.height);
	}
	else
	{
	    GrSetGCForeground(button->gc, GR_RGB(0, 0, 0));
		GrDrawImageToFit(pid, button->gc, 0, 0, 
			winfo.width, winfo.height, button->WSpec.button.imageid[0]);
		//GrFillRect(pid, button->gc, 0, 0, winfo.width, winfo.height);
	}
    
    
	if (button->WSpec.button.caption) {
		GrGetGCTextSize(button->gc, button->WSpec.button.caption, -1, GR_TFASCII, &captionx, &captiony, &base);
		captionx = (winfo.width - captionx) / 2;
		captiony = (winfo.height - captiony) / 2;
		if (captionx < 0) captionx = 0;
		if (captiony < 0) captiony = 0; 
		
		//GrSetGCForeground(button->gc, 0);
		GrSetGCUseBackground(button->gc, GR_FALSE);
		tnDrawText(pid, button->gc, captionx, captiony, 
			button->WSpec.button.caption, -1, GR_TFASCII | GR_TFTOP);
		//	button->WSpec.button.caption, 2, GR_TFUC32 | GR_TFTOP);
	}
    
	if (button->nFlags & GR_WM_PROPS_TRANSPARENT)
	{
		//op = MWROP_BLENDCONSTANT | 150;
		op = MWROP_SRC_TRANSLUCENCY;		
		GrCopyArea(button->wid, button->gc, 0, 0, winfo.width, winfo.height, parent->background, winfo.x, winfo.y, MWROP_COPY);
	}
	else
		op = MWROP_COPY;
    
	GrCopyArea(button->wid, button->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);
}

/** Push button destroy routine 
@param widget, the Button to DESTROY!
*/
void DestroyButton(TN_WIDGET *widget)
{
	free(widget->WSpec.button.caption);
	if (widget->WSpec.button.imageid[0])
		GrFreeImage(widget->WSpec.button.imageid[0]);
	if (widget->WSpec.button.imageid[1])
		GrFreeImage(widget->WSpec.button.imageid[1]);

	DeleteFromRegistry(widget);
	return;
}

/** Set the button caption
@param widget, the Button to set;
@param caption, the caption string to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnSetButtonCaption(TN_WIDGET *widget,char *caption)
{
	if(widget->type != TN_BUTTON)
		return -1;

	if(widget->WSpec.button.caption)
		free(widget->WSpec.button.caption);
	widget->WSpec.button.caption= (char *) strdup(caption);
	GrClearWindow(widget->wid,GR_TRUE);	
	return 1;
}
/** Get the button caption
@param widget, the Button to Get;
@param caption, the caption string to get.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnGetButtonCaption(TN_WIDGET *widget,char *caption)
{
        if(widget->type != TN_BUTTON)
                return -1;
        strcpy(caption,widget->WSpec.button.caption);
	return 1;
}

/** Set the button image
@param widget, the Button to Set;
@param image, the image id to set.
@return -1 for FAIL & 1 for SUCCESS
*/
int tnSetButtonImage(TN_WIDGET *widget, GR_IMAGE_ID iid[])
{
    if(widget->type != TN_BUTTON)
        return -1;
    
    if (widget->WSpec.button.imageid[0])
		GrFreeImage(widget->WSpec.button.imageid[0]);
	if (widget->WSpec.button.imageid[1])
		GrFreeImage(widget->WSpec.button.imageid[1]);
		
    widget->WSpec.button.imageid[0] = iid[0];
	widget->WSpec.button.imageid[1] = iid[1];
	
	return 1;
}
