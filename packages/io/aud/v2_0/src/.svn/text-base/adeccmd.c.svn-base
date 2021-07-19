#include <cyg/kernel/kapi.h>
#include <pkgconf/io_aud.h>
#include <stdlib.h>

#include "../include/adecctrl.h"
#include "../include/adeccmd.h"
	

tMsg_Queue gpCmdq[MAX_QUEUE_NUM];
unsigned  MsgID; 

#ifndef ECOS_MSG_API
#define Q_HEAD	0
#define Q_TAIL		(MAX_QUEUE_SIZE - 1)
#define MSG_COPY(dst, src)		do{	\
	dst.MsgID = src.MsgID;		\
	dst.Para1 = src.Para1;		\
	dst.Para2 = src.Para2;		\
	dst.Para3 = src.Para3;		\
}while(0)
#endif

int MsgQ_Create(unsigned id)
{
	if( id < 0 || id >= MAX_QUEUE_NUM)
		return -1;
    MsgID = -1;
    return 0;

#ifdef ECOS_MSG_API	
	cyg_mbox_create(&gpCmdq[id].handle, &gpCmdq[id].mbox);	
#else
	gpCmdq[id].msg = (tMsg *)malloc(MAX_QUEUE_SIZE * sizeof(tMsg));
	if( !gpCmdq[id].msg )
		return -1;
	gpCmdq[id].fullness = 0;
	gpCmdq[id].empty = 1;
	gpCmdq[id].sem = 0;
#endif
	return 0;
}

void MsgQ_Delete(unsigned id)
{
#ifdef ECOS_MSG_API
//	cyg_mbox_delete(gpCmdq[id].handle);
#else
	if( gpCmdq[id].msg )
		free(gpCmdq[id].msg);
#endif
}

int MsgQ_Send(unsigned id, tMsg Msg)
{
    MsgID = Msg.MsgID;
    return 0;

#ifdef ECOS_MSG_API
	if( cyg_mbox_put(gpCmdq[id].handle, (void *)&Msg) == false )
	{
		printf("cyg_mbox_put false!\n");
		return -1;
	}
#else
	int i;

	if(gpCmdq[id].fullness >= MAX_QUEUE_SIZE)		// msg queue is full
		return -1;
	if(gpCmdq[id].sem)
		return -1;

	gpCmdq[id].sem = 1;
	
	if(gpCmdq[id].empty == 1){		// empty
		MSG_COPY(gpCmdq[id].msg[Q_TAIL], Msg);
		gpCmdq[id].empty = 0;
		gpCmdq[id].fullness++;
	}
	else if( gpCmdq[id].empty == 0 ){		// not empty
		for(i = Q_HEAD; i < Q_TAIL; i++)
			MSG_COPY(gpCmdq[id].msg[i], gpCmdq[id].msg[i+1]);
		MSG_COPY(gpCmdq[id].msg[Q_TAIL], Msg);
		gpCmdq[id].fullness++;
	}

	gpCmdq[id].sem = 0;
#endif

	return 0;
}

int MsgQ_Rcv(unsigned id, tMsg *Msg)
{
    int i;
    for(i = 0;i < 10;i++)
    {
        if(MsgID == -1)
        {
            cyg_thread_delay(10);
        }
        else
        {
            Msg->MsgID = MsgID;
            break;
        }
    }
	if( MsgID == -1 )
	{
		return -1;
	}
	else
	{
        MsgID = -1;
        return 1;
	}    
#ifdef ECOS_MSG_API
    tMsg *tmpMsg = NULL;
//    tmpMsg = (tMsg *)cyg_mbox_get(gpCmdq[id].handle);

    tmpMsg = (tMsg *)cyg_mbox_timed_get(gpCmdq[id].handle,1000);
	if( NULL == tmpMsg )
	{
	    printf("Msg NULL\n");
		return -1;
	}
    Msg->MsgID = tmpMsg->MsgID;
    Msg->Para1 = tmpMsg->Para1;
    Msg->Para2 = tmpMsg->Para2;
    Msg->Para3 = tmpMsg->Para3;
#else
	if( gpCmdq[id].empty == 1 )			// msg queue is empty
		return -1;
	if( gpCmdq[id].sem )
		return -1;

	gpCmdq[id].sem = 1;
	
	if( gpCmdq[id].fullness >= MAX_QUEUE_SIZE ){
		Msg->MsgID = gpCmdq[id].msg[Q_HEAD].MsgID;
		Msg->Para1 = gpCmdq[id].msg[Q_HEAD].Para1;
		Msg->Para2 = gpCmdq[id].msg[Q_HEAD].Para2;
		Msg->Para3 = gpCmdq[id].msg[Q_HEAD].Para3;
		gpCmdq[id].fullness--;
	}
	else{
		Msg->MsgID = gpCmdq[id].msg[MAX_QUEUE_SIZE - gpCmdq[id].fullness].MsgID;
		Msg->Para1 = gpCmdq[id].msg[MAX_QUEUE_SIZE - gpCmdq[id].fullness].Para1;
		Msg->Para2 = gpCmdq[id].msg[MAX_QUEUE_SIZE - gpCmdq[id].fullness].Para2;
		Msg->Para3 = gpCmdq[id].msg[MAX_QUEUE_SIZE - gpCmdq[id].fullness].Para3;
		gpCmdq[id].fullness--;
		if( 0 == gpCmdq[id].fullness )
			gpCmdq[id].empty = 1;
	}

	gpCmdq[id].sem = 0;
#endif
	return 1;
}

#if 0
int AUD_ParseCmd(AUD_tCtrlBlk *pAcb)
{
    

    switch (Msg.MsgID)
    {
        case MSG_PRES_START:
            pAcb->CtrlState = AUD_CTRL_STATE_DEC;
            break;

        case MSG_PRES_STOP:
            pAcb->CtrlState = AUD_CTRL_STATE_IDLE;            
            break;

        case MSG_PRES_INFLUSH:
            pAcb->FlushInState = AUD_FLUSH_STATE_ACTIVE;
            pAcb->AbvThresh = 0;
            break;

        case MSG_PRES_OUTFLUSH:
            pAcb->FlushOutState = AUD_FLUSH_STATE_ACTIVE;            
            break;

        case MSG_PRES_SET_TIMEBASE:
        default:
            assert(0);
            return ERROR;
    }
   
    return OKAY;
}

#endif
