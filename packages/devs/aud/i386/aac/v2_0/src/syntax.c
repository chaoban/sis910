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
** $Id: syntax.c,v 1.89 2007/11/01 12:33:40 menno Exp $
**/

/*
   Reads the AAC bitstream as defined in 14496-3 (MPEG-4 Audio)
*/

#include "common.h"
#include "aacdec.h"

#include <stdlib.h>
#include <string.h>

#include <cyg/io/aud/adecctrl.h>
#include "syntax.h"
#include "specrec.h"
#include "huffman.h"
#include "bits.h"
#include "pulse.h"
//#include "analysis.h"
#include "drc.h"
#ifdef ERROR_RESILIENCE
#include "rvlc.h"
#endif
#ifdef SBR_DEC
#include "sbr_syntax.h"
#endif


/* static function declarations */
static void decode_sce_lfe(AACDecHandle hDecoder, AACDecFrameInfo *hInfo, bitfile *ld, unsigned char id_syn_ele);
static void decode_cpe(AACDecHandle hDecoder, AACDecFrameInfo *hInfo, bitfile *ld, unsigned char id_syn_ele);
static unsigned char single_lfe_channel_element(AACDecHandle hDecoder, bitfile *ld, unsigned char channel, unsigned char *tag);
static unsigned char channel_pair_element(AACDecHandle hDecoder, bitfile *ld, unsigned char channel, unsigned char *tag);
#ifdef COUPLING_DEC
static unsigned char coupling_channel_element(AACDecHandle hDecoder, bitfile *ld);
#endif
static unsigned short data_stream_element(AACDecHandle hDecoder, bitfile *ld);
static unsigned char program_config_element(bitfile *ld, program_config *pce);
static unsigned char fill_element(AACDecHandle hDecoder, bitfile *ld, drc_info *drc
#ifdef SBR_DEC
                            ,unsigned char sbr_ele
#endif
                            );
static unsigned char individual_channel_stream(AACDecHandle hDecoder, element *ele,
                                bitfile *ld, ic_stream *ics, unsigned char scal_flag, short *spec_data);
static unsigned char ics_info(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld, unsigned char common_window);
static unsigned char section_data(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld);
static unsigned char scale_factor_data(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld);
#ifdef SSR_DEC
static void gain_control_data(bitfile *ld, ic_stream *ics);
#endif
static unsigned char spectral_data(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld, short *spectral_data);
static unsigned short extension_payload(bitfile *ld, drc_info *drc, unsigned short count);
static unsigned char pulse_data(ic_stream *ics, pulse_info *pul, bitfile *ld);
static void tns_data(ic_stream *ics, tns_info *tns, bitfile *ld);
#ifdef LTP_DEC
static unsigned char ltp_data(AACDecHandle hDecoder, ic_stream *ics, ltp_info *ltp, bitfile *ld);
#endif
static unsigned char adts_fixed_header(bitfile *ld, adts_header *adts);
static void adts_variable_header(bitfile *ld, adts_header *adts);
static void adts_error_check(bitfile *ld, adts_header *adts);
static unsigned char dynamic_range_info(bitfile *ld, drc_info *drc);
static unsigned char excluded_channels(bitfile *ld,drc_info *drc);
static unsigned char side_info(AACDecHandle hDecoder, element *ele, bitfile *ld, ic_stream *ics, unsigned char scal_flag);
#ifdef DRM
static char DRM_aac_scalable_main_header(AACDecHandle hDecoder, ic_stream *ics1, ic_stream *ics2,
                                  bitfile *ld, unsigned char this_layer_stereo);
#endif


/* Table 4.4.1 */
char GASpecificConfig(bitfile *ld, mp4AudioSpecificConfig *mp4ASC,
                        program_config *pce_out)
{
    program_config pce;

    /* 1024 or 960 */
    mp4ASC->frameLengthFlag = Get1bit(ld);
#ifndef ALLOW_SMALL_FRAMELENGTH
    if (mp4ASC->frameLengthFlag == 1)
        return -3;
#endif

    mp4ASC->dependsOnCoreCoder = Get1bit(ld);
    if (mp4ASC->dependsOnCoreCoder == 1)
    {
        mp4ASC->coreCoderDelay = (unsigned short)Getbits(ld, 14);
    }

    mp4ASC->extensionFlag = Get1bit(ld);
    if (mp4ASC->channelsConfiguration == 0)
    {
        if (program_config_element(ld, &pce))
            return -3;
        //mp4ASC->channelsConfiguration = pce.channels;

        if (pce_out != NULL)
            memcpy(pce_out, &pce, sizeof(program_config));

        /*
        if (pce.num_valid_cc_elements)
            return -3;
        */
    }

#ifdef ERROR_RESILIENCE
    if (mp4ASC->extensionFlag == 1)
    {
        /* Error resilience not supported yet */
        if (mp4ASC->objectTypeIndex >= ER_OBJECT_START)
        {
            mp4ASC->aacSectionDataResilienceFlag = Get1bit(ld);
            mp4ASC->aacScalefactorDataResilienceFlag = Get1bit(ld);
            mp4ASC->aacSpectralDataResilienceFlag = Get1bit(ld);

            /* 1 bit: extensionFlag3 */
        }
    }
#endif

    return 0;
}

/* Table 4.4.2 */
/* An MPEG-4 Audio decoder is only required to follow the Program
   Configuration Element in GASpecificConfig(). The decoder shall ignore
   any Program Configuration Elements that may occur in raw data blocks.
   PCEs transmitted in raw data blocks cannot be used to convey decoder
   configuration information.
*/
static unsigned char program_config_element(bitfile *ld, program_config *pce)
{
    unsigned char i;

    memset(pce, 0, sizeof(program_config));

    pce->channels = 0;

    pce->element_instance_tag = (unsigned char)Getbits(ld, 4);

    pce->object_type = (unsigned char)Getbits(ld, 2);
    pce->sf_index = (unsigned char)Getbits(ld, 4);
    pce->num_front_channel_elements = (unsigned char)Getbits(ld, 4);
    pce->num_side_channel_elements = (unsigned char)Getbits(ld, 4);
    pce->num_back_channel_elements = (unsigned char)Getbits(ld, 4);
    pce->num_lfe_channel_elements = (unsigned char)Getbits(ld, 2);
    pce->num_assoc_data_elements = (unsigned char)Getbits(ld, 3);
    pce->num_valid_cc_elements = (unsigned char)Getbits(ld, 4);

    pce->mono_mixdown_present = Get1bit(ld);
    if (pce->mono_mixdown_present == 1)
    {
        pce->mono_mixdown_element_number = (unsigned char)Getbits(ld, 4);
    }

    pce->stereo_mixdown_present = Get1bit(ld);
    if (pce->stereo_mixdown_present == 1)
    {
        pce->stereo_mixdown_element_number = (unsigned char)Getbits(ld, 4);
    }

    pce->matrix_mixdown_idx_present = Get1bit(ld);
    if (pce->matrix_mixdown_idx_present == 1)
    {
        pce->matrix_mixdown_idx = (unsigned char)Getbits(ld, 2);
        pce->pseudo_surround_enable = Get1bit(ld);
    }

    for (i = 0; i < pce->num_front_channel_elements; i++)
    {
        pce->front_element_is_cpe[i] = Get1bit(ld);
        pce->front_element_tag_select[i] = (unsigned char)Getbits(ld, 4);

        if (pce->front_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->front_element_tag_select[i]] = pce->channels;
            pce->num_front_channels += 2;
            pce->channels += 2;
        } else {
            pce->sce_channel[pce->front_element_tag_select[i]] = pce->channels;
            pce->num_front_channels++;
            pce->channels++;
        }
    }

    for (i = 0; i < pce->num_side_channel_elements; i++)
    {
        pce->side_element_is_cpe[i] = Get1bit(ld);
        pce->side_element_tag_select[i] = (unsigned char)Getbits(ld, 4);

        if (pce->side_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->side_element_tag_select[i]] = pce->channels;
            pce->num_side_channels += 2;
            pce->channels += 2;
        } else {
            pce->sce_channel[pce->side_element_tag_select[i]] = pce->channels;
            pce->num_side_channels++;
            pce->channels++;
        }
    }

    for (i = 0; i < pce->num_back_channel_elements; i++)
    {
        pce->back_element_is_cpe[i] = Get1bit(ld);
        pce->back_element_tag_select[i] = (unsigned char)Getbits(ld, 4);

        if (pce->back_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->back_element_tag_select[i]] = pce->channels;
            pce->channels += 2;
            pce->num_back_channels += 2;
        } else {
            pce->sce_channel[pce->back_element_tag_select[i]] = pce->channels;
            pce->num_back_channels++;
            pce->channels++;
        }
    }

    for (i = 0; i < pce->num_lfe_channel_elements; i++)
    {
        pce->lfe_element_tag_select[i] = (unsigned char)Getbits(ld, 4);

        pce->sce_channel[pce->lfe_element_tag_select[i]] = pce->channels;
        pce->num_lfe_channels++;
        pce->channels++;
    }

    for (i = 0; i < pce->num_assoc_data_elements; i++)
        pce->assoc_data_element_tag_select[i] = (unsigned char)Getbits(ld, 4);

    for (i = 0; i < pce->num_valid_cc_elements; i++)
    {
        pce->cc_element_is_ind_sw[i] = Get1bit(ld);
        pce->valid_cc_element_tag_select[i] = (unsigned char)Getbits(ld, 4);
    }

    Byte_align(ld);

    pce->comment_field_bytes = (unsigned char)Getbits(ld, 8);

    for (i = 0; i < pce->comment_field_bytes; i++)
    {
        pce->comment_field_data[i] = (unsigned char)Getbits(ld, 8);
    }
    pce->comment_field_data[i] = 0;

    if (pce->channels > MAX_CHANNELS)
        return 22;

    return 0;
}

