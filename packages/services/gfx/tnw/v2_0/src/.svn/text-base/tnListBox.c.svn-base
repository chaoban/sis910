#include "tnWidgets.h"
/**Create ListBox widget
@param parent, the parent of the list box to be created
@param flags, the types of the list box including the visible, enable and transparent
@param rect, the rect of the list box to be created
@param listitem, the string array of the list items
@param listicon, the image id array of the icons of the list items
@param count, the item count, can be set to 0
@param imageid[], it's 4 dimensions, the first is the list background image, the second is the foreground image of the list items , the third is the background image of the scroll bar, the fourth is the thumb image of scroll bar
*/
TN_WIDGET *CreateListBox(TN_WIDGET * parent, int flags, GR_RECT *rect, char **listitem, GR_IMAGE_ID *listicon,
		int count, GR_IMAGE_ID imageid[], GR_WINDOW_ID wnd, int nDrawType)
{
	TN_WIDGET	*widget = NULL;
	int		i;	
	GR_RECT scrlRect;
	GR_IMAGE_ID scrlImg[2];
	GR_GC_INFO gcinfo;
    GR_FONT_INFO finfo;

	// create widget for listbox
	printf("CreateListBox 1\n");
	widget = CreateWidget(parent, rect, GR_EVENT_MASK_TIMER | GR_EVENT_MASK_KEY_DOWN 
	    | GR_EVENT_MASK_FOCUS_IN | GR_EVENT_MASK_FOCUS_OUT | GR_EVENT_MASK_EXPOSURE, flags, 0);
	if (widget == NULL) return NULL;

	// initialize widget.
	widget->type = TN_LISTBOX;
	widget->WSpec.listbox.list = NULL;
  	widget->WSpec.listbox.last = NULL;
  	widget->WSpec.listbox.showtop = NULL;
  	widget->WSpec.listbox.scrlbar = NULL;
    widget->WSpec.listbox.numitems = 0;
	widget->WSpec.listbox.iBack = imageid[0];
	widget->WSpec.listbox.iSelect = imageid[1];
	widget->WSpec.listbox.wBack = wnd;	

	if(imageid[2] == -1 ||  imageid[3] == -1)
		widget->WSpec.listbox.scrlbar= NULL;
	else
	{	    		
		scrlImg[0] = imageid[2];
		scrlImg[1] = imageid[3];
		scrlRect.y = rect->y;
		scrlRect.height = rect->height;
		scrlRect.x = rect->x + rect->width - 16;
		scrlRect.width = 16;
		widget->WSpec.listbox.scrlbar = CreateScrollBar(parent, flags, &scrlRect, TN_VERTICAL, 0, count-1, 
							TN_SCROLLBAR_LINESTEP, TN_SCROLLBAR_PAGESTEP, scrlImg);

	}
	
	GrGetGCInfo(widget->gc,&gcinfo);
	GrGetFontInfo(gcinfo.font,&finfo);

	if(nDrawType)
	{
	    widget->WSpec.listbox.nGapHeight = 4;
	    widget->WSpec.listbox.nType = 1;
	    widget->WSpec.listbox.nColumn = 4;
	  	widget->WSpec.listbox.nItemWidth = widget->WSpec.listbox.scrlbar ? (rect->width -10)/4: (rect->width)/4 ;
        widget->WSpec.listbox.nItemHeight = widget->WSpec.listbox.nItemWidth * 3 / 4;            
        widget->WSpec.listbox.rIcon.x = widget->WSpec.listbox.nItemWidth * 0.1;
        widget->WSpec.listbox.rIcon.y = widget->WSpec.listbox.nItemHeight * 0.1;
        widget->WSpec.listbox.rIcon.width = widget->WSpec.listbox.nItemWidth * 0.8;
  	    widget->WSpec.listbox.rIcon.height = widget->WSpec.listbox.nItemHeight * 0.8;  	    
  	    widget->WSpec.listbox.rString.x = widget->WSpec.listbox.nItemWidth * 0.3;
  	    widget->WSpec.listbox.rString.y = widget->WSpec.listbox.nItemHeight*0.9;
  	    widget->WSpec.listbox.nItemHeight += finfo.height;
  	    widget->WSpec.listbox.nShowItemsNum = (rect->height / widget->WSpec.listbox.nItemHeight) * 4;  	     	      	    
  	}
  	else
  	{
  	    widget->WSpec.listbox.nType = 0;
  	    widget->WSpec.listbox.nColumn = 1;
	  	widget->WSpec.listbox.nItemWidth = widget->WSpec.listbox.scrlbar ? (rect->width -10): (rect->width);
        widget->WSpec.listbox.nItemHeight = 32;    
        widget->WSpec.listbox.rIcon.x = 0;
        widget->WSpec.listbox.rIcon.y = 0;
        widget->WSpec.listbox.rIcon.width = 32;
  	    widget->WSpec.listbox.rIcon.height = 32;
  	    widget->WSpec.listbox.rString.x = 35;
  	    widget->WSpec.listbox.rString.y = finfo.height * 0.5;
  	    widget->WSpec.listbox.nShowItemsNum = rect->height / widget->WSpec.listbox.nItemHeight;  	    
  	    widget->WSpec.listbox.nGapHeight = 0;
  	}  	
	

	for(i=0;i<count;i++)
	{
	   tnAddItemToListBox(widget,listitem[i], listicon[i]);
		//p=NewListEntry();
		//p->string=(char *)malloc((strlen(listitem[i])+1)*sizeof(char));
		//if(p->string == NULL)	return NULL;
		//strcpy(p->string,listitem[i]);
		//p->icon = listicon[i];
		//printf("CreateList i = %d, %s\n",i,p->string);					
	    //
		//if(widget->WSpec.listbox.list==NULL)
		//{
		//	widget->WSpec.listbox.list=p;			
	  	//}
	  	//else
	  	//{
	   	//  	widget->WSpec.listbox.last->next=p;
		//  	p->prev = widget->WSpec.listbox.last;
		//  	
  	    //}
  	    //widget->WSpec.listbox.last=p;	  
  	}  
  	//widget->WSpec.listbox.numitems = count;
  	//widget->WSpec.listbox.nPages = (widget->WSpec.listbox.numitems -1) / widget->WSpec.listbox.nShowItemsNum + 1;
  	printf("CreateListBox 5\n");
  	widget->WSpec.listbox.nCurrentPage = 1;
  	widget->WSpec.listbox.showtop = widget->WSpec.listbox.list;

	return widget;	
}

