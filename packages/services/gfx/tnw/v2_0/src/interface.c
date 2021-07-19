/**
 * @file interface.c, by solarhe at 2008/03/18
 */
#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#endif // __ECOS
#include "tnWidgets.h"

// Functions not provided in eCos by standard...
char * strdup(char *src)
{
    char *dst;

    if ((dst = (char *)malloc(strlen(src) + 1)) == NULL)
	    return(NULL);    
    strcpy(dst, src);
    
    return(dst);
}

/** Switch the resource file for new subject.
@param rc_info, the resource info pointer 
@param fname, the new resource file name
@param lang, the language setting("en" for English, eg.)
@retval -1 for FAIL & 0 for SUCCESS
*/
int 
switch_subject(PRCINFO rc_info, char *fname, char *lang)
{
	int 		i;
	TN_WIDGET	*pWidget;
#ifdef __ECOS
	cyg_handle_t	handle;
#endif //__ECOS

	// first clear rc_info->
	destroy_rc(rc_info);

	// load the new rc file.
	if (load_rc(rc_info, fname, lang) < 0)
		return -1;

	// make all active window to update 
	for (i = 0; i < HASH_MAX_QUEUES; i++) {
#ifdef __ECOS
		handle = cyg_thread_self();
#endif // __ECOS
		for (pWidget = Registry[i]; pWidget != NULL; pWidget = pWidget->next) {
#ifdef __ECOS
			if (handle == pWidget->handle)
#endif // __ECOS
				UpdateFront(pWidget, rc_info);		
		}
	}

	return 0;
}

/** Switch the language under the same resource file.
@param rc_info, the resource info pointer 
@param fname, the resource file name
@param lang, the language setting("en" for English, eg.)
@retval -1 for FAIL & 0 for SUCCESS
*/
int 
switch_lang(PRCINFO rc_info, char *fname, char *lang)
{
	int 		i;
	TN_WIDGET	*pWidget;
#ifdef __ECOS
	cyg_handle_t	handle;
#endif //__ECOS

	// first clear rc_info->
	destroy_rc(rc_info);

	// load the new rc file.
	if (load_rc(rc_info, fname, lang) < 0)
		return -1;

	// make all active window to update 
	for (i = 0; i < HASH_MAX_QUEUES; i++) {
#ifdef __ECOS
		handle = cyg_thread_self();
#endif // __ECOS
		for (pWidget = Registry[i]; pWidget != NULL; pWidget = pWidget->next) {
#ifdef __ECOS
			if (handle == pWidget->handle)
#endif // __ECOS
				UpdateFront(pWidget, rc_info);		
		}
	}

	return 0;
}

/** 
Load a image from resource
@param rcinfo, the resource info pointer
@param id, the image id in resource
@retval GR_IMAGE_ID
*/
GR_IMAGE_ID 
LoadImageFromRC(PRCINFO rcinfo, int id, HW_RECT *rect)
{
	char 	tmp[255];
	char	fnm[255];
	GR_IMAGE_ID imageid;
	GR_IMAGE_INFO iinfo;
    
	if(id < 0) return -1;
	    
	get_rc_info(rcinfo, RC_IMG, id, &tmp);
	strcpy(fnm, rcinfo->path);
	strcat(fnm, tmp);

    imageid =  GrLoadImageFromFile(fnm, 0);
    //return GrLoadImageFromFile(fnm, 0);
    
    GrGetImageInfo(imageid, &iinfo);
    
    if(( rect != NULL) && ((iinfo.width * iinfo.height) > (rect->width * rect->height)))
    {
        GdStretchImageToFit( imageid, rect->width, rect->height);        
    }
    
	return imageid;	
}

/** 
Create push button widget from resource
@param parent, the parent of the button to be created
@param flags, the types of the button including the visible, enable and transparent
@param id, the id of the button resource to be created
@param rcinfo, the resource info pointer
@retval  TN_BUTTON
*/
TN_WIDGET*
CreateButtonFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo)
{
	RCBTN		rcbtn;
	char		tmp[32];
	GR_IMAGE_ID	img[2];
	TN_WIDGET	*pWidget;

	get_rc_info(rcinfo, RC_BTN, id, &rcbtn);
	get_rc_info(rcinfo, RC_STR, rcbtn.cap, tmp);
	printf("CreateButtonFromRC cap %s\n",tmp);
//diag_mallocinfo();	
	img[0] = LoadImageFromRC(rcinfo, rcbtn.img[0], &(rcbtn.rect));
//diag_mallocinfo();	
	img[1] = LoadImageFromRC(rcinfo, rcbtn.img[1], &(rcbtn.rect));
//diag_mallocinfo();	
	pWidget = CreateButton(parent, flags, &(rcbtn.rect), tmp, img, rcbtn.keymap);	
	if (pWidget == NULL) return NULL;

	pWidget->nResourceID = id;
	return pWidget;
}

