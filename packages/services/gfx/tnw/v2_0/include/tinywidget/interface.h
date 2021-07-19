/**
 * @file interface.h, by solarhe 2008/03/18
 */

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#define CONTROL_DEBUG

#ifdef CONTROL_DEBUG
#define printf_b printf
#else
#define printf_b 
#endif

char * strup(char *src);

extern int switch_subject(PRCINFO rc_info, char *fname, char *lang);
extern int switch_lang(PRCINFO rc_info, char *fname, char *lang);
extern GR_IMAGE_ID LoadImageFromRC(PRCINFO rcinfo, int id, HW_RECT *rect);
//extern GR_IMAGE_ID LoadImageFromRC(PRCINFO rcinfo, int id);
extern GR_IMAGE_ID CreateImageFromWindow(TN_WIDGET *wnd, HW_RECT *rect);

extern TN_WIDGET* CreateButtonFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo);
extern TN_WIDGET* CreateCheckButtonFromRC(TN_WIDGET * parent, int flags, int id, PRCINFO rcinfo);
extern TN_WIDGET* CreateRadioButtonFromRC(TN_WIDGET * parent, int flags, int id, PRDBGROUP group, PRCINFO rcinfo);
extern TN_WIDGET* CreateLabelFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo);
extern TN_WIDGET* CreateProgressBarFromRC(TN_WIDGET * parent, int flags, int id, PRCINFO rcinfo);
extern TN_WIDGET* CreateWindowFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo);
extern TN_WIDGET* CreateListBoxFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo);
extern TN_WIDGET* CreatePictureFromRC(TN_WIDGET *parent, int flags, int id, PRCINFO rcinfo) ;
extern PBTNGROUP  CreateBtnGroupFromRC(TN_WIDGET *parent, int id, PRCINFO rcinfo);


extern void UpdateFrontButton(TN_WIDGET *widget, PRCINFO rcinfo);
extern void UpdateFrontCheckButton(TN_WIDGET * widget, PRCINFO rcinfo);
extern void UpdateFrontRadioButton(TN_WIDGET * widget, PRCINFO rcinfo);
extern void UpdateFrontLabel(TN_WIDGET *widget, PRCINFO rcinfo);
extern void UpdateFrontListBox(TN_WIDGET * widget, PRCINFO rcinfo);
extern void UpdateFrontProgressBar(TN_WIDGET * widget, PRCINFO rcinfo);
extern void UpdateFrontWindow(TN_WIDGET *widget, PRCINFO rcinfo);
extern void UpdateFrontPicture(TN_WIDGET *widget, PRCINFO rcinfo);
extern void UpdateFrontScrollBar(TN_WIDGET * widget, GR_RECT rect);
extern void getRcFilePath(char * allpath,char *resourcepath, char *subject, char * ap );


#endif //_INTERFACE_H_
