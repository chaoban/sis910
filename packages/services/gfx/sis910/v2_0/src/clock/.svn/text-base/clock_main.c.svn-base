#include<stdio.h>
#include <time.h>

#include "tinywidget/tnWidgets.h"
#include "../share/ecos_config.h"
#include "clock.h"
#include<string.h>

//#include"event_handle.h"
//#include "rc.h"
//#include "calendar.h"
#include "worldtime.h"
#include "../share/global.h"

#define CLOCK_YEAR    1
#define CLOCK_MONTH   2
#define CLOCK_DAY     3
#define CLOCK_HOUR    4
#define CLOCK_MINUTE  5
#define ALARM_STATE   6
#define ALARM_HOUR    7
#define ALARM_MINUTE  8
#define CLOCK_SHOW_STATE  9
#define CLOCK_SHOW_LOCATE  10

#define PREVMON_COLOR MWRGB(200, 0, 0)
#define CURMON_COLOR MWRGB(255, 255, 255)
#define NEXTMON_COLOR MWRGB(0, 0, 200)

DPF_EVENT gAlarmEvent;
extern cyg_handle_t dpfDesktopMboxHandle;
static struct configFile *gpcfgFile=NULL;
static GR_BOOL gbClockOpen = GR_FALSE;
RCINFO	gClockRcinfo;
char grcfileDir[128];
TN_WIDGET *window,*w_time,*w_alarm,*w_showclock,*w_calendar;
		
TN_WIDGET 	*button[4];
TN_WIDGET 	*btn_time[5];
TN_WIDGET 	*btn_alarm[4];
TN_WIDGET	*btn_clock[2];
TN_WIDGET 	*wbk,*wtitle;
			
//TN_WIDGET *btn_settime[3];
TN_WIDGET *qj_label[57];
TN_WIDGET *ckb[37];
TN_WIDGET *gbtnGRYear, *gbtnGRMonth, *gbtnGRDay;
TN_WIDGET *gppicYear[4],*pic_gang[2],*gppicMonth[2],*gppicDay[2];
TN_WIDGET *pic_hour[2],*pic_dot,*pic_minute[2],*pic_second[2];
TN_WIDGET *btn_world;

GR_IMAGE_ID LED_NUM[10], CALENDAR_CHK[6];
		
struct tm calendar_day;
int gcalendar_year, gcalendar_month, gcalendar_day;
static LIST gImportantDays;

void ClockFocusExchange(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *focus=widget;
    GR_KEY ch;
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_PREV:
            do
                focus=GetPrevSibling(focus);
            while((focus->type != TN_BUTTON) && (focus->type != TN_CHECKBUTTON));
            GrSetFocus(focus->wid);                   
            break;
        case DPFKEY_NEXT:
            do
                focus=GetNextSibling(focus);
            while ((focus->type != TN_BUTTON) && (focus->type != TN_CHECKBUTTON));
            GrSetFocus(focus->wid);
            break;                      
    }
}

