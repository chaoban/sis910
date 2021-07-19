#ifndef _TNRADIOBUTTONGROUP_H_
#define _TNRADIOBUTTONGROUP_H_

#define RADIOBUTTONGROUP_CALLBACKS 2  
#include "tnBase.h"
#include <microwin/nano-X.h>

typedef void (*RDBGROUPCB)(TN_WIDGET* widget); 

typedef struct tagRDBITEM{
	struct tagRDBITEM *next;
	TN_WIDGET	  *widget;
} RDBITEM, *PRDBITEM;

typedef struct {
	RDBITEM		head;
	TN_WIDGET	*widget;
	RDBGROUPCB	pfn;
} RDBGROUP, *PRDBGROUP;

PRDBGROUP CreateRdbGroup(void);
void AddRadioButton(PRDBGROUP group, TN_WIDGET* widget);
void RegisterRdbGroupCallBack(PRDBGROUP group, RDBGROUPCB callback);
void SwitchSelect(PRDBGROUP group, TN_WIDGET* widget);
void DestroyRdbGroup(PRDBGROUP group);
TN_WIDGET* GetSelectRdb(PRDBGROUP group);

#endif /*_TNRADIOBUTTONGROUP_H_*/
