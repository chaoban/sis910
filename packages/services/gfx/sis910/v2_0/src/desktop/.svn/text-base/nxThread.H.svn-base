#ifndef __MW_THREAD_H__
#define __MW_THREAD_H__
#include <pthread.h>
#include "../share/global.h"

#ifdef __ECOS
#define STACKSIZE ( 65536 )

typedef void fun(CYG_ADDRWORD);
struct tNxThread {
    char *name;
    fun *entry;
    int prio;
    cyg_thread t_obj;
    cyg_handle_t t;    
    char stack[STACKSIZE];
    struct THREAD_PARAMETER parameters;
};
#else
#define CFG_FILE_PATH "./resource/config.ini"
#define RESOURCE_PATH "./resource"
#endif
#ifndef DEBUG
#define DEBUG
#endif
#ifdef DEBUG
#define dprintf(x, args...) fprintf(stderr, "%s: %d: " x "\n",__FILE__, __LINE__, ##args);
#else
#define dprintf(x...) do{}while(0)
#endif //end of __ECOS


#endif //end of __MW_THREAD_H__