void update_calendar()
{
    {
        int year1 = 0, year2 = 0, year3 = 0, year4 = 0;
        int month1 = 0, month2 = 0;
        int day1 = 0, day2 = 0;
        year4 = gcalendar_year %10;
        year3 = (gcalendar_year / 10) %10;
        year2 = (gcalendar_year / 100) %10;
        year1 = gcalendar_year / 1000;
        
        month2 = (gcalendar_month)%10;
        month1 = (gcalendar_month) / 10;
        
        day2 = gcalendar_day %10;
        day1 = gcalendar_day / 10;
        
        tnSetPictureID(gppicYear[0],LED_NUM[year1]);
        tnSetPictureID(gppicYear[1],LED_NUM[year2]);
        tnSetPictureID(gppicYear[2],LED_NUM[year3]);
        tnSetPictureID(gppicYear[3],LED_NUM[year4]);
        
        tnSetPictureID(gppicMonth[0],LED_NUM[month1]);
        tnSetPictureID(gppicMonth[1],LED_NUM[month2]);
        
        tnSetPictureID(gppicDay[0],LED_NUM[day1]);
        tnSetPictureID(gppicDay[1],LED_NUM[day2]);
        
        //tnSetPictureID(gppicTimeH[0],LED_NUM[0]);
        //tnSetPictureID(gppicTimeH[1],LED_NUM[8]);
        //
        //tnSetPictureID(gppicTimM[0],LED_NUM[1]);
        //tnSetPictureID(gppicTimM[1],LED_NUM[5]);
    }
    {
        int i = 0;
        int firstday = 3;
        int nowdays = 0, prevdays = 0, day = 1;
        int prevmonth = 1;
        char str[3];
        unsigned int keybase, key;
        GR_COLOR color;
      	
        firstday = gGetWeekDay(gcalendar_year, gcalendar_month,1);
        nowdays = gGetNumDaysOfMonth(gcalendar_year, gcalendar_month);
        
        if(gcalendar_month == 1)
        {
            keybase = 1200;
            prevdays = 31;
        }
        else
        {
            keybase = (gcalendar_month -1)*100;
            prevdays = gGetNumDaysOfMonth(gcalendar_year, gcalendar_month -1);
        }
        
        color = PREVMON_COLOR;        
                
        for( i = firstday - 1; 0 <= i; i--)
        {                            
            if(listSearch( &gImportantDays, prevdays + keybase))
                tnSetCheckButtonStatus( ckb[i], GR_TRUE);
            else
                tnSetCheckButtonStatus( ckb[i], GR_FALSE);
            
            sprintf( str, "%d", prevdays);
            tnSetCheckButtonCaptionColor(ckb[i],color);
            tnSetCheckButtonCaption(ckb[i], str);             
                
            prevdays --;
            
        }
        
        color = CURMON_COLOR;
        keybase = gcalendar_month*100;
        
        for( i = firstday; i < 36; i++)
        {
            if(listSearch( &gImportantDays, day + keybase))
            {                
                tnSetCheckButtonStatus( ckb[i], GR_TRUE);
            }
            else
                tnSetCheckButtonStatus( ckb[i], GR_FALSE);
                            
            sprintf( str, "%d", day); 
            tnSetCheckButtonCaptionColor(ckb[i],color);
            tnSetCheckButtonCaption(ckb[i], str);                        
                
            day ++;
            if( day > nowdays)
            {
                keybase = (gcalendar_month == 12) ? 100 : (gcalendar_month + 1) * 100;                    
                color = NEXTMON_COLOR;
                day = 1;
            }
        }
    }
}

void CalendarKeyDown(TN_WIDGET *widget, DATA_POINTER dptr) 
{    
    int *data = (int *) dptr;
    GR_KEY ch;
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_VALDOWN:
            if( *data == gcalendar_year)
                gcalendar_year = (gcalendar_year == 1) ? 9999: gcalendar_year - 1;    
            else if( *data == gcalendar_month)
                gcalendar_month = (gcalendar_month == 1) ? 12: gcalendar_month - 1;    
            
                printf("CalendarKeyDown %d\n", gcalendar_month);
            update_calendar();
            break;
        case DPFKEY_VALUP:
            if( *data == gcalendar_year)
                gcalendar_year = (gcalendar_year == 9999) ? 0: gcalendar_year + 1;
           else if( *data == gcalendar_month)
                gcalendar_month = (gcalendar_month == 12) ? 1: gcalendar_month + 1;
                
            update_calendar();
            break;
        case DPFKEY_PREV:
        case DPFKEY_NEXT:
            ClockFocusExchange( widget, dptr);
            break;
    }
    
}
        
