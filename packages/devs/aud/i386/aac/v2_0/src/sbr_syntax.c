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
** $Id: sbr_syntax.c,v 1.38 2007/11/01 12:33:36 menno Exp $
**/

#include "common.h"
#include "aacdec.h"

#ifdef SBR_DEC
#include <string.h>
#include "sbr_syntax.h"
#include "syntax.h"
#include "sbr_huff.h"
#include "sbr_fbt.h"
#include "sbr_tf_grid.h"
#include "sbr_e_nf.h"
#include "bits.h"
#ifdef PS_DEC
#include "ps_dec.h"
#endif
#ifdef DRM_PS
#include "drm_dec.h"
#endif

/* static function declarations */
/* static function declarations */
static void sbr_header(bitfile *ld, sbr_info *sbr);
static unsigned char calc_sbr_tables(sbr_info *sbr, unsigned char start_freq, unsigned char stop_freq,
                               unsigned char samplerate_mode, unsigned char freq_scale,
                               unsigned char alter_scale, unsigned char xover_band);
static unsigned char sbr_data(bitfile *ld, sbr_info *sbr);
static unsigned short sbr_extension(bitfile *ld, sbr_info *sbr,
                              unsigned char bs_extension_id, unsigned short num_bits_left);
static unsigned char sbr_single_channel_element(bitfile *ld, sbr_info *sbr);
static unsigned char sbr_channel_pair_element(bitfile *ld, sbr_info *sbr);
static unsigned char sbr_grid(bitfile *ld, sbr_info *sbr, unsigned char ch);
static void sbr_dtdf(bitfile *ld, sbr_info *sbr, unsigned char ch);
static void invf_mode(bitfile *ld, sbr_info *sbr, unsigned char ch);
static void sinusoidal_coding(bitfile *ld, sbr_info *sbr, unsigned char ch);


static void sbr_reset(sbr_info *sbr)
{
#if 0
    printf("%d\n", sbr->bs_start_freq_prev);
    printf("%d\n", sbr->bs_stop_freq_prev);
    printf("%d\n", sbr->bs_freq_scale_prev);
    printf("%d\n", sbr->bs_alter_scale_prev);
    printf("%d\n", sbr->bs_xover_band_prev);
    printf("%d\n\n", sbr->bs_noise_bands_prev);
#endif

    /* if these are different from the previous frame: Reset = 1 */
    if ((sbr->bs_start_freq != sbr->bs_start_freq_prev) ||
        (sbr->bs_stop_freq != sbr->bs_stop_freq_prev) ||
        (sbr->bs_freq_scale != sbr->bs_freq_scale_prev) ||
        (sbr->bs_alter_scale != sbr->bs_alter_scale_prev) ||
        (sbr->bs_xover_band != sbr->bs_xover_band_prev) ||
        (sbr->bs_noise_bands != sbr->bs_noise_bands_prev))
    {
        sbr->Reset = 1;
    } else {
        sbr->Reset = 0;
    }

    sbr->bs_start_freq_prev = sbr->bs_start_freq;
    sbr->bs_stop_freq_prev = sbr->bs_stop_freq;
    sbr->bs_freq_scale_prev = sbr->bs_freq_scale;
    sbr->bs_alter_scale_prev = sbr->bs_alter_scale;
    sbr->bs_xover_band_prev = sbr->bs_xover_band;
    sbr->bs_noise_bands_prev = sbr->bs_noise_bands;
}

static unsigned char calc_sbr_tables(sbr_info *sbr, unsigned char start_freq, unsigned char stop_freq,
                               unsigned char samplerate_mode, unsigned char freq_scale,
                               unsigned char alter_scale, unsigned char xover_band)
{
    unsigned char result = 0;
    unsigned char k2;

    /* calculate the Master Frequency Table */
    sbr->k0 = qmf_start_channel(start_freq, samplerate_mode, sbr->sample_rate);
    k2 = qmf_stop_channel(stop_freq, sbr->sample_rate, sbr->k0);

    /* check k0 and k2 */
    if (sbr->sample_rate >= 48000)
    {
        if ((k2 - sbr->k0) > 32)
            result += 1;
    } else if (sbr->sample_rate <= 32000) {
        if ((k2 - sbr->k0) > 48)
            result += 1;
    } else { /* (sbr->sample_rate == 44100) */
        if ((k2 - sbr->k0) > 45)
            result += 1;
    }

    if (freq_scale == 0)
    {
        result += master_frequency_table_fs0(sbr, sbr->k0, k2, alter_scale);
    } else {
        result += master_frequency_table(sbr, sbr->k0, k2, freq_scale, alter_scale);
    }
    result += derived_frequency_table(sbr, xover_band, k2);

    result = (result > 0) ? 1 : 0;

    return result;
}

