/* tcx 2008-03-24 */

/************************************************************
 * common.h contains general definitions and global variables
 * declaration, always include as 1st header in all files
 ************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

#include <pkgconf/devs_aud_wma.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * **************** General Definitions ****************
 * for values which is 0 , use GBLDEF0 will place them in BSS.
 */
#ifdef MAIN
#define GBLDEF(name,value)      name = value
#define GBLDEF_0(name,value)    name = value
#define GBLDEF0(name)           name
#else
#define GBLDEF(name,value)      extern name
#define GBLDEF_0(name, value)   extern name 
#define GBLDEF0(name)           extern name 
#endif

#ifdef SISPKG_IO_AUD_DEC_WMA_DEBUG
#define EPRINTF(x)	do {printf x;} while (0)
#define VPRINTF(x)  	do {printf x;} while (0)
#define GPRINTF(x)	do {printf x;} while (0)
#else
#define EPRINTF(a)	{}
#define VPRINTF(x) 	{}
#define GPRINTF(x)	{}
#endif

/************************************************************************
    Very Useful 
*************************************************************************/
#define cat2(i,j)       i##j
#define cat3(i,j,k)     i##j##k

#define max2(a,b)   ((a)>(b)?(a):(b))
#define max3(a,b,c) max2(max2(a,b),c)
#define min2(a,b)   ((a)<(b)?(a):(b))
#define min3(a,b,c) min2(min2(a,b),c)

#define uncache(a)  (0x40000000 | (int) (a))
#define cache(a)    ((int)(a) & ~0x40000000)

#define SWAP(a,b)   { int t; t=a; a=b; b=t; }

#define DIM(xx)		((sizeof(xx))/sizeof((xx)[0]))
#define PROTECT(a)      (a)

/************************************************************
 * for portability reason
 ************************************************************/
#ifndef uchar_type
typedef unsigned char   uchar;
#define uchar_type
#endif

typedef unsigned short  ushort;

#ifndef uint_type
typedef unsigned int    uint;
#define uint_type
#endif

typedef unsigned short  wchar;

#ifndef TYPE_T
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long long    int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef signed char         INT8;
typedef signed short        INT16;
typedef signed int          INT32;
typedef signed long long    INT64;

typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef unsigned long       UNSIGNED;
/*
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
*/
#define TYPE_T
#endif

/************************************************************
 * some common macros
 ************************************************************/

#ifndef OKAY
#define OKAY    0
#endif

#ifndef ERROR
#define ERROR   -1
#endif

#ifndef FAIL
#define FAIL    -1
#endif

#ifndef TRUE
#define TRUE    (1 == 1)
#define FALSE   (!TRUE)
#endif

#ifndef NULL
#define NULL    0
#endif

/************************************************************
 * classifier for public functions
 ************************************************************/
#define EXPORT          extern
#define IMPORT          extern
#define PUBLIC
#define PRIVATE         static

/************************************************************
 * for bit opt 
 ************************************************************/
/// for bit opt
extern uchar* gpBitBuff; //dst buff for ASF DMX; src buff for WMA Decoder
extern int bitBuffSetIndex;
extern int bitBuffGetIndex;

#define bb_reset()	do {\
	bitBuffSetIndex = 0;\
	bitBuffGetIndex = 0;\
}while(0)

#define bb_memcpy(src, size)	do {\
	memcpy((gpBitBuff + bitBuffSetIndex), (src), (size));\
	bitBuffSetIndex += (size);\
}while(0)

#define bb_set8(val)	do {\
	*(gpBitBuff + bitBuffSetIndex) = (uchar)(val);\
	bitBuffSetIndex += 1;\
}while(0)

#define bb_setle16(val)		do {\
	bb_set8((val)&0xff);\
	bb_set8(((val)>>8)&0xff);\
}while(0)

#define bb_setle32(val)	do {\
	bb_setle16((val)&0xffff);\
	bb_setle16(((val)>>16)&0xffff);\
}while(0)


int ExtractBits(int numbits); // this function implemented in wma.c
int PeekBits(int numbits); // this function implemented in wma.c


/************************************************************
 * DMX
 ************************************************************/
enum { 
    DMX_STATUS_STOPPED,     ///< Idle
    DMX_STATUS_ACTIVE,      ///< Processing or demuxing the file
    DMX_STATUS_ERROR        ///< Stopped, in error state
};

