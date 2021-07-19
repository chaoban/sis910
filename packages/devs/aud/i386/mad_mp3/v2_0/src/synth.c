/*
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: synth.c,v 1.25 2004/01/23 09:41:33 rob Exp $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "global.h"

#include "fixed.h"
#include "frame.h"
#include "synth.h"

/*
 * NAME:	synth->init()
 * DESCRIPTION:	initialize synth struct
 */
void mad_synth_init(struct mad_synth *synth)
{
	mad_synth_mute(synth);

	synth->phase = 0;

	synth->pcm.samplerate = 0;
	synth->pcm.channels = 0;
	synth->pcm.length = 0;
}

/*
 * NAME:	synth->mute()
 * DESCRIPTION:	zero all polyphase filterbank values, resetting synthesis
 */
void mad_synth_mute(struct mad_synth *synth)
{
	unsigned int ch, s, v;

	for (ch = 0; ch < 2; ++ch)
	{
		for (s = 0; s < 16; ++s)
		{
			for (v = 0; v < 8; ++v)
			{
				synth->filter[ch][0][0][s][v] = synth->filter[ch][0][1][s][v] = synth->filter[ch][1][0][s][v] = synth->filter[ch][1][1][s][v] = 0;
			}
		}
	}
}

/*
 * An optional optimization called here the Subband Synthesis Optimization
 * (SSO) improves the performance of subband synthesis at the expense of
 * accuracy.
 *
 * The idea is to simplify 32x32->64-bit multiplication to 32x32->32 such
 * that extra scaling and rounding are not necessary. This often allows the
 * compiler to use faster 32-bit multiply-accumulate instructions instead of
 * explicit 64-bit multiply, shift, and add instructions.
 *
 * SSO works like this: a full 32x32->64-bit multiply of two mad_fixed_t
 * values requires the result to be right-shifted 28 bits to be properly
 * scaled to the same fixed-point format. Right shifts can be applied at any
 * time to either operand or to the result, so the optimization involves
 * careful placement of these shifts to minimize the loss of accuracy.
 *
 * First, a 14-bit shift is applied with rounding at compile-time to the D[]
 * table of coefficients for the subband synthesis window. This only loses 2
 * bits of accuracy because the lower 12 bits are always zero. A second
 * 12-bit shift occurs after the DCT calculation. This loses 12 bits of
 * accuracy. Finally, a third 2-bit shift occurs just before the sample is
 * saved in the PCM buffer. 14 + 12 + 2 == 28 bits.
 */

/* FPM_DEFAULT without OPT_SSO will actually lose accuracy and performance */

#if defined(FPM_DEFAULT) && !defined(OPT_SSO)
#define OPT_SSO
#endif

/* second SSO shift, with rounding */

#if defined(OPT_SSO)
#define SHIFT(x)  (((x) + (1L << 11)) >> 12)
#else
#define SHIFT(x)  (x)
#endif

/* possible DCT speed optimization */

#if defined(OPT_SPEED) && defined(MAD_F_MLX)
#define OPT_DCTO
#define MUL(x, y)  \
	({ mad_fixed64hi_t hi;  \
	   mad_fixed64lo_t lo;  \
	   MAD_F_MLX(hi, lo, (x), (y));  \
	   hi << (32 - MAD_F_SCALEBITS - 3);  \
	})
#else
#undef OPT_DCTO
#define MUL(x, y)  mad_f_mul((x), (y))
#endif

/*
 * NAME:	dct32()
 * DESCRIPTION:	perform fast in[32]->out[32] DCT
 */