static void decode_sce_lfe(AACDecHandle hDecoder, AACDecFrameInfo *hInfo, bitfile *ld, unsigned char id_syn_ele)
{
    unsigned char channels = hDecoder->fr_channels;
    unsigned char tag = 0;

    if (channels+1 > MAX_CHANNELS)
    {
        hInfo->error = 12;
        return;
    }
    if (hDecoder->fr_ch_ele+1 > MAX_SYNTAX_ELEMENTS)
    {
        hInfo->error = 13;
        return;
    }

    /* for SCE hDecoder->element_output_channels[] is not set here because this
       can become 2 when some form of Parametric Stereo coding is used
    */

    /* save the syntax element id */
    hDecoder->element_id[hDecoder->fr_ch_ele] = id_syn_ele;

    /* decode the element */
    hInfo->error = single_lfe_channel_element(hDecoder, ld, channels, &tag);

    /* map output channels position to internal data channels */
    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 2)
    {
        /* this might be faulty when pce_set is true */
        hDecoder->internal_channel[channels] = channels;
        hDecoder->internal_channel[channels+1] = channels+1;
    } else {
        if (hDecoder->pce_set)
            hDecoder->internal_channel[hDecoder->pce.sce_channel[tag]] = channels;
        else
            hDecoder->internal_channel[channels] = channels;
    }

    hDecoder->fr_channels += hDecoder->element_output_channels[hDecoder->fr_ch_ele];
    hDecoder->fr_ch_ele++;
}

static void decode_cpe(AACDecHandle hDecoder, AACDecFrameInfo *hInfo,
                       bitfile *ld, unsigned char id_syn_ele)
{
    unsigned char channels = hDecoder->fr_channels;
    unsigned char tag = 0;

    if (channels+2 > MAX_CHANNELS)
    {
        hInfo->error = 12;
        return;
    }
    if (hDecoder->fr_ch_ele+1 > MAX_SYNTAX_ELEMENTS)
    {
        hInfo->error = 13;
        return;
    }

    /* for CPE the number of output channels is always 2 */
    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 0)
    {
        /* element_output_channels not set yet */
        hDecoder->element_output_channels[hDecoder->fr_ch_ele] = 2;
    } else if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] != 2) {
        /* element inconsistency */
        hInfo->error = 21;
        return;
    }

    /* save the syntax element id */
    hDecoder->element_id[hDecoder->fr_ch_ele] = id_syn_ele;

    /* decode the element */
    hInfo->error = channel_pair_element(hDecoder, ld, channels, &tag);

    /* map output channel position to internal data channels */
    if (hDecoder->pce_set)
    {
        hDecoder->internal_channel[hDecoder->pce.cpe_channel[tag]] = channels;
        hDecoder->internal_channel[hDecoder->pce.cpe_channel[tag]+1] = channels+1;
    } else {
        hDecoder->internal_channel[channels] = channels;
        hDecoder->internal_channel[channels+1] = channels+1;
    }

    hDecoder->fr_channels += 2;
    hDecoder->fr_ch_ele++;
}