/* table 2 */
unsigned char sbr_extension_data(bitfile *ld, sbr_info *sbr, unsigned short cnt,
                           unsigned char psResetFlag)
{
    unsigned char result = 0;
    unsigned short num_align_bits = 0;
    unsigned short num_sbr_bits1 = (unsigned short)Get_processed_bits(ld);
    unsigned short num_sbr_bits2;

    unsigned char saved_start_freq, saved_samplerate_mode;
    unsigned char saved_stop_freq, saved_freq_scale;
    unsigned char saved_alter_scale, saved_xover_band;

#if (defined(PS_DEC) || defined(DRM_PS))
    if (psResetFlag)
        sbr->psResetFlag = psResetFlag;
#endif

#ifdef DRM
    if (!sbr->Is_DRM_SBR)
#endif
    {
        unsigned char bs_extension_type = (unsigned char)Getbits(ld, 4);

        if (bs_extension_type == EXT_SBR_DATA_CRC)
        {
            sbr->bs_sbr_crc_bits = (unsigned short)Getbits(ld, 10);
        }
    }

    /* save old header values, in case the new ones are corrupted */
    saved_start_freq = sbr->bs_start_freq;
    saved_samplerate_mode = sbr->bs_samplerate_mode;
    saved_stop_freq = sbr->bs_stop_freq;
    saved_freq_scale = sbr->bs_freq_scale;
    saved_alter_scale = sbr->bs_alter_scale;
    saved_xover_band = sbr->bs_xover_band;

    sbr->bs_header_flag = Get1bit(ld);

    if (sbr->bs_header_flag)
        sbr_header(ld, sbr);

    /* Reset? */
    sbr_reset(sbr);

    /* first frame should have a header */
    //if (!(sbr->frame == 0 && sbr->bs_header_flag == 0))
    if (sbr->header_count != 0)
    {
        if (sbr->Reset || (sbr->bs_header_flag && sbr->just_seeked))
        {
            unsigned char rt = calc_sbr_tables(sbr, sbr->bs_start_freq, sbr->bs_stop_freq,
                sbr->bs_samplerate_mode, sbr->bs_freq_scale,
                sbr->bs_alter_scale, sbr->bs_xover_band);

            /* if an error occured with the new header values revert to the old ones */
            if (rt > 0)
            {
                calc_sbr_tables(sbr, saved_start_freq, saved_stop_freq,
                    saved_samplerate_mode, saved_freq_scale,
                    saved_alter_scale, saved_xover_band);
            }
        }

        if (result == 0)
        {
            result = sbr_data(ld, sbr);

            /* sbr_data() returning an error means that there was an error in
               envelope_time_border_vector().
               In this case the old time border vector is saved and all the previous
               data normally read after sbr_grid() is saved.
            */
            /* to be on the safe side, calculate old sbr tables in case of error */
            if ((result > 0) &&
                (sbr->Reset || (sbr->bs_header_flag && sbr->just_seeked)))
            {
                calc_sbr_tables(sbr, saved_start_freq, saved_stop_freq,
                    saved_samplerate_mode, saved_freq_scale,
                    saved_alter_scale, saved_xover_band);          
            }

            /* we should be able to safely set result to 0 now, */
            /* but practise indicates this doesn't work well */
        }
    } else {
        result = 1;
    }

    num_sbr_bits2 = (unsigned short)Get_processed_bits(ld) - num_sbr_bits1;

    /* check if we read more bits then were available for sbr */
    if (8*cnt < num_sbr_bits2)
    {
        Resetbits(ld, num_sbr_bits1 + 8*cnt);
        num_sbr_bits2 = 8*cnt;

#ifdef PS_DEC
        /* turn off PS for the unfortunate case that we randomly read some
         * PS data that looks correct */
        sbr->ps_used = 0;
#endif

        /* Make sure it doesn't decode SBR in this frame, or we'll get glitches */
        return 1;
    }

#ifdef DRM
    if (!sbr->Is_DRM_SBR)
#endif
    {       
        /* -4 does not apply, bs_extension_type is re-read in this function */
        num_align_bits = 8*cnt /*- 4*/ - num_sbr_bits2;

        while (num_align_bits > 7)
        {
            Getbits(ld, 8);
            num_align_bits -= 8;
        }
        Getbits(ld, num_align_bits);
    }

    return result;
}

