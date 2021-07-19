#ifndef	__CLOCK_EVENT_H__
	#define __CLOCK_EVENT_H__
void button_keydown(TN_WIDGET* pwidget,DATA_POINTER p);
void OnClicked(TN_WIDGET* pwidget,DATA_POINTER p);

void keydown_world(TN_WIDGET* pwidget,DATA_POINTER p);

void keydown_AlarmTone(TN_WIDGET* pwidget,DATA_POINTER p);

int Write_AlarmConfig();
int Read_AlarmConfig();

int Write_ClockConfig();
int Read_ClockConfig();

int SetSystemTimeData();
int GetSysTimeData();

#endif	//end of __CLOCK_EVENT_H__
