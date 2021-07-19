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
** $Id: pns.c,v 1.38 2007/11/01 12:33:32 menno Exp $
**/
#include <math.h>
#include "common.h"
#include "aacdec.h"

#include "pns.h"

unsigned char is_noise(ic_stream *ics, unsigned char group, unsigned char sfb)
{
    if (ics->sfb_cb[group][sfb] == NOISE_HCB)
        return 1;
    return 0;
}

/* static function declarations */
static void gen_rand_vector(real_t *spec, short scale_factor, unsigned short size,
                            unsigned char sub,
                            /* RNG states */ unsigned int *__r1, unsigned int *__r2);


/* The function gen_rand_vector(addr, size) generates a vector of length
   <size> with signed random values of average energy MEAN_NRG per random
   value. A suitable random number generator can be realized using one
   multiplication/accumulation per random value.
*/
static void gen_rand_vector(real_t *spec, short scale_factor, unsigned short size,
                                   unsigned char sub,
                                   /* RNG states */ unsigned int *__r1, unsigned int *__r2)
{
    unsigned short i;
    real_t energy = 0.0;

    real_t scale = (real_t)1.0/(real_t)size;

    for (i = 0; i < size; i++)
    {
        real_t tmp = scale*(real_t)(int)ne_rng(__r1, __r2);
        spec[i] = tmp;
        energy += tmp*tmp;
    }

    scale = (real_t)1.0/(real_t)sqrt(energy);
    scale *= (real_t)pow(2.0, 0.25 * scale_factor);
    for (i = 0; i < size; i++)
    {
        spec[i] *= scale;
    }
}

void pns_decode(ic_stream *ics_left, ic_stream *ics_right,
                real_t *spec_left, real_t *spec_right, unsigned short frame_len,
                unsigned char channel_pair, unsigned char object_type,
                /* RNG states */ unsigned int *__r1, unsigned int *__r2)
{
    unsigned char g, sfb, b;
    unsigned short size, offs;

    unsigned char group = 0;
    unsigned short nshort = frame_len >> 3;

    unsigned char sub = 0;

    for (g = 0; g < ics_left->num_window_groups; g++)
    {
        /* Do perceptual noise substitution decoding */
        for (b = 0; b < ics_left->window_group_length[g]; b++)
        {
            for (sfb = 0; sfb < ics_left->max_sfb; sfb++)
            {
                if (is_noise(ics_left, g, sfb))
                {
#ifdef LTP_DEC
                    /* Simultaneous use of LTP and PNS is not prevented in the
                       syntax. If both LTP, and PNS are enabled on the same
                       scalefactor band, PNS takes precedence, and no prediction
                       is applied to this band.
                    */
                    ics_left->ltp.long_used[sfb] = 0;
                    ics_left->ltp2.long_used[sfb] = 0;
#endif

#ifdef MAIN_DEC
                    /* For scalefactor bands coded using PNS the corresponding
                       predictors are switched to "off".
                    */
                    ics_left->pred.prediction_used[sfb] = 0;
#endif

                    offs = ics_left->swb_offset[sfb];
                    size = min(ics_left->swb_offset[sfb+1], ics_left->swb_offset_max) - offs;

                    /* Generate random vector */
                    gen_rand_vector(&spec_left[(group*nshort)+offs],
                        ics_left->scale_factors[g][sfb], size, sub, __r1, __r2);
                }

/* From the spec:
   If the same scalefactor band and group is coded by perceptual noise
   substitution in both channels of a channel pair, the correlation of
   the noise signal can be controlled by means of the ms_used field: While
   the default noise generation process works independently for each channel
   (separate generation of random vectors), the same random vector is used
   for both channels if ms_used[] is set for a particular scalefactor band
   and group. In this case, no M/S stereo coding is carried out (because M/S
   stereo coding and noise substitution coding are mutually exclusive).
   If the same scalefactor band and group is coded by perceptual noise
   substitution in only one channel of a channel pair the setting of ms_used[]
   is not evaluated.
*/
                if (channel_pair)
                {
                    if (is_noise(ics_right, g, sfb))
                    {
                        if (((ics_left->ms_mask_present == 1) &&
                            (ics_left->ms_used[g][sfb])) ||
                            (ics_left->ms_mask_present == 2))
                        {
                            unsigned short c;

                            offs = ics_right->swb_offset[sfb];
                            size = min(ics_right->swb_offset[sfb+1], ics_right->swb_offset_max) - offs;

                            for (c = 0; c < size; c++)
                            {
                                spec_right[(group*nshort) + offs + c] =
                                    spec_left[(group*nshort) + offs + c];
                            }
                        } else /*if (ics_left->ms_mask_present == 0)*/ {
#ifdef LTP_DEC
                            ics_right->ltp.long_used[sfb] = 0;
                            ics_right->ltp2.long_used[sfb] = 0;
#endif
#ifdef MAIN_DEC
                            ics_right->pred.prediction_used[sfb] = 0;
#endif

                            offs = ics_right->swb_offset[sfb];
                            size = min(ics_right->swb_offset[sfb+1], ics_right->swb_offset_max) - offs;

                            /* Generate random vector */
                            gen_rand_vector(&spec_right[(group*nshort)+offs],
                                ics_right->scale_factors[g][sfb], size, sub, __r1, __r2);
                        }
                    }
                }
            } /* sfb */
            group++;
        } /* b */
    } /* g */
}
