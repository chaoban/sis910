#include <fcntl.h>
#include <stdio.h>
#include <cyg/io/file.h>
#include <sys/stat.h>
#include <cyg/io/aud/UB.h>

#include "common.h"
#include "asf.h"

/***************************************************************************************/
#define BUF_SIZE        1024
#define SRC_BUF_SIZE    512*1024

#if SISPKG_IO_AUD_DEC_WMA_DEBUG && 1
int gpASFCtrlCnt[32] = {0};
#define INC_ASF_CTR(xx)         do { gpASFCtrlCnt[xx] ++; } while (0)
#else
#define INC_ASF_CTR(xx)         { }
#endif

#if SISPKG_IO_AUD_DEC_WMA_DEBUG
#define CHK(xx) do {                                                                    \
    if (!(xx)) {                                                                        \
        EPRINTF(("Assert error: %s, file %s, line %d\n", (#xx), __FILE__, __LINE__));   \
    }                                                                                   \
}while (0);

#define PLINE    VPRINTF(("file %s, line %d\n", __FILE__, __LINE__));

#define PRINTF_GUID1(_xx_) GPRINTF(("\t0x%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",   \
            (_xx_).Data1, (_xx_).Data2, (_xx_).Data3,  (_xx_).Data4[0],  (_xx_).Data4[1],       \
            (_xx_).Data4[2],  (_xx_).Data4[3], (_xx_).Data4[4],  (_xx_).Data4[5], (_xx_).Data4[6],  (_xx_).Data4[7]));

#define PRINTF_GUID2(_xx_, len) GPRINTF(("\t0x%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x, %x\n",   \
            (_xx_).Data1, (_xx_).Data2, (_xx_).Data3,  (_xx_).Data4[0],  (_xx_).Data4[1],       \
            (_xx_).Data4[2],  (_xx_).Data4[3], (_xx_).Data4[4],  (_xx_).Data4[5], (_xx_).Data4[6],  (_xx_).Data4[7],    \
            (int)len ))
#else
#define CHK(xx) {}
#define PLINE	{}
#define PRINTF_GUID1(_xx_)	{}
#define PRINTF_GUID2(_xx_, len)	{}
#endif

DEFINE_GUID_ASF(CLSID_CAsfUnknownObject,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);

DEFINE_GUID_ASF(CLSID_CAsfNullClockType,
        0x1d70b760,0x748e,0x11cf,0x9c,0x0f,0x00,0xa0,0xc9,0x03,0x49,0xcb);