void SelectNextItem(TN_WIDGET * widget, int nCount)
{
    struct listentry *p;
    int i = 0;
    int nFind = 0, nCurrent = 1;
    p = widget->WSpec.listbox.showtop;

    for(i = 1; i <= (widget->WSpec.listbox.nShowItemsNum + widget->WSpec.listbox.nColumn); i++)
    {
                            
        if(p->selected)
        {
            p->selected = GR_FALSE;
            if(nCount == 1)
            {
                nFind = 1;
                nCurrent = widget->WSpec.listbox.nColumn;
            }
            else
                nFind = nCurrent;            
        }                                   
        
        if(p->next)
        {
            p = p->next;
            if (nCurrent == widget->WSpec.listbox.nColumn)
                nCurrent = 1;
            else
                nCurrent ++; 
        }
        else
        {
            if(nFind > nCurrent)
                nFind = nCurrent;
            else
            {                
                p = widget->WSpec.listbox.list;
                widget->WSpec.listbox.showtop = p;
                nCurrent = 1;
            }
        }
        
        if( i == widget->WSpec.listbox.nShowItemsNum )
        {
            widget->WSpec.listbox.showtop = p;
        }
            
        if(nFind == nCurrent) break;
            
    }      
        
    p->selected = GR_TRUE;
    printf("SelectNextItem %s on page %d\n",p->string, widget->WSpec.listbox.nCurrentPage);
        

}

