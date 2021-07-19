#ifndef __GLOBAL_FUNC_H
#define __GLOBAL_FUNC_H
#include <cyg/kernel/kapi.h>
#include <tinywidget/tnWidgets.h>
#include <time.h> 

#define DPFKEY_PREV MWKEY_UP
#define DPFKEY_NEXT MWKEY_DOWN
#define DPFKEY_VALDOWN MWKEY_LEFT
#define DPFKEY_VALUP MWKEY_RIGHT
#define DPFKEY_PLAY  MWKEY_F1
#define DPFKEY_PAUSE  MWKEY_F2
#define DPFKEY_STOP  MWKEY_F3
#define DPFKEY_PREV_SCAN  MWKEY_F4
#define DPFKEY_NEXT_SCAN  MWKEY_F5


#define SET_BTN_VALUE_NUM(str, nCur, nPlus, nMin, nMax) \
        {                                      \
            nCur=atoi(str);                    \
            nCur = nCur+ nPlus;                \
            if(nCur < nMin) nCur = nMax;       \
            if(nCur > nMax) nCur = nMin;       \
            sprintf(str,"%d",nCur);            \
        }

#define SET_BTN_VALUE_STR(str, str_op1, str_op2)  \
        {                                     \
            if( strstr(str, str_op1 ))        \
                sprintf(str,"%s",str_op2);  \
            else                              \
                sprintf(str,"%s",str_op1);  \
        }


struct THREAD_PARAMETER
{
    cyg_ucount32 thread_data_index;
    void *others;    
};

struct menuReturnData
{
    TN_WIDGET *wFocus;
    RCINFO *prcinfo;       
};

#define APLAYER_EXIT 0
#define APLAYER_MINIMIZE 1
#define APLAYER_MAXMIZE 2
#define APLAYER_NEW 3
#define APLAYER_PLAY 4
#define APLAYER_PAUSE 5
#define APLAYER_STOP 6
#define APLAYER_NEXT 7
#define APLAYER_PREV 8
#define APLAYER_VOLUP 9
#define APLAYER_VOLDWN 10

enum DPF_EVENT_TYPE{
    DPF_AP_RETURN = 100,
    DPF_SUBJECT_UPDATE,
    DPF_LANGUAGE_UPDATE,
    DPF_APLAYER_STATE,
    DPF_ALARM_UPDATE
};

typedef struct{
    enum DPF_EVENT_TYPE type;
    char *subjecttitle;    
}DPF_EVENT_SUBJECT;

typedef struct{
    enum DPF_EVENT_TYPE type;
    char langtitle[64];
}DPF_EVENT_LANGUAGE;

typedef struct{
    enum DPF_EVENT_TYPE type;
    int state;
}DPF_EVENT_APLAYER;

typedef struct{
    enum DPF_EVENT_TYPE type;
}DPF_EVENT_ALARM;

typedef union{
    enum DPF_EVENT_TYPE type;
    DPF_EVENT_SUBJECT subject;
    DPF_EVENT_LANGUAGE language;
    DPF_EVENT_APLAYER aplayer;
    DPF_EVENT_ALARM alarm;
}DPF_EVENT;

void OnMenuEscape(TN_WIDGET* pwidget,DATA_POINTER p);
void OnSubMenuEscape(TN_WIDGET* pwidget,DATA_POINTER p);
void OnControlFocusExchange(TN_WIDGET *widget, DATA_POINTER dptr);

int gSetCurrTime ( struct timeval* tv/*, struct timezone* tz*/);
void gGetCurrDay(struct tm* ptm);
int gGetWeekDay(int year,int month,int day);
int gGetNumDaysOfMonth(int year,int month);
int FreeStrList(char **fi,int n);

typedef struct{
    unsigned int key;
    void *data;
    struct LISTITEM *next;
}LISTITEM, *PLISTITEM;

typedef struct{
    int count;
    PLISTITEM head;    
    
}LIST, *PLIST;

void listInsert( PLIST plist, unsigned int key, void *data);
void listDel( PLIST plist, unsigned int key);
PLISTITEM listSearch( PLIST plist, unsigned int key);
void listDump(PLIST plist);

extern struct configFile;
void gGetImportantDay(struct configFile *cfg, PLIST plist);
void gSetImportantDay(struct configFile *cfg, PLIST plist);

#endif //__GLOBAL_FUNC_H
