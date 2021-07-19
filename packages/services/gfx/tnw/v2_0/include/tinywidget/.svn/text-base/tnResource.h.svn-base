/**
 * @file rc.h, by solarhe at 2008/2/7
 */

#ifndef	__RC_H__
#define __RC_H__

#include <microwin/nano-X.h>

#define RC_NONE	-1
#define RC_STR	0
#define RC_IMG	1
#define RC_BTN	2
#define RC_CKB 	3	
#define RC_RDB	4
#define RC_LBL	5
#define RC_LBX	6	
#define RC_PGB	7
#define RC_WND	8
#define RC_PIC	9
#define RC_BTNGRP 10
#define RC_LAN	12	
#define RC_KEYWORD	12	

#define ELACK_MEMORY	-1
#define EMULT_SECTION	-2
#define ESECT_EMPTY	-3
#define EUDEF_SECTION	-4
#define EWRNG_PLACE	-5
#define EPLUS_COMMAND	-6
#define EWRNG_KEYWORD	-7

typedef char *PCHAR;
typedef PCHAR RCSTR, *PRCSTR;
typedef PCHAR RCIMG, *PRCIMG;

typedef GR_RECT HW_RECT, *PHW_RECT;

typedef struct {
	HW_RECT	rect;
	int	cap;
	int	img[2];
	int keymap[4];
}RCBTN, *PRCBTN;

typedef struct {
	HW_RECT	rect;
	int	cap;
	int	img[4];
	int keymap[4];
}RCCKB, *PRCCKB;

typedef struct {
	HW_RECT	rect;
	int	cap;
	int	img[4];
	int keymap[4];
}RCRDB, *PRCRDB;

typedef struct {
	HW_RECT	rect;
	int	cap;
	int	img;
}RCLBL, *PRCLBL;

typedef struct {
	HW_RECT	rect;
	int type;
	int	img[4];
	int	count;
	int	*item;
	int	*icon;	
}RCLBX, *PRCLBX;

typedef struct {
	HW_RECT	rect;
	int	cap;
	int	img[2];
	int	ntype;
	int	stepsize;
}RCPGB, *PRCPGB;

typedef struct {
	HW_RECT	rect;
	int	img;
}RCWND, *PRCWND;

typedef struct {
	HW_RECT	rect;
	int	img;
	int	stretch;
}RCPIC, *PRCPIC;

typedef struct{
    HW_RECT rect;
    int type;
    int drawdata[5];
}RCBTNGRP, *PRCBTNGRP;

typedef struct {
	int	id;
	int	flag;
	char*	lang;
	FILE*	file;
}RCRUN, *PRCRUN;

typedef struct {
	void**	rc;
	int	count;
}RCITEM, *PRCITEM;

typedef struct {
	RCITEM	items[11];
	char*	path;
}RCINFO, *PRCINFO;

typedef int (*CMD_FN)(PRCINFO rcinfo, PRCRUN run_info, char *param);

/** map the keyword to their interpreting function */
typedef struct {
	char*	cmd;
	CMD_FN	pfn;
	int	sz;
} CMDMAP;

extern int get_rc_info(PRCINFO rc_info, int type, int id, void *rc);
extern void destroy_rc(PRCINFO rc_info);
extern int load_rc(PRCINFO rc_info, char *fname, char *lang);

#endif // __RC_H__