/** 
Create check button widget from resource
@param parent, the parent of the Check Button to be created
@param flags, the types of the CheckButton including the visible, enable and transparent
@param id, the id of the CheckButton resource to be created
@param rcinfo, the resource info pointer
@retval  TN_CHECKBUTTON
*/
TN_WIDGET*
CreateCheckButtonFromRC(TN_WIDGET * parent, int flags, int id, PRCINFO rcinfo)
{
	RCCKB  		rcckb;
	TN_WIDGET	*widget;
	char 		tmp[32];
	GR_IMAGE_ID	img[4];

	get_rc_info(rcinfo, RC_CKB, id, &rcckb);
	get_rc_info(rcinfo, RC_STR, rcckb.cap, tmp);
	img[0] = LoadImageFromRC(rcinfo, rcckb.img[0], &(rcckb.rect));
	img[1] = LoadImageFromRC(rcinfo, rcckb.img[1], &(rcckb.rect));
	img[2] = LoadImageFromRC(rcinfo, rcckb.img[2], &(rcckb.rect));
	img[3] = LoadImageFromRC(rcinfo, rcckb.img[3], &(rcckb.rect));

	widget = CreateCheckButton(parent, flags, &(rcckb.rect), tmp, img, rcckb.keymap);

	if(widget == NULL) return NULL;
	widget->nResourceID = id;
	
	return widget;
}

/** 
Create radio button widget from resource
@param parent, the parent of the radio button to be created
@param flags, the types of the radio button including the visible, enable and transparent
@param id, the id of the radio button resource to be created
@param rcinfo, the resource info pointer
@retval  TN_RADIOBUTTON
*/
TN_WIDGET *
CreateRadioButtonFromRC (TN_WIDGET * parent, int flags, int id, PRDBGROUP group, PRCINFO rcinfo)
{
	RCRDB  		rcrdb;
	TN_WIDGET	*widget;
	char 		tmp[32];
	GR_IMAGE_ID	img[4];
	
	get_rc_info(rcinfo, RC_RDB, id, &rcrdb);
	get_rc_info(rcinfo, RC_STR, rcrdb.cap, tmp);
	img[0] = LoadImageFromRC(rcinfo, rcrdb.img[0], &(rcrdb.rect));
	img[1] = LoadImageFromRC(rcinfo, rcrdb.img[1], &(rcrdb.rect));
	img[2] = LoadImageFromRC(rcinfo, rcrdb.img[2], &(rcrdb.rect));
	img[3] = LoadImageFromRC(rcinfo, rcrdb.img[3], &(rcrdb.rect));

	widget = CreateRadioButton(parent, flags, &(rcrdb.rect), tmp, img, group, rcrdb.keymap);
	if(widget == NULL) return NULL;
	widget->nResourceID = id;
	
	return widget;
}

/** 
Create label widget from resource
@param parent, the parent of the label to be created
@param flags, the types of the label including the visible, enable and transparent
@param id, the id of the label resource to be created
@param rcinfo, the resource info pointer
@retval  TN_LABEL
*/
TN_WIDGET*
CreateLabelFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo)
{
	RCLBL		rclbl;
	char		tmp[32];
	TN_WIDGET	*pWidget;
	GR_IMAGE_ID	img = 0;
	GR_WINDOW_ID wBack = 0;
	
	get_rc_info(rcinfo, RC_LBL, id, &rclbl);
	get_rc_info(rcinfo, RC_STR, rclbl.cap, tmp);
//diag_mallocinfo();	
	img = LoadImageFromRC(rcinfo, rclbl.img, &(rclbl.rect));
//diag_mallocinfo();		
	
	if(img == 0)
	{
	    wBack = CreateImageFromWindow(parent, &(rclbl.rect));
	}
	
	pWidget = CreateLabel(parent, flags, &(rclbl.rect), tmp, img, wBack);
	
	if (pWidget == NULL) return NULL;

	pWidget->nResourceID = id;

	return pWidget;
}