static
void dct32(mad_fixed_t const in[32], unsigned int slot, mad_fixed_t lo[16][8], mad_fixed_t hi[16][8])
{
	mad_fixed_t a0, a1, a2, a3, a4, a5, a6;
	mad_fixed_t a10, a11, a12, a13, a14, a15, a16, a17, a18, a19;
	mad_fixed_t a20, a21, a22, a23, a24, a25, a26, a27, a28, a29;
	mad_fixed_t a30, a31, a32, a33, a34, a35, a36, a37, a38, a39;
	mad_fixed_t a40, a41, a42, a43, a44, a45, a46, a47, a48, a49;
	mad_fixed_t a50, a51, a52, a53, a54, a55, a56, a57, a58, a59;
	mad_fixed_t a60, a61, a62, a63, a64, a65, a66, a67, a68, a69;
	mad_fixed_t a70, a71, a72, a73, a74, a75, a76, a77, a78, a79;
	mad_fixed_t a80, a81, a82, a83, a84, a85;
	/* costab[i] = cos(PI / (2 * 32) * i) */

#if defined(OPT_DCTO)
#define costab1	MAD_F(0x7fd8878e)
#define costab2	MAD_F(0x7f62368f)
#define costab3	MAD_F(0x7e9d55fc)
#define costab4	MAD_F(0x7d8a5f40)
#define costab5	MAD_F(0x7c29fbee)
#define costab6	MAD_F(0x7a7d055b)
#define costab7	MAD_F(0x78848414)
#define costab8	MAD_F(0x7641af3d)
#define costab9	MAD_F(0x73b5ebd1)
#define costab10	MAD_F(0x70e2cbc6)
#define costab11	MAD_F(0x6dca0d14)
#define costab12	MAD_F(0x6a6d98a4)
#define costab13	MAD_F(0x66cf8120)
#define costab14	MAD_F(0x62f201ac)
#define costab15	MAD_F(0x5ed77c8a)
#define costab16	MAD_F(0x5a82799a)
#define costab17	MAD_F(0x55f5a4d2)
#define costab18	MAD_F(0x5133cc94)
#define costab19	MAD_F(0x4c3fdff4)
#define costab20	MAD_F(0x471cece7)
#define costab21	MAD_F(0x41ce1e65)
#define costab22	MAD_F(0x3c56ba70)
#define costab23	MAD_F(0x36ba2014)
#define costab24	MAD_F(0x30fbc54d)
#define costab25	MAD_F(0x2b1f34eb)
#define costab26	MAD_F(0x25280c5e)
#define costab27	MAD_F(0x1f19f97b)
#define costab28	MAD_F(0x18f8b83c)
#define costab29	MAD_F(0x12c8106f)
#define costab30	MAD_F(0x0c8bd35e)
#define costab31	MAD_F(0x0647d97c)
#else
#define costab1	MAD_F(0x0ffb10f2)  /* 0.998795456 */
#define costab2	MAD_F(0x0fec46d2)  /* 0.995184727 */
#define costab3	MAD_F(0x0fd3aac0)  /* 0.989176510 */
#define costab4	MAD_F(0x0fb14be8)  /* 0.980785280 */
#define costab5	MAD_F(0x0f853f7e)  /* 0.970031253 */
#define costab6	MAD_F(0x0f4fa0ab)  /* 0.956940336 */
#define costab7	MAD_F(0x0f109082)  /* 0.941544065 */
#define costab8	MAD_F(0x0ec835e8)  /* 0.923879533 */
#define costab9	MAD_F(0x0e76bd7a)  /* 0.903989293 */
#define costab10	MAD_F(0x0e1c5979)  /* 0.881921264 */
#define costab11	MAD_F(0x0db941a3)  /* 0.857728610 */
#define costab12	MAD_F(0x0d4db315)  /* 0.831469612 */
#define costab13	MAD_F(0x0cd9f024)  /* 0.803207531 */
#define costab14	MAD_F(0x0c5e4036)  /* 0.773010453 */
#define costab15	MAD_F(0x0bdaef91)  /* 0.740951125 */
#define costab16	MAD_F(0x0b504f33)  /* 0.707106781 */
#define costab17	MAD_F(0x0abeb49a)  /* 0.671558955 */
#define costab18	MAD_F(0x0a267993)  /* 0.634393284 */
#define costab19	MAD_F(0x0987fbfe)  /* 0.595699304 */
#define costab20	MAD_F(0x08e39d9d)  /* 0.555570233 */
#define costab21	MAD_F(0x0839c3cd)  /* 0.514102744 */
#define costab22	MAD_F(0x078ad74e)  /* 0.471396737 */
#define costab23	MAD_F(0x06d74402)  /* 0.427555093 */
#define costab24	MAD_F(0x061f78aa)  /* 0.382683432 */
#define costab25	MAD_F(0x0563e69d)  /* 0.336889853 */
#define costab26	MAD_F(0x04a5018c)  /* 0.290284677 */
#define costab27	MAD_F(0x03e33f2f)  /* 0.242980180 */
#define costab28	MAD_F(0x031f1708)  /* 0.195090322 */
#define costab29	MAD_F(0x0259020e)  /* 0.146730474 */
#define costab30	MAD_F(0x01917a6c)  /* 0.098017140 */
#define costab31	MAD_F(0x00c8fb30)  /* 0.049067674 */
#endif

	a0 = in[0] + in[31];   
	a1 = in[15] + in[16];  
	a5 = MUL(in[0] - in[31], costab1);
	a6 = MUL(in[15] - in[16], costab31);

	a19 = a5 + a6;
	a35 = MUL(a5 - a6, costab2);
	a11 = a0 + a1;
	a27 = MUL(a0 - a1, costab2);

	a0 = in[7] + in[24];  
	a1 = in[8] + in[23];  
	a5 = MUL(in[7] - in[24], costab15);
	a6 = MUL(in[8] - in[23], costab17);

	a20 = a5 + a6;
	a36 = MUL(a5 - a6, costab30);
	a12 = a0 + a1;
	a28 = MUL(a0 - a1, costab30);

	a0 = in[3] + in[28];  
	a1 = in[12] + in[19];  
	a5 = MUL(in[3] - in[28], costab7);
	a6 = MUL(in[12] - in[19], costab25);

	a21 = a5 + a6;
	a37 = MUL(a5 - a6, costab14);
	a13 = a0 + a1;
	a29 = MUL(a0 - a1, costab14);

	a0 = in[4] + in[27];  
	a1 = in[11] + in[20];  
	a5 = MUL(in[4] - in[27], costab9);
	a6 = MUL(in[11] - in[20], costab23);

	a22 = a5 + a6;
	a38 = MUL(a5 - a6, costab18);
	a14 = a0 + a1;
	a30 = MUL(a0 - a1, costab18);

	a0 = in[1] + in[30];  
	a1 = in[14] + in[17];  
	a5 = MUL(in[1] - in[30], costab3);
	a6 = MUL(in[14] - in[17], costab29);

	a23 = a5 + a6;
	a39 = MUL(a5 - a6, costab6);
	a15 = a0 + a1;
	a31 = MUL(a0 - a1, costab6);

	a0 = in[6] + in[25];  
	a1 = in[9] + in[22];  
	a5 = MUL(in[6] - in[25], costab13);
	a6 = MUL(in[9] - in[22], costab19);

	a24 = a5 + a6;
	a40 = MUL(a5 - a6, costab26);
	a16 = a0 + a1;
	a32 = MUL(a0 - a1, costab26);

	a0 = in[2] + in[29];  
	a1 = in[13] + in[18];  
	a5 = MUL(in[2] - in[29], costab5);
	a6 = MUL(in[13] - in[18], costab27);

	a25 = a5 + a6;
	a41 = MUL(a5 - a6, costab10);
	a17 = a0 + a1;
	a33 = MUL(a0 - a1, costab10);

	a0 = in[5] + in[26];  
	a1 = in[10] + in[21];  
	a5 = MUL(in[5] - in[26], costab11);
	a6 = MUL(in[10] - in[21], costab21);

	a26 = a5 + a6;
	a42 = MUL(a5 - a6, costab22);
	a18 = a0 + a1;
	a34 = MUL(a0 - a1, costab22);

	a51 = a11 + a12;  
	a52 = a13 + a14;  
	a53 = a15 + a16;  
	a54 = a17 + a18;  
	a55 = a19 + a20;  
	a56 = a21 + a22;  
	a57 = a23 + a24;  
	a58 = a25 + a26;
	
	a67 = MUL(a11 - a12, costab4);  
	a68 = MUL(a13 - a14, costab28); 
	a69 = MUL(a15 - a16, costab12); 
	a70 = MUL(a17 - a18, costab20); 
	a71 = MUL(a19 - a20, costab4);  
	a72 = MUL(a21 - a22, costab28); 
	a73 = MUL(a23 - a24, costab12); 
	a74 = MUL(a25 - a26, costab20); 	  

	a59 = a27 + a28;  
	a60 = a29 + a30;  
	a61 = a31 + a32;  
	a62 = a33 + a34; 	
	a63 = a35 + a36;  
	a64 = a37 + a38;  
	a65 = a39 + a40;  
	a66 = a41 + a42; 

	a75 = MUL(a27 - a28, costab4);  
	a76 = MUL(a29 - a30, costab28); 
	a77 = MUL(a31 - a32, costab12); 
	a78 = MUL(a33 - a34, costab20); 
	a79 = MUL(a35 - a36, costab4); 
	a80 = MUL(a37 - a38, costab28);
	a81 = MUL(a39 - a40, costab12);
	a82 = MUL(a41 - a42, costab20); 

	a3 = a51 + a52;
	a4 = a53 + a54;

	/*  0 */ hi[15][slot] = SHIFT(a3 + a4);
	/* 16 */ lo[0][slot] = SHIFT(MUL(a3 - a4, costab16));

	a0 = a55 + a56;
	a1 = a57 + a58;
	
	a2 = a0 + a1;

	/*  1 */ hi[14][slot] = SHIFT(a2);

	a10 = a59 + a60;
	a11 = a61 + a62;

	a12 = a10 + a11;

	/*  2 */ hi[13][slot] = SHIFT(a12);

	a13 = a63 + a64;
	a14 = a65 + a66;

	a15 = a13 + a14;

	a16 = (a15 * 2) - a2;

	/*  3 */ hi[12][slot] = SHIFT(a16);

	a17 = a67 + a68;
	a18 = a69 + a70;

	a19 = a17 + a18;

	/*  4 */ hi[11][slot] = SHIFT(a19);

	a20 = a71 + a72;
	a21 = a73 + a74;

	a22 = a20 + a21;

	a23 = (a22 * 2) - a16;

	/*  5 */ hi[10][slot] = SHIFT(a23);

	a24 = a75 + a76;
	a25 = a77 + a78;

	a26 = a24 + a25;

	a27 = (a26 * 2) - a12;

	/*  6 */ hi[9][slot] = SHIFT(a27);

	a28 = a79 + a80;
	a29 = a81 + a82;

	a30 = a28 + a29;

	a31 = (a30 * 2) - a15;

	a32 = (a31 * 2) - a23;

	/*  7 */ hi[8][slot] = SHIFT(a32);

	a33 = MUL(a51 - a52, costab8);
	a34 = MUL(a53 - a54, costab24);
	a35 = a33 + a34;

	/*  8 */ hi[7][slot] = SHIFT(a35);
	/* 24 */ lo[8][slot] = SHIFT((MUL(a33 - a34, costab16) * 2) - a35);

	a36 = MUL(a55 - a56, costab8);
	a37 = MUL(a57 - a58, costab24);
	a38 = a36 + a37;

	a39 = (a38 * 2) - a32;

	/*  9 */ hi[6][slot] = SHIFT(a39);

	a40 = MUL(a59 - a60, costab8);
	a41 = MUL(a61 - a62, costab24);
	a42 = a40 + a41;

	a43 = (a42 * 2) - a27;

	/* 10 */ hi[5][slot] = SHIFT(a43);

	a44 = MUL(a63 - a64, costab8);
	a45 = MUL(a65 - a66, costab24);
	a46 = a44 + a45;

	a47 = (a46 * 2) - a31;

	a48 = (a47 * 2) - a39;

	/* 11 */ hi[4][slot] = SHIFT(a48);

	a49 = MUL(a67 - a68, costab8);
	a50 = MUL(a69 - a70, costab24);
	a51 = a49 + a50;

	a52 = (a51 * 2) - a19;

	/* 12 */ hi[3][slot] = SHIFT(a52);

	a53 = (MUL(a17 - a18, costab16) * 2) - a52;

	/* 20 */ lo[4][slot] = SHIFT(a53);
	/* 28 */ lo[12][slot] = SHIFT((((MUL(a49 - a50, costab16) * 2) - a51) * 2) - a53);

	a54 = MUL(a71 - a72, costab8);
	a55 = MUL(a73 - a74, costab24);
	a56 = a54 + a55;

	a57 = (a56 * 2) - a22;

	a58 = (a57 * 2) - a48;

	/* 13 */ hi[2][slot] = SHIFT(a58);

	a59 = (MUL(a20 - a21, costab16) * 2) - a57;

	a60 = MUL(a75 - a76, costab8);
	a61 = MUL(a77 - a78, costab24);
	a62 = a60 + a61;

	a63 = (a62 * 2) - a26;

	a64 = (a63 * 2) - a43;

	/* 14 */ hi[1][slot] = SHIFT(a64);

	a65 = (MUL(a10 - a11, costab16) * 2) - a64;

	/* 18 */ lo[2][slot] = SHIFT(a65);

	a66 = (MUL(a24 - a25, costab16) * 2) - a63;

	a67 = (a66 * 2) - a65;

	/* 22 */ lo[6][slot] = SHIFT(a67);

	a68 = (((MUL(a40 - a41, costab16) * 2) - a42) * 2) - a67;

	/* 26 */ lo[10][slot] = SHIFT(a68);
	/* 30 */ lo[14][slot] = SHIFT((((((MUL(a60 - a61, costab16) * 2) - a62) * 2) - a66) * 2) - a68);

	a69 = MUL(a79 - a80, costab8);
	a70 = MUL(a81 - a82, costab24);
	a71 = a69 + a70;

	a72 = (a71 * 2) - a30;

	a73 = (a72 * 2) - a47;

	a74 = (MUL(a13 - a14, costab16) * 2) - a73;

	a75 = (a73 * 2) - a58;

	/* 15 */ hi[0][slot] = SHIFT(a75);

	a76 = (MUL(a0 - a1, costab16) * 2) - a75;

	/* 17 */ lo[1][slot] = SHIFT(a76);

	a77 = (a74 * 2) - a76;

	/* 19 */ lo[3][slot] = SHIFT(a77);

	a78 = (a59 * 2) - a77;

	/* 21 */ lo[5][slot] = SHIFT(a78);

	a79 = (MUL(a28 - a29, costab16) * 2) - a72;

	a80 = (a79 * 2) - a74;

	a81 = (a80 * 2) - a78;

	/* 23 */ lo[7][slot] = SHIFT(a81);

	a82 = (((MUL(a36 - a37, costab16) * 2) - a38) * 2) - a81;

	/* 25 */ lo[9][slot] = SHIFT(a82);

	a83 = (((MUL(a44 - a45, costab16) * 2) - a46) * 2) - a80;

	a84 = (a83 * 2) - a82;

	/* 27 */ lo[11][slot] = SHIFT(a84);

	a85 = (((((MUL(a54 - a55, costab16) * 2) - a56) * 2) - a59) * 2) - a84;

	/* 29 */ lo[13][slot] = SHIFT(a85);
	/* 31 */ lo[15][slot] = SHIFT((((((((MUL(a69 - a70, costab16) * 2) - a71) * 2) - a79) * 2) - a83) * 2) - a85);
	/*
	 * Totals:
	 *  80 multiplies
	 *  80 additions
	 * 119 subtractions
	 *  49 shifts (not counting SSO)
	 */
}

