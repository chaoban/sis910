 #include<stdarg.h>
#include<stdlib.h>
#include "tnWidgets.h"

#ifdef __ECOS
#include <cyg/kernel/kapi.h>
cyg_mutex_t	mut_access_registry;
#endif


PfnUpdateFront	arrUpdateFront[MAXWIDGETS];	

/**
The message main loop.
*/
void
tnMainLoop (void)
{
	GrMainLoop(EventResolveRoutine);
}

/**
The event resolve routine in the control library.
@param event, the event recieve from the Nano-X server
*/
void
EventResolveRoutine (GR_EVENT * event)
{
  GR_WINDOW_ID wid;
  TN_WIDGET *WidgetPtr;
  //GR_EVENT_KEYSTROKE * kev;
  switch (event->type)
    {
    case GR_EVENT_TYPE_ERROR:
      GrDefaultErrorHandler (event);
      break;

    case GR_EVENT_TYPE_NONE:
      break;

    /*case GR_EVENT_TYPE_EXPOSURE:
    case GR_EVENT_TYPE_BUTTON_DOWN:
    case GR_EVENT_TYPE_BUTTON_UP:
    case GR_EVENT_TYPE_MOUSE_ENTER:
    case GR_EVENT_TYPE_MOUSE_EXIT:
    case GR_EVENT_TYPE_MOUSE_MOTION:
    case GR_EVENT_TYPE_MOUSE_POSITION:
    case GR_EVENT_TYPE_KEY_DOWN:
    case GR_EVENT_TYPE_KEY_UP:
    case GR_EVENT_TYPE_FOCUS_IN:
    case GR_EVENT_TYPE_FOCUS_OUT:
    case GR_EVENT_TYPE_UPDATE:
    case GR_EVENT_TYPE_CHLD_UPDATE:
    case GR_EVENT_TYPE_CLOSE_REQ*/
    //case GR_EVENT_TYPE_TIMER:               
    //    printf("GR_EVENT_TYPE_TIMER\n");    
	//    break;                              
    //case GR_EVENT_TYPE_KEY_DOWN:            
	//    kev = (GR_EVENT_KEYSTROKE *)&event; 
    //    printf("DOWN %d (%04x) %04x\n",     
	//    kev->ch, kev->ch, kev->modifiers);	
	 //   break;                              
    case GR_EVENT_TYPE_KEY_DOWN:
		if (event->keystroke.hotkey)
		    wid = event->keystroke.subwid;
		else
		    wid = event->keystroke.wid;
		WidgetPtr = GetFromRegistry (wid);
        if (WidgetPtr == NULL)
	        break;
        ProcessEvent(event, WidgetPtr);
        break;
    default:
      wid = event->general.wid;
      WidgetPtr = GetFromRegistry (wid);
      if (WidgetPtr == NULL)
	      break;
      ProcessEvent(event, WidgetPtr);
      break;
      
    case GR_EVENT_TYPE_FDINPUT:
      break;
    }
  return;
}

/**
A part of the EventResolveRoutine(). Used to enter the specified control types' event handler.
@param event, the event recieve from the Nano-X server
@param WidgetPtr, the control widget who recieve the event.
*/
void ProcessEvent(GR_EVENT *event,TN_WIDGET *WidgetPtr)
{
	/*I hate switch..cases*/
	(*EventDispatch[WidgetPtr->type]) (event,WidgetPtr);
	return;
}

/**
Register a call back function on a control widget.
@param widget, the control widget to install the call back function
@param uevent, the event types on the control widget; generally are like these:WIDGET_CLICKED, WIDGET_ESCAPED, WIDGET_KEYDOWN, WIDGET_TIMER, WIDGET_EXPOSURE, WIDGET_SELECTED
@param fp, the call back function pointer.
@param dptr, the reserved data to transfer into the call back function.
@retval -1 for FAIL & 1 for SUCCESS
*/
int
tnRegisterCallBack (TN_WIDGET * widget,
		    USER_EVENT uevent, CallBackFuncPtr fp, DATA_POINTER dptr)
{
  	if(widget==NULL || uevent >= WIDGET_CALLBACK) 
		return -1;

	widget->CallBack[uevent].fp = fp;
	widget->CallBack[uevent].dptr = dptr;

    	return 1;
}

