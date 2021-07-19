/**
 * @file tnResource.c, by solarhe at 2008/2/7
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tnWidgets.h"	//for using window hash table.
#include "tnTrim.h"
#include "tnResource.h"

static int cmd_str(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_img(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_btn(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_ckb(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_rdb(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_lbl(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_lbx(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_pgb(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_wnd(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_pic(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_btngrp(PRCINFO rc_info, PRCRUN run_info, char *param);
static int cmd_rc(PRCINFO rc_info, PRCRUN run_info, char *param);

static CMDMAP	cmd_map[] = {		
	{"str",	cmd_str, sizeof(RCSTR)},	/**< 0 the string command and its interpreting function */
	{"img",	cmd_img, sizeof(RCIMG)},	/**< 1 the image command and its interpreting fucntion */
	{"btn", cmd_btn, sizeof(RCBTN)},	/**< 2 the button command and its interpreting function */
	{"ckb", cmd_ckb, sizeof(RCCKB)},	/**< 3 the checkbtn command and its interpreting function */ 
	{"rdb", cmd_rdb, sizeof(RCRDB)},	/**< 4 the radio button command and its interpreting function */
	{"lbl", cmd_lbl, sizeof(RCLBL)},	/**< 5 the radio button command and its interpreting function */
	{"lbx", cmd_lbx, sizeof(RCLBX)},	/**< 6 the listbox command and its interpreting function */
	{"pgb", cmd_pgb, sizeof(RCPGB)},	/**< 7 the progressbar command and its interpreting function */ 
	{"wnd", cmd_wnd, sizeof(RCWND)},	/**< 8 the window command and its interpreting function */
	{"pic", cmd_pic, sizeof(RCPIC)},	/**< 9 the pic command and its interpreting function */
	{"tet", cmd_btngrp, sizeof(RCBTNGRP)},	/**< 10 the pic command and its interpreting function */
	{"rc", 	cmd_rc}				/**< 11 the rc command to name section and its interpreting function */
};

// interpret command...
static void
inter_rect(PHW_RECT rect, char *param)
{
	rect->x = cut_item_int(param, ',', 0);
	rect->y = cut_item_int(param, ',', 1);
	rect->width = cut_item_int(param, ',', 2);
	rect->height = cut_item_int(param, ',', 3);	
}

static int 
cmd_str(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCSTR	rcstr;

	if (run_info->flag != RC_STR) {
		if (run_info->flag != RC_LAN) return EWRNG_PLACE;
		else return 0;
	}
	if (run_info->id >= rc_info->items[RC_STR].count) return EPLUS_COMMAND;
	if ((rcstr = malloc(strlen(param) + 1)) == NULL) return ELACK_MEMORY;

	memcpy(rcstr, param, strlen(param) + 1);

	rc_info->items[RC_STR].rc[run_info->id++] = (void *)rcstr;

	return 0;
}

static int 
cmd_img(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCIMG	rcimg;

	if (run_info->flag != RC_IMG) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_IMG].count) return EPLUS_COMMAND; 
	if ((rcimg = malloc(strlen(param) + 1)) == NULL) return ELACK_MEMORY;

	memcpy(rcimg, param, strlen(param) + 1);
	rc_info->items[RC_IMG].rc[run_info->id++] = (void *)rcimg;

	return 0;
}

static int 
cmd_btn(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCBTN	rcbtn;

	if (run_info->flag != RC_BTN) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_BTN].count) return EPLUS_COMMAND; 
	if ((rcbtn = malloc(sizeof(RCBTN))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rcbtn->rect), param);
	rcbtn->cap = cut_item_int(param, ',', 4);
	rcbtn->img[0] = cut_item_int(param, ',', 5);
	rcbtn->img[1] = cut_item_int(param, ',', 6);
	rcbtn->keymap[0] = cut_item_int(param, ',', 7);
	rcbtn->keymap[1] = cut_item_int(param, ',', 8);
	rcbtn->keymap[2] = cut_item_int(param, ',', 9);
	rcbtn->keymap[3] = cut_item_int(param, ',', 10);
	rc_info->items[RC_BTN].rc[run_info->id++] = (void *)rcbtn;

	return 0;
}

