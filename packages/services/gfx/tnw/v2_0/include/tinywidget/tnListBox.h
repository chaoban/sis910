#ifndef _TNLISTBOX_H_
#define _TNLISTBOX_H_
#include "tnBase.h"
#include <microwin/nano-X.h>

struct listentry{
		  char *string;
		  GR_BOOL selected;
		  GR_IMAGE_ID icon;
		  struct listentry *next;
		  struct listentry *prev;
};

typedef struct
{
  	struct listentry *list;
  	struct listentry *last;
  	struct listentry *showtop;
  	int numitems;
  	TN_WIDGET	*scrlbar;
  	GR_IMAGE_ID iSelect;
  	GR_IMAGE_ID iBack;
  	GR_WINDOW_ID wBack;
  	GR_IMAGE_ID imageid[2];
  	int nColumn;
  	int nPages;
  	int nCurrentPage;
  	int nShowItemsNum;
  	int nItemWidth;          
  	int nItemHeight;
  	GR_RECT rIcon;
  	GR_RECT rString;
  	int nGapHeight;
  	int nType;          	            
}
TN_STRUCT_LISTBOX;

TN_WIDGET *CreateListBox(TN_WIDGET * parent, int flags, GR_RECT *rect, char **listitem, GR_IMAGE_ID *listicon,
		int count, GR_IMAGE_ID imageid[], GR_WINDOW_ID wnd, int nDrawType);
void ListBoxEventHandler (GR_EVENT *, TN_WIDGET *);
void DrawListBox (TN_WIDGET *);
struct listentry * NewListEntry(void);
int tnGetSelectedListItems(TN_WIDGET *,char ***,int *);
int tnGetAllListItems(TN_WIDGET *,char ***,int *);
int tnGetSelectedListNum(TN_WIDGET *);
int tnGetSelectedListPos(TN_WIDGET *,int **,int *);
int tnGetListItemPos(TN_WIDGET *, char *);
int tnAddItemToListBox(TN_WIDGET *,char *, GR_IMAGE_ID);
int tnAddItemToListBoxAt(TN_WIDGET *,char *,int, GR_IMAGE_ID);
int tnExchangeItems(TN_WIDGET *, int, int);
int tnDeleteItemFromListBox(TN_WIDGET *,char *);
int tnDeleteItemFromListBoxAt(TN_WIDGET *,int);
int tnDeleteSelectedItems(TN_WIDGET *);
int tnDeleteAllItemsFromListBox(TN_WIDGET *);
int tnSetSelectedListItem(TN_WIDGET *,char *,GR_BOOL);
int tnSetSelectedListItemAt(TN_WIDGET *,int,GR_BOOL);
int tnListItemsLineUp(TN_WIDGET *,int);
int tnListItemsLineDown(TN_WIDGET *,int);
int tnGetListTop(TN_WIDGET *);
/*No Resize Now
int tnGetListBoxResize(TN_WIDGET *, GR_BOOL *);
int tnSetListBoxResize(TN_WIDGET *, GR_BOOL );
*/
void DeletePtr(TN_WIDGET *,struct listentry *);
void DestroyListBox(TN_WIDGET *);
#endif /*_TNLISTBOX_H_*/