void SelectPrevItem(TN_WIDGET * widget, int nCount)
{
    int i = 0;
    int nFind = 0, nBackCount = 0;
    struct listentry *p, *q;
    int nStart = 0;
    
    p = widget->WSpec.listbox.showtop;
    
    for(i = 1; i <= widget->WSpec.listbox.nShowItemsNum; i++)
    {
        if(p->selected)
        {
            p->selected = GR_FALSE;
            nFind = i;
            break;
        }                                   
        
        if(p->next)
            p = p->next;
        else
        {
            nFind = i;
            break;
        }        
    }
    
    if(nFind > nCount)
    {
        nBackCount = nCount;
        nStart = 1;
    }
    else        
    {
        p = widget->WSpec.listbox.showtop;
        nBackCount = nCount - nFind + 1;
        int nBackPageItems = 0;
        
        if(p->prev)
        {
            nBackPageItems = widget->WSpec.listbox.nShowItemsNum;
            p = p->prev;
            nStart = 2;            
        }
        else
        {
            nBackPageItems = widget->WSpec.listbox.numitems%widget->WSpec.listbox.nShowItemsNum;
            if(!nBackPageItems)
                nBackPageItems = widget->WSpec.listbox.nShowItemsNum;

            p = widget->WSpec.listbox.last;
          
            nStart= widget->WSpec.listbox.nColumn  - (widget->WSpec.listbox.numitems % widget->WSpec.listbox.nColumn) + 2;
            
            if(nStart > widget->WSpec.listbox.nColumn + 1 ) nStart = 2;
            
        }
        
        q = p;
        
        for(i = 2; i<=nBackPageItems;i++) q = q->prev;        
        
        widget->WSpec.listbox.showtop = q;                    

    }
    
    for(i = nStart; i <= nBackCount; i++)    p = p->prev;    
    
    printf("SelectPrevItem back %s on page %d\n",p->string,  widget->WSpec.listbox.nCurrentPage);
    p->selected = GR_TRUE;

}
/**
Event Handler for the List Box 
@param evnet, the event structure
@param widget, the widget generate the event
*/
void
ListBoxEventHandler (GR_EVENT * event, TN_WIDGET * widget)
{
    GR_WINDOW_INFO winfo;
    int i,index;
    int currtop;
    TN_WIDGET	*scrlbar;
    scrlbar = widget->WSpec.listbox.scrlbar;
    GrGetWindowInfo(widget->wid, &winfo);
    
#ifdef WIDGET_KEY_DEBUG
    check_key_event(event, "ListBoxEventHandler");
#endif
    
    switch (event->type)
    {
        case GR_EVENT_TYPE_FOCUS_IN:
            printf("----------------List Box GR_EVENT_TYPE_FOCUS_IN-------------\n");
            if(!widget->visible || !widget->enabled)
    	        break; 
            //widget->WSpec.listbox.bFocus = GR_TRUE;
            printf("----------------List Box GR_EVENT_TYPE_FOCUS_IN  visible-------------\n");
            GrClearWindow(widget->wid, GR_FALSE);
            DrawListBox(widget);
            break;
            
        case GR_EVENT_TYPE_FOCUS_OUT:
            printf("----------------List Box GR_EVENT_TYPE_FOCUS_OUT-------------\n");
            if(!widget->visible || !widget->enabled)
    	        break; 
            //widget->WSpec.listbox.bFocus = GR_TRUE;
            
            GrClearWindow(widget->wid, GR_FALSE);
            DrawListBox(widget);
            break;
            
        case GR_EVENT_TYPE_KEY_DOWN:
             printf("DrawListBox: GR_EVENT_TYPE_KEY_DOWN\n");                                          
            if(!widget->visible || !widget->enabled)
    	        break; 
            switch (event->keystroke.ch) {
            	case MWKEY_ENTER:
            		if(widget->WSpec.listbox.list == NULL)
            			break;
            			
            		if (widget->CallBack[WIDGET_CLICKED].fp)
            			(*(widget->CallBack[WIDGET_CLICKED].fp))(widget, widget->CallBack[WIDGET_CLICKED].dptr);
            		break;
            	case MWKEY_ESCAPE:
            		if (widget->CallBack[WIDGET_ESCAPED].fp)	
            			(*(widget->CallBack[WIDGET_ESCAPED].fp))(widget, widget->CallBack[WIDGET_ESCAPED].dptr);
            		break;
            	default:
            	    if(widget->WSpec.listbox.list == NULL)
            	    	break;
            	    i = winfo.height / 32;
            	    currtop = tnGetListTop(widget);
            	    if(event->keystroke.ch == MWKEY_DOWN)
            	    {
            	        //SelectNextItem(widget, widget->WSpec.listbox.nColumn);
            	        SelectNextItem(widget, 1);
            	    }
            	    else if(event->keystroke.ch == MWKEY_UP){
            	        //SelectPrevItem(widget, widget->WSpec.listbox.nColumn);
            	        SelectPrevItem(widget, 1);
            	    }
            	    else if(event->keystroke.ch == MWKEY_RIGHT){
            	        SelectNextItem(widget, 1);
            	    }
            	    else if(event->keystroke.ch == MWKEY_LEFT){
            	        SelectPrevItem(widget, 1);
            	    }       
            	    
            	    printf("before DrawListBox: key board\n");
            	    
            	    if(scrlbar)
            	    {   
            	        index= tnGetListItemPos(widget, widget->WSpec.listbox.showtop->string);
            	        widget->WSpec.listbox.nCurrentPage = index / widget->WSpec.listbox.nShowItemsNum + 1;
            	        printf("Current Page = %d :%d\n",widget->WSpec.listbox.nPages, widget->WSpec.listbox.nCurrentPage);            	                        	        
                        tnSetThumbPosition(scrlbar, widget->WSpec.listbox.nCurrentPage);                        
                    }
                    
                    DrawListBox(widget);
                    printf("DrawListBox end\n");
                        
            	    //if (widget->CallBack[WIDGET_KEYDOWN].fp)
            	    //{
            	    //    widget->data = (void *)((long)(event->keystroke.ch));
            	    //    (*(widget->CallBack[WIDGET_KEYDOWN].fp))(widget, widget->CallBack[WIDGET_KEYDOWN].dptr);
            	    //}
            	    break;
            }
            break;
       case GR_EVENT_TYPE_EXPOSURE:
            if(widget->visible)
            {
                printf("before DrawListBox: Type exposure\n");
          		DrawListBox (widget);
          	}
            break;
       case GR_EVENT_TYPE_TIMER:
            if(!widget->enabled)
    		    break; 
    	    if (widget->CallBack[WIDGET_TIMER].fp) 
    		    widget->CallBack[WIDGET_TIMER].fp(widget, widget->CallBack[WIDGET_TIMER].dptr);
    	    break;
    
        }
       return;	
}