void OnValueSetting(TN_WIDGET *widget, DATA_POINTER dptr)
{
    GR_KEY ch;
    int nPlus = 0, nCurrent;
    char str[4];
    ch=(long)widget->data;
    
    switch(ch)
    {
        case DPFKEY_PREV:
        case DPFKEY_NEXT:            
            OnControlFocusExchange( widget, dptr);
            return;
        case DPFKEY_VALUP:
            nPlus = 1;
            break;
        case DPFKEY_VALDOWN:
            nPlus = -1;
            break;        
    }
    
    int n = (int)dptr;
    
    tnGetButtonCaption(widget,str);    
                   
    switch(n)
    {
        case CLOCK_YEAR://Year            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 1, 9999);
            break;
            
        case CLOCK_MONTH://Month            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 1, 12);
            break;
            
        case CLOCK_DAY://Day            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 1, 31);
            break;
            
        case CLOCK_HOUR://Hour            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 0, 23);
            break;
            
        case CLOCK_MINUTE://Minute            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 0, 59);
            break;
            
        case ALARM_STATE:
            SET_BTN_VALUE_STR(str, "ON", "OFF");            
            break;
            
        case ALARM_HOUR:            
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 0, 23);
            break;
            
        case ALARM_MINUTE:
            SET_BTN_VALUE_NUM(str, nCurrent, nPlus, 0, 59);
            break;
            
        case CLOCK_SHOW_STATE:
            SET_BTN_VALUE_STR(str, "ON", "OFF");
            break;
            
        case CLOCK_SHOW_LOCATE:
            SET_BTN_VALUE_STR(str, "Left", "Right");
            break;
            
        default:
            return;
    }
    
    tnSetButtonCaption(widget,str);
}


void SaveClockTime(TN_WIDGET* pwidget,DATA_POINTER p)
{
    struct timeval tv;
    struct tm set_tm;
    time_t time_ep;
    char 	month_str[8],year_str[8],day_str[8],hour_str[8],minute_str[8];
    
    tnGetButtonCaption(btn_time[0],year_str);
    tnGetButtonCaption(btn_time[1],month_str);
    tnGetButtonCaption(btn_time[2],day_str);
    tnGetButtonCaption(btn_time[3],hour_str);
    tnGetButtonCaption(btn_time[4],minute_str);
    
    set_tm.tm_year = atoi(year_str)-1900;
    set_tm.tm_mon= atoi(month_str)-1;
    set_tm.tm_mday = atoi(day_str);
    set_tm.tm_hour = atoi(hour_str);
    set_tm.tm_min	= atoi(minute_str);
    set_tm.tm_sec	= 0;
    
    time_ep=mktime(&set_tm);
    tv.tv_sec = time_ep;
    tv.tv_usec= 0;
    
    if( gSetCurrTime(&tv) < 0)
    {
        printf("set time error!!!\n");
    }
    else 
    {
        printf("set time successfull!!\n");
    }
    
    OnSubMenuEscape( pwidget, p);
}

void SaveAlarmTime(TN_WIDGET* pwidget,DATA_POINTER p)
{
    char 	tmp[8];
    
    tnGetButtonCaption(btn_alarm[0],tmp);
    setConfigString(gpcfgFile,"CLOCK","AlarmState",tmp);
    tnGetButtonCaption(btn_alarm[1],tmp);
    setConfigString(gpcfgFile,"CLOCK","AlarmHour",tmp);
    tnGetButtonCaption(btn_alarm[2],tmp);
    setConfigString(gpcfgFile,"CLOCK","AlarmMinute",tmp);
        
    gAlarmEvent.alarm.type = DPF_ALARM_UPDATE;
    cyg_mbox_put(dpfDesktopMboxHandle, (void *) &gAlarmEvent);
    
    OnSubMenuEscape( pwidget, p);
}

void SaveClockShow(TN_WIDGET* pwidget,DATA_POINTER p)
{
    char 	tmp[8];
    
    tnGetButtonCaption(btn_clock[0],tmp);
    setConfigString(gpcfgFile,"CLOCK","ShowClock",tmp);
    tnGetButtonCaption(btn_clock[1],tmp);
    setConfigString(gpcfgFile,"CLOCK","ClockLoc",tmp);
    printf("SaveClockShow successfull!!\n");
    OnSubMenuEscape( pwidget, p);
}
void SaveTipDays(TN_WIDGET* pwidget,DATA_POINTER p)
{    
    gSetImportantDay( gpcfgFile, &gImportantDays);
    listFree( &gImportantDays);
    printf("%s - before %d\n", __FUNCTION__, w_calendar->wid);
    OnSubMenuEscape( w_calendar, p);
    printf("%s - after %d\n", __FUNCTION__, w_calendar->wid);
    w_calendar = NULL;
    //printf("%s - %d\n", __FUNCTION__, w_calendar->wid);
}

