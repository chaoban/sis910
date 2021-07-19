#include<string.h>
#include "tnWidgets.h"
/** Create window widget 
@param parent, the parent of the window to be created
@param flags, the types of the window including the visible, enable and transparent
@param rect, the rect of the window to be created
@param iid, the background image id of the window
@param bgcolor, if no image fill the window's background with this color
*/
TN_WIDGET*
CreateWindow(TN_WIDGET *parent, int flags, GR_RECT *rect, GR_IMAGE_ID iid, GR_COLOR bgcolor)
{
	TN_WIDGET		*pWidget=NULL;
    
    //pao-shu
    //printf("CreateWindow 1\n");
	pWidget = CreateWidget(parent, rect, GR_EVENT_MASK_EXPOSURE 
		| GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_TIMER, flags, bgcolor);  
    //printf("CreateWindow 2\n");
	// initialize window related information.
	pWidget->type = TN_WINDOW;
	pWidget->WSpec.window.iBack = iid;
	pWidget->WSpec.window.cBack = bgcolor;


    pWidget->background = GrNewPixmap(rect->width, rect->height, NULL);
    GrDrawImageToFit(pWidget->background, pWidget->gc, 0, 0, rect->width, rect->height, iid);
    
    GrSetBackgroundPixmap(pWidget->wid, pWidget->background, GR_BACKGROUND_STRETCH);

	GrClearWindow(pWidget->wid, GR_TRUE);
	
	return pWidget;
}

/**
Event Handler for the Window
@param evnet, the event structure
@param widget, the widget generate the event
*/
void 
WindowEventHandler(GR_EVENT* event, TN_WIDGET *widget)
{
	
#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "WindowEventHandler");
#endif

	switch (event->type)
	{
		case GR_EVENT_TYPE_EXPOSURE:
			if(widget->visible)	
				DrawWindow(widget);		
			break;
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
		case GR_EVENT_TYPE_TIMER:
			if(!widget->enabled)
				break; 
			if (widget->CallBack[WIDGET_TIMER].fp) 
				widget->CallBack[WIDGET_TIMER].fp(widget, 
					widget->CallBack[WIDGET_TIMER].dptr);
			break;
	}
}

/** Window draw routine 
@param widget, the Window to DRAW!
*/
void DrawWindow(TN_WIDGET *widget)
{
	GR_WINDOW_INFO	winfo;
	GR_WINDOW_ID	pid;
	GR_IMAGE_ID	iBack;
	GR_COLOR	cBack;
	int		op;

	iBack = widget->WSpec.window.iBack;
	cBack = widget->WSpec.window.cBack;
	GrGetWindowInfo(widget->wid, &winfo);

    //GrSetGCForeground(widget->gc, cBack);
    //GrFillRect(widget->wid, widget->gc, 0, 0, winfo.width, winfo.height);
    //GrDrawImageToFit(widget->wid, widget->gc, 0, 0, winfo.width, winfo.height, iBack);
    //pao-shu
    ////printf("DrawWindow \n");
	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
    
    
	if (iBack == 0)
	{
		GrSetGCForeground(widget->gc, cBack);
		GrFillRect(pid, widget->gc, 0, 0, winfo.width, winfo.height);
		
	  //{
	  //  int j;
	  //  cyg_uint32 i;
	  //  for(j=0;j<2;j++)
	  //  {
	  //  	for(i=0;i<40000000;i++)
	  //  	{
	  //  		asm("NOP");
	  //  	}
	  //  }
      //}
	} 
	else
	{
		GrDrawImageToFit(pid, widget->gc, 0, 0, winfo.width, winfo.height, iBack);
	}
	
	if (widget->CallBack[WIDGET_EXPOSURE].fp) 
	{
		widget->WSpec.window.pid = pid;
		widget->CallBack[WIDGET_EXPOSURE].fp(widget, 
			widget->CallBack[WIDGET_EXPOSURE].dptr);
	}
    
	if (widget->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;
    
	GrCopyArea(widget->wid, widget->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);
}


/**
Create Image form window
@param wnd, the source of window to be copyed
@param rect, the copy range of window
@retval GR_IMAGE_ID
*/
GR_WINDOW_ID CreateImageFromWindow(TN_WIDGET *widget, HW_RECT *rect)
{
	GR_WINDOW_INFO	winfo;
	GR_WINDOW_ID	pid;
	GR_WINDOW_ID	return_id;
	GR_GC_ID	gc;
	GR_IMAGE_ID	iBack;
	GR_COLOR	cBack;

	iBack = widget->WSpec.window.iBack;
	cBack = widget->WSpec.window.cBack;
	GrGetWindowInfo(widget->wid, &winfo);

	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
	return_id = GrNewPixmap(rect->width, rect->height, NULL);
	gc = GrNewGC();


	if (iBack == 0) {
		GrSetGCForeground(widget->gc, cBack);
		GrFillRect(pid, widget->gc, 0, 0, winfo.width, winfo.height);
	} else {
		GrDrawImageToFit(pid, widget->gc, 0, 0, winfo.width, winfo.height, iBack);
	}
	
	if (widget->CallBack[WIDGET_EXPOSURE].fp) 
	{
		widget->WSpec.window.pid = pid;
		widget->CallBack[WIDGET_EXPOSURE].fp(widget, 
			widget->CallBack[WIDGET_EXPOSURE].dptr);
	}
	
	GrCopyArea(return_id, gc, 0, 0, rect->width, rect->height, pid, rect->x, rect->y, MWROP_SRCCOPY);
	GrDestroyWindow(pid);
	GrDestroyGC(gc);
	
	return return_id;
}
/** Window destroy routine 
@param widget, the Window to DESTROY!
*/
void DestroyWindow(TN_WIDGET *widget)
{        
	if (widget->WSpec.window.iBack)
		GrFreeImage(widget->WSpec.window.iBack);
			
	GrDestroyWindow(widget->background);
	
	DeleteFromRegistry(widget);	
	return;
}

/** NOT used yet!
*/
void tnGetClickedPos(TN_WIDGET *widget,int *x,int *y)
{
	//*x=widget->WSpec.window.clickedx;
	//*y=widget->WSpec.window.clickedy;
	return;
}

/** Get the Window's title!
@param widget, the Window to get the title.
@param caption, the caption string to get.
@retval -1 for FAIL & 1 for success.
*/
int tnGetWindowTitle(TN_WIDGET *widget, char *caption)
{
	GR_WM_PROPERTIES props;
	if(widget->type!=TN_WINDOW)
		return -1;
	GrGetWMProperties(widget->wid,&props);
	strcpy(caption,props.title);
	return 1;
}

/** Set the Window's title!
@param widget, the Window to set the title.
@param caption, the caption string to set.
@retval -1 for FAIL & 1 for success.
*/					
int tnSetWindowTitle(TN_WIDGET *widget,char *caption)
{
	GR_WM_PROPERTIES props;
	if(widget->type!=TN_WINDOW)
		return -1;
	GrGetWMProperties(widget->wid,&props);
	props.title=caption;
	GrSetWMProperties(widget->wid,&props);
	return 1;
}

