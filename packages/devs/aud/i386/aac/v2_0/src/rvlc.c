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
** $Id: rvlc.c,v 1.21 2007/11/01 12:33:34 menno Exp $
**/

/* RVLC scalefactor decoding
 *
 * RVLC works like this:
 *  1. Only symmetric huffman codewords are used
 *  2. Total length of the scalefactor data is stored in the bitsream
 *  3. Scalefactors are DPCM coded
 *  4. Next to the starting value for DPCM the ending value is also stored
 *
 * With all this it is possible to read the scalefactor data from 2 sides.
 * If there is a bit error in the scalefactor data it is possible to start
 * decoding from the other end of the data, to find all but 1 scalefactor.
 */

#include "common.h"
#include "aacdec.h"

#include <stdlib.h>

#include "syntax.h"
#include "bits.h"
#include "rvlc.h"


#ifdef ERROR_RESILIENCE

//#define PRINT_RVLC

/* static function declarations */
static unsigned char rvlc_decode_sf_forward(ic_stream *ics,
                                      bitfile *ld_sf,
                                      bitfile *ld_esc,
                                      unsigned char *is_used);
#if 0
static uint8_t rvlc_decode_sf_reverse(ic_stream *ics,
                                      bitfile *ld_sf,
                                      bitfile *ld_esc,
                                      uint8_t is_used);
#endif
static char rvlc_huffman_sf(bitfile *ld_sf, bitfile *ld_esc,
                              char direction);
static char rvlc_huffman_esc(bitfile *ld_esc, char direction);


unsigned char rvlc_scale_factor_data(ic_stream *ics, bitfile *ld)
{
    unsigned char bits = 9;

    ics->sf_concealment = Get1bit(ld);
    ics->rev_global_gain = (unsigned char)Getbits(ld, 8);

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
        bits = 11;

    /* the number of bits used for the huffman codewords */
    ics->length_of_rvlc_sf = (unsigned short)Getbits(ld, bits);

    if (ics->noise_used)
    {
        ics->dpcm_noise_nrg = (unsigned short)Getbits(ld, 9);

        ics->length_of_rvlc_sf -= 9;
    }

    ics->sf_escapes_present = Get1bit(ld);

    if (ics->sf_escapes_present)
    {
        ics->length_of_rvlc_escapes = (unsigned char)Getbits(ld, 8);
    }

    if (ics->noise_used)
    {
        ics->dpcm_noise_last_position = (unsigned short)Getbits(ld, 9);
    }

    return 0;
}

unsigned char rvlc_decode_scale_factors(ic_stream *ics, bitfile *ld)
{
    unsigned char result;
    unsigned char intensity_used = 0;
    unsigned char *rvlc_sf_buffer = NULL;
    unsigned char *rvlc_esc_buffer = NULL;
    bitfile ld_rvlc_sf, ld_rvlc_esc;
//    bitfile ld_rvlc_sf_rev, ld_rvlc_esc_rev;

    if (ics->length_of_rvlc_sf > 0)
    {
        /* We read length_of_rvlc_sf bits here to put it in a
           seperate bitfile.
        */
        rvlc_sf_buffer = Getbitbuffer(ld, ics->length_of_rvlc_sf);

        Initbits(&ld_rvlc_sf, (void*)rvlc_sf_buffer, bit2byte(ics->length_of_rvlc_sf));
//        Initbits_rev(&ld_rvlc_sf_rev, (void*)rvlc_sf_buffer,
//            ics->length_of_rvlc_sf);
    }

    if (ics->sf_escapes_present)
    {
        /* We read length_of_rvlc_escapes bits here to put it in a
           seperate bitfile.
        */
        rvlc_esc_buffer = Getbitbuffer(ld, ics->length_of_rvlc_escapes);

        Initbits(&ld_rvlc_esc, (void*)rvlc_esc_buffer, bit2byte(ics->length_of_rvlc_escapes));
//        Initbits_rev(&ld_rvlc_esc_rev, (void*)rvlc_esc_buffer,
//            ics->length_of_rvlc_escapes);
    }

    /* decode the rvlc scale factors and escapes */
    result = rvlc_decode_sf_forward(ics, &ld_rvlc_sf,
        &ld_rvlc_esc, &intensity_used);
//    result = rvlc_decode_sf_reverse(ics, &ld_rvlc_sf_rev,
//        &ld_rvlc_esc_rev, intensity_used);


    if (rvlc_esc_buffer) free(rvlc_esc_buffer);
    if (rvlc_sf_buffer) free(rvlc_sf_buffer);

    return result;
}