void raw_data_block(bitfile *ld, AACDecHandle hDecoder, AACDecFrameInfo *hInfo, program_config *pce, drc_info *drc)
{
    unsigned char id_syn_ele;
    unsigned char ele_this_frame = 0;

    hDecoder->fr_channels = 0;
    hDecoder->fr_ch_ele = 0;
    hDecoder->first_syn_ele = 25;
    hDecoder->has_lfe = 0;

#ifdef ERROR_RESILIENCE
    if (hDecoder->object_type < ER_OBJECT_START)
    {
#endif
        /* Table 4.4.3: raw_data_block() */
        while ((id_syn_ele = (unsigned char)Getbits(ld, LEN_SE_ID)) != ID_END)
        {
            switch (id_syn_ele) {
            case ID_SCE:
                ele_this_frame++;
                if (hDecoder->first_syn_ele == 25) hDecoder->first_syn_ele = id_syn_ele;
                decode_sce_lfe(hDecoder, hInfo, ld, id_syn_ele);
                if (hInfo->error > 0)
                    return;
                break;
            case ID_CPE:
                ele_this_frame++;
                if (hDecoder->first_syn_ele == 25) hDecoder->first_syn_ele = id_syn_ele;
                decode_cpe(hDecoder, hInfo, ld, id_syn_ele);
                if (hInfo->error > 0)
                    return;
                break;
            case ID_LFE:
#ifdef DRM
                hInfo->error = 32;
#else
                ele_this_frame++;
                hDecoder->has_lfe++;
                decode_sce_lfe(hDecoder, hInfo, ld, id_syn_ele);
#endif
                if (hInfo->error > 0)
                    return;
                break;
            case ID_CCE: /* not implemented yet, but skip the bits */
#ifdef DRM
                hInfo->error = 32;
#else
                ele_this_frame++;
#ifdef COUPLING_DEC
                hInfo->error = coupling_channel_element(hDecoder, ld);
#else
                hInfo->error = 6;
#endif
#endif
                if (hInfo->error > 0)
                    return;
                break;
            case ID_DSE:
                ele_this_frame++;
                data_stream_element(hDecoder, ld);
                break;
            case ID_PCE:
                if (ele_this_frame != 0)
                {
                    hInfo->error = 31;
                    return;
                }
                ele_this_frame++;
                /* 14496-4: 5.6.4.1.2.1.3: */
                /* program_configuration_element()'s in access units shall be ignored */
                program_config_element(ld, pce);
                //if ((hInfo->error = program_config_element(ld, pce)) > 0)
                //    return;
                //hDecoder->pce_set = 1;
                break;
            case ID_FIL:
                ele_this_frame++;
                /* one sbr_info describes a channel_element not a channel! */
                /* if we encounter SBR data here: error */
                /* SBR data will be read directly in the SCE/LFE/CPE element */
                if ((hInfo->error = fill_element(hDecoder, ld, drc
#ifdef SBR_DEC
                    , INVALID_SBR_ELEMENT
#endif
                    )) > 0)
                    return;
                break;
            }
        }
#ifdef ERROR_RESILIENCE
    } else {
        /* Table 262: er_raw_data_block() */
        switch (hDecoder->channelConfiguration)
        {
        case 1:
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            if (hInfo->error > 0)
                return;
            break;
        case 2:
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            if (hInfo->error > 0)
                return;
            break;
        case 3:
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            if (hInfo->error > 0)
                return;
            break;
        case 4:
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            if (hInfo->error > 0)
                return;
            break;
        case 5:
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            if (hInfo->error > 0)
                return;
            break;
        case 6:
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_sce_lfe(hDecoder, hInfo, ld, ID_LFE);
            if (hInfo->error > 0)
                return;
            break;
        case 7: /* 8 channels */
            decode_sce_lfe(hDecoder, hInfo, ld, ID_SCE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_cpe(hDecoder, hInfo, ld, ID_CPE);
            decode_sce_lfe(hDecoder, hInfo, ld, ID_LFE);
            if (hInfo->error > 0)
                return;
            break;
        default:
            hInfo->error = 7;
            return;
        }
#if 0
        cnt = bits_to_decode() / 8;
        while (cnt >= 1)
        {
            cnt -= extension_payload(ld, cnt);
        }
#endif
    }
#endif

    /* new in corrigendum 14496-3:2002 */
#ifdef DRM
    if (hDecoder->object_type != DRM_ER_LC)
#endif
{
    Byte_align(ld);
}    
    return;
}

/* Table 4.4.4 and */
/* Table 4.4.9 */
static element sce;
static unsigned char single_lfe_channel_element(AACDecHandle hDecoder, bitfile *ld, unsigned char channel, unsigned char *tag)
{
    unsigned char retval = 0;
    memset(&sce, 0, sizeof(element));
    ic_stream *ics = &(sce.ics1);
    short *spec_data = (short *)malloc(1024 * sizeof(short));
    if(!spec_data)
    {
        return 40;
    }
    memset(spec_data, 0, 1024 * sizeof(short));
    sce.element_instance_tag = (unsigned char)Getbits(ld, LEN_TAG);

    *tag = sce.element_instance_tag;
    sce.channel = channel;
    sce.paired_channel = -1;

    retval = individual_channel_stream(hDecoder, &sce, ld, ics, 0, spec_data);
    if (retval > 0)
    {
        free(spec_data);
        return retval;
    }
    /* IS not allowed in single channel */
    if (ics->is_used)
    {
        free(spec_data);
        return 32;
    }
#ifdef SBR_DEC
    /* check if next bitstream element is a fill element */
    /* if so, read it now so SBR decoding can be done in case of a file with SBR */
    if (Showbits(ld, LEN_SE_ID) == ID_FIL)
    {
        Flushbits(ld, LEN_SE_ID);

        /* one sbr_info describes a channel_element not a channel! */
        if ((retval = fill_element(hDecoder, ld, hDecoder->drc, hDecoder->fr_ch_ele)) > 0)
        {
            free(spec_data);
            return retval;
        }
    }
#endif

    /* noiseless coding is done, spectral reconstruction is done now */
    retval = reconstruct_single_channel(hDecoder, ics, &sce, spec_data);
    if (retval > 0)
    {
        free(spec_data);
        return retval;
    }
    free(spec_data);
    return 0;
}

/* Table 4.4.5 */
static element cpe;
static unsigned char channel_pair_element(AACDecHandle hDecoder,
                                   bitfile *ld,  unsigned char channels, unsigned char *tag)
{
    short *spec_data1 = (short *)malloc(1024 * sizeof(short));
    if(!spec_data1)
    {
        return 40;
    }
    short *spec_data2 = (short *)malloc(1024 * sizeof(short));
    if(!spec_data2)
    {
        free(spec_data1);
        return 40;
    }
    memset(spec_data1, 0, 1024 * sizeof(short));
    memset(spec_data2, 0, 1024 * sizeof(short));
    memset(&cpe, 0, sizeof(element));
    ic_stream *ics1 = &(cpe.ics1);
    ic_stream *ics2 = &(cpe.ics2);
    unsigned char result;
    cpe.channel        = channels;
    cpe.paired_channel = channels+1;

    cpe.element_instance_tag = (unsigned char)Getbits(ld, LEN_TAG);
    *tag = cpe.element_instance_tag;

    if ((cpe.common_window = Get1bit(ld)) & 1)
    {
        /* both channels have common ics information */
        if ((result = ics_info(hDecoder, ics1, ld, cpe.common_window)) > 0)
        {
            free(spec_data1);
            free(spec_data2);
            return result;
        }
        ics1->ms_mask_present = (unsigned char)Getbits(ld, 2);
        if (ics1->ms_mask_present == 3)
        {
            /* bitstream error */
            free(spec_data1);
            free(spec_data2);
            return 32;
        }
        if (ics1->ms_mask_present == 1)
        {
            unsigned char g, sfb;
            for (g = 0; g < ics1->num_window_groups; g++)
            {
                for (sfb = 0; sfb < ics1->max_sfb; sfb++)
                {
                    ics1->ms_used[g][sfb] = Get1bit(ld);
                }
            }
        }

#ifdef ERROR_RESILIENCE
        if ((hDecoder->object_type >= ER_OBJECT_START) && (ics1->predictor_data_present))
        {
            if ((
#ifdef LTP_DEC
                ics1->ltp.data_present =
#endif
                Get1bit(ld)) & 1)
            {
#ifdef LTP_DEC
                if ((result = ltp_data(hDecoder, ics1, &(ics1->ltp), ld)) > 0)
                {
                    free(spec_data1);
                    free(spec_data2);
                    return result;
                }
#else
                free(spec_data1);
                free(spec_data2);
                return 26;
#endif
            }
        }
#endif

        memcpy(ics2, ics1, sizeof(ic_stream));
    } else {
        ics1->ms_mask_present = 0;
    }

    if ((result = individual_channel_stream(hDecoder, &cpe, ld, ics1,
        0, spec_data1)) > 0)
    {
        free(spec_data1);
        free(spec_data2);
        return result;
    }

#ifdef ERROR_RESILIENCE
    if (cpe.common_window && (hDecoder->object_type >= ER_OBJECT_START) &&
        (ics1->predictor_data_present))
    {
        if ((
#ifdef LTP_DEC
            ics1->ltp2.data_present =
#endif
            Get1bit(ld)) & 1)
        {
#ifdef LTP_DEC
            if ((result = ltp_data(hDecoder, ics1, &(ics1->ltp2), ld)) > 0)
            {
                free(spec_data1);
                free(spec_data2);
                return result;
            }
#else
            free(spec_data1);
            free(spec_data2);
            return 26;
#endif
        }
    }
#endif

    if ((result = individual_channel_stream(hDecoder, &cpe, ld, ics2,
        0, spec_data2)) > 0)
    {
        free(spec_data1);
        free(spec_data2);
        return result;
    }

#ifdef SBR_DEC
    /* check if next bitstream element is a fill element */
    /* if so, read it now so SBR decoding can be done in case of a file with SBR */
    if (Showbits(ld, LEN_SE_ID) == ID_FIL)
    {
        Flushbits(ld, LEN_SE_ID);

        /* one sbr_info describes a channel_element not a channel! */
        if ((result = fill_element(hDecoder, ld, hDecoder->drc, hDecoder->fr_ch_ele)) > 0)
        {
            free(spec_data1);
            free(spec_data2);
            return result;
        }
    }
#endif

    /* noiseless coding is done, spectral reconstruction is done now */
    if ((result = reconstruct_channel_pair(hDecoder, ics1, ics2, &cpe,
        spec_data1, spec_data2)) > 0)
    {
        free(spec_data1);
        free(spec_data2);
        return result;
    }
    free(spec_data1);
    free(spec_data2);
    return 0;
}

/* Table 4.4.6 */
static unsigned char ics_info(AACDecHandle hDecoder, ic_stream *ics,
                      bitfile *ld, unsigned char common_window)
{
    unsigned char retval = 0;
    unsigned char ics_reserved_bit;

    ics_reserved_bit = Get1bit(ld);
    if (ics_reserved_bit != 0)
        return 32;
    ics->window_sequence = (unsigned char)Getbits(ld, 2);
    ics->window_shape = Get1bit(ld);

#ifdef LD_DEC
    /* No block switching in LD */
    if ((hDecoder->object_type == LD) && (ics->window_sequence != ONLY_LONG_SEQUENCE))
        return 32;
#endif

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        ics->max_sfb = (unsigned char)Getbits(ld, 4);
        ics->scale_factor_grouping = (unsigned char)Getbits(ld, 7);
    } else {
        ics->max_sfb = (unsigned char)Getbits(ld, 6);
    }

    /* get the grouping information */
    if ((retval = window_grouping_info(hDecoder, ics)) > 0)
        return retval;


    /* should be an error */
    /* check the range of max_sfb */
    if (ics->max_sfb > ics->num_swb)
        return 16;

    if (ics->window_sequence != EIGHT_SHORT_SEQUENCE)
    {
        if ((ics->predictor_data_present = Get1bit(ld)) & 1)
        {
            if (hDecoder->object_type == MAIN) /* MPEG2 style AAC predictor */
            {
                unsigned char sfb;

                unsigned char limit = min(ics->max_sfb, max_pred_sfb(hDecoder->sf_index));
#ifdef MAIN_DEC
                ics->pred.limit = limit;
#endif

                if ((
#ifdef MAIN_DEC
                    ics->pred.predictor_reset =
#endif
                    Get1bit(ld)) & 1)
                {
#ifdef MAIN_DEC
                    ics->pred.predictor_reset_group_number =
#endif
                        (unsigned char)Getbits(ld, 5);
                }

                for (sfb = 0; sfb < limit; sfb++)
                {
#ifdef MAIN_DEC
                    ics->pred.prediction_used[sfb] =
#endif
                        Get1bit(ld);
                }
            }
#ifdef LTP_DEC
            else { /* Long Term Prediction */
                if (hDecoder->object_type < ER_OBJECT_START)
                {
                    if ((ics->ltp.data_present = Get1bit(ld)) & 1)
                    {
                        if ((retval = ltp_data(hDecoder, ics, &(ics->ltp), ld)) > 0)
                        {
                            return retval;
                        }
                    }
                    if (common_window)
                    {
                        if ((ics->ltp2.data_present = Get1bit(ld)) & 1)
                        {
                            if ((retval = ltp_data(hDecoder, ics, &(ics->ltp2), ld)) > 0)
                            {
                                return retval;
                            }
                        }
                    }
                }
#ifdef ERROR_RESILIENCE
                if (!common_window && (hDecoder->object_type >= ER_OBJECT_START))
                {
                    if ((ics->ltp.data_present = Get1bit(ld)) & 1)
                    {
                        ltp_data(hDecoder, ics, &(ics->ltp), ld);
                    }
                }
#endif
            }
#endif
        }
    }

    return retval;
}