#undef MUL
#undef SHIFT

/* third SSO shift and/or D[] optimization preshift */

#if defined(OPT_SSO)
#if MAD_F_FRACBITS != 28
#error "MAD_F_FRACBITS must be 28 to use OPT_SSO"
#endif
#ifdef SH3_OPTIMAL
#define ML0(hi, lo, x, y)												\
	asm volatile ("mul.l	%3,	%2\n\t"										\
		 		  "sts	  macl,	%1"											\
		 		  : "=&r" (hi), "=&r" (lo)									\
		 		  : "%r" (x), "r" (y))
#define MLA(hi, lo, x, y)   												\
		({mad_fixed_t __result; 											 \
		   ML0(hi, (__result), (x),(y));									  \
		   lo += (__result);												   \
		 			})
#define MLN(hi, lo)		((lo)  = -(lo))
#define MLZ(hi, lo)		((void) (hi), (mad_fixed_t) (lo))
#define SHIFT(x)		((x) >> 2)
#define PRESHIFT(x)		((MAD_F(x) + (1L << 13)) >> 14)
#else//SH3_OPTIMAL
#define ML0(hi, lo, x, y)	((lo)  = (x) * (y))
#define MLA(hi, lo, x, y)	((lo) += (x) * (y))
#define MLN(hi, lo)		((lo)  = -(lo))
#define MLZ(hi, lo)		((void) (hi), (mad_fixed_t) (lo))
#define SHIFT(x)		((x) >> 2)
#define PRESHIFT(x)		((MAD_F(x) + (1L << 13)) >> 14)
#endif //SH3_OPTIMAL
#else //OPT_SSO
#define ML0(hi, lo, x, y)	MAD_F_ML0((hi), (lo), (x), (y))
#define MLA(hi, lo, x, y)	MAD_F_MLA((hi), (lo), (x), (y))
#define MLN(hi, lo)		MAD_F_MLN((hi), (lo))
#define MLZ(hi, lo)		MAD_F_MLZ((hi), (lo))
#define SHIFT(x)		(x)
#if defined(MAD_F_SCALEBITS)
#undef  MAD_F_SCALEBITS
#define MAD_F_SCALEBITS	(MAD_F_FRACBITS - 12)
#define PRESHIFT(x)		(MAD_F(x) >> 12)
#else
#define PRESHIFT(x)		MAD_F(x)
#endif
#endif

