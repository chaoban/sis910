#include "tnWidgets.h"
/** Create Picture widget
@param parent, the parent of the picture to be created
@param flags, the types of the picture including the visible, enable and transparent
@param rect, the rect of the picture to be created
@param fname, the full name of the picture including the full absolute directory
@param stretch, whether to stretch the picture, NOTICE: TRUE is NOT to stretch and FALSE to stretch the picture!!!
*/
TN_WIDGET *CreatePicture(TN_WIDGET *parent, int flags, GR_RECT *rect, char *fname, GR_BOOL stretch)
{
	TN_WIDGET	*pWidget;

	pWidget = CreateWidget(parent, rect, GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_FOCUS_IN | GR_EVENT_MASK_FOCUS_OUT
		       	| GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMER, flags, 0);

	if (pWidget == NULL) return NULL;	

	// initialize picture related informatino.
	pWidget->type = TN_PICTURE;
	pWidget->WSpec.picture.imageid = 0;
	pWidget->WSpec.picture.bFocus = GR_FALSE;
		
	if (fname != NULL) {
		pWidget->WSpec.picture.filename = (char *)malloc((strlen(fname) + 1) * sizeof(char));
		strcpy((pWidget->WSpec.picture.filename), fname);
		pWidget->WSpec.picture.imageid = GrLoadImageFromFile(fname, 0);
	} else 
		pWidget->WSpec.picture.filename = NULL;

	pWidget->WSpec.picture.stretch = stretch;
	pWidget->WSpec.picture.fscale = 1;
	pWidget->WSpec.picture.offsetX = 0;
    pWidget->WSpec.picture.offsetY = 0;

	GrSetGCUseBackground(pWidget->gc, GR_FALSE);

	return pWidget;
}	

