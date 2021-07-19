#ifndef _ASF_H_
#define _ASF_H_

#include "common.h"

#define own
typedef enum tagMediaType_ASF
{
	Audio_ASF=0,
	Video_ASF=1,
	TOTAL_MEDIA_TYPE=2
} EMediaType_ASF;

typedef struct tagGUID {
    uint32  Data1;
    uint16  Data2;
    uint16  Data3;
    uint8   Data4[8];
} tGUID;

/************************************************************************/
#ifndef MAKEFOURCC_ASF
#define MAKEFOURCC_ASF(ch0, ch1, ch2, ch3)                              \
	((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) |               \
	((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))

#define mmioFOURCC_ASF(ch0, ch1, ch2, ch3)  MAKEFOURCC_ASF(ch0, ch1, ch2, ch3)
#endif

#define DEFINE_GUID_ASF(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)    \
	const tGUID name                                                        \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }


#define IsEqualGUID_ASF(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(tGUID)))

/******* video output type guids, in preference order  *****/
#define FOURCC_WMVP     mmioFOURCC_ASF('W','M','V','P')
#define FOURCC_WMV3     mmioFOURCC_ASF('W','M','V','3')
#define FOURCC_WMVA     mmioFOURCC_ASF('W','M','V','A')
#define FOURCC_WVC1     mmioFOURCC_ASF('W','V','C','1')
#define FOURCC_WMV2     mmioFOURCC_ASF('W','M','V','2')
#define FOURCC_WMV1     mmioFOURCC_ASF('W','M','V','1')
#define FOURCC_M4S2     mmioFOURCC_ASF('M','4','S','2')
#define FOURCC_MP43     mmioFOURCC_ASF('M','P','4','3')
#define FOURCC_mp43     mmioFOURCC_ASF('m','m','4','3')
#define FOURCC_MP4S     mmioFOURCC_ASF('M','P','4','S')
#define FOURCC_mp4s     mmioFOURCC_ASF('m','p','4','s')
#define FOURCC_MP42     mmioFOURCC_ASF('M','P','4','2')
#define FOURCC_mp42     mmioFOURCC_ASF('m','m','4','2')
#define FOURCC_MSS1     mmioFOURCC_ASF('M','S','S','1')
#define FOURCC_MSS2     mmioFOURCC_ASF('M','S','S','2')
#define FOURCC_WMS2     mmioFOURCC_ASF('W','M','S','2')
#define FOURCC_WMS1     mmioFOURCC_ASF('W','M','S','1')
#define FOURCC_MPG4     mmioFOURCC_ASF('M','P','G','4')
#define FOURCC_MSS1     mmioFOURCC_ASF('M','S','S','1')
#define FOURCC_MSS2     mmioFOURCC_ASF('M','S','S','2')
/***********************************************************/


/******* video intput type guids, in preference order  *****/
#define FOURCC_I420             0x30323449
#define FOURCC_IYUV             0x56555949
#define FOURCC_YV12             0x32315659
#define FOURCC_YUY2             0x32595559
#define FOURCC_UYVY             0x59565955
#define FOURCC_YVYU             0x55595659
#define FOURCC_YVU9             0x39555659
#define FOURCC_BI_RGB           0x00000000
#define FOURCC_BI_BITFIELDS     0x00000003
#define FOURCC_BI_RLE8          0x00000001
#define FOURCC_BI_RLE4          0x00000002

////////////////////////////////////////////

#define WAVE_FORMAT_PCM                 0x0001
//#define WAVE_FORMAT_WMA1_ASF            0x0160
#define WAVE_FORMAT_WMA_STD             0x0161
#define WAVE_FORMAT_WMA_PRO             0x0162
#define WAVE_FORMAT_WMA_LLS             0x0163
#define WAVE_FORMAT_WMA_VOICE           0x000A

///////////////////////////////////////////

#define ASF_DRM_RIGHT_NONSDMI   0x02
#define ASF_DRM_RIGHT_SDMI      0x10

//#define ARRAYSIZE(x) sizeof(x)/sizeof(x[0])

typedef struct TAGAudioStreamInfo
{
	uint16      u16FormatTag;
	uint16      u16NumChannels;
	uint32      u32SamplesPerSecond;
	uint32      u32BytesPerSecond;
	uint16      u16BlockAlign;
	uint16      u16BitsPerSample;
	uint16      u16CodecSpecifDataLen;
	//uint16         u16ValidBitsPerSample; // bits of precision
	//uint32         u32ChannelMask;       // which channels are present in stream
	//uint16         u16StreamId;
} tAudioInfo;

