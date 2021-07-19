
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pkgconf/fs_fat.h>
#include <cyg/fs/fatfs.h>
#include <cyg/fileio/fileio.h>

#include "nano-X.h"
#include "device.h"
#include "audio_io.h"
#define MWINCLUDECOLORS
static GR_WINDOW_ID	w;             
#define TITLE 		"Audio Player"

#define BM_WIDTH 	160
#define BM_HEIGHT 	250

extern int KbdOpen(void);

static void
audio(void)
{
    char* path = NULL;

printf("audio\n");

    path = "/LOVE.mp3";

    MediaDecoder_Start(path);
//    MediaDecoder_SetCmd(MEDIA_CMD_PLAY);
//    MediaDecoder_Clear();
}
static void
audio_player(GR_EVENT_KEYSTROKE	*kp)
{
    char* path = NULL;

printf("audio_player\n");

//    path = "/LOVE.mp3";
//
//    MediaDecoder_Start(path);
//    MediaDecoder_SetCmd(MEDIA_CMD_PLAY);
//    MediaDecoder_Clear();
	switch(kp->ch) 
	{
		case MWKEY_UP:
            printf("PLAY be pressed.\n");
            MediaDecoder_SetCmd(MEDIA_CMD_PLAY);
			return;
		case MWKEY_DOWN:
            printf("STOP be pressed.\n");
            MediaDecoder_SetCmd(MEDIA_CMD_STOP);
			return;
		case MWKEY_LEFT:
            printf("PAUSE be pressed.\n");
            MediaDecoder_SetCmd(MEDIA_CMD_PAUSE);
			return;
		case MWKEY_RIGHT:
            printf("CONTINUE be pressed.\n");
            MediaDecoder_SetCmd(MEDIA_CMD_CONTINUE);
			return;
		default:
		    printf("%d be pressed.\n",(int)kp->ch);
		    return;
	}
}

#ifdef __ECOS                            
int
audio_main(int argc, char* argv[])
#else
int
main(int argc, char* argv[])
#endif
{
    GR_EVENT        event;          /* current event */
	GR_WM_PROPERTIES props;

    if (GrOpen() < 0) {
        fprintf(stderr, "nxkbd: cannot open graphics\n");
        exit(1);
    }

	if (KbdOpen() < 0) {
                fprintf(stderr, "nxkbd: cannot open kbd named pipe\n");
    }
        
	//GrGetScreenInfo(&si);
	
    w = GrNewWindow(GR_ROOT_WINDOW_ID, 
                    0, 0, (2*BM_WIDTH+10), BM_HEIGHT, 
                    0, 0, 255);
                    
	
    GrSelectEvents(w, GR_EVENT_MASK_CLOSE_REQ |
                      GR_EVENT_MASK_KEY_DOWN |
                      GR_EVENT_MASK_EXPOSURE );
		
	props.flags = GR_WM_FLAGS_PROPS | GR_WM_FLAGS_TITLE;
	//props.props = GR_WM_PROPS_NOFOCUS;
	props.props = /*GR_WM_PROPS_NOMOVE |*/ GR_WM_PROPS_NORAISE |
		GR_WM_PROPS_BORDER | GR_WM_PROPS_CAPTION | GR_WM_PROPS_CLOSEBOX;
	props.title = TITLE;
	GrSetWMProperties(w, &props);

    GrMapWindow(w);
    GrRaiseWindow(w);


    
    while (1) {
            GrGetNextEvent(&event);
		
            switch(event.type) {
			case GR_EVENT_TYPE_CLOSE_REQ:
				GrClose();
				exit(0);
				/* no return*/
			case GR_EVENT_TYPE_EXPOSURE:
				audio();				
				break;
			case GR_EVENT_TYPE_KEY_DOWN:
			    printf("GR_EVENT_TYPE_KEY_DOWN\n");
				audio_player(&event.keystroke);
				break;
		    }
	}
	
	/*NOTREACHED*/
	return 0;
 }
