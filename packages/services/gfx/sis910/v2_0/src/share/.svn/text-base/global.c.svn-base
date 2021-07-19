#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include <time.h>               
#include <sys/time.h> 


/*****************************************************************************
	Menu escape in main window
******************************************************************************/
void OnMenuEscape(TN_WIDGET* pwidget,DATA_POINTER p)
{
    struct menuReturnData *data = NULL;
    
    TN_WIDGET *parent = NULL, *focus = NULL;
    RCINFO *prcinfo = NULL;    
    
    if(pwidget->type == TN_WINDOW)
        parent = pwidget;
    else
        parent=GetParent(pwidget);

    tnDestroyWidget(parent);
    printf("OnMenuEscape\n");
    
    if (p != NULL)
    {
        data = (struct menuReturnData *) p;
        focus = data->wFocus;
        
        if(focus != NULL)
        {            
            if ((focus->type==TN_BUTTON) &&(!focus->enabled))
                focus->enabled=GR_TRUE;        
            GrSetFocus(focus->wid);
        }
                
        printf("OnMenuEscape p\n");
                
        prcinfo = data->prcinfo;        
        
        if(prcinfo != NULL)
        {            
            tnEndApp(prcinfo);//Must be excuted last.
        }        

    }
}

void OnSubMenuEscape(TN_WIDGET* pwidget,DATA_POINTER p)
{
    TN_WIDGET *focus = NULL,*wnd = NULL;
    
    focus=(TN_WIDGET *)p;
    
    //Check for reduplicate clicking
    //if(GsFindWindow(pwidget->wid) == NULL)
    //{
    //    printf("double clicking OnSubMenuEscape \n");
    //    return;
    //}
    
    if (pwidget->type==TN_WINDOW)
        wnd=pwidget;
    else
        wnd=GetParent(pwidget);
        
    tnDestroyWidget(wnd);    
        
    if ((focus->type==TN_BUTTON) &&(!focus->enabled)) 
        focus->enabled=GR_TRUE;
        
    GrSetFocus(focus->wid);
}

/*****************************************************************************
	button focus exchange in main window
******************************************************************************/	
void OnControlFocusExchange(TN_WIDGET *widget, DATA_POINTER dptr)
{
    TN_WIDGET *focus=widget;
    GR_KEY ch;
    ch=(long)widget->data;
    switch (ch)
    {
        case DPFKEY_PREV:
        case DPFKEY_VALDOWN:
            do
                focus=GetPrevSibling(focus);
            while(focus->type!=TN_BUTTON);
            GrSetFocus(focus->wid);                   
            break;
        case DPFKEY_NEXT:
        case DPFKEY_VALUP:
            do
                focus=GetNextSibling(focus);
            while (focus->type!=TN_BUTTON);
            GrSetFocus(focus->wid);
            break;                      
    }
}



int gSetCurrTime( struct timeval* tv/*, struct timezone* tz*/)
{
    cyg_libc_time_settime(tv->tv_sec);
    return  1;
}

void gGetCurrDay(struct tm* cur_day)
{
    /////time struct
    struct timeval tv;
    struct timezone tz;
    time_t now;    
    struct tm* test;
    char date_b[128];
    
    gettimeofday(&tv, &tz);
#ifdef __ECOS
    now = tv.tv_sec;
#else   
    now = tv.tv_sec - (60 * tz.tz_minuteswest);
#endif
    test = gmtime(&now);
    
        
    now = time(NULL);   
    test = localtime(&now);
    
    memcpy(cur_day,test,sizeof(struct tm));
    
    time_t ticks;
    struct tm *t;

    //strftime(date_b, 127, "%Y-%m-%d", cur_day);
    //printf("todate date =%s\n", date_b);
    //printf("gGetCurrDay year %d, month %d, day %d\n",cur_day->tm_year,  cur_day->tm_mon,cur_day->tm_mday);

}  

int gGetWeekDay(int year,int month,int day)
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

//int gGetNumDaysOfMonth(struct tm* ptm)
//{	
//    //int m=ptm->tm_mon;
//    //ptm->tm_mon++;
//    int m=ptm->tm_mon+1;
//    if( m ==1 || m==3 || m==5 || m==7 ||
//        m==8 || m==10 || m==12)
//	    return 31;
//	else if(m==4 || m==6 || m==9 || m==11)
//	    return 30;
//	else if((ptm->tm_year+1900)%4 == 0)
//	    return 29;
//	else return 28;
//
//}

