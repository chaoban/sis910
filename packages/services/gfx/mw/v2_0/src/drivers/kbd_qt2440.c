/*
 * Copyright (c) 2000 Greg Haerr <greg@censoft.com>
 *
 * QT2440 Keyboard Driver
 */
#include <stdio.h>
#include "device.h"
#include <fcntl.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include <cyg/infra/diag.h>		// For diagnostic printing.

#define	ClearPending(bit)		\
	CYG_MACRO_START				\
		rSRCPND = bit;			\
		rINTPND = bit;			\
		rINTPND;				\
	CYG_MACRO_END

#define QT2440_SCANCODE_UP     0x1
#define QT2440_SCANCODE_DOWN   0x2
#define QT2440_SCANCODE_LEFT   0x3
#define QT2440_SCANCODE_RIGHT  0x4
#define QT2440_SCANCODE_ENTER  0x5
#define QT2440_SCANCODE_ESCAPE 0x6
#define QT2440_SCANCODE_F1     0x7
#define QT2440_SCANCODE_F2     0x8

#ifdef __ECOS
#define KEYBOARD "/dev/kbd"
#else
#define KEYBOARD "/dev/h3600_key"
#endif

static int  qt2440kb_Open(KBDDEVICE *pkd);
static void qt2440kb_Close(void);
static void qt2440kb_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers);
static int  qt2440kb_Read(MWKEY *buf, MWKEYMOD *modifiers, MWSCANCODE *scancode);
static int  qt2440kb_Poll(void);

KBDDEVICE kbddev = {
	qt2440kb_Open,
	qt2440kb_Close,
	qt2440kb_GetModifierInfo,
	qt2440kb_Read,
	qt2440kb_Poll
};

static int fd;

/*
 * chaoban code
 */


/*
 * Poll for keyboard events
 */
static int
qt2440kb_Poll(void)
{
    unsigned char key;
    int push=1;
   /* while (push) {
		poll_int0();							 
		poll_int2();
		poll_int11_19();
		
		key = Key_Scan();
		if(key != 0xff) {
			//key=1,2,3, 或4，分別表示第1,2,3 或第4個按鍵被按下一次。
		    //key=5,6,7,或8，分別表示第1,2,3 或第4個按鍵被按下兩次。

			diag_printf("Button %x is pressed!\n", key);
			push =0;
		}
	}*/
	diag_printf("POLL Button is pressed!\n");
	return 1;
}

/*
 * Open the keyboard.
 */
static int
qt2440kb_Open(KBDDEVICE *pkd)
{
	/* Open the keyboard and get it ready for use */
	fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);

	if (fd < 0) {
		DPRINTF("%s - Can't open keyboard!\n", __FUNCTION__);
		return -1;
	}

	return fd;
}

/*
 * Close the keyboard.
 */
static void
qt2440kb_Close(void)
{
	close(fd);
	fd = -1;
}

/*
 * Return the possible modifiers for the keyboard.
 */
static  void
qt2440kb_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers)
{
	if (modifiers)
		*modifiers = 0;		/* no modifiers available */
	if (curmodifiers)
		*curmodifiers = 0;
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the modifier keys (ALT, SHIFT, etc).  Returns -1 on error, 0 if no data
 * is ready, 1 on a keypress, and 2 on keyrelease.
 * This is a non-blocking call.
 */
static int
qt2440kb_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode)
{

	int keydown = 0;
	int cc = 0;
	char buf[1];

	cc = read(fd, &buf, 1);
    DPRINTF("Ipaq - Unknown scancode %c\n", buf[0]);
	if (cc < 0) {
		if ((errno != EINTR) && (errno != EAGAIN)
		    && (errno != EINVAL)) {
			perror("QT2440 KEY");
			return (-1);
		} else {
			return (0);
		}
	}
	if (cc == 0)
		return (0);
    
    if(*buf > 0x0)
        keydown = 1; /*Key pressed but not released*/
    
    *scancode =*buf;
    
    switch(*scancode){
        case QT2440_SCANCODE_UP:
            *kbuf = MWKEY_UP;
            //*kbuf = 100;
            printf("qt2440kb_Read %d %d\n",*kbuf, *scancode);
            break;
            
        case QT2440_SCANCODE_DOWN:
            *kbuf = MWKEY_DOWN;
            //*kbuf = 101;
            printf("qt2440kb_Read %d %d\n",*kbuf, *scancode);
            break;
            
        case QT2440_SCANCODE_LEFT:
            *kbuf = MWKEY_LEFT;
            break;
                    
        case QT2440_SCANCODE_RIGHT:
            *kbuf = MWKEY_RIGHT;
            break;        
            
        case QT2440_SCANCODE_ENTER:
            *kbuf = MWKEY_ENTER;
            break;        
        case QT2440_SCANCODE_ESCAPE:
            *kbuf = MWKEY_ESCAPE;
            break;
                    
        case QT2440_SCANCODE_F1:
            *kbuf = MWKEY_F1;
            break;
            
        case QT2440_SCANCODE_F2:
            *kbuf = MWKEY_F2;
            break;
            
        default:        
            DPRINTF("QT2440 - Unknown scancode %d\n", *scancode);
            return 0;
    }
    
    return keydown;            	
}
