#include<stdio.h>
#include "tinywidget/tnWidgets.h"
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "../config/config.h"

extern struct tm *show_day,*now_day;
extern TN_WIDGET *window,*w_time,*w_alarm,*w_showclock,*w_calendar;
extern	TN_WIDGET *button[4];
extern TN_WIDGET *qj_label[57];
extern TN_WIDGET *ckb[37];
extern TN_WIDGET *gppicYear[4],*pic_gang[2],*gppicMonth[2],*gppicDay[2];
extern	TN_WIDGET *pic_hour[2],*pic_dot,*pic_minute[2],*pic_second[2];


#include "calendar.h"


void keydown_calendar(TN_WIDGET* pwidget,DATA_POINTER p)
{
    long ch = (long)pwidget->data;
    TN_WIDGET *pw;
    if (ch == MWKEY_BACKSPACE)
    {
        Write_ImportantDay();
        GrMapWindow(window->wid);
        GrSetFocus(button[0]->wid);
        GrUnmapWindow(w_calendar->wid);
        return;
    }
    
    if (ch == MWKEY_LEFT)
    {	
        if(pwidget == ckb[0])
            pw = ckb[36];
        else
            pw = GetPrevSibling(pwidget);
        GrSetFocus(pw->wid);	
        return;
    }
    
    if (ch == MWKEY_RIGHT)
    {
        if(pwidget == ckb[36])
            pw =ckb[0];
        else
            pw = GetNextSibling(pwidget);
        GrSetFocus(pw->wid);	
        return;
    }	
    
    if (ch == MWKEY_UP)
    {	
        monthadd_func();
        return;
    }
    
    if (ch == MWKEY_DOWN)
    {
        monthsub_func();
        return;
    }

}




void click_calendar(TN_WIDGET* pwidget,DATA_POINTER p)
{
    GR_BOOL state;
    tnGetCheckButtonStatus(pwidget,&state);
    if(state)
    tnSetCheckButtonStatus(pwidget,GR_FALSE);
    else
    tnSetCheckButtonStatus(pwidget,GR_TRUE);
    
}


int Read_ImportantDay()
{
    struct configFile *day_cfg;
    char *All_important,*This_important;
    int year_num,month_num,day_num;
    int first_weekday = GetWeekDay(show_day->tm_year+1900,show_day->tm_mon+1,1);  

    day_cfg = readConfigFile(CFG_PATH);
    if(!day_cfg)
    {
        printf("can not open confige file!!\n");
        return -1;
    }
    All_important = getConfigString(day_cfg, "CLOCK", "ImportantDay");
    if(!All_important)                    
        return  -1;
           
    This_important = strtok(All_important,",");
    sscanf(This_important,"%d/%d/%d",&year_num,&month_num,&day_num);
    if(month_num>12 || month_num<0 || day_num>31 ||day_num<0)
    {
        printf("bad data format!\n");
        return  -1;
    }               
    if((year_num==show_day->tm_year+1900) && (month_num==show_day->tm_mon+1))
        tnSetCheckButtonStatus(ckb[day_num+first_weekday-1],1);
    
    while(This_important = strtok(NULL,","))
    {
        sscanf(This_important,"%d/%d/%d",&year_num,&month_num,&day_num);
        if(month_num>12 || month_num<0 || day_num>31 ||day_num<0)
        {
            printf("bad data format!\n");
            return  -1;
        }               
        if((year_num==show_day->tm_year+1900) && (month_num==show_day->tm_mon+1))
            tnSetCheckButtonStatus(ckb[day_num+first_weekday-1],1);
    }
    unloadConfigFile(day_cfg);
    return 1;
}

int Write_ImportantDay()
{
    struct configFile *day_cfg;
    //char *All_important,*This_important;
    char All_important[128]="";
    int year_num,month_num,day_num;
    GR_BOOL is;
               
    year_num = show_day->tm_year+1900;
    month_num=show_day->tm_mon+1;
    int first_weekday = GetWeekDay(year_num,month_num,1);  
    int i;
    int n=GetNumDaysOfMonth(show_day);
    day_cfg = readConfigFile(CFG_PATH);
	if(!day_cfg)
	{
	    printf("can not open confige file!!\n");
	    return -1;
	}
       
    for(i=1;i<=n;i++)
    {
        tnGetCheckButtonStatus(ckb[i+first_weekday-1], &is);
        if(is)           
        {
            char v[32];
            sprintf(v,"%d/%d/%d,",year_num,month_num,i);
            strcat(All_important,v);
        }              
    }
    setConfigString(day_cfg, "CLOCK", "ImportantDay", All_important);
    saveConfigFile(day_cfg,CFG_PATH);
    unloadConfigFile(day_cfg);
    
}
		