int gGetNumDaysOfMonth(int year,int month)
{	
    if( month ==1 || month==3 || month==5 || month==7 ||
        month==8 || month==10 || month==12)
	    return 31;
	else if(month==4 || month==6 || month==9 || month==11)
	    return 30;
	else if(year%4 == 0)
	    return 29;
	else return 28;

}

void listInsert( PLIST plist, unsigned int key, void *data)
{
    PLISTITEM item = plist->head;
    PLISTITEM previtem = plist->head;
    PLISTITEM newitem;
    int nIndex = 0;
    
    newitem = (PLISTITEM) malloc(sizeof(LISTITEM));
    if(newitem == NULL)
    {
        printf("%s - malloc fail", __FUNCTION__);
        return;
    }
    newitem->key = key;
    newitem->data = data;
    
    if(plist->count)
    {
        while(item != NULL)
        {
            if(item->key > key) break;
            else if(item->key == key)
            {
                free(newitem); 
                return;
            }
            else
            {
                previtem = item;
                item = item->next;
                nIndex ++;                
            }
        }
    }
    
    newitem->next = item;
    
    if(nIndex)
        previtem->next = newitem;
    else
        plist->head = newitem;
        
    plist->count++;

}


void listDel( PLIST plist, unsigned int key)
{
    PLISTITEM item = plist->head;
    PLISTITEM previtem = plist->head;
    PLISTITEM newitem;
    int nindex = 0;
    
    if(plist->count)
    {
        while(item != NULL)
        {
            if(item->key == key) break;
            else
            {
                previtem = item;
                item = item->next;
                nindex ++;                
            }
        }
    }
    
    if(item != NULL)
    {
        if(nindex == 0)
            plist->head = item->next;
        else
            previtem->next = item->next;
        plist->count --;
        free(item);
    }
}

PLISTITEM listSearch( PLIST plist, unsigned int key)
{
    PLISTITEM item = plist->head;
    PLISTITEM previtem = plist->head;
    PLISTITEM newitem;
    bool found = false;
    
    if(plist->count)
    {
        while(item != NULL)
        {
            if(item->key == key)
            {
                found = true;
                break;
            }
            else
            {
                previtem = item;
                item = item->next;                
            }
        }
    }
    
    //if(found)
        return item;
    //else
    //    return NULL;
}

void listFree(PLIST plist)
{
    PLISTITEM item = plist->head;
    PLISTITEM nextitem;    

    if(plist->count)
    {
        while(item != NULL)
        {
            nextitem = item->next;
            free(item);
            item = nextitem;
        }
    }
    
    plist->head = NULL;
    plist->count = 0;
}

void listDump(PLIST plist)
{
    PLISTITEM item = plist->head;    

    if(plist->count)
    {
        while(item != NULL)
        {
            printf("%s - %d\n", __FUNCTION__, item->key);
            item = item->next;
        }
    }
}

void gGetImportantDay(struct configFile *cfg, PLIST plist)
{    
    char *tmp;
    char all[128];
    unsigned int day;    

    tmp = getConfigString(cfg, "CLOCK", "ImportantDay");    
    if(!tmp)                    
        return  -1;
    
    strcpy(all, tmp);
    printf("%s - %s\n", __FUNCTION__, all);
    tmp = strtok(all,",");
    while(tmp!=NULL)
    {        
        day = atoi(tmp);
        listInsert( plist, day, NULL);
        tmp=strtok(NULL,",");
    }   
    
    return 1;
}

void gSetImportantDay(struct configFile *cfg, PLIST plist)
{   
    int i = 0; 
    char All_important[128] ="";
    unsigned int day;
    PLISTITEM item = plist->head;    

    for(;i < plist->count; i++)
    {
        char v[32];
        sprintf(v,"%4d,",item->key);
        strcat(All_important,v);
        
        item = item->next;
    }
    
    setConfigString(cfg,"CLOCK","ImportantDay",All_important);        
    printf("%s - %s\n", __FUNCTION__, All_important);
    
}

int FreeStrList(char **fi,int n)
{
    int i=0;
    for(;i<n;i++)
        free(	fi[i]	);
    
    free(fi);    
    return 0;
}