/* Table 4.4.7 */
static unsigned char pulse_data(ic_stream *ics, pulse_info *pul, bitfile *ld)
{
    unsigned char i;

    pul->number_pulse = (unsigned char)Getbits(ld, 2);
    pul->pulse_start_sfb = (unsigned char)Getbits(ld, 6);

    /* check the range of pulse_start_sfb */
    if (pul->pulse_start_sfb > ics->num_swb)
        return 16;

    for (i = 0; i < pul->number_pulse+1; i++)
    {
        pul->pulse_offset[i] = (unsigned char)Getbits(ld, 5);
#if 0
        printf("%d\n", pul->pulse_offset[i]);
#endif
        pul->pulse_amp[i] = (unsigned char)Getbits(ld, 4);
#if 0
        printf("%d\n", pul->pulse_amp[i]);
#endif
    }

    return 0;
}

#ifdef COUPLING_DEC
/* Table 4.4.8: Currently just for skipping the bits... */
static unsigned char coupling_channel_element(AACDecHandle hDecoder, bitfile *ld)
{
    unsigned char c, result = 0;
    unsigned char ind_sw_cce_flag = 0;
    unsigned char num_gain_element_lists = 0;
    unsigned char num_coupled_elements = 0;

    element *el_empty = (element *)malloc(sizeof(element));
    if(!el_empty)
    {
        return 40;
    }
    memset(el_empty, 0, sizeof(element));
    ic_stream *ics_empty = (ic_stream *)malloc(sizeof(ic_stream));
    if(!ics_empty)
    {
        free(el_empty);
        return 40;
    }
    memset(ics_empty, 0, sizeof(ic_stream));
    short *sh_data = (short *)malloc(1024 * sizeof(short));
    if(!sh_data)
    {
        free(el_empty);
        free(ics_empty);
        return 40;
    }

    c = Getbits(ld, LEN_TAG);

    ind_sw_cce_flag = Get1bit(ld);
    num_coupled_elements = Getbits(ld, 3);

    for (c = 0; c < num_coupled_elements + 1; c++)
    {
        unsigned char cc_target_is_cpe, cc_target_tag_select;

        num_gain_element_lists++;

        cc_target_is_cpe = Get1bit(ld);
        cc_target_tag_select = Getbits(ld, 4);

        if (cc_target_is_cpe)
        {
            unsigned char cc_l = Get1bit(ld);
            unsigned char cc_r = Get1bit(ld);

            if (cc_l && cc_r)
                num_gain_element_lists++;
        }
    }

    Get1bit(ld);
    Get1bit(ld);
    Getbits(ld, 2);

    if ((result = individual_channel_stream(hDecoder, el_empty, ld, ics_empty,
        0, sh_data)) > 0)
    {
        free(el_empty);
        free(ics_empty);
        free(sh_data);
        return result;
    }

    /* IS not allowed in single channel */
    if (ics->is_used)
    {
        free(el_empty);
        free(ics_empty);
        free(sh_data);
        return 32;
    }
    for (c = 1; c < num_gain_element_lists; c++)
    {
        unsigned char cge;

        if (ind_sw_cce_flag)
        {
            cge = 1;
        } else {
            cge = Get1bit(ld);
        }

        if (cge)
        {
            huffman_scale_factor(ld);
        } else {
            unsigned char g, sfb;

            for (g = 0; g < ics_empty->num_window_groups; g++)
            {
                for (sfb = 0; sfb < ics_empty->max_sfb; sfb++)
                {
                    if (ics_empty->sfb_cb[g][sfb] != ZERO_HCB)
                        huffman_scale_factor(ld);
                }
            }
        }
    }
    free(el_empty);
    free(ics_empty);
    free(sh_data);
    return 0;
}
#endif

/* Table 4.4.10 */
static unsigned short data_stream_element(AACDecHandle hDecoder, bitfile *ld)
{
    unsigned char byte_aligned;
    unsigned short i, count;

    /* element_instance_tag = */ Getbits(ld, LEN_TAG);
    byte_aligned = Get1bit(ld);
    count = (unsigned short)Getbits(ld, 8);
    if (count == 255)
    {
        count += (unsigned short)Getbits(ld, 8);
    }
    if (byte_aligned)
        Byte_align(ld);

    for (i = 0; i < count; i++)
    {
        Getbits(ld, LEN_BYTE);
    }

    return count;
}

/* Table 4.4.11 */
static unsigned char fill_element(AACDecHandle hDecoder, bitfile *ld, drc_info *drc
#ifdef SBR_DEC
                            ,unsigned char sbr_ele
#endif
                            )
{
    unsigned short count;
#ifdef SBR_DEC
    unsigned char bs_extension_type;
#endif

    count = (unsigned short)Getbits(ld, 4);
    if (count == 15)
    {
        count += (unsigned short)Getbits(ld, 8) - 1;
    }

    if (count > 0)
    {
#ifdef SBR_DEC
        bs_extension_type = (unsigned char)Showbits(ld, 4);

        if( ((bs_extension_type == EXT_SBR_DATA) ||
            (bs_extension_type == EXT_SBR_DATA_CRC))
            && hDecoder->sbr_present_flag == 1)
        {
            if (sbr_ele == INVALID_SBR_ELEMENT)
                return 24;

            if (!hDecoder->sbr[sbr_ele])
            {
                hDecoder->sbr[sbr_ele] = sbrDecodeInit(hDecoder->frameLength,
                    hDecoder->element_id[sbr_ele], 2*get_sample_rate(hDecoder->sf_index),
                    hDecoder->downSampledSBR
#ifdef DRM
                    , 0
#endif
                    );
            }

            hDecoder->sbr_present_flag = 1;

            /* parse the SBR data */
            hDecoder->sbr[sbr_ele]->ret = sbr_extension_data(ld, hDecoder->sbr[sbr_ele], count,
                hDecoder->postSeekResetFlag);

#if (defined(PS_DEC) || defined(DRM_PS))
            if (hDecoder->sbr[sbr_ele]->ps_used)
            {
                hDecoder->ps_used[sbr_ele] = 1;

                /* set element independent flag to 1 as well */
                hDecoder->ps_used_global = 1;
            }
#endif
        } else {
#endif
#ifdef DRM
            if(hDecoder->object_type = DRM_ER_LC)
            {
                return 30;
            }
            else
            {
#endif
                while (count > 0)
                {
                    count -= extension_payload(ld, drc, count);		
                }
#ifdef DRM
            }
#endif
#ifdef SBR_DEC
        }
#endif
    }

    return 0;
}

/* Table 4.4.12 */
#ifdef SSR_DEC
static void gain_control_data(bitfile *ld, ic_stream *ics)
{
    unsigned char bd, wd, ad;
    ssr_info *ssr = &(ics->ssr);

    ssr->max_band = (unsigned char)Getbits(ld, 2);

    if (ics->window_sequence == ONLY_LONG_SEQUENCE)
    {
        for (bd = 1; bd <= ssr->max_band; bd++)
        {
            for (wd = 0; wd < 1; wd++)
            {
                ssr->adjust_num[bd][wd] = (unsigned char)Getbits(ld, 3);

                for (ad = 0; ad < ssr->adjust_num[bd][wd]; ad++)
                {
                    ssr->alevcode[bd][wd][ad] = (unsigned char)Getbits(ld, 4);
                    ssr->aloccode[bd][wd][ad] = (unsigned char)Getbits(ld, 5);
                }
            }
        }
    } else if (ics->window_sequence == LONG_START_SEQUENCE) {
        for (bd = 1; bd <= ssr->max_band; bd++)
        {
            for (wd = 0; wd < 2; wd++)
            {
                ssr->adjust_num[bd][wd] = (unsigned char)Getbits(ld, 3);

                for (ad = 0; ad < ssr->adjust_num[bd][wd]; ad++)
                {
                    ssr->alevcode[bd][wd][ad] = (unsigned char)Getbits(ld, 4);
                    if (wd == 0)
                    {
                        ssr->aloccode[bd][wd][ad] = (unsigned char)Getbits(ld, 4);
                    } else {
                        ssr->aloccode[bd][wd][ad] = (unsigned char)Getbits(ld, 2);
                    }
                }
            }
        }
    } else if (ics->window_sequence == EIGHT_SHORT_SEQUENCE) {
        for (bd = 1; bd <= ssr->max_band; bd++)
        {
            for (wd = 0; wd < 8; wd++)
            {
                ssr->adjust_num[bd][wd] = (unsigned char)Getbits(ld, 3);

                for (ad = 0; ad < ssr->adjust_num[bd][wd]; ad++)
                {
                    ssr->alevcode[bd][wd][ad] = (unsigned char)Getbits(ld, 4);
                    ssr->aloccode[bd][wd][ad] = (unsigned char)Getbits(ld, 2);
                }
            }
        }
    } else if (ics->window_sequence == LONG_STOP_SEQUENCE) {
        for (bd = 1; bd <= ssr->max_band; bd++)
        {
            for (wd = 0; wd < 2; wd++)
            {
                ssr->adjust_num[bd][wd] = (unsigned char)Getbits(ld, 3);

                for (ad = 0; ad < ssr->adjust_num[bd][wd]; ad++)
                {
                    ssr->alevcode[bd][wd][ad] = (unsigned char)Getbits(ld, 4);

                    if (wd == 0)
                    {
                        ssr->aloccode[bd][wd][ad] = (unsigned char)Getbits(ld, 4);
                    } else {
                        ssr->aloccode[bd][wd][ad] = (unsigned char)Getbits(ld, 5);
                    }
                }
            }
        }
    }
}
#endif