#ifdef __ECOS
/**
Initialise the control library. This is only used in the ECOS.
@retval TN_WIDGET(always is the root window)
*/
TN_WIDGET *
tnControlLibInitialize()
{
  	TN_WIDGET 	*MainWidget;
  	int 		i;

	// connect to the nano-X server and load resource from rc file.

	if (GrOpen() < 0) return NULL;

#ifdef CYGPKG_HAL_R16
    GdSetHwUsage(1);
#endif    
     
  	AddDestroyFuncs();
  	AddEventDispatchFuncs();
	AddUpdateFront();
  	for(i=0; i < HASH_MAX_QUEUES; i++)
  	{
	  	Registry[i] = (TN_WIDGET *)malloc(sizeof(TN_WIDGET));
		Registry[i]->type = -100;
	  	Registry[i]->next = Registry[i]->prev = NULL;
  	}

  	MainWidget = (TN_WIDGET *) malloc (sizeof (TN_WIDGET));
  	MainWidget->wid = GR_ROOT_WINDOW_ID;
  	MainWidget->type=TN_APP_TYPE;
  	AddToRegistry (MainWidget);
  	//TN_DEFAULT_FONT_NO=GrCreateFont(MWFONT_GUI_VAR, 0, NULL);  	
  	TN_DEFAULT_FONT_NO=GrCreateFont("System", 0, NULL);
  	//TN_DEFAULT_FONT_NO=GrCreateFont("TestFont", 0, NULL);
  	printf("tnControlLibInitialize %d\n",TN_DEFAULT_FONT_NO);

  	return MainWidget;
}

/**
Initialise the application.
@retval TN_WIDGET(always is the root window)
*/
TN_WIDGET *
tnAppInitialize()
{
    int i;
	// connect to the nano-X server and load resource from rc file.
	if (GrOpen() < 0) return NULL;

  	AddDestroyFuncs();
  	AddEventDispatchFuncs();
	AddUpdateFront();
	
	//for(i=0; i < HASH_MAX_QUEUES; i++)
  	//{
	//  	Registry[i] = (TN_WIDGET *)malloc(sizeof(TN_WIDGET));
	//	Registry[i]->type = -100;
	//  	Registry[i]->next = Registry[i]->prev = NULL;
  	//}
  	
  	//TN_DEFAULT_FONT_NO=GrCreateFont(MWFONT_GUI_VAR, 0, NULL);
	return GetFromRegistry(GR_ROOT_WINDOW_ID);
}

#else
/**
Initialise the application. This is only used in Linux OS
@retval TN_WIDGET(always is the root window)
*/
TN_WIDGET *
tnAppInitialize()
{
  	TN_WIDGET 	*MainWidget;
  	int 		i;

	// connect to the nano-X server and load resource from rc file.
	if (GrOpen() < 0) return NULL;

  	AddDestroyFuncs();
  	AddEventDispatchFuncs();
	AddUpdateFront();
  	for(i=0; i < HASH_MAX_QUEUES; i++)
  	{
	  	Registry[i] = (TN_WIDGET *)malloc(sizeof(TN_WIDGET));
		Registry[i]->type = -100;
	  	Registry[i]->next = Registry[i]->prev = NULL;
  	}

  	MainWidget = (TN_WIDGET *) malloc (sizeof (TN_WIDGET));
  	MainWidget->wid = GR_ROOT_WINDOW_ID;
  	MainWidget->type=TN_APP_TYPE;
  	AddToRegistry (MainWidget);
  	//TN_DEFAULT_FONT_NO=GrCreateFont(MWFONT_GUI_VAR, 0, NULL);
  	TN_DEFAULT_FONT_NO=GrCreateFont("TestFont", 0, NULL);
  	printf("tnAppInitialize %d\n",TN_DEFAULT_FONT_NO);
  	

  	return MainWidget;
}
#endif