typedef struct TAGVideoStreamInfo
{
	uint32      u32Width;
	uint32      u32Height;
	uint8       u8Reserved;
	uint16      u16FormatDataSize;
	uint32      biSize;
	int32       biWidth;
	int32       biHeight;
	uint16      biPlanes;
	uint16      biBitCount;
	uint32      biCompression;
	uint32      biSizeImage;
	int32       biXPelsPerMeter;
	int32       biYPelsPerMeter;
	uint32      biClrUsed;
	uint32      biClrImportant;
} tVideoInfo;

typedef enum tagVideoProfile_ASF
{
	SIMPLE_PROFILE = 0,
	MAIN_PROFILE,
	PC_PROFILE,
	MAIN_BROADCAST_PROFILE  // advanced profile ?
} tVideoProfile_ASF;


typedef enum tagVideoFormat_ASF
{
	// uncompressed
	YUY2_WMV,
	UYVY_WMV,
	YVYU_WMV,
	RGB24_WMV,
	RGB555_WMV,
	RGB565_WMV,
	RGB32_WMV,
	RGB8_WMV,
	IYUV_WMV,
	I420_WMV,
	YV12_WMV,
	RGB565_444_WMV,

	// compressed
	WMV2_WMV,
	WMV1_WMV,

	// Ignore Audio Only
	IGNORE_VIDEO
	/*
	WMS2_WMV,
	WMS1_WMV,
	M4S2_WMV,
	MP4S_WMV,
	MP43_WMV,
	MP42_WMV,
	MPG4_WMV
	*/
} tVideoFormat_ASF;

typedef enum tagASFDecodeDispRotateDegree
{
	ASF_DispRotate0 = 0,
	ASF_DispRotate90,
	ASF_DispFlip,
	ASF_DispRotate270
} tASFDecodeDispRotateDegree;



typedef enum tagAudioType_ASF
{
	// uncompressed
	PCM_WMA,
	WMA3_WMA,
	WMA2_WMA,
	WMA1_WMA
} tAudioType_ASF;

typedef struct tagHeaderObj_ASF
{
	tGUID	        gObjID;
	uint64			u64ObjSize;
	uint32			u32NumObj;
	int8			u8Reserved1;
	int8			u8Reserved2;
} tHeaderObj;

typedef struct tagFileProp_ASF
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	tGUID			gFileID;
	uint64			u64FileSize;
	uint64			u64CreateDate;
	uint64			u64PacketCount;
	uint64			u64PlayDuration;
	uint64			u64SendDuration;
	uint64			u64Preroll;
	uint32			u32Flags;
	uint32			u32MinPackSize;
	uint32			u32MaxPackSize;
	uint32			u32MaxBitrate;
} tFileProp_ASF;

#define	EXTRA_BYTE_STREAM_PROP	10
typedef struct tagStreamProp_ASF
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	tGUID			gStreamType;
	tGUID			gErrCorrectionType;
	uint64			u64TimeOffset;
	uint32			u32TypeDataLength;
	uint32			u32ErrCorrectionLength;
	uint16			u16Flags;
	uint32			u32Reserved;

	uint8			uStreamNum;
	uint8			uEncryptFlag;
	own uint8		*pTypeData;
	own uint8		*pErrCorrectionData;
} tStreamProp_ASF;

typedef struct tagHeaderExt_ASF
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	tGUID			gReserved;
	uint16			uReserved;
	uint32			uDataSize;
} tHeaderExt_ASF;

typedef struct strBinaryStreamInfo_ASF
{
	int32           bNothing;
	uint16          u16StreamId;
} strBinaryInfo_ASF;

typedef struct tagASFContentDescription
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	uint16		    uTitleLen;
	uint16		    uAuthorLen;
	uint16		    uCopyrightLen;
	uint16		    uDescriptionLen;    /* rarely used */
	uint16		    uRatingLen;         /* rarely used */

	own wchar		*pwTitle;
	own wchar		*pwAuthor;
	own wchar		*pwCopyright;
	own wchar		*pwDescription;
	own wchar		*pwRating;
} tContentDescription;

typedef struct tagBitrateRecords
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	uint16	        u16Flags;
	uint32	        u32AveBitrate;
} tBitrateRec;