static
mad_fixed_t const D[17][32] =
{
	#include "D.dat"

};

#if defined(ASO_SYNTH)
void synth_full(struct mad_synth *, struct mad_frame const *, unsigned int, unsigned int);
#else
/*
 * NAME:	synth->full()
 * DESCRIPTION:	perform full frequency PCM synthesis
 */
#ifdef SH3_SYN_HWMAC
#include "sismac.h"
static
void synth_full(struct mad_synth *synth, struct mad_frame const * frame, unsigned int nch, unsigned int ns)
{
	unsigned int phase, ch, s, sb, pe, po;
	mad_fixed_t *pcm1, *pcm2, (*filter)[2][2][16][8];
	mad_fixed_t const (*sbsample)[36][32];
	register mad_fixed_t(*fe)[8], (*fx)[8], (*fo)[8];
	register mad_fixed_t const (*Dptr)[32], * ptr;
	register mad_fixed64hi_t hi;
	register mad_fixed64lo_t lo;

	for (ch = 0; ch < nch; ++ch)
	{
		sbsample = &frame->sbsample[ch];
		filter = &synth->filter[ch];
		phase = synth->phase;
		pcm1 = synth->pcm.samples[ch];

		for (s = 0; s < ns; ++s)
		{
			dct32((*sbsample)[s], phase >> 1, (*filter)[0][phase & 1], (*filter)[1][phase & 1]);

			pe = phase & ~1;
			po = ((phase - 1) & 0xf) | 1;

			// calculate 32 samples 

			fe = &(*filter)[0][phase & 1][0];
			fx = &(*filter)[0][~phase & 1][0];
			fo = &(*filter)[1][~phase & 1][0];

			Dptr = &D[0];

			ptr = *Dptr + po;
	  HW_MAC_CLEAR;
			//for performace, not use loop

			//A array data
	  HW_MAC_FX_ARRAY_WRITE;

			//B array data
	  HW_MAC_PTR_BRRAY_WRITE;  

			//Control
	  HW_MAC_SET;  
			//  	ML0(hi, lo, (*fx)[0], ptr[ 0]);
			//  	MLA(hi, lo, (*fx)[1], ptr[14]);
			//  	MLA(hi, lo, (*fx)[2], ptr[12]);
			//  	MLA(hi, lo, (*fx)[3], ptr[10]);
			//  	MLA(hi, lo, (*fx)[4], ptr[ 8]);
			//  	MLA(hi, lo, (*fx)[5], ptr[ 6]);
			//  	MLA(hi, lo, (*fx)[6], ptr[ 4]);
			//  	MLA(hi, lo, (*fx)[7], ptr[ 2]);
			//24 bit may be over flow
			lo = iior(MACL);
			MLN(hi, lo);

			ptr = *Dptr + pe;
	  HW_MAC_CLEAR;
			//need to load lo to mac first
			iiow(0x9004000c, 0x1);
			iiow(0x9004040c, lo);
	  
	  HW_MAC_FE_ARRAY_WRITE;
	  HW_MAC_PTR_BRRAY_WRITE;
	  HW_MAC_SET_II;
			lo = iior(MACL);
			//hi =iior(MACH);
			//  	MLA(hi, lo, (*fe)[0], ptr[ 0]);
			//  	MLA(hi, lo, (*fe)[1], ptr[14]);
			//  	MLA(hi, lo, (*fe)[2], ptr[12]);
			//  	MLA(hi, lo, (*fe)[3], ptr[10]);
			//  	MLA(hi, lo, (*fe)[4], ptr[ 8]);
			//  	MLA(hi, lo, (*fe)[5], ptr[ 6]);
			//  	MLA(hi, lo, (*fe)[6], ptr[ 4]);
			//  	MLA(hi, lo, (*fe)[7], ptr[ 2]);

			*pcm1++ = SHIFT(MLZ(hi, lo));

			pcm2 = pcm1 + 30;

			for (sb = 1; sb < 16; ++sb)
			{
				++fe;
				++Dptr;

				// D[32 - sb][i] == -D[sb][31 - i]

				ptr = *Dptr + po;
		HW_MAC_CLEAR;
				//A array data
		HW_MAC_FO_ARRAY_WRITE;

				//B array data
		HW_MAC_PTR_BRRAY_WRITE;  

				//Control
		HW_MAC_SET;  
				//    	ML0(hi, lo, (*fo)[0], ptr[ 0]);
				//    	MLA(hi, lo, (*fo)[1], ptr[14]);
				//    	MLA(hi, lo, (*fo)[2], ptr[12]);
				//    	MLA(hi, lo, (*fo)[3], ptr[10]);
				//    	MLA(hi, lo, (*fo)[4], ptr[ 8]);
				//    	MLA(hi, lo, (*fo)[5], ptr[ 6]);
				//    	MLA(hi, lo, (*fo)[6], ptr[ 4]);
				//    	MLA(hi, lo, (*fo)[7], ptr[ 2]);
				lo = iior(MACL);
				MLN(hi, lo);



				ptr = *Dptr + pe;
		HW_MAC_CLEAR;
				iiow(0x9004000c, 0x1);
				iiow(0x9004040c, lo);
	  
		HW_MAC_FE_R_ARRAY_WRITE;
		HW_MAC_PTR_R_BRRAY_WRITE;
		HW_MAC_SET_II;
				lo = iior(MACL);
				//hi =iior(MACH);
				//    	MLA(hi, lo, (*fe)[7], ptr[ 2]);
				//    	MLA(hi, lo, (*fe)[6], ptr[ 4]);
				//    	MLA(hi, lo, (*fe)[5], ptr[ 6]);
				//    	MLA(hi, lo, (*fe)[4], ptr[ 8]);
				//    	MLA(hi, lo, (*fe)[3], ptr[10]);
				//    	MLA(hi, lo, (*fe)[2], ptr[12]);
				//    	MLA(hi, lo, (*fe)[1], ptr[14]);
				//    	MLA(hi, lo, (*fe)[0], ptr[ 0]);

				*pcm1++ = SHIFT(MLZ(hi, lo));

				ptr = *Dptr - pe;

		HW_MAC_CLEAR;
				//A array data
		HW_MAC_FE_ARRAY_WRITE;
		HW_MAC_FO_R_ARRAY_WRITE;

				//B array data
		  
		HW_MAC_PTR_II_BRRAY_WRITE; 

				//    	ML0(hi, lo, (*fe)[0], ptr[31 - 16]);
				//    	MLA(hi, lo, (*fe)[1], ptr[31 - 14]);
				//    	MLA(hi, lo, (*fe)[2], ptr[31 - 12]);
				//    	MLA(hi, lo, (*fe)[3], ptr[31 - 10]);
				//    	MLA(hi, lo, (*fe)[4], ptr[31 -  8]);
				//    	MLA(hi, lo, (*fe)[5], ptr[31 -  6]);
				//    	MLA(hi, lo, (*fe)[6], ptr[31 -  4]);
				//    	MLA(hi, lo, (*fe)[7], ptr[31 -  2]);

				ptr = *Dptr - po;
		HW_MAC_PTR_III_BRRAY_WRITE;

				//    	MLA(hi, lo, (*fo)[7], ptr[31 -  2]);
				//    	MLA(hi, lo, (*fo)[6], ptr[31 -  4]);
				//    	MLA(hi, lo, (*fo)[5], ptr[31 -  6]);
				//    	MLA(hi, lo, (*fo)[4], ptr[31 -  8]);
				//    	MLA(hi, lo, (*fo)[3], ptr[31 - 10]);
				//    	MLA(hi, lo, (*fo)[2], ptr[31 - 12]);
				//    	MLA(hi, lo, (*fo)[1], ptr[31 - 14]);
				//    	MLA(hi, lo, (*fo)[0], ptr[31 - 16]);
		HW_MAC_SET_III;
				lo = iior(MACL);
				*pcm2-- = SHIFT(MLZ(hi, lo));

				++fo;
			}

			++Dptr;

			ptr = *Dptr + po;
	  HW_MAC_CLEAR;
			//A array data
	  HW_MAC_FO_ARRAY_WRITE;

			//B array data
	  HW_MAC_PTR_BRRAY_WRITE;  

			//Control
	  HW_MAC_SET;  
			lo = iior(MACL);

			//  	ML0(hi, lo, (*fo)[0], ptr[ 0]);
			//  	MLA(hi, lo, (*fo)[1], ptr[14]);
			//  	MLA(hi, lo, (*fo)[2], ptr[12]);
			//  	MLA(hi, lo, (*fo)[3], ptr[10]);
			//  	MLA(hi, lo, (*fo)[4], ptr[ 8]);
			//  	MLA(hi, lo, (*fo)[5], ptr[ 6]);
			//  	MLA(hi, lo, (*fo)[6], ptr[ 4]);
			//  	MLA(hi, lo, (*fo)[7], ptr[ 2]);

			*pcm1 = SHIFT(-MLZ(hi, lo));
			pcm1 += 16;

			phase = (phase + 1) % 16;
		}
	}
}
#else //FPM_DEFAULT
static
void synth_full(struct mad_synth *synth, struct mad_frame const * frame, unsigned int nch, unsigned int ns)
{
	unsigned int phase, ch, s, sb, pe, po;
	mad_fixed_t *pcm1, *pcm2, (*filter)[2][2][16][8];
	mad_fixed_t const (*sbsample)[36][32];
	register mad_fixed_t(*fe)[8], (*fx)[8], (*fo)[8];
	register mad_fixed_t const (*Dptr)[32], * ptr;
	register mad_fixed64hi_t hi;
	register mad_fixed64lo_t lo;

	for (ch = 0; ch < nch; ++ch)
	{
		sbsample = &frame->sbsample[ch];
		filter = &synth->filter[ch];
		phase = synth->phase;
		pcm1 = synth->pcm.samples[ch];

		for (s = 0; s < ns; ++s)
		{
			dct32((*sbsample)[s], phase >> 1, (*filter)[0][phase & 1], (*filter)[1][phase & 1]);

			pe = phase & ~1;
			po = ((phase - 1) & 0xf) | 1;

			/* calculate 32 samples */

			fe = &(*filter)[0][phase & 1][0];
			fx = &(*filter)[0][~phase & 1][0];
			fo = &(*filter)[1][~phase & 1][0];

			Dptr = &D[0];

			ptr = *Dptr + po;
			ML0(hi, lo, (*fx)[0], ptr[0]);
			MLA(hi, lo, (*fx)[1], ptr[14]);
			MLA(hi, lo, (*fx)[2], ptr[12]);
			MLA(hi, lo, (*fx)[3], ptr[10]);
			MLA(hi, lo, (*fx)[4], ptr[8]);
			MLA(hi, lo, (*fx)[5], ptr[6]);
			MLA(hi, lo, (*fx)[6], ptr[4]);
			MLA(hi, lo, (*fx)[7], ptr[2]);
			MLN(hi, lo);

			ptr = *Dptr + pe;
			MLA(hi, lo, (*fe)[0], ptr[0]);
			MLA(hi, lo, (*fe)[1], ptr[14]);
			MLA(hi, lo, (*fe)[2], ptr[12]);
			MLA(hi, lo, (*fe)[3], ptr[10]);
			MLA(hi, lo, (*fe)[4], ptr[8]);
			MLA(hi, lo, (*fe)[5], ptr[6]);
			MLA(hi, lo, (*fe)[6], ptr[4]);
			MLA(hi, lo, (*fe)[7], ptr[2]);

			*pcm1++ = SHIFT(MLZ(hi, lo));

			pcm2 = pcm1 + 30;

			for (sb = 1; sb < 16; ++sb)
			{
				++fe;
				++Dptr;

				/* D[32 - sb][i] == -D[sb][31 - i] */

				ptr = *Dptr + po;
				ML0(hi, lo, (*fo)[0], ptr[0]);
				MLA(hi, lo, (*fo)[1], ptr[14]);
				MLA(hi, lo, (*fo)[2], ptr[12]);
				MLA(hi, lo, (*fo)[3], ptr[10]);
				MLA(hi, lo, (*fo)[4], ptr[8]);
				MLA(hi, lo, (*fo)[5], ptr[6]);
				MLA(hi, lo, (*fo)[6], ptr[4]);
				MLA(hi, lo, (*fo)[7], ptr[2]);
				MLN(hi, lo);

				ptr = *Dptr + pe;
				MLA(hi, lo, (*fe)[7], ptr[2]);
				MLA(hi, lo, (*fe)[6], ptr[4]);
				MLA(hi, lo, (*fe)[5], ptr[6]);
				MLA(hi, lo, (*fe)[4], ptr[8]);
				MLA(hi, lo, (*fe)[3], ptr[10]);
				MLA(hi, lo, (*fe)[2], ptr[12]);
				MLA(hi, lo, (*fe)[1], ptr[14]);
				MLA(hi, lo, (*fe)[0], ptr[0]);

				*pcm1++ = SHIFT(MLZ(hi, lo));

				ptr = *Dptr - pe;
				ML0(hi, lo, (*fe)[0], ptr[31 - 16]);
				MLA(hi, lo, (*fe)[1], ptr[31 - 14]);
				MLA(hi, lo, (*fe)[2], ptr[31 - 12]);
				MLA(hi, lo, (*fe)[3], ptr[31 - 10]);
				MLA(hi, lo, (*fe)[4], ptr[31 - 8]);
				MLA(hi, lo, (*fe)[5], ptr[31 - 6]);
				MLA(hi, lo, (*fe)[6], ptr[31 - 4]);
				MLA(hi, lo, (*fe)[7], ptr[31 - 2]);

				ptr = *Dptr - po;
				MLA(hi, lo, (*fo)[7], ptr[31 - 2]);
				MLA(hi, lo, (*fo)[6], ptr[31 - 4]);
				MLA(hi, lo, (*fo)[5], ptr[31 - 6]);
				MLA(hi, lo, (*fo)[4], ptr[31 - 8]);
				MLA(hi, lo, (*fo)[3], ptr[31 - 10]);
				MLA(hi, lo, (*fo)[2], ptr[31 - 12]);
				MLA(hi, lo, (*fo)[1], ptr[31 - 14]);
				MLA(hi, lo, (*fo)[0], ptr[31 - 16]);

				*pcm2-- = SHIFT(MLZ(hi, lo));

				++fo;
			}

			++Dptr;

			ptr = *Dptr + po;
			ML0(hi, lo, (*fo)[0], ptr[0]);
			MLA(hi, lo, (*fo)[1], ptr[14]);
			MLA(hi, lo, (*fo)[2], ptr[12]);
			MLA(hi, lo, (*fo)[3], ptr[10]);
			MLA(hi, lo, (*fo)[4], ptr[8]);
			MLA(hi, lo, (*fo)[5], ptr[6]);
			MLA(hi, lo, (*fo)[6], ptr[4]);
			MLA(hi, lo, (*fo)[7], ptr[2]);

			*pcm1 = SHIFT(-MLZ(hi, lo));
			pcm1 += 16;

			phase = (phase + 1) % 16;
		}
	}
}
#endif //SH3_SYN_HWMAC
#endif