void calendar_init()
{
    show_day = (struct tm*)malloc(sizeof(struct tm));
    now_day = GetCurrDay();
    memcpy(show_day,now_day,sizeof(struct tm));

}

void draw_calendar()
{
    int nday=GetNumDaysOfMonth(show_day);
    int first_weekday = GetWeekDay(show_day->tm_year+1900,show_day->tm_mon+1,1);
    int i=0;
    for(i=0;i<=36;i++)
    {
        tnSetCheckButtonCaption(ckb[i]," ");
        tnSetCheckButtonStatus(ckb[i],0);
    }
    for(i=1;i<=nday;i++)
    {
        char str[4];
        sprintf(str,"%d",i);
        tnSetCheckButtonCaption(ckb[i+first_weekday-1],str);						
    }
    	
    Read_ImportantDay();
    draw_dataLED();
    draw_timeLED();
}


void timer_calendar()
{
    now_day = GetCurrDay();
    draw_timeLED();
    //free(now_day);
}


void draw_dataLED()
{
    char pic_year1_str[128],pic_year2_str[128],pic_year3_str[128],pic_year4_str[128];
    char pic_day1_str[128],pic_day2_str[128];
    char pic_month1_str[128],pic_month2_str[128];
#ifdef __ECOS
    //sprintf(pic_year1_str,"/resource/Green/clock/%d.bmp",( show_day->tm_year+1900)/1000);
    //sprintf(pic_year2_str,"/resource/Green/clock/%d.bmp",((show_day->tm_year+1900)/100)%10);
    //sprintf(pic_year3_str,"/resource/Green/clock/%d.bmp",((show_day->tm_year+1900)/10)%10);
    //sprintf(pic_year4_str,"/resource/Green/clock/%d.bmp",(show_day->tm_year+1900)%10);
    //
    //sprintf(pic_month1_str,"/resource/Green/clock/%d.bmp",(show_day->tm_mon+1)/10);
    //sprintf(pic_month2_str,"/resource/Green/clock/%d.bmp",(show_day->tm_mon+1)%10);
    //
    //sprintf(pic_day1_str,"/resource/Green/clock/%d.bmp",show_day->tm_mday/10);
    //sprintf(pic_day2_str,"/resource/Green/clock/%d.bmp",show_day->tm_mday%10);
    sprintf(pic_year1_str,"/romdisk/resource/800x480/Clock/%d.jpg",( show_day->tm_year+1900)/1000);
    sprintf(pic_year2_str,"/romdisk/resource/800x480/Clock/%d.jpg",((show_day->tm_year+1900)/100)%10);
    sprintf(pic_year3_str,"/romdisk/resource/800x480/Clock/%d.jpg",((show_day->tm_year+1900)/10)%10);
    sprintf(pic_year4_str,"/romdisk/resource/800x480/Clock/%d.jpg",(show_day->tm_year+1900)%10);
    
    sprintf(pic_month1_str,"/romdisk/resource/800x480/Clock/%d.jpg",(show_day->tm_mon+1)/10);
    sprintf(pic_month2_str,"/romdisk/resource/800x480/Clock/%d.jpg",(show_day->tm_mon+1)%10);
    
    sprintf(pic_day1_str,"/romdisk/resource/800x480/Clock/%d.jpg",show_day->tm_mday/10);
    sprintf(pic_day2_str,"/romdisk/resource/800x480/Clock/%d.jpg",show_day->tm_mday%10);
#else
    sprintf(pic_year1_str,"/num/%d.bmp",( show_day->tm_year+1900)/1000);
    sprintf(pic_year2_str,"/num/%d.bmp",((show_day->tm_year+1900)/100)%10);
    sprintf(pic_year3_str,"/num/%d.bmp",((show_day->tm_year+1900)/10)%10);
    sprintf(pic_year4_str,"/num/%d.bmp",(show_day->tm_year+1900)%10);
    
    sprintf(pic_month1_str,"/num/%d.bmp",(show_day->tm_mon+1)/10);
    sprintf(pic_month2_str,"/num/%d.bmp",(show_day->tm_mon+1)%10);
    
    sprintf(pic_day1_str,"/num/%d.bmp",show_day->tm_mday/10);
    sprintf(pic_day2_str,"/num/%d.bmp",show_day->tm_mday%10);
#endif
    ///year
    tnSetPicture(gppicYear[0],pic_year1_str);
    tnSetPicture(gppicYear[1],pic_year2_str);
    tnSetPicture(gppicYear[2],pic_year3_str);
    tnSetPicture(gppicYear[3],pic_year4_str);
    ///month		
    tnSetPicture(gppicMonth[0],pic_month1_str);
    tnSetPicture(gppicMonth[1],pic_month2_str);	
    ////day
    tnSetPicture(gppicDay[0],pic_day1_str);
    tnSetPicture(gppicDay[1],pic_day2_str);
	
}

