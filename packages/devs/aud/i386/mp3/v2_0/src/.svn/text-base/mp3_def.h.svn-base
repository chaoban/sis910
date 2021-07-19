#ifndef MP3_DEF_H
#define MP3_DEF_H

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880
#endif

#ifndef FALSE
#define         FALSE                   0
#endif
#ifndef TRUE
#define         TRUE                    1
#endif

#define REAL_IS_FLOAT

typedef signed short  sample_t;

#ifdef REAL_IS_FLOAT
#define real float
#elif defined(REAL_IS_LONG_DOUBLE)
#define real long double
#else
#define real double
#endif

#define         FALSE                   0
#define         TRUE                    1

#define         SBLIMIT                 32
#define         SSLIMIT                 18

#define         MPG_MD_STEREO           0
#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3

#define MAXFRAMESIZE 2880

#define         MAX_U_32_NUM            0xFFFFFFFF

/* Pre Shift fo 16 to 8 bit converter table */
#define AUSHIFT (3)

#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008

#define NUMTOCENTRIES 100

#define FRAMES_AND_BYTES (FRAMES_FLAG | BYTES_FLAG)

struct frame {
    int stereo;
    int jsbound;
    int single;
    int lsf;
    int mpeg25;
    int header_change;
    int lay;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */
};

struct gr_info_s {
      int scfsi;
      unsigned part2_3_length;
      unsigned big_values;
      unsigned scalefac_compress;
      unsigned block_type;
      unsigned mixed_block_flag;
      unsigned table_select[3];
      unsigned subblock_gain[3];
      unsigned maxband[3];
      unsigned maxbandl;
      unsigned maxb;
      unsigned region1start;
      unsigned region2start;
      unsigned preflag;
      unsigned scalefac_scale;
      unsigned count1table_select;
      real *full_gain[3];
      real *pow2gain;
};

struct III_sideinfo
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct {
    struct gr_info_s gr[2];
  } ch[2];
};


#define NUMTOCENTRIES 100

/*structure to receive extracted header */
/* toc may be NULL*/
typedef struct
{
  int		h_id;			/* from MPEG header, 0=MPEG2, 1=MPEG1 */
  int		samprate;		/* determined from MPEG header */
  int		flags;			/* from Vbr header data */
  int		frames;			/* total bit stream frames from Vbr header data */
  int		bytes;			/* total bit stream bytes from Vbr header data*/
  int		vbr_scale;		/* encoded vbr scale from Vbr header data*/
  unsigned char	toc[NUMTOCENTRIES];	/* may be NULL if toc not desired*/
  int           headersize;             /* size of VBR header, in bytes */
  int           enc_delay;              /* encoder delay */
  int           enc_padding;            /* encoder paddign added at end of stream */
}   VBRTAGDATA;


#endif
