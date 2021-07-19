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
** $Id: mdct.c,v 1.47 2007/11/01 12:33:31 menno Exp $
**/

/*
 * Fast (I)MDCT Implementation using (I)FFT ((Inverse) Fast Fourier Transform)
 * and consists of three steps: pre-(I)FFT complex multiplication, complex
 * (I)FFT, post-(I)FFT complex multiplication,
 * 
 * As described in:
 *  P. Duhamel, Y. Mahieux, and J.P. Petit, "A Fast Algorithm for the
 *  Implementation of Filter Banks Based on 'Time Domain Aliasing
 *  Cancellation’," IEEE Proc. on ICASSP‘91, 1991, pp. 2209-2212.
 *
 *
 * As of April 6th 2002 completely rewritten.
 * This (I)MDCT can now be used for any data size n, where n is divisible by 8.
 *
 */

#include "common.h"
#include "aacdec.h"

#include <stdlib.h>

#include <assert.h>

#include "cfft.h"
#include "mdct.h"
#include "mdct_tab.h"


mdct_info *faad_mdct_init(unsigned short N)
{
    mdct_info *mdct = (mdct_info*)malloc(sizeof(mdct_info));

    assert(N % 8 == 0);

    mdct->N = N;

    /* NOTE: For "small framelengths" in FIXED_POINT the coefficients need to be
     * scaled by sqrt("(nearest power of 2) > N" / N) */

    /* RE(mdct->sincos[k]) = scale*(real_t)(cos(2.0*M_PI*(k+1./8.) / (real_t)N));
     * IM(mdct->sincos[k]) = scale*(real_t)(sin(2.0*M_PI*(k+1./8.) / (real_t)N)); */
    /* scale is 1 for fixed point, sqrt(N) for floating point */
    switch (N)
    {
    case 2048: mdct->sincos = (complex_t*)mdct_tab_2048; break;
    case 256:  mdct->sincos = (complex_t*)mdct_tab_256;  break;
#ifdef LD_DEC
    case 1024: mdct->sincos = (complex_t*)mdct_tab_1024; break;
#endif
#ifdef ALLOW_SMALL_FRAMELENGTH
    case 1920: mdct->sincos = (complex_t*)mdct_tab_1920; break;
    case 240:  mdct->sincos = (complex_t*)mdct_tab_240;  break;
#ifdef LD_DEC
    case 960:  mdct->sincos = (complex_t*)mdct_tab_960;  break;
#endif
#endif
#ifdef SSR_DEC
    case 512:  mdct->sincos = (complex_t*)mdct_tab_512;  break;
    case 64:   mdct->sincos = (complex_t*)mdct_tab_64;   break;
#endif
    }

    /* initialise fft */
    mdct->cfft = cffti(N/4);

#ifdef PROFILE
    mdct->cycles = 0;
    mdct->fft_cycles = 0;
#endif

    return mdct;
}

void faad_mdct_end(mdct_info *mdct)
{
    if (mdct != NULL)
    {
#ifdef PROFILE
        printf("MDCT[%.4d]:         %I64d cycles\n", mdct->N, mdct->cycles);
        printf("CFFT[%.4d]:         %I64d cycles\n", mdct->N/4, mdct->fft_cycles);
#endif

        cfftu(mdct->cfft);

        free(mdct);
    }
}

