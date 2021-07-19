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
** $Id: bits.c,v 1.44 2007/11/01 12:33:29 menno Exp $
**/

#include "common.h"
#include "aacdec.h"

#include <stdlib.h>
#include "bits.h"

static void Flushbits_ex(bitfile *ld, unsigned int bits);

static unsigned int getdword(void *mem)
{
    unsigned int tmp;
#ifndef ARCH_IS_BIG_ENDIAN
    ((unsigned char*)&tmp)[0] = ((unsigned char*)mem)[3];
    ((unsigned char*)&tmp)[1] = ((unsigned char*)mem)[2];
    ((unsigned char*)&tmp)[2] = ((unsigned char*)mem)[1];
    ((unsigned char*)&tmp)[3] = ((unsigned char*)mem)[0];
#else
    ((unsigned char*)&tmp)[0] = ((unsigned char*)mem)[0];
    ((unsigned char*)&tmp)[1] = ((unsigned char*)mem)[1];
    ((unsigned char*)&tmp)[2] = ((unsigned char*)mem)[2];
    ((unsigned char*)&tmp)[3] = ((unsigned char*)mem)[3];
#endif

    return tmp;
}

/* reads only n bytes from the stream instead of the standard 4 */
static unsigned int getdword_n(void *mem, int n)
{
    unsigned int tmp = 0;
#ifndef ARCH_IS_BIG_ENDIAN
    switch (n)
    {
    case 3:
        ((unsigned char*)&tmp)[1] = ((unsigned char*)mem)[2];
    case 2:
        ((unsigned char*)&tmp)[2] = ((unsigned char*)mem)[1];
    case 1:
        ((unsigned char*)&tmp)[3] = ((unsigned char*)mem)[0];
    default:
        break;
    }
#else
    switch (n)
    {
    case 3:
        ((unsigned char*)&tmp)[2] = ((unsigned char*)mem)[2];
    case 2:
        ((unsigned char*)&tmp)[1] = ((unsigned char*)mem)[1];
    case 1:
        ((unsigned char*)&tmp)[0] = ((unsigned char*)mem)[0];
    default:
        break;
    }
#endif

    return tmp;
}


unsigned int Showbits(bitfile *ld, unsigned int bits)
{
    if (bits <= ld->bits_left)
    {
        //return (ld->bufa >> (ld->bits_left - bits)) & bitmask[bits];
        return (ld->bufa << (32 - ld->bits_left)) >> (32 - bits);
    }

    bits -= ld->bits_left;
    //return ((ld->bufa & bitmask[ld->bits_left]) << bits) | (ld->bufb >> (32 - bits));
    return ((ld->bufa & ((1<<ld->bits_left)-1)) << bits) | (ld->bufb >> (32 - bits));
}

void Flushbits(bitfile *ld, unsigned int bits)
{
    /* do nothing if error */
    if (ld->error != 0)
        return;

    if (bits < ld->bits_left)
    {
        ld->bits_left -= bits;
    } else {
        Flushbits_ex(ld, bits);
    }
}

/* return next n bits (right adjusted) */
unsigned int Getbits(bitfile *ld, unsigned int n)
{
    unsigned int ret;

    if (n == 0)
        return 0;

    ret = Showbits(ld, n);
    Flushbits(ld, n);

    return ret;
}

unsigned char Get1bit(bitfile *ld)
{
    unsigned char r;
    
    if (ld->bits_left > 0)
    {
        ld->bits_left--;
        r = (unsigned char)((ld->bufa >> ld->bits_left) & 1);
        return r;
    }
   
    r = (unsigned char)Getbits(ld, 1);

    return r;
}

