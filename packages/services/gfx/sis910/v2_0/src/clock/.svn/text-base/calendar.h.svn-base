#ifndef __CALENDAR_H__
	#define __CALENDAR_H__
//#include <time.h>
//#include <sys/time.h>

void click_calendar(TN_WIDGET* pwidget,DATA_POINTER p);
void keydown_calendar(TN_WIDGET* pwidget,DATA_POINTER p);

void draw_calendar();
void timer_calendar();

void calendar_init();

void draw_dataLED();
void draw_timeLED();

int GetWeekDay(int year,int month,int day);
struct tm* GetCurrDay();
int GetNumDaysOfMonth(struct tm* ptm);

void yearadd_func();
void yearsub_func();
void monthadd_func();
void monthsub_func();

#endif	//end of __CALENDAR_H__
