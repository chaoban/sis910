#include "tnWidgets.h"
/** Create Scroll Bar widget
@param parent, the parent of the scroll bar to be created
@param flags, the types of the scroll bar including the visible, enable and transparent
@param rect, the rect of the scroll bar to be created
@param orientation, the orientation of the scroll bar
@param minval, the min value is always set to be 0
@param maxval, the max value is always set to be the numbers of the listbox items
@param linestep, the line step is always set to be 1
@param pagestep, the page step is always set to be 5
@param imageid[], it's 2 dimensions, the first unit is the background image id, and the second is the thumb image id
*/
TN_WIDGET *CreateScrollBar(TN_WIDGET * parent, int flags, GR_RECT *rect, int orientation,
	      int minval, int maxval, int linestep, int pagestep, GR_IMAGE_ID imageid[])
{
	TN_WIDGET	*widget = NULL;
	int thumbsize;
	//GR_IMAGE_INFO iinfo;

	if(orientation==TN_VERTICAL){
  		if (rect->height == 0 || rect->height < (3*TN_SCROLLBAR_COMPONENT_SIZE)) 
  	    		rect->height = TN_SCROLLBAR_VERTICAL_HEIGHT;
  		if(rect->width == 0)
  	    		rect->width = TN_SCROLLBAR_VERTICAL_WIDTH;
  	}
  	else{
	  	if (rect->height == 0)
     		  	rect->height = TN_SCROLLBAR_HORIZONTAL_HEIGHT;
 	  	if(rect->width == 0 || rect->width < (3*TN_SCROLLBAR_COMPONENT_SIZE))
     		  	rect->width = TN_SCROLLBAR_HORIZONTAL_WIDTH;
  	}
	
	// create widget for scrollbar
	widget = CreateWidget(parent, rect, GR_EVENT_MASK_BUTTON_UP | GR_EVENT_MASK_BUTTON_DOWN | GR_EVENT_MASK_MOUSE_MOTION |
		  GR_EVENT_MASK_EXPOSURE, flags, 0);
	
	if (widget == NULL) return NULL;

	// initialize widget.
	widget->type = TN_SCROLLBAR;
	widget->WSpec.scrollbar.imageid[0] = imageid[0];
	widget->WSpec.scrollbar.imageid[1] = imageid[1];
  	widget->WSpec.scrollbar.orientation=orientation;
  	widget->WSpec.scrollbar.minval=(minval==-1)?TN_SCROLLBAR_MINVAL:minval;
  	widget->WSpec.scrollbar.maxval=(maxval==-1)?TN_SCROLLBAR_MAXVAL:maxval;
  	widget->WSpec.scrollbar.pagestep=(pagestep==-1)?TN_SCROLLBAR_PAGESTEP:pagestep;
  	widget->WSpec.scrollbar.linestep=(linestep==-1)?TN_SCROLLBAR_LINESTEP:linestep;
    	widget->WSpec.scrollbar.thumbpos=widget->WSpec.scrollbar.minval;
  	widget->WSpec.scrollbar.LastScrollEvent=TN_SCROLL_NOSCROLL;
	widget->WSpec.scrollbar.st_thumb_down = GR_FALSE;
	
  	thumbsize = TN_SCROLLBAR_COMPONENT_SIZE - 
		((widget->WSpec.scrollbar.maxval - widget->WSpec.scrollbar.minval - 100)*TN_SCROLLBAR_COMPONENT_SIZE)/100;
  	if(orientation == TN_VERTICAL)
	  	widget->WSpec.scrollbar.thumbsize = (rect->height - 40) * thumbsize / 120;
  	else
	  	widget->WSpec.scrollbar.thumbsize = (rect->width - 40) * thumbsize / 120;
	/*
 	GrGetImageInfo(widget->WSpec.scrollbar.imageid[1], &iinfo);
	if(orientation==TN_VERTICAL)
		thumbsize = iinfo.height;
	else
		thumbsize = iinfo.width;

	widget->WSpec.scrollbar.thumbsize = thumbsize;
	
  	if(widget->WSpec.scrollbar.thumbsize < TN_MIN_THUMBSIZE)
	  	widget->WSpec.scrollbar.thumbsize = TN_MIN_THUMBSIZE;   

  	if(orientation==TN_VERTICAL){
		  widget->WSpec.scrollbar.thumb=
		  	GrNewWindow (widget->wid, 1, 1, rect->width, widget->WSpec.scrollbar.thumbsize , 0, 0, 0);
   	}
  	else{
		  widget->WSpec.scrollbar.thumb=
		  	GrNewWindow (widget->wid, 1, 1, widget->WSpec.scrollbar.thumbsize, rect->height, 0, 0, 0);  
  	}
  	
	GrMapWindow(widget->WSpec.scrollbar.thumb);
  	GrSelectEvents (widget->WSpec.scrollbar.thumb,0);
	*/
  	return widget;
}

