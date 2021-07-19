
#include "tinywidget/tnWidgets.h"
#include<string.h>
#include <time.h>
#include<stdlib.h>
#include <sys/time.h>
#include "../config/config.h"

extern 	TN_WIDGET *window,*w_time,*w_alarm,*w_showclock,*w_calendar;
extern	TN_WIDGET *alarm_state,*alarm_time,*alarm_tone;
extern	TN_WIDGET *w_settime,*w_setdata,*w_worldtime;
extern	TN_WIDGET *alarm_state,*alarm_time,*alarm_tone;	

extern RCINFO gClockRcinfo;

extern	TN_WIDGET *button[4];
extern	TN_WIDGET *btn_time[5],*btn_world;
extern	TN_WIDGET 	*btn_alarm[4];
extern	TN_WIDGET	*btn_clock[2];
extern 	TN_WIDGET	*btn_settime[5];
extern 	TN_WIDGET *qj_label[57];
extern 	TN_WIDGET *ckb[37];

struct tm *show_day,*now_day;

int Write_AlarmConfig()
{
    struct configFile *alarm_cfg;
    char *AlarmState;
    int AlarmHour,AlarmMinute;
    
    alarm_cfg = readConfigFile(CFG_PATH);
    if(!alarm_cfg)
    {
        printf("can not open confige file!!\n");
        return -1;
    }
    	
    char t[8];
    tnGetButtonCaption(btn_alarm[0],t);
    if(!strcmp(t,"ON"))
        AlarmState="ON";
    else 
        AlarmState="OFF";
    
    tnGetButtonCaption(btn_alarm[1],t);
    AlarmHour = atoi(t);
    
    tnGetButtonCaption(btn_alarm[2],t);
    AlarmMinute = atoi(t);
    
    setConfigString(alarm_cfg,"CLOCK","AlarmState",AlarmState);
    setConfigInt(alarm_cfg,"CLOCK","AlarmHour",AlarmHour);
    setConfigInt(alarm_cfg,"CLOCK","AlarmMinute",AlarmMinute);
    
    saveConfigFile(alarm_cfg,CFG_PATH);
    unloadConfigFile(alarm_cfg);    
}

int Read_AlarmConfig()
{
    struct configFile *alarm_cfg;
    char *AlarmState;
    int AlarmHour,AlarmMinute;
    char hour_str[4],minute_str[4];
    alarm_cfg = readConfigFile(CFG_PATH);
    if(!alarm_cfg)
    {
        printf("can not open confige file!!\n");
        return -1;
    }
    	
    AlarmState = getConfigString(alarm_cfg,"CLOCK","AlarmState");	
    AlarmHour =  getConfigInt(alarm_cfg,"CLOCK","AlarmHour");
    AlarmMinute= getConfigInt(alarm_cfg,"CLOCK","AlarmMinute");
    
    sprintf(hour_str,"%d",AlarmHour);
    sprintf(minute_str,"%d",AlarmMinute);
    
    if(AlarmState)
    tnSetButtonCaption(btn_alarm[0],AlarmState);
    
    tnSetButtonCaption(btn_alarm[1],hour_str);
    tnSetButtonCaption(btn_alarm[2],minute_str);
    
    unloadConfigFile(alarm_cfg);
}

int Write_ClockConfig()
{
    struct configFile *clock_cfg;
    char *ClockState,*ClockLocation;
    
    clock_cfg = readConfigFile(CFG_PATH);
    if(!clock_cfg)
    {
        printf("can not open confige file!!\n");
        return -1;
    }
    	
    char t[8];
    tnGetButtonCaption(btn_clock[0],t);
    if(!strcmp(t,"ON"))
        ClockState="ON";
    else 
        ClockState="OFF";
    
    tnGetButtonCaption(btn_clock[1],t);
    if(!strcmp(t,"Right"))
        ClockLocation = "Right";
    else 
        ClockLocation = "Left";
    		
    setConfigString(clock_cfg,"CLOCK","ShowClock",ClockState);
    setConfigString(clock_cfg,"CLOCK","ClockLocation",ClockLocation);		
    
    saveConfigFile(clock_cfg,CFG_PATH);
    unloadConfigFile(clock_cfg);
    
}

int Read_ClockConfig()
{struct configFile *clock_cfg;
	char *ClockState,*ClockLocation;
	
	clock_cfg = readConfigFile(CFG_PATH);
	if(!clock_cfg)
	{
	    printf("can not open confige file!!\n");
	    return -1;
	}
		
	ClockState = getConfigString(clock_cfg,"CLOCK","ShowClock");
	ClockLocation=getConfigString(clock_cfg,"CLOCK","ClockLocation");
			
	if(ClockState)
	    tnSetButtonCaption(btn_clock[0],ClockState);
	if(ClockLocation)
	    tnSetButtonCaption(btn_clock[1],ClockLocation);
			
	unloadConfigFile(clock_cfg);
}

#ifdef __ECOS
int settimeofday ( const struct timeval *tv,const struct timezone *tz)
{
    cyg_libc_time_settime(tv->tv_sec);
    return  1;
}
#endif

