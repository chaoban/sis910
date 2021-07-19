#ifndef __GLOBAL_H__
#define __GLOBAL_H__

enum MAIL_TYPE {
    MAIL_RETURN=100, 
    MAIL_APLAYER_RETURN, 
    MAIL_APLAYER_BACKGROUND,
    MAIL_APLAYER_FOREGROUND,
    MAIL_SUBJECT_GREEN,
    MAIL_SUBJECT_BLUE,
    MAIL_FB_OPEN
};


#ifdef __ECOS
#include <cyg/kernel/kapi.h>
extern cyg_handle_t dpfDesktopMboxHandle;
extern cyg_handle_t dpfAudMboxHandle;
extern cyg_ucount32 dpfgNanoxDataIndex;
extern char dpfFbOpenedFileName[512];
#endif //end of __ECOS
#endif //end of __GLOBAL_H__