static int 
cmd_ckb(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCCKB	rcckb;

	if (run_info->flag != RC_CKB) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_CKB].count) return EPLUS_COMMAND; 
	if ((rcckb = malloc(sizeof(RCCKB))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rcckb->rect), param);
	rcckb->cap = cut_item_int(param, ',', 4);
	rcckb->img[0] = cut_item_int(param, ',', 5);
	rcckb->img[1] = cut_item_int(param, ',', 6);
	rcckb->img[2] = cut_item_int(param, ',', 7);
	rcckb->img[3] = cut_item_int(param, ',', 8);
	rcckb->keymap[0] = cut_item_int(param, ',', 9);
	rcckb->keymap[1] = cut_item_int(param, ',', 10);
	rcckb->keymap[2] = cut_item_int(param, ',', 11);
	rcckb->keymap[3] = cut_item_int(param, ',', 12);
	rc_info->items[RC_CKB].rc[run_info->id++] = (void *)rcckb;

	return 0;
}

static int 
cmd_rdb(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCRDB	rcrdb;

	if (run_info->flag != RC_RDB) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_RDB].count) return EPLUS_COMMAND; 
	if ((rcrdb = malloc(sizeof(RCRDB))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rcrdb->rect), param);
	rcrdb->cap = cut_item_int(param, ',', 4);
	rcrdb->img[0] = cut_item_int(param, ',', 5);
	rcrdb->img[1] = cut_item_int(param, ',', 6);
	rcrdb->img[2] = cut_item_int(param, ',', 7);
	rcrdb->img[3] = cut_item_int(param, ',', 8);
	rcrdb->keymap[0] = cut_item_int(param, ',', 9);
	rcrdb->keymap[1] = cut_item_int(param, ',', 10);
	rcrdb->keymap[2] = cut_item_int(param, ',', 11);
	rcrdb->keymap[3] = cut_item_int(param, ',', 12);
	rc_info->items[RC_RDB].rc[run_info->id++] = (void *)rcrdb;

	return 0;
}

static int 
cmd_lbl(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCLBL	rclbl;

	if (run_info->flag != RC_LBL) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_LBL].count) return EPLUS_COMMAND; 
	if ((rclbl = malloc(sizeof(RCLBL))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rclbl->rect), param);
	rclbl->cap = cut_item_int(param, ',', 4);
	rclbl->img = cut_item_int(param, ',', 5);
	rc_info->items[RC_LBL].rc[run_info->id++] = (void *)rclbl;

	return 0;
}

static int 
cmd_lbx(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCLBX	rclbx;
	int	count;
	int 	*strtmp, *icotmp, *strbase, *icobase;
	
	if (run_info->flag != RC_LBX) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_LBX].count) return EPLUS_COMMAND; 
	if ((rclbx = malloc(sizeof(RCLBX))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rclbx->rect), param);
	rclbx->type   = cut_item_int(param, ',', 4);
	rclbx->img[0] = cut_item_int(param, ',', 5);
	rclbx->img[1] = cut_item_int(param, ',', 6);
	rclbx->img[2] = cut_item_int(param, ',', 7);
	rclbx->img[3] = cut_item_int(param, ',', 8); 
	rclbx->count = cut_item_int(param, ',', 9); 
	
	strtmp = malloc(sizeof(int)*rclbx->count);
	icotmp = malloc(sizeof(int)*rclbx->count);
	strbase = strtmp;
	icobase = icotmp;
	for(count = 0; count < 2 * rclbx->count; count += 2)
	{
		*strtmp = cut_item_int(param, ',', count + 10);
		*icotmp = cut_item_int(param, ',', count + 11);
		strtmp++;
		icotmp++;
	}
	
	rclbx->item = strbase;
	rclbx->icon = icobase;
	rc_info->items[RC_LBX].rc[run_info->id++] = (void *)rclbx;
	
	return 0;

}

static int 
cmd_pgb(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	PRCPGB	rcpgb;

	if (run_info->flag != RC_PGB) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_PGB].count) return EPLUS_COMMAND; 
	if ((rcpgb = malloc(sizeof(RCPGB))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rcpgb->rect), param);
	rcpgb->img[0] = cut_item_int(param, ',', 4);
	rcpgb->img[1] = cut_item_int(param, ',', 5);
	rcpgb->ntype = cut_item_int(param, ',', 6);
	rcpgb->stepsize = cut_item_int(param, ',', 7);
	rc_info->items[RC_PGB].rc[run_info->id++] = (void *)rcpgb;

	return 0;
}