unsigned char faad_imdct(mdct_info *mdct, real_t *X_in, real_t *X_out)
{
    unsigned short k;

    complex_t x;
    complex_t *Z1 = (complex_t *)malloc(512 * sizeof(complex_t));
    if(!Z1)
    {
        return 40;
    }
    complex_t *sincos = mdct->sincos;

    unsigned short N  = mdct->N;
    unsigned short N2 = N >> 1;
    unsigned short N4 = N >> 2;
    unsigned short N8 = N >> 3;

#ifdef PROFILE
    short count1, count2 = faad_get_ts();
#endif


    /* pre-IFFT complex multiplication */
    for (k = 0; k < N4; k++)
    {
        ComplexMult(&IM(Z1[k]), &RE(Z1[k]),
            X_in[2*k], X_in[N2 - 1 - 2*k], RE(sincos[k]), IM(sincos[k]));
    }

#ifdef PROFILE
    count1 = faad_get_ts();
#endif

    /* complex IFFT, any non-scaling FFT can be used here */
    cfftb(mdct->cfft, Z1);

#ifdef PROFILE
    count1 = faad_get_ts() - count1;
#endif

    /* post-IFFT complex multiplication */
    for (k = 0; k < N4; k++)
    {
        RE(x) = RE(Z1[k]);
        IM(x) = IM(Z1[k]);
        ComplexMult(&IM(Z1[k]), &RE(Z1[k]),
            IM(x), RE(x), RE(sincos[k]), IM(sincos[k]));

    }

    /* reordering */
    for (k = 0; k < N8; k+=2)
    {
        X_out[              2*k] =  IM(Z1[N8 +     k]);
        X_out[          2 + 2*k] =  IM(Z1[N8 + 1 + k]);

        X_out[          1 + 2*k] = -RE(Z1[N8 - 1 - k]);
        X_out[          3 + 2*k] = -RE(Z1[N8 - 2 - k]);

        X_out[N4 +          2*k] =  RE(Z1[         k]);
        X_out[N4 +    + 2 + 2*k] =  RE(Z1[     1 + k]);

        X_out[N4 +      1 + 2*k] = -IM(Z1[N4 - 1 - k]);
        X_out[N4 +      3 + 2*k] = -IM(Z1[N4 - 2 - k]);

        X_out[N2 +          2*k] =  RE(Z1[N8 +     k]);
        X_out[N2 +    + 2 + 2*k] =  RE(Z1[N8 + 1 + k]);

        X_out[N2 +      1 + 2*k] = -IM(Z1[N8 - 1 - k]);
        X_out[N2 +      3 + 2*k] = -IM(Z1[N8 - 2 - k]);

        X_out[N2 + N4 +     2*k] = -IM(Z1[         k]);
        X_out[N2 + N4 + 2 + 2*k] = -IM(Z1[     1 + k]);

        X_out[N2 + N4 + 1 + 2*k] =  RE(Z1[N4 - 1 - k]);
        X_out[N2 + N4 + 3 + 2*k] =  RE(Z1[N4 - 2 - k]);
    }

#ifdef PROFILE
    count2 = faad_get_ts() - count2;
    mdct->fft_cycles += count1;
    mdct->cycles += (count2 - count1);
#endif
    free(Z1);
    return 0;
}

#ifdef LTP_DEC
unsigned char faad_mdct(mdct_info *mdct, real_t *X_in, real_t *X_out)
{
    unsigned short k;

    complex_t x;
    complex_t *Z1 = (complex_t *)malloc(512 * sizeof(complex_t));
    if(!Z1)
    {
        return 40;
    }
    complex_t *sincos = mdct->sincos;

    unsigned short N  = mdct->N;
    unsigned short N2 = N >> 1;
    unsigned short N4 = N >> 2;
    unsigned short N8 = N >> 3;

    real_t scale = REAL_CONST(N);

    /* pre-FFT complex multiplication */
    for (k = 0; k < N8; k++)
    {
        unsigned short n = k << 1;
        RE(x) = X_in[N - N4 - 1 - n] + X_in[N - N4 +     n];
        IM(x) = X_in[    N4 +     n] - X_in[    N4 - 1 - n];

        ComplexMult(&RE(Z1[k]), &IM(Z1[k]),
            RE(x), IM(x), RE(sincos[k]), IM(sincos[k]));

        RE(Z1[k]) = MUL_R(RE(Z1[k]), scale);
        IM(Z1[k]) = MUL_R(IM(Z1[k]), scale);

        RE(x) =  X_in[N2 - 1 - n] - X_in[        n];
        IM(x) =  X_in[N2 +     n] + X_in[N - 1 - n];

        ComplexMult(&RE(Z1[k + N8]), &IM(Z1[k + N8]),
            RE(x), IM(x), RE(sincos[k + N8]), IM(sincos[k + N8]));

        RE(Z1[k + N8]) = MUL_R(RE(Z1[k + N8]), scale);
        IM(Z1[k + N8]) = MUL_R(IM(Z1[k + N8]), scale);
    }

    /* complex FFT, any non-scaling FFT can be used here  */
    cfftf(mdct->cfft, Z1);

    /* post-FFT complex multiplication */
    for (k = 0; k < N4; k++)
    {
        unsigned short n = k << 1;
        ComplexMult(&RE(x), &IM(x),
            RE(Z1[k]), IM(Z1[k]), RE(sincos[k]), IM(sincos[k]));

        X_out[         n] = -RE(x);
        X_out[N2 - 1 - n] =  IM(x);
        X_out[N2 +     n] = -IM(x);
        X_out[N  - 1 - n] =  RE(x);
    }
    free(Z1);
    return 0;
}
#endif