/**
Initialise the essential paranmeters of a control widget.
@param parent, the parent of the control widget to create.
@param rect, the rect to create the control widget.
@param flags, the types of the button including the visible, enable and transparent
@param clrbkg, the backgroud color, now is set to RGB(0,0,0).
@retval TN_WIDGET(the created widget)
*/
TN_WIDGET* CreateWidget(TN_WIDGET *parent, GR_RECT *rect, int emask, int flags, GR_COLOR clrbkg)
{
	GR_WINDOW_ID		wid = parent ? parent->wid : GR_ROOT_WINDOW_ID;
	GR_WM_PROPERTIES	props;
	TN_WIDGET		*pWidget=NULL;
	int			i;
	
	if (rect == NULL) return NULL;
    
	pWidget = (TN_WIDGET *)malloc(sizeof(TN_WIDGET));
	if (pWidget == NULL) return NULL;

	memset(pWidget, 0, sizeof(TN_WIDGET));

	pWidget->gc = GrNewGC();
	//GrSetGCFont(pWidget->gc, GrCreateFont("System", 0, NULL));
	GrSetGCFont(pWidget->gc, TN_DEFAULT_FONT_NO);
	pWidget->data = NULL;
	pWidget->nResourceID = -1;
	pWidget->nFlags = flags;
	pWidget->next = NULL;
	pWidget->fgcolor = GR_ARGB(255,0, 0, 0);
    
#ifdef __ECOS
	pWidget->handle = cyg_thread_self();
#endif // __ECOS

	// create the nano-x window.
	pWidget->wid = GrNewWindow(wid, rect->x, rect->y,
		rect->width, rect->height, 0, clrbkg,0);

	// set the WM properties.
	props.flags = GR_WM_FLAGS_PROPS;
	props.props = GR_WM_PROPS_NODECORATE;// | GR_WM_PROPS_NOBACKGROUND;
	if (flags & GR_WM_PROPS_NOBACKGROUND) props.props |= GR_WM_PROPS_NOBACKGROUND;
	if (flags & GR_WM_PROPS_TRANSPARENT) props.props |= GR_WM_PROPS_TRANSPARENT;
	GrSetWMProperties(pWidget->wid, &props);

	// select event which need to process.	
	GrSelectEvents(pWidget->wid, emask); 
	pWidget->enabled = GR_TRUE;
	pWidget->visible = GR_TRUE;

	AddToRegistry(pWidget);

	// initialize callback table.
	for (i = 0; i < WIDGET_CALLBACK; i++)
		pWidget->CallBack[i].fp = NULL;

	// Map the window.
	GrSetGCUseBackground(pWidget->gc, GR_FALSE);
	GrMapWindow(pWidget->wid);

	return pWidget;
} 

/**
Destroy a control widget and all its children.
@param widget, the control widget to destroy.
@retval -1 for FAIL & 1 for SUCCESS
*/
int
tnDestroyWidget(TN_WIDGET* widget)
{
	
	GR_WINDOW_INFO 	winfo;
	GR_WINDOW_ID 	widChild;
	TN_WIDGET 	*pChild;

	if (widget == NULL) return -1;
    
	// first destroy all children window.
	GrGetWindowInfo(widget->wid, &winfo);
	widChild = winfo.child;
	while (widChild) {
		pChild = GetFromRegistry(widChild);
		tnDestroyWidget(pChild);
		GrGetWindowInfo(widget->wid, &winfo);
		widChild = winfo.child;
	}

	GrDestroyWindow(widget->wid);
	GrDestroyGC(widget->gc);
	Destroy[widget->type](widget);	
	widget = NULL;	
	
	return 1;
}

/**
Set whether to show the widget.
@param widget, the widget to set.
@param visible, TRUE for visible & FALSE for invisible.
*/
void tnSetVisible(TN_WIDGET *widget, GR_BOOL visible)
{
	widget->visible=visible;
	if(visible==GR_FALSE)
		GrUnmapWindow(widget->wid);
	else 
		GrMapWindow(widget->wid);
	return;
}

