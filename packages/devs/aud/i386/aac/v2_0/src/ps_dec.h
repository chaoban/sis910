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
** $Id: ps_dec.h,v 1.12 2007/11/01 12:33:33 menno Exp $
**/

#ifndef __PS_DEC_H__
#define __PS_DEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bits.h"

#define EXTENSION_ID_PS 2

#define MAX_PS_ENVELOPES 5
#define NO_ALLPASS_LINKS 3

typedef struct
{
    /* bitstream parameters */
    unsigned char enable_iid;
    unsigned char enable_icc;
    unsigned char enable_ext;

    unsigned char iid_mode;
    unsigned char icc_mode;
    unsigned char nr_iid_par;
    unsigned char nr_ipdopd_par;
    unsigned char nr_icc_par;

    unsigned char frame_class;
    unsigned char num_env;

    unsigned char border_position[MAX_PS_ENVELOPES+1];

    unsigned char iid_dt[MAX_PS_ENVELOPES];
    unsigned char icc_dt[MAX_PS_ENVELOPES];

    unsigned char enable_ipdopd;
    unsigned char ipd_mode;
    unsigned char ipd_dt[MAX_PS_ENVELOPES];
    unsigned char opd_dt[MAX_PS_ENVELOPES];

    /* indices */
    char iid_index_prev[34];
    char icc_index_prev[34];
    char ipd_index_prev[17];
    char opd_index_prev[17];
    char iid_index[MAX_PS_ENVELOPES][34];
    char icc_index[MAX_PS_ENVELOPES][34];
    char ipd_index[MAX_PS_ENVELOPES][17];
    char opd_index[MAX_PS_ENVELOPES][17];

    char ipd_index_1[17];
    char opd_index_1[17];
    char ipd_index_2[17];
    char opd_index_2[17];

    /* ps data was correctly read */
    unsigned char ps_data_available;

    /* a header has been read */
    unsigned char header_read;

    /* hybrid filterbank parameters */
    void *hyb;
    unsigned char use34hybrid_bands;

    /**/
    unsigned char num_groups;
    unsigned char num_hybrid_groups;
    unsigned char nr_par_bands;
    unsigned char nr_allpass_bands;
    unsigned char decay_cutoff;

    unsigned char *group_border;
    unsigned short *map_group2bk;

    /* filter delay handling */
    unsigned char saved_delay;
    unsigned char delay_buf_index_ser[NO_ALLPASS_LINKS];
    unsigned char num_sample_delay_ser[NO_ALLPASS_LINKS];
    unsigned char delay_D[64];
    unsigned char delay_buf_index_delay[64];

    complex_t delay_Qmf[14][64]; /* 14 samples delay max, 64 QMF channels */
    complex_t delay_SubQmf[2][32]; /* 2 samples delay max (SubQmf is always allpass filtered) */
    complex_t delay_Qmf_ser[NO_ALLPASS_LINKS][5][64]; /* 5 samples delay max (table 8.34), 64 QMF channels */
    complex_t delay_SubQmf_ser[NO_ALLPASS_LINKS][5][32]; /* 5 samples delay max (table 8.34) */

    /* transients */
    real_t alpha_decay;
    real_t alpha_smooth;

    real_t P_PeakDecayNrg[34];
    real_t P_prev[34];
    real_t P_SmoothPeakDecayDiffNrg_prev[34];

    /* mixing and phase */
    complex_t h11_prev[50];
    complex_t h12_prev[50];
    complex_t h21_prev[50];
    complex_t h22_prev[50];
    unsigned char phase_hist;
    complex_t ipd_prev[20][2];
    complex_t opd_prev[20][2];

} ps_info;

/* ps_syntax.c */
unsigned short ps_data(ps_info *ps, bitfile *ld, unsigned char *header);

/* ps_dec.c */
ps_info *ps_init(unsigned char sr_index);
void ps_free(ps_info *ps);

unsigned char ps_decode(ps_info *ps, qmf_t X_left[38][64], qmf_t X_right[38][64]);


#ifdef __cplusplus
}
#endif
#endif