void TipDay(TN_WIDGET* pwidget,DATA_POINTER p)
{
    GR_BOOL bstate;
    int day, keybase;
    char tmp[8];
    GR_COLOR color;
    int month;
    
    tnGetCheckButtonCaptionColor( pwidget, &color);
    
    if(color == CURMON_COLOR)
        month = gcalendar_month;
    else if(color == PREVMON_COLOR)
        month = (gcalendar_month == 1)? 12 : (gcalendar_month - 1);
    else
        month = (gcalendar_month == 12) ? 1 : (gcalendar_month + 1);
    
    keybase = month * 100;
    
    tnGetCheckButtonStatus( pwidget, &bstate);
    tnGetCheckButtonCaption( pwidget,tmp);
    day = atoi(tmp);        
    
    if(bstate)
        listInsert( &gImportantDays, day + keybase, NULL);
    else
        listDel( &gImportantDays, day + keybase);
    
}

/*****************************************************************************
	Menu click in main window
******************************************************************************/
void OnClockClicked(TN_WIDGET* pwidget,DATA_POINTER p)
{
    int i=0;
    
    //Check for reduplicate clicking
    if(GsFindWindow(w_time->wid))
    {
        return;
    }
    
    w_time = CreateWindowFromRC( NULL, 0, ID_WND_TIME,&gClockRcinfo);
    
    btn_time[0] = CreateButtonFromRC( w_time, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_YEAR,&gClockRcinfo);
    btn_time[1] = CreateButtonFromRC( w_time, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MONTH,&gClockRcinfo);
    btn_time[2] = CreateButtonFromRC( w_time, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_DAY,&gClockRcinfo);
    btn_time[3] = CreateButtonFromRC( w_time, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_HOUR,&gClockRcinfo);
    btn_time[4] = CreateButtonFromRC( w_time, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_MINUTE,&gClockRcinfo);
	btn_world = CreateButtonFromRC( w_time, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_WORLDTIME,&gClockRcinfo);
		
	for(i = ID_LBL_TIME0;i <= ID_LBL_TIME5;i++)
	    qj_label[i] = CreateLabelFromRC(w_time,0,i,&gClockRcinfo);


    tnRegisterCallBack(btn_time[0], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_YEAR);
    tnRegisterCallBack(btn_time[1], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_MONTH);
    tnRegisterCallBack(btn_time[2], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_DAY);
    tnRegisterCallBack(btn_time[3], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_HOUR);
    tnRegisterCallBack(btn_time[4], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_MINUTE);
    tnRegisterCallBack(btn_world, WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
    
    tnRegisterCallBack( w_time, WIDGET_CLICKED, (CallBackFuncPtr)SaveClockTime, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[0], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockTime, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[1], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockTime, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[2], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockTime, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[3], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockTime, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[4], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockTime, (DATA_POINTER)button[0]);
    
    tnRegisterCallBack( w_time, WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[0], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[1], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[2], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[3], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);
    tnRegisterCallBack( btn_time[4], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);
    
    tnRegisterCallBack( btn_world, WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[0]);	
    tnRegisterCallBack( btn_world,  WIDGET_CLICKED,(CallBackFuncPtr) OnCreateWorldWindow,NULL);
    
    GrSetFocus(btn_time[0]->wid);
    
    
    time_t ticks;
    struct tm *t;
    char tmp[4];

    ticks = time(NULL);
    t = localtime(&ticks);
    strftime(tmp, 127, "%Y", t);
    tnSetButtonCaption( btn_time[0],tmp);
    t = localtime(&ticks);
    strftime(tmp, 127, "%m", t);
    tnSetButtonCaption( btn_time[1],tmp);
    strftime(tmp, 127, "%d", t);
    tnSetButtonCaption( btn_time[2],tmp);
    strftime(tmp, 127, "%H", t);
    tnSetButtonCaption( btn_time[3],tmp);
    strftime(tmp, 127, "%M", t);
    tnSetButtonCaption( btn_time[4],tmp);
    
    
}

void OnAlarmClicked(TN_WIDGET* pwidget,DATA_POINTER p)
{
    int i=0;
    char tmp[4];
    char *state;
    int num;
    
    //Check for reduplicate clicking
    if(GsFindWindow(w_alarm->wid))
    {
        return;
    }
    
    w_alarm =CreateWindowFromRC(NULL, 0, ID_WND_ALARM,&gClockRcinfo);
    
    btn_alarm[0]= CreateButtonFromRC(w_alarm, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ALARMON,&gClockRcinfo);
    btn_alarm[1]= CreateButtonFromRC(w_alarm, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ALARMTIME1,&gClockRcinfo);
    btn_alarm[2]= CreateButtonFromRC(w_alarm, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ALARMTIME2,&gClockRcinfo);    
    btn_alarm[3]= CreateButtonFromRC(w_alarm, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ALARMBRO,&gClockRcinfo);
    
    for(i = ID_LBL_ALARM0; i <= ID_LBL_ALARM3;i++)                                          
       qj_label[i] = CreateLabelFromRC( w_alarm, TN_CONTROL_FLAGS_DEFAULT, i,&gClockRcinfo);

        
    tnRegisterCallBack(btn_alarm[0], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) ALARM_STATE);
    tnRegisterCallBack(btn_alarm[1], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) ALARM_HOUR);
    tnRegisterCallBack(btn_alarm[2], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) ALARM_MINUTE);
    tnRegisterCallBack(btn_alarm[3], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) NULL);   
    
    tnRegisterCallBack( w_alarm, WIDGET_CLICKED, (CallBackFuncPtr)SaveAlarmTime, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[0], WIDGET_CLICKED, (CallBackFuncPtr)SaveAlarmTime, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[1], WIDGET_CLICKED, (CallBackFuncPtr)SaveAlarmTime, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[2], WIDGET_CLICKED, (CallBackFuncPtr)SaveAlarmTime, (DATA_POINTER)button[1]);    
    
    tnRegisterCallBack( w_alarm, WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[0], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[1], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[2], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[1]);
    tnRegisterCallBack( btn_alarm[3], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[1]);    
    
    GrSetFocus(btn_alarm[0]->wid);        
    
    state = (char *)getConfigString(gpcfgFile, "CLOCK", "AlarmState");
    tnSetButtonCaption(btn_alarm[0],state);
    
    num=getConfigInt(gpcfgFile, "CLOCK", "AlarmHour");
    sprintf(tmp,"%d",num);
    tnSetButtonCaption(btn_alarm[1],tmp);
    num=getConfigInt(gpcfgFile, "CLOCK", "AlarmMinute");
    sprintf(tmp,"%d",num);
    tnSetButtonCaption(btn_alarm[2],tmp);
 
}   
    