DEFINE_GUID_ASF(CLSID_CAsfHeaderObject,
        0x75b22630,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

DEFINE_GUID_ASF(CLSID_CAsfHeaderExtensionObject,
        0x5FBF03B5, 0xA92E, 0x11CF, 0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65);

DEFINE_GUID_ASF(CLSID_CAsfFilePropertie,
        0x8cabdca1, 0xa947, 0x11cf, 0x8e, 0xe4, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

DEFINE_GUID_ASF(CLSID_CAsfStreamPropertiesObject,
        0xb7dc0791, 0xa9b7, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

DEFINE_GUID_ASF(CLSID_CAsfContentDescriptionObject,
        0x75b22633,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

DEFINE_GUID_ASF(CLSID_CAsfMarkerObject,
        0xf487cd01, 0xa951, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

DEFINE_GUID_ASF(CLSID_CAsfSimpleIndexObject,
        0x33000890, 0xe5b1, 0x11cf, 0x89, 0xf4, 0x0, 0xa0, 0xc9, 0x3, 0x49, 0xcb);

DEFINE_GUID_ASF(CLSID_CAsfDataObject,
        0x75b22636,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

DEFINE_GUID_ASF(CLSID_CAsfFramesObjectV0,
        0x75b22637,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

DEFINE_GUID_ASF(CLSID_CAsfClockObjectV0,
        0x5fbf03b5, 0xa92e, 0x11cf, 0x8e, 0xe3, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

DEFINE_GUID_ASF(CLSID_CAsfStreamRoutingObjectV0,
        0x6ba83691, 0xb8d7, 0x11cf, 0x96, 0xc, 0x0, 0xa0, 0xc9, 0xa, 0x8e, 0x34);

// {1EFB1A30-0B62-11d0-A39B-00A0C90348F6}
DEFINE_GUID_ASF(CLSID_CAsfScriptCommandObject,
        0x1efb1a30, 0xb62, 0x11d0, 0xa3, 0x9b, 0x0, 0xa0, 0xc9, 0x3, 0x48, 0xf6);

// {4B1ACBE3-100B-11d0-A39B-00A0C90348F6}
DEFINE_GUID_ASF(CLSID_AsfXScriptCommandIndex,
        0x4b1acbe3,0x100b,0x11d0,0xa3,0x9b,0x0,0xa0,0xc9,0x3,0x48,0xf6);

// {86D15240-311D-11d0-A3A4-00A0C90348F6}
DEFINE_GUID_ASF(CLSID_CAsfCodecListObject,
        0x86d15240, 0x311d, 0x11d0, 0xa3, 0xa4, 0x0, 0xa0, 0xc9, 0x3, 0x48, 0xf6);

//
// ASF Mutual Exclusion Object == {0xD6E229DC-35DA-11D1-9034-00A0C90349BE}
//
DEFINE_GUID_ASF( CLSID_CAsfMutualExclusionObject,
        0xD6E229DC, 0x35DA, 0x11D1, 0x90, 0x34, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xBE );

//
// Same content different bitrates == {0xD6E22A01-35DA-11D1-9034-00A0C90349BE}
//
DEFINE_GUID_ASF( CLSID_SameContentDifferentBitrates,
        0xD6E22A01, 0x35DA, 0x11D1, 0x90, 0x34, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xBE );

//
// ASF Extended Stream Properties Object == {7BF875CE-468D-11d1-8D82-006097C9A2B2}
//
DEFINE_GUID_ASF( CLSID_CAsfStreamBitratePropertiesObject,
        0x7bf875ce, 0x468d, 0x11d1, 0x8d, 0x82, 0x0, 0x60, 0x97, 0xc9, 0xa2, 0xb2 );

DEFINE_GUID_ASF( CLSID_CAsfPaddingObject,
        0x1806D474, 0xCADF, 0x4509, 0xA4, 0xBA, 0x9A, 0xAB, 0xCB, 0x96, 0xAA, 0xE8);

//
// ASF Content Branding Object == {2211B3FA-BD23-11d2-B4B7-00A0C955FC6E}
//
DEFINE_GUID_ASF( CLSID_CAsfContentBrandingObject,
        0x2211b3fa, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x0, 0xa0, 0xc9, 0x55, 0xfc, 0x6e );

//
// ASF Content Encryption Object == {2211B3FB-BD23-11d2-B4B7-00A0C955FC6E}
//
DEFINE_GUID_ASF( CLSID_CAsfContentEncryptionObject,
        0x2211b3fb, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x0, 0xa0, 0xc9, 0x55, 0xfc, 0x6e );

DEFINE_GUID_ASF( CLSID_CAsfContentEncryptionObjectEx,
        0x298ae614, 0x2622, 0x4c17, 0xb9, 0x35, 0xda, 0xe0, 0x7e, 0xe9, 0x28, 0x9c);

//
// ASF Content Encryption Object Ex2 == {FF889EF1-ADEE-40DA-9E71-98704BB928CE}
//
DEFINE_GUID_ASF( CLSID_CAsfContentEncryptionObjectEx2,
        0xff889ef1, 0xadee, 0x40da, 0x9e, 0x71, 0x98, 0x70, 0x4b, 0xb9, 0x28, 0xce);

//
// ASF  Mercury Rep Data
//
DEFINE_GUID_ASF( CLSID_CAsfMercuryRepData,
        0x8484f884, 0x62e6, 0x4073, 0x97, 0x44, 0x28, 0x96, 0x71, 0xce, 0xb0, 0x16);
//
// ASF Digital Signature Object == {2211B3FC-BD23-11d2-B4B7-00A0C955FC6E}
//
DEFINE_GUID_ASF( CLSID_CAsfDigitalSignatureObject,
        0x2211b3fc, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x0, 0xa0, 0xc9, 0x55, 0xfc, 0x6e );

//
// ASF Extended Content Desc Object == {D2D0A440-E307-11d2-97F0-00A0C95EA850}
//
DEFINE_GUID_ASF( CLSID_CAsfExtendedContentDescObject,
        0xd2d0a440, 0xe307, 0x11d2, 0x97, 0xf0, 0x0, 0xa0, 0xc9, 0x5e, 0xa8, 0x50);

// Clock types

// ASF_RESERVED_1, same as CLSID_CAsfPacketClock1
// {ABD3D211-A9BA-11cf-8EE6-00C00C205365}
DEFINE_GUID_ASF(CLSID_CAsfReserved1,
            0xabd3d211, 0xa9ba, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

// {ABD3D211-A9BA-11cf-8EE6-00C00C205365}
DEFINE_GUID_ASF(CLSID_CAsfPacketClock1,
        0xabd3d211, 0xa9ba, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

// {ABD3D213-A9BA-11cf-8EE6-00C00C205365}
DEFINE_GUID_ASF(CLSID_CAsfPacketClock2,
        0xabd3d213, 0xa9ba, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

// {ABD3D214-A9BA-11cf-8EE6-00C00C205365}
DEFINE_GUID_ASF(CLSID_CAsfPacketClock3,
        0xabd3d214, 0xa9ba, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

// marker support
//
// {4CFEDB20-75F6-11cf-9C0F-00A0C90349CB}
DEFINE_GUID_ASF( CLSID_AsfXMarkerIndex,
        0x4cfedb20, 0x75f6, 0x11cf, 0x9c, 0xf, 0x0, 0xa0, 0xc9, 0x3, 0x49, 0xcb);

DEFINE_GUID_ASF( CLSID_CAsfLicenseStoreObject,
        0x8fa7857b, 0xddc0, 0x11d3, 0xb6, 0xbd, 0x0, 0xc0, 0x4f, 0x61, 0xd, 0x62);


DEFINE_GUID_ASF(CLSID_AsfXNPlus1Parity,
        0x541d0f50,0x5b4b,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_CAsfIndexObjectV2New,
        0xd6e229d3, 0x35da, 0x11d1, 0x90, 0x34, 0x0, 0xa0, 0xc9, 0x3, 0x49, 0xbe);

/*
 * ======================================================================
 * A Null error masking strategy for use until we get smarter.
 * ======================================================================
 */

DEFINE_GUID_ASF(CLSID_AsfXNullErrorMaskingStrategy,
        0x20fb5700,0x5b55,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_AsfXAcmAudioErrorMaskingStrategy,
        0xbfc3cd50,0x618f,0x11cf,0x8b,0xb2,0x00,0xaa,0x00,0xb4,0xe2,0x20);

// {49F1A440-4ECE-11d0-A3AC-00A0C90348F6}
DEFINE_GUID_ASF(CLSID_AsfXSignatureAudioErrorMaskingStrategy,
        0x49f1a440, 0x4ece, 0x11d0, 0xa3, 0xac, 0x0, 0xa0, 0xc9, 0x3, 0x48, 0xf6);

DEFINE_GUID_ASF(CLSID_AsfXStreamTypeAcmAudio,
        0xf8699e40,0x5b4d,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_AsfXStreamTypeUncompressedDibVideo,
        0x3725b2e0,0x5b4e,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_AsfXStreamTypeIcmVideo,
        0xbc19efc0,0x5b4d,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_AsfXStreamTypeJpegJfifVideo,
        0xb61be100,0x5b4e,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_AsfXStreamTypeAnsiUrl,
        0x14082850,0x5b4f,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

DEFINE_GUID_ASF(CLSID_AsfXStreamTypeDegradableJpeg,
        0x35907de0,0xe415,0x11cf,0xa9,0x17,0x00,0x80,0x5f,0x5c,0x44,0x2b);

// {59DACFC0-59E6-11d0-A3AC-00A0C90348F6}
DEFINE_GUID_ASF(CLSID_AsfXStreamTypeScriptCommand,
        0x59dacfc0, 0x59e6, 0x11d0, 0xa3, 0xac, 0x0, 0xa0, 0xc9, 0x3, 0x48, 0xf6);


// {86D15241-311D-11d0-A3A4-00A0C90348F6}
DEFINE_GUID_ASF(CLSID_AsfXCodecList,
        0x86d15241, 0x311d, 0x11d0, 0xa3, 0xa4, 0x0, 0xa0, 0xc9, 0x3, 0x48, 0xf6);

//
// Good old SCR and duration clock license
//
// {97F6F860-747E-11cf-9C0F-00A0C90349CB}
DEFINE_GUID_ASF( CLSID_AsfXPacketClock,
        0x97f6f860, 0x747e, 0x11cf, 0x9c, 0xf, 0x0, 0xa0, 0xc9, 0x3, 0x49, 0xcb );

//DEFINE_GUID_ASF( CLSID_AsfXMetaDataObject, 0xa69609e7, 0x517b, 0x11d2, 0xb6, 0xaf, 0x00, 0xc0, 0x4f, 0xd9, 0x08, 0xe9 );

DEFINE_GUID_ASF( CLSID_AsfXMetaDataObject,
        0xC5F8CBEA, 0x5BAF, 0x4877, 0x84, 0x67, 0xAA, 0x8C, 0x44, 0xFA, 0x4C, 0xCA );

DEFINE_GUID_ASF( CLSID_CAsfStreamPropertiesObjectEx,
        0x14e6a5cb, 0xc672, 0x4332, 0x83, 0x99, 0xa9, 0x69, 0x52, 0x6, 0x5b, 0x5a);

/// for bit opt
uchar* gpBitBuff; //dst buff for ASF DMX; src buff for WMA Decoder
int bitBuffSetIndex = 0; //for bb_set*() and bb_memcpy(), bytes
int bitBuffGetIndex = 0; // for ExtractBits() and PeekBits(), bits
////////////////////////////////////

static inline uint8 abv_get8(void)
{
	uchar dst;
	ABV_read(sizeof(uchar), &dst);
	return (uint8)dst;
}

#define abv_getle16() (uint16) (abv_get8() | (abv_get8()<<8))
#define abv_getle32() (uint32) (abv_getle16() | (abv_getle16()<<16) )
#define abv_getle64() (uint64) ((uint64)getle32() | ((uint64)abv_getle32()<<32) )

#define abv_getbe16() (uint16) ( (abv_get8()<<8) | (abv_get8()) )
#define abv_getbe32() (uint32) ( (abv_getbe16()<<16) | (abv_getbe16()) )
#define abv_getbe64() (uint64) ( ((uint64)abv_getbe32()<<32) | ((uint64)abv_getbe32()) )

#define abv_getGUID(a) do {  \
	int sss; \
	a.Data1 = abv_getle32(); \
	a.Data2 = abv_getle16(); \
	a.Data3 = abv_getle16(); \
	for (sss=0; sss<8; sss++) { \
		a.Data4[sss] = abv_get8(); \
	} \
} while (0)

#define get8(_xx_)  (uint8)  (*(uint8*)(_xx_))
#define getle16(_xx_) (uint16) (get8(_xx_) | (get8((int)(_xx_)+1)<<8))
#define getle32(_xx_) (uint32) (getle16(_xx_) | (getle16((int)(_xx_)+2)<<16) )
#define getle64(_xx_) (uint64) ((uint64)getle32(_xx_) | ((uint64)getle32((int)(_xx_)+4)<<32) )

#define getbe16(_xx_) (uint16) ( (get8((int)(_xx_))<<8) | (get8((int)(_xx_)+1)) )
#define getbe32(_xx_) (uint32) ( (getbe16(_xx_)<<16) | (getbe16((int)(_xx_)+2)) )
#define getbe64(_xx_) (uint64) ( ((uint64)getbe32(_xx_)<<32) | ((uint64)getbe32((int)(_xx_)+4)) )

#define getGUID(p, a) do {              \
	int sss;                            \
	a.Data1 = getle32(p);               \
	a.Data2 = getle16((int)p+4);        \
	a.Data3 = getle16((int)p+6);        \
	for (sss=0; sss<8; sss++) {         \
	    a.Data4[sss] = get8((int)p + 8 + sss);  \
	}                                   \
} while (0)

#define GUIDINFO(a) &(CLSID_##a), #a

#define SAFE_FREE(p) do{if (p) {free(p); p = 0;}} while (0);

enum {
	STATE_IDLE      = 0,
	STATE_ACTIVE    = 1,
	STATE_FLUSH     = 2
};

typedef struct TAGGUIDParseFunc
{
	const tGUID *pGUID;
	const int8 *strGUID;
	int (*parseFunc) (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
} tGUIDParseFunc;

typedef struct tSrcBuf {
	int             FId;            ///< File ID
	int             BufSize;
	uchar           *pBuf;          ///< Buffer

	//UBuff           UB;             ///< UB control block

	int             Ch;             ///< Source DMA channel
} tSrcBuf;

typedef struct tDstCh {
	//UDMA_ch         *pCh;

	int             opened;
} tDstCh;

typedef struct tagAsfPriv {
	int32               state;

	int32			    iAudioStreamID;
	int32				iVideoStreamID;

	tHeaderObj			headerObj;
	tFileProp_ASF		fileProp;
	tHeaderExt_ASF		headerExt;
	tStreamProp_ASF		streamProp[TOTAL_MEDIA_TYPE];
	tASFCodecEntry		codecEntry[TOTAL_MEDIA_TYPE];
	tContentDescription	contentDesc;
	tStreamBitrateProp		bitrateProp;
	tSimpleIndexObject	simpleIndex;
	tMarkerEntry        markerEntry;
	tASFMetaDataEntry   metaData;

	uint8               numOfStream;
	uint8               hasAudio;
	uint8               hasVideo;

	tAudioInfo			audioInfo;
	tVideoInfo			videoInfo;

	// codec info
	uint8				*pbAudioSpecificData;// no need to free, point to somewhere pStreamProp
	uint32				uAudioSpecificDataLen;
	uint32				uAudioInfoLen;
	uint8				*pbVideoSpecificData;// no need to free, point to somewhere pStreamProp
	uint32				uVideoSpecificDataLen;
	uint32				uVideoInfoLen;

	// parse packet
	tPacketParser		packetParser;

	uint32				uAudioNum;          //
	uint32				uVideoNum;          // frame num
	uint32				uTimeStampBase;		// 1st frame time stamp

	uint32				uVideoIndexCnt;
	uint32				uLastKeyPacket;
	uint32				uLastKeyPackNum;
	uint32				uLastKeyTimeStamp;

	int                 Fid;
	//	FILE_INFO           *pFileInfo;
	own uint8           *pBuf;

	tSrcBuf             srcBuf;
	tDstCh              dstCh[4];
} tAsfPriv;
/*
static int  AsfCreate (DMX_tCtxt *pCtxt, long fileSize, DMX_tFileSummary *pSummary);
static int  AsfDestroy (DMX_tCtxt *pCtxt);
static void AsfEngine (DMX_tCtxt *pCtxt);

static int  AsfParseAll (DMX_tCtxt *pCtxt, uint64 uObjLen);
*/
static int  AsfParseHeaderObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseHeaderExtensionObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseFilePropObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseStreamPropObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseContentDescObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseExtendedContentDescObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseScriptCommandObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseMarkerObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseCodecListObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseStreamBitratePropObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParsePaddingObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseDataObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseSimpleIndexObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);
static int  AsfParseUnknownObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen);

static int  AsfParseOnePacket(DMX_tCtxt *pCtxt);
static int  AsfOutputHeader (DMX_tCtxt *pCtxt);
static int  AsfOutputData (DMX_tCtxt *pCtxt, tASFPayLoadEntry *pPayLoad );

static const tGUIDParseFunc guidPB[] =
{
	// header
	{GUIDINFO(CAsfHeaderObject),                AsfParseHeaderObject},
	{GUIDINFO(CAsfHeaderExtensionObject),       AsfParseHeaderExtensionObject},
	{GUIDINFO(CAsfFilePropertie),               AsfParseFilePropObject},
	{GUIDINFO(CAsfStreamPropertiesObject),      AsfParseStreamPropObject},
	{GUIDINFO(CAsfContentDescriptionObject),    AsfParseContentDescObject},
	{GUIDINFO(CAsfExtendedContentDescObject),   AsfParseExtendedContentDescObject},
	{GUIDINFO(CAsfScriptCommandObject),         AsfParseScriptCommandObject},
	{GUIDINFO(CAsfMarkerObject),                AsfParseMarkerObject},
	{GUIDINFO(CAsfCodecListObject),             AsfParseCodecListObject},
	{GUIDINFO(CAsfStreamBitratePropertiesObject),   AsfParseStreamBitratePropObject},
	{GUIDINFO(CAsfPaddingObject),               AsfParsePaddingObject},

	// data
	{GUIDINFO(CAsfDataObject),                  AsfParseDataObject},
	// simple index
	{GUIDINFO(CAsfSimpleIndexObject),           AsfParseSimpleIndexObject},
	// unknow object
	{GUIDINFO(CAsfUnknownObject),               AsfParseUnknownObject},
};

static int firstTimeEngine = 1; // for AsfEngine to out Asf Header data
int  AsfCreate (DMX_tCtxt *pCtxt, long fileSize, DMX_tFileSummary *pSummary)
{
	int Status = OKAY, iRet, i;
	tAsfPriv *pPriv = 0;

	// data structure
	CHK(sizeof(uint16) == 2 );
	CHK(sizeof(wchar)  == 2 );
	CHK(sizeof(uint32) == 4 );
	CHK(sizeof(uint64) == 8 );
	CHK(sizeof(tGUID)  == 16);

	firstTimeEngine = 1;
	pCtxt->pPriv = pPriv = (tAsfPriv*) malloc (sizeof(tAsfPriv));
	CHK(pCtxt->pPriv);
	memset (pPriv, 0, sizeof(tAsfPriv));

	pPriv->pBuf = (uchar *)malloc(BUF_SIZE); CHK(pPriv->pBuf); VPRINTF (("pBuf at %x\n", (int) pPriv->pBuf));

	iRet = AsfParseAll (pCtxt, (uint64) fileSize);

	if (iRet != OKAY ) {
		assert (0);
		goto AsfCreate_END;
	}

	pSummary->StartPts = 0;
	pSummary->NumStream = pPriv->numOfStream;   CHK(pPriv->numOfStream>=1 && pPriv->numOfStream<=2);
	i = 0;

	/// FIXME mimetype
	if ( pPriv->hasAudio) {
		pSummary->pStreamInfoTbl[i].SSNum = i;
		pSummary->pStreamInfoTbl[i].Type = DMX_STREAM_TYPE_AUDIO;

		switch (pPriv->audioInfo.u16FormatTag)
		{
			case WAVE_FORMAT_WMA_STD:
				pSummary->pStreamInfoTbl[i].mime_type = MIMETYPE_WMA;
			break;

			/// FIXME no support yet
			case WAVE_FORMAT_WMA_PRO:
			case WAVE_FORMAT_WMA_LLS:
			case WAVE_FORMAT_WMA_VOICE:
			default:
				pSummary->pStreamInfoTbl[i].mime_type = MIMETYPE_UNKNOWN;
				assert (0);
			break;
		}
		i ++;
	}
	
	if (pPriv->hasVideo) {
		assert(0); // now only for audio 
		/*
		pSummary->pStreamInfoTbl[i].SSNum = i;
		pSummary->pStreamInfoTbl[i].Type = DMX_STREAM_TYPE_VIDEO;
		pSummary->pStreamInfoTbl[i].mime_type = mimetypeFromFourCC (pPriv->videoInfo.biCompression);
		i ++;
		*/
	}
	
	CHK(i == pSummary->NumStream);

	/// new gpBitBuff for dump header and packet data, now only for WMA audio.
	i = pPriv->fileProp.u32MaxPackSize * 2 + pPriv->uAudioSpecificDataLen + 18;
	gpBitBuff = (uchar*)malloc(i);
	memset(gpBitBuff, 0, i); 
	///////////////////////////////////////////

	return OKAY;

AsfCreate_END:
	/// FIXME cleanup
	Status = ERROR;
	return Status;
}

int  AsfDestroy (DMX_tCtxt *pCtxt)
{
    tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
    int32 i;

    for (i=0; i<TOTAL_MEDIA_TYPE; i++) {
        SAFE_FREE (pPriv->streamProp[i].pTypeData);
        SAFE_FREE (pPriv->streamProp[i].pErrCorrectionData);

        SAFE_FREE (pPriv->codecEntry[i].pbCodecName);
        SAFE_FREE (pPriv->codecEntry[i].pbCodecDescription);
        SAFE_FREE (pPriv->codecEntry[i].pbCodecInfo);
    }
    SAFE_FREE (pPriv->contentDesc.pwTitle);
    SAFE_FREE (pPriv->contentDesc.pwAuthor);
    SAFE_FREE (pPriv->contentDesc.pwCopyright);
    SAFE_FREE (pPriv->contentDesc.pwDescription);
    SAFE_FREE (pPriv->contentDesc.pwRating);

    SAFE_FREE (pPriv->bitrateProp.pRecord);
    
    SAFE_FREE (pPriv->markerEntry.m_pwDescName);

    SAFE_FREE (pPriv->simpleIndex.pEntry);

    SAFE_FREE (pPriv->metaData.pDescRec);
    
    SAFE_FREE (pPriv->pBuf);
    SAFE_FREE (pPriv);
    return OKAY;
}

void AsfEngine (DMX_tCtxt *pCtxt)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
//	tSrcBuf *pSrcBuf = &pPriv->srcBuf;
	//struct DMX_tFileSummary *pSummary = &pCtxt->FileSummary;

	//VPRINTF (("engine\n"));
//	if (pPriv->state == STATE_IDLE)
//		return;

	//VPRINTF (("fullness %x, packsize %x\n", pPriv->srcBuf.UB.fullness, pPriv->fileProp.u32MaxPackSize));
	if (firstTimeEngine) 
	{
		bb_reset();// reset gpBitBuff;
		VPRINTF (("ASF dmx engine\n"));
		firstTimeEngine = 0;
		AsfOutputHeader (pCtxt);
	}
	else
	{
		AsfParseOnePacket (pCtxt);
	}

	//VPRINTF (("exit engine\n"));
	return;
}

int  AsfParseAll(DMX_tCtxt *pCtxt, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	uint64 parseOffset=0;

	while (parseOffset < uObjLen) 
	{
		tGUID objID;
		uint64 objSize;
//		int64 iOffset;
		int32 i=0, iUnknown=1, iRet;

		iRet = ABV_read(sizeof(tGUID) + sizeof(int64), pPriv->pBuf);
		getGUID(pPriv->pBuf, objID);
		objSize = getle64(pPriv->pBuf + sizeof(tGUID));
		PRINTF_GUID2 (objID, objSize);

		for (i = 0; i < sizeof(guidPB) / sizeof(tGUIDParseFunc) - 1; i++) 
		{
			if( IsEqualGUID_ASF(&objID, guidPB[i].pGUID)) 
			{
				VPRINTF (("--- match %s ---\n", guidPB[i].strGUID));
				guidPB[i].parseFunc(pCtxt, objID, objSize);
				iUnknown = 0;
				break;
			}
		}

		////we need break the while() because of abv_ without seek() opt
		if(IsEqualGUID_ASF(&objID, &CLSID_CAsfDataObject)) 
			break;
		////////////////////////////////////

		if(iUnknown) 
		{
			AsfParseUnknownObject(pCtxt, objID, objSize);
		}
	}//while (parseOffset < uObjLen) 
	VPRINTF (("parsing end\n"));

	return OKAY;
}

static int  AsfParseHeaderObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	tHeaderObj *pHeader;
//	int64 iStartOffset, parseOffset;
	int iRet;
	uint8 *p = pPriv->pBuf;
	int headSize = /*16+8+*/4+2;

	pHeader = &pPriv->headerObj;

	//iStartOffset = parseOffset =  FS_tell (pPriv->Fid);
	iRet = ABV_read(headSize, pPriv->pBuf);

	//getGUID(pPriv->pBuf, pHeader->gObjID); p += sizeof(tGUID);
	//pHeader->u64ObjSize = getle64(p); p += sizeof(uint64);
	pHeader->gObjID = objId;
	pHeader->u64ObjSize = uObjLen;
	PRINTF_GUID2 (pHeader->gObjID, pHeader->u64ObjSize);

	CHK( IsEqualGUID_ASF (&pHeader->gObjID, &CLSID_CAsfHeaderObject) );
	//CHK( pHeader->u64ObjSize == uObjLen);

	pHeader->u32NumObj = getle32(p); p += sizeof(uint32);
	pHeader->u8Reserved1 = get8(p); p += sizeof(uint8);
	pHeader->u8Reserved2 = get8(p); p += sizeof(uint8);

	VPRINTF (("numofobj %d, resv1 %d resv2 %d\n", pHeader->u32NumObj, pHeader->u8Reserved1, pHeader->u8Reserved2));

	//parseOffset =  FS_tell (pPriv->Fid);
	while(pHeader->u32NumObj)
	{
		tGUID objID;
		uint64 objSize;
		int32 i=0, iUnknown=1;

		//INC_ASF_CTR (2);
		p = pPriv->pBuf;
		iRet = ABV_read(sizeof(tGUID) + sizeof(uint64), pPriv->pBuf);

		getGUID(pPriv->pBuf, objID); p += sizeof(tGUID);
		objSize = getle64(p); p += sizeof(uint64);

		//iRet = FS_seek (pPriv->Fid, FILE_SEEK_SET,  parseOffset);

		for(i=0; i < sizeof(guidPB) / sizeof(tGUIDParseFunc) - 1; i++) 
		{
			if(IsEqualGUID_ASF( &objID, guidPB[i].pGUID)) 
			{
				VPRINTF (("--- match %s --- \n", guidPB[i].strGUID));
				guidPB[i].parseFunc(pCtxt, objID, objSize);
				iUnknown=0;
				break;
			}
		}

		if(iUnknown )
		{
			AsfParseUnknownObject(pCtxt, objID, objSize);
		}

		//parseOffset += objSize;
		//CHK(parseOffset == FS_tell (pPriv->Fid));
		pHeader->u32NumObj--;
	}//while(pHeader->u32NumObj)

	//CHK( FS_tell (pPriv->Fid) == (int64)uObjLen + iStartOffset);

	return OKAY;
}

static int  AsfParseHeaderExtensionObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	tHeaderExt_ASF *pHeaderExt = &pPriv->headerExt;
	uint8 *p = pPriv->pBuf;
	uint32 size_of_headerExt = /*sizeof(tGUID) + sizeof(uint64) + */sizeof(tGUID) + sizeof(uint16) + sizeof(uint32);

	VPRINTF(("header extentention, preliminary support\n"));
	ABV_read(size_of_headerExt, pPriv->pBuf);

	//getGUID(p, pHeaderExt->gObjID);            p += sizeof(tGUID);
	//pHeaderExt->u64ObjSize = getle64(p);       p += sizeof(uint64);
	pHeaderExt->gObjID = objId;
	pHeaderExt->u64ObjSize = uObjLen;

	CHK( IsEqualGUID_ASF (&pHeaderExt->gObjID, &CLSID_CAsfHeaderExtensionObject) );
	//CHK(pHeaderExt->u64ObjSize == uObjLen);
	PRINTF_GUID2 (objId, uObjLen);

	getGUID(p, pHeaderExt->gReserved); p += sizeof(tGUID);
	pHeaderExt->uReserved = getle16(p); p += sizeof(uint16);
	CHK(IsEqualGUID_ASF(&pHeaderExt->gReserved, &CLSID_CAsfReserved1) );
	CHK(pHeaderExt->uReserved == 6);

	pHeaderExt->uDataSize = getle32(p); p += sizeof(uint32);

	//CHK(size_of_headerExt + pHeaderExt->uDataSize == uObjLen);
	//FS_seek (pPriv->Fid, FILE_SEEK_CUR, (int64)uObjLen - size_of_headerExt);
	ABV_skip((int64)uObjLen - size_of_headerExt - sizeof(tGUID) - sizeof(uint64), SEEK_CUR);
	return OKAY;
}

static int  AsfParseFilePropObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tFileProp_ASF *pFileProp ;
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
//	int64 iStartOffset;
	int32 headSize = sizeof(tFileProp_ASF) - sizeof(tGUID) - sizeof(uint64), iRet;
	uint8 *p=pPriv->pBuf;

//	iStartOffset = FS_tell (pPriv->Fid);

	pFileProp = &pPriv->fileProp;
	iRet = ABV_read(headSize, pPriv->pBuf);

	//getGUID(p, pFileProp->gObjID);         p += sizeof(tGUID);
	//pFileProp->u64ObjSize = getle64(p);    p += sizeof(uint64);
	pFileProp->gObjID = objId;
	pFileProp->u64ObjSize = uObjLen;
	PRINTF_GUID2 (objId, uObjLen);

	CHK( IsEqualGUID_ASF (&pFileProp->gObjID, &CLSID_CAsfFilePropertie) );
	//CHK( pFileProp->u64ObjSize == uObjLen);

	getGUID(p, pFileProp->gFileID); p += sizeof(tGUID);
	pFileProp->u64FileSize = getle64(p); p += sizeof(uint64);

	pFileProp->u64CreateDate = getle64(p); p += sizeof(uint64);
	pFileProp->u64PacketCount = getle64(p); p += sizeof(uint64);
	pFileProp->u64PlayDuration = getle64(p); p += sizeof(uint64);
	pFileProp->u64SendDuration = getle64(p); p += sizeof(uint64);
	pFileProp->u64Preroll = getle64(p); p += sizeof(uint64);
	pFileProp->u32Flags = getle32(p); p += sizeof(uint32);
	pFileProp->u32MinPackSize = getle32(p); p += sizeof(uint32);
	pFileProp->u32MaxPackSize = getle32(p); p += sizeof(uint32);
	pFileProp->u32MaxBitrate = getle32(p); p += sizeof(uint32);

	VPRINTF (("packount = %d, maxbitrate = %d\n", (int) pFileProp->u64PacketCount, (int) pFileProp->u32MaxBitrate));
	CHK(pFileProp->u32MinPackSize == pFileProp->u32MaxPackSize);
	//CHK( FS_tell (pPriv->Fid) == iStartOffset + uObjLen);

	return OKAY;
}

static int  AsfParseStreamPropObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	tStreamProp_ASF stStreamProp, *pStreamProp;
//	int64 iStartOffset;
	int iRet;
	uint8 *p= pPriv->pBuf;
	EMediaType_ASF eIndex;

	CHK(uObjLen <= BUF_SIZE);

	//iStartOffset = FS_tell (pPriv->Fid);
	iRet = ABV_read(uObjLen - sizeof(tGUID) - sizeof(uint64), pPriv->pBuf);

	//getGUID(p, stStreamProp.gObjID);           p += sizeof(tGUID);
	//stStreamProp.u64ObjSize = getle64(p);      p += sizeof(uint64);
	stStreamProp.gObjID = objId;
	stStreamProp.u64ObjSize = uObjLen;
	PRINTF_GUID2(stStreamProp.gObjID, stStreamProp.u64ObjSize);

	CHK(IsEqualGUID_ASF(&stStreamProp.gObjID, &CLSID_CAsfStreamPropertiesObject) );
	//CHK( stStreamProp.u64ObjSize == uObjLen);

	getGUID(p, stStreamProp.gStreamType); p += sizeof(tGUID);
	getGUID(p, stStreamProp.gErrCorrectionType); p += sizeof(tGUID);

	stStreamProp.u64TimeOffset = getle64(p); p += sizeof(uint64);
	stStreamProp.u32TypeDataLength = getle32(p); p += sizeof(uint32);
	stStreamProp.u32ErrCorrectionLength = getle32(p); p += sizeof(uint32);
	stStreamProp.u16Flags = getle16(p); p += sizeof(uint16);
	stStreamProp.u32Reserved = getle32(p); p += sizeof(uint32);       //CHK( stStreamProp.u32Reserved == 0);

	if(IsEqualGUID_ASF(&stStreamProp.gStreamType, &CLSID_AsfXStreamTypeAcmAudio)) 
	{
		VPRINTF (("audio stream prop\n"));
		pPriv->iAudioStreamID = stStreamProp.u16Flags & ((1<<7)-1);
		eIndex = Audio_ASF;

		pPriv->hasAudio = 1;
	}
	else if(IsEqualGUID_ASF (&stStreamProp.gStreamType, &CLSID_AsfXStreamTypeIcmVideo)) 
	{
		VPRINTF (("video stream prop\n"));
		pPriv->iVideoStreamID = stStreamProp.u16Flags & ((1<<7)-1);
		eIndex = Video_ASF;

		pPriv->hasVideo = 1;
	}
	else {
		CHK(0);
		return ERROR;
	}

	pPriv->streamProp[eIndex] = stStreamProp;
	pStreamProp = &pPriv->streamProp[eIndex];

	pStreamProp->uEncryptFlag = pStreamProp->u16Flags&0x80;
	pStreamProp->uStreamNum = pStreamProp->u16Flags&0x7f;

	VPRINTF(("encrypt = %d, stream num = %d\n", pStreamProp->uEncryptFlag, pStreamProp->uStreamNum));

	pStreamProp->pTypeData = malloc((int32)pStreamProp->u32TypeDataLength); CHK(pStreamProp->pTypeData);
	pStreamProp->pErrCorrectionData = malloc((int32)pStreamProp->u32ErrCorrectionLength);

	memcpy(pStreamProp->pTypeData, p, pStreamProp->u32TypeDataLength); p += pStreamProp->u32TypeDataLength;
	memcpy(pStreamProp->pErrCorrectionData, p, pStreamProp->u32ErrCorrectionLength); p += pStreamProp->u32ErrCorrectionLength;

	uint8 *p1;
	if(IsEqualGUID_ASF(&pStreamProp->gStreamType, &CLSID_AsfXStreamTypeAcmAudio)) 
	{
		// audio
		tAudioInfo *pAud = &pPriv->audioInfo;

		p1 = pStreamProp->pTypeData;
		pAud->u16FormatTag = getle16(p1); p1 += sizeof(uint16);
		pAud->u16NumChannels = getle16(p1); p1 += sizeof(uint16);
		pAud->u32SamplesPerSecond = getle32(p1); p1 += sizeof(uint32);
		pAud->u32BytesPerSecond = getle32(p1); p1 += sizeof(uint32);
		pAud->u16BlockAlign = getle16(p1); p1 += sizeof(uint16);
		pAud->u16BitsPerSample = getle16(p1); p1 += sizeof(uint16);
		pAud->u16CodecSpecifDataLen = getle16(p1); p1 += sizeof(uint16);
		CHK(p1 == pStreamProp->pTypeData + 18);
		pPriv->uAudioSpecificDataLen = pAud->u16CodecSpecifDataLen;
		pPriv->pbAudioSpecificData = pStreamProp->pTypeData + 18;
#if 0
		VPRINTF (("Format Tage = %d\n", pAud->u16FormatTag));
		VPRINTF (("chnl = %d\n", pAud->u16NumChannels));
		VPRINTF (("freq = %d\n", pAud->u32SamplesPerSecond));
		VPRINTF (("byte rate = %d\n", pAud->u32BytesPerSecond));
		VPRINTF (("block = %d\n", pAud->u16BlockAlign));
		VPRINTF (("u16BitsPerSample = %d\n", pAud->u16BitsPerSample));
		VPRINTF (("u16CodecSpecifDataLen = %d\n", pAud->u16CodecSpecifDataLen));

		CHK(0);
#endif
	}
	else if(IsEqualGUID_ASF (&pStreamProp->gStreamType, &CLSID_AsfXStreamTypeIcmVideo)) 
	{
		// video
		tVideoInfo *pVid = &pPriv->videoInfo;
		int size_of_vid_info = 51;  // sizoef (tVideoInfo) if packed

		p1 = pStreamProp->pTypeData;

		pVid->u32Width = getle32(p1); p1 += sizeof(uint32);
		pVid->u32Height = getle32(p1); p1 += sizeof(uint32);
		pVid->u8Reserved = get8(p1); p1 += sizeof(uint8); CHK(pVid->u8Reserved == 2);
		pVid->u16FormatDataSize = getle16(p1); p1 += sizeof(uint16);
		pVid->biSize = getle32(p1); p1 += sizeof(uint32);
		pVid->biWidth = getle32(p1); p1 += sizeof(uint32);
		pVid->biHeight = getle32(p1); p1 += sizeof(uint32);
		pVid->biPlanes = getle16(p1); p1 += sizeof(uint16);
		pVid->biBitCount = getle16(p1); p1 += sizeof(uint16);
		pVid->biCompression = getle32(p1); p1 += sizeof(uint32);
		pVid->biSizeImage = getle32(p1); p1 += sizeof(uint32);
		pVid->biXPelsPerMeter = getle32(p1); p1 += sizeof(uint32);
		pVid->biYPelsPerMeter = getle32(p1); p1 += sizeof(uint32);
		pVid->biClrUsed = getle32(p1); p1 += sizeof(uint32);
		pVid->biClrImportant = getle32(p1); p1 += sizeof(uint32);

		CHK(p1-pStreamProp->pTypeData == size_of_vid_info);

		pPriv->uVideoInfoLen = pStreamProp->u32TypeDataLength;

		pPriv->pbVideoSpecificData = (pStreamProp->pTypeData) + size_of_vid_info;
		pPriv->uVideoSpecificDataLen = pStreamProp->u32TypeDataLength- size_of_vid_info;

		VPRINTF (("video width %d height %d compression %x spec_data_len %d\n", pVid->biWidth, pVid->biHeight, pVid->biCompression, pPriv->uVideoSpecificDataLen));
#if 0
		VPRINTF (("pPriv->pbVideoSpecificData %x\n", pPriv->pbVideoSpecificData));
		CHK(0);
#endif
	}
	else {
		CHK(0);
		return ERROR;
	}

	pPriv->numOfStream ++;
	CHK(p-pPriv->pBuf == uObjLen - sizeof(tGUID) - sizeof(uint64))

	return OKAY;
}

static int  AsfParseContentDescObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	tContentDescription *pContentDesc = &pPriv->contentDesc;
//	int64 iStartOffset;
	int iRet;
	uint8 *p= pPriv->pBuf;
	uint32 size_of_content_desc = /*sizeof(tGUID) + sizeof(uint64) + */sizeof(uint16) * 5;

	//iStartOffset = FS_tell (pPriv->Fid);
	iRet = ABV_read(size_of_content_desc, pPriv->pBuf);

	//getGUID(p, pContentDesc->gObjID); p += sizeof(tGUID);
	//pContentDesc->u64ObjSize = getle64(p); p += sizeof(uint64);
	pContentDesc->gObjID = objId;
	pContentDesc->u64ObjSize = uObjLen;	
	PRINTF_GUID2 (objId, uObjLen);
	CHK(IsEqualGUID_ASF(&pContentDesc->gObjID, &CLSID_CAsfContentDescriptionObject));
	//CHK(pContentDesc->u64ObjSize == uObjLen);

	pContentDesc->uTitleLen = getle16(p); p += sizeof(uint16);
	pContentDesc->uAuthorLen = getle16(p); p += sizeof(uint16);
	pContentDesc->uCopyrightLen = getle16(p); p += sizeof(uint16);
	pContentDesc->uDescriptionLen = getle16(p); p += sizeof(uint16);
	pContentDesc->uRatingLen = getle16(p); p += sizeof(uint16);

	CHK( size_of_content_desc + pContentDesc->uTitleLen+pContentDesc->uAuthorLen+pContentDesc->uCopyrightLen+pContentDesc->uDescriptionLen+ pContentDesc->uRatingLen == uObjLen - sizeof(tGUID) - sizeof(uint64));

	//FS_seek ( pPriv->Fid,  FILE_SEEK_CUR, (int64)uObjLen - size_of_content_desc);
	ABV_skip((int64)uObjLen - size_of_content_desc - 16 - 8, SEEK_CUR);

	return OKAY;
}

static int  AsfParseExtendedContentDescObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
//	tAsfPriv *pPriv = (tAsfPriv*)pCtxt->pPriv;
//	int64 iStartOffset;
//	int iRet;
//	tGUID objID;
	uint64 objSize;
//	uint8 *p= pPriv->pBuf;

	//iStartOffset = FS_tell (pPriv->Fid);
	//iRet = ABV_read(sizeof(tGUID) + sizeof(uint64), pPriv->pBuf);

	//getGUID(p, objID);             p += sizeof(tGUID);
	//objSize = getle64(p);          p += sizeof(uint64);
	PRINTF_GUID2 (objId, uObjLen);

	CHK(IsEqualGUID_ASF(&objId, &CLSID_CAsfExtendedContentDescObject));
	//CHK(objSize == uObjLen);

	//FS_seek (pPriv->Fid, FILE_SEEK_CUR, (int64)uObjLen - (sizeof(tGUID) + sizeof(uint64)));
	ABV_skip((int64)uObjLen - sizeof(tGUID) - sizeof(uint64), SEEK_CUR);
	return OKAY;
}

static int  AsfParseScriptCommandObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	AsfParseUnknownObject (pCtxt, objId, uObjLen);
	assert (0);
	return OKAY;
}

static int  AsfParseMarkerObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	AsfParseUnknownObject (pCtxt, objId, uObjLen);
	assert (0);
	return OKAY;
}

static int  AsfParseCodecListObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
//	int64 iStartOffset;
	EMediaType_ASF eIndex = 0;
	uint32 uCodecCount;
	int iRet;
	tGUID  objID;
	uint64 objSize;
	uint8 *p = pPriv->pBuf;

	//iStartOffset = FS_tell (pPriv->Fid);

	CHK(uObjLen <= BUF_SIZE);
	iRet = ABV_read(uObjLen - sizeof(tGUID) - sizeof(uint64), pPriv->pBuf);

	//getGUID(p, objID) ;             p += sizeof(tGUID);
	//objSize = getle64(p);          p += sizeof(uint64);
	objID = objId;
	objSize = uObjLen;
	PRINTF_GUID2 (objId, uObjLen);

	CHK(IsEqualGUID_ASF(&objID, &CLSID_CAsfCodecListObject) );
	//CHK(objSize == uObjLen);

	getGUID(p, objID); p += sizeof(tGUID);
	CHK(IsEqualGUID_ASF(&objID, &CLSID_AsfXCodecList) );
	uCodecCount = getle32(p); p += sizeof(uint32);
	CHK(uCodecCount <= 2);

	while(uCodecCount) {
		tASFCodecEntry *pEntry;
		uint16 uType;

		//INC_ASF_CTR (3);
		uType = getle16(p); p += sizeof(uint16);

		switch(uType)
		{
			case 0x0001:
				eIndex = Video_ASF;
				break;
			case 0x0002:
				eIndex = Audio_ASF;
				break;
			default:
				CHK(0);
		}
		pEntry = &pPriv->codecEntry[eIndex];

		pEntry->u16CodecType = uType;

		pEntry->u16CodecNameLength = getle16(p); p += sizeof(uint16);
		p += pEntry->u16CodecNameLength*2;

		pEntry->u16CodecDescLength = getle16(p); p += sizeof(uint16);
		p += pEntry->u16CodecDescLength * 2;

		pEntry->u16CodecInfoLen = getle16(p); p += sizeof(uint16);
		p += pEntry->u16CodecInfoLen;

		uCodecCount--;
	}//while(uCodecCount) {

	//CHK( (FS_tell (pPriv->Fid) - iStartOffset) == (int64)uObjLen);
	return OKAY;
}

static int  AsfParseStreamBitratePropObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
/*
	AsfParseUnknownObject(pCtxt, objId, uObjLen);
	assert (0);
*/

	PRINTF_GUID2(objId, uObjLen);
	VPRINTF(("we do nothing.\n"));
	ABV_skip((int64)uObjLen - sizeof(tGUID) - sizeof(uint64), SEEK_CUR);

	return OKAY;
}

static int  AsfParsePaddingObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
/*
	AsfParseUnknownObject(pCtxt, objId, uObjLen);
	assert (0);
*/
	PRINTF_GUID2(objId, uObjLen);
	VPRINTF(("we do nothing.\n"));
	ABV_skip((int64)uObjLen - sizeof(tGUID) - sizeof(uint64), SEEK_CUR);

	return OKAY;
}

static int  AsfParseDataObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
//	int64 iStartOffset;
	int iRet;
	uint8 *p = pPriv->pBuf;
	tPacketParser *pPacketParser = &pPriv->packetParser;
	uint32 size_of_data_object = /*sizeof(tGUID) + sizeof(uint64) + */sizeof(tGUID) + sizeof(uint64) + sizeof(uint16);

	//iStartOffset = FS_tell (pPriv->Fid);

	iRet = ABV_read(size_of_data_object, pPriv->pBuf);

	//getGUID(p, pPacketParser->stASFDataHeader.gObjID) ;         p += sizeof(tGUID);
	//pPacketParser->stASFDataHeader.u64ObjSize = getle64(p);    p += sizeof(uint64);
	pPacketParser->stASFDataHeader.gObjID = objId;
	pPacketParser->stASFDataHeader.u64ObjSize = uObjLen;
	PRINTF_GUID2 (objId, uObjLen);

	CHK(IsEqualGUID_ASF( & pPacketParser->stASFDataHeader.gObjID, &CLSID_CAsfDataObject));
	//CHK(pPacketParser->stASFDataHeader.u64ObjSize == uObjLen);

	getGUID(p,  pPacketParser->stASFDataHeader.gFileID); p += sizeof(tGUID);
	pPacketParser->stASFDataHeader.u64TotalDataPackets = getle64(p); p += sizeof(uint64);
	pPacketParser->stASFDataHeader.u16Reserved = getle16(p); p += sizeof(uint16);
	CHK(pPacketParser->stASFDataHeader.u16Reserved == 0x0101);

	//pPacketParser->iTotalStartOffset = pPacketParser->uCurPacketOffset = FS_tell (pPriv->Fid);
	//pPacketParser->iTotalEndOffset = pPacketParser->iTotalStartOffset+uObjLen-size_of_data_object;

	pPacketParser->uPacketSize = pPriv->fileProp.u32MaxPackSize;

	/*
	VPRINTF (("packet %d, each %d, total %d, start %x\n", (int)pPacketParser->stASFDataHeader.u64TotalDataPackets,
	    (int)pPacketParser->uPacketSize, (int)uObjLen-size_of_data_object, (int)pPacketParser->uCurPacketOffset));
	CHK( pPacketParser->stASFDataHeader.u64TotalDataPackets * pPacketParser->uPacketSize
	==uObjLen-size_of_data_object)
	FS_seek (pPriv->Fid, FILE_SEEK_CUR, (int64)uObjLen - size_of_data_object);
	*/
	
	/* 	we skip some "other top-level objects" if they attended. 
		maybe is wrong. 
	*/
	
	return OKAY;
}

