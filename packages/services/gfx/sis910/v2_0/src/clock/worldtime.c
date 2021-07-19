#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "tinywidget/tnWidgets.h"    
#include "../share/global.h"

#include "worldtime.h"

TN_WIDGET *world_window;
TN_WIDGET *lab[4];
TN_WIDGET *worldmap;

int area_pos[24]={260,273,286,299,312,13,26,39,
				52,65,78,91,104,117,130,143,
				156,169,182,195,208,221,234,247};
//char *area_name[24]={"beijing","tokyo","sydney","Okhotsk","Welington","Samoa/Midway","Honolulu","Alaska",
//							"L.A","Denver","Chicago","New York","Caracas","Buenos Aires","Mid-Atlantic","London",
//							"Paris/Rome","Athens","Moscow","Abu Dhbi","Tashkent","Delhi","Almaty","Bangkok"};				
//int time_offset[24]={0,1,2,3,4,-19,-18,-17,
//							-16,-15,-14,-13,-12,-11,-10,-9,
//							-8,-7,-6,-5,-4,-3,-2,-1};

int time_offset[24]={-19,-18,-17,
							-16,-15,-14,-13,-12,-11,-10,-9,
							-8,-7,-6,-5,-4,-3,-2,-1,
							0,1,2,3,4};

char *area_name[24]={"Samoa/Midway","Honolulu","Alaska",
							"L.A","Denver","Chicago","New York","Caracas","Buenos Aires","Mid-Atlantic","London",
							"Paris/Rome","Athens","Moscow","Abu Dhbi","Tashkent","Delhi","Almaty","Bangkok",
							"beijing","tokyo","sydney","Okhotsk","Welington"};				


int cur_pos = 19;
GR_RECT area_unit;//x,y: basic area(Samoa/Midway), width: area unit, height: liine length
//////////////	beijing,
extern RCINFO gClockRcinfo;
extern TN_WIDGET *btn_time[5], *btn_world;
extern TN_WIDGET *window,*w_time;
extern struct tm *now_day;

void OnCreateWorldWindow()
{		
    //GR_RECT r={200,100,400,330};
    char bj[32];
    world_window = CreateWindowFromRC( NULL,0,ID_WND_WORLDTIME,&gClockRcinfo);
    
    worldmap = CreatePictureFromRC(world_window,0,ID_PIC_WORLDMAP,&gClockRcinfo);
     
    lab[0]= CreateLabelFromRC(world_window,0,ID_LBL_LOCALAREA,&gClockRcinfo);
    lab[1]= CreateLabelFromRC(world_window,0,ID_LBL_LOCALTIME,&gClockRcinfo);
    lab[2]= CreateLabelFromRC(world_window,0,ID_LBL_WORLDAREA,&gClockRcinfo);
    lab[3]= CreateLabelFromRC(world_window,0,ID_LBL_WORLDTIME,&gClockRcinfo);
    
    now_day = (struct tm *) GetCurrDay();
    sprintf(bj,"%d:%d  %d-%d-%d",now_day->tm_hour,now_day->tm_min,now_day->tm_year+1900,now_day->tm_mon+1,
    		now_day->tm_mday);
    
    tnSetLabelCaption(lab[0],area_name[cur_pos]);
    tnSetLabelCaption(lab[1],bj);
    tnSetLabelCaption(lab[2],area_name[cur_pos]);
    tnSetLabelCaption(lab[3],bj); 
    
    tnRegisterCallBack( world_window, WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)btn_world);
    tnRegisterCallBack( worldmap, WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)btn_world); 		 
    tnRegisterCallBack(world_window,  WIDGET_KEYDOWN,(CallBackFuncPtr)OnWorldTimeSetting,NULL);
    tnRegisterCallBack(worldmap,  WIDGET_KEYDOWN,(CallBackFuncPtr)OnWorldTimeSetting,NULL);
    
    GrSetFocus(world_window->wid);
    
    //Initial World time
    {
        GR_WINDOW_INFO 	winfo;
        int offset = 0;
        GrGetWindowInfo (worldmap->wid, &winfo);
        
        offset = winfo.width / 24;
        area_unit.x = offset*2;
        area_unit.y = winfo.y;
        area_unit.width = offset;
        area_unit.height = winfo.height;
        
        offset = area_unit.width * cur_pos + area_unit.x;
        GrLine(worldmap->wid,worldmap->gc,offset,area_unit.y,offset,area_unit.height);
    }
}


void DestroyWorldWindow()
{
    tnDestroyWidget(world_window);
    GrMapWindow(w_time->wid);
    GrSetFocus(btn_time[4]->wid);
}

void OnWorldTimeSetting(TN_WIDGET* pwidget,DATA_POINTER p)
{
    long		ch;
    char area_time[32];
    struct tm t;
    int offset;
    
    memcpy(&t,now_day,sizeof(struct tm));
    
    ch = (long)pwidget->data;
    if (ch == MWKEY_LEFT)
        cur_pos--;
    else if (ch == MWKEY_RIGHT)
        cur_pos++;
        	
	
	if(cur_pos < 0)   cur_pos = 23;
	if(cur_pos > 23)    cur_pos = 0;
	
	t.tm_hour += time_offset[cur_pos];
	if(t.tm_hour>23)
	{
	    t.tm_hour = 0;
	    t.tm_mday++;
	}
	if(t.tm_hour<0)
	{
	    t.tm_hour = 23;
	    t.tm_mday++;					
	}
	
	DrawPicture(worldmap);
	sprintf(area_time,"%d:%d  %d-%d-%d",t.tm_hour,
	t.tm_min,t.tm_year+1900,t.tm_mon+1,t.tm_mday);
	
	printf("World Time:%s\n",area_time);
			
	tnSetLabelCaption(lab[2],area_name[cur_pos]);
	tnSetLabelCaption(lab[3],area_time);
	offset = area_unit.width * cur_pos + area_unit.x;
    GrLine(worldmap->wid,worldmap->gc,offset,area_unit.y,offset,area_unit.height);
}

