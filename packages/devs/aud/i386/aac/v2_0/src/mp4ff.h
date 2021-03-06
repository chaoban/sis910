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
** $Id: mp4ff.h,v 1.26 2007/11/01 12:33:29 menno Exp $
**/

#ifndef MP4FF_H
#define MP4FF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <cyg/io/aud/adecctrl.h>

#define MAX_TRACKS 1024
#define TRACK_UNKNOWN 0
#define TRACK_AUDIO   1
#define TRACK_VIDEO   2
#define TRACK_SYSTEM  3


#define SUBATOMIC 128

/* atoms without subatoms */
#define ATOM_FTYP 129
#define ATOM_MDAT 130
#define ATOM_MVHD 131
#define ATOM_TKHD 132
#define ATOM_TREF 133
#define ATOM_MDHD 134
#define ATOM_VMHD 135
#define ATOM_SMHD 136
#define ATOM_HMHD 137
#define ATOM_STSD 138
#define ATOM_STTS 139
#define ATOM_STSZ 140
#define ATOM_STZ2 141
#define ATOM_STCO 142
#define ATOM_STSC 143
#define ATOM_MP4A 144
#define ATOM_MP4V 145
#define ATOM_MP4S 146
#define ATOM_ESDS 147
#define ATOM_META 148 /* iTunes Metadata box */
#define ATOM_NAME 149 /* iTunes Metadata name box */
#define ATOM_DATA 150 /* iTunes Metadata data box */
#define ATOM_CTTS 151
#define ATOM_FRMA 152
#define ATOM_IVIV 153
#define ATOM_PRIV 154
#define ATOM_USER 155
#define ATOM_KEY  156

#define ATOM_UNKNOWN 255
#define ATOM_FREE ATOM_UNKNOWN
#define ATOM_SKIP ATOM_UNKNOWN

/* atoms with subatoms */
#define ATOM_MOOV 1
#define ATOM_TRAK 2
#define ATOM_EDTS 3
#define ATOM_MDIA 4
#define ATOM_MINF 5
#define ATOM_STBL 6
#define ATOM_UDTA 7
#define ATOM_ILST 8 /* iTunes Metadata list */
#define ATOM_TITLE 9
#define ATOM_ARTIST 10
#define ATOM_WRITER 11
#define ATOM_ALBUM 12
#define ATOM_DATE 13
#define ATOM_TOOL 14
#define ATOM_COMMENT 15
#define ATOM_GENRE1 16
#define ATOM_TRACK 17
#define ATOM_DISC 18
#define ATOM_COMPILATION 19
#define ATOM_GENRE2 20
#define ATOM_TEMPO 21
#define ATOM_COVER 22
#define ATOM_DRMS 23
#define ATOM_SINF 24
#define ATOM_SCHI 25

#ifdef HAVE_CONFIG_H
#include "../../config.h"   
#endif

#if !(defined(_WIN32) || defined(_WIN32_WCE))
#define stricmp strcasecmp
#else
#define stricmp _stricmp
#define strdup _strdup
#endif

/* metadata tag structure */
typedef struct
{
    char *item;
    char *value;
} mp4ff_tag_t;

/* metadata list structure */
typedef struct
{
    mp4ff_tag_t *tags;
    unsigned int count;
} mp4ff_metadata_t;


typedef struct
{
    int type;
    int channelCount;
    int sampleSize;
    unsigned short sampleRate;
    int audioType;

    /* stsd */
    int stsd_entry_count;

    /* stsz */
    int stsz_sample_size;
    int stsz_sample_count;
    int *stsz_table;

    /* stts */
    int stts_entry_count;
    int *stts_sample_count;
    int *stts_sample_delta;

    /* stsc */
    int stsc_entry_count;
    int *stsc_first_chunk;
    int *stsc_samples_per_chunk;
    int *stsc_sample_desc_index;

    /* stsc */
    int stco_entry_count;
    int *stco_chunk_offset;

    /* ctts */
    int ctts_entry_count;
    int *ctts_sample_count;
    int *ctts_sample_offset;

    /* esde */
    unsigned char *decoderConfig;
    int decoderConfigLen;

    unsigned int maxBitrate;
    unsigned int avgBitrate;

    unsigned int timeScale;
    unsigned long duration;

} mp4ff_track_t;

typedef struct
{
    unsigned int (*read)(unsigned char *buffer, unsigned int length);
    unsigned int (*seek)(unsigned long position);
} mp4ff_callback_t;

/* mp4 main file structure */
typedef struct
{
    /* stream to read from */
    mp4ff_callback_t *stream;
    long current_position;

    int moov_read;
    unsigned long moov_offset;
    unsigned long moov_size;
    unsigned char last_atom;
    unsigned long file_size;

    /* mvhd */
    int time_scale;
    int duration;

    /* incremental track index while reading the file */
    int total_tracks;

    /* track data */
    mp4ff_track_t *track[MAX_TRACKS];

    /* metadata */
    mp4ff_metadata_t tags;
} mp4ff_t;


