///////////////////////////////////////////////////////////////////////////////////////////////
//
//	ADEC.h
//
///////////////////////////////////////////////////////////////////////////////////////////////


/**
** \file
** \brief General audio decoding modules and defines
*/

#ifndef ADEC_H
#define ADEC_H

/// Audio types
typedef enum {
    AUDTYPE_INVALID = 0,
    AUDTYPE_AC3,
    AUDTYPE_EAC3,
    AUDTYPE_MLP,
    AUDTYPE_THD,
    AUDTYPE_DTS,
    AUDTYPE_LPCM_BD,
    AUDTYPE_DTS_LBR,
    AUDTYPE_MPG2,
    AUDTYPE_MP3,
    AUDTYPE_AAC,
    AUDTYPE_WMA,
    AUDTYPE_OGG,
    MADTYPE_MP3,
    NUM_AUDTYPE
} AUD_tType;

/// Speaker positions
typedef enum {
    SPK_LEFT = 0,
    SPK_RIGHT,
    SPK_LEFT_SUR,
    SPK_RIGHT_SUR,
    SPK_CENTER,
    SPK_SUBWOOFER,
    SPK_REAR_LEFT_SUR,
    SPK_REAR_RIGHT_SUR,
    SPK_MONO = SPK_LEFT,
    SPK_SUR  = SPK_LEFT_SUR,
    SPK_CENTER_SUR = SPK_REAR_LEFT_SUR,
} AUD_tSpkrPos;

typedef enum {
    AUD_INVALID_TYPE = 0,
    AUD_INVALID_STREAM,
} AUD_tErrorCode;
	
typedef enum
{
 MPEG1 = 0,
 MPEG2 = 1,
 MPEG25 = 2,
}MP3_VERSION;

/// Audio stream attributes
typedef struct {
    AUD_tType   Type;           ///< Audio type
    unsigned    NumCh;          ///< Number of channels
    unsigned    Pts;            ///< PTS at the output
    unsigned    NumFrames;      ///< Number of frames decoded
    unsigned    FrameSize;      ///< Frame size,1152 or 576
    unsigned    BitsPerSamp;    ///< Bits per sample,8 or 16 or 24
    unsigned    SampleFreq;     ///< Sampling frequency
    unsigned    ByteRate;       ///< Byte rate
    unsigned    ChOrder[8];     ///< Stream channel order
    unsigned    MpegVersion;    ///< mpeg version
    unsigned    MpegLayer;      ///< layer version
    unsigned    SongLength;     ///< Song length in secs
    unsigned    SongLengthM;    ///< song length in milliseconds
    unsigned    Status;         ///< current status
    unsigned    IsVBR;          ///< VBR or CBR
    unsigned    c_ID3v1;        ///<ID3 v1>
    unsigned    c_ID3v2;        ///<ID3 v2>
    char        *Artist;
	char        *TrackTitle;
	char        *TrackNum;
	char        *Album;
	char        *Year;
	char        *Comment;
	char        *Genre;
} AUD_tAttr;

#endif /* ADEC_H */