enum { 
    DMX_STREAM_TYPE_UNKNOWN = 0,
    DMX_STREAM_TYPE_VIDEO = 1,
    DMX_STREAM_TYPE_AUDIO = 2,
    DMX_STREAM_TYPE_SUB_PIC = 3
};

/// MIME types.
/// Make sure this matches with MimeTypeName[][] in names.h.
typedef enum {
    MIMETYPE_UNKNOWN     = 0, ///< Initial value
    MIMETYPE_UNDEFINED   = 1, ///< Unsupported
    MIMETYPE_DIRECTORY   = 2, ///< Not a file

    MIMETYPE_MPEG1V,        ///< MPEG1 pure video
    MIMETYPE_MPEG2V,        ///< MPEG2 pure video
    MIMETYPE_MPEG4V,	    ///< MPEG 4 video
    MIMETYPE_VC1,           ///< VC1 video Annex E, EBDU
    MIMETYPE_RCV,           ///< WMV3/RCV, VC1 video Annex L, Metadata with minor difference
    MIMETYPE_JPEG_VID,      ///< JPEG playback hack using VDEC
    MIMETYPE_264,           ///< pure H264 video in "es" format
    MIMETYPE_264BS,         ///< H264 video in Annex B byte stream format
    MIMETYPE_AVS,           ///< AVS pure video
    MIMETYPE_RV30,         ///< RM video
    MIMETYPE_RV9,           ///< RV9 video
    
    PURE_VIDEO_FIRST        = MIMETYPE_MPEG1V,
    PURE_VIDEO_LAST         = MIMETYPE_RV9,

    MIMETYPE_AAC,	   ///< AAC but not sure if ADIF or ADTS
    MIMETYPE_AAC_ADIF,	   ///< AAC ADIF
    MIMETYPE_AAC_ADTS,	   ///< AAC ADTS
    MIMETYPE_AC3,	   ///< AC3 aka Dolby Digital
    MIMETYPE_DD_PLUS,      ///< Dolby Digital Plus
    MIMETYPE_DD_PLUS2,     ///< DD+ for secondary audio
    MIMETYPE_AMR,	   ///< AMR
    MIMETYPE_CDDA,	   ///< CDDA
    MIMETYPE_DMC,
    MIMETYPE_DTS,	   ///< DTS
    MIMETYPE_DTS_HD,       ///< DTS-HD
    MIMETYPE_DTS_XLL,      ///< DTS-HD XLL
    MIMETYPE_DTS_LBR,      ///< DTS-HD LBR
    MIMETYPE_FLAC,         ///< FLAC
    MIMETYPE_LPCM,         ///< LPCM
    MIMETYPE_LPCM_BD,	   ///< Blu-ray LPCM
    MIMETYPE_MLP,	   ///< MLP for DVD audio
    MIMETYPE_MLP_HD,	   ///< Dolby TrueHD
    MIMETYPE_MP3,	   ///< MP3
    MIMETYPE_MP3_LSF,	   ///< MP3 LSF
    MIMETYPE_MPA,	   ///< MPEG audio
    MIMETYPE_OGG,	   ///< OGG with only Vorbis
    MIMETYPE_PCM,	   ///< PCM ???
    MIMETYPE_SACD,
    MIMETYPE_VORBIS,	   ///< Pure Vorbis
    MIMETYPE_WAV,	   ///< WAVE

    PURE_AUDIO_FIRST	= MIMETYPE_AAC,
    PURE_AUDIO_LAST	= MIMETYPE_WAV,

    MIMETYPE_BMP,	   ///< BMP
    MIMETYPE_GIF,	   ///< GIF
    MIMETYPE_JPEG,         ///< JPEG
    MIMETYPE_MJPEG,	   ///< AVI with motion JPEG
    MIMETYPE_PNG,	   ///< PNG
    MIMETYPE_MNG,	   ///< MNG
    MIMETYPE_TIF,	   ///< TIFF

    STILL_FIRST		= MIMETYPE_BMP,
    STILL_LAST		= MIMETYPE_TIF,

    MIMETYPE_BD_PG,	   ///< BD presentation graphics
    MIMETYPE_BD_IG,	   ///< BD interactive graphics
    MIMETYPE_BD_TSUB,	   ///< BD text subtitle

    MIMETYPE_CDG,	   ///< CDG graphics
    MIMETYPE_DVD_A_PS,	   ///< DVD-Audio MPEG2 program stream
    MIMETYPE_DVD_V_PS,	   ///< DVD-Video MPEG2 program stream
    MIMETYPE_DVD_ASVU_PS,  ///< DVD-Audio ASVU file
    MIMETYPE_HDDVD_V_PS,   ///< HD-DVD-Video MPEG2 program stream
    
    MIMETYPE_DVD_VR_PS,	   ///< DVD-VR MPEG2 program stream
    MIMETYPE_M4A,	   ///< MPEG4 system with audio only (e.g. iPod!)
    MIMETYPE_MCG,	   ///< Proprietory MP3 + CDG
    MIMETYPE_MP3CDG,	   ///< ESS MP3 + CDG
    MIMETYPE_MPEG,	   ///< Unspecified MPEG stream
    MIMETYPE_MPEG1_SS,	   ///< Generic MPEG1 system
    MIMETYPE_MPEG2_PS,	   ///< Generic MPEG2 program
    MIMETYPE_MPEG2_TS,	   ///< Generic MPEG2 transport, 188 bytes packet
    MIMETYPE_MPEG2_TS192,  ///< BD MPEG2 transport, 192 bytes packet
    MIMETYPE_MPEG2_TS204,  ///< Generic MPEG2 transport, 204 bytes packet
    MIMETYPE_VCD,	   ///< VCD MPEG1 system

    PACKETIZED_FIRST	= MIMETYPE_CDG,
    PACKETIZED_LAST	= MIMETYPE_VCD,

    MIMETYPE_ASF,	   ///< ASF with unknown contents
    MIMETYPE_AVI,	   ///< AVI with unknown contents
    MIMETYPE_DIVX,	   ///< AVI with MPEG4 video and whatever audio
    MIMETYPE_MPEG4,	   ///< MPEG 4 system
    MIMETYPE_OGM,	   ///< OGG with unknown contents
    MIMETYPE_MKV,	   ///< Matroska
    MIMETYPE_RM,	   ///< RealMedia container
    MIMETYPE_RMVB,	   ///< RealMedia container variable bitrate
    MIMETYPE_WMA,      ///< ASF with WMA
    MIMETYPE_WMV,	   ///< ASF with WMV. Might have WMA too. 

    MUXED_FIRST		= MIMETYPE_ASF,
    MUXED_LAST		= MIMETYPE_WMV,

    MIMETYPE_DVD_A_NAV,	   ///< DVD-Audio info file
    MIMETYPE_DVD_V_NAV,	   ///< DVD-Video info
    MIMETYPE_DVD_VR_NAV,   ///< DVD-VR info
    MIMETYPE_DPOF,	   ///< DPOF
    MIMETYPE_M3U,	   ///< M3U

    MIMETYPE_NO_MORE

} tMimeType;