void OnShowClockClicked(TN_WIDGET* pwidget,DATA_POINTER p)
{
    int i=0;
    char *state;
    //Check for reduplicate clicking
    if(GsFindWindow(w_showclock->wid))
    {
        return;
    }
    
    w_showclock =CreateWindowFromRC(NULL, 0, ID_WND_CLOCK,&gClockRcinfo);
    
    btn_clock[0] = CreateButtonFromRC(w_showclock, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CLOCKON,&gClockRcinfo);
    btn_clock[1] = CreateButtonFromRC(w_showclock, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CLOCKLOCA,&gClockRcinfo);	
    
    for(i = ID_LBL_CLOCK0; i<= ID_LBL_CLOCK1; i ++)
        qj_label[i] = CreateLabelFromRC(w_showclock,0,i,&gClockRcinfo);
    
    tnRegisterCallBack(btn_clock[0], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_SHOW_STATE);
    tnRegisterCallBack(btn_clock[1], WIDGET_KEYDOWN, (CallBackFuncPtr)OnValueSetting, (DATA_POINTER) CLOCK_SHOW_LOCATE);

    tnRegisterCallBack( w_showclock, WIDGET_CLICKED, (CallBackFuncPtr)SaveClockShow, (DATA_POINTER)button[2]);
    tnRegisterCallBack( btn_clock[0], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockShow, (DATA_POINTER)button[2]);
    tnRegisterCallBack( btn_clock[1], WIDGET_CLICKED, (CallBackFuncPtr)SaveClockShow, (DATA_POINTER)button[2]);
            
    tnRegisterCallBack( w_showclock, WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[2]);
    tnRegisterCallBack( btn_clock[0], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[2]);
    tnRegisterCallBack( btn_clock[1], WIDGET_ESCAPED, (CallBackFuncPtr)OnSubMenuEscape, (DATA_POINTER)button[2]);
        
    GrSetFocus(btn_clock[0]->wid);
    
    state = (char *)getConfigString(gpcfgFile, "CLOCK", "ShowClock");
    tnSetButtonCaption(btn_clock[0],state);
    
    state=(char *)getConfigString(gpcfgFile, "CLOCK", "ClockLoc");
    tnSetButtonCaption(btn_clock[1],state);

}