/* table 3 */
static void sbr_header(bitfile *ld, sbr_info *sbr)
{
    unsigned char bs_header_extra_1, bs_header_extra_2;

    sbr->header_count++;

    sbr->bs_amp_res = Get1bit(ld);

    /* bs_start_freq and bs_stop_freq must define a fequency band that does
       not exceed 48 channels */
    sbr->bs_start_freq = (unsigned char)Getbits(ld, 4);
    sbr->bs_stop_freq = (unsigned char)Getbits(ld, 4);
    sbr->bs_xover_band = (unsigned char)Getbits(ld, 3);
    Getbits(ld, 2);
    bs_header_extra_1 = (unsigned char)Get1bit(ld);
    bs_header_extra_2 = (unsigned char)Get1bit(ld);

    if (bs_header_extra_1)
    {
        sbr->bs_freq_scale = (unsigned char)Getbits(ld, 2);
        sbr->bs_alter_scale = (unsigned char)Get1bit(ld);
        sbr->bs_noise_bands = (unsigned char)Getbits(ld, 2);
    } else {
        /* Default values */
        sbr->bs_freq_scale = 2;
        sbr->bs_alter_scale = 1;
        sbr->bs_noise_bands = 2;
    }

    if (bs_header_extra_2)
    {
        sbr->bs_limiter_bands = (unsigned char)Getbits(ld, 2);
        sbr->bs_limiter_gains = (unsigned char)Getbits(ld, 2);
        sbr->bs_interpol_freq = (unsigned char)Get1bit(ld);
        sbr->bs_smoothing_mode = (unsigned char)Get1bit(ld);
    } else {
        /* Default values */
        sbr->bs_limiter_bands = 2;
        sbr->bs_limiter_gains = 2;
        sbr->bs_interpol_freq = 1;
        sbr->bs_smoothing_mode = 1;
    }

#if 0
    /* print the header to screen */
    printf("bs_amp_res: %d\n", sbr->bs_amp_res);
    printf("bs_start_freq: %d\n", sbr->bs_start_freq);
    printf("bs_stop_freq: %d\n", sbr->bs_stop_freq);
    printf("bs_xover_band: %d\n", sbr->bs_xover_band);
    if (bs_header_extra_1)
    {
        printf("bs_freq_scale: %d\n", sbr->bs_freq_scale);
        printf("bs_alter_scale: %d\n", sbr->bs_alter_scale);
        printf("bs_noise_bands: %d\n", sbr->bs_noise_bands);
    }
    if (bs_header_extra_2)
    {
        printf("bs_limiter_bands: %d\n", sbr->bs_limiter_bands);
        printf("bs_limiter_gains: %d\n", sbr->bs_limiter_gains);
        printf("bs_interpol_freq: %d\n", sbr->bs_interpol_freq);
        printf("bs_smoothing_mode: %d\n", sbr->bs_smoothing_mode);
    }
    printf("\n");
#endif
}

/* table 4 */
static unsigned char sbr_data(bitfile *ld, sbr_info *sbr)
{
    unsigned char result;
#if 0
    sbr->bs_samplerate_mode = faad_get1bit(ld
        DEBUGVAR(1,219,"sbr_data(): bs_samplerate_mode"));
#endif

    sbr->rate = (sbr->bs_samplerate_mode) ? 2 : 1;

    switch (sbr->id_aac)
    {
    case ID_SCE:
		if ((result = sbr_single_channel_element(ld, sbr)) > 0)
			return result;
        break;
    case ID_CPE:
		if ((result = sbr_channel_pair_element(ld, sbr)) > 0)
			return result;
        break;
    }

	return 0;
}

