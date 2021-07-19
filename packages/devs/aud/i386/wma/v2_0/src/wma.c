#include <cyg/io/aud/adecctrl.h>

#include "common.h"
#include "asf.h"
#include "wma.h"
#include "wmadec.h"


static DMX_tCtxt gASFDmx;
extern uint file_size;

#if SISPKG_IO_AUD_DEC_WMA_DEBUG && 1
#define CHK(xx) do {                                                                    \
	if (!(xx)) {                                                                        \
	    EPRINTF(("Assert error: %s, file %s, line %d\n", (#xx), __FILE__, __LINE__));   \
	}                                                                                   \
	}while (0);

#define PLINE    VPRINTF(("file %s, line %d\n", __FILE__, __LINE__));
#else
#define CHK(xx) 	{}
#define PLINE	{}
#endif

//int pDbg[128]={0};
#define SAFE_FREE(p) do{if (p) {free(p); p = 0;}} while (0);

static void WMA_Init(AUD_tCtrlBlk *pAcb);
static int  WMA_Decode(AUD_tCtrlBlk *pAcb);
static int  WMA_Cleanup(AUD_tCtrlBlk *pAcb);
static int  WMA_GetAttr(AUD_tCtrlBlk *pAcb);

int ExtractBits(int numbits)
{
	uint32 val = 0;
	assert((numbits >= 0) && (numbits <= 32));

	val = PeekBits(numbits);
	bitBuffGetIndex += numbits;
	
	return val;
}

int PeekBits(int numbits)
{
	int index = 0, lOff = 0, bitLen = 0, maskbits;
	uint64 val = 0;
	assert((numbits >= 0) && (numbits <= 32)); //? <=

	index = bitBuffGetIndex/8;
	lOff = bitBuffGetIndex%8;
	bitLen = lOff + numbits;

	if(bitLen <= 8)
	{
		val = *(gpBitBuff + index);
		val <<= lOff;
		val &= 0xFF;
		maskbits = 8;
	}
	else if(bitLen <= 16)
	{
		val = *(gpBitBuff + index);
		val <<= 8;
		val |= *(gpBitBuff + index + 1);
		val <<= lOff;
		val &= 0xFFFF;
		maskbits = 16;
	}
	else if(bitLen <= 24)
	{
		val = *(gpBitBuff + index);
		val <<= 8;
		val |= *(gpBitBuff + index + 1);
		val <<= 8;
		val |= *(gpBitBuff + index + 2);
		val <<= lOff;
		val &= 0xFFFFFF;
		maskbits = 24;
	}
	else if(bitLen <= 32)
	{
		val = *(gpBitBuff + index);
		val <<= 8;
		val |= *(gpBitBuff + index + 1);
		val <<= 8;
		val |= *(gpBitBuff + index + 2);
		val <<= 8;
		val |= *(gpBitBuff + index + 3);
		val <<= lOff;
		val &= 0xFFFFFFFF;
		maskbits = 32;
	}
	else
	{
		val = *(gpBitBuff + index);
		val <<= 8;
		val |= *(gpBitBuff + index + 1);
		val <<= 8;
		val |= *(gpBitBuff + index + 2);
		val <<= 8;
		val |= *(gpBitBuff + index + 3);
		val <<= 8;
		val |= *(gpBitBuff + index + 4);
		val <<= lOff;
		val &= 0xFFFFFFFFFF;
		maskbits = 40;
	}

	val >>= (maskbits - numbits);
	
	return val;
}


static int  WMA_DecodeHeader (AUD_tCtrlBlk *pAcb)
{
	assert (pAcb);
	VPRINTF (("WMA_DecodeHeader\n"));
	uint32 val;
	tWmaPriv *pPriv = pAcb->pPrivAttr; assert (pPriv);
	WmaObject *pWmaDec = &pPriv->stWmaDec; assert (pWmaDec);

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	assert (val == 0x161);
	pWmaDec->version_number = 2;

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	pWmaDec->num_channels = val;
	pAcb->Attr.NumCh = val;
	VPRINTF(("u16NumChannels = %d\n", pWmaDec->num_channels));

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	val |= ExtractBits (8)<<16;
	val |= ExtractBits (8)<<24;
	pWmaDec->samples_per_second = val;
	pAcb->Attr.SampleFreq = val;
	VPRINTF(("samples_per_second = %d\n", pWmaDec->samples_per_second));

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	val |= ExtractBits (8)<<16;
	val |= ExtractBits (8)<<24;
	pWmaDec->bytes_per_second = val;
	pAcb->Attr.ByteRate = val;
	VPRINTF(("bytes_per_second = %d\n", pWmaDec->bytes_per_second));

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	pWmaDec->bytes_per_packet = val;
	VPRINTF(("bytes_per_packet = %d\n", pWmaDec->bytes_per_packet));

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	assert (val == 16);

	val = ExtractBits (8) ;
	val |= ExtractBits (8)<<8;
	assert (val == 10);

	ExtractBits (8);
	ExtractBits (8);
	ExtractBits (8);
	ExtractBits (8);
	pWmaDec->encoder_options = ExtractBits (8);
	VPRINTF(("enc option = %d\n", pWmaDec->encoder_options));
	ExtractBits (8);
	ExtractBits (8);
	ExtractBits (8);
	ExtractBits (8);
	ExtractBits (8);
}

static void WMA_Init(AUD_tCtrlBlk *pAcb)
{
	tWmaPriv *pPriv;
	WmaObject *pWmaDec;
	
	AsfCreate(&gASFDmx, (long)file_size, &gASFDmx.FileSummary);
	AsfEngine(&gASFDmx); // output header data

	VPRINTF(("WMA_Init\n"));
	pAcb->pPrivAttr = pPriv = malloc (sizeof (tWmaPriv)); assert (pAcb->pPrivAttr);
	memset (pAcb->pPrivAttr, 0, sizeof (tWmaPriv));

	pWmaDec = &pPriv->stWmaDec;

	pWmaDec->pDecCtrl = pAcb;
	pPriv->inited = 1;

	return;
}

static int WMA_Decode(AUD_tCtrlBlk *pAcb)
{

	//AsfEngine(&gASFDmx); // DMX one packet at first


	tWmaPriv *pPriv= pAcb->pPrivAttr;
	WmaObject *pWmaDec = &pPriv->stWmaDec;
	
	/// call AsfEngine() to DMX packet, keep two packets data in gpBitBuff;
	if(bitBuffSetIndex < pWmaDec->bytes_per_packet) //the first frame
	{
		bb_reset();
		AsfEngine(&gASFDmx);
		AsfEngine(&gASFDmx);
	}
	if(bitBuffGetIndex >= (pWmaDec->bytes_per_packet * 8))
	{
		bitBuffSetIndex -= pWmaDec->bytes_per_packet;
		if(bitBuffSetIndex == 0)
		{
			return pWmaDec->currFrame; // return frame number for log print;
			return 1; // at the end of frame
		}
		bitBuffGetIndex -= (pWmaDec->bytes_per_packet * 8);
		memcpy(gpBitBuff, (gpBitBuff + pWmaDec->bytes_per_packet), pWmaDec->bytes_per_packet);
		AsfEngine(&gASFDmx);

		if(bitBuffSetIndex == pWmaDec->bytes_per_packet)// for the end frame decode
		{
			*(gpBitBuff + pWmaDec->bytes_per_packet) += 0x10; 
			*(gpBitBuff + pWmaDec->bytes_per_packet) &= 0xF0; 
			memset((gpBitBuff + pWmaDec->bytes_per_packet + 1), 0, (pWmaDec->bytes_per_packet - 1));
		}
	}
	///////////////////////////////////////////////////////////////

	VPRINTF (("before calling WmaDecEngine\n"));
	assert (pPriv); assert (pPriv->inited); assert (pWmaDec);

	return WmaDecEngine (pWmaDec);
}

static int WMA_Cleanup(AUD_tCtrlBlk *pAcb)
{
	tWmaPriv *pPriv= pAcb->pPrivAttr;
	WmaObject *pWmaDec = &pPriv->stWmaDec;

	assert (pAcb); assert (pPriv); assert (pPriv->inited); assert (pWmaDec);
	VPRINTF(("WMA_Cleanup\n"));

	VPRINTF(("call AsfDestroy() at first.\n"));
	AsfDestroy(&gASFDmx);
	
	WmaDecDestroy (pWmaDec);
	SAFE_FREE(pAcb->pPrivAttr);

	return OKAY;
}

static int WMA_GetAttr(AUD_tCtrlBlk *pAcb)
{
	tWmaPriv *pPriv = pAcb->pPrivAttr;
	WmaObject *pWmaDec = &pPriv->stWmaDec;
	AUD_tAttr *pAttr = &pAcb->Attr;

	VPRINTF(("WMA_GetAttr\n"));
	assert (pAcb); assert (pPriv); assert (pPriv->inited); assert (pWmaDec);
	assert (pAttr);

	WMA_DecodeHeader (pAcb);

	WmaDecInit (pWmaDec,
				pWmaDec->version_number,
				pWmaDec->samples_per_second,
				pWmaDec->num_channels,
				pWmaDec->bytes_per_second,
				pWmaDec->bytes_per_packet*8,
				pWmaDec->encoder_options);

	pAttr->Type = AUDTYPE_WMA;
	pAttr->BitsPerSamp = 16; assert (pAttr->BitsPerSamp==16);
	pAttr->NumCh = pWmaDec->num_channels; //assert ( pAttr->NumCh==2);

	pAttr->ChOrder[0] = 0;
	pAttr->ChOrder[1] = 1;
	pAttr->SampleFreq = pWmaDec->samples_per_second;
	pAttr->ByteRate = pWmaDec->bytes_per_second;

	return OKAY;
}

struct AUD_tModule AUD_ModuleWMA = {
        .Type    = AUDTYPE_WMA,
        .Init    = WMA_Init,
        .Decode  = WMA_Decode,
        .Cleanup = WMA_Cleanup,
        .GetAttr = WMA_GetAttr,
};
