#include<stdio.h>
#include "tnWidgets.h"
#include "../resource/Blue/Demo/demo.h"


TN_WIDGET*	ckb[8];
RCINFO		rcinfo;

void ckb_clicked(TN_WIDGET* widget, DATA_POINTER data)
{
	
}

void ckb_keydown(TN_WIDGET* widget, DATA_POINTER data)
{
	int 	i = (int)data;
	MWKEY	ch = (MWKEY)((long)widget->data);

	if (ch == MWKEY_LEFT) {
		i--;
		if (i < 0) i = 7;

		GrSetFocus(ckb[i]->wid);
	}

	if (ch == MWKEY_RIGHT) {
		i++;
		if (i > 7) i = 0;

		GrSetFocus(ckb[i]->wid);
	}
}

int main(int argc,char **argv)
{
	TN_WIDGET* 	wnd;
	int		i;
	
	// initialize control libraries.
	tnAppInitialize();
	if (load_rc(&rcinfo, "./resource/Blue/Demo/demo.rc", "en") < 0) {
		printf("Failed to load rc file!\n");
		return -1;
	}

	// create main window.
	wnd = CreateWindowFromRC(NULL, 0, ID_WND1, &rcinfo);

	for (i = 0; i < 8; i++) {
		ckb[i] = CreateCheckButtonFromRC(wnd, 0, i, &rcinfo);
		tnRegisterCallBack(ckb[i], WIDGET_CLICKED, ckb_clicked, (DATA_POINTER)NULL);
		tnRegisterCallBack(ckb[i], WIDGET_KEYDOWN, ckb_keydown, (DATA_POINTER)i);
	}

	GrSetFocus(ckb[0]->wid);
	tnMainLoop();

	tnEndApp(&rcinfo);
	return 0;
}