/** 
Create List Box widget from resource
@param parent, the parent of the list box to be created
@param flags, the types of the list box including the visible, enable and transparent
@param id, the id of the list box resource to be created
@param rcinfo, the resource info pointer
@retval  TN_LISTBOX
*/
TN_WIDGET*
CreateListBoxFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo)
{
	RCLBX		rclsbox;
	TN_WIDGET	*widget;
	char		**item, tmp[64];
	int		n ;
	GR_IMAGE_ID	img[4] = {0, 0, 0, 0};
	GR_IMAGE_ID 	*icon;
	GR_WINDOW_ID wBack = 0;
	
	get_rc_info(rcinfo, RC_LBX, id, &rclsbox);
	item = (char **)malloc(rclsbox.count * sizeof(char *));
	icon = (GR_IMAGE_ID*)malloc(rclsbox.count * sizeof(GR_IMAGE_ID));

	for(n = 0; n < rclsbox.count; n++)
	{
		get_rc_info(rcinfo, RC_STR, rclsbox.item[n], tmp);

		item[n] = strdup(tmp);

		if(item[n] == NULL)	return NULL;
		icon[n] = LoadImageFromRC(rcinfo, rclsbox.icon[n], NULL);
	}

	//img[0] = LoadImageFromRC(rcinfo, rclsbox.img[0]);
	img[1] = LoadImageFromRC(rcinfo, rclsbox.img[1], NULL);
	img[2] = LoadImageFromRC(rcinfo, rclsbox.img[2], NULL);
	img[3] = LoadImageFromRC(rcinfo, rclsbox.img[3], NULL);

    if(img[0] == 0)
    {
        wBack = CreateImageFromWindow(parent, &(rclsbox.rect));
    }

	widget = CreateListBox(parent, flags, &(rclsbox.rect), item, icon, rclsbox.count, img, wBack, rclsbox.type);
	
	for(n = 0; n < rclsbox.count; n++)
	{
	    printf(" Free item %d %x\n", n, item[n]);
	    free(item[n]);
	}
	
	free(item);
	
	if(!widget)	return NULL;

	widget->nResourceID = id;
	
	return widget;
}

/** 
Create progress bar widget from resource
@param parent, the parent of the ProgressBar to be created
@param flags, the types of the ProgressBar including the visible, enable and transparent
@param id, the id of the ProgressBar resource to be created
@param rcinfo, the resource info pointer
@retval  TN_PROGRESSBAR
*/
TN_WIDGET*
CreateProgressBarFromRC(TN_WIDGET * parent, int flags, int id, PRCINFO rcinfo)
{
	RCPGB		rcpgb;
	TN_WIDGET	*widget;
	GR_IMAGE_ID	img[2];
	
	get_rc_info(rcinfo, RC_PGB, id, &rcpgb);
	img[0] = LoadImageFromRC(rcinfo, rcpgb.img[0], &(rcpgb.rect));
	img[1] = LoadImageFromRC(rcinfo, rcpgb.img[1], &(rcpgb.rect));

	widget = CreateProgressBar(parent, flags, &(rcpgb.rect), rcpgb.ntype, rcpgb.stepsize, img);

	if(widget == NULL) return NULL;
	widget->nResourceID = id;
	
	return widget;
}

/** 
Create window widget from resource
@param parent, the parent of the window to be created
@param flags, the types of the window including the visible, enable and transparent
@param id, the id of the window resource to be created
@param rcinfo, the resource info pointer
@retval  TN_WINDOW
*/
TN_WIDGET*
CreateWindowFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo) 
{
	RCWND		rcwnd;
	GR_IMAGE_ID	img;
	TN_WIDGET	*pWidget = NULL;
    
	get_rc_info(rcinfo, RC_WND, id, &rcwnd);
//diag_mallocinfo();
	//img = LoadImageFromRC(rcinfo, rcwnd.img, &rcwnd.rect);
	printf(" %s - %s\n", __FUNCTION__, rcinfo->path);
	img = LoadImageFromRC(rcinfo, rcwnd.img, NULL);
//diag_mallocinfo();
	pWidget = CreateWindow(parent, flags, &rcwnd.rect, img, GR_ARGB(255, 255, 255, 0));

	if (pWidget == NULL) return NULL;

	pWidget->nResourceID = id;
	return pWidget;
}