void Initbits(bitfile *ld, const void *_buffer, const unsigned int buffer_size)
{
    unsigned int tmp;

    if (ld == NULL)
        return;

    // useless
    //memset(ld, 0, sizeof(bitfile));

    if (buffer_size == 0 || _buffer == NULL)
    {
        ld->error = 1;
        return;
    }

    ld->buffer = _buffer;

    ld->buffer_size = buffer_size;
    ld->bytes_left  = buffer_size;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((unsigned int*)ld->buffer);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((unsigned int*)ld->buffer, ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufa = tmp;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((unsigned int*)ld->buffer + 1);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((unsigned int*)ld->buffer + 1, ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;

    ld->start = (unsigned int*)ld->buffer;
    ld->tail = ((unsigned int*)ld->buffer + 2);

    ld->bits_left = 32;

    ld->error = 0;
}

void Byte_align(bitfile *ld)
{
    int remainder;
   
    remainder = (32 - ld->bits_left) & 0x7;
   
    if (remainder)
    {
        Flushbits(ld, 8 - remainder);
    }
}

void Flushbits_ex(bitfile *ld, unsigned int bits)
{
    unsigned int tmp;

    ld->bufa = ld->bufb;
    if (ld->bytes_left >= 4)
    {
        tmp = getdword(ld->tail);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n(ld->tail, ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;
    ld->tail++;
    ld->bits_left += (32 - bits);
    //ld->bytes_left -= 4;
//    if (ld->bytes_left == 0)
//        ld->no_more_reading = 1;
//    if (ld->bytes_left < 0)
//        ld->error = 1;
}

unsigned int Get_processed_bits(bitfile *ld)
{
    return (unsigned int)(8 * (4*(ld->tail - ld->start) - 4) - (ld->bits_left));   
}
#ifdef SBR_DEC

/* reset to a certain point */
void Resetbits(bitfile *ld, int bits)
{
    unsigned int tmp;
    int words = bits >> 5;
    int remainder = bits & 0x1F;

    ld->bytes_left = ld->buffer_size - words*4;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword(&ld->start[words]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n(&ld->start[words], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufa = tmp;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword(&ld->start[words+1]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n(&ld->start[words+1], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;

    ld->bits_left = 32 - remainder;
    ld->tail = &ld->start[words+2];

    /* recheck for reading too many bytes */
    ld->error = 0;
//    if (ld->bytes_left == 0)
//        ld->no_more_reading = 1;
//    if (ld->bytes_left < 0)
//        ld->error = 1;
}
#endif

#ifdef ERROR_RESILIENCE

unsigned int showbits_hcr(bits_t *ld, unsigned char bits)
{
    if (bits == 0) return 0;
    if (ld->len <= 32)
    {
        /* huffman_spectral_data_2 needs to read more than may be available, bits maybe
           > ld->len, deliver 0 than */
        if (ld->len >= bits)
            return ((ld->bufa >> (ld->len - bits)) & (0xFFFFFFFF >> (32 - bits)));
        else
            return ((ld->bufa << (bits - ld->len)) & (0xFFFFFFFF >> (32 - bits)));
    } else {
        if ((ld->len - bits) < 32)
        {
            return ( (ld->bufb & (0xFFFFFFFF >> (64 - ld->len))) << (bits - ld->len + 32)) |
                (ld->bufa >> (ld->len - bits));
        } else {
            return ((ld->bufb >> (ld->len - bits - 32)) & (0xFFFFFFFF >> (32 - bits)));
        }
    }
}

/* return 1 if position is outside of buffer, 0 otherwise */
char flushbits_hcr( bits_t *ld, unsigned char bits)
{
    ld->len -= bits;

    if (ld->len <0)
    {
        ld->len = 0;
        return 1;
    } else {
        return 0;
    }
}

char getbits_hcr(bits_t *ld, unsigned char n, unsigned int *result)
{
    *result = showbits_hcr(ld, n);
    return flushbits_hcr(ld, n);
}

char get1bit_hcr(bits_t *ld, unsigned char *result)
{
    unsigned int res;
    char ret;

    ret = getbits_hcr(ld, 1, &res);
    *result = (char)(res & 1);
    return ret;
}

/* reversed bit reading routines, used for RVLC and HCR */
void Initbits_rev(bitfile *ld, void *buffer,
                       unsigned int bits_in_buffer)
{
    unsigned int tmp;
    int index;

    ld->buffer_size = bit2byte(bits_in_buffer);

    index = (bits_in_buffer+31)/32 - 1;

    ld->start = (unsigned int*)buffer + index - 2;

    tmp = getdword((unsigned int*)buffer + index);
    ld->bufa = tmp;

    tmp = getdword((unsigned int*)buffer + index - 1);
    ld->bufb = tmp;

    ld->tail = (unsigned int*)buffer + index;

    ld->bits_left = bits_in_buffer % 32;
    if (ld->bits_left == 0)
        ld->bits_left = 32;

    ld->bytes_left = ld->buffer_size;
    ld->error = 0;
}

/* reversed bitreading routines */
static unsigned int Showbits_rev(bitfile *ld, unsigned int bits)
{
    unsigned char i;
    unsigned int B = 0;

    if (bits <= ld->bits_left)
    {
        for (i = 0; i < bits; i++)
        {
            if (ld->bufa & (1 << (i + (32 - ld->bits_left))))
                B |= (1 << (bits - i - 1));
        }
        return B;
    } else {
        for (i = 0; i < ld->bits_left; i++)
        {
            if (ld->bufa & (1 << (i + (32 - ld->bits_left))))
                B |= (1 << (bits - i - 1));
        }
        for (i = 0; i < bits - ld->bits_left; i++)
        {
            if (ld->bufb & (1 << (i + (32-ld->bits_left))))
                B |= (1 << (bits - ld->bits_left - i - 1));
        }
        return B;
    }
}

static void Flushbits_rev(bitfile *ld, unsigned int bits)
{
    /* do nothing if error */
    if (ld->error != 0)
        return;

    if (bits < ld->bits_left)
    {
        ld->bits_left -= bits;
    } else {
        unsigned int tmp;

        ld->bufa = ld->bufb;
        tmp = getdword(ld->start);
        ld->bufb = tmp;
        ld->start--;
        ld->bits_left += (32 - bits);

        if (ld->bytes_left < 4)
        {
            ld->error = 1;
            ld->bytes_left = 0;
        } else {
            ld->bytes_left -= 4;
        }
//        if (ld->bytes_left == 0)
//            ld->no_more_reading = 1;
    }
}

unsigned int Getbits_rev(bitfile *ld, unsigned int n)
{
    unsigned int ret;

    if (n == 0)
        return 0;

    ret = Showbits_rev(ld, n);
    Flushbits_rev(ld, n);


    return ret;
}

unsigned char *Getbitbuffer(bitfile *ld, unsigned int bits)
{
    int i;
    unsigned int temp;
    int bytes = bits >> 3;
    int remainder = bits & 0x7;

    unsigned char *buffer = (unsigned char*)malloc((bytes+1)*sizeof(unsigned char));

    for (i = 0; i < bytes; i++)
    {
        buffer[i] = (unsigned char)Getbits(ld, 8);
    }

    if (remainder)
    {
        temp = Getbits(ld, remainder) << (8-remainder);

        buffer[bytes] = (unsigned char)temp;
    }

    return buffer;
}


#endif
#ifdef DRM
/* return the original data buffer */
void *faad_origbitbuffer(bitfile *ld)
{
    return (void*)ld->start;
}

/* return the original data buffer size */
unsigned int faad_origbitbuffer_size(bitfile *ld)
{
    return ld->buffer_size;
}

/* rewind to beginning */
static void Rewindbits(bitfile *ld)
{
    unsigned int tmp;

    ld->bytes_left = ld->buffer_size;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((unsigned int*)&ld->start[0]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((unsigned int*)&ld->start[0], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufa = tmp;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((unsigned int*)&ld->start[1]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((unsigned int*)&ld->start[1], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;

    ld->bits_left = 32;
    ld->tail = &ld->start[2];
}

unsigned char faad_check_CRC(bitfile *ld, unsigned short len)
{
    int bytes, rem;
    unsigned int CRC;
    unsigned int r=255;  /* Initialize to all ones */

    /* CRC polynome used x^8 + x^4 + x^3 + x^2 +1 */
#define GPOLY 0435

    Rewindbits(ld);

    CRC = (unsigned int) ~Getbits(ld, 8) & 0xFF;          /* CRC is stored inverted */

    bytes = len >> 3;
    rem = len & 0x7;

    for (; bytes > 0; bytes--)
    {
        r = crc_table_G8[( r ^ Getbits(ld, 8) ) & 0xFF];
    }
    for (; rem > 0; rem--)
    {
        r = ( (r << 1) ^ (( ( Get1bit(ld)  & 1) ^ ((r >> 7) & 1)) * GPOLY )) & 0xFF;
    }

    if (r != CRC)
  //  if (0)
    {
        return 28;
    } else {
        return 0;
    }
}

#endif

/* EOF */
