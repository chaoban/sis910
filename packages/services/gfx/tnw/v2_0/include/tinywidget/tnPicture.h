#ifndef _TNPICTURE_H_
#define _TNPICTURE_H_
#define PICTURE_CALLBACKS 1 
#include "tnBase.h"
#include <microwin/nano-X.h>
#define TN_PICTURE_HEIGHT 200 
#define TN_PICTURE_WIDTH  200

typedef struct
{
	char *filename;
	GR_IMAGE_ID imageid;
	GR_BOOL stretch;
	float fscale;
	int offsetX, offsetY;
	GR_BOOL	bFocus;
}
TN_STRUCT_PICTURE;

typedef struct
{
	int height;		/* Image height in Pixels */
	int width;		/* Image width in Pixels */
	int bpp;		/* bits per pixel (1,4 or 8) */
	int bytesperpixel;	/* bytes per pixel */
}
TN_IMAGE_INFO;

TN_WIDGET *CreatePicture(TN_WIDGET *parent, int flags, GR_RECT *rect, char *fname, GR_BOOL stretch);
void DrawPicture (TN_WIDGET *);
void PictureEventHandler(GR_EVENT *,TN_WIDGET *);
int tnSetPictureStretch(TN_WIDGET *widget,GR_BOOL stretch, int scale, int x, int y);
int tnSetPicture(TN_WIDGET *,char *);
int tnSetPictureID(TN_WIDGET *,GR_IMAGE_ID id);
void DestroyPicture(TN_WIDGET*);
int tnGetPictureProps(TN_WIDGET *,GR_BOOL *, char **);
int tnGetPictureImageProps(TN_WIDGET *, TN_IMAGE_INFO *);
int tnPictureDup(TN_WIDGET *dest,TN_WIDGET *src);
#endif /*_TNPICTURE_H_*/
