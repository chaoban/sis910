#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#ifdef AUD_DEBUG 
#include <stdio.h>
#define assert(a)	do{	\
	printf("assertion failed in %s at line %d\n", __FILE__, __LINE__);	\
}while(a)

#define FUNC_DEBUG()	do{ \
    printf("Come into File %s Function %s\n", __FILE__, __FUNCTION__);	\
}while(0)

#define EPRINTF(a)  printf a
#else
#define assert(a)

#define FUNC_DEBUG()

#define EPRINTF(a)
#endif


#endif	// DEBUG_H_INCLUDED