static unsigned char rvlc_decode_sf_forward(ic_stream *ics, bitfile *ld_sf, bitfile *ld_esc,
                                      unsigned char *intensity_used)
{
    unsigned char g, sfb;
    char t = 0;
    char error = 0;
    char noise_pcm_flag = 1;

    short scale_factor = ics->global_gain;
    short is_position = 0;
    short noise_energy = ics->global_gain - 90 - 256;

#ifdef PRINT_RVLC
    printf("\nglobal_gain: %d\n", ics->global_gain);
#endif

    for (g = 0; g < ics->num_window_groups; g++)
    {
        for (sfb = 0; sfb < ics->max_sfb; sfb++)
        {
            if (error)
            {
                ics->scale_factors[g][sfb] = 0;
            } else {
                switch (ics->sfb_cb[g][sfb])
                {
                case ZERO_HCB: /* zero book */
                    ics->scale_factors[g][sfb] = 0;
                    break;
                case INTENSITY_HCB: /* intensity books */
                case INTENSITY_HCB2:

                    *intensity_used = 1;

                    /* decode intensity position */
                    t = rvlc_huffman_sf(ld_sf, ld_esc, +1);

                    is_position += t;
                    ics->scale_factors[g][sfb] = is_position;

                    break;
                case NOISE_HCB: /* noise books */

                    /* decode noise energy */
                    if (noise_pcm_flag)
                    {
                        short n = ics->dpcm_noise_nrg;
                        noise_pcm_flag = 0;
                        noise_energy += n;
                    } else {
                        t = rvlc_huffman_sf(ld_sf, ld_esc, +1);
                        noise_energy += t;
                    }

                    ics->scale_factors[g][sfb] = noise_energy;

                    break;
                default: /* spectral books */

                    /* decode scale factor */
                    t = rvlc_huffman_sf(ld_sf, ld_esc, +1);

                    scale_factor += t;
                    if (scale_factor < 0)
                        return 4;

                    ics->scale_factors[g][sfb] = scale_factor;

                    break;
                }
#ifdef PRINT_RVLC
                printf("%3d:%4d%4d\n", sfb, ics->sfb_cb[g][sfb],
                    ics->scale_factors[g][sfb]);
#endif
                if (t == 99)
                {
                    error = 1;
                }
            }
        }
    }
#ifdef PRINT_RVLC
    printf("\n\n");
#endif

    return 0;
}

#if 0 // not used right now, doesn't work correctly yet
static uint8_t rvlc_decode_sf_reverse(ic_stream *ics, bitfile *ld_sf, bitfile *ld_esc,
                                      uint8_t intensity_used)
{
    int8_t g, sfb;
    int8_t t = 0;
    int8_t error = 0;
    int8_t noise_pcm_flag = 1, is_pcm_flag = 1, sf_pcm_flag = 1;

    int16_t scale_factor = ics->rev_global_gain;
    int16_t is_position = 0;
    int16_t noise_energy = ics->rev_global_gain;

#ifdef PRINT_RVLC
    printf("\nrev_global_gain: %d\n", ics->rev_global_gain);
#endif

    if (intensity_used)
    {
        is_position = rvlc_huffman_sf(ld_sf, ld_esc, -1);
#ifdef PRINT_RVLC
        printf("is_position: %d\n", is_position);
#endif
    }

    for (g = ics->num_window_groups-1; g >= 0; g--)
    {
        for (sfb = ics->max_sfb-1; sfb >= 0; sfb--)
        {
            if (error)
            {
                ics->scale_factors[g][sfb] = 0;
            } else {
                switch (ics->sfb_cb[g][sfb])
                {
                case ZERO_HCB: /* zero book */
                    ics->scale_factors[g][sfb] = 0;
                    break;
                case INTENSITY_HCB: /* intensity books */
                case INTENSITY_HCB2:

                    if (is_pcm_flag)
                    {
                        is_pcm_flag = 0;
                        ics->scale_factors[g][sfb] = is_position;
                    } else {
                        t = rvlc_huffman_sf(ld_sf, ld_esc, -1);
                        is_position -= t;

                        ics->scale_factors[g][sfb] = (uint8_t)is_position;
                    }
                    break;
                case NOISE_HCB: /* noise books */

                    /* decode noise energy */
                    if (noise_pcm_flag)
                    {
                        noise_pcm_flag = 0;
                        noise_energy = ics->dpcm_noise_last_position;
                    } else {
                        t = rvlc_huffman_sf(ld_sf, ld_esc, -1);
                        noise_energy -= t;
                    }

                    ics->scale_factors[g][sfb] = (uint8_t)noise_energy;
                    break;
                default: /* spectral books */

                    if (sf_pcm_flag || (sfb == 0))
                    {
                        sf_pcm_flag = 0;
                        if (sfb == 0)
                            scale_factor = ics->global_gain;
                    } else {
                        /* decode scale factor */
                        t = rvlc_huffman_sf(ld_sf, ld_esc, -1);
                        scale_factor -= t;
                    }

                    if (scale_factor < 0)
                        return 4;

                    ics->scale_factors[g][sfb] = (uint8_t)scale_factor;
                    break;
                }
#ifdef PRINT_RVLC
                printf("%3d:%4d%4d\n", sfb, ics->sfb_cb[g][sfb],
                    ics->scale_factors[g][sfb]);
#endif
                if (t == 99)
                {
                    error = 1;
                }
            }
        }
    }

#ifdef PRINT_RVLC
    printf("\n\n");
#endif

    return 0;
}
#endif