/** 
Create picture widget from resource
@param parent, the parent of the picture to be created
@param flags, the types of the picture including the visible, enable and transparent
@param id, the id of the picture resource to be created
@param rcinfo, the resource info pointer
@retval  TN_PICTURE
*/
TN_WIDGET*
CreatePictureFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo) 
{
	RCPIC		rcpic;
	GR_IMAGE_ID	img;
	TN_WIDGET	*pWidget;
	GR_BOOL		stretch;
	
	get_rc_info(rcinfo, RC_PIC, id, &rcpic);
	img = LoadImageFromRC(rcinfo, rcpic.img, &rcpic.rect);
	stretch = rcpic.stretch ?  GR_TRUE : GR_FALSE;

	pWidget = CreateWidget(parent, &rcpic.rect, GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_FOCUS_IN | GR_EVENT_MASK_FOCUS_OUT
		       	| GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMER, flags, 0);

	if (pWidget == NULL) return NULL;
	// initialize picture related informatino.
	pWidget->type = TN_PICTURE;
	pWidget->WSpec.picture.imageid = img;
	pWidget->WSpec.picture.bFocus = GR_FALSE;
	pWidget->WSpec.picture.filename = NULL;
	pWidget->WSpec.picture.stretch = stretch;

	GrSetGCUseBackground(pWidget->gc, GR_FALSE);
	
	pWidget->nResourceID = id;
	return pWidget;
}

/** 
Create button group from resource
@param parent, the parent of the picture to be created
@param id, the id of button group to be created
@param rcinfo, PBTNGROUP resource info pointer
@retval  TN_PICTURE
*/

PBTNGROUP
CreateBtnGroupFromRC(TN_WIDGET *parent, int id, PRCINFO rcinfo) 
{
	RCBTNGRP		rcbtngrp;
	PBTNGROUP       pbtngroup;
		
	get_rc_info(rcinfo, RC_BTNGRP, id, &rcbtngrp);
	printf("CreateBtnGroupFromRC rect: %d, %d, %d, %d\n",rcbtngrp.rect.x,rcbtngrp.rect.y,rcbtngrp.rect.width,rcbtngrp.rect.height);
	printf("CreateBtnGroupFromRC %d, %d, %d, %d, %d\n", rcbtngrp.drawdata[0],rcbtngrp.drawdata[1],rcbtngrp.drawdata[2],rcbtngrp.drawdata[3],rcbtngrp.drawdata[4]);
	pbtngroup = CreateBtnGroup( parent, &(rcbtngrp.rect), rcbtngrp.type, &(rcbtngrp.drawdata));
	
	pbtngroup->nResourceID = id;	
	
	return pbtngroup;
}