/*
 * NAME:	synth->half()
 * DESCRIPTION:	perform half frequency PCM synthesis
 */
static
void synth_half(struct mad_synth *synth, struct mad_frame const * frame, unsigned int nch, unsigned int ns)
{
	unsigned int phase, ch, s, sb, pe, po;
	mad_fixed_t *pcm1, *pcm2, (*filter)[2][2][16][8];
	mad_fixed_t const (*sbsample)[36][32];
	register mad_fixed_t(*fe)[8], (*fx)[8], (*fo)[8];
	register mad_fixed_t const (*Dptr)[32], * ptr;
	register mad_fixed64hi_t hi;
	register mad_fixed64lo_t lo;

	for (ch = 0; ch < nch; ++ch)
	{
		sbsample = &frame->sbsample[ch];
		filter = &synth->filter[ch];
		phase = synth->phase;
		pcm1 = synth->pcm.samples[ch];

		for (s = 0; s < ns; ++s)
		{
			dct32((*sbsample)[s], phase >> 1, (*filter)[0][phase & 1], (*filter)[1][phase & 1]);

			pe = phase & ~1;
			po = ((phase - 1) & 0xf) | 1;

			/* calculate 16 samples */

			fe = &(*filter)[0][phase & 1][0];
			fx = &(*filter)[0][~phase & 1][0];
			fo = &(*filter)[1][~phase & 1][0];

			Dptr = &D[0];

			ptr = *Dptr + po;
			ML0(hi, lo, (*fx)[0], ptr[0]);
			MLA(hi, lo, (*fx)[1], ptr[14]);
			MLA(hi, lo, (*fx)[2], ptr[12]);
			MLA(hi, lo, (*fx)[3], ptr[10]);
			MLA(hi, lo, (*fx)[4], ptr[8]);
			MLA(hi, lo, (*fx)[5], ptr[6]);
			MLA(hi, lo, (*fx)[6], ptr[4]);
			MLA(hi, lo, (*fx)[7], ptr[2]);
			MLN(hi, lo);

			ptr = *Dptr + pe;
			MLA(hi, lo, (*fe)[0], ptr[0]);
			MLA(hi, lo, (*fe)[1], ptr[14]);
			MLA(hi, lo, (*fe)[2], ptr[12]);
			MLA(hi, lo, (*fe)[3], ptr[10]);
			MLA(hi, lo, (*fe)[4], ptr[8]);
			MLA(hi, lo, (*fe)[5], ptr[6]);
			MLA(hi, lo, (*fe)[6], ptr[4]);
			MLA(hi, lo, (*fe)[7], ptr[2]);

			*pcm1++ = SHIFT(MLZ(hi, lo));

			pcm2 = pcm1 + 14;

			for (sb = 1; sb < 16; ++sb)
			{
				++fe;
				++Dptr;

				/* D[32 - sb][i] == -D[sb][31 - i] */

				if (!(sb & 1))
				{
					ptr = *Dptr + po;
					ML0(hi, lo, (*fo)[0], ptr[0]);
					MLA(hi, lo, (*fo)[1], ptr[14]);
					MLA(hi, lo, (*fo)[2], ptr[12]);
					MLA(hi, lo, (*fo)[3], ptr[10]);
					MLA(hi, lo, (*fo)[4], ptr[8]);
					MLA(hi, lo, (*fo)[5], ptr[6]);
					MLA(hi, lo, (*fo)[6], ptr[4]);
					MLA(hi, lo, (*fo)[7], ptr[2]);
					MLN(hi, lo);

					ptr = *Dptr + pe;
					MLA(hi, lo, (*fe)[7], ptr[2]);
					MLA(hi, lo, (*fe)[6], ptr[4]);
					MLA(hi, lo, (*fe)[5], ptr[6]);
					MLA(hi, lo, (*fe)[4], ptr[8]);
					MLA(hi, lo, (*fe)[3], ptr[10]);
					MLA(hi, lo, (*fe)[2], ptr[12]);
					MLA(hi, lo, (*fe)[1], ptr[14]);
					MLA(hi, lo, (*fe)[0], ptr[0]);

					*pcm1++ = SHIFT(MLZ(hi, lo));

					ptr = *Dptr - po;
					ML0(hi, lo, (*fo)[7], ptr[31 - 2]);
					MLA(hi, lo, (*fo)[6], ptr[31 - 4]);
					MLA(hi, lo, (*fo)[5], ptr[31 - 6]);
					MLA(hi, lo, (*fo)[4], ptr[31 - 8]);
					MLA(hi, lo, (*fo)[3], ptr[31 - 10]);
					MLA(hi, lo, (*fo)[2], ptr[31 - 12]);
					MLA(hi, lo, (*fo)[1], ptr[31 - 14]);
					MLA(hi, lo, (*fo)[0], ptr[31 - 16]);

					ptr = *Dptr - pe;
					MLA(hi, lo, (*fe)[0], ptr[31 - 16]);
					MLA(hi, lo, (*fe)[1], ptr[31 - 14]);
					MLA(hi, lo, (*fe)[2], ptr[31 - 12]);
					MLA(hi, lo, (*fe)[3], ptr[31 - 10]);
					MLA(hi, lo, (*fe)[4], ptr[31 - 8]);
					MLA(hi, lo, (*fe)[5], ptr[31 - 6]);
					MLA(hi, lo, (*fe)[6], ptr[31 - 4]);
					MLA(hi, lo, (*fe)[7], ptr[31 - 2]);

					*pcm2-- = SHIFT(MLZ(hi, lo));
				}

				++fo;
			}

			++Dptr;

			ptr = *Dptr + po;
			ML0(hi, lo, (*fo)[0], ptr[0]);
			MLA(hi, lo, (*fo)[1], ptr[14]);
			MLA(hi, lo, (*fo)[2], ptr[12]);
			MLA(hi, lo, (*fo)[3], ptr[10]);
			MLA(hi, lo, (*fo)[4], ptr[8]);
			MLA(hi, lo, (*fo)[5], ptr[6]);
			MLA(hi, lo, (*fo)[6], ptr[4]);
			MLA(hi, lo, (*fo)[7], ptr[2]);

			*pcm1 = SHIFT(-MLZ(hi, lo));
			pcm1 += 8;

			phase = (phase + 1) % 16;
		}
	}
}

/*
 * NAME:	synth->frame()
 * DESCRIPTION:	perform PCM synthesis of frame subband samples
 */
void mad_synth_frame(struct mad_synth *synth, struct mad_frame const * frame)
{
	unsigned int nch, ns;
	void (*synth_frame) (struct mad_synth *, struct mad_frame const *, unsigned int, unsigned int);

	nch = MAD_NCHANNELS(&frame->header);
	ns = MAD_NSBSAMPLES(&frame->header);

	synth->pcm.samplerate = frame->header.samplerate;
	synth->pcm.channels = nch;
	synth->pcm.length = 32 * ns;

	synth_frame = synth_full;

	if (frame->options & MAD_OPTION_HALFSAMPLERATE)
		//if(1)
	{
		synth->pcm.samplerate /= 2;
		synth->pcm.length /= 2;

		synth_frame = synth_half;
	}

	synth_frame(synth, frame, nch, ns);

	synth->phase = (synth->phase + ns) % 16;
}