/**
Set whether to enable the widget.
@param widget, the widget to set.
@param visible, TRUE for enable & FALSE for disable.
*/
void tnSetEnabled(TN_WIDGET *widget, GR_BOOL enabled)
{
	GR_WINDOW_INFO winfo;
	GR_WINDOW_ID siblingwid;
	TN_WIDGET *sibling;
	widget->enabled=enabled;
	GrClearWindow(widget->wid,GR_TRUE);
	GrGetWindowInfo(widget->wid,&winfo);
	siblingwid=winfo.child;
	while(siblingwid!=0)
	{
		sibling=GetFromRegistry(siblingwid);
		if(sibling)
		{
			sibling->enabled=enabled;
			GrClearWindow(sibling->wid,GR_TRUE);
		}
		GrGetWindowInfo(siblingwid,&winfo);
		siblingwid=winfo.sibling;
	}
}

/**
Set foreground color of the widget.
@param widget, the widget to set.
@param color,the color value to be set.
*/
void tnSetWidgetFGColor(TN_WIDGET *widget, GR_COLOR color)
{
    widget->fgcolor = color;
}
#ifdef __ECOS
/**
Add a new control widget to the Registry[]. This is used only in ECOS.
@param ptr, the control widget to add.
*/
void 
AddToRegistry(TN_WIDGET *ptr)
{
  //    TN_WIDGET *prev = NULL, *next = NULL;
       
	int hindex = ptr->wid % HASH_MAX_QUEUES;
	cyg_mutex_lock(&mut_access_registry);	
	ptr->next  = Registry[hindex]->next;
	ptr->prev  = Registry[hindex];	
	Registry[hindex]->next = ptr;	
//	next = ptr->next;
	if(ptr->next != NULL)
	{
		(ptr->next)->prev = ptr;
	}

	cyg_mutex_unlock(&mut_access_registry);
	return;
}

/**
Get a control widget from the Registry[]. This is used only in ECOS.
@param wid, the widget' wid.
@retval TN_WIDGET(the got widget)
*/
TN_WIDGET *
GetFromRegistry(GR_WINDOW_ID wid)
{
    
    //int hindex = wid % HASH_MAX_QUEUES;
	//TN_WIDGET *temp = Registry[hindex]->next;
	//while(temp && temp->wid != wid)
	//	temp=temp->next;
	//return temp;
	
	int hindex = wid % HASH_MAX_QUEUES;
	cyg_mutex_lock(&mut_access_registry);
	bool find = false;
	TN_WIDGET *temp = Registry[hindex]->next;
	
	//while(temp && temp->wid != wid)
	while(temp)
	{
	    if(temp->wid == wid)
	    { 
	        find = true;	        
	        break;
	    }
		temp=temp->next;
	}
	
	cyg_mutex_unlock(&mut_access_registry);
	//if(!find) printf(" %s - not find %d\n",__FUNCTION__, wid);
	if(find) return temp;
	else return NULL;
}

/**
Delete a control widget from the Registry[]. This is used only in ECOS.
@param ptr, the control widget to delete.
*/
void
DeleteFromRegistry(TN_WIDGET *ptr)
{
	TN_WIDGET *temp;
	if(ptr == NULL) return;
	cyg_mutex_lock(&mut_access_registry);
	temp = ptr->prev;
	temp->next = ptr->next;
	if(ptr->next != NULL)
		(ptr->next)->prev = temp;
	ptr->next = NULL;
	ptr->prev = NULL;
	ptr->wid = 0;
	free(ptr);
	ptr = NULL;
	cyg_mutex_unlock(&mut_access_registry);
	return;
}

#else
/**
Add a new control widget to the Registry[].
@param ptr, the control widget to add.
*/
void 
AddToRegistry(TN_WIDGET *ptr)
{
	int hindex = ptr->wid % HASH_MAX_QUEUES;
	ptr->next  = Registry[hindex]->next;
	ptr->prev  = Registry[hindex];
	Registry[hindex]->next = ptr;
	if(ptr->next != NULL)
		(ptr->next)->prev = ptr;
	return;
}

