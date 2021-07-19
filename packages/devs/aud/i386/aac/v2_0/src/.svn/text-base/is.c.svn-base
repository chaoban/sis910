/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2005 M. Bakker, Nero AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** The "appropriate copyright message" mentioned in section 2c of the GPLv2
** must read: "Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Nero AG through Mpeg4AAClicense@nero.com.
**
** $Id: is.c,v 1.28 2007/11/01 12:33:31 menno Exp $
**/
#include <math.h>
#include "common.h"
#include "aacdec.h"

#include "syntax.h"
#include "is.h"
char is_intensity(ic_stream *ics, unsigned char group, unsigned char sfb)
{
    switch (ics->sfb_cb[group][sfb])
    {
    case INTENSITY_HCB:
        return 1;
    case INTENSITY_HCB2:
        return -1;
    default:
        return 0;
    }
}

static char invert_intensity(ic_stream *ics, unsigned char group, unsigned char sfb)
{
    if (ics->ms_mask_present == 1)
        return (1-2*ics->ms_used[group][sfb]);
    return 1;
}

void is_decode(ic_stream *ics, ic_stream *icsr, real_t *l_spec, real_t *r_spec,
               unsigned short frame_len)
{
    unsigned char g, sfb, b;
    unsigned short i;
    real_t scale;
    unsigned short nshort = frame_len/8;
    unsigned char group = 0;

    for (g = 0; g < icsr->num_window_groups; g++)
    {
        /* Do intensity stereo decoding */
        for (b = 0; b < icsr->window_group_length[g]; b++)
        {
            for (sfb = 0; sfb < icsr->max_sfb; sfb++)
            {
                if (is_intensity(icsr, g, sfb))
                {
#ifdef MAIN_DEC
                    /* For scalefactor bands coded in intensity stereo the
                       corresponding predictors in the right channel are
                       switched to "off".
                     */
                    ics->pred.prediction_used[sfb] = 0;
                    icsr->pred.prediction_used[sfb] = 0;
#endif

                    scale = (real_t)pow(0.5, (0.25*icsr->scale_factors[g][sfb]));

                    /* Scale from left to right channel,
                       do not touch left channel */
                    for (i = icsr->swb_offset[sfb]; i < min(icsr->swb_offset[sfb+1], ics->swb_offset_max); i++)
                    {
                        r_spec[(group*nshort)+i] = MUL_R(l_spec[(group*nshort)+i], scale);
                        if (is_intensity(icsr, g, sfb) != invert_intensity(ics, g, sfb))
                            r_spec[(group*nshort)+i] = -r_spec[(group*nshort)+i];
                    }
                }
            }
            group++;
        }
    }
}