/* table 5 */
static unsigned char sbr_single_channel_element(bitfile *ld, sbr_info *sbr)
{
    unsigned char result;

    if (Get1bit(ld))
    {
        Getbits(ld, 4);
    }

#ifdef DRM
    /* bs_coupling, from sbr_channel_pair_base_element(bs_amp_res) */
    if (sbr->Is_DRM_SBR)
    {
        Get1bit(ld);
    }
#endif

    if ((result = sbr_grid(ld, sbr, 0)) > 0)
        return result;

    sbr_dtdf(ld, sbr, 0);
    invf_mode(ld, sbr, 0);
    sbr_envelope(ld, sbr, 0);
    sbr_noise(ld, sbr, 0);


    envelope_noise_dequantisation(sbr, 0);


    memset(sbr->bs_add_harmonic[0], 0, 64*sizeof(unsigned char));

    sbr->bs_add_harmonic_flag[0] = Get1bit(ld);
    if (sbr->bs_add_harmonic_flag[0])
        sinusoidal_coding(ld, sbr, 0);

    sbr->bs_extended_data = Get1bit(ld);

    if (sbr->bs_extended_data)
    {
        unsigned short nr_bits_left;
#if (defined(PS_DEC) || defined(DRM_PS))
        unsigned char ps_ext_read = 0;
#endif
        unsigned short cnt = (unsigned short)Getbits(ld, 4);
        if (cnt == 15)
        {
            cnt += (unsigned short)Getbits(ld, 8);
        }

        nr_bits_left = 8 * cnt;
        while (nr_bits_left > 7)
        {
            unsigned short tmp_nr_bits = 0;

            sbr->bs_extension_id = (unsigned char)Getbits(ld, 2);
            tmp_nr_bits += 2;

            /* allow only 1 PS extension element per extension data */
#if (defined(PS_DEC) || defined(DRM_PS))
#if (defined(PS_DEC) && defined(DRM_PS))
            if (sbr->bs_extension_id == EXTENSION_ID_PS || sbr->bs_extension_id == DRM_PARAMETRIC_STEREO)
#else
#ifdef PS_DEC
            if (sbr->bs_extension_id == EXTENSION_ID_PS)
#else
#ifdef DRM_PS
            if (sbr->bs_extension_id == DRM_PARAMETRIC_STEREO)
#endif
#endif
#endif
            {
                if (ps_ext_read == 0)
                {
                    ps_ext_read = 1;
                } else {
                    /* to be safe make it 3, will switch to "default"
                     * in sbr_extension() */
#ifdef DRM
                    return 1;
#else
                    sbr->bs_extension_id = 3;
#endif
                }
            }
#endif

            tmp_nr_bits += sbr_extension(ld, sbr, sbr->bs_extension_id, nr_bits_left);

            /* check if the data read is bigger than the number of available bits */
            if (tmp_nr_bits > nr_bits_left)
                return 1;

            nr_bits_left -= tmp_nr_bits;
        }

        /* Corrigendum */
        if (nr_bits_left > 0)
        {
            Getbits(ld, nr_bits_left);
        }
    }

    return 0;
}

