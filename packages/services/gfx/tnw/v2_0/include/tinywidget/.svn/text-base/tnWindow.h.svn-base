 /*Header file for the Window widget
 * This file is part of `TinyWidgets', a widget set for the nano-X GUI which is  * a part of the Microwindows project (www.microwindows.org).
 * Copyright C 2000
 * Sunil Soman <sunil_soman@vsnl.com>
 * Amit Kulkarni <amms@vsnl.net>
 * Navin Thadani <navs@vsnl.net>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; either version 2.1 of the License, 
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License 
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _TNWINDOW_H_
#define _TNWINDOW_H_

#define WINDOW_CALLBACKS 3
#include "tnBase.h"
#define TN_WINDOW_HEIGHT 200	/*Default Window width & height */
#define TN_WINDOW_WIDTH 300

typedef struct
{
  GR_IMAGE_ID	iBack;
  GR_COLOR	cBack;
  GR_WINDOW_ID	pid;
}TN_STRUCT_WINDOW;

TN_WIDGET *CreateWindow(TN_WIDGET *parent, int flags, GR_RECT *rect, GR_IMAGE_ID iid, GR_COLOR bgcolor);
TN_WIDGET *CreateWindowFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo); 
void DrawWindow(TN_WIDGET *widget);
void UpdateFrontWindow(TN_WIDGET *widget, PRCINFO rcinfo);
void WindowEventHandler (GR_EVENT *, TN_WIDGET *);
void DestroyWindow(TN_WIDGET *);
void tnGetClickedPos(TN_WIDGET *, int * , int *);
int tnGetWindowTitle(TN_WIDGET *, char *);
int tnSetWindowTitle(TN_WIDGET *, char *);
GR_WINDOW_ID CreateImageFromWindow(TN_WIDGET *widget, HW_RECT *rect);

#endif	/*_TNWINDOW_H_*/