int SetSystemTimeData()
{
    struct timeval tv;
    struct tm set_tm;
    time_t time_ep;
    
    int 	month,year,day,hour,minute;
    char 	month_str[8],year_str[8],day_str[8],hour_str[8],minute_str[8];
    tnGetButtonCaption(btn_time[0],hour_str);
    tnGetButtonCaption(btn_time[1],minute_str);
    tnGetButtonCaption(btn_time[2],year_str);
    tnGetButtonCaption(btn_time[3],month_str);
    tnGetButtonCaption(btn_time[4],day_str);
    
    set_tm.tm_year = atoi(year_str)-1900;
    set_tm.tm_mon= atoi(month_str)-1;
    set_tm.tm_mday = atoi(day_str);
    set_tm.tm_hour = atoi(hour_str);
    set_tm.tm_min	= atoi(minute_str);
    set_tm.tm_sec	= 0;
    
    time_ep=mktime(&set_tm);
    tv.tv_sec = time_ep;
    tv.tv_usec= 0;
    
    if( settimeofday(&tv,(struct timezone*)0) < 0)
    {
        printf("set time error!!!\n");
        return -1;
    }
    else 
    {
        printf("set time successfull!!\n");
        return 0;
    }
    	
}

int GetSysTimeData()
{		
    char hour[8],minute[8],year[8],month[8],day[8];
    GetCurrDay();
    
    sprintf(hour,"%d",now_day->tm_hour);
    sprintf(minute,"%d",now_day->tm_min);
    sprintf(year,"%d",now_day->tm_year+1900);
    sprintf(month,"%d",now_day->tm_mon+1);
    sprintf(day,"%d",now_day->tm_mday);
    
    tnSetButtonCaption(btn_time[0],hour);
    tnSetButtonCaption(btn_time[1],minute);
    tnSetButtonCaption(btn_time[2],year);
    tnSetButtonCaption(btn_time[3],month);
    tnSetButtonCaption(btn_time[4],day);
    
}



void button_keydown(TN_WIDGET* pwidget,DATA_POINTER data)
{
    long		ch;
    TN_WIDGET*	pw;
    
    ch = (long)pwidget->data;
    if (ch == MWKEY_LEFT)
    {
        if(pwidget == button[0])
    	    pw = button[3];
        else
        pw = GetPrevSibling(pwidget);
            GrSetFocus(pw->wid);	
        return;
    }
    if (ch == MWKEY_RIGHT)
    {
        if(pwidget == button[3])
            pw = button[0];
        else
    	pw = GetNextSibling(pwidget);
    	GrSetFocus(pw->wid);	
    	return;
    }	
    
}



void OnClicked(TN_WIDGET* pwidget,DATA_POINTER p)
{
    //	int n=((unsigned int)pwidget-(unsigned int)button[0])/144;
    int n = (int)p;
    switch(n)
    {	
        case 0:					
            GetSysTimeData();
            GrMapWindow(w_time->wid);
            GrSetFocus(btn_time[0]->wid);
            //GrUnmapWindow(window->wid);
            break;
        case 1:
            Read_AlarmConfig();
            GrMapWindow(w_alarm->wid);
            GrSetFocus(btn_alarm[0]->wid);
            //GrUnmapWindow(window->wid);
       	    break;
        case 2:				
            Read_ClockConfig();
            GrMapWindow(w_showclock->wid);
            GrSetFocus(btn_clock[0]->wid);
            //GrUnmapWindow(window->wid);
            break;
        case 3:
            GrMapWindow(w_calendar->wid);
            //GrSetFocus(w_calendar->wid);
            GrSetFocus(ckb[0]->wid);
            GrUnmapWindow(window->wid);
            break;    
    }
    					
}


void keydown_world(TN_WIDGET* pwidget,DATA_POINTER p)
{
    long		ch;
    TN_WIDGET*	pw;
    char str[16]="";
    int hour;
    ch = (long)pwidget->data;
    if(ch == MWKEY_BACKSPACE)
    {
        //	SetSystemTimeData();
        GrMapWindow(window->wid);
        GrSetFocus(button[0]->wid);
        GrUnmapWindow(w_time->wid);
        return;
    }
    
    
    if (ch == MWKEY_LEFT)
    {
        pw = btn_time[4];
        GrSetFocus(pw->wid);	
        return;
    }
    if (ch == MWKEY_RIGHT)
    {
        pw = btn_time[0];
        GrSetFocus(pw->wid);	
        return;
    }	
    

}

void keydown_AlarmTone(TN_WIDGET* pwidget,DATA_POINTER p)
{
    long		ch;
    TN_WIDGET*	pw;
    ch = (long)pwidget->data;
    if(ch == MWKEY_BACKSPACE)
    {	
        Write_AlarmConfig();
        GrMapWindow(window->wid);
        //GrClearWindow(button[1]->wid,1);
        GrSetFocus(button[0]->wid);
        GrUnmapWindow(w_alarm->wid);
        return;
    }
    
    if (ch == MWKEY_LEFT)
    {
        pw = GetPrevSibling(pwidget);
        GrSetFocus(pw->wid);	
        return;
    }
    if (ch == MWKEY_RIGHT)
    {
        pw = btn_alarm[0];
        GrSetFocus(pw->wid);	
        return;
    }	
}


void Click_Worldtime()
{}


/*
void Show_hour()
{	char hour[4];
struct timeval tv;
struct timezone tz;
struct tm* tp=(struct tm*)malloc(sizeof(struct tm));
time_t now;
	gettimeofday(&tv, &tz);
	now = tv.tv_sec - (60 * tz.tz_minuteswest);
	tp = gmtime(&now);
	sprintf(hour,"%d",tp->tm_hour);
	tnSetButtonCaption(btn_settime[0],hour);

}
*/

void Show_minute()
{}








