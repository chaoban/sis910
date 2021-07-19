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
** $Id: mp4.c,v 1.38 2007/11/01 12:33:32 menno Exp $
**/

#include "common.h"
#include "aacdec.h"

#include <stdlib.h>
#include <string.h>

#include "bits.h"
#include "mp4.h"
#include "syntax.h"

/* defines if an object type can be decoded by this library or not */
static unsigned char ObjectTypesTable[32] = {
    0, /*  0 NULL */
#ifdef MAIN_DEC
    1, /*  1 AAC Main */
#else
    0, /*  1 AAC Main */
#endif
    1, /*  2 AAC LC */
#ifdef SSR_DEC
    1, /*  3 AAC SSR */
#else
    0, /*  3 AAC SSR */
#endif
#ifdef LTP_DEC
    1, /*  4 AAC LTP */
#else
    0, /*  4 AAC LTP */
#endif
#ifdef SBR_DEC
    1, /*  5 SBR */
#else
    0, /*  5 SBR */
#endif
    0, /*  6 AAC Scalable */
    0, /*  7 TwinVQ */
    0, /*  8 CELP */
    0, /*  9 HVXC */
    0, /* 10 Reserved */
    0, /* 11 Reserved */
    0, /* 12 TTSI */
    0, /* 13 Main synthetic */
    0, /* 14 Wavetable synthesis */
    0, /* 15 General MIDI */
    0, /* 16 Algorithmic Synthesis and Audio FX */

    /* MPEG-4 Version 2 */
#ifdef ERROR_RESILIENCE
    1, /* 17 ER AAC LC */
    0, /* 18 (Reserved) */
#ifdef LTP_DEC
    1, /* 19 ER AAC LTP */
#else
    0, /* 19 ER AAC LTP */
#endif
    0, /* 20 ER AAC scalable */
    0, /* 21 ER TwinVQ */
    0, /* 22 ER BSAC */
#ifdef LD_DEC
    1, /* 23 ER AAC LD */
#else
    0, /* 23 ER AAC LD */
#endif
    0, /* 24 ER CELP */
    0, /* 25 ER HVXC */
    0, /* 26 ER HILN */
    0, /* 27 ER Parametric */
#else /* No ER defined */
    0, /* 17 ER AAC LC */
    0, /* 18 (Reserved) */
    0, /* 19 ER AAC LTP */
    0, /* 20 ER AAC scalable */
    0, /* 21 ER TwinVQ */
    0, /* 22 ER BSAC */
    0, /* 23 ER AAC LD */
    0, /* 24 ER CELP */
    0, /* 25 ER HVXC */
    0, /* 26 ER HILN */
    0, /* 27 ER Parametric */
#endif
    0, /* 28 (Reserved) */
    0, /* 29 (Reserved) */
    0, /* 30 (Reserved) */
    0  /* 31 (Reserved) */
};

/* Table 1.6.1 */
char NeAACDecAudioSpecificConfig(unsigned char *pBuffer,
                                               unsigned int buffer_size,
                                               mp4AudioSpecificConfig *mp4ASC)
{
    return AudioSpecificConfig2(pBuffer, buffer_size, mp4ASC, NULL);
}