/* table 6 */
static unsigned char sbr_channel_pair_element(bitfile *ld, sbr_info *sbr)
{
    unsigned char n, result;

    if (Get1bit(ld))
    {
        Getbits(ld, 4);
        Getbits(ld, 4);
    }

    sbr->bs_coupling = Get1bit(ld);

    if (sbr->bs_coupling)
    {
        if ((result = sbr_grid(ld, sbr, 0)) > 0)
            return result;

        /* need to copy some data from left to right */
        sbr->bs_frame_class[1] = sbr->bs_frame_class[0];
        sbr->L_E[1] = sbr->L_E[0];
        sbr->L_Q[1] = sbr->L_Q[0];
        sbr->bs_pointer[1] = sbr->bs_pointer[0];

        for (n = 0; n <= sbr->L_E[0]; n++)
        {
            sbr->t_E[1][n] = sbr->t_E[0][n];
            sbr->f[1][n] = sbr->f[0][n];
        }
        for (n = 0; n <= sbr->L_Q[0]; n++)
            sbr->t_Q[1][n] = sbr->t_Q[0][n];

        sbr_dtdf(ld, sbr, 0);
        sbr_dtdf(ld, sbr, 1);
        invf_mode(ld, sbr, 0);

        /* more copying */
        for (n = 0; n < sbr->N_Q; n++)
            sbr->bs_invf_mode[1][n] = sbr->bs_invf_mode[0][n];

        sbr_envelope(ld, sbr, 0);
        sbr_noise(ld, sbr, 0);
        sbr_envelope(ld, sbr, 1);
        sbr_noise(ld, sbr, 1);

        memset(sbr->bs_add_harmonic[0], 0, 64*sizeof(unsigned char));
        memset(sbr->bs_add_harmonic[1], 0, 64*sizeof(unsigned char));

        sbr->bs_add_harmonic_flag[0] = Get1bit(ld);
        if (sbr->bs_add_harmonic_flag[0])
            sinusoidal_coding(ld, sbr, 0);

        sbr->bs_add_harmonic_flag[1] = Get1bit(ld);
        if (sbr->bs_add_harmonic_flag[1])
            sinusoidal_coding(ld, sbr, 1);
    } else {
        unsigned char saved_t_E[6] = {0}, saved_t_Q[3] = {0};
        unsigned char saved_L_E = sbr->L_E[0];
        unsigned char saved_L_Q = sbr->L_Q[0];
        unsigned char saved_frame_class = sbr->bs_frame_class[0];

        for (n = 0; n < saved_L_E; n++)
            saved_t_E[n] = sbr->t_E[0][n];
        for (n = 0; n < saved_L_Q; n++)
            saved_t_Q[n] = sbr->t_Q[0][n];

        if ((result = sbr_grid(ld, sbr, 0)) > 0)
            return result;
        if ((result = sbr_grid(ld, sbr, 1)) > 0)
        {
            /* restore first channel data as well */
            sbr->bs_frame_class[0] = saved_frame_class;
            sbr->L_E[0] = saved_L_E;
            sbr->L_Q[0] = saved_L_Q;
            for (n = 0; n < 6; n++)
                sbr->t_E[0][n] = saved_t_E[n];
            for (n = 0; n < 3; n++)
                sbr->t_Q[0][n] = saved_t_Q[n];

            return result;
        }
        sbr_dtdf(ld, sbr, 0);
        sbr_dtdf(ld, sbr, 1);
        invf_mode(ld, sbr, 0);
        invf_mode(ld, sbr, 1);
        sbr_envelope(ld, sbr, 0);
        sbr_envelope(ld, sbr, 1);
        sbr_noise(ld, sbr, 0);
        sbr_noise(ld, sbr, 1);

        memset(sbr->bs_add_harmonic[0], 0, 64*sizeof(unsigned char));
        memset(sbr->bs_add_harmonic[1], 0, 64*sizeof(unsigned char));

        sbr->bs_add_harmonic_flag[0] = Get1bit(ld);
        if (sbr->bs_add_harmonic_flag[0])
            sinusoidal_coding(ld, sbr, 0);

        sbr->bs_add_harmonic_flag[1] = Get1bit(ld);
        if (sbr->bs_add_harmonic_flag[1])
            sinusoidal_coding(ld, sbr, 1);
    }

    envelope_noise_dequantisation(sbr, 0);
    envelope_noise_dequantisation(sbr, 1);

    if (sbr->bs_coupling)
        unmap_envelope_noise(sbr);


    sbr->bs_extended_data = Get1bit(ld);
    if (sbr->bs_extended_data)
    {
        unsigned short nr_bits_left;
        unsigned short cnt = (unsigned short)Getbits(ld, 4);
        if (cnt == 15)
        {
            cnt += (unsigned short)Getbits(ld, 8);
        }

        nr_bits_left = 8 * cnt;
        while (nr_bits_left > 7)
        {
            unsigned short tmp_nr_bits = 0;

            sbr->bs_extension_id = (unsigned char)Getbits(ld, 2);
            tmp_nr_bits += 2;
            tmp_nr_bits += sbr_extension(ld, sbr, sbr->bs_extension_id, nr_bits_left);

            /* check if the data read is bigger than the number of available bits */
            if (tmp_nr_bits > nr_bits_left)
                return 1;

            nr_bits_left -= tmp_nr_bits;
        }

        /* Corrigendum */
        if (nr_bits_left > 0)
        {
            Getbits(ld, nr_bits_left);
        }
    }

    return 0;
}