/**
Event Handler for the Scroll Bar
@param evnet, the event structure
@param widget, the widget generate the event
*/
void
ScrollBarEventHandler (GR_EVENT * event, TN_WIDGET * widget)
{

    int min = widget->WSpec.scrollbar.minval;
    int max = widget->WSpec.scrollbar.maxval;	
    int range = max - min;	
    
#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "ScrollBarEventHandler");
#endif
    
    switch (event->type)
    {
	case GR_EVENT_TYPE_BUTTON_DOWN:
	if(!widget->enabled || !widget->visible)
	  	break;	
	/*No Need to handle the event
      if(range==0)
	      widget->WSpec.scrollbar.LastScrollEvent=TN_SCROLL_NOSCROLL;
      else if(event->button.subwid == widget->WSpec.scrollbar.upleft)
      {
	      widget->WSpec.scrollbar.st_upleft_down=GR_TRUE;
	      widget->WSpec.scrollbar.LastScrollEvent=TN_SCROLL_LINEDOWN;
 	      if((widget->WSpec.scrollbar.thumbpos - widget->WSpec.scrollbar.linestep) >= min)
	         widget->WSpec.scrollbar.thumbpos-=widget->WSpec.scrollbar.linestep;
	      else
		 widget->WSpec.scrollbar.thumbpos = widget->WSpec.scrollbar.minval;
      }
     
      else if(event->button.subwid == widget->WSpec.scrollbar.downright)
      {
	      widget->WSpec.scrollbar.st_downright_down=GR_TRUE;
	      widget->WSpec.scrollbar.LastScrollEvent=TN_SCROLL_LINEUP;	      
	      if( (widget->WSpec.scrollbar.thumbpos + widget->WSpec.scrollbar.linestep) <= max)
	      widget->WSpec.scrollbar.thumbpos+=widget->WSpec.scrollbar.linestep;
	      else
		      widget->WSpec.scrollbar.thumbpos = widget->WSpec.scrollbar.maxval;
      }
      else if(event->button.subwid==widget->WSpec.scrollbar.thumb)
      {
	      //Drag thumb code
	      widget->WSpec.scrollbar.st_thumb_down=GR_TRUE;
      }
      
      else if(event->button.subwid == widget->wid)
      {
	      GrGetWindowInfo(widget->wid,&winfo_scrollbar);     
	      GrGetWindowInfo(widget->WSpec.scrollbar.thumb,&winfo_thumb);
	      //x=abs(winfo_scrollbar.x-winfo_thumb.x);
	      //y=abs(winfo_scrollbar.y-winfo_thumb.y);
	      if(widget->WSpec.scrollbar.orientation == TN_VERTICAL)
	      	{
			if(event->button.y < winfo_thumb.y)			//PageUp
			{
				widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_PAGEUP;
				
		      		if( (widget->WSpec.scrollbar.thumbpos - widget->WSpec.scrollbar.pagestep) >= min)
	   		      		widget->WSpec.scrollbar.thumbpos-=widget->WSpec.scrollbar.pagestep;
    		      		else
    			      		widget->WSpec.scrollbar.thumbpos = min;
			}
			else if(event->button.y > winfo_thumb.y + winfo_thumb.height)	//PageDown
			{
				widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_PAGEDOWN;
				
		      		if( (widget->WSpec.scrollbar.thumbpos + widget->WSpec.scrollbar.pagestep) <= max)
	   		      		widget->WSpec.scrollbar.thumbpos += widget->WSpec.scrollbar.pagestep;
    		      		else
    			      		widget->WSpec.scrollbar.thumbpos = max;
			}
			else									//No Scroll
				widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_NOSCROLL;				
	      	}
		else if(widget->WSpec.scrollbar.orientation == TN_HORIZONTAL)
		{
			if(event->button.x < winfo_thumb.x)			//PageUp
			{
				widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_PAGEUP;
				
		      		if( (widget->WSpec.scrollbar.thumbpos - widget->WSpec.scrollbar.pagestep) >= min)
	   		      		widget->WSpec.scrollbar.thumbpos-=widget->WSpec.scrollbar.pagestep;
    		      		else
    			      		widget->WSpec.scrollbar.thumbpos = min;
			}
			else if(event->button.x > winfo_thumb.x + winfo_thumb.height)	//PageDown
			{
				widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_PAGEDOWN;
				
		      		if( (widget->WSpec.scrollbar.thumbpos + widget->WSpec.scrollbar.pagestep) <= max)
	   		      		widget->WSpec.scrollbar.thumbpos += widget->WSpec.scrollbar.pagestep;
    		      		else
    			      		widget->WSpec.scrollbar.thumbpos = max;
			}
			else									//No Scroll
				widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_NOSCROLL;
		}
      }      
	  else
	  	widget->WSpec.scrollbar.LastScrollEvent = TN_SCROLL_NOSCROLL;
	  
      if(range!=0)
      {
       GrClearWindow (widget->wid, GR_FALSE);
       DrawScrollBar(widget);
      }
      
      if (!widget->WSpec.scrollbar.st_thumb_down && widget->WSpec.scrollbar.CallBack[CLICKED].fp)	
	  (*(widget->WSpec.scrollbar.CallBack[CLICKED].fp)) (widget, widget->WSpec.scrollbar.CallBack[CLICKED].dptr);*/
      break;
      
    case GR_EVENT_TYPE_BUTTON_UP:
      if(range == 0 || !widget->enabled || !widget->visible)
		break;	
	/*No Need to handle the event
      widget->WSpec.scrollbar.st_upleft_down=GR_FALSE;
      widget->WSpec.scrollbar.st_downright_down=GR_FALSE;
      GrClearWindow(widget->wid,GR_FALSE);
	DrawScrollBar(widget);
	*/
	if(widget->WSpec.scrollbar.st_thumb_down)
	{	  
		widget->WSpec.scrollbar.st_thumb_down=GR_FALSE;
		/*DrawThumb(widget);
		if(widget->WSpec.scrollbar.CallBack[CLICKED].fp)      
			(*(widget->WSpec.scrollbar.CallBack[CLICKED].fp)) (widget,widget->WSpec.scrollbar.CallBack[CLICKED].dptr);*/
	}
		break;	 
	case GR_EVENT_TYPE_MOUSE_MOTION:
		if(range == 0 || !widget->enabled || !widget->visible)
			break;
	/*No Need to handle the event
		if(widget->WSpec.scrollbar.st_thumb_down)
		{
			widget->WSpec.scrollbar.LastScrollEvent= TN_SCROLL_THUMBMOVE;
			
			GrGetWindowInfo(widget->wid,&winfo_scrollbar);
			GrGetWindowInfo(widget->WSpec.scrollbar.thumb,&winfo_thumb);
			if(widget->WSpec.scrollbar.orientation==TN_VERTICAL)
				new_thumbpos = event->button.y + widget->WSpec.scrollbar.thumbpos - winfo_thumb.y - winfo_thumb.height/2;
			else
				new_thumbpos = event->button.x + widget->WSpec.scrollbar.thumbpos - winfo_thumb.x - winfo_thumb.width/2; 

			if(new_thumbpos > max)
				new_thumbpos=max;
			else if(new_thumbpos < min)
				new_thumbpos=min;
			widget->WSpec.scrollbar.thumbpos=new_thumbpos;
			
			DrawThumb(widget);
	
			if(widget->WSpec.scrollbar.CallBack[CLICKED].fp)      
			{
				(*(widget->WSpec.scrollbar.CallBack[CLICKED].fp)) (widget,
							widget->WSpec.scrollbar.CallBack[CLICKED].dptr);			
			}*/
		break;	    
		
	case GR_EVENT_TYPE_EXPOSURE:
	      if(widget->visible)
	         DrawScrollBar(widget);
      break;
    }
}