/**
Get a control widget from the Registry[].
@param wid, the widget' wid.
@retval TN_WIDGET(the got widget)
*/
TN_WIDGET *
GetFromRegistry(GR_WINDOW_ID wid)
{
	int hindex = wid % HASH_MAX_QUEUES;
	TN_WIDGET *temp = Registry[hindex]->next;
	while(temp && temp->wid != wid)
		temp=temp->next;
	return temp;
}

/**
Delete a control widget from the Registry[].
@param ptr, the control widget to delete.
*/
void
DeleteFromRegistry(TN_WIDGET *ptr)
{
	TN_WIDGET *temp;
	if(ptr == NULL) return;
	temp = ptr->prev;
	temp->next = ptr->next;
	if(ptr->next != NULL)
		(ptr->next)->prev = temp;
	ptr->next = NULL;
	ptr->prev = NULL;
	free(ptr);
	ptr = NULL;
	return;
}
#endif

/**
Add the specified widget's destoy routines.
*/
void AddDestroyFuncs(void)
{
	Destroy[TN_WINDOW]=DestroyWindow;
  	Destroy[TN_BUTTON]=DestroyButton;
    Destroy[TN_LABEL]=DestroyLabel;
	Destroy[TN_CHECKBUTTON]=DestroyCheckButton;
	Destroy[TN_RADIOBUTTON]=DestroyRadioButton;
	Destroy[TN_SCROLLBAR]=DestroyScrollBar;	
	Destroy[TN_PROGRESSBAR]=DestroyProgressBar;
	Destroy[TN_LISTBOX]=DestroyListBox;
	Destroy[TN_PICTURE]=DestroyPicture;
}

/**
Add the specified widget's update routines.
*/
void AddUpdateFront(void)
{
	arrUpdateFront[TN_WINDOW] = UpdateFrontWindow;
	arrUpdateFront[TN_BUTTON] = UpdateFrontButton;
	arrUpdateFront[TN_PROGRESSBAR] = UpdateFrontProgressBar;
	arrUpdateFront[TN_LISTBOX] = UpdateFrontListBox;
	arrUpdateFront[TN_CHECKBUTTON] = UpdateFrontCheckButton;
	arrUpdateFront[TN_RADIOBUTTON] = UpdateFrontRadioButton;
	arrUpdateFront[TN_LABEL] = UpdateFrontLabel;
	arrUpdateFront[TN_PICTURE] = UpdateFrontPicture;
}

/**
Add the specified widget's evnet resolve routines.
*/		  
void AddEventDispatchFuncs(void)
{
	EventDispatch[TN_WINDOW]=WindowEventHandler;
	EventDispatch[TN_BUTTON]=ButtonEventHandler;
	EventDispatch[TN_LABEL]=LabelEventHandler;
	EventDispatch[TN_CHECKBUTTON]=CheckButtonEventHandler;
	EventDispatch[TN_RADIOBUTTON]=RadioButtonEventHandler;
	EventDispatch[TN_SCROLLBAR]=ScrollBarEventHandler;
	EventDispatch[TN_PROGRESSBAR]=ProgressBarEventHandler;
	EventDispatch[TN_LISTBOX]=ListBoxEventHandler;
	EventDispatch[TN_PICTURE]=PictureEventHandler;
}

/**
Get the specified widget's properties.
@param widget, the control widget to get.
@param props, the got properties of the widget.
*/
void tnGetWidgetProps(TN_WIDGET *widget,TN_WIDGET_PROPS *props)
{
	GR_WINDOW_INFO winfo,parentinfo;
	GR_WINDOW_ID sibling,child;
	
	props->type = widget->type;
	GrGetWindowInfo(widget->wid,&winfo);
 	props->height=winfo.height;
	props->width=winfo.width;
	GrGetWindowInfo(winfo.parent,&parentinfo);
	props->posx=winfo.x-parentinfo.x;
	props->posy=winfo.y-parentinfo.y;
	props->parent=GetFromRegistry(winfo.parent);
	sibling = winfo.sibling;
	child = winfo.child;
	while(1)
	{
		props->child=GetFromRegistry(child);
		if(props->child || !child)
			break;
		GrGetWindowInfo(child, &winfo);
		child = winfo.sibling;
		
	}
	while(1)
	{
		props->sibling=GetFromRegistry(sibling);
		if(props->sibling || !sibling)
			break;
		GrGetWindowInfo(sibling, &winfo);
		sibling = winfo.sibling;
	}
	
	return;
}