static int  AsfParseSimpleIndexObject(DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
/* this function will never be called in this version.*/
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	tSimpleIndexObject *pSimpleIndex = &pPriv->simpleIndex;
//	int64 iStartOffset;
	int iRet;
	uint8 *p = pPriv->pBuf;
	uint32 size_of_simple_index_obj = /*sizeof(tGUID) + sizeof(uint64) + */sizeof(tGUID) + sizeof(uint64) + sizeof(uint32) + sizeof(uint32);

//	iStartOffset = FS_tell (pPriv->Fid);
	iRet = ABV_read(size_of_simple_index_obj, pPriv->pBuf);

	//getGUID(p, pSimpleIndex->gObjID);              p += sizeof(tGUID);
	//pSimpleIndex->u64ObjSize = getle64(p);         p += sizeof(uint64);
	pSimpleIndex->gObjID = objId;
	pSimpleIndex->u64ObjSize = uObjLen;
	PRINTF_GUID2 (objId, uObjLen);

	CHK( IsEqualGUID_ASF (&pSimpleIndex->gObjID, &CLSID_CAsfSimpleIndexObject));
	//CHK( pSimpleIndex->u64ObjSize == uObjLen);

	getGUID(p, pSimpleIndex->gFileID);             p += sizeof(tGUID);
	pSimpleIndex->u64TimeInterval = getle64(p);    p += sizeof(uint64);
	pSimpleIndex->u32MaxPacketCount = getle32(p);  p += sizeof(uint32);
	pSimpleIndex->u32EntryCount = getle32(p);      p += sizeof(uint32);

	uint32 size_of_index_entry = sizeof(uint32) + sizeof(uint16);
	CHK( size_of_simple_index_obj + pSimpleIndex->u32EntryCount*size_of_index_entry == uObjLen);

	/// FIXME read simple index entries
	//FS_seek (pPriv->Fid, FILE_SEEK_CUR, (int64)pSimpleIndex->u32EntryCount*size_of_index_entry);

	return OKAY;
}

