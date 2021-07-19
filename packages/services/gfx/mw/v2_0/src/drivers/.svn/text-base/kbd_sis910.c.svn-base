#include <stdio.h>
#include "device.h"
#include <fcntl.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include <cyg/infra/diag.h>		// For diagnostic printing.

#ifdef __ECOS
#define KEYBOARD "/dev/kbd"
#else
#define KEYBOARD "/dev/h3600_key"
#endif

static int  sis910kb_Open(KBDDEVICE *pkd);
static void sis910kb_Close(void);
static void sis910kb_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers);
static int  sis910kb_Read(MWKEY *buf, MWKEYMOD *modifiers, MWSCANCODE *scancode);
static int  sis910kb_Poll(void);

static int fd;

#define SIS910_SCANCODE_UP     0x1
#define SIS910_SCANCODE_DOWN   0x2
#define SIS910_SCANCODE_LEFT   0x4
#define SIS910_SCANCODE_RIGHT  0x8
#define SIS910_SCANCODE_ENTER  0x10
#define SIS910_SCANCODE_ESCAPE 0x20


static int sis910kb_Poll(void)
{
	return 1;
}
static int sis910kb_Open(KBDDEVICE *pkd)
{
	/* Open the keyboard and get it ready for use */
	fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);

	if (fd < 0) 
	{
		DPRINTF("%s - Can't open keyboard!\n", __FUNCTION__);
		return -1;
	}    
    return fd;
}
static void sis910kb_Close(void)
{
	close(fd);
	fd = -1;
}
static  void sis910kb_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers)
{
	if (modifiers)
		*modifiers = 0;		/* no modifiers available */
	if (curmodifiers)
		*curmodifiers = 0;
}
static int sis910kb_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode)
{
	int keydown = 0;
	int cc = 0;
	char buf[1];
	static time_t oldticks = 0;
	time_t newticks = 0;
     
    newticks = time(NULL);
    
    if((newticks - oldticks)< 1)
        return 0;
    else
        oldticks = newticks;
    
	cc = read(fd, &buf, 1);

	if (cc < 0) 
	{	    
		if ((errno != EINTR) && (errno != EAGAIN) && (errno != EINVAL)) 
		{
			perror("sis910 KEY");
			return (-1);
		} 
		else 
		{
			return (0);
		}
	}
	if (cc == 0)
	{	    
		return (0);
	}
    
    if(*buf > 0x0)
        keydown = 1; /*Key pressed but not released*/
    
    *scancode =*buf;
    
    
    switch(*scancode)
    {        
        case SIS910_SCANCODE_UP:
            *kbuf = MWKEY_UP;
            break;
            
        case SIS910_SCANCODE_DOWN:
            *kbuf = MWKEY_DOWN;
            break;
            
        case SIS910_SCANCODE_LEFT:
            *kbuf = MWKEY_LEFT;
            break;
                    
        case SIS910_SCANCODE_RIGHT:
            *kbuf = MWKEY_RIGHT;
            break;        
            
        case SIS910_SCANCODE_ENTER:
            *kbuf = MWKEY_ENTER;
            break;        
            
        case SIS910_SCANCODE_ESCAPE:
            *kbuf = MWKEY_ESCAPE;
            break;
            
        default: 
            DPRINTF("kbd_sis910 - Unknown scancode %d\n", *scancode);
            return 0;
    }
    return keydown;   
}
KBDDEVICE kbddev = {
	sis910kb_Open,
	sis910kb_Close,
	sis910kb_GetModifierInfo,
	sis910kb_Read,
	sis910kb_Poll
};