/**
Set the specified widget's properties.
@param widget, the control widget to set.
@param props, the new properties of the widget.
*/
void tnSetWidgetProps(TN_WIDGET *widget, TN_WIDGET_PROPS *props)
{
	GR_WINDOW_INFO winfo,parentinfo;
	GrGetWindowInfo(widget->wid,&winfo);
	if(winfo.height!=props->height || winfo.width!=props->width)
		GrResizeWindow(widget->wid,props->width,props->height);
	GrGetWindowInfo(winfo.parent,&parentinfo);
	if((parentinfo.x-winfo.x)!=props->posx || (parentinfo.y-winfo.y)!=props->posy)
		GrMoveWindow(widget->wid,props->posx,props->posy);
	GrClearWindow(widget->wid,GR_TRUE);
	return;
}

/**
Add a data pointer to the reserved widget structure -- widget->data.
@param widget, the control widget to add at.
@param p, the data pointer to add.
*/
void tnAttachData(TN_WIDGET *widget,DATA_POINTER p)
{
	widget->data=p;
	return;
}

/**
Get the data pointer from the reserved widget structure -- widget->data.
@param widget, the control widget to get from.
@retval the data pointer.
*/
DATA_POINTER tnGetAttachedData(TN_WIDGET *widget)
{
	return(widget->data);	
}

/**
Check if the control widget has the right parent.
@param child, the GR_WINDOW_ID to be chekced in this function.
@param parent_widget, the parent of the child.
@retval 1 for right parent & 0 for wrong.
*/
int CheckValidParent(int child,TN_WIDGET *parent_widget)
{
	int parent_type=parent_widget->type;
	
	switch(child)
	{
		case TN_WINDOW:
			return(parent_type==TN_APP_TYPE);
		case TN_BUTTON:
		case TN_SCROLLBAR:
		case TN_TEXTBOX:
		case TN_LISTBOX:
			return(parent_type==TN_WINDOW || parent_type==TN_COMBOBOX);
		case TN_LABEL:
		case TN_CHECKBUTTON:
		case TN_RADIOBUTTONGROUP:
		case TN_PICTURE:
		case TN_COMBOBOX:
		case TN_RAWWIDGET:
		case TN_PROGRESSBAR:
			return(parent_type==TN_WINDOW);
		case TN_RADIOBUTTON:
			//return(parent_type==TN_RADIOBUTTONGROUP);
			return(parent_type==TN_WINDOW);
		default:
			/*
			 * FIXME: these are actually for the menu related 
			 * widgets. There are a lot of parent checking issues 
			 * here. So currently ignore their parent types.
			 */ 
			return 1;
	}
}

/**
End the application.
@param rcinfo, the resource info of the application.
*/
void tnEndApp(PRCINFO rcinfo)
{
	destroy_rc(rcinfo);
	GrClose();
	exit(0);
}

/**
Get the parent of the widget.
@param widget, the widget to get its parent.
@retval the got parent.
*/
TN_WIDGET *GetParent(TN_WIDGET *widget)
{
	TN_WIDGET *parent;
	GR_WINDOW_INFO winfo;
	GrGetWindowInfo(widget->wid,&winfo);
	parent=GetFromRegistry(winfo.parent);
	if(!parent)
		return NULL;
	return parent;
}