static int AsfParseUnknownObject (DMX_tCtxt *pCtxt, tGUID objId, uint64 uObjLen)
{
//	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
//	tGUID objID;
//	uint64 objSize;
//	uint8 *p = pPriv->pBuf;

	//ABV_read(sizeof(tGUID) + sizeof(uint64), pPriv->pBuf);

	//getGUID(p, objID);         p += sizeof(tGUID);
	//objSize = getle64(p);      p += sizeof(uint64);
	//CHK(objSize == uObjLen);

	PRINTF_GUID2(objId, uObjLen);

	//FS_seek (pPriv->Fid, FILE_SEEK_CUR, (int64)uObjLen - sizeof(tGUID) - sizeof(uint64));
	ABV_skip((int64)uObjLen - sizeof(tGUID) - sizeof(uint64), SEEK_CUR);
	assert (0);
	return OKAY;
}

static int AsfParseOnePacket(DMX_tCtxt *pCtxt)
{
	tAsfPriv *pPriv = (tAsfPriv*)pCtxt->pPriv;
	tPacketParser *pParser = &pPriv->packetParser;

	uint8 uErrCorrFlags, uPropFlags;
	uint8 uMultiPayload, uSeqType, uPadLenType, uPackLenType;
	uint8 uRepDataLenType, uOffMedObjLenType, uMedObjNumLenType, uStmNumLentype;
	uint32 uPackLen, uSeq, uPadLen, uSendTime, uDuration, uIsKey;
	EMediaType_ASF eIndex;
	tASFPayLoadEntry *pPayLoad;
	//UBuff *pUb =  &pPriv->srcBuf.UB;
//	uint32 packetSize = pPriv->fileProp.u32MaxPackSize;
	int nIndex = 0;

	if(pParser->uCurPacket == pParser->stASFDataHeader.u64TotalDataPackets)
	{
		VPRINTF (("\n\n\nall packets have been pasred.\n"));	
		return 0;
	}
	VPRINTF (("\n\n\ncurrent packet %d\n", pParser->uCurPacket ));
	
	// Error Correction
	uErrCorrFlags = abv_get8(); nIndex++;
	if( uErrCorrFlags&0x80)
	{
		CHK(uErrCorrFlags == 0x82);
		// fixed 2 bytes now
		abv_getle16(); nIndex += 2;
		// should the one at payload parsing info
		uErrCorrFlags = abv_get8(); nIndex++;
	}
	CHK(!(uErrCorrFlags&0x80) );

	uPackLenType = (uErrCorrFlags>>5) & 0x3;
	uPadLenType = (uErrCorrFlags>>3) & 0x3;
	uSeqType = (uErrCorrFlags>>1) & 0x3;
	uMultiPayload = uErrCorrFlags&1;

	uPropFlags = abv_get8(); nIndex++;
	uStmNumLentype = (uPropFlags>>6) & 3;
	uMedObjNumLenType = (uPropFlags>>4) & 3;
	uOffMedObjLenType = (uPropFlags>>2) & 3;
	uRepDataLenType = (uPropFlags>>0) & 3;

	uPackLen = uPackLenType == 0 ? pParser->uPacketSize :
		uPackLenType==1 ? abv_get8() : 
		uPackLenType==2 ? abv_getle16() : 
		abv_getle32();
	nIndex += uPackLenType == 0 ? 0 :
		uPackLenType==1 ? 1 : 
		uPackLenType==2 ? 2 : 4;

	uSeq = uSeqType == 0 ? 0 :
		uSeqType==1? abv_get8() : 
		uSeqType==2? abv_getle16() : 
		abv_getle32();
	nIndex += uSeqType == 0 ? 0 :
		uSeqType==1? 1 : 
		uSeqType==2? 2 : 4;
	

	uPadLen = uPadLenType == 0 ? 0 :
		uPadLenType==1 ? abv_get8():
		uPadLenType==2 ? abv_getle16():
		abv_getle32();
	nIndex += uPadLenType == 0 ? 0 :
		uPadLenType==1 ? 1 :
		uPadLenType==2 ? 2 :4;

	uSendTime = abv_getle32(); nIndex += 4;
	uDuration = abv_getle16(); nIndex += 2;

	//VPRINTF (("send time %d duration %d", uSendTime, uDuration));
	if(uMultiPayload)
	{
		//VPRINTF (("Multiple %d\n", pParser->uCurPacket));
		uint8 uPayloadFlags, uNumPayload, uPayloadLenType, uStrmNum;
		uint32 uMedObjNum, uOffMedObj, uRepDataLen, uPayloadLen;
		uint64 uMedObjSize, uMedObjTime;

		uPayloadFlags = abv_get8();
		uNumPayload = uPayloadFlags & 0x3f;
		uPayloadLenType = (uPayloadFlags>>6)&3;

		while(uNumPayload)
		{
			uStrmNum = abv_get8();
			eIndex = (uStrmNum&(~80))== pPriv->iAudioStreamID?Audio_ASF:Video_ASF;
			uIsKey = uStrmNum&0x80?1:0;
			pPayLoad = &(pParser->pPayloadList[eIndex]);

			//INC_ASF_CTR (4);
			pPayLoad->isKey = uIsKey;
			uMedObjNum = uMedObjNumLenType==0?0:
			uMedObjNumLenType==1? abv_get8():
			uMedObjNumLenType==2? abv_getle16():
			abv_getle32();
			uOffMedObj = uOffMedObjLenType==0?0:
			uOffMedObjLenType==1? abv_get8():
			uOffMedObjLenType==2? abv_getle16():
			abv_getle32();
			uRepDataLen = uRepDataLenType==0?0:
			uRepDataLenType==1? abv_get8():
			uRepDataLenType==2? abv_getle16():
			abv_getle32();
			
			//rep data
			if (uRepDataLen == 1)
			{// multiple, compress
				CHK(0); //fix me, tcx
				/*
				uint8 uPtsDelta;

				uPtsDelta = abv_get8();
				uPayloadLen = uPayloadLenType==0?0:
					uPayloadLenType==1? abv_get8():
					uPayloadLenType==2? abv_getle16():
					abv_getle32();
				uMedObjTime = uOffMedObj;

				while(uPayloadLen)
				{// sub payload
					INC_ASF_CTR (5);
					pPayLoad = &(pParser->pPayloadList[eIndex]);
					pPayLoad->uFrmLen = pPayLoad->uLength = abv_get8();
					//pPayLoad->iOffset = pUb->rdptr;
					pPayLoad->iPts = uMedObjTime;
					pPayLoad->uFrameNum = 0;
					pPayLoad->iLast = 1;
					pPayLoad->uPacketNum = pParser->uCurPacket;

					//AsfOutputData (pCtxt, pPayLoad);

					uMedObjTime += uPtsDelta;
					uPayloadLen -= (1 + pPayLoad->uLength);

					memset (pPayLoad, 0, sizeof(tASFPayLoadEntry));
				}//while(uPayloadLen)
				*/
			}//if (uRepDataLen == 1)
			else
			{
				uMedObjSize = abv_getle32();
				uMedObjTime = abv_getle32();

				//if (uRepDataLen-8)
				//UB_advance_rdptr (pUb, uRepDataLen-8);

				uPayloadLen = uPayloadLenType==0?0:
					uPayloadLenType==1? abv_get8():
					uPayloadLenType==2? abv_getle16():
					abv_getle32();

				pPayLoad->uFrmLen = uMedObjSize;
				pPayLoad->uLength = uPayloadLen;
				//pPayLoad->iOffset = pUb->rdptr;
				pPayLoad->iPts = uMedObjTime;
				pPayLoad->uFrameNum = 0;
				pPayLoad->iLast = (uMedObjSize==(uOffMedObj+uPayloadLen));
				pPayLoad->uPacketNum = pParser->uCurPacket;

				//AsfOutputData (pCtxt, pPayLoad);

				if (pPayLoad->iLast) 
				{
					memset (pPayLoad, 0, sizeof(tASFPayLoadEntry));
				}
			}//else
			uNumPayload--;
		}//while(uNumPayload)
	}//if(uMultiPayload)
	else
	{// single
		//VPRINTF (("Single %d\n", pParser->uCurPacket));

		uint8 uStrmNum;
		uint32 uMedObjNum, uOffMedObj, uRepDataLen;
		uint64 uMedObjSize, uMedObjTime=0;

		uStrmNum = abv_get8(); nIndex++;
		eIndex = (uStrmNum&(~0x80))== pPriv->iAudioStreamID?Audio_ASF:Video_ASF;
		uIsKey = uStrmNum&0x80?1:0;
		pPayLoad = &(pParser->pPayloadList[eIndex]);

		pPayLoad->isKey = uIsKey;

		uMedObjNum = uMedObjNumLenType==0?0:
			uMedObjNumLenType==1? abv_get8():
			uMedObjNumLenType==2? abv_getle16():
			abv_getle32();
		nIndex += uMedObjNumLenType==0?0:
			uMedObjNumLenType==1? 1:
			uMedObjNumLenType==2? 2:4;

		uOffMedObj = uOffMedObjLenType==0?0:
			uOffMedObjLenType==1? abv_get8():
			uOffMedObjLenType==2? abv_getle16():
			abv_getle32();
		nIndex += uOffMedObjLenType==0?0:
			uOffMedObjLenType==1? 1:
			uOffMedObjLenType==2? 2:4;

		uRepDataLen = uRepDataLenType==0?0:
			uRepDataLenType==1? abv_get8():
			uRepDataLenType==2? abv_getle16():
			abv_getle32();
		nIndex += uRepDataLenType==0?0:
			uRepDataLenType==1? 1:
			uRepDataLenType==2? 2:4;

		if(uRepDataLen==1)
		{
			CHK(0); /// fix me, tcx
			/*
			uint8 uPtsDelta;
			int32 iByteRemain;

			// not yet
			CHK(0);
			uPtsDelta = abv_get8();

			if ( pUb->rdptr-pUb->start > pParser->uCurPacketOffset )
			{
				iByteRemain = (int32) (uPackLen-uPadLen-(pUb->rdptr-pUb->start-pParser->uCurPacketOffset));\
			}
			else
			{
				iByteRemain = (int32) (uPackLen-uPadLen-(pUb->rdptr-pUb->start+pUb->size-pParser->uCurPacketOffset));
			}

			while(iByteRemain>0)
			{// sub payload
				CHK(0);
				pPayLoad = &(pParser->pPayloadList[eIndex]);
				pPayLoad->uFrmLen = pPayLoad->uLength = UB_get8(pUb);
				pPayLoad->iOffset = pUb->rdptr;
				pPayLoad->iPts = uMedObjTime;
				pPayLoad->uFrameNum = 0;
				pPayLoad->iLast = 1;
				pPayLoad->uPacketNum = pParser->uCurPacket;

				AsfOutputData (pCtxt, pPayLoad);

				uMedObjTime += uPtsDelta;
				uPackLen -= pPayLoad->uLength;

				memset (pPayLoad, 0, sizeof(tASFPayLoadEntry));
			}

			CHK(iByteRemain==0);
			*/
		}//if(uRepDataLen==1)
		else
		{
			uMedObjSize = abv_getle32(); nIndex += 4;
			uMedObjTime = abv_getle32(); nIndex += 4;

			//if (uRepDataLen-8)
			//	UB_advance_rdptr (pUb, uRepDataLen-8);
			CHK((uRepDataLen - 8) == 0);

			pPayLoad->uLength = uPackLen - uPadLen - nIndex;
			pPayLoad->uFrmLen = uMedObjSize;
			//pPayLoad->iOffset = pUb->rdptr;
			pPayLoad->iPts = uMedObjTime;
			pPayLoad->uFrameNum = 0;
			pPayLoad->iLast = (uMedObjSize==(uOffMedObj+pPayLoad->uLength));
			pPayLoad->uPacketNum = pParser->uCurPacket;

			AsfOutputData (pCtxt, pPayLoad);

			if (pPayLoad->iLast == 1) {
				memset (pPayLoad, 0, sizeof(tASFPayLoadEntry));
			}
		}// (uRepDataLen==8)
	}// single
	
	if (uPadLen) // skip the padding data
	{
		ABV_skip(uPadLen, SEEK_CUR);
	}

	pParser->uCurPacket++;
	return 0;
}