#ifdef DRM
/* Table 4.4.13 ASME */
void DRM_aac_scalable_main_element(AACDecHandle hDecoder, AACDecFrameInfo *hInfo,
                       bitfile *ld, program_config *pce, drc_info *drc)
{
    unsigned char retval = 0;
    unsigned char channels = hDecoder->fr_channels = 0;
    unsigned char ch;
    unsigned char this_layer_stereo = (hDecoder->channelConfiguration > 1) ? 1 : 0;
    element *cpe = (element *)malloc(sizeof(element));
    if(!cpe)
    {
        assert(0);
    }
    ic_stream *ics1 = &(cpe->ics1);
    ic_stream *ics2 = &(cpe->ics2);
    short *spec_data;
    short *spec_data1 = (short *)malloc(1024 * sizeof(short));
    if(!spec_data1)
    {
        free(cpe);
        assert(0);
    }
    short *spec_data2 = (short *)malloc(1024 * sizeof(short));
    if(!spec_data2)
    {
        free(cpe);
        free(spec_data1);
        assert(0);
    }
    memset(spec_data1, 0, 1024 * sizeof(short));
    memset(spec_data2, 0, 1024 * sizeof(short));

    hDecoder->fr_ch_ele = 0;

    hInfo->error = DRM_aac_scalable_main_header(hDecoder, ics1, ics2, ld, this_layer_stereo);
    if (hInfo->error > 0)
    {
        free(cpe);
        free(spec_data1);
        free(spec_data2);
        return;
    }
    cpe->common_window = 1;
    if (this_layer_stereo)
    {
        hDecoder->element_id[0] = ID_CPE;
        if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 0)
            hDecoder->element_output_channels[hDecoder->fr_ch_ele] = 2;
    } else {
        hDecoder->element_id[0] = ID_SCE;
    }

    if (this_layer_stereo)
    {
        cpe->channel        = 0;
        cpe->paired_channel = 1;
    }


    /* Stereo2 / Mono1 */
    ics1->tns_data_present = Get1bit(ld);

#if defined(LTP_DEC)
    ics1->ltp.data_present = Get1bit(ld);
#elif defined (DRM)
    if(Get1bit(ld)) {
         hInfo->error = 26;
         free(cpe);
         free(spec_data1);
         free(spec_data2);
         return;
    }
#else
    Get1bit(ld);
#endif    

    hInfo->error = side_info(hDecoder, cpe, ld, ics1, 1);
    if (hInfo->error > 0)
    {
        free(cpe);
        free(spec_data1);
        free(spec_data2);
        return;
    }
    if (this_layer_stereo)
    {
        /* Stereo3 */
        ics2->tns_data_present = Get1bit(ld);
#ifdef LTP_DEC
        ics1->ltp.data_present =
#endif
            Get1bit(ld);
        hInfo->error = side_info(hDecoder, cpe, ld, ics2, 1);
        if (hInfo->error > 0)
        {
            free(cpe);
            free(spec_data1);
            free(spec_data2);
            return;
        }
    }
    /* Stereo4 / Mono2 */
    if (ics1->tns_data_present)
        tns_data(ics1, &(ics1->tns), ld);
    if (this_layer_stereo)
    {
        /* Stereo5 */
        if (ics2->tns_data_present)
            tns_data(ics2, &(ics2->tns), ld);
    }

#ifdef DRM
    /* CRC check */
    if (hDecoder->object_type == DRM_ER_LC)
    {
        if ((hInfo->error = (unsigned char)faad_check_CRC(ld, (unsigned short)Get_processed_bits(ld) - 8)) > 0)
        {
            free(cpe);
            free(spec_data1);
            free(spec_data2);
            return;
        }
    }
#endif

    /* Stereo6 / Mono3 */
    /* error resilient spectral data decoding */
    if ((hInfo->error = reordered_spectral_data(hDecoder, ics1, ld, spec_data1)) > 0)
    {
        free(cpe);
        free(spec_data1);
        free(spec_data2);
        return;
    }
    if (this_layer_stereo)
    {
        /* Stereo7 */
        /* error resilient spectral data decoding */
        if ((hInfo->error = reordered_spectral_data(hDecoder, ics2, ld, spec_data2)) > 0)
        {
            free(cpe);
            free(spec_data1);
            free(spec_data2);
            return;
        }
    }


#ifdef DRM
#ifdef SBR_DEC
    /* In case of DRM we need to read the SBR info before channel reconstruction */
    if ((hDecoder->sbr_present_flag == 1) && (hDecoder->object_type == DRM_ER_LC))
    {
        bitfile ld_sbr = {0};
        unsigned int i;
        unsigned short count = 0;
        unsigned char *revbuffer;
        unsigned char *prevbufstart;
        unsigned char *pbufend;

        /* all forward bitreading should be finished at this point */
        unsigned int bitsconsumed = Get_processed_bits();
        unsigned int buffer_size = faad_origbitbuffer_size();
        unsigned char *buffer = (unsigned char*)faad_origbitbuffer();

        if (bitsconsumed + 8 > buffer_size*8)
        {
            hInfo->error = 14;
            free(cpe);
            free(spec_data1);
            free(spec_data2);
            return;
        }

        if (!hDecoder->sbr[0])
        {
            hDecoder->sbr[0] = sbrDecodeInit(hDecoder->frameLength, hDecoder->element_id[0],
                2*get_sample_rate(hDecoder->sf_index), 0 /* ds SBR */, 1);
        }

        /* Reverse bit reading of SBR data in DRM audio frame */
        revbuffer = (unsigned char*)malloc(buffer_size*sizeof(unsigned char));
        prevbufstart = revbuffer;
        pbufend = &buffer[buffer_size - 1];
        for (i = 0; i < buffer_size; i++)
            *prevbufstart++ = tabFlipbits[*pbufend--];

        /* Set SBR data */
        /* consider 8 bits from AAC-CRC */
        /* SBR buffer size is original buffer size minus AAC buffer size */
        count = (unsigned short)bit2byte(buffer_size*8 - bitsconsumed);
        faad_initbits(&ld_sbr, revbuffer, count);

        hDecoder->sbr[0]->sample_rate = get_sample_rate(hDecoder->sf_index);
        hDecoder->sbr[0]->sample_rate *= 2;

        Getbits(&ld_sbr, 8); /* Skip 8-bit CRC */

        hDecoder->sbr[0]->ret = sbr_extension_data(&ld_sbr, hDecoder->sbr[0], count, hDecoder->postSeekResetFlag);
#if (defined(PS_DEC) || defined(DRM_PS))
        if (hDecoder->sbr[0]->ps_used)
        {
            hDecoder->ps_used[0] = 1;
            hDecoder->ps_used_global = 1;
        }
#endif

        if (ld_sbr.error)
        {
            hDecoder->sbr[0]->ret = 1;
        }

        /* check CRC */
        /* no need to check it if there was already an error */
        if (hDecoder->sbr[0]->ret == 0)
            hDecoder->sbr[0]->ret = (unsigned char)faad_check_CRC(&ld_sbr, (unsigned short)Get_processed_bits(&ld_sbr) - 8);

        /* SBR data was corrupted, disable it until the next header */
        if (hDecoder->sbr[0]->ret != 0)
        {
            hDecoder->sbr[0]->header_count = 0;
        }

        if (revbuffer)
            free(revbuffer);
    }
#endif
#endif

    if (this_layer_stereo)
    {
        hInfo->error = reconstruct_channel_pair(hDecoder, ics1, ics2, cpe, spec_data1, spec_data2);
        if (hInfo->error > 0)
        {
            free(cpe);
            free(spec_data1);
            free(spec_data2);
            return;
        }
    } else {
        hInfo->error = reconstruct_single_channel(hDecoder, ics1, cpe, spec_data1);
        if (hInfo->error > 0)
        {
            free(cpe);
            free(spec_data1);
            free(spec_data2);
            return;
        }
    }

    /* map output channels position to internal data channels */
    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 2)
    {
        /* this might be faulty when pce_set is true */
        hDecoder->internal_channel[channels] = channels;
        hDecoder->internal_channel[channels+1] = channels+1;
    } else {
        hDecoder->internal_channel[channels] = channels;
    }

    hDecoder->fr_channels += hDecoder->element_output_channels[hDecoder->fr_ch_ele];
    hDecoder->fr_ch_ele++;
    free(cpe);
    free(spec_data1);
    free(spec_data2);
    return;
}

