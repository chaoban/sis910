/**
 *\file decoder.c
 *\brief it is a simulator of audio decoder. This simulator can work like a decoder. It have some status -- pause, stop, play-- just like
 a really decoder does. It prints the file name which is now decoded. It offers a set of API discribing in share/interface.h, liking a real
 decoder does.
 *\author yifj
 *\date 2008-04-07
 *
 *$Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "engine.h"
#include "handle.h"
#include "decoder.h"
#include "assert.h"
#include "../config/config.h"
#include "../desktop/global.h"

#ifdef __ECOS
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
static void decoder_function(CYG_ADDRWORD data);
static cyg_handle_t decoder_handle;
static cyg_thread decoder_thread;
static unsigned char decoder_stack[STACKSIZE];
static cyg_mutex_t mutex1;
#define DECODE_LEN 5*5
#else
#include <pthread.h>
static void *decoder_function();
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
#define DECODE_LEN 5*300
#endif // __ECOS

#include"dirent.h"
#include"sys/stat.h"
//#include"signal.h"
#include"unistd.h"

#define SLEEP_TIME 1

static int decoder_ready = 0, count_time = 0, destroy_decode = 0;
static int volatile set_flag = 0;
tAudSatusType decode_status = AUD_STATUS_STOP;
tAudCmdType last_cmd = AUD_CMD_STOP;
char *file_path = NULL;

/**
 *\brief get the value of set_flag.
 the set_flag == 1 : the last_cmd which indicate the lastest received cmd should be executed.
 the set_flag == 0 : there is no need to execute any command.
 *\author yifj

 *\date 2008-04-07
 *\return the value of set_flag
 */
static int get_the_flag()
{
    int value;
#ifdef __ECOS
    cyg_mutex_lock(&mutex1);
#else
    pthread_mutex_lock(&mutex1);
#endif //__ECOS
    value = set_flag;
#ifdef __ECOS
    cyg_mutex_unlock(&mutex1);
#else
    pthread_mutex_unlock(&mutex1);
#endif //__ECOS
    return value;
}

/**
 *\brief set the value of set_flag. 
 *\author yifj
 *\param value 1:set 0:unset
 *\date 2008-04-07
 *\return 1
 */
static int set_the_flag(int value)
{
#ifdef __ECOS
    cyg_mutex_lock(&mutex1);
#else
    pthread_mutex_lock(&mutex1);
#endif //__ECOS
    set_flag = value;
#ifdef __ECOS
    cyg_mutex_unlock(&mutex1);
#else
    pthread_mutex_unlock(&mutex1);
#endif //__ECOS
    return 1;
}

/**
 *\brief the decoder processing.
 the decoder have there states -- play, pause and stop. The decoder performed as the state and check the set_flag periodly to find out whether
 there is any command need to be executed.
 *\author yifj
 *\param data reserved for future using in ECOS condition.
 *\date 2008-04-07
 *\return 
 */
#ifdef __ECOS
static void decoder_function(CYG_ADDRWORD data)
#else
static void *decoder_function()
#endif //__ECOS
{
    int i;
    DPRINTF(("decoder created! \n"));
    count_time = DECODE_LEN;

    while (count_time > 0) {
#ifdef __ECOS
        cyg_thread_delay(40);
#else
        ApRelinquish();
#endif //__ECOS
        //DPRINTF(("I got here!! \n"));
        if (get_the_flag() == 1) {
            switch (last_cmd) {
                case AUD_CMD_PLAY:
                    decode_status = AUD_STATUS_PLAY;
                    //set_the_flag(0);
                    goto play_flag;
                    break;
                case AUD_CMD_CONTINUE:
                    decode_status = AUD_STATUS_PLAY;
                    //set_the_flag(0);
                    goto play_flag;
                    break;
                case AUD_CMD_PAUSE:
                    decode_status = AUD_STATUS_PAUSE;
                    //set_the_flag(0);
                    goto pause_flag;
                    break;
                case AUD_CMD_STOP:
                    decode_status = AUD_STATUS_STOP;
                    set_the_flag(0);
                    goto stop_flag;
                    break;
            }
        }
        continue;
      play_flag:
        count_time--;
        DPRINTF(("it is in file: %s \n", file_path));
      pause_flag:
#ifdef __ECOS
        cyg_thread_delay(40);
#else
        ApRelinquish();
#endif
    }
  stop_flag:
    decode_status = AUD_STATUS_STOP;
    while (destroy_decode == 0) {
#ifdef __ECOS
        cyg_thread_delay(40);
#else
        ApRelinquish();
#endif //__ECOS
    }
    DPRINTF(("decoder destroied! \n"));
    destroy_decode = 1;
}

/**
 *\brief create a decoder to decode a special audio file
 *\author yifj
 *\param path the audio file name which need to be decoded.
 *\date 2008-04-07
 *\return AUDIO_FALSE: failed to create the decode, AUDIO_TRUE: succed to create the decoder.
 */
int Audio_Start(char *path)
{
#ifdef __ECOS
    cyg_mutex_init(&mutex1);
#else
    pthread_t tidp;
#endif //__ECOS
    int error;
    decoder_ready = 1;
    file_path = malloc(strlen(path) + 1);
    if (file_path == NULL) {
        EPRINTF(("malloc error happened! \n"));
        assert(0);
        exit(0);
    }
    strcpy(file_path, path);
#ifdef __ECOS
    cyg_thread_create(17, &decoder_function, 0, "audio_decoder", decoder_stack, STACKSIZE, &decoder_handle,
                      &decoder_thread);
    cyg_thread_resume(decoder_handle);
#else
    error = pthread_create(&tidp, NULL, decoder_function, NULL);
    if (error != 0) {
        EPRINTF(("thread create failure\n"));
        return AUDIO_FALSE;
    }
#endif // __ECOS
    return AUDIO_TRUE;
}

/**
 *\brief send the command to decoder. it is usually called by audio player UI.
 *\author yifj
 *\param AudioCmd The command need to be executed.
 *\date 2008-04-07
 *\return
 */
void Audio_SetCmd(tAudCmdType AudioCmd)
{
    set_the_flag(1);
    last_cmd = AudioCmd;
}

//Volume
/**
 *\brief set the decoder volume
 *\author yifj
 *\param value the volume value need to set.
 *\date 2008-04-07
 *\return
 */
int Set_Volume(int value)
{
}

//status
/**
 *\brief get the decoder status
 *\author yifj

 *\date 2008-04-07
 *\return status
 */
tAudSatusType Get_Status()
{
    return decode_status;
}

/**
 *\brief get the time which have been used for decoding the audio file when Type == AUD_TIME_NOW
 get the overall time which is need for decoding the audio file when Type == AUD_TIME_TOTAL
 *\author yifj
 *\param Type AUD_TIME_NOW or AUD_TIME_TOTAL
 *\date 2008-04-07
 *\return  0: wrong input param, other: time
 */
int Get_Timer(tTimeQueryType Type)
{
    if (Type == AUD_TIME_NOW)
        return (DECODE_LEN - count_time);
    else if (Type == AUD_TIME_TOTAL)
        return DECODE_LEN;
    else
        assert(0);
    return 0;
}

/**
 *\brief clear the decoder when the special file is no longer need to be decoder.
 *\author yifj

 *\date 2008-04-07
 *\return
 */
int Audio_Clear()
{
    destroy_decode = 1;
    free(file_path);
#ifdef __ECOS
    cyg_mutex_destroy(&mutex1);
#endif //__ECOS
    return 1;
}