static int  AsfOutputHeader (DMX_tCtxt *pCtxt)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;
	int i;
	struct DMX_tFileSummary *pSummary = &pCtxt->FileSummary;
	DMX_tStreamInfo *pStrmInfo=0;

	for(i=0; i<pSummary->NumStream; i++) 
	{
		pStrmInfo = &pSummary->pStreamInfoTbl[i];

		CHK(pStrmInfo);

		/// output specific data
		if(pStrmInfo->Type == DMX_STREAM_TYPE_VIDEO) 
		{
			assert(0); //this DMX only for audio
		}
		else if(pStrmInfo->Type == DMX_STREAM_TYPE_AUDIO) 
		{
			VPRINTF (("audio header\n"));
			bb_setle16(pPriv->audioInfo.u16FormatTag);
			bb_setle16(pPriv->audioInfo.u16NumChannels);
			bb_setle32(pPriv->audioInfo.u32SamplesPerSecond);
			bb_setle32(pPriv->audioInfo.u32BytesPerSecond);
			bb_setle16(pPriv->audioInfo.u16BlockAlign);
			bb_setle16(pPriv->audioInfo.u16BitsPerSample);
			bb_setle16(pPriv->audioInfo.u16CodecSpecifDataLen);

			bb_memcpy((uchar*)(pPriv->pbAudioSpecificData), pPriv->uAudioSpecificDataLen);
		}
	}//for (i=0; i<pSummary->NumStream; i++) 

	return OKAY;
}