/** Scroll Bar draw routine 
@param scrollbar, the Scroll Bar to DRAW!
*/
void DrawScrollBar(TN_WIDGET *scrollbar)
{
	GR_WINDOW_INFO winfo;
	GR_WINDOW_ID	pid;
	int available_pixels,new_mid;
	int new_x,new_y;
	int op;
	
	GrGetWindowInfo (scrollbar->wid, &winfo);
	pid = GrNewPixmap(winfo.width, winfo.height, NULL);
	
	GrDrawImageToFit(pid, scrollbar->gc, 0, 0, 
			winfo.width, winfo.height, scrollbar->WSpec.scrollbar.imageid[0]);

	if((scrollbar->WSpec.scrollbar.maxval - scrollbar->WSpec.scrollbar.minval) == 0 || !scrollbar->enabled)	
	{
		//GrDrawImageToFit(pid, scrollbar->gc, 0, 0, 
		//	winfo.width, winfo.height, scrollbar->WSpec.scrollbar.imageid[1]);
			printf("%s - return\n",__FUNCTION__);
	    GrDestroyWindow(pid);
		return;
	}

	if(scrollbar->WSpec.scrollbar.orientation==TN_VERTICAL)
	{
		available_pixels = winfo.height - scrollbar->WSpec.scrollbar.thumbsize;
		if(available_pixels)
		{
			new_mid=(((available_pixels*(scrollbar->WSpec.scrollbar.thumbpos-scrollbar->WSpec.scrollbar.minval))/(scrollbar->WSpec.scrollbar.maxval-scrollbar->WSpec.scrollbar.minval)))+(scrollbar->WSpec.scrollbar.thumbsize/2);
			new_x=0;
			new_y=new_mid-(scrollbar->WSpec.scrollbar.thumbsize/2);
			GrDrawImageToFit(pid, scrollbar->gc, new_x, new_y, 
					winfo.width, scrollbar->WSpec.scrollbar.thumbsize, scrollbar->WSpec.scrollbar.imageid[1]);
		}
	}
	else
	{
		available_pixels = winfo.width - scrollbar->WSpec.scrollbar.thumbsize;
	      	if(available_pixels)
		{
			new_mid=(((available_pixels*(scrollbar->WSpec.scrollbar.thumbpos-scrollbar->WSpec.scrollbar.minval))/(scrollbar->WSpec.scrollbar.maxval-scrollbar->WSpec.scrollbar.minval)))+(scrollbar->WSpec.scrollbar.thumbsize/2);
			new_x=new_mid-(scrollbar->WSpec.scrollbar.thumbsize/2);
			new_y=0;
			GrDrawImageToFit(pid, scrollbar->gc, new_x, new_y, 
					scrollbar->WSpec.scrollbar.thumbsize, winfo.height, scrollbar->WSpec.scrollbar.imageid[1]);
     		}	
	}
	
	if (scrollbar->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;
	
	GrCopyArea(scrollbar->wid, scrollbar->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrDestroyWindow(pid);
}

/*Thumb draw routine*/
/*
void
DrawThumb(TN_WIDGET * scrollbar)
{
	GR_WINDOW_ID  wid=scrollbar->WSpec.scrollbar.thumb;
	GR_WINDOW_INFO winfo;
	int available_pixels,new_mid;
	int new_x,new_y;

	GrGetWindowInfo(scrollbar->wid,&winfo);
 	if((scrollbar->WSpec.scrollbar.maxval - scrollbar->WSpec.scrollbar.minval) == 0 || !scrollbar->enabled)	
	{
		if(scrollbar->WSpec.scrollbar.orientation==TN_VERTICAL)
			GrDrawImageToFit(wid, scrollbar->gc, 0, 0, 
				winfo.width, winfo.height/2, scrollbar->WSpec.scrollbar.imageid[1]);
		else
			GrDrawImageToFit(wid, scrollbar->gc, 0, 0, 
				winfo.width/2, winfo.height, scrollbar->WSpec.scrollbar.imageid[1]);
		return;
	}
	
	if(scrollbar->WSpec.scrollbar.orientation==TN_VERTICAL)
	{
		available_pixels = winfo.height - scrollbar->WSpec.scrollbar.thumbsize;
		if(available_pixels)
		{
			new_mid=(((available_pixels*(scrollbar->WSpec.scrollbar.thumbpos-scrollbar->WSpec.scrollbar.minval))/(scrollbar->WSpec.scrollbar.maxval-scrollbar->WSpec.scrollbar.minval)))+(scrollbar->WSpec.scrollbar.thumbsize/2);
			new_x=0;
			new_y=new_mid-(scrollbar->WSpec.scrollbar.thumbsize/2);
			GrMoveWindow(wid,new_x,new_y);
		}
	}
	else
	{
		available_pixels = winfo.width - scrollbar->WSpec.scrollbar.thumbsize;
	       	if(available_pixels)
		{
			new_mid=(((available_pixels*(scrollbar->WSpec.scrollbar.thumbpos-scrollbar->WSpec.scrollbar.minval))/(scrollbar->WSpec.scrollbar.maxval-scrollbar->WSpec.scrollbar.minval)))+(scrollbar->WSpec.scrollbar.thumbsize/2);
			new_x=new_mid-(scrollbar->WSpec.scrollbar.thumbsize/2);
			new_y=0;
			GrMoveWindow(wid,new_x,new_y);
     		}	
	}
	GrClearWindow(wid,GR_FALSE);
	GrGetWindowInfo(wid,&winfo);
	
	GrDrawImageToFit(wid, scrollbar->gc, 0, 0, 
		winfo.width, winfo.height, scrollbar->WSpec.scrollbar.imageid[1]);

	return;
}
*/

/** Get the last event of the Scroll Bar. But NOT used now!
@param widget, the ScrollBar to get;
@retval TN_SCROLL_xxxxx(in tnScrollBar.h)
*/	
int tnGetLastScrollEvent(TN_WIDGET *widget)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;

	else return(widget->WSpec.scrollbar.LastScrollEvent);
}

/** Set the last event of the Scroll Bar. But NOT used now!
@param widget, the ScrollBar to set;
@param lastevent, the TN_SCROLL_xxxx types to set.
@retval -1 for FAIL & 0 for SUCCESS
*/
int tnSetLastScrollEvent(TN_WIDGET *widget, int lastevent)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;

	widget->WSpec.scrollbar.LastScrollEvent = lastevent;

	return 0;
}

