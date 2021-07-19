#include<stdio.h>
#include "../include/tnWidgets.h"

TN_WIDGET *main_widget,*lbox1,*window1,*window2=NULL,*button1,*button2,*textbox1,*label1,*label2;

GR_IMAGE_ID	img;

void OnDraw(void *p)
{
	/*
	TN_WIDGET *pWidget = (TN_WIDGET*)p;

	GR_GC_ID gc = GrNewGC();

	GrSetGCBackground(gc, GR_RGB(255, 0, 0));
	GrSetGCUseBackground(gc, GR_FALSE);
	GrDrawImageToFit(pWidget->wid, gc, 0, 0, 61, 56, img);	
	*/
		
	printf("OnDraw\n");
}

void Timer(void *p)
{
	printf("Timer\n");
}

int main(int argc,char **argv)
{
	int i;

	main_widget=tnAppInitialize("/home/solarhe/control/demo/resource/test.rc", "en");
	window1= CreateWindow(main_widget, 50, 50, 400, 300, -1, GR_RGB(255, 255, 255)); 
	//window1 = CreateWindowFromRC(main_widget, 0, 0);

	GrCreateTimer(window1->wid, 300);
	tnRegisterCallBack(window1, EXPOSURE, (CallBackFuncPtr)OnDraw, window1);
	tnRegisterCallBack(window1, TIMER, (CallBackFuncPtr)Timer, NULL);

	
	for (i = 0; i < 16; i++)
		CreateButtonFromRC(window1, TN_CONTROL_FLAGS_DEFAULT, i);

	//img = GrLoadImageFromFile("/home/solarhe/icon.bmp", 0);
	tnMainLoop();
	return 0;
}

