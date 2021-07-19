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
** $Id: sbr_dec.h,v 1.39 2007/11/01 12:33:34 menno Exp $
**/

#ifndef __SBR_DEC_H__
#define __SBR_DEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PS_DEC
#include "ps_dec.h"
#endif
#ifdef DRM_PS
#include "drm_dec.h"
#endif

/* MAX_NTSRHFG: maximum of number_time_slots * rate + HFGen. 16*2+8 */
#define MAX_NTSRHFG 40
#define MAX_NTSR    32 /* max number_time_slots * rate, ok for DRM and not DRM mode */

/* MAX_M: maximum value for M */
#define MAX_M       49
/* MAX_L_E: maximum value for L_E */
#define MAX_L_E      5

typedef struct {
    real_t *x;
    short x_index;
    unsigned char channels;
} qmfa_info;

typedef struct {
    real_t *v;
    short v_index;
    unsigned char channels;
} qmfs_info;

typedef struct
{
    unsigned int sample_rate;
    unsigned int maxAACLine;

    unsigned char rate;
    unsigned char just_seeked;
    unsigned char ret;

    unsigned char amp_res[2];

    unsigned char k0;
    unsigned char kx;
    unsigned char M;
    unsigned char N_master;
    unsigned char N_high;
    unsigned char N_low;
    unsigned char N_Q;
    unsigned char N_L[4];
    unsigned char n[2];

    unsigned char f_master[64];
    unsigned char f_table_res[2][64];
    unsigned char f_table_noise[64];
    unsigned char f_table_lim[4][64];
#ifdef SBR_LOW_POWER
    unsigned char f_group[5][64];
    unsigned char N_G[5];
#endif

    unsigned char table_map_k_to_g[64];

    unsigned char abs_bord_lead[2];
    unsigned char abs_bord_trail[2];
    unsigned char n_rel_lead[2];
    unsigned char n_rel_trail[2];

    unsigned char L_E[2];
    unsigned char L_E_prev[2];
    unsigned char L_Q[2];

    unsigned char t_E[2][MAX_L_E+1];
    unsigned char t_Q[2][3];
    unsigned char f[2][MAX_L_E+1];
    unsigned char f_prev[2];

    real_t *G_temp_prev[2][5];
    real_t *Q_temp_prev[2][5];
    unsigned char GQ_ringbuf_index[2];

    short E[2][64][MAX_L_E];
    short E_prev[2][64];

    real_t E_orig[2][64][MAX_L_E];

    real_t E_curr[2][64][MAX_L_E];
    int Q[2][64][2];

    real_t Q_div[2][64][2];
    real_t Q_div2[2][64][2];

    int Q_prev[2][64];

    char l_A[2];
    char l_A_prev[2];

    unsigned char bs_invf_mode[2][MAX_L_E];
    unsigned char bs_invf_mode_prev[2][MAX_L_E];
    real_t bwArray[2][64];
    real_t bwArray_prev[2][64];

    unsigned char noPatches;
    unsigned char patchNoSubbands[64];
    unsigned char patchStartSubband[64];

    unsigned char bs_add_harmonic[2][64];
    unsigned char bs_add_harmonic_prev[2][64];

    unsigned short index_noise_prev[2];
    unsigned char psi_is_prev[2];

    unsigned char bs_start_freq_prev;
    unsigned char bs_stop_freq_prev;
    unsigned char bs_xover_band_prev;
    unsigned char bs_freq_scale_prev;
    unsigned char bs_alter_scale_prev;
    unsigned char bs_noise_bands_prev;

    char prevEnvIsShort[2];

    char kx_prev;
    unsigned char bsco;
    unsigned char bsco_prev;
    unsigned char M_prev;
    unsigned short frame_len;

    unsigned char Reset;
    unsigned int frame;
    unsigned int header_count;

    unsigned char id_aac;
    qmfa_info *qmfa[2];
    qmfs_info *qmfs[2];

    qmf_t Xsbr[2][MAX_NTSRHFG][64];

#ifdef DRM
    unsigned char Is_DRM_SBR;
#ifdef DRM_PS
    drm_ps_info *drm_ps;
#endif
#endif

    unsigned char numTimeSlotsRate;
    unsigned char numTimeSlots;
    unsigned char tHFGen;
    unsigned char tHFAdj;

#ifdef PS_DEC
    ps_info *ps;
#endif
#if (defined(PS_DEC) || defined(DRM_PS))
    unsigned char ps_used;
    unsigned char psResetFlag;
#endif

    /* to get it compiling */
    /* we'll see during the coding of all the tools, whether
       these are all used or not.
    */
    unsigned char bs_header_flag;
    unsigned char bs_crc_flag;
    unsigned short bs_sbr_crc_bits;
    unsigned char bs_protocol_version;
    unsigned char bs_amp_res;
    unsigned char bs_start_freq;
    unsigned char bs_stop_freq;
    unsigned char bs_xover_band;
    unsigned char bs_freq_scale;
    unsigned char bs_alter_scale;
    unsigned char bs_noise_bands;
    unsigned char bs_limiter_bands;
    unsigned char bs_limiter_gains;
    unsigned char bs_interpol_freq;
    unsigned char bs_smoothing_mode;
    unsigned char bs_samplerate_mode;
    unsigned char bs_add_harmonic_flag[2];
    unsigned char bs_add_harmonic_flag_prev[2];
    unsigned char bs_extended_data;
    unsigned char bs_extension_id;
    unsigned char bs_extension_data;
    unsigned char bs_coupling;
    unsigned char bs_frame_class[2];
    unsigned char bs_rel_bord[2][9];
    unsigned char bs_rel_bord_0[2][9];
    unsigned char bs_rel_bord_1[2][9];
    unsigned char bs_pointer[2];
    unsigned char bs_abs_bord_0[2];
    unsigned char bs_abs_bord_1[2];
    unsigned char bs_num_rel_0[2];
    unsigned char bs_num_rel_1[2];
    unsigned char bs_df_env[2][9];
    unsigned char bs_df_noise[2][3];
} sbr_info;

sbr_info *sbrDecodeInit(unsigned short framelength, unsigned char id_aac,
                        unsigned int sample_rate, unsigned char downSampledSBR
#ifdef DRM
                        , unsigned char IsDRM
#endif
                        );
void sbrDecodeEnd(sbr_info *sbr);
void sbrReset(sbr_info *sbr);

unsigned char sbrDecodeCoupleFrame(sbr_info *sbr, real_t *left_chan, real_t *right_chan,
                             const unsigned char just_seeked, const unsigned char downSampledSBR);
unsigned char sbrDecodeSingleFrame(sbr_info *sbr, real_t *channel,
                             const unsigned char just_seeked, const unsigned char downSampledSBR);
#if (defined(PS_DEC) || defined(DRM_PS))
unsigned char sbrDecodeSingleFramePS(sbr_info *sbr, real_t *left_channel, real_t *right_channel,
                               const unsigned char just_seeked, const unsigned char downSampledSBR);
#endif


#ifdef __cplusplus
}
#endif
#endif