void OnCalendarClicked(TN_WIDGET* pwidget,DATA_POINTER p)
{
    int i=0;
    char tempStr[128];
    
    //Check for reduplicate clicking
    //if(GsFindWindow(w_calendar->wid))
    if(GetFromRegistry(w_calendar->wid) != NULL)
    {
        printf("%s - double click %d\n",__FUNCTION__, w_calendar->wid);
        return;
    }
    
    w_calendar=CreateWindowFromRC(NULL,0, ID_WND_CALENDAR,&gClockRcinfo);    

//////////Create	Picture !!!!
    GetFullFilename(grcfileDir,"0.jpg",tempStr);
	LED_NUM[0] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"1.jpg",tempStr);
	LED_NUM[1] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"2.jpg",tempStr);
	LED_NUM[2] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"3.jpg",tempStr);
	LED_NUM[3] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"4.jpg",tempStr);
	LED_NUM[4] = GrLoadImageFromFile(tempStr,0);	
	GetFullFilename(grcfileDir,"5.jpg",tempStr);
	LED_NUM[5] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"6.jpg",tempStr);
	LED_NUM[6] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"7.jpg",tempStr);
	LED_NUM[7] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"8.jpg",tempStr);	
	LED_NUM[8] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"9.jpg",tempStr);
	LED_NUM[9] = GrLoadImageFromFile(tempStr,0);
	
    gbtnGRYear  = CreateButtonFromRC(w_calendar,0,ID_BTN_GRYEAR,&gClockRcinfo);
    gbtnGRMonth  = CreateButtonFromRC(w_calendar,0,ID_BTN_GRMONTH,&gClockRcinfo);
    gbtnGRDay  = CreateButtonFromRC(w_calendar,0,ID_BTN_GRDAY,&gClockRcinfo);
    
    gppicYear[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_YEAR1,&gClockRcinfo);	
    gppicYear[1] = CreatePictureFromRC(w_calendar,0,ID_PIC_YEAR2,&gClockRcinfo);
    gppicYear[2] = CreatePictureFromRC(w_calendar,0,ID_PIC_YEAR3,&gClockRcinfo);
    gppicYear[3] = CreatePictureFromRC(w_calendar,0,ID_PIC_YEAR4,&gClockRcinfo);
    
    gppicMonth[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_MONTH1,&gClockRcinfo);
    gppicMonth[1] = CreatePictureFromRC(w_calendar,0,ID_PIC_MONTH2,&gClockRcinfo);
    
    pic_gang[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_GANG1,&gClockRcinfo);
    pic_gang[1] = CreatePictureFromRC(w_calendar,0,ID_PIC_GANG2,&gClockRcinfo);
    
    gppicDay[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_DAY1,&gClockRcinfo);
    gppicDay[1] = CreatePictureFromRC(w_calendar,0,ID_PIC_DAY2,&gClockRcinfo);
    
    pic_hour[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_HOUR1,&gClockRcinfo);
    pic_hour[1] = CreatePictureFromRC(w_calendar,0,ID_PIC_HOUR2,&gClockRcinfo);
    
    pic_minute[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_MINUTE1,&gClockRcinfo);
    pic_minute[1] = CreatePictureFromRC(w_calendar,0,ID_PIC_MINUTE2,&gClockRcinfo);
    
    pic_dot = CreatePictureFromRC(w_calendar,0,ID_PIC_DOT1,&gClockRcinfo);
    
    pic_second[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_SEC1,&gClockRcinfo);
    pic_second[0] = CreatePictureFromRC(w_calendar,0,ID_PIC_SEC2,&gClockRcinfo);

    tnRegisterCallBack(gbtnGRYear, WIDGET_KEYDOWN, (CallBackFuncPtr)CalendarKeyDown, (DATA_POINTER)&gcalendar_year);        
    tnRegisterCallBack(gbtnGRMonth, WIDGET_KEYDOWN, (CallBackFuncPtr)CalendarKeyDown, (DATA_POINTER)&gcalendar_month);
    tnRegisterCallBack(gbtnGRDay, WIDGET_KEYDOWN, (CallBackFuncPtr)CalendarKeyDown, (DATA_POINTER)&gcalendar_day);
    tnRegisterCallBack(gbtnGRYear, WIDGET_ESCAPED, (CallBackFuncPtr)SaveTipDays, (DATA_POINTER)button[3]);
    tnRegisterCallBack(gbtnGRMonth, WIDGET_ESCAPED, (CallBackFuncPtr)SaveTipDays, (DATA_POINTER)button[3]);
    tnRegisterCallBack(gbtnGRDay, WIDGET_ESCAPED, (CallBackFuncPtr)SaveTipDays, (DATA_POINTER)button[3]);
    ///////////////////////////////////////////////////////////////////////////
    for( i = ID_LBL_CA0; i <= ID_LBL_CA6; i++)
        qj_label[i] = CreateLabelFromRC(w_calendar,0,i,&gClockRcinfo);	
    
    GetFullFilename(grcfileDir,"ch1.jpg",tempStr);
	CALENDAR_CHK[0] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"ch2.jpg",tempStr);
	CALENDAR_CHK[1] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"ch3.jpg",tempStr);
	CALENDAR_CHK[2] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"ch4.jpg",tempStr);
	CALENDAR_CHK[3] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"ch5.jpg",tempStr);
	CALENDAR_CHK[4] = GrLoadImageFromFile(tempStr,0);
	GetFullFilename(grcfileDir,"ch6.jpg",tempStr);
	CALENDAR_CHK[5] = GrLoadImageFromFile(tempStr,0);
		
    for( i = ID_CKB_CA0; i <= ID_CKB_CA34;i++)
    {
        ckb[i] = CreateCheckButtonFromRC(w_calendar,0,i,&gClockRcinfo);
        if( i%7)
            SetCheckButtonImageID (ckb[i],CALENDAR_CHK[0], CALENDAR_CHK[1], CALENDAR_CHK[4], CALENDAR_CHK[5]);
        else
            SetCheckButtonImageID (ckb[i],CALENDAR_CHK[2], CALENDAR_CHK[3], CALENDAR_CHK[4], CALENDAR_CHK[5]);
        
    }
    
    printf("calendar go on \n");    
    //qj_label[ID_LBL_CA7] = CreateLabelFromRC(w_calendar,0,ID_LBL_CA7,&gClockRcinfo);
    
    for( i = ID_CKB_CA0; i <= ID_CKB_CA34;i++)
    {        
        tnRegisterCallBack(ckb[i], WIDGET_CLICKED, (CallBackFuncPtr)TipDay, NULL);
        tnRegisterCallBack(ckb[i], WIDGET_KEYDOWN, (CallBackFuncPtr)ClockFocusExchange, NULL);
        tnRegisterCallBack(ckb[i], WIDGET_ESCAPED, (CallBackFuncPtr)SaveTipDays, (DATA_POINTER)button[3]);
    }
    
    tnRegisterCallBack( w_calendar, WIDGET_ESCAPED, (CallBackFuncPtr)SaveTipDays, (DATA_POINTER)button[3]);
        
    GrSetFocus(gbtnGRYear->wid);
    
    gGetCurrDay(&calendar_day);
    
    gcalendar_year = calendar_day.tm_year;
    gcalendar_month = calendar_day.tm_mon;    
    gcalendar_day = calendar_day.tm_mday;
    
    gcalendar_year = 2009;
    gcalendar_month = 2;    
    gcalendar_day = 2;
 
    listFree( &gImportantDays);
    gGetImportantDay( gpcfgFile, &gImportantDays);
    update_calendar();        
}