/* Table 4.4.15 */
static char DRM_aac_scalable_main_header(AACDecHandle hDecoder, ic_stream *ics1, ic_stream *ics2,
                                         bitfile *ld, unsigned char this_layer_stereo)
{
    unsigned char retval = 0;
    unsigned char ch;
    ic_stream *ics;
    unsigned char ics_reserved_bit;

    ics_reserved_bit = Get1bit(ld);
    if (ics_reserved_bit != 0)
        return 32;
    ics1->window_sequence = (unsigned char)Getbits(ld, 2);
    ics1->window_shape = Get1bit(ld);

    if (ics1->window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        ics1->max_sfb = (unsigned char)Getbits(ld, 4);
        ics1->scale_factor_grouping = (unsigned char)Getbits(ld, 7);
    } else {
        ics1->max_sfb = (unsigned char)Getbits(ld, 6);
    }

    /* get the grouping information */
    if ((retval = window_grouping_info(hDecoder, ics1)) > 0)
        return retval;

    /* should be an error */
    /* check the range of max_sfb */
    if (ics1->max_sfb > ics1->num_swb)
        return 16;

    if (this_layer_stereo)
    {
        ics1->ms_mask_present = (unsigned char)Getbits(ld, 2);
        if (ics1->ms_mask_present == 3)
        {
            /* bitstream error */
            return 32;
        }
        if (ics1->ms_mask_present == 1)
        {
            unsigned char g, sfb;
            for (g = 0; g < ics1->num_window_groups; g++)
            {
                for (sfb = 0; sfb < ics1->max_sfb; sfb++)
                {
                    ics1->ms_used[g][sfb] = Get1bit(ld);
                }
            }
        }

        memcpy(ics2, ics1, sizeof(ic_stream));
    } else {
        ics1->ms_mask_present = 0;
    }

    return 0;
}
#endif

static unsigned char side_info(AACDecHandle hDecoder, element *ele,
                           bitfile *ld, ic_stream *ics, unsigned char scal_flag)
{
    unsigned char result;

    ics->global_gain = (unsigned char)Getbits(ld, 8);

    if (!ele->common_window && !scal_flag)
    {
        if ((result = ics_info(hDecoder, ics, ld, ele->common_window)) > 0)
            return result;
    }

    if ((result = section_data(hDecoder, ics, ld)) > 0)
        return result;

    if ((result = scale_factor_data(hDecoder, ics, ld)) > 0)
        return result;

    if (!scal_flag)
    {
        /**
         **  NOTE: It could be that pulse data is available in scalable AAC too,
         **        as said in Amendment 1, this could be only the case for ER AAC,
         **        though. (have to check this out later)
         **/
        /* get pulse data */
        if ((ics->pulse_data_present = Get1bit(ld)) & 1)
        {
            if ((result = pulse_data(ics, &(ics->pul), ld)) > 0)
                return result;
        }

        /* get tns data */
        if ((ics->tns_data_present = Get1bit(ld)) & 1)
        {
#ifdef ERROR_RESILIENCE
            if (hDecoder->object_type < ER_OBJECT_START)
#endif
                tns_data(ics, &(ics->tns), ld);
        }

        /* get gain control data */
        if ((ics->gain_control_data_present = Get1bit(ld)) & 1)
        {
#ifdef SSR_DEC
            if (hDecoder->object_type != SSR)
                return 1;
            else
                gain_control_data(ld, ics);
#else
            return 1;
#endif
        }
    }

#ifdef ERROR_RESILIENCE
    if (hDecoder->aacSpectralDataResilienceFlag)
    {
        ics->length_of_reordered_spectral_data = (unsigned short)Getbits(ld, 14);

        if (hDecoder->channelConfiguration == 2)
        {
            if (ics->length_of_reordered_spectral_data > 6144)
                ics->length_of_reordered_spectral_data = 6144;
        } else {
            if (ics->length_of_reordered_spectral_data > 12288)
                ics->length_of_reordered_spectral_data = 12288;
        }

        ics->length_of_longest_codeword = (unsigned char)Getbits(ld, 6);
        if (ics->length_of_longest_codeword >= 49)
            ics->length_of_longest_codeword = 49;
    }

    /* RVLC spectral data is put here */
    if (hDecoder->aacScalefactorDataResilienceFlag)
    {
        if ((result = rvlc_decode_scale_factors(ics, ld)) > 0)
            return result;
    }
#endif

    return 0;
}

/* Table 4.4.24 */
static unsigned char individual_channel_stream(AACDecHandle hDecoder, element *ele,
                    bitfile *ld, ic_stream *ics, unsigned char scal_flag, short *spec_data)
{
    unsigned char result;

    result = side_info(hDecoder, ele, ld, ics, scal_flag);
    if (result > 0)
        return result;

    if (hDecoder->object_type >= ER_OBJECT_START) 
    {
        if (ics->tns_data_present)
            tns_data(ics, &(ics->tns), ld);
    }

#ifdef DRM
    /* CRC check */
    if (hDecoder->object_type == DRM_ER_LC)
    {
        if ((result = (unsigned char)faad_check_CRC(ld, (unsigned short)Get_processed_bits(ld) - 8)) > 0)
            return result;
    }
#endif

#ifdef ERROR_RESILIENCE
    if (hDecoder->aacSpectralDataResilienceFlag)
    {
        /* error resilient spectral data decoding */
        if ((result = reordered_spectral_data(hDecoder, ics, ld, spec_data)) > 0)
        {
            return result;
        }
    } else {
#endif
        /* decode the spectral data */
        if ((result = spectral_data(hDecoder, ics, ld, spec_data)) > 0)
        {
            return result;
        }
#ifdef ERROR_RESILIENCE
    }
#endif

    /* pulse coding reconstruction */
    if (ics->pulse_data_present)
    {
        if (ics->window_sequence != EIGHT_SHORT_SEQUENCE)
        {
            if ((result = pulse_decode(ics, spec_data, hDecoder->frameLength)) > 0)
                return result;
        } else {
            return 2; /* pulse coding not allowed for short blocks */
        }
    }

    return 0;
}

/* Table 4.4.25 */
static unsigned char section_data(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld)
{
    unsigned char g;
    unsigned char sect_esc_val, sect_bits;

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
        sect_bits = 3;
    else
        sect_bits = 5;
    sect_esc_val = (1<<sect_bits) - 1;

#if 0
    printf("\ntotal sfb %d\n", ics->max_sfb);
    printf("   sect    top     cb\n");
#endif

    for (g = 0; g < ics->num_window_groups; g++)
    {
        unsigned char k = 0;
        unsigned char i = 0;

        while (k < ics->max_sfb)
        {
#ifdef ERROR_RESILIENCE
            unsigned char vcb11 = 0;
#endif
            unsigned char sfb;
            unsigned char sect_len_incr;
            unsigned short sect_len = 0;
            unsigned char sect_cb_bits = 4;

            /* if "Getbits" detects error and returns "0", "k" is never
               incremented and we cannot leave the while loop */
            if (ld->error != 0)
                return 14;			

#ifdef ERROR_RESILIENCE
            if (hDecoder->aacSectionDataResilienceFlag)
                sect_cb_bits = 5;
#endif

            ics->sect_cb[g][i] = (unsigned char)Getbits(ld, sect_cb_bits);

            if (ics->sect_cb[g][i] == 12)
                return 32;

#if 0
            printf("%d\n", ics->sect_cb[g][i]);
#endif

            if (ics->sect_cb[g][i] == NOISE_HCB)
#ifdef DRM
                /* PNS not allowed in DRM */
                if(hDecoder->object_type == DRM_ER_LC)
                {
                    return 29;
                }
                else
                {
#endif
                    ics->noise_used = 1;
#ifdef DRM
                }
#endif            
            if (ics->sect_cb[g][i] == INTENSITY_HCB2 || ics->sect_cb[g][i] == INTENSITY_HCB)
                ics->is_used = 1;

#ifdef ERROR_RESILIENCE
            if (hDecoder->aacSectionDataResilienceFlag)
            {
                if ((ics->sect_cb[g][i] == 11) ||
                    ((ics->sect_cb[g][i] >= 16) && (ics->sect_cb[g][i] <= 32)))
                {
                    vcb11 = 1;
                }
            }
            if (vcb11)
            {
                sect_len_incr = 1;
            } else {
#endif
                sect_len_incr = (unsigned char)Getbits(ld, sect_bits);
#ifdef ERROR_RESILIENCE
            }
#endif
            while ((sect_len_incr == sect_esc_val) /* &&
                (k+sect_len < ics->max_sfb)*/)
            {
                sect_len += sect_len_incr;
                sect_len_incr = (unsigned char)Getbits(ld, sect_bits);
            }

            sect_len += sect_len_incr;

            ics->sect_start[g][i] = k;
            ics->sect_end[g][i] = k + sect_len;

#if 0
            printf("%d\n", ics->sect_start[g][i]);
#endif
#if 0
            printf("%d\n", ics->sect_end[g][i]);
#endif

            if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
            {
                if (k + sect_len > 8*15)
                    return 15;
                if (i >= 8*15)
                    return 15;
            } else {
                if (k + sect_len > MAX_SFB)
                    return 15;
                if (i >= MAX_SFB)
                    return 15;
            }

            for (sfb = k; sfb < k + sect_len; sfb++)
            {
                ics->sfb_cb[g][sfb] = ics->sect_cb[g][i];
#if 0
                printf("%d\n", ics->sfb_cb[g][sfb]);
#endif
            }

#if 0
            printf(" %6d %6d %6d\n",
                i,
                ics->sect_end[g][i],
                ics->sect_cb[g][i]);
#endif

            k += sect_len;
            i++;
        }
        ics->num_sec[g] = i;

        /* the sum of all sect_len_incr elements for a given window
         * group shall equal max_sfb */
        if (k != ics->max_sfb)
        {
            return 32;
        }
#if 0
        printf("%d\n", ics->num_sec[g]);
#endif
    }

#if 0
    printf("\n");
#endif

    return 0;
}

