#include "tnWidgets.h"

/** 
Create radio button group.
@return, the pointer of radio button group.  
*/
PRDBGROUP
CreateRdbGroup(void)
{
	PRDBGROUP	rdb_group;

	if ((rdb_group = malloc(sizeof(RDBGROUP))) == NULL)
		return NULL;

	memset(rdb_group, 0, sizeof(RDBGROUP));
	return rdb_group;
}

/** 
Add a radio button to radio button group.
@param group, the pointer of radio button group.
@param widget, the radio button which will be add.
*/
void
AddRadioButton(PRDBGROUP group, TN_WIDGET* widget)
{
	PRDBITEM item;

	if (group == NULL || widget == NULL) return;
	if (widget->type != TN_RADIOBUTTON) return;

	item = malloc(sizeof(RDBITEM));
	if (item == NULL) return;
	memset(item, 0, sizeof(RDBITEM));

	item->next = group->head.next;	
	item->widget = widget;
	group->head.next = item;
	group->widget = widget;
}

/** 
change the selection for a radio button in the group was clicked.
@param group, radio button group.
@param widget, the new radio button which will be selected.
*/
void
SwitchSelect(PRDBGROUP group, TN_WIDGET* widget)
{
	PRDBITEM 	item;
	TN_WIDGET*	pWidget;

	if (group == NULL || widget == NULL) return;

	// clear last selection.
	for (item = group->head.next; item != NULL; item = item->next) {
		pWidget = item->widget;
		if (pWidget->WSpec.radiobutton.st_down) {
			pWidget->WSpec.radiobutton.st_down = GR_FALSE;
			GrClearWindow(pWidget->wid, GR_TRUE);
		}
	}	
	
	// set new select.
	widget->WSpec.radiobutton.st_down = GR_TRUE;
	GrClearWindow(widget->wid, GR_TRUE);

	if (group->pfn)
		group->pfn(widget);
}

void
RegisteRdbGroupCallBack(PRDBGROUP group, RDBGROUPCB callback)
{
	if (group != NULL)
		group->pfn = callback;
}	

/**
Destroy the radio button group which pointed by group.
@param group, the radio button group which will be destroyed.
*/
void
DestroyRdbGroup(PRDBGROUP group)
{
	PRDBITEM item;

	if (group == NULL) return;

	while (group->head.next) {
		item = group->head.next;
		group->head.next = item->next;
		free(item);
	}
}

/**
Get the selected radio button in the radio button group pointed by group.
@param group, radio button group.
*/
TN_WIDGET*
GetSelectRdb(PRDBGROUP group)
{
	if (group == NULL) return NULL;

	return group->widget;
}

