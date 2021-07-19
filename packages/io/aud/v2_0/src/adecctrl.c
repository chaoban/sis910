///////////////////////////////////////////////////////////////////////////////////////////////
//
//	ADECCTRL.C
//
///////////////////////////////////////////////////////////////////////////////////////////////
#include <cyg/infra/cyg_type.h>
#include "../include/adecctrl.h"
#include <pkgconf/io_aud.h>	// zhang sen
#include <pkgconf/devs_mad_mp3.h>
#include <stdio.h>
#include <cyg/infra/cyg_ass.h>       // Assertion support

#include <cyg/io/iis/io_iis.h>

static cyg_handle_t decoder_thread_handle;
static cyg_thread decoder_thread_object;
static AUD_tCtrlBlk *gpAudCtrl = NULL;
static AUD_tModule *gpModuleList[MAX_ADEC_NUM];

#ifdef OUTPUT_WAV
extern char outpath[];
#endif

char stack[DECODER_THREAD_STACK_SIZE];
static int frame_num = 0;
/*
  *  Interface of Audio Decoder
  */
extern AUD_tModule MAD_ModuleMP3;

//inline function defintion
#define CreateAudCtrlBlk()	({		\
	int i;		\
    for (i = 0; i < MAX_ADEC_THREADS; i++)	\
		if (gpAudCtrl[i].Tid == -1)		\
			break;		\
    (gpAudCtrl + i);		\
})

#define GetAudCtrlBlk(Tid)	({		\
	int i;		\
    for (i=0; i<MAX_ADEC_THREADS; i++) 		\
    		if (gpAudCtrl[i].Tid == Tid)	\
            break;		\
     (gpAudCtrl + i);	\
})

#define ADEC_REGISTER(dec)	do{	\
	int i;		\
	for(i = 0; i < MAX_ADEC_NUM; i++)		\
		if(NULL == gpModuleList[i]){	\
			gpModuleList[i] = dec;			\
			break;			\
		}						\
}while(0)

#define ADEC_CLEANUP()		do{		\
	int i;		\
	for(i = 0; i < MAX_ADEC_NUM; i++)		\
		if(NULL != gpModuleList[i])		\
			gpModuleList[i] = NULL;		\
}while(0)


static AUD_tModule * AudTypeToModule(AUD_tType Type);
static int CreateDecTask(AUD_tCtrlBlk *pAcb);


//********************************************************************************//
//  Audio initialization:  Called at system startup.
// ******************************************************************************//
void AUD_Init(void)
{
	int i;
	DEBUG_FUNCTION();

    gpAudCtrl = (AUD_tCtrlBlk *) malloc(MAX_ADEC_THREADS * sizeof(AUD_tCtrlBlk));
    CYG_ASSERT(gpAudCtrl, "Audio Control block Memory Alloction");

    for (i=0; i< MAX_ADEC_THREADS; i++)
    {
        gpAudCtrl[i].Tid = -1;
        gpAudCtrl[i].InitDone = -1;
    }

	ADEC_REGISTER(&MAD_ModuleMP3);
}

//
//  Audio setup: Set up some of the decoding parameters and create decoding
//  task dynamically.
//
void AUD_Create(int Tid, FILE* File, int AbvSize, AUD_tType Type)
{
 	int Status = OKAY;
    AUD_tCtrlBlk *pAcb = CreateAudCtrlBlk();

	DEBUG_FUNCTION();

#ifdef OUTPUT_WAV
	strcpy(outpath ,"/OUTPUT.WAV");
#endif

    if (!pAcb)
    {
    	Status = ERROR;
    }

    pAcb->Tid = Tid;
    pAcb->AFile = File;
    pAcb->Attr.Type = Type;
    if (pAcb->Attr.Type == AUDTYPE_INVALID)
    {
    		Status = ERROR;
    }

    if (Status == OKAY)
    {
    		pAcb->pModule = AudTypeToModule(pAcb->Attr.Type);
    }

    if ( !pAcb->pModule )
    {
    		Status = ERROR;
    }

    //Create decode thread and do decoder
    if (Status == OKAY)
    {
    	ABV_init( &pAcb->Abv, AbvSize, AbvSize );
        fread(pAcb->Abv.start, 1, AbvSize, File);
        //set up abv data
        pAcb->Abv.wr_ok = 1;
        pAcb->Abv.wrptr = pAcb->Abv.start + AbvSize;
        pAcb->Abv.end = pAcb->Abv.start + AbvSize -1;

        //set up Dbv buffer
        DBV_init(&(pAcb->Dbv), DBV_PER_CH_SIZE, DBV_PER_CH_SIZE);

        #ifdef OUTPUT_I2S
        I2S_alloc(DBV_PER_CH_SIZE);
        #endif

        Status = CreateDecTask(pAcb);
    }
    else
    {
        pAcb->Tid = -1;
    }
}