/** 
Update the button widget's members from new resource
@param widget, the button widget to be updated
@param rcinfo, the new resource info pointer
*/
void 
UpdateFrontButton(TN_WIDGET *widget, PRCINFO rcinfo)
{
	RCBTN		rcbtn;
	char		tmp[32];
	GR_IMAGE_ID	img[2];

	if (widget->nResourceID == -1) return;
	
	get_rc_info(rcinfo, RC_BTN, widget->nResourceID, &rcbtn);
	get_rc_info(rcinfo, RC_STR, rcbtn.cap, tmp);
    //printf("UpdateFrontButton: %s\n",  tmp);
    img[0] = LoadImageFromRC(rcinfo, rcbtn.img[0], &rcbtn.rect);
	img[1] = LoadImageFromRC(rcinfo, rcbtn.img[1], &rcbtn.rect);
		
	tnSetButtonImage(widget, img);
	
	tnSetButtonCaption(widget, tmp);	

	GrResizeWindow(widget->wid, rcbtn.rect.width, rcbtn.rect.height);
	GrMoveWindow(widget->wid, rcbtn.rect.x, rcbtn.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the check button widget's members from new resource
@param widget, the check button widget to be updated
@param rcinfo, the new resource info pointer
*/
void 
UpdateFrontCheckButton(TN_WIDGET * widget, PRCINFO rcinfo)
{
	RCCKB  	rcckb;
	char 	tmp[32];

	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_CKB, widget->nResourceID, &rcckb);
	get_rc_info(rcinfo, RC_STR, rcckb.cap, tmp);

	widget->WSpec.checkbutton.imageid[0] = LoadImageFromRC(rcinfo, rcckb.img[0], &rcckb.rect);
	widget->WSpec.checkbutton.imageid[1] = LoadImageFromRC(rcinfo, rcckb.img[1], &rcckb.rect);
	widget->WSpec.checkbutton.imageid[2] = LoadImageFromRC(rcinfo, rcckb.img[2], &rcckb.rect);
	widget->WSpec.checkbutton.imageid[3] = LoadImageFromRC(rcinfo, rcckb.img[3], &rcckb.rect);

	if (widget->WSpec.checkbutton.caption)
		free(widget->WSpec.checkbutton.caption);
	widget->WSpec.checkbutton.caption = malloc(strlen(tmp) + 1);
	strcpy(widget->WSpec.checkbutton.caption, tmp);

	GrResizeWindow(widget->wid, rcckb.rect.width, rcckb.rect.height);
	GrMoveWindow(widget->wid, rcckb.rect.x, rcckb.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the radio button widget's members from new resource
@param widget, the radio button widget to be updated
@param rcinfo, the new resource info pointer
*/
void 
UpdateFrontRadioButton(TN_WIDGET * widget, PRCINFO rcinfo)
{
	RCRDB  	rcrdb;
	char 	tmp[32];
	
	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_RDB, widget->nResourceID, &rcrdb);
	get_rc_info(rcinfo, RC_STR, rcrdb.cap, tmp);
	widget->WSpec.radiobutton.imageid[0] = LoadImageFromRC(rcinfo, rcrdb.img[0], &rcrdb.rect);
	widget->WSpec.radiobutton.imageid[1] = LoadImageFromRC(rcinfo, rcrdb.img[1], &rcrdb.rect);
	widget->WSpec.radiobutton.imageid[2] = LoadImageFromRC(rcinfo, rcrdb.img[2], &rcrdb.rect);
	widget->WSpec.radiobutton.imageid[3] = LoadImageFromRC(rcinfo, rcrdb.img[3], &rcrdb.rect);

	if (widget->WSpec.radiobutton.caption)
		free(widget->WSpec.radiobutton.caption);
	widget->WSpec.radiobutton.caption = malloc(strlen(tmp) + 1);
	strcpy(widget->WSpec.radiobutton.caption, tmp);


	GrResizeWindow(widget->wid, rcrdb.rect.width, rcrdb.rect.height);
	GrMoveWindow(widget->wid, rcrdb.rect.x, rcrdb.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the label widget's members from new resource
@param widget, the label widget to be updated
@param rcinfo, the new resource info pointer
*/
void UpdateFrontLabel(TN_WIDGET *widget, PRCINFO rcinfo)
{
	RCLBL  		rclbl;
	char		tmp[32];
	GR_IMAGE_ID iid;
	
	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_LBL, widget->nResourceID, &rclbl);
	get_rc_info(rcinfo, RC_STR, rclbl.cap, tmp);
	
    iid = LoadImageFromRC(rcinfo, rclbl.img, &rclbl.rect);
    
    tnGetLabelImage(widget, iid);
   
	tnSetLabelCaption( widget, tmp);

	GrResizeWindow(widget->wid, rclbl.rect.width, rclbl.rect.height);
	GrMoveWindow(widget->wid, rclbl.rect.x, rclbl.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the list box widget's members from new resource
@param widget, the list box widget to be updated
@param rcinfo, the new resource info pointer
*/
void UpdateFrontListBox(TN_WIDGET * widget, PRCINFO rcinfo)
{
	RCLBX		rclsbox;
	char		tmp[64];
	int		n ;
	GR_IMAGE_ID *icon;
	struct listentry *p;
	TN_WIDGET	*scrlbar;
	
	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_LBX, widget->nResourceID, &rclsbox);
	icon = (GR_IMAGE_ID*)malloc(rclsbox.count * sizeof(GR_IMAGE_ID));
	p = widget->WSpec.listbox.list;
	
	for(n = 0; n < rclsbox.count; n++)
	{
		get_rc_info(rcinfo, RC_STR, rclsbox.item[n], tmp);
		icon[n] = LoadImageFromRC(rcinfo, rclsbox.icon[n], NULL);
		if(p){
			p->icon = icon[n];
			if(p->string)
				free(p->string);
			p->string = (char *)malloc((strlen(tmp)+1)*sizeof(char));	
			strcpy(p->string, tmp);
			if(p->next)
				p = p->next;
		}
	}
	widget->WSpec.listbox.imageid[0] = LoadImageFromRC(rcinfo, rclsbox.img[0], &rclsbox.rect);
	widget->WSpec.listbox.imageid[1] = LoadImageFromRC(rcinfo, rclsbox.img[1], &rclsbox.rect);
	
	GrResizeWindow(widget->wid, rclsbox.rect.width, rclsbox.rect.height);
	GrMoveWindow(widget->wid, rclsbox.rect.x, rclsbox.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);

	if(widget->WSpec.listbox.scrlbar){
		scrlbar = widget->WSpec.listbox.scrlbar;
		scrlbar->WSpec.scrollbar.imageid[0] = LoadImageFromRC(rcinfo, rclsbox.img[2], NULL);
		scrlbar->WSpec.scrollbar.imageid[1] = LoadImageFromRC(rcinfo, rclsbox.img[3], NULL);
		UpdateFrontScrollBar(scrlbar, rclsbox.rect);
	}
}

/** 
Update the progress bar widget's members from new resource
@param widget, the progress bar widget to be updated
@param rcinfo, the new resource info pointer
*/
void 
UpdateFrontProgressBar(TN_WIDGET * widget, PRCINFO rcinfo)
{
	RCPGB	rcpgb;
	
	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_PGB, widget->nResourceID, &rcpgb);
	widget->WSpec.progressbar.imageid[0] = LoadImageFromRC(rcinfo, rcpgb.img[0], &rcpgb.rect);
	widget->WSpec.progressbar.imageid[1] = LoadImageFromRC(rcinfo, rcpgb.img[1], &rcpgb.rect);
	
	GrResizeWindow(widget->wid, rcpgb.rect.width, rcpgb.rect.height);
	GrMoveWindow(widget->wid, rcpgb.rect.x, rcpgb.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the window widget's members from new resource
@param widget, the window widget to be updated
@param rcinfo, the new resource info pointer
*/
void 
UpdateFrontWindow(TN_WIDGET *widget, PRCINFO rcinfo)
{
	RCWND		rcwnd;
	GR_IMAGE_ID	img;

	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_WND, widget->nResourceID, &rcwnd); 	
	img = LoadImageFromRC(rcinfo, rcwnd.img, &rcwnd.rect);
	widget->WSpec.window.iBack = img;

	GrResizeWindow(widget->wid, rcwnd.rect.width, rcwnd.rect.height);
	GrMoveWindow(widget->wid, rcwnd.rect.x, rcwnd.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the scroll bar widget's members from new resource. This is only called by UpdateFrontListBox().
@param widget, the scroll bar widget to be updated
@param rect, the rect of the scroll bar
*/
void UpdateFrontScrollBar(TN_WIDGET * widget, GR_RECT rect)
{
	GR_WINDOW_INFO winfo;

	if (widget->nResourceID == -1) return;

	GrGetWindowInfo (widget->wid, &winfo);
	
	GrResizeWindow(widget->wid, winfo.width, rect.height);
	GrMoveWindow(widget->wid, rect.x + rect.width,rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

/** 
Update the picture widget's members from new resource
@param widget, the picture widget to be updated
@param rcinfo, the new resource info pointer
*/
void 
UpdateFrontPicture(TN_WIDGET *widget, PRCINFO rcinfo)
{
	RCPIC	rcpic;
	GR_BOOL	stretch;
	
	if (widget->nResourceID == -1) return;

	get_rc_info(rcinfo, RC_PIC, widget->nResourceID, &rcpic);
	widget->WSpec.picture.imageid = LoadImageFromRC(rcinfo, rcpic.img, &rcpic.rect);
	stretch = rcpic.stretch ?  GR_TRUE : GR_FALSE;
	widget->WSpec.picture.stretch = stretch;
	
	GrResizeWindow(widget->wid, rcpic.rect.width, rcpic.rect.height);
	GrMoveWindow(widget->wid, rcpic.rect.x, rcpic.rect.y);
	GrClearWindow(widget->wid, GR_TRUE);
}

///////////////////////////////////////////////////////
/*by amboo
get the whole file path accroding to the "subject" and "ap"
the rc name is same for different subject
*/
void getRcFilePath(char * allpath,char *resourcepath, char *subject, char * ap )
{
    if (allpath==NULL)
        return;
    strcpy(allpath,resourcepath);
    strcat(allpath,"/");
    strcat(allpath,subject);
    strcat(allpath,"/");
    //strcat(allpath,ap);
    //strcat(allpath,"/");
    strcat(allpath,ap);
    strcat(allpath,".rc");
}