/**
Get the previous sibling of the widget.
@param widget, the widget to get its previous sibling.
@retval the got previous sibling.
*/
TN_WIDGET *GetPrevSibling(TN_WIDGET *widget)
{
	GR_WINDOW_INFO	wininfo;
	TN_WIDGET	*next;

	GrGetWindowInfo(widget->wid, &wininfo);
	next = GetFromRegistry(wininfo.sibling);

	if (next == NULL) {
		GrGetWindowInfo(wininfo.parent, &wininfo);
		next = GetFromRegistry(wininfo.child);
	}			

	return next;
}

/**
Get the next sibling of the widget.
@param widget, the widget to get its next sibling.
@retval the got next sibling.
*/
TN_WIDGET *GetNextSibling(TN_WIDGET *widget)
{

	
	GR_WINDOW_INFO	wininfo, parent;
	GR_WINDOW_ID	wid, next;

	GrGetWindowInfo(widget->wid, &wininfo);
	GrGetWindowInfo(wininfo.parent, &parent);

	wid = parent.child; 
	while (wid != widget->wid) {
		GrGetWindowInfo(wid, &wininfo);
		next = wid;
		wid = wininfo.sibling;
	}

	if (wid == parent.child)
	{
		wid = parent.child;
		while (wid != 0) {
			GrGetWindowInfo(wid, &wininfo);
			next = wid;	
			wid = wininfo.sibling;
		}
	}

	return GetFromRegistry(next); 
}

/**
The update function dispatcher.
@param widget, the widget to have its UpdateFront function.
@param rcinfo, the resource info pointer.
*/
void UpdateFront(TN_WIDGET *widget, PRCINFO rcinfo)
{
	switch(widget->type)
	{
		case TN_WINDOW:
			arrUpdateFront[TN_WINDOW](widget, rcinfo);
			break;
		case TN_BUTTON:
			arrUpdateFront[TN_BUTTON](widget, rcinfo);
			break;
		case TN_PROGRESSBAR:
			arrUpdateFront[TN_PROGRESSBAR](widget, rcinfo);
			break;
		case TN_LISTBOX:
			arrUpdateFront[TN_LISTBOX](widget, rcinfo);
			break;
		case TN_CHECKBUTTON:
			arrUpdateFront[TN_CHECKBUTTON](widget, rcinfo);
			break;
		case TN_PICTURE:
			arrUpdateFront[TN_PICTURE](widget, rcinfo);
			break;
		case TN_RADIOBUTTON:
			arrUpdateFront[TN_RADIOBUTTON](widget, rcinfo);
			break;
		case TN_LABEL:
			arrUpdateFront[TN_LABEL](widget, rcinfo);
			break;
		default:
			break;
	}
}

/**
To draw text with the transparent background.
@param wid, the widget's wid you want to draw the texts.
@param gc, the gc id.
@param x, the X coordinates of the left-top corner of the texts.
@param y, the Y coordinates of the left-top corner of the texts.
@param caption, the text string to draw.
@param count, how many character of the caption to show.
@parent flag, which type of the text to show(TF_ASCII, eg.)
*/
void tnDrawText(GR_WINDOW_ID wid, GR_GC_ID gc, int x, int y, char *caption, int count, int flag)
{
	GR_GC_ID	gcDraw;
	GR_GC_ID	gcCopy;
	
	gcDraw = GrNewGC();
	GrSetGCForeground(gcDraw, GR_ARGB(250,0, 0, 0));
	GrSetGCBackground(gcDraw, GR_ARGB(250,0, 0, 0));
	GrSetGCMode(gcDraw, GR_MODE_COPY);	
	GrSetGCFont(gcDraw, TN_DEFAULT_FONT_NO);
	GrText(wid, gcDraw,x, y, caption, count, flag);

	gcCopy = GrCopyGC(gc);
	GrSetGCBackground(gcCopy, GR_ARGB(250,0, 0, 0));
	GrSetGCMode(gcCopy, GR_MODE_COPY);
	GrSetGCFont(gcCopy, TN_DEFAULT_FONT_NO);
	GrText(wid, gcCopy,x, y, caption, count, flag);

	GrDestroyGC(gcDraw);
	GrDestroyGC(gcCopy);
}	