/* integer log[2](x): input range [0,10) */
static char sbr_log2(const unsigned char val)
{
    char log2tab[] = { 0, 0, 1, 2, 2, 3, 3, 3, 3, 4 };
    if (val < 10)
        return log2tab[val];
    else
        return 0;
}


/* table 7 */
static unsigned char sbr_grid(bitfile *ld, sbr_info *sbr, unsigned char ch)
{
    unsigned char i, env, rel, result;
    unsigned char bs_abs_bord, bs_abs_bord_1;
    unsigned char bs_num_env = 0;
    unsigned char saved_L_E = sbr->L_E[ch];
    unsigned char saved_L_Q = sbr->L_Q[ch];
    unsigned char saved_frame_class = sbr->bs_frame_class[ch];

    sbr->bs_frame_class[ch] = (unsigned char)Getbits(ld, 2);

    switch (sbr->bs_frame_class[ch])
    {
    case FIXFIX:
        i = (unsigned char)Getbits(ld, 2);

        bs_num_env = min(1 << i, 5);

        i = (unsigned char)Get1bit(ld);
        for (env = 0; env < bs_num_env; env++)
            sbr->f[ch][env] = i;

        sbr->abs_bord_lead[ch] = 0;
        sbr->abs_bord_trail[ch] = sbr->numTimeSlots;
        sbr->n_rel_lead[ch] = bs_num_env - 1;
        sbr->n_rel_trail[ch] = 0;
        break;

    case FIXVAR:
        bs_abs_bord = (unsigned char)Getbits(ld, 2) + sbr->numTimeSlots;
        bs_num_env = (unsigned char)Getbits(ld, 2) + 1;

        for (rel = 0; rel < bs_num_env-1; rel++)
        {
            sbr->bs_rel_bord[ch][rel] = 2 * (unsigned char)Getbits(ld, 2) + 2;
        }
        i = sbr_log2(bs_num_env + 1);
        sbr->bs_pointer[ch] = (unsigned char)Getbits(ld, i);

        for (env = 0; env < bs_num_env; env++)
        {
            sbr->f[ch][bs_num_env - env - 1] = (unsigned char)Get1bit(ld);
        }

        sbr->abs_bord_lead[ch] = 0;
        sbr->abs_bord_trail[ch] = bs_abs_bord;
        sbr->n_rel_lead[ch] = 0;
        sbr->n_rel_trail[ch] = bs_num_env - 1;
        break;

    case VARFIX:
        bs_abs_bord = (unsigned char)Getbits(ld, 2);
        bs_num_env = (unsigned char)Getbits(ld, 2) + 1;

        for (rel = 0; rel < bs_num_env-1; rel++)
        {
            sbr->bs_rel_bord[ch][rel] = 2 * (unsigned char)Getbits(ld, 2) + 2;
        }
        i = sbr_log2(bs_num_env + 1);
        sbr->bs_pointer[ch] = (unsigned char)Getbits(ld, i);

        for (env = 0; env < bs_num_env; env++)
        {
            sbr->f[ch][env] = (unsigned char)Get1bit(ld);
        }

        sbr->abs_bord_lead[ch] = bs_abs_bord;
        sbr->abs_bord_trail[ch] = sbr->numTimeSlots;
        sbr->n_rel_lead[ch] = bs_num_env - 1;
        sbr->n_rel_trail[ch] = 0;
        break;

    case VARVAR:
        bs_abs_bord = (unsigned char)Getbits(ld, 2);
        bs_abs_bord_1 = (unsigned char)Getbits(ld, 2) + sbr->numTimeSlots;
        sbr->bs_num_rel_0[ch] = (unsigned char)Getbits(ld, 2);
        sbr->bs_num_rel_1[ch] = (unsigned char)Getbits(ld, 2);

        bs_num_env = min(5, sbr->bs_num_rel_0[ch] + sbr->bs_num_rel_1[ch] + 1);

        for (rel = 0; rel < sbr->bs_num_rel_0[ch]; rel++)
        {
            sbr->bs_rel_bord_0[ch][rel] = 2 * (unsigned char)Getbits(ld, 2) + 2;
        }
        for(rel = 0; rel < sbr->bs_num_rel_1[ch]; rel++)
        {
            sbr->bs_rel_bord_1[ch][rel] = 2 * (unsigned char)Getbits(ld, 2) + 2;
        }
        i = sbr_log2(sbr->bs_num_rel_0[ch] + sbr->bs_num_rel_1[ch] + 2);
        sbr->bs_pointer[ch] = (unsigned char)Getbits(ld, i);

        for (env = 0; env < bs_num_env; env++)
        {
            sbr->f[ch][env] = (unsigned char)Get1bit(ld);
        }

        sbr->abs_bord_lead[ch] = bs_abs_bord;
        sbr->abs_bord_trail[ch] = bs_abs_bord_1;
        sbr->n_rel_lead[ch] = sbr->bs_num_rel_0[ch];
        sbr->n_rel_trail[ch] = sbr->bs_num_rel_1[ch];
        break;
    }

    if (sbr->bs_frame_class[ch] == VARVAR)
        sbr->L_E[ch] = min(bs_num_env, 5);
    else
        sbr->L_E[ch] = min(bs_num_env, 4);

    if (sbr->L_E[ch] <= 0)
        return 1;

    if (sbr->L_E[ch] > 1)
        sbr->L_Q[ch] = 2;
    else
        sbr->L_Q[ch] = 1;

    /* TODO: this code can probably be integrated into the code above! */
    if ((result = envelope_time_border_vector(sbr, ch)) > 0)
    {
        sbr->bs_frame_class[ch] = saved_frame_class;
        sbr->L_E[ch] = saved_L_E;
        sbr->L_Q[ch] = saved_L_Q;
        return result;
    }
    noise_floor_time_border_vector(sbr, ch);

#if 0
    for (env = 0; env < bs_num_env; env++)
    {
        printf("freq_res[ch:%d][env:%d]: %d\n", ch, env, sbr->f[ch][env]);
    }
#endif

    return 0;
}