#ifdef __ECOS
void Clock_thread(CYG_ADDRWORD data)
{
    struct menuReturnData returndata;
    struct THREAD_PARAMETER *paraData = (struct THREAD_PARAMETER *) data;            
    
    INIT_PER_THREAD_DATA(paraData->thread_data_index);
    
    returndata.wFocus = (TN_WIDGET *)paraData->others;
  	returndata.prcinfo = &gClockRcinfo;  	

#else		
int main(int argc, char **argv)
{       
#endif	//end of __ECOS
    char *gSubject=NULL;
    char *gLang=NULL;
    char rcfilepath[200];	
	//TN_WIDGET *title;
		    
	//loadconfigfile
    gpcfgFile=readConfigFile(CFG_FILE_PATH);
    if(!gpcfgFile)
    {
        printf("can't load the configure file\n");
        return;
    }
	gSubject=getConfigString(gpcfgFile,"setting","subject");
	if (gSubject==NULL) return;
    
    gLang=getConfigString(gpcfgFile,"setting","language");
	
	if (gLang==NULL) return;
    
#ifdef __ECOS
//    tnControlLibInitialize();
    tnAppInitialize();    
#else
	tnAppInitialize();
    
#endif
    getRcFilePath(rcfilepath, RESOURCE_PATH, gSubject, "Clock");  
    
    if(GetPath(rcfilepath,grcfileDir) < 0) return;      
    
    if(load_rc(&gClockRcinfo,rcfilepath, "en")<0)
    {
        printf("Failed to load rc file!\n");
        return;
   	}
   	window = CreateWindowFromRC(NULL, 0, ID_WCLOCK,&gClockRcinfo);
   	wtitle = CreateLabelFromRC(window,0,ID_LBL_CLOCK,&gClockRcinfo);
   	
   	//// Create menu buttons!!!
    button[0] = CreateButtonFromRC(window, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_TIME,&gClockRcinfo);
    button[1] = CreateButtonFromRC(window, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_ALARM,&gClockRcinfo);
    button[2] = CreateButtonFromRC(window, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CLOCK,&gClockRcinfo);
    button[3] = CreateButtonFromRC(window, TN_CONTROL_FLAGS_DEFAULT, ID_BTN_CALENDAR,&gClockRcinfo);
    
    //////////////////register Callbackfunction!!!
    tnRegisterCallBack(button[0], WIDGET_CLICKED, (CallBackFuncPtr) OnClockClicked, (DATA_POINTER) 0);
    tnRegisterCallBack(button[1], WIDGET_CLICKED, (CallBackFuncPtr) OnAlarmClicked, (DATA_POINTER) 1);
    tnRegisterCallBack(button[2], WIDGET_CLICKED, (CallBackFuncPtr) OnShowClockClicked, (DATA_POINTER) 2);
    tnRegisterCallBack(button[3], WIDGET_CLICKED, (CallBackFuncPtr) OnCalendarClicked, (DATA_POINTER) 3);
    
    tnRegisterCallBack(button[0], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack(button[1], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack(button[2], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);
	tnRegisterCallBack(button[3], WIDGET_KEYDOWN, (CallBackFuncPtr)OnControlFocusExchange, NULL);	
    
	tnRegisterCallBack(button[0], WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape,(DATA_POINTER) &returndata);
	tnRegisterCallBack(button[1], WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape,(DATA_POINTER) &returndata);
	tnRegisterCallBack(button[2], WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape,(DATA_POINTER) &returndata);
	tnRegisterCallBack(button[3], WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape,(DATA_POINTER) &returndata);
	
	tnRegisterCallBack(window, WIDGET_ESCAPED, (CallBackFuncPtr)OnMenuEscape,(DATA_POINTER) &returndata);
	
	GrSetFocus(button[0]->wid);		
				
	tnMainLoop();
	return;
}