char AudioSpecificConfig2(unsigned char *pBuffer,
                            unsigned int buffer_size,
                            mp4AudioSpecificConfig *mp4ASC,
                            program_config *pce)
{
    bitfile ld;
    char result = 0;
#ifdef SBR_DEC
    char bits_to_decode = 0;
#endif

    if (pBuffer == NULL)
        return -7;
    if (mp4ASC == NULL)
        return -8;

    memset(mp4ASC, 0, sizeof(mp4AudioSpecificConfig));
    Initbits(&ld, pBuffer, buffer_size);
    Byte_align(&ld);

    mp4ASC->objectTypeIndex = (unsigned char)Getbits(&ld, 5);

    mp4ASC->samplingFrequencyIndex = (unsigned char)Getbits(&ld, 4);

    mp4ASC->channelsConfiguration = (unsigned char)Getbits(&ld, 4);

    mp4ASC->samplingFrequency = get_sample_rate(mp4ASC->samplingFrequencyIndex);

    if (ObjectTypesTable[mp4ASC->objectTypeIndex] != 1)
    {
        return -1;
    }

    if (mp4ASC->samplingFrequency == 0)
    {
        return -2;
    }

    if (mp4ASC->channelsConfiguration > 7)
    {
        return -3;
    }

#if (defined(PS_DEC) || defined(DRM_PS))
    /* check if we have a mono file */
    if (mp4ASC->channelsConfiguration == 1)
    {
        /* upMatrix to 2 channels for implicit signalling of PS */
        mp4ASC->channelsConfiguration = 2;
    }
#endif

#ifdef SBR_DEC
    mp4ASC->sbr_present_flag = -1;
    if (mp4ASC->objectTypeIndex == 5)
    {
        unsigned char tmp;

        mp4ASC->sbr_present_flag = 1;
        tmp = (unsigned char)Getbits(&ld, 4);
        /* check for downsampled SBR */
        if (tmp == mp4ASC->samplingFrequencyIndex)
            mp4ASC->downSampledSBR = 1;
        mp4ASC->samplingFrequencyIndex = tmp;
        if (mp4ASC->samplingFrequencyIndex == 15)
        {
            mp4ASC->samplingFrequency = (unsigned int)Getbits(&ld, 24);
        } else {
            mp4ASC->samplingFrequency = get_sample_rate(mp4ASC->samplingFrequencyIndex);
        }
        mp4ASC->objectTypeIndex = (unsigned char)Getbits(&ld, 5);
    }
#endif

    /* get GASpecificConfig */
    if (mp4ASC->objectTypeIndex == 1 || mp4ASC->objectTypeIndex == 2 ||
        mp4ASC->objectTypeIndex == 3 || mp4ASC->objectTypeIndex == 4 ||
        mp4ASC->objectTypeIndex == 6 || mp4ASC->objectTypeIndex == 7)
    {
        result = GASpecificConfig(&ld, mp4ASC, pce);

#ifdef ERROR_RESILIENCE
    } else if (mp4ASC->objectTypeIndex >= ER_OBJECT_START) { /* ER */
        result = GASpecificConfig(&ld, mp4ASC, pce);
        mp4ASC->epConfig = (unsigned char)Getbits(&ld, 2);

        if (mp4ASC->epConfig != 0)
            result = -5;
#endif

    } else {
        result = -4;
    }

#ifdef SSR_DEC
    /* shorter frames not allowed for SSR */
    if ((mp4ASC->objectTypeIndex == 4) && mp4ASC->frameLengthFlag)
        return -6;
#endif


#ifdef SBR_DEC
    bits_to_decode = (char)(buffer_size*8 - Get_processed_bits(&ld));

    if ((mp4ASC->objectTypeIndex != 5) && (bits_to_decode >= 16))
    {
        short syncExtensionType = (short)Getbits(&ld, 11);

        if (syncExtensionType == 0x2b7)
        {
            unsigned char tmp_OTi = (unsigned char)Getbits(&ld, 5);

            if (tmp_OTi == 5)
            {
                mp4ASC->sbr_present_flag = (unsigned char)Get1bit(&ld);

                if (mp4ASC->sbr_present_flag)
                {
                    unsigned char tmp;

					/* Don't set OT to SBR until checked that it is actually there */
					mp4ASC->objectTypeIndex = tmp_OTi;

                    tmp = (unsigned char)Getbits(&ld, 4);

                    /* check for downsampled SBR */
                    if (tmp == mp4ASC->samplingFrequencyIndex)
                        mp4ASC->downSampledSBR = 1;
                    mp4ASC->samplingFrequencyIndex = tmp;

                    if (mp4ASC->samplingFrequencyIndex == 15)
                    {
                        mp4ASC->samplingFrequency = (unsigned int)Getbits(&ld, 24);
                    } else {
                        mp4ASC->samplingFrequency = get_sample_rate(mp4ASC->samplingFrequencyIndex);
                    }
                }
            }
        }
    }

    /* no SBR signalled, this could mean either implicit signalling or no SBR in this file */
    /* MPEG specification states: assume SBR on files with samplerate <= 24000 Hz */
    if (mp4ASC->sbr_present_flag == 1)
    {
        if (mp4ASC->samplingFrequency <= 24000)
        {
            mp4ASC->samplingFrequency *= 2;
            mp4ASC->forceUpSampling = 1;
        } else /* > 24000*/ {
            mp4ASC->downSampledSBR = 1;
        }
    }
#endif

    return result;
}