/* table 8 */
static void sbr_dtdf(bitfile *ld, sbr_info *sbr, unsigned char ch)
{
    unsigned char i;

    for (i = 0; i < sbr->L_E[ch]; i++)
    {
        sbr->bs_df_env[ch][i] = Get1bit(ld);
    }

    for (i = 0; i < sbr->L_Q[ch]; i++)
    {
        sbr->bs_df_noise[ch][i] = Get1bit(ld);
    }
}

/* table 9 */
static void invf_mode(bitfile *ld, sbr_info *sbr, unsigned char ch)
{
    unsigned char n;

    for (n = 0; n < sbr->N_Q; n++)
    {
        sbr->bs_invf_mode[ch][n] = (unsigned char)Getbits(ld, 2);
    }
}

static unsigned short sbr_extension(bitfile *ld, sbr_info *sbr,
                              unsigned char bs_extension_id, unsigned short num_bits_left)
{
#ifdef PS_DEC
    unsigned char header;
    unsigned short ret;
#endif

    switch (bs_extension_id)
    {
#ifdef PS_DEC
    case EXTENSION_ID_PS:
        if (!sbr->ps)
        {
            sbr->ps = ps_init(get_sr_index(sbr->sample_rate));
        }
        if (sbr->psResetFlag)
        {
            sbr->ps->header_read = 0;
        }
        ret = ps_data(sbr->ps, ld, &header);

        /* enable PS if and only if: a header has been decoded */
        if (sbr->ps_used == 0 && header == 1)
        {
            sbr->ps_used = 1;
        }

        if (header == 1)
        {
            sbr->psResetFlag = 0;
        }

        return ret;
#endif
#ifdef DRM_PS
    case DRM_PARAMETRIC_STEREO:
        sbr->ps_used = 1;
        if (!sbr->drm_ps)
        {
            sbr->drm_ps = drm_ps_init();
        }
        return drm_ps_data(sbr->drm_ps, ld);
#endif
    default:
        sbr->bs_extension_data = (unsigned char)Getbits(ld, 6);
        return 6;
    }
}

/* table 12 */
static void sinusoidal_coding(bitfile *ld, sbr_info *sbr, unsigned char ch)
{
    unsigned char n;

    for (n = 0; n < sbr->N_high; n++)
    {
        sbr->bs_add_harmonic[ch][n] = Get1bit(ld);
    }
}

#endif /* SBR_DEC */