//
//  Audio Cleanup: Flush out data in PCM buffer and tear down ABV and PCM
//  buffers.  Also delete dynamically allocated decoding task.
//
void AUD_Cleanup(int Tid)
{
	AUD_tCtrlBlk *pAcb = GetAudCtrlBlk(Tid);
	AUD_tModule *pModule = pAcb->pModule;

	DEBUG_FUNCTION();

	if(pAcb->AFile != NULL)
	{
		printf("Close MP3 file\n");
		fclose(pAcb->AFile);
	}
    pModule->Cleanup(pAcb);

	ADEC_CLEANUP();
 	//destroy of decoded thread
 	cyg_thread_release(decoder_thread_handle);
 	//cyg_semaphore_destroy(&audio_decoder_sem);

    //APP_Cleanup(pAcb);

    //if (pAcb->pPrivAttr)
    //{
    //    FREE(pAcb->pPrivAttr);
    //    pAcb->pPrivAttr = NULL;
    //}

    //need to do
    //PCM_tear_down();
    //ABV_tear_down(CatId, &pAcb->Abv);
    //DBV_tear_down(&pAcb->Dbv, pAcb->DbvAllocAddr);
    pAcb->Tid = -1;

}

//
//  Decoding Task
//

void TASK_Decode(void *argv)
{
	AUD_tCtrlBlk *pAcb = (AUD_tCtrlBlk *) argv;
    cyg_tick_count_t t1 = 0, t2 = 0, t3 = 0;

    DEBUG_FUNCTION();

    t1 = cyg_current_time();

    ADEC_Init(pAcb);

    while (1)
    {

        if (ADEC_Engine(pAcb))
        {
            frame_num++;
            diag_printf("%dth frame decoded finish!!\n",frame_num);
        }
        else
        {
            AUD_Cleanup(1);

            printf("total decode time =");
            t2 = cyg_current_time();
            t3 = t2 - t1;
            tick2time(t3);

            cyg_thread_yield();
            break;
        }

    }

}

//
//  Decoder-specific initialization function will be called here.
//
void ADEC_Init(AUD_tCtrlBlk *pAcb)
{
    int i, ret = 0;
    AUD_tAttr *pAttr = &pAcb->Attr;
    AUD_tModule *pModule = pAcb->pModule;

    //need to do
    //pAcb->DbvAllocAddr = DBV_init(&(pAcb->Dbv), DBV_PER_CH_SIZE *2, DBV_PER_CH_SIZE * 2);


    pAcb->CtrlState = AUD_CTRL_STATE_IDLE;
    pAcb->FlushInState = pAcb->FlushOutState = AUD_FLUSH_STATE_IDLE;

    pAcb->pPrivAttr = NULL;
    pAcb->AbvThresh = DEF_ABV_THRESHOLD;

   // pAcb->AbvIsr = (pAcb->CatId == AUD_CAT_ID_PRI) ?
   //                     ABV_drain_pri : ABV_drain_sec;

    pAttr->NumFrames = 0;
    for (i=0; i<MAX_NUM_CHANNEL; i++)
    {
        pAttr->ChOrder[i] = -1;
    }
	//call decoder
   if( OKAY == (ret = pModule->Init(pAcb)))
    {
	    pAcb->InitDone = 1;
	}
	if (pModule->GetAttr(pAcb))
	{
        pAcb->CtrlState = AUD_CTRL_STATE_IDLE;
    }
#ifdef OUTPUT_I2S
    //init iis driver
    iis_set_value(pAttr->BitsPerSamp, pAttr->SampleFreq); //iis_set_value(bitrate, samplerate)
#endif //OUTPUT_I2S

}