/** ListBox draw routine
@param listbox, the List Box to DRAW!
*/
#if 1
void
DrawListBox (TN_WIDGET * listbox)
{
    GR_WINDOW_INFO winfo, swinfo;
    GR_WINDOW_ID pid;
    int op, i,nColumnIndex  = 0;
    GR_COORD x = 0,y = 0;
    struct listentry *p;
    TN_STRUCT_LISTBOX *pListBoxData;
    MWIMAGEINFO imageinfo;
    
    if(!listbox->visible)
       return;
 
    pListBoxData = &(listbox->WSpec.listbox);
    
    GrGetWindowInfo (listbox->wid, &winfo);

    if(pListBoxData->scrlbar != NULL)
    {
        tnSetThumbPosition(pListBoxData->scrlbar, pListBoxData->nCurrentPage);        
    }
    else
    {        
    	//nitems = pListBoxData->numitems;	
        GrResizeWindow(listbox->wid,winfo.width, pListBoxData->numitems*(pListBoxData->nItemHeight));
    }

    GrGetWindowInfo (listbox->wid, &winfo);
    if(pListBoxData->scrlbar){
        GrGetWindowInfo(pListBoxData->scrlbar->wid, &swinfo);
        GrResizeWindow(pListBoxData->scrlbar->wid, swinfo.width, winfo.height);
        GrClearWindow(pListBoxData->scrlbar->wid,GR_FALSE);
    }
     
    if(!listbox->enabled)
    {
        GrSetGCForeground(listbox->gc, GR_RGB(200,200,200));
        GrFillRect(listbox->wid,listbox->gc,0,0,winfo.width,winfo.height);
    }
 
    pid =GrNewPixmap(winfo.width, winfo.height, NULL);	  
      	
    if (listbox->nFlags & GR_WM_PROPS_TRANSPARENT)
        op = MWROP_BLENDCONSTANT | 150;
    else
        op = MWROP_COPY;
    
    if(pListBoxData->iBack)
        GrDrawImageToFit(pid, listbox->gc, 0, 0, winfo.width , winfo.height, pListBoxData->iBack);
    else
        GrCopyArea(pid, listbox->gc, 0, 0, winfo.width, winfo.height, pListBoxData->wBack, 0, 0, MWROP_COPY);
      
    for(i=0,p=pListBoxData->showtop; p && i<pListBoxData->nShowItemsNum; i++, p=p->next)
    {   
        if(p == NULL)
            break;     
        if(p->selected)
        {
            GrDrawImageToFit(pid, listbox->gc, x, y, 
    	    pListBoxData->nItemWidth, pListBoxData->nItemHeight, pListBoxData->iSelect);
        }
        //Draw item's icon
        if(p->icon != -1)
        {            
	        GrGetImageInfo(p->icon, &imageinfo);
	        //diag_mallocinfo();
            GrDrawImageToFit(pid, listbox->gc, x + pListBoxData->rIcon.x + (pListBoxData->rIcon.width - imageinfo.width)* 0.5,
                                               y + pListBoxData->rIcon.y + (pListBoxData->rIcon.height - imageinfo.height)* 0.5,
                                             imageinfo.width,  imageinfo.height, p->icon);
        }
    
        GrSetGCForeground(listbox->gc, 0);
        {                        
            if(pListBoxData->nType)
            {
                int		captionx;
	            int		captiony;
	            int		base;
                GrGetGCTextSize(listbox->gc, p->string, -1, GR_TFASCII, &captionx, &captiony, &base);
		        captionx = (pListBoxData->nItemWidth - captionx) / 2;
		        if (captionx < 0) captionx = 0;
                tnDrawText(pid, listbox->gc, x + captionx, y+pListBoxData->rString.y, p->string, -1, GR_TFASCII | GR_TFTOP);
            }
            else
                tnDrawText(pid, listbox->gc, x+pListBoxData->rString.x, y+pListBoxData->rString.y, p->string, -1, GR_TFASCII | GR_TFTOP);
        }
        
        nColumnIndex ++;
        
        if( nColumnIndex == pListBoxData->nColumn)
        {
            nColumnIndex = 0;
            x = 0;
            y += pListBoxData->nItemHeight;
            y += pListBoxData->nGapHeight;
        }
        else
            x += pListBoxData->nItemWidth;
    
    }
    
    GrCopyArea(listbox->wid, listbox->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
    GrDestroyWindow(pid);	
}
#else
void
DrawListBox (TN_WIDGET * listbox)
{
	  GR_WINDOW_INFO winfo, swinfo;
	  GR_GC_INFO gcinfo;
	  GR_FONT_INFO finfo;
	  GR_GC_ID	gc;
	  GR_WINDOW_ID	pid, itempid;
	  int op, i, nitems, currtop;
	  GR_COORD x,y;
	  struct listentry *p;
	  TN_WIDGET	*scrlbar;
	  
	  
	  if(!listbox->visible)
	     return;
  
	  GrGetWindowInfo (listbox->wid, &winfo);
	  GrGetGCInfo(listbox->gc,&gcinfo);
	  GrGetFontInfo(gcinfo.font,&finfo);

	  if(listbox->WSpec.listbox.scrlbar != NULL){
  		nitems = winfo.height/32;
	  	scrlbar = listbox->WSpec.listbox.scrlbar;
		tnSetScrollRange(scrlbar, 0, listbox->WSpec.listbox.numitems-1);
	  }
	  else{
  		nitems = listbox->WSpec.listbox.numitems;	
		GrResizeWindow(listbox->wid,winfo.width, nitems*32);
  	  }
   
	  GrGetWindowInfo (listbox->wid, &winfo);
	  if(scrlbar){
		GrGetWindowInfo(scrlbar->wid, &swinfo);
		GrResizeWindow(scrlbar->wid, swinfo.width, winfo.height);
    	  }
	  
	  if(!listbox->enabled)
 	  {
		GrSetGCForeground(listbox->gc, GR_RGB(200,200,200));
		GrFillRect(pid,listbox->gc,0,0,winfo.width,winfo.height);
    	  }
	  
	  pid =GrNewPixmap(winfo.width, winfo.height, NULL);	  
    	
	  if (listbox->nFlags & GR_WM_PROPS_TRANSPARENT)
		op = MWROP_BLENDCONSTANT | 150;
  	  else
		op = MWROP_COPY;

	  if(listbox->WSpec.listbox.iBack)
	      GrDrawImageToFit(pid, listbox->gc, 0, 0, winfo.width , winfo.height, listbox->WSpec.listbox.iBack);
	  else
	      GrCopyArea(pid, listbox->gc, 0, 0, winfo.width, winfo.height, listbox->WSpec.listbox.wBack, 0, 0, MWROP_COPY);

  	 /* for (p = listbox->WSpec.listbox.showtop,i=0; p && !p->selected; p = p->next,i++);
		
	  if(i+1>nitems && p->selected){
	
		  currtop = tnGetListTop(listbox);
  		  i= tnGetListItemPos(listbox, p->string);

  	  	  if(i >= currtop + nitems){
			tnListItemsLineUp(listbox, listbox->WSpec.listbox.numitems - (i - currtop));
  	  	  }
	  	  if(i < currtop){
			tnListItemsLineDown(listbox, currtop - i);
	  	  }
	  }*/
		  
	  for(i=0,x=0,y=0,p=listbox->WSpec.listbox.currtop; p && i<nitems; i++, p=p->next, y+=32)
	  { 
		  if(p->selected)
		  {
		      GrDrawImageToFit(pid, listbox->gc, 0, y, 
				listbox->WSpec.listbox.scrlbar?winfo.width-10:winfo.width, 32, listbox->WSpec.listbox.iSelect);
		  }
		 //Draw item's icon
		  if(p->icon != -1)
			  GrDrawImageToFit(pid, listbox->gc, 0, y, 32, 32, p->icon);
	  
		  GrSetGCForeground(listbox->gc, 0);
		  tnDrawText(pid, listbox->gc, 35, y+(32-finfo.height)/2, p->string, -1, GR_TFASCII | GR_TFTOP);

	  }

	  GrCopyArea(listbox->wid, listbox->gc, 0, 0, winfo.width, winfo.height, pid, 0, 0, op);
	  GrDestroyWindow(pid);	
}
#endif
/** Create a new struct listentry
@retval the struct listentry
*/
struct listentry * NewListEntry(void)
{
	struct listentry *temp=(struct listentry *)malloc(sizeof(struct listentry));
	temp->selected=GR_FALSE;
	temp->next=NULL;	
	temp->prev = NULL;
	
	return temp;
}

/** Get the selected list box items' strings and slected count.
@param widget, the ListBox to get the items' property;
@param listinfo, the strings array of the selected items of the ListBox.
@param count, the numberof the selected items of the ListBox.
@retval -1 for FAIL & 1 for SUCCESS
*/
int
tnGetSelectedListItems(TN_WIDGET *widget, char ***listinfo, int *count)
{
	struct listentry *p;
	
	int i;

	if(widget->type != TN_LISTBOX)
		return -1;
	
	*count=tnGetSelectedListNum(widget);
	*listinfo=(char **)malloc((*count) * sizeof(char *));
	
	for(p=widget->WSpec.listbox.list, i=0 ;p;p=p->next)
		if(p->selected)
		{
			//(*listinfo)[i]=(char *)malloc((strlen(p->string) + 1)*sizeof (char));
			//strcpy((*listinfo)[i],p->string);
			(*listinfo)[i] = (char *)strdup(p->string);
			
			i++;
		}
	return 1;
}

/** Get all of list box items' property.
@param widget, the ListBox to get the items' property;
@param listinfo, the strings array of all of the items of the ListBox.
@param count, the number of the items of the ListBox.
@retval -1 for FAIL & 1 for SUCCESS
*/
int
tnGetAllListItems(TN_WIDGET *widget, char ***listinfo, int *count)
{
	struct listentry *p;
	
	int i;
	*count=0;

	if(widget->type != TN_LISTBOX)
		return -1;
		
	*listinfo=(char **)malloc(widget->WSpec.listbox.numitems*sizeof(char *));
	for(p=widget->WSpec.listbox.list, i=0 ;p;p=p->next)
		{
			(*listinfo)[i]=(char *)malloc((strlen(p->string) + 1)*sizeof (char));
			strcpy((*listinfo)[i],p->string);
			i++;
			(*count)++;
		}
	return 1;
}

/** Get the selected list box items' property
@param widget, the ListBox to get the items' property;
@retval the number of the selected items
*/
int tnGetSelectedListNum(TN_WIDGET *widget)
{
	struct listentry *p;
        int count;
	if(widget->type != TN_LISTBOX)
		return -1;
	for(p=widget->WSpec.listbox.list , count=0 ;p;p=p->next)
   		if(p->selected)
			count ++;
	return count;
}

/** Get the selected list box items' position and selected count
@param widget, the ListBox to get the items' property;
@param pos, the postion array of the selected items of the ListBox.
@param count, the numberof the selected items of the ListBox.
@retval -1 for FAIL & 1 for SUCCESS
*/	
int
tnGetSelectedListPos(TN_WIDGET *widget,int **pos,int *count)
{
	struct listentry *p;
	int i,j;
	if(widget->type != TN_LISTBOX)
		return -1;
	 *count=tnGetSelectedListNum(widget);
	 (*pos)=(int *)malloc((*count) * sizeof(int));
	 
	 for(p=widget->WSpec.listbox.list, i=0,j=0 ;p;p=p->next,j++)
		 if(p->selected)
			 (*pos)[i++]=j;
	 
	 return 1;
}	 

/** Get a list item's position.
@param widget, the ListBox to get the list items' postion;
@param s, the string of the list item.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnGetListItemPos(TN_WIDGET *widget,char *s)
{
	struct listentry *p;
	int i;
	if(widget->type != TN_LISTBOX)
		return -1;
	for(p=widget->WSpec.listbox.list,i=0;p;p=p->next,i++)
		if(!strcmp(p->string,s))
			return i;
	return -1;
}

/** Add a item to a List Box.
@param listbox, the ListBox to get the items' property;
@param str, the string of the list item to add to the list box.
@param icon, the icon image to add.
@retval -1 for FAIL & 1 for SUCCESS
*/	
int tnAddItemToListBox(TN_WIDGET *listbox,char *str, GR_IMAGE_ID icon)
{
	struct listentry *newentry;
	if(listbox->type!=TN_LISTBOX)
		return -1;
	if(str==NULL)
		return -1;
	newentry = NewListEntry();
	newentry->string = (char *)strdup(str);

	if(icon)
	{
	    MWIMAGEINFO imageinfo;
	    GrGetImageInfo(icon, &imageinfo);
	    if((imageinfo.width > listbox->WSpec.listbox.rIcon.width)||
	        (imageinfo.height > listbox->WSpec.listbox.rIcon.height))
	    {
	        if(imageinfo.width > imageinfo.height)
	            GdStretchImageToFit(icon,listbox->WSpec.listbox.rIcon.width,listbox->WSpec.listbox.rIcon.width *imageinfo.height/imageinfo.width);
	        else
	            GdStretchImageToFit(icon,listbox->WSpec.listbox.rIcon.height*imageinfo.width/imageinfo.height,listbox->WSpec.listbox.rIcon.height);
	    }
	    
		newentry->icon = icon;
	}
	else
	    newentry->icon = -1;
	    
	if(listbox->WSpec.listbox.last)
	{
		listbox->WSpec.listbox.last->next = newentry;
		newentry->prev = listbox->WSpec.listbox.last;
	}
	else
	{
		listbox->WSpec.listbox.list = newentry;
		listbox->WSpec.listbox.showtop = newentry;
	}
	listbox->WSpec.listbox.last = newentry;
	(listbox->WSpec.listbox.numitems)++;
	listbox->WSpec.listbox.nPages = (listbox->WSpec.listbox.numitems -1) / listbox->WSpec.listbox.nShowItemsNum + 1;
	
	if(listbox->WSpec.listbox.scrlbar != NULL)
	{
	    tnSetScrollRange(listbox->WSpec.listbox.scrlbar, 1, listbox->WSpec.listbox.nPages);
	    tnSetVisible(listbox->WSpec.listbox.scrlbar, (listbox->WSpec.listbox.nPages == 1) ? GR_FALSE: GR_TRUE);	            
    }
            	            
	GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}

/** Add a item to a specified position of the List Box.
@param listbox, the ListBox to get the items' property;
@param str, the string of the list item to add to the list box.
@param n, the position of the item to add at.
@param icon, the icon image to add.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnAddItemToListBoxAt(TN_WIDGET *listbox,char *str,int n, GR_IMAGE_ID icon)
{
	struct listentry *newentry,*p;
	int i = 0;
        if(listbox->type!=TN_LISTBOX)
                return -1;
	if(str==NULL)
                return -1;
	if(n < 0)
		return -1;
      	newentry = NewListEntry();
	newentry->string = (char *)malloc((strlen(str)+1)*sizeof(char));
      	strcpy(newentry->string,str);
	if(icon)
		newentry->icon = icon;	
	p = listbox->WSpec.listbox.list;

	if(p == NULL){
		listbox->WSpec.listbox.list =  newentry;
		listbox->WSpec.listbox.last =  newentry;
		listbox->WSpec.listbox.showtop =  newentry;
		newentry->next = NULL;
		newentry->prev = NULL;
		(listbox->WSpec.listbox.numitems)++;
		listbox->WSpec.listbox.nPages = (listbox->WSpec.listbox.numitems -1) / listbox->WSpec.listbox.nShowItemsNum + 1;
		
        if(listbox->WSpec.listbox.scrlbar != NULL)
	    {
	        tnSetScrollRange(listbox->WSpec.listbox.scrlbar, 1, listbox->WSpec.listbox.nPages);
            tnSetVisible(listbox->WSpec.listbox.scrlbar, (listbox->WSpec.listbox.nPages == 1) ? GR_FALSE: GR_TRUE);
        }
		
		GrClearWindow(listbox->wid,GR_TRUE);

		return 1;
	}
	
	while(p && i < n)
	{
		p = p->next;
		i++;
	}	
	if(i != n)
		printf("WARNING: Ecceed the max position! The item will be place at the bottom!\n");
		
	if(p)
	{
		newentry->next = p;
		newentry->prev = p->prev ? p->prev : NULL;
		p->prev = newentry;
		if(newentry->prev)
			newentry->prev->next = newentry;
		else
			listbox->WSpec.listbox.list = newentry;
	}		
	else{
		p = listbox->WSpec.listbox.list;
	    	while(p->next){
			p = p->next;
		}
		p->next = newentry;
		newentry->prev = p;
	    	listbox->WSpec.listbox.last = newentry;
	}

	if(n == 0)
		listbox->WSpec.listbox.showtop = newentry;
	
	(listbox->WSpec.listbox.numitems)++;
	listbox->WSpec.listbox.nPages = (listbox->WSpec.listbox.numitems -1) / listbox->WSpec.listbox.nShowItemsNum + 1;
	
    if(listbox->WSpec.listbox.scrlbar != NULL)
	{
	    tnSetScrollRange(listbox->WSpec.listbox.scrlbar, 1, listbox->WSpec.listbox.nPages);
        tnSetVisible(listbox->WSpec.listbox.scrlbar, (listbox->WSpec.listbox.nPages == 1) ? GR_FALSE: GR_TRUE);
    }
            	            
	GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}

/** Exchange TWO items' positions in the List Box.
@param listbox, the ListBox to get the items' property;
@param n1, the position of the first item to exchange.
@param n2, the position of the second item to exchange.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnExchangeItems(TN_WIDGET *listbox, int n1, int n2)
{
	struct listentry *p, *q;
	GR_IMAGE_ID	picon;
	GR_WINDOW_INFO	winfo;
	int i,tmp;
	char *s;
	
	if(listbox->type!=TN_LISTBOX)
                return -1;
	if(n1 < 0 || n2 < 0 || n1 > listbox->WSpec.listbox.numitems
		|| n2 > listbox->WSpec.listbox.numitems){
		printf("Bad items number!\n");
		return -1;
	}
	if(n1 == n2)	// No need to exchange
		return 1;

	GrGetWindowInfo(listbox->wid, &winfo);
			
	if(n1 > n2){
		tmp = n1;
		n1 = n2;
		n2 = tmp;
	}
	
	for(p=listbox->WSpec.listbox.list,i=0; p && i<n1 ; p=p->next,i++);
	s = (char*)malloc((strlen(p->string)+1)*sizeof(char));
	strcpy(s, p->string);	
	free(p->string);
	if(p->icon)
		picon = p->icon;
	
	for(q=listbox->WSpec.listbox.list,i=0; q && i<n2 ; q=q->next,i++);
	p->string = (char*)malloc((strlen(q->string)+1)*sizeof(char));
	strcpy(p->string, q->string);
	free(q->string);
	if(q->icon)
		p->icon = q->icon;
	if(picon)
		q->icon = picon;
	q->string = (char*)malloc((strlen(s)+1)*sizeof(char));
	strcpy(q->string, s);

	if(p->selected != q->selected){
		p->selected = !p->selected;
		q->selected = !q->selected;
	}
	
	if(q == listbox->WSpec.listbox.showtop){
		listbox->WSpec.listbox.showtop = p;
	}
	else{
		tmp = winfo.height / 32; //calculate the num of actually view items
		for(q=listbox->WSpec.listbox.showtop,i=0; q && q != p ; q=q->next,i++);
		if(i + 1 == tmp){	// p is the bottom item in a list view
			q = listbox->WSpec.listbox.showtop;
			listbox->WSpec.listbox.showtop = q->next;
		}
	}
	
	GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}

/** Delete a list entry from the List Box.
@param listbox, the ListBox to get the items' property;
@param p, the list entry to delete.
*/
void DeletePtr(TN_WIDGET *listbox,struct listentry *p)
{
	if(p->prev)
		(p->prev)->next = p->next;
	else
		listbox->WSpec.listbox.list = p->next;
	if(p->next)
		(p->next)->prev = p->prev;
	else
		listbox->WSpec.listbox.last = p->prev;
	if(listbox->WSpec.listbox.showtop == p)
	{
		if(p->prev)
			listbox->WSpec.listbox.showtop = p->prev;
		else
			listbox->WSpec.listbox.showtop = p->next;
	}
	p->prev = NULL;
	p->next = NULL;
	if(p->string)
		free(p->string);
        free(p);
	(listbox->WSpec.listbox.numitems)--;
	listbox->WSpec.listbox.nPages = (listbox->WSpec.listbox.numitems -1) / listbox->WSpec.listbox.nShowItemsNum + 1;
	
	if(listbox->WSpec.listbox.scrlbar != NULL)
	{
        tnSetVisible(listbox->WSpec.listbox.scrlbar, (listbox->WSpec.listbox.nPages == 1) ? GR_FALSE: GR_TRUE);
    }
    
	return;
}

/** Delete a specified item from the List Box.
@param listbox, the ListBox to delete the item;
@param str, the string of the list item to delete from the list box.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnDeleteItemFromListBox(TN_WIDGET *listbox,char *str)
{
	struct listentry *p;
        if(listbox->type!=TN_LISTBOX)
                return -1;
        if(str==NULL)
                return -1;
	p = listbox->WSpec.listbox.list;
        while(p && strcmp(p->string,str)!=0)
                p = p->next;
	if(!p)
		return -1;
	DeletePtr(listbox,p);	
	GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}

/** Delete a item from a specified position of the List Box.
@param listbox, the ListBox to delete the item from;
@param n, the position of the item to delete from.
@retval -1 for FAIL & 1 for SUCCESS
*/	
int tnDeleteItemFromListBoxAt(TN_WIDGET *listbox,int n)
{
	struct listentry *p;
    int i;
    if(listbox->type!=TN_LISTBOX)
        return -1;
    if(n < 0)
        return -1;
    
    for(p = listbox->WSpec.listbox.list,i=0;p && i < n; p=p->next,i++);
	    if(!p)
		    return -1;
	DeletePtr(listbox,p);
		 
    GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}

/** Set a item to a specified status.
@param listbox, the ListBox to set the items' status;
@param n, the position of the item to set at.
@param selected, whether the item is selected.
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnSetSelectedListItemAt(TN_WIDGET *listbox,int n,GR_BOOL selected)
{
	struct listentry *p;
	int i;
	if(listbox->type!=TN_LISTBOX)
		return -1;
	if(n < 0)
		return -1;
	for(p = listbox->WSpec.listbox.list,i=0;p && i < n; p=p->next,i++);
	    if(!p)
		    return -1;
		
	p->selected = selected;
	GrClearWindow(listbox->wid,GR_TRUE);	
	return 1;
}

/** Delete all selected items from the List Box.
@param listbox, the ListBox to delete the items;
@retval -1 for FAIL & 1 for SUCCESS
*/		
int tnDeleteSelectedItems(TN_WIDGET *listbox)
{
	struct listentry *p,*temp;
        if(listbox->type!=TN_LISTBOX)
                return -1;
	for(p = listbox->WSpec.listbox.list;p ;)
	{	temp = p->next;
		if(p->selected)
		   DeletePtr(listbox,p);
		p = temp;
	}
	GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}

/** Delete all items from the List Box.
@param listbox, the ListBox to delete the items;
@retval -1 for FAIL & 1 for SUCCESS
*/	
int tnDeleteAllItemsFromListBox(TN_WIDGET *listbox)
{
    struct listentry *p,*temp;
    if(listbox->type!=TN_LISTBOX)
        return -1;
    
    for(p = listbox->WSpec.listbox.list;p ;)
    {
        temp = p->next;
		p->next = NULL;
		p->prev = NULL;
		if(p->string)
		{		    
			free(p->string);
		}
		free(p);
		p=temp;
    }
	listbox->WSpec.listbox.list = NULL;
	listbox->WSpec.listbox.last = NULL;
	listbox->WSpec.listbox.showtop = NULL;
	listbox->WSpec.listbox.numitems = 0;
	listbox->WSpec.listbox.nCurrentPage = 1;
    GrClearWindow(listbox->wid,GR_TRUE);
    return 1;
}

/** Set all items from the List Box.
@param listbox, the ListBox to set the item's property;
@param str, the item's string to set the property.
@param selected, whether the item is set to be selected
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnSetSelectedListItem(TN_WIDGET *listbox,char *str,GR_BOOL selected)
{
	struct listentry *p;
	if(listbox->type != TN_LISTBOX)
		return -1;
	if(str == NULL)
		return -1;
	p = listbox->WSpec.listbox.list;
	while(p && strcmp(p->string,str)!=0)
		p=p->next;
	if(!p)
		return -1;

	p->selected = selected;
	GrClearWindow(listbox->wid,GR_TRUE);
	return 1;
}


/** Get the num of the current item from the top item of the List Box.
@param listbox, the ListBox to get the num;
@retval -1 for FAIL & 1 for SUCCESS
*/
int tnGetListTop(TN_WIDGET *listbox)
{
	struct listentry *p;
	int i;
	if(listbox->type != TN_LISTBOX)
		return -1;
	for(p = (listbox->WSpec.listbox.showtop)->prev,i=0;p;i++,p=p->prev);
	return i;
}
/*No Resize Now
int tnGetListBoxResize(TN_WIDGET *listbox,GR_BOOL *resize)
{
	if(listbox->type != TN_LISTBOX)
		return -1;
	*resize = listbox->WSpec.listbox.resize;
	return 1;
}
*/
/*No Resize Now
int tnSetListBoxResize(TN_WIDGET *listbox, GR_BOOL resize)
{
	if(listbox->type != TN_LISTBOX)
		return -1;
	listbox->WSpec.listbox.resize = resize;
	return 1;
}
*/				
		
/** List Box destroy routine 
@param widget, the List Box to DESTROY!
*/
void DestroyListBox(TN_WIDGET *widget)
{
	struct listentry *temp=widget->WSpec.listbox.list;
	struct listentry *prev=temp;
	while(temp!=NULL)
	{
		prev=temp;
		temp=temp->next;
		if(prev->string)
			free(prev->string);
		if(prev->icon)
			GrFreeImage(prev->icon);
		free(prev);
		prev=NULL;
	}

	if (widget->WSpec.listbox.iSelect)
		GrFreeImage(widget->WSpec.listbox.iSelect);
	if (widget->WSpec.listbox.iBack)
		GrFreeImage(widget->WSpec.listbox.iBack);
	
	if (widget->WSpec.listbox.wBack)
	    GrDestroyWindow(widget->WSpec.listbox.wBack);
	
	if( widget->WSpec.listbox.imageid[0])
	    GrDestroyWindow(widget->WSpec.listbox.imageid[0]);
	if( widget->WSpec.listbox.imageid[1])
	    GrDestroyWindow(widget->WSpec.listbox.imageid[1]);      	
	
	printf("DestroyListBox \n");
	DeleteFromRegistry(widget);
	return;
}