static int  AsfOutputData (DMX_tCtxt *pCtxt, tASFPayLoadEntry *pPayLoad)
{
	tAsfPriv *pPriv = (tAsfPriv*) pCtxt->pPriv;

	uint8 isVideo = (pPayLoad-pPriv->packetParser.pPayloadList) == Video_ASF;
	int vch=-1, ach=-1, i;
	DMX_tFileSummary *pSummary = &pCtxt->FileSummary;
	uchar* pTmp = 0;

	for(i=0; i<pSummary->NumStream; i++)
	{
		int avType = pSummary->pStreamInfoTbl[i].Type;

		if (avType == DMX_STREAM_TYPE_AUDIO && !isVideo)
		{
			ach = i;
			break;
		}
		else if (avType == DMX_STREAM_TYPE_VIDEO && isVideo) 
		{
			vch = i;
			break;
		}
	}
	CHK(i < pSummary->NumStream);

	//pSummary->pStreamInfoTbl[i].SSNum;
	if (isVideo ) 
	{
		assert (0); // now only for audio DMX
	}
	else 
	{
		assert(ach>=0);
		pTmp = (uchar*)malloc(pPayLoad->uLength);
		assert(pTmp);
		ABV_read(pPayLoad->uLength, pTmp);
		bb_memcpy(pTmp, pPayLoad->uLength);
		free(pTmp); pTmp = 0;
	}

	if (pPayLoad->iLast) 
	{
		//VPRINTF (("\t\t%s ==> frame %d len %d pts %d, iskey %d\n", isVideo?"video":"audio",isVideo?pPriv->uVideoNum: pPriv->uAudioNum, pPayLoad->uFrmLen, (int32)pPayLoad->iPts, pPayLoad->isKey));

#if DEBUG
		CHK(pPayLoad->uFrmLenAcc == pPayLoad->uFrmLen);
#endif
		if ( isVideo )
			pPriv->uVideoNum ++;
		else
			pPriv->uAudioNum ++;
	}

	pPayLoad->order ++;
	return 0;
}

static DMX_tCtxt gASFDmx;
