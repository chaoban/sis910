#ifndef LAYER3_H_INCLUDED
#define LAYER3_H_INCLUDED

#include <pkgconf/devs_aud_mp3.h>
#include <cyg/io/aud/adecctrl.h>
#include "mpglib.h"

int decode_fromfile(AUD_tCtrlBlk *pAcb, short pcm_l[], short pcm_r[]);

int  do_layer3_sideinfo(PMPSTR mp);
int  do_layer3( PMPSTR mp,unsigned char *pcm_sample,int *pcm_point,
                int (*synth_1to1_mono_ptr)(PMPSTR,real *,unsigned char *,int *),
                int (*synth_1to1_ptr)(PMPSTR,real *,int,unsigned char *, int *) );

#endif