void draw_timeLED()
{
    char pic_hour1_str[128],pic_hour2_str[128];
    char pic_min1_str[128] ,pic_min2_str[128];
    char pic_sec1_str[128] ,pic_sec2_str[128];
#ifdef __ECOS
    sprintf(pic_hour1_str,"/romdisk/resource/800x480/Clock/%d.jpg",now_day->tm_hour/10);
    sprintf(pic_hour2_str,"/romdisk/resource/800x480/Clock/%d.jpg",now_day->tm_hour%10);
    
    sprintf(pic_min1_str,"/romdisk/resource/800x480/Clock/%d.jpg",now_day->tm_min/10);
    sprintf(pic_min2_str,"/romdisk/resource/800x480/Clock/%d.jpg",now_day->tm_min%10);
    
    sprintf(pic_sec1_str,"/romdisk/resource/800x480/Clock/%d.jpg",now_day->tm_sec/10);
    sprintf(pic_sec2_str,"/romdisk/resource/800x480/Clock/%d.jpg",now_day->tm_sec%10);
#else
    sprintf(pic_hour1_str,"/num/%d.bmp",now_day->tm_hour/10);
    sprintf(pic_hour2_str,"/num/%d.bmp",now_day->tm_hour%10);
    
    sprintf(pic_min1_str,"/num/%d.bmp",now_day->tm_min/10);
    sprintf(pic_min2_str,"/num/%d.bmp",now_day->tm_min%10);
    
    sprintf(pic_sec1_str,"/num/%d.bmp",now_day->tm_sec/10);
    sprintf(pic_sec2_str,"/num/%d.bmp",now_day->tm_sec%10);
#endif	
    tnSetPicture(pic_hour[0],pic_hour1_str);
    tnSetPicture(pic_hour[1],pic_hour2_str);
    tnSetPicture(pic_minute[0],pic_min1_str);
    tnSetPicture(pic_minute[1],pic_min2_str);
    tnSetPicture(pic_second[0],pic_sec1_str);
    tnSetPicture(pic_second[1],pic_sec2_str);
	
}



int GetWeekDay(int year,int month,int day)

{

    int j,count=0;
    //  int Day_Index;
    //  int days[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    int MonthAdd[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
    count = MonthAdd[month-1];
    
    count = count + day;
    
    if(( (year%4 == 0 && year %100 != 0) || (year % 400 == 0)) && month >= 3)
        count += 1;
    count = count + (year - 1901) * 365;
    for(j = 1901;j < year;j++)
    {
        if( (j % 4 == 0 && j % 100 != 0) || (j % 400 == 0)	)
            count++;
    }
    return ((count+1) % 7);
}

struct tm* GetCurrDay()
{
    /////time struct
    struct timeval tv;
    struct timezone tz;
    time_t now;
    
    gettimeofday(&tv, &tz);
#ifdef __ECOS
    now = tv.tv_sec;
#else   
    now = tv.tv_sec - (60 * tz.tz_minuteswest);
#endif
    now_day = gmtime(&now);
    
    return now_day;
}

char* GetCurrDayStr()
{
    /////time struct
    struct timeval tv;
    struct timezone tz;
    struct tm *tp;
    time_t now;

    char *CurrDate=(char*)malloc(256*sizeof(char));	

    gettimeofday(&tv, &tz);
#ifdef __ECOS
    now = tv.tv_sec;
#else
    now = tv.tv_sec - (60 * tz.tz_minuteswest);
#endif
    tp = gmtime(&now);
    
    char *t=ctime(&now);
    strcpy(CurrDate,t);
    		
    return CurrDate;
    		
}


int GetNumDaysOfMonth(struct tm* ptm)
{	
    //int m=ptm->tm_mon;
    //ptm->tm_mon++;
    int m=ptm->tm_mon+1;
    if( m ==1 || m==3 || m==5 || m==7 ||
        m==8 || m==10 || m==12)
	    return 31;
	else if(m==4 || m==6 || m==9 || m==11)
	    return 30;
	else if((ptm->tm_year+1900)%4 == 0)
	    return 29;
	else return 28;

}


////////////////////////////////////////////////////////////////////////////////
///
//
//							CallBackFunction!!!!!
//


void yearadd_func()
{	show_day->tm_year++;
	draw_calendar();
				
}

void yearsub_func()
{	show_day->tm_year--;
	draw_calendar();
}

void monthadd_func()
{		show_day->tm_mon++;
		if(show_day->tm_mon > 11)
			show_day->tm_mon -=12;
		draw_calendar();
}

void monthsub_func()
{		show_day->tm_mon--;
		if(show_day->tm_mon < 0)
			show_day->tm_mon +=12;
		draw_calendar();
}

 //							CallBackFunction END!!!
///////////////////////////////////////////////////////////////////////////////////