typedef struct tagBitStreamBitrateProp
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	uint32	        u16RecCount;
	own tBitrateRec *pRecord;
} tStreamBitrateProp;


/******************************************************************
**   Extended content description types:
**   ECD_STRING -- wchar string
**   ECD_BINARY -- binary (byte) data
**   ECD_BOOL   -- BOOL (int) data
**   ECD_WORD  -- contains one word,
**   ECD_DWORD  -- contains one dword,
**   ECD_QWORD  -- contains one qword,
*******************************************************************/

enum
{
	ECD_STRING  = 0,
	ECD_BINARY  = 1,
	ECD_BOOL    = 2,
	ECD_DWORD   = 3,
	ECD_QWORD   = 4,
	ECD_WORD    = 5
};

typedef struct tagASFMarkerEntry
{
	tGUID		    gObjID;
	uint64			u64ObjSize;

	uint64          m_qOffset;
	uint64          m_qtime;
	uint16          m_wEntryLen;
	uint32          m_dwSendTime;
	uint32          m_dwFlags;
	uint32          m_dwDescLen;
	own uint16      *m_pwDescName;
} tMarkerEntry;

typedef struct tagASFIndexEntries
{
	/*
	tGUID			gObjID;
	uint64			u64ObjSize;
	*/
	uint32          u32PacketNum;
	uint16          u16PacketCount;
} tSimpleIndexEntry;

typedef struct tagSimpleIndexObject
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	tGUID			gFileID;
	uint64			u64TimeInterval;
	uint32			u32MaxPacketCount;
	uint32			u32EntryCount;
	own tSimpleIndexEntry	*pEntry;
} tSimpleIndexObject;

typedef struct tagASFCodecEntry
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	uint16		    u16CodecType;       // 0x0001   Video codec
	                                    // 0x0002   Audio codec
	                                    // 0xFFFF   Unknown codec
	uint16		    u16CodecNameLength;     // Specifies the number of Unicode characters stored in the Codec Name field.
	own wchar		*pbCodecName;           // Specifies an array of Unicode characters that contains the name of the codec used to create the content.
	uint16		    u16CodecDescLength;     // Specifies the number of Unicode characters stored in the Codec Description field.
	own wchar		*pbCodecDescription;    // Specifies an array of Unicode characters that contains the description of the format used to create the content.
	uint16		    u16CodecInfoLen;        // Specifies the number of bytes stored in the Codec Information field.
	own uint8		*pbCodecInfo;           // Specifies an opaque array of information bytes about the codec used to create the content.
} tASFCodecEntry;

typedef struct tagASFMetaDataDescRecords
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	uint16          wLangIdIndex;   // Specifies the index of the language ID in the Language List Object
	uint16          wStreamNumber;  // Specifies whether the entry applies to a specific media stream or
	                                // whether it applies to the whole file. A value of 0 in this field
	                                // indicates that it applies to the whole file; otherwise, the entry
	                                // applies only to the indicated stream number.
	uint16          wNameLenth;     // Specifies the number of valid bytes stored in the Name field.
	uint16          wDataType;      // Specifies the type of information being stored. The following values are recognized:
	                                // Value Type   Description
	                                //  0x0000      Unicode string. The data consists of a sequence of Unicode characters.
	                                //  0x0001      BYTE array. The type of the data is implementation-specific.
	                                //  0x0002      BOOL. The data is 2 bytes long and should be interpreted as a 16-bit unsigned integer. Only 0x0000 or 0x0001 are permitted values.
	                                //  0x0003      DWORD. The data is 4 bytes long and should be interpreted as a 32-bit unsigned integer.
	                                //  0x0004      QWORD. The data is 8 bytes long and should be interpreted as a 64-bit unsigned integer.
	                                //  0x0005      WORD. The data is 2 bytes long and should be interpreted as a 16-bit unsigned integer.
	                                //  0x0006      GUID. The data is 8 bytes long and should be interpreted as a 128-bit GUID.
	                                //  0x8000      Extended Unicode string. The data consists of a sequence of Unicode characters.
	                                //  0x8001      Extended BYTE array. The type of the data is implementation-specific.
	                                //  0x8002      Extended BOOL. The data is 2 bytes long and should be interpreted as a 16-bit unsigned integer.  Only 0x0000 or 0x0001 are permitted values.
	                                //  0x8003      Extended DWORD. The data is 4 bytes long and should be interpreted as a 32-bit unsigned integer.
	                                //  0x8004      Extended QWORD. The data is 8 bytes long and should be interpreted as a 64-bit unsigned integer.
	                                //  0x8005      Extended WORD. The data is 2 bytes long and should be interpreted as a 16-bit unsigned integer.
	                                //  0x8006      Extended GUID. The data is 8 bytes long and should be interpreted as a 128-bit GUID.
	uint32          wDataLength;    // Specifies the length in bytes of the Data field.
	own uint16      *pwName;        // Specifies the name that uniquely identifies the attribute being described. Names are case-sensitive.
	own void        *pData;         // Specifies the actual metadata being stored. The Data field should be interpreted according to the value
	                                // stored in the Data Type field. If the Data Type is one of the extended values, this field may not contain
	                                //any data if the file is being streamed over the wire.
} ASFMetaDataDescRecords;