/** Get the thumb position of the Scroll Bar. 
@param widget, the ScrollBar to get;
@return the current thumb position of the Scroll Bar
*/
int tnGetThumbPosition(TN_WIDGET *widget)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;
	else return(widget->WSpec.scrollbar.thumbpos);
}

/** Set the scroll range of the Scroll Bar. 
@param widget, the ScrollBar to set;
@param minval, the min value of the Scroll Bar
@param maxval, the max value of the Scroll Bar
*/
int tnSetScrollRange(TN_WIDGET *scrollbar,int minval,int maxval)
{
	GR_WINDOW_INFO winfo;
	int width,height;//thumbsize;
	if(scrollbar->type!=TN_SCROLLBAR)
		return -1;
	if(maxval < minval)
	  return -1;
	scrollbar->WSpec.scrollbar.minval = minval;
	scrollbar->WSpec.scrollbar.maxval = maxval;
	/*
	if(minval>maxval)
		thumbsize=TN_SCROLLBAR_COMPONENT_SIZE-((scrollbar->WSpec.scrollbar.maxval-scrollbar->WSpec.scrollbar.minval-100)*TN_SCROLLBAR_COMPONENT_SIZE)/100;
	else
		thumbsize=TN_SCROLLBAR_COMPONENT_SIZE;
	*/
	
	GrGetWindowInfo(scrollbar->wid,&winfo);
	/*
	if(scrollbar->WSpec.scrollbar.orientation == TN_VERTICAL)
		scrollbar->WSpec.scrollbar.thumbsize= (winfo.height-40) * thumbsize/120;
	else
		scrollbar->WSpec.scrollbar.thumbsize= (winfo.width-40) * thumbsize/120;
	*/
	if(scrollbar->WSpec.scrollbar.thumbsize<TN_MIN_THUMBSIZE)
          scrollbar->WSpec.scrollbar.thumbsize=TN_MIN_THUMBSIZE;
	if(scrollbar->WSpec.scrollbar.thumbpos > maxval)
		scrollbar->WSpec.scrollbar.thumbpos = maxval;
	if(scrollbar->WSpec.scrollbar.thumbpos < minval)
		scrollbar->WSpec.scrollbar.thumbpos = minval;

	if(scrollbar->WSpec.scrollbar.orientation == TN_VERTICAL)
	{
		width = winfo.width;
		height = scrollbar->WSpec.scrollbar.thumbsize;
	}
	else
	{
		width = scrollbar->WSpec.scrollbar.thumbsize;
		height = winfo.height;
	}
	//GrResizeWindow(scrollbar->WSpec.scrollbar.thumb,width,height);
	GrClearWindow(scrollbar->wid,GR_FALSE);
	DrawScrollBar(scrollbar);
	return 1;
}