/* mp4util.c */
int mp4ff_read_data(mp4ff_t *f, char *data, unsigned int size);
int mp4ff_write_data(mp4ff_t *f, char *data, unsigned int size);
unsigned long mp4ff_read_int64(mp4ff_t *f);
unsigned int mp4ff_read_int32(mp4ff_t *f);
unsigned int mp4ff_read_int24(mp4ff_t *f);
unsigned short mp4ff_read_int16(mp4ff_t *f);
unsigned char mp4ff_read_char(mp4ff_t *f);
int mp4ff_write_int32(mp4ff_t *f,const unsigned int data);
unsigned int mp4ff_read_mp4_descr_length(mp4ff_t *f);
long mp4ff_position(const mp4ff_t *f);
int mp4ff_set_position(mp4ff_t *f, const long position);
int mp4ff_truncate(mp4ff_t * f);
char * mp4ff_read_string(mp4ff_t * f,unsigned int length);

/* mp4atom.c */
unsigned long mp4ff_atom_read_header(mp4ff_t *f, unsigned char *atom_type, unsigned char *header_size);

int mp4ff_atom_read(mp4ff_t *f, const int size, const unsigned char atom_type);

#ifdef USE_TAGGING
int mp4ff_read_meta(mp4ff_t *f, const unsigned long size);
#endif
int mp4ff_audio_frame_size(const mp4ff_t *f, const int track, const int sample);
int mp4ff_set_sample_position(mp4ff_t *f, const int track, const int sample);

#ifdef USE_TAGGING
/* mp4meta.c */
static int mp4ff_tag_add_field(mp4ff_metadata_t *tags, const char *item, const char *value);
static int mp4ff_tag_set_field(mp4ff_metadata_t *tags, const char *item, const char *value);
static int mp4ff_set_metadata_name(mp4ff_t *f, const unsigned char atom_type, char **name);
static int mp4ff_parse_tag(mp4ff_t *f, const unsigned char parent_atom_type, const int size);
static int mp4ff_meta_find_by_name(const mp4ff_t *f, const char *item, char **value);
int mp4ff_parse_metadata(mp4ff_t *f, const int size);
int mp4ff_tag_delete(mp4ff_metadata_t *tags);
int mp4ff_meta_get_num_items(const mp4ff_t *f);
int mp4ff_meta_get_by_index(const mp4ff_t *f, unsigned int index,
                            char **item, char **value);
int mp4ff_meta_get_title(const mp4ff_t *f, char **value);
int mp4ff_meta_get_artist(const mp4ff_t *f, char **value);
int mp4ff_meta_get_writer(const mp4ff_t *f, char **value);
int mp4ff_meta_get_album(const mp4ff_t *f, char **value);
int mp4ff_meta_get_date(const mp4ff_t *f, char **value);
int mp4ff_meta_get_tool(const mp4ff_t *f, char **value);
int mp4ff_meta_get_comment(const mp4ff_t *f, char **value);
int mp4ff_meta_get_genre(const mp4ff_t *f, char **value);
int mp4ff_meta_get_track(const mp4ff_t *f, char **value);
int mp4ff_meta_get_disc(const mp4ff_t *f, char **value);
int mp4ff_meta_get_compilation(const mp4ff_t *f, char **value);
int mp4ff_meta_get_tempo(const mp4ff_t *f, char **value);
int mp4ff_meta_get_coverart(const mp4ff_t *f, char **value);
#endif

/* mp4ff.c */
#ifdef USE_TAGGING
mp4ff_t *mp4ff_open_edit(mp4ff_callback_t *f);
#endif
void mp4ff_close(mp4ff_t *ff);
//void mp4ff_track_add(mp4ff_t *f);
int parse_sub_atoms(mp4ff_t *f, const unsigned long total_size,int meta_only);
int parse_atoms(mp4ff_t *f,int meta_only);

int mp4ff_get_sample_duration(const mp4ff_t *f, const int track, const int sample);
long mp4ff_get_sample_position(const mp4ff_t *f, const int track, const int sample);
int mp4ff_get_sample_offset(const mp4ff_t *f, const int track, const int sample);
int mp4ff_find_sample(const mp4ff_t *f, const int track, const long offset,int * toskip);

unsigned int mp4ff_read_sample(mp4ff_t *f, const int track, const int sample);
int mp4ff_get_decoder_config(const mp4ff_t *f, const int track,
                                 unsigned char** ppBuf, unsigned int* pBufSize);
int mp4ff_total_tracks(const mp4ff_t *f);
int mp4ff_time_scale(const mp4ff_t *f, const int track);
int mp4ff_num_samples(const mp4ff_t *f, const int track);

unsigned int mp4ff_meta_genre_to_index(const char * genrestr);//returns 1-based index, 0 if not found
const char * mp4ff_meta_index_to_genre(unsigned int idx);//returns pointer to static string

int Init_MP4file(AUD_tCtrlBlk * pAcb);
/* API */
#endif