/**
Event Handler for the Picture 
@param evnet, the event structure
@param widget, the widget generate the event
*/
void
PictureEventHandler (GR_EVENT * event, TN_WIDGET * widget)
{
    
#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "PictureEventHandler");
#endif

  switch (event->type)
    {
     case GR_EVENT_TYPE_FOCUS_IN:
		if(!widget->visible || !widget->enabled)
			break; 
		widget->WSpec.picture.bFocus = GR_TRUE;
		GrClearWindow(widget->wid, GR_FALSE);
		DrawPicture(widget);
		break;

	case GR_EVENT_TYPE_FOCUS_OUT:
		if(!widget->visible || !widget->enabled)
			break; 
		widget->WSpec.picture.bFocus = GR_FALSE;
		GrClearWindow(widget->wid, GR_FALSE);
		DrawPicture(widget);
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
    	case GR_EVENT_TYPE_EXPOSURE:
		if(widget->visible)	
	      		DrawPicture (widget);
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


/**Picture Draw routine
@param picture, the Picture to DRAW!
*/
void
DrawPicture (TN_WIDGET * picture)
{
	GR_WINDOW_ID	pid;
	GR_WINDOW_INFO winfo;
 	GR_IMAGE_INFO iinfo;
 	int nsrcx, nsrcy, nsrcwidth, nsrcheight;
 	int newWidth = 0, newHeight = 0;
	int 	op;

  	if(picture->WSpec.picture.imageid == 0)
	  return;
  	GrGetWindowInfo (picture->wid, &winfo);
	GrGetImageInfo(picture->WSpec.picture.imageid,&iinfo);
	if(picture->WSpec.picture.stretch)
  	{
  	     
  	    if((iinfo.width > winfo.width)||(iinfo.height > winfo.height))
	    {
	            newHeight = iinfo.height*winfo.width / iinfo.width;
	            if(newHeight > winfo.height)
	            {
	                newWidth = iinfo.width*winfo.height / iinfo.height;
	                newHeight = winfo.height;
	            }
	            else
	                newWidth = winfo.width;	            	            
	    }
	    else	        
	    {
	        newWidth = iinfo.width;
	        newHeight = iinfo.height;
	    }
	  	GrResizeWindow(picture->wid,newWidth,newHeight);
	  	GrMoveWindow(picture->wid, winfo.x + (winfo.width-newWidth)/2, winfo.y + (winfo.height-newHeight)/2);
	  	GrGetWindowInfo(picture->wid,&winfo);
	  	//picture->WSpec.picture.fscale = 2.0;
	  	//nsrcwidth = iinfo.width*picture->WSpec.picture.fscale;
        //nsrcheight = iinfo.height*picture->WSpec.picture.fscale;
	  	//nsrcx = (iinfo.width / 2 + picture->WSpec.picture.offsetX) * picture->WSpec.picture.fscale -(winfo.width/2);
        //nsrcy = (iinfo.height / 2 + picture->WSpec.picture.offsetY) * picture->WSpec.picture.fscale  -(winfo.height/2);
        nsrcwidth = winfo.width*picture->WSpec.picture.fscale;
        nsrcheight = winfo.height*picture->WSpec.picture.fscale;
	  	nsrcx = (winfo.width / 2 + picture->WSpec.picture.offsetX) * picture->WSpec.picture.fscale -(winfo.width/2);
        nsrcy = (winfo.height / 2 + picture->WSpec.picture.offsetY) * picture->WSpec.picture.fscale  -(winfo.height/2);

        newWidth = winfo.width;
	    newHeight = winfo.height;
  	}
  	else
  	{
        nsrcx = 0;
        nsrcy = 0;
        nsrcwidth = winfo.width;
        nsrcheight = winfo.height;
        newWidth = winfo.width;
	    newHeight = winfo.height;
	    
  	}		
  	//pid = GrNewPixmap(nsrcwidth, nsrcheight, NULL);
  	//GrDrawImageToFit(picture->wid,picture->gc,0,0,nsrcwidth, nsrcheight,picture->WSpec.picture.imageid);  	
  	pid = GrNewPixmap(nsrcwidth, nsrcheight, NULL);
  	GrDrawImageToFit(pid,picture->gc,0,0,nsrcwidth, nsrcheight,picture->WSpec.picture.imageid);
    
 	if(picture->WSpec.picture.bFocus){
	//To be edit ??	  
  	}
  	
  	if (picture->CallBack[WIDGET_EXPOSURE].fp) 
	{		
		picture->CallBack[WIDGET_EXPOSURE].fp(picture, 
			picture->CallBack[WIDGET_EXPOSURE].dptr);
	}
	
	if (picture->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
	else
		op = MWROP_COPY;
    
	//GrCopyArea(picture->wid, picture->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	GrCopyArea(picture->wid, picture->gc, 0, 0, winfo.width, winfo.height, pid, nsrcx, nsrcy, op);
	GrDestroyWindow(pid);

/*  GrDrawImageFromFile(picture->wid,picture->gc,0,0,winfo.width,winfo.height,picture->WSpec.picture.filename,0);*/
}

/** Set the Picture stretch flag 
@param widget, the Window to set the stretch flag.
@param stretch, the stretch flag to set.
@retval -1 for FAIL & 1 for success.
*/
int 
tnSetPictureStretch(TN_WIDGET *widget,GR_BOOL stretch, int scale, int x, int y)
{
	if(widget->type != TN_PICTURE)
		return -1;
	widget->WSpec.picture.stretch = stretch;
	widget->WSpec.picture.fscale = (float) scale / 100;
    widget->WSpec.picture.offsetX = x;
    widget->WSpec.picture.offsetY = y;
	GrClearWindow(widget->wid,GR_FALSE);
	DrawPicture(widget);
	return 1;
}

/** Set the Picture content 
@param widget, the Window to set the stretch flag.
@param filename, full name of the picture
@retval -1 for FAIL & 1 for success.
*/
int 
tnSetPicture(TN_WIDGET *widget ,char *filename)
{
     if(widget->type != TN_PICTURE)
		return -1;
	 if(filename == NULL)
		 return -1;	
	 free(widget->WSpec.picture.filename); 	
	 widget->WSpec.picture.filename =
	            (char *)malloc((strlen(filename)+1)*sizeof(char));
	 strcpy ((widget->WSpec.picture.filename), filename);
	 GrFreeImage(widget->WSpec.picture.imageid);
     widget->WSpec.picture.imageid = GrLoadImageFromFile(filename,0);
	 GrClearWindow(widget->wid,GR_FALSE);
	 DrawPicture(widget);
	 return 1;
}

/** Set the Picture content 
@param widget, the Window to set the stretch flag.
@param id, image id  name of the picture
@retval -1 for FAIL & 1 for success.
*/
int 
tnSetPictureID(TN_WIDGET *widget ,GR_IMAGE_ID id)
{
    if(widget->type != TN_PICTURE)
	    return -1;
	if(id == 0)
		 return -1;	
	if( id != widget->WSpec.picture.imageid)
	{
        widget->WSpec.picture.imageid = id;
	    GrClearWindow(widget->wid,GR_FALSE);
	    DrawPicture(widget);
	}
	
	return 1;
}

/** Picture destroy routine 
@param widget, the Picture to DESTROY!
*/
void DestroyPicture(TN_WIDGET *widget)
{
	if(widget->WSpec.picture.filename)
		free(widget->WSpec.picture.filename);
	if (widget->WSpec.picture.imageid)
		GrFreeImage(widget->WSpec.picture.imageid);
	
	DeleteFromRegistry(widget);
	return;
}

/** Dup two Pictures
@param dest, the destination Picture to DUP.
@param src, the source Picture to DUP.
@retval -1 for FAIL & 1 for success.
*/
int
tnPictureDup(TN_WIDGET *dest,TN_WIDGET *src)
{
	if(!dest || dest->type!=TN_PICTURE)
		return -1;
	if(!src || src->type!=TN_PICTURE)
		return -1;
	dest->WSpec.picture.imageid = src->WSpec.picture.imageid;
	GrClearWindow(dest->wid,GR_TRUE);
	return 1;
}

/** Get the Picture's property.
@param widget, the Picture to get.
@param stretch, the stretch flag to get.
@param stretch, the file name to get.
@retval -1 for FAIL & 1 for success.
*/
int
tnGetPictureProps(TN_WIDGET *widget,GR_BOOL *stretch, char **filename)
{
	 if(widget->type != TN_PICTURE)
		 return -1;
	 *stretch = widget->WSpec.picture.stretch;
	 if(widget->WSpec.picture.filename)
		 *filename = (char *) strdup(widget->WSpec.picture.filename);
	 return 1;
}

/** Get the Picture's image property.
@param widget, the Picture to get.
@param iinfo, the image info to get.
@retval -1 for FAIL & 1 for SUCCESS.
*/
int 
tnGetPictureImageProps(TN_WIDGET *widget, TN_IMAGE_INFO *iinfo)
{
	
	 GR_IMAGE_INFO griinfo;
	 if(widget->type != TN_PICTURE || iinfo == NULL )
		 return -1;
	 if(widget->WSpec.picture.imageid == 0)
		 return -1;
	 GrGetImageInfo(widget->WSpec.picture.imageid, &griinfo);
	 iinfo->height = griinfo.height;
	 iinfo->width = griinfo.width;
	 iinfo->bpp = griinfo.bpp;
	 iinfo->bytesperpixel = griinfo.bytesperpixel;
	 return 1;
}