/*
 *  decode_scale_factors()
 *   decodes the scalefactors from the bitstream
 */
/*
 * All scalefactors (and also the stereo positions and pns energies) are
 * transmitted using Huffman coded DPCM relative to the previous active
 * scalefactor (respectively previous stereo position or previous pns energy,
 * see subclause 4.6.2 and 4.6.3). The first active scalefactor is
 * differentially coded relative to the global gain.
 */
static unsigned char decode_scale_factors(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld)
{
    unsigned char g, sfb;
    short t;
    char noise_pcm_flag = 1;

    short scale_factor = ics->global_gain;
    short is_position = 0;
    short noise_energy = ics->global_gain - 90;

    for (g = 0; g < ics->num_window_groups; g++)
    {
        for (sfb = 0; sfb < ics->max_sfb; sfb++)
        {
            switch (ics->sfb_cb[g][sfb])
            {
            case ZERO_HCB: /* zero book */
                ics->scale_factors[g][sfb] = 0;
//#define SF_PRINT
#ifdef SF_PRINT
                printf("%d\n", ics->scale_factors[g][sfb]);
#endif
                break;
            case INTENSITY_HCB: /* intensity books */
            case INTENSITY_HCB2:

                /* decode intensity position */
                t = huffman_scale_factor(ld);
                is_position += (t - 60);
                ics->scale_factors[g][sfb] = is_position;
#ifdef SF_PRINT
                printf("%d\n", ics->scale_factors[g][sfb]);
#endif

                break;
            case NOISE_HCB: /* noise books */

#ifdef DRM
               if(hDecoder->object_type == DRM_ER_LC)
               {
                   /* PNS not allowed in DRM */
                   return 29;
               }
               else
               {
#endif              
                    /* decode noise energy */
                    if (noise_pcm_flag)
                    {
                        noise_pcm_flag = 0;
                        t = (short)Getbits(ld, 9) - 256;
                    } else {
                        t = huffman_scale_factor(ld);
                        t -= 60;
                    }
                    noise_energy += t;
                    ics->scale_factors[g][sfb] = noise_energy;
#ifdef SF_PRINT
                    printf("%d\n", ics->scale_factors[g][sfb]);
#endif
#ifdef DRM
               }
#endif
                break;
            default: /* spectral books */

                /* ics->scale_factors[g][sfb] must be between 0 and 255 */

                ics->scale_factors[g][sfb] = 0;

                /* decode scale factor */
                t = huffman_scale_factor(ld);
                scale_factor += (t - 60);
                if (scale_factor < 0 || scale_factor > 255)
                    return 4;
                ics->scale_factors[g][sfb] = scale_factor;
#ifdef SF_PRINT
                printf("%d\n", ics->scale_factors[g][sfb]);
#endif

                break;
            }
        }
    }

    return 0;
}

/* Table 4.4.26 */
static unsigned char scale_factor_data(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld)
{
    unsigned char ret = 0;
#ifdef PROFILE
    short count = faad_get_ts();
#endif

#ifdef ERROR_RESILIENCE
    if (!hDecoder->aacScalefactorDataResilienceFlag)
    {
#endif
        ret = decode_scale_factors(hDecoder, ics, ld);
#ifdef ERROR_RESILIENCE
    } else {
        /* In ER AAC the parameters for RVLC are seperated from the actual
           data that holds the scale_factors.
           Strangely enough, 2 parameters for HCR are put inbetween them.
        */
        ret = rvlc_scale_factor_data(ics, ld);
    }
#endif

#ifdef PROFILE
    count = faad_get_ts() - count;
    hDecoder->scalefac_cycles += count;
#endif

    return ret;
}

/* Table 4.4.27 */
static void tns_data(ic_stream *ics, tns_info *tns, bitfile *ld)
{
    unsigned char w, filt, i, start_coef_bits, coef_bits;
    unsigned char n_filt_bits = 2;
    unsigned char length_bits = 6;
    unsigned char order_bits = 5;

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        n_filt_bits = 1;
        length_bits = 4;
        order_bits = 3;
    }

    for (w = 0; w < ics->num_windows; w++)
    {
        tns->n_filt[w] = (unsigned char)Getbits(ld, n_filt_bits);
#if 0
        printf("%d\n", tns->n_filt[w]);
#endif

        if (tns->n_filt[w])
        {
            if ((tns->coef_res[w] = Get1bit(ld)) & 1)
            {
                start_coef_bits = 4;
            } else {
                start_coef_bits = 3;
            }
#if 0
            printf("%d\n", tns->coef_res[w]);
#endif
        }

        for (filt = 0; filt < tns->n_filt[w]; filt++)
        {
            tns->length[w][filt] = (unsigned char)Getbits(ld, length_bits);
#if 0
            printf("%d\n", tns->length[w][filt]);
#endif
            tns->order[w][filt]  = (unsigned char)Getbits(ld, order_bits);
#if 0
            printf("%d\n", tns->order[w][filt]);
#endif
            if (tns->order[w][filt])
            {
                tns->direction[w][filt] = Get1bit(ld);
#if 0
                printf("%d\n", tns->direction[w][filt]);
#endif
                tns->coef_compress[w][filt] = Get1bit(ld);
#if 0
                printf("%d\n", tns->coef_compress[w][filt]);
#endif

                coef_bits = start_coef_bits - tns->coef_compress[w][filt];
                for (i = 0; i < tns->order[w][filt]; i++)
                {
                    tns->coef[w][filt][i] = (unsigned char)Getbits(ld, coef_bits);
#if 0
                    printf("%d\n", tns->coef[w][filt][i]);
#endif
                }
            }
        }
    }
}

#ifdef LTP_DEC
/* Table 4.4.28 */
static unsigned char ltp_data(AACDecHandle hDecoder, ic_stream *ics, ltp_info *ltp, bitfile *ld)
{
    unsigned char sfb, w;

    ltp->lag = 0;

#ifdef LD_DEC
    if (hDecoder->object_type == LD)
    {
        ltp->lag_update = (unsigned char)Getbits(ld, 1);

        if (ltp->lag_update)
        {
            ltp->lag = (unsigned short)Getbits(ld, 10);
        }
    } else {
#endif
        ltp->lag = (unsigned short)Getbits(ld, 11);
#ifdef LD_DEC
    }
#endif

    /* Check length of lag */
    if (ltp->lag > (hDecoder->frameLength << 1))
        return 18;

    ltp->coef = (unsigned char)Getbits(ld, 3);

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        for (w = 0; w < ics->num_windows; w++)
        {
            if ((ltp->short_used[w] = Get1bit(ld)) & 1)
            {
                ltp->short_lag_present[w] = Get1bit(ld);
                if (ltp->short_lag_present[w])
                {
                    ltp->short_lag[w] = (unsigned char)Getbits(ld, 4);
                }
            }
        }
    } else {
        ltp->last_band = (ics->max_sfb < MAX_LTP_SFB ? ics->max_sfb : MAX_LTP_SFB);

        for (sfb = 0; sfb < ltp->last_band; sfb++)
        {
            ltp->long_used[sfb] = Get1bit(ld);
        }
    }

    return 0;
}
#endif