/// Information about sub-stream in a file
typedef struct DMX_tStreamInfo {
    int         SSNum;          ///< Identifying sub-stream number
    int         Type;           ///< DMX_SUB_STREAM_TYPE_xxxx
    tMimeType   mime_type;      ///< Mime_type of sub-stream
    unsigned    StartPts;       ///< Starting PTS of sub-stream. 0 if unknown
    int         MaxBitRate;     ///< Maximum bit rate. 0 if unknown
    int         AvgBitRate;     ///< Average bit rate. 0 if unknown
} DMX_tStreamInfo;

#define DMX_MAX_STREAM  8

/// General information about a file
typedef struct DMX_tFileSummary {
    unsigned            StartPts;       ///< Starting PTS in the file
    int                 NumStream;      ///< Number of streams in the file
    int                 MaxBitRate;     ///< Maximum bit rate. 0 if unknown
    int                 AvgBitRate;     ///< Average bit rate. 0 if unknown
    DMX_tStreamInfo     pStreamInfoTbl[DMX_MAX_STREAM];
                                        ///< Basic info for each stream
} DMX_tFileSummary;

typedef struct DMX_tCtxt {
	int                 MimeType;
	//    FILE_INFO           *pFileInfo; ///< Source file FS access info
	DMX_tFileSummary    FileSummary;   ///< Summary info about the source file
	//    struct DMX_tModule  *pModule;   ///< Module corresponding 
	                                ///< to the file format
	void                *pPriv;     /// Module's private variables/structure.
} DMX_tCtxt;

#endif /* _COMMON_H_ */