//
//  Calls the decoding function. Make sure ABV is not too empty and DBV is not
//  too full before calling.
//  When audio play is done, call the cleanup function.
//
int ADEC_Engine(AUD_tCtrlBlk *pAcb)
{
	int ret = 0;
	AUD_tModule *pModule = pAcb->pModule;

    AUD_ParseCmd(pAcb);

    switch(pAcb->CtrlState)
    {
        case AUD_CTRL_STATE_DEC:
        {
            ret = pModule->Decode(pAcb);
            break;
        }

        case AUD_CTRL_STATE_IDLE:
            frame_num--;
            return 1;

        case AUD_CTRL_STATE_STOP:
            return 0;

//
//		case AUD_CTRL_STATE_ERROR:
//			break;
//
//		case AUD_CTRL_STATE_SWITCH:
//			break;

        default:
            CYG_ASSERT(0,"aud engine error");
            return -1;
    }
    return ret;
}


//
//  Create/Delete the decoding task
//
static int CreateDecTask(AUD_tCtrlBlk *pAcb)
{
	DEBUG_FUNCTION();

	cyg_thread_create(
		7,//sched_info
		TASK_Decode,//entry_func
		pAcb,//entry_data
		"Audio_Decoder_Thread",//name
		(void *)stack,//stack_base
		DECODER_THREAD_STACK_SIZE,//stack_size
		&decoder_thread_handle,//handle
		&decoder_thread_object//thread
	);
    cyg_thread_resume(decoder_thread_handle);
    return OKAY;
}


//
//  Find the appropriate audio module by the type provided//
//
static AUD_tModule * AudTypeToModule(AUD_tType Type)
{
    int i;
    for (i = 0; i < DIM(gpModuleList); i++)
    {
        if (gpModuleList[i]->Type == Type)
        {
            return gpModuleList[i];
        }
    }

    CYG_ASSERT(0, "can't find corresponding audio decoder Module");
    return NULL;
}

int AUD_ParseCmd(AUD_tCtrlBlk *pAcb)
{
    tMsg *Msg;
    int ret;
    ret = MsgQ_Rcv(AUD_PLAYER_CTRL, Msg);

    if(ret == -1)
    {
        return OKAY;
    }

    switch (Msg->MsgID)
    {
        case MSG_PRES_START:
            pAcb->CtrlState = AUD_CTRL_STATE_DEC;
            break;

        case MSG_PRES_STOP:
            pAcb->CtrlState = AUD_CTRL_STATE_STOP;
            break;

        case MSG_PRES_PAUSE:
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
            return ERROR;
    }

    return OKAY;
}
//
//  Get current playing time
//
#ifdef AUD_TIME_CONTROL
double aud_cur_time = 0.0;
double aud_total_time = 0.0;

double Get_Current_Play_time(int sample_num, int sample_rate)
{
	double tmp;

	tmp = (double)((double)sample_num / (double)sample_rate);

	aud_cur_time += tmp;

	return aud_cur_time;
}
#endif

#define TICKS_PER_SECOND	100
void tick2time(cyg_tick_count_t t)
{
    cyg_uint32 totalseconds = t/TICKS_PER_SECOND;
    cyg_uint32 tempseconds = totalseconds;
    cyg_uint32 day = 0;
    cyg_uint32 hour = 0;
    cyg_uint32 minute = 0;
    cyg_uint32 sec = 0;
    cyg_uint32 msec = 0;
    if(tempseconds < 0)
    {
        diag_printf("Error tick is a minus number!\n");
    }

	/* DAY */
    if(tempseconds >= 86400){
		day = (tempseconds / 86400);
		tempseconds -= (day * 86400);
    }
    /* HOUR */
    if(tempseconds >= 3600){
		hour = (tempseconds/3600);
		tempseconds -= (hour*3600);
    }
    /* MINUTE */
    if(tempseconds >= 60){
		minute = (tempseconds/60);
		tempseconds -= (minute*60);
    }
    /* SECOND */
    sec = tempseconds;

    /* 1/100 SECOND */
    msec = t%TICKS_PER_SECOND;

    if(day > 0)
    {
        diag_printf(" %d day %d hour %d min %d.%02d sec.\n", day, hour, minute, sec, msec);
    }
    else if(hour > 0)
    {
        diag_printf(" %d hour %d min %d.%02d sec.\n", hour, minute, sec, msec);
    }
    else if(minute > 0)
    {
        diag_printf(" %d min %d.%02d sec.\n", minute, sec, msec);
    }
    else
    {
        diag_printf(" %d.%02d sec.\n", sec, msec);
    }
}