/** Set the thumb position of the Scroll Bar. 
@param widget, the ScrollBar to set;
@param n, the thumb position to set.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnSetThumbPosition(TN_WIDGET *scrollbar,int n)
{
	if(scrollbar->type!=TN_SCROLLBAR)
                return -1;
	if(n < scrollbar->WSpec.scrollbar.minval || n > scrollbar->WSpec.scrollbar.maxval)
		return -1;
	 scrollbar->WSpec.scrollbar.thumbpos = n;
	 GrClearWindow(scrollbar->wid,GR_FALSE);
	 DrawScrollBar(scrollbar);
	 return 1;
}

/** Scroll Bar destroy routine 
@param widget, the Scroll Bar to DESTROY!
*/
void DestroyScrollBar(TN_WIDGET *widget)
{
	if(widget->WSpec.scrollbar.imageid[0])
		GrFreeImage(widget->WSpec.scrollbar.imageid[0]);
	if(widget->WSpec.scrollbar.imageid[1])
		GrFreeImage(widget->WSpec.scrollbar.imageid[1]);
	
	DeleteFromRegistry(widget);
	return;
}

/** Get the orientation of the Scroll Bar. 
@param widget, the ScrollBar to get;
@param orientation, the orientation to get.
@retval the current thumb position of the Scroll Bar
*/
int tnGetScrollBarOrientation(TN_WIDGET *widget, int *orientation)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;
	*orientation=widget->WSpec.scrollbar.orientation;
	return 1;
}

