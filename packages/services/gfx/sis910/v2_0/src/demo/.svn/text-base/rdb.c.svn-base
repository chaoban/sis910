#include<stdio.h>
#include "tnWidgets.h"
#include "../resource/Blue/Demo/demo.h"

RCINFO		rcinfo;
TN_WIDGET*	rdb[8];

void rdb_clicked(TN_WIDGET* widget)
{
}

void rdb_keydown(TN_WIDGET* widget, DATA_POINTER data)
{
	int 	i = (int)data;
	MWKEY	ch = (MWKEY)((long)widget->data);

	if (ch == MWKEY_LEFT) {
		i--;
		if (i < 0) i = 7;

		GrSetFocus(rdb[i]->wid);
	}

	if (ch == MWKEY_RIGHT) {
		i++;
		if (i > 7) i = 0;

		GrSetFocus(rdb[i]->wid);
	}
}

int main(int argc,char **argv)
{
	TN_WIDGET* 	wnd;
	PRDBGROUP	group;
	int		i;
	
	// initialize control libraries.
	tnAppInitialize();
	if (load_rc(&rcinfo, "./resource/Blue/Demo/demo.rc", "en") < 0) {
		printf("Failed to load rc file!\n");
		return -1;
	}

	// create main window.
	wnd = CreateWindowFromRC(NULL, 0, ID_WND1, &rcinfo);

	// create radio button group for mutex.
	group = CreateRdbGroup();
	RegisteRdbGroupCallBack(group, rdb_clicked); 

	// create radio button.
	for (i = 0; i < 8; i++) { 
		rdb[i] = CreateRadioButtonFromRC(wnd, 0, i, group, &rcinfo);
		rdb[i]->data = (void *)i;
		tnRegisterCallBack(rdb[i], WIDGET_KEYDOWN, rdb_keydown, (DATA_POINTER)i);
	}

	GrSetFocus(rdb[0]->wid);
	SwitchSelect(group, rdb[0]);

	tnMainLoop();
	DestroyRdbGroup(group);
	tnEndApp(&rcinfo);
	return 0;
}