static int 
cmd_wnd(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	RCWND	*rcwnd;		

	if (run_info->flag != RC_WND) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_WND].count) return EPLUS_COMMAND; 
	if ((rcwnd = malloc(sizeof(RCWND))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rcwnd->rect), param);
	rcwnd->img = cut_item_int(param, ',', 4); //need to modify.
	rc_info->items[RC_WND].rc[run_info->id++] = (void *)rcwnd;

	return 0;
}

static int 
cmd_pic(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	RCPIC	*rcpic;		

	if (run_info->flag != RC_PIC) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_PIC].count) return EPLUS_COMMAND; 
	if ((rcpic = malloc(sizeof(RCPIC))) == NULL) return ELACK_MEMORY;

	inter_rect(&(rcpic->rect), param);
	rcpic->img = cut_item_int(param, ',', 4); //need to modify.
	rcpic->stretch= cut_item_int(param, ',', 5);
	rc_info->items[RC_PIC].rc[run_info->id++] = (void *)rcpic;

	return 0;
}

static int 
cmd_btngrp(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	RCBTNGRP	*rcbtngrp;		

	if (run_info->flag != RC_BTNGRP) return EWRNG_PLACE;
	if (run_info->id >= rc_info->items[RC_BTNGRP].count) return EPLUS_COMMAND; 
	if ((rcbtngrp = malloc(sizeof(RCBTNGRP))) == NULL) return ELACK_MEMORY;
    
	inter_rect(&(rcbtngrp->rect), param);	
	rcbtngrp->type = cut_item_int(param, ',', 4); //need to modify.
	rcbtngrp->drawdata[0] = cut_item_int(param, ',', 5);
	rcbtngrp->drawdata[1] = cut_item_int(param, ',', 6);
	rcbtngrp->drawdata[2] = cut_item_int(param, ',', 7);
	rcbtngrp->drawdata[3] = cut_item_int(param, ',', 8);
	rcbtngrp->drawdata[4] = cut_item_int(param, ',', 9);
			
	rc_info->items[RC_BTNGRP].rc[run_info->id++] = (void *)rcbtngrp;

	return 0;
}

static int
cmd_rc(PRCINFO rc_info, PRCRUN run_info, char *param)
{
	char	tmp[255];
	int	item;
	int	i;

	// find out the section name.
	cut_item(param, tmp, ',', 0);

	for (i = 0; i < (RC_KEYWORD - 1); i++) {
		if (!strcmp(cmd_map[i].cmd, tmp)) {
			// only one xxx section could be include in the rc file.
			if (!strcmp(tmp, cmd_map[RC_STR].cmd)) {
			    cut_item(param, tmp, ',', 2);
			    if (strcmp(run_info->lang, tmp)) {
				    run_info->flag = RC_LAN;
				    return 0;
			    }
			} else {
			       	if (rc_info->items[i].rc != NULL) 
				    return EMULT_SECTION;		
			}
            
			// get the section count and save in nitem.
			item = cut_item_int(param, ',', 1);
			if (!item) return ESECT_EMPTY;

			rc_info->items[i].rc = malloc(sizeof(void*) * item);
			if (rc_info->items[i].rc == NULL)
				return ELACK_MEMORY;

			run_info->flag = i;
			run_info->id = 0;
			rc_info->items[i].count = item;

			return 0;
		}
	}

	return EWRNG_KEYWORD;
}

static void
exit_rc(PRCRUN run_info)
{
	if (run_info == NULL)
		return;

	if (run_info->lang)
		free(run_info->lang);

	if (run_info->file)
		fclose(run_info->file);

	free(run_info);
}