/* Table 4.4.29 */
static unsigned char spectral_data(AACDecHandle hDecoder, ic_stream *ics, bitfile *ld, short *spectral_data)
{
    unsigned char i;
    unsigned char g;
    unsigned short inc, k, p = 0;
    unsigned char groups = 0;
    unsigned char sect_cb;
    unsigned char result;
    unsigned short nshort = hDecoder->frameLength/8;

#ifdef PROFILE
    short count = faad_get_ts();
#endif

    for(g = 0; g < ics->num_window_groups; g++)
    {
        p = groups*nshort;

        for (i = 0; i < ics->num_sec[g]; i++)
        {
            sect_cb = ics->sect_cb[g][i];

            inc = (sect_cb >= FIRST_PAIR_HCB) ? 2 : 4;

            switch (sect_cb)
            {
            case ZERO_HCB:
            case NOISE_HCB:
            case INTENSITY_HCB:
            case INTENSITY_HCB2:
//#define SD_PRINT
#ifdef SD_PRINT
                {
                    int j;
                    for (j = ics->sect_sfb_offset[g][ics->sect_start[g][i]]; j < ics->sect_sfb_offset[g][ics->sect_end[g][i]]; j++)
                    {
                        printf("%d\n", 0);
                    }
                }
#endif
//#define SFBO_PRINT
#ifdef SFBO_PRINT
                printf("%d\n", ics->sect_sfb_offset[g][ics->sect_start[g][i]]);
#endif
                p += (ics->sect_sfb_offset[g][ics->sect_end[g][i]] -
                    ics->sect_sfb_offset[g][ics->sect_start[g][i]]);
                break;
            default:
#ifdef SFBO_PRINT
                printf("%d\n", ics->sect_sfb_offset[g][ics->sect_start[g][i]]);
#endif
                for (k = ics->sect_sfb_offset[g][ics->sect_start[g][i]];
                     k < ics->sect_sfb_offset[g][ics->sect_end[g][i]]; k += inc)
                {
                    if ((result = huffman_spectral_data(sect_cb, ld, &spectral_data[p])) > 0)
                        return result;
#ifdef SD_PRINT
                    {
                        int j;
                        for (j = p; j < p+inc; j++)
                        {
                            printf("%d\n", spectral_data[j]);
                        }
                    }
#endif
                    p += inc;
                }
                break;
            }
        }
        groups += ics->window_group_length[g];
    }

#ifdef PROFILE
    count = faad_get_ts() - count;
    hDecoder->spectral_cycles += count;
#endif

    return 0;
}

/* Table 4.4.30 */
static unsigned short extension_payload(bitfile *ld, drc_info *drc, unsigned short count)
{
    unsigned short i, n, dataElementLength;
    unsigned char dataElementLengthPart;
    unsigned char align = 4, data_element_version, loopCounter;

    unsigned char extension_type = (unsigned char)Getbits(ld, 4);

    switch (extension_type)
    {
    case EXT_DYNAMIC_RANGE:
        drc->present = 1;
        n = dynamic_range_info(ld, drc);
        return n;
    case EXT_FILL_DATA:
        /* fill_nibble = */ Getbits(ld, 4); /* must be ‘0000’ */
        for (i = 0; i < count-1; i++)
        {
            /* fill_byte[i] = */ Getbits(ld, 8); /* must be ‘10100101’ */
        }
        return count;
    case EXT_DATA_ELEMENT:
        data_element_version = (unsigned char)Getbits(ld, 4);
        switch (data_element_version)
        {
        case ANC_DATA:
            loopCounter = 0;
            dataElementLength = 0;
            do {
                dataElementLengthPart = (unsigned char)Getbits(ld, 8);
                dataElementLength += dataElementLengthPart;
                loopCounter++;
            } while (dataElementLengthPart == 255);

            for (i = 0; i < dataElementLength; i++)
            {
                /* data_element_byte[i] = */ Getbits(ld, 8);
                return (dataElementLength+loopCounter+1);
            }
        default:
            align = 0;
        }
    case EXT_FIL:
    default:
        Getbits(ld, align);
        for (i = 0; i < count-1; i++)
        {
            /* other_bits[i] = */ Getbits(ld, 8);
        }
        return count;
    }
}

/* Table 4.4.31 */
static unsigned char dynamic_range_info(bitfile *ld, drc_info *drc)
{
    unsigned char i, n = 1;
    unsigned char band_incr;

    drc->num_bands = 1;

    if (Get1bit(ld) & 1)
    {
        drc->pce_instance_tag = (unsigned char)Getbits(ld, 4);
        /* drc->drc_tag_reserved_bits = */ Getbits(ld, 4);
        n++;
    }

    drc->excluded_chns_present = Get1bit(ld);
    if (drc->excluded_chns_present == 1)
    {
        n += excluded_channels(ld, drc);
    }

    if (Get1bit(ld) & 1)
    {
        band_incr = (unsigned char)Getbits(ld, 4);
        /* drc->drc_bands_reserved_bits = */ Getbits(ld, 4);
        n++;
        drc->num_bands += band_incr;

        for (i = 0; i < drc->num_bands; i++)
        {
            drc->band_top[i] = (unsigned char)Getbits(ld, 8);
            n++;
        }
    }

    if (Get1bit(ld) & 1)
    {
        drc->prog_ref_level = (unsigned char)Getbits(ld, 7);
        /* drc->prog_ref_level_reserved_bits = */ Get1bit(ld);
        n++;
    }

    for (i = 0; i < drc->num_bands; i++)
    {
        drc->dyn_rng_sgn[i] = Get1bit(ld);
        drc->dyn_rng_ctl[i] = (unsigned char)Getbits(ld, 7);
        n++;
    }

    return n;
}

/* Table 4.4.32 */
static unsigned char excluded_channels(bitfile *ld, drc_info *drc)
{
    unsigned char i, n = 0;
    unsigned char num_excl_chan = 7;

    for (i = 0; i < 7; i++)
    {
        drc->exclude_mask[i] = Get1bit(ld);
    }
    n++;

    while ((drc->additional_excluded_chns[n-1] = Get1bit(ld)) == 1)
    {
        for (i = num_excl_chan; i < num_excl_chan+7; i++)
        {
            drc->exclude_mask[i] = Get1bit(ld);
        }
        n++;
        num_excl_chan += 7;
    }

    return n;
}

/* Annex A: Audio Interchange Formats */

/* Table 1.A.2 */
void get_adif_header(bitfile *ld, adif_header *adif)
{
    unsigned char i;

    /* adif_id[0] = */ Getbits(ld, 8);
    /* adif_id[1] = */ Getbits(ld, 8);
    /* adif_id[2] = */ Getbits(ld, 8);
    /* adif_id[3] = */ Getbits(ld, 8);
    adif->copyright_id_present = Get1bit(ld);
    if(adif->copyright_id_present)
    {
        for (i = 0; i < 72/8; i++)
        {
            adif->copyright_id[i] = (char)Getbits(ld, 8);
        }
        adif->copyright_id[i] = 0;
    }
    adif->original_copy  = Get1bit(ld);
    adif->home = Get1bit(ld);
    adif->bitstream_type = Get1bit(ld);
    adif->bitrate = Getbits(ld, 23);
    adif->num_program_config_elements = (unsigned char)Getbits(ld, 4);

    for (i = 0; i < adif->num_program_config_elements + 1; i++)
    {
        if(adif->bitstream_type == 0)
        {
            adif->adif_buffer_fullness = Getbits(ld, 20);
        } else {
            adif->adif_buffer_fullness = 0;
        }

        program_config_element(ld, &adif->pce[i]);
    }
}

/* Table 1.A.5 */
unsigned char adts_frame(bitfile *ld, adts_header *adts)
{
    /* Byte_align(NULL); */
    if (adts_fixed_header(ld, adts))
        return 5;
    adts_variable_header(ld, adts);
    adts_error_check(ld, adts);

    return 0;
}

/* Table 1.A.6 */
static unsigned char adts_fixed_header(bitfile *ld, adts_header *adts)
{
    unsigned short i;
    unsigned char sync_err = 1;

    /* try to recover from sync errors */
    for (i = 0; i < 768; i++)
    {
        adts->syncword = (unsigned short)Showbits(ld, 12);
        if (adts->syncword != 0xFFF)
        {
            Getbits(ld, 8);
        } else {
            sync_err = 0;
            Getbits(ld, 12);
            break;
        }
    }
    if (sync_err)
        return 5;

    adts->id = Get1bit(ld);
    adts->layer = (unsigned char)Getbits(ld, 2);
    adts->protection_absent = Get1bit(ld);
    adts->profile = (unsigned char)Getbits(ld, 2);
    adts->sf_index = (unsigned char)Getbits(ld, 4);
    adts->private_bit = Get1bit(ld);
    adts->channel_configuration = (unsigned char)Getbits(ld, 3);
    adts->original = Get1bit(ld);
    adts->home = Get1bit(ld);

    if (adts->old_format == 1)
    {
        /* Removed in corrigendum 14496-3:2002 */
        if (adts->id == 0)
        {
            adts->emphasis = (unsigned char)Getbits(ld, 2);
        }
    }

    return 0;
}

/* Table 1.A.7 */
static void adts_variable_header(bitfile *ld, adts_header *adts)
{
    adts->copyright_identification_bit = Get1bit(ld);
    adts->copyright_identification_start = Get1bit(ld);
    adts->aac_frame_length = (unsigned short)Getbits(ld, 13);
    adts->adts_buffer_fullness = (unsigned short)Getbits(ld, 11);
    adts->no_raw_data_blocks_in_frame = (unsigned char)Getbits(ld, 2);
}

/* Table 1.A.8 */
static void adts_error_check(bitfile *ld, adts_header *adts)
{
    if (adts->protection_absent == 0)
    {
        adts->crc_check = (unsigned short)Getbits(ld, 16);
    }
}