/* index == 99 means not allowed codeword */
static rvlc_huff_table book_rvlc[] = {
    /*index  length  codeword */
    {  0, 1,   0 }, /*         0 */
    { -1, 3,   5 }, /*       101 */
    {  1, 3,   7 }, /*       111 */
    { -2, 4,   9 }, /*      1001 */
    { -3, 5,  17 }, /*     10001 */
    {  2, 5,  27 }, /*     11011 */
    { -4, 6,  33 }, /*    100001 */
    { 99, 6,  50 }, /*    110010 */
    {  3, 6,  51 }, /*    110011 */
    { 99, 6,  52 }, /*    110100 */
    { -7, 7,  65 }, /*   1000001 */
    { 99, 7,  96 }, /*   1100000 */
    { 99, 7,  98 }, /*   1100010 */
    {  7, 7,  99 }, /*   1100011 */
    {  4, 7, 107 }, /*   1101011 */
    { -5, 8, 129 }, /*  10000001 */
    { 99, 8, 194 }, /*  11000010 */
    {  5, 8, 195 }, /*  11000011 */
    { 99, 8, 212 }, /*  11010100 */
    { 99, 9, 256 }, /* 100000000 */
    { -6, 9, 257 }, /* 100000001 */
    { 99, 9, 426 }, /* 110101010 */
    {  6, 9, 427 }, /* 110101011 */
    { 99, 10,  0 } /* Shouldn't come this far */
};

static rvlc_huff_table book_escape[] = {
    /*index  length  codeword */
    { 1, 2, 0 },
    { 0, 2, 2 },
    { 3, 3, 2 },
    { 2, 3, 6 },
    { 4, 4, 14 },
    { 7, 5, 13 },
    { 6, 5, 15 },
    { 5, 5, 31 },
    { 11, 6, 24 },
    { 10, 6, 25 },
    { 9, 6, 29 },
    { 8, 6, 61 },
    { 13, 7, 56  },
    { 12, 7, 120 },
    { 15, 8, 114 },
    { 14, 8, 242 },
    { 17, 9, 230 },
    { 16, 9, 486 },
    { 19, 10, 463  },
    { 18, 10, 974  },
    { 22, 11, 925  },
    { 20, 11, 1950 },
    { 21, 11, 1951 },
    { 23, 12, 1848 },
    { 25, 13, 3698 },
    { 24, 14, 7399 },
    { 26, 15, 14797 },
    { 49, 19, 236736 },
    { 50, 19, 236737 },
    { 51, 19, 236738 },
    { 52, 19, 236739 },
    { 53, 19, 236740 },
    { 27, 20, 473482 },
    { 28, 20, 473483 },
    { 29, 20, 473484 },
    { 30, 20, 473485 },
    { 31, 20, 473486 },
    { 32, 20, 473487 },
    { 33, 20, 473488 },
    { 34, 20, 473489 },
    { 35, 20, 473490 },
    { 36, 20, 473491 },
    { 37, 20, 473492 },
    { 38, 20, 473493 },
    { 39, 20, 473494 },
    { 40, 20, 473495 },
    { 41, 20, 473496 },
    { 42, 20, 473497 },
    { 43, 20, 473498 },
    { 44, 20, 473499 },
    { 45, 20, 473500 },
    { 46, 20, 473501 },
    { 47, 20, 473502 },
    { 48, 20, 473503 },
    { 99, 21,  0 } /* Shouldn't come this far */
};

static char rvlc_huffman_sf(bitfile *ld_sf, bitfile *ld_esc,
                              char direction)
{
    unsigned char i, j;
    char index;
    unsigned int cw;
    rvlc_huff_table *h = book_rvlc;
    
    i = h->len;
    if (direction > 0)
        cw = Getbits(ld_sf, i);
    else
        cw = Getbits_rev(ld_sf, i);

    while ((cw != h->cw)
        && (i < 10))
    {
        h++;
        j = h->len-i;
        i += j;
        cw <<= j;
        if (direction > 0)
            cw |= Getbits(ld_sf, j);
        else
            cw |= Getbits_rev(ld_sf, j);
    }

    index = h->index;

    if (index == +ESC_VAL)
    {
        char esc = rvlc_huffman_esc(ld_esc, direction);
        if (esc == 99)
            return 99;
        index += esc;
#ifdef PRINT_RVLC
        printf("esc: %d - ", esc);
#endif
    }
    if (index == -ESC_VAL)
    {
        char esc = rvlc_huffman_esc(ld_esc, direction);
        if (esc == 99)
            return 99;
        index -= esc;
#ifdef PRINT_RVLC
        printf("esc: %d - ", esc);
#endif
    }

    return index;
}

static char rvlc_huffman_esc(bitfile *ld,
                               char direction)
{
    unsigned char i, j;
    unsigned int cw;
    rvlc_huff_table *h = book_escape;

    i = h->len;
    if (direction > 0)
        cw = Getbits(ld, i);
    else
        cw = Getbits_rev(ld, i);

    while ((cw != h->cw)
        && (i < 21))
    {
        h++;
        j = h->len-i;
        i += j;
        cw <<= j;
        if (direction > 0)
            cw |= Getbits(ld, j);
        else
            cw |= Getbits_rev(ld, j);
    }

    return h->index;
}

#endif