static PRCRUN
init_rc(PRCINFO rc_info, char *fname, char *lang)
{
	char	full_name[255];
	PRCRUN	run_info;
	int	i;

	if (fname == NULL || lang == NULL || rc_info == NULL)
		return NULL;

	// if absolute path was used.
	if (fname[0] == '/') 
		strcpy(full_name, fname);

	// if relative path was used.
	else {
		getcwd(full_name, 255);
		strcat(full_name, "/romdisk");
		strcat(full_name, fname);
	}	

	// create RUNINFO to save parameter of runtime.
	if ((run_info = malloc(sizeof(RCRUN))) == NULL) 
		return NULL;
	memset(run_info, 0, sizeof(RCRUN));

	// open rc file and store the language to run_info->file.
	if ((run_info->file = fopen(full_name, "rt")) == NULL) {
		exit_rc(run_info);
		printf("Can't open the resource file to read\n");
		return NULL;
	}

	// store the language to run_info->lang.
	if ((run_info->lang = malloc(strlen(lang) + 1)) == NULL) {
		exit_rc(run_info);
		return NULL;
	}
	strcpy(run_info->lang, lang);

	// initialize run_info other member.
	run_info->id = 0;
	run_info->flag = RC_NONE;

	// initalize RCINFO.
	memset(rc_info, 0, sizeof(RCINFO));

	// find out the path where the rc file lied in.
	if ((rc_info->path = malloc(strlen(full_name) + 1)) == NULL) {
		exit_rc(run_info);
		return NULL;
	}

	strcpy(rc_info->path, full_name);
	for (i = strlen(rc_info->path) - 1; i > 0; i--) {
		if (rc_info->path[i] == '/')
			break;
	}
	rc_info->path[i + 1] = 0;

	return run_info;
}

int 
get_rc_info(PRCINFO rc_info, int type, int id, void *rc)
{
	if (type >= RC_KEYWORD) return -1;
	if (id < 0 || id >= rc_info->items[type].count) return -1;	

	if (type == RC_STR) {
		strcpy(rc, rc_info->items[RC_STR].rc[id]);
		return 0;
	}
	if (type == RC_IMG) {
		strcpy(rc, rc_info->items[RC_IMG].rc[id]);
		return 0;
	}
	memcpy(rc, rc_info->items[type].rc[id], cmd_map[type].sz); 	

	return 0;
}

void 
destroy_rc(PRCINFO rc_info)
{
	int 	i, j;
	PRCLBX	lbx;

	for (i = 0; i < (RC_KEYWORD - 1); i++) {
		if (rc_info->items[i].rc != NULL) {
			for (j = 0; j < rc_info->items[i].count; j++) {		
				if (i == RC_LBX)
				{
					lbx = (PRCLBX)rc_info->items[i].rc[j];
					free(lbx->item);
					free(lbx->icon);
				}
				free(rc_info->items[i].rc[j]);
			}

			free(rc_info->items[i].rc);
		}
	}

	if (rc_info->path != NULL)
		free(rc_info->path);
}

int 
load_rc(PRCINFO rc_info, char *fname, char *lang)
{
	PRCRUN	run_info;
	unsigned char	tmp[255];
	unsigned char	*ptmp;
	unsigned char	command[4];
	unsigned char	param[255];
	int		c;
	int		i;
	int		ret;
	int		line = 0;

	// initialize RUNINFO.
	if ((run_info = init_rc(rc_info, fname, lang)) == NULL) {
		printf("Failed to initialize RC\n"); 
		return -1;
	}

	// read line from file to interpret.
	while (fgets(tmp, 255, run_info->file)) {

		line++;
		// replace the "\r\n" or "\n" with NULL, and NULL
		// means the end of the string.
		c = tmp[strlen(tmp) - 1];
		if (c == '\n' || c == '\r')
			tmp[strlen(tmp) - 1] = 0;
		c = tmp[strlen(tmp) - 1];
		if (c == '\n' || c == '\r')
			tmp[strlen(tmp) - 1] = 0;
		
		// delete the remark, ;remark 
		for (c = 0; c < (int)strlen(tmp); c++) {
			if (tmp[c] == ';') {
				tmp[c] = 0;
				break;
			}
		}

		if (strlen(tmp) == 0) continue;

		// replace the '\t' with ' '.
		str_swap(tmp, '\t', ' ');

		// delete the ' ' from the begining of string.
		ptmp = trim_left(tmp);
		if (strlen(ptmp) == 0) continue;

		// replace the ' ' from the string. 
		ptmp = trim(ptmp);

		// 
		cut_item(ptmp, command, '=', 0);
		cut_item(ptmp, param, '=', 1);

		for (i = 0; i < RC_KEYWORD; i++) {
			if (!strcmp(command, cmd_map[i].cmd)) {
				if (ret = cmd_map[i].pfn(rc_info, run_info, param)) {
					exit_rc(run_info);
					printf("Error: %d on line: %d\n", ret, line);
					return ret;
				}
				else
					break;
			}
		}
	}

	exit_rc(run_info);
	return ret;
}