typedef struct tagASFMetaDataEntry
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	uint16          m_wDescRecordsCount;
	own ASFMetaDataDescRecords *pDescRec;
} tASFMetaDataEntry;


typedef struct tagASFData
{
	tGUID			gObjID;
	uint64			u64ObjSize;

	tGUID			gFileID;
	uint64			u64TotalDataPackets;
	int16			u16Reserved;
} tASFDataObj;


typedef struct tagASFPayLoadEntry
{
	uint32	        uFrameNum;
	int64	        iPts;
	int64	        iOffset;        // current offset
	uint32	        uLength;        // current len
	uint32          uFrmLen;
#if DEBUG
	uint32          uFrmLenAcc;     // accrued frame len
#endif

	uint32	        uPacketNum;
	uint8           isKey;
	uint8	        iLast;
	uint8           order;
} tASFPayLoadEntry;

typedef struct tagPacketParser
{
	tASFDataObj		stASFDataHeader;

	int64			iTotalStartOffset;
	int64			iTotalEndOffset;
	uint32			uPacketSize;

	uint32			uCurPacket;
	uint32			uCurSendTime;
	int64			uCurPacketOffset;

	tASFPayLoadEntry	pPayloadList[TOTAL_MEDIA_TYPE];
	//	uint32			uAVAccessIndex[TOTAL_MEDIA_TYPE];
	//	uint32			uAVParseIndex[TOTAL_MEDIA_TYPE];

} tPacketParser;

/*
typedef struct tagPacketMuxer
{
	tASFDataObj		stASFDataHeader;
	int64		    iStartOffset;
	int64		    iEndOffset;
	uint32		    uPacketSize;

	uint32		    uCurPacket;
	uint32		    uBufOffset;
	own uint8		*pBuf;
	uint64		    uCurTimeStamp;

	uint32		    uKeyFrameMax;
	uint32		    uKeyFrame;
	own tASFPayLoadEntry	*pKeyLoad;
} tPacketMuxer;
*/
// for mux only
typedef struct tagPayloadParse
{
	uint8		    u8LenTypeFlags;
	uint8		    u8PropFlags;
	uint16		    u16PaddingLen;
	uint32		    u32SendTime;
	uint16		    u16Duration;
} tPayLoadParse;

typedef struct tagSinglePayload
{
	uint8		    u8StreamNum;
	uint8		    u8MediaObjNum;
	uint32		    u32OffsetMediaObject;
	uint8		    u8RepDataLen;
	uint32		    u32MediaObjSize;
	uint32		    u32MediaObjTime;
} tSinglePayload;

typedef struct tagMultiPayloadHeader
{
	uint8		    u8PayloadFlags;
} tMultiPayloadHeader;


typedef struct tagMultiPayload
{
	uint8		    u8StreamNum;
	uint8		    u8MediaObjNum;
	uint32		    u32OffsetMediaObject;
	uint8		    u8RepDataLen;
	uint32		    u32MediaObjSize;
	uint32		    u32MediaObjTime;
	uint16		    u16PayloadLen;
} tMultiPayload;

int  AsfCreate (DMX_tCtxt *pCtxt, long fileSize, DMX_tFileSummary *pSummary);
int  AsfDestroy (DMX_tCtxt *pCtxt);
void AsfEngine (DMX_tCtxt *pCtxt);
int  AsfParseAll (DMX_tCtxt *pCtxt, uint64 uObjLen);
#endif //_ASF_H_
