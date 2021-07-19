#include<stdio.h>
#include "tnWidgets.h"
#include "../resource/Blue/Demo/demo.h"

RCINFO		rcinfo;

void lbx_clicked(TN_WIDGET* widget, DATA_POINTER data)
{
}

int main(int argc,char **argv)
{
	TN_WIDGET 		*wnd;
	TN_WIDGET		*lbx;
	struct listentry	*p;	
	
	// initialize control libraries.
	tnAppInitialize();
	if (load_rc(&rcinfo, "./resource/Blue/Demo/demo.rc", "en") < 0) {
		printf("Failed to load rc file!\n");
		tnEndApp(&rcinfo);
	}

	// create main window.
	wnd = CreateWindowFromRC(NULL, 0, ID_WND3, &rcinfo);
	lbx = CreateListBoxFromRC(wnd, 0, ID_LBX, &rcinfo);
	tnRegisterCallBack(lbx, WIDGET_CLICKED, lbx_clicked, (DATA_POINTER)NULL);	

	GrSetFocus(lbx->wid);
	p = lbx->WSpec.listbox.list;
	p->selected = GR_TRUE;
	
	tnMainLoop();
	tnEndApp(&rcinfo);
	return 0;
}