/** Set the orientation of the Scroll Bar.
@param widget, the ScrollBar to set;
@param orientation, the orientation to set.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnSetScrollBarOrientation(TN_WIDGET *widget, int orientation)
{
	GR_WINDOW_INFO winfo;
	int height,width;
	
	if(widget->type!=TN_SCROLLBAR)
		return -1;
	if(orientation==widget->WSpec.scrollbar.orientation)
		return 1;
	
  	GrGetWindowInfo(widget->wid,&winfo);
	height=winfo.height;
	width=winfo.width;
	
	if(orientation==TN_VERTICAL)
  	{
  		if (height == 0||height<(3*TN_SCROLLBAR_COMPONENT_SIZE)) 
  		    height = TN_SCROLLBAR_VERTICAL_HEIGHT;
  		    
		width = TN_SCROLLBAR_VERTICAL_WIDTH;
  	}
  	else
  	{
	     	  height = TN_SCROLLBAR_HORIZONTAL_HEIGHT;
	 	  if(width==0||width<(3*TN_SCROLLBAR_COMPONENT_SIZE))
     			  width = TN_SCROLLBAR_HORIZONTAL_WIDTH;
  	}
	
	GrResizeWindow(widget->wid,width,height);
	
	widget->WSpec.scrollbar.orientation=orientation;
	GrClearWindow(widget->wid,GR_TRUE);
	return 1;
}

/** Get the scroll range of the Scroll Bar. 
@param widget, the ScrollBar to get;
@param minval, the min value to get.
@param maxval, the max value to get.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnGetScrollRange(TN_WIDGET *widget, int *minval,int *maxval)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;
	*minval=widget->WSpec.scrollbar.minval;
	*maxval=widget->WSpec.scrollbar.maxval;
	return 1;
}

/** Get the scroll step sizes of the Scroll Bar. 
@param widget, the ScrollBar to get;
@param pagestep, the page step to get.
@param linestep, the line step to get.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnGetScrollStepSizes(TN_WIDGET *widget, int *pagestep,int *linestep)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;
	*pagestep=widget->WSpec.scrollbar.pagestep;
	*linestep=widget->WSpec.scrollbar.linestep;
	return 1;
}

/** Set the scroll step sizes of the Scroll Bar.
@param widget, the ScrollBar to set;
@param pagestep, the page step to set
@param linestep, the line step to set
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnSetScrollStepSizes(TN_WIDGET *widget, int pagestep,int linestep)
{
	if(widget->type!=TN_SCROLLBAR)
		return -1;
	widget->WSpec.scrollbar.pagestep=pagestep;
	widget->WSpec.scrollbar.linestep=linestep;
	return 1;
}

