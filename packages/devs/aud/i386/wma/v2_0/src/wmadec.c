#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "wmadec.h"
#include "wmabts.h"
#include "wmaentropy.h"
#include "wmatable.h"

#define	WMA_ALLOC(size)		malloc(size)
#define	WMA_FREE(p)			if ((void*)p) {free(p);} else {assert(0);}

#define CHK(xx) assert(xx)
//extern int pDbg[];

#if SISPKG_IO_AUD_DEC_WMA_DEBUG
#define PLINE    VPRINTF(("file %s, line %d\n", __FILE__, __LINE__));
#else
#define PLINE    {}
#endif

extern WMA_PACKET_BTS gWmaPacketBts;

static int32 WmaGetSamplePerFrame (int32 samplePerSec, uint32 bitPerSec, int32 num_channels, int32 version);
static int32 WmaPrvAllocate (WmaObject* pWmaDec);
static void  WmaSetBarkIndex (WmaObject*	pWmaDec);
static int32 WmaInitNoiseSub (WmaObject *pWmaDec);
static void  WmaUpdateMaxEsc(WmaObject* pWmaDec);
static int32 WmaDecodeFrameHeaderBark (WmaObject*	pWmaDec);
static int32 WmaDecodeFrameHeaderLpc (WmaObject*	pWmaDec);
static int32 WmaDecodeFrameHeaderNoiseSub (WmaObject*	pWmaDec);

static int32 WmaDecodeCoefHighRate  (WmaObject* pWmaDec);
static int32 WmaDecodeCoefLowMidRate  (WmaObject* pWmaDec);
static int32 WmaSetCoefTable  (WmaObject* pWmaDec);

static int32 WmaUpdateSubFrameConfig (WmaObject* pWmaDec);
static int32 WmaCalcQuantStep(int32 quantStep, FastFloat *pQuantStep);
static int32 WmaWeightedQuantization(WmaObject *pWmaDec, int32 ch);
static int32 WmaInverseQuantizeLowRate (WmaObject*	pWmaDec, uint8 ch);

static void WmaResampleWeightFactorLPC (WmaObject* pWmaDec, PerChannelInfo* ppcinfo);
static void WmaInitInverseQuadRootTable (WmaObject* pWmaDec);
static void WmaGetBandWeightLowRate (WmaObject* pWmaDec);

static void  WmaLsp2Lpc(uint8 *lsfQ, int32 *lpc, int32 order);
static int32 WmaLpcToSpectrum(WmaObject* pWmaDec, const int32* rgLpcCoef, PerChannelInfo* ppcinfo);

static void WmaConvolve(int32 *in1, int32 l1, int32 *in2, int32 l2, int32 *out, int32 *lout);
static void WmaConvolve_odd(int32* in1, int32 l1, int32* in2, int32 l2, int32* out, int32 *lout);

static void  WmaDoLpc4( const int32 k, const int32* pTmp, uint32* pWF, const int32 iSizeBy2, const int32 S1, const int32 C1 );
static uint32 InverseQuadRootOfSumSquares (int32 F1, int32 F2 );
static int32 Align2FracBits( const int32 iValue, const int32 cFracBits, const int32 cAlignFracBits );
static FastFloat ffltMultiply( FastFloat ffltA, FastFloat ffltB );

#define SQUARE(x) ( (uint64)(((int64)x * (int64)x)) )
#define SUM_SQUARES(x,y) (SQUARE(x)+SQUARE(y))

static int32 ROUNDF(float f)
{
    if (f < 0.0f)
        return (int32) (f - 0.5f);
    else
        return (int32) (f + 0.5f);
}

int32 LOG2(int32 i)
{   // returns n where n = log2(2^n) = log2(2^(n+1)-1)
    int32 iLog2 = 0;
    assert (i != 0);
    while ((i >> iLog2) > 1)
        iLog2++;

    return iLog2;
}

static int32 MultBp2Debug(int32 x, int32 y) {
    float fx,fy,fa,rel,dif;

    int32 a;
    a = MULT_BP2(x,y);

    fx = (float)x;
    fy = (float)y;
    fa = fx*fy/(1<<30);
    dif = (float)fabs(  (float) (((int)fa)-a) );
    rel = (float)fabs(dif/fa);
    if ((fa > 5.0f) && (rel > .5f)) {
        assert(0);
    }
    return a;
}

#define MULT_BP2X(x,y) MultBp2Debug(x,y)

#if 0
static uint64 SFTL_64 (uint64 a, int32 b) 
{
	uint64 val;
	uint32 hi, lo;

	hi = (uint32) (a>>32); 
	lo = (uint32) a; 
	assert (b < 64);

	if (b>=32) {
		hi = lo << (b-32); 
		lo = 0;
	}
	else {
		hi <<= b;
		hi |= lo >> (32-b);
		//hi |= lo << (b-32);		// equal
		lo <<= b;
	}
	val = (((uint64)hi)<<32) | lo;

	assert (val == ((uint64)a<<b));
	return val;
}


static uint64 SFTR_64 (uint64 a, int32 b) 
{
	uint64 val;
	uint32 hi, lo;

	hi = (uint32) (a>>32); 
	lo = (uint32) a; 
	assert (b < 64);

	if (b>=32) {
		lo = hi >> (b-32);
		//lo = hi << (32-b);	// equal
		hi = 0;		
	}
	else {
		lo >>= b;
		lo |= hi << (32-b);
		hi >>= b;
		//hi <<= (-b);
	}
	
	val = (((uint64)hi)<<32) | lo;

	assert (val == ((uint64)a>>b));
	return val;
}
#else
#define	SFTL_64(a, b) ((a) << (b))
#define	SFTR_64(a, b) ((a) >> (b))
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

int WmaDecInit (WmaObject*	pWmaDec,
                int32		version_number,
                int32		samples_per_second,
                uint16		num_channels,
                int32		bytes_per_second,
                int32		bits_per_packet,		// wzhong, seems constant packet size
                uint16		encoder_options
                )
{
	int   ret = 0;
	int32 subband;

	/// clear all
	//memset (pWmaDec, 0, sizeof (WmaObject));

	/// input setup
	pWmaDec->version_number = version_number; assert (version_number==2);
	pWmaDec->samples_per_second = samples_per_second;
	pWmaDec->num_channels = num_channels;
	pWmaDec->bytes_per_second = bytes_per_second;
	pWmaDec->bits_per_packet = bits_per_packet; assert ( !(bits_per_packet&7));
	pWmaDec->bytes_per_packet = bits_per_packet >> 3;
	pWmaDec->encoder_options = encoder_options;

	subband = WmaGetSamplePerFrame (samples_per_second, bits_per_packet, num_channels, version_number);
	pWmaDec->subBand         = subband;

	// 3.3 Weighting Mode
    pWmaDec->weightingMode    =	(encoder_options & ENCOPT_BARK) ? BARK_MODE : LPC_MODE;
	pWmaDec->fV5Lpc = !! (encoder_options & ENCOPT_V5LPC);
	//assert (encoder_options & ENCOPT_V5LPC == 0); 

    assert ( num_channels <= 2 );

	// 3.4 Allow Super and/or Sub Frames
    pWmaDec->allowSuperFrame  = !!(encoder_options & ENCOPT_SUPERFRAME);
    pWmaDec->allowSubFrame    = pWmaDec->allowSuperFrame && !!(encoder_options & ENCOPT_SUBFRAME);

	// 3.5 Maximum Number of Subframes in a Frame
    if (pWmaDec->allowSubFrame) {
        pWmaDec->maxSubFrameDiv = ((encoder_options & ENCOPT_SUBFRAMEDIVMASK) >>
            ENCOPT_SUBFRAMEDIVSHR);
        if (bytes_per_second / num_channels >= 4000)
            pWmaDec->maxSubFrameDiv = (8 << pWmaDec->maxSubFrameDiv);
        else
            pWmaDec->maxSubFrameDiv = (2 << pWmaDec->maxSubFrameDiv);
    }
    else
        pWmaDec->maxSubFrameDiv = 1;

    if (pWmaDec->maxSubFrameDiv > subband / MIN_FRAME_SIZE)
        pWmaDec->maxSubFrameDiv = subband / MIN_FRAME_SIZE;

	//3.8 Dithering Level
    if (pWmaDec->weightingMode == LPC_MODE)
        pWmaDec->fltDitherLevel = 0.04F;
    else // BARK_MODE
        pWmaDec->fltDitherLevel = 0.02F;

	// 3.6
    pWmaDec->fltBitsPerSample  = (float)(bytes_per_second*8.0f/(pWmaDec->samples_per_second*pWmaDec->num_channels));
    pWmaDec->fltWeightedBitsPerSample = pWmaDec->fltBitsPerSample;
	if (pWmaDec->num_channels > 1)
        pWmaDec->fltWeightedBitsPerSample *= (float) STEREO_WEIGHT;

    //decide secondary parameters
    //first the frame sizes
    pWmaDec->frameSample			= 2 * pWmaDec->subBand;
    pWmaDec->frameSampleHalf		= pWmaDec->subBand;
    pWmaDec->frameSampleQuad		= pWmaDec->subBand / 2;

    pWmaDec->subFrameSample			= pWmaDec->frameSample;
    pWmaDec->subFrameSampleHalf		= pWmaDec->frameSampleHalf;
    pWmaDec->subFrameSampleQuad		= pWmaDec->frameSampleQuad;

    pWmaDec->coefRecurQ1			= 0;
    pWmaDec->coefRecurQ2			= pWmaDec->subFrameSampleHalf;
	pWmaDec->coefRecurQ3			= pWmaDec->subFrameSampleHalf;
    pWmaDec->coefRecurQ4			= pWmaDec->subFrameSample;

    //init now; but can be reassigned in each frame; if so, be careful with tran. det.
	pWmaDec->minSubFrameSampleHalf	= pWmaDec->subFrameSampleHalf / pWmaDec->maxSubFrameDiv;
    pWmaDec->possibleWinSize		= LOG2 ((uint32)pWmaDec->maxSubFrameDiv) + 1;
	pWmaDec->log2_sz_raio			= LOG2 (LOG2 ((uint32)pWmaDec->maxSubFrameDiv)) + 1;
    pWmaDec->bitsSubbandMax			= LOG2 ((uint32)pWmaDec->subBand);

	// 3.2 Cutoff Frequencies
    if(pWmaDec->version_number == 1) {
		assert (0);
        pWmaDec->lowCutOffLong    = LOW_CUTOFF_V1;
	}
    else
        pWmaDec->lowCutOffLong    = LOW_CUTOFF;
    pWmaDec->highCutOffLong       = pWmaDec->subBand - 9 * pWmaDec->subBand / 100; //need investigation

    //default
    pWmaDec->lowCutOff            = pWmaDec->lowCutOffLong;
    pWmaDec->highCutOff           = pWmaDec->highCutOffLong;
	WmaSetBarkIndex (pWmaDec);

	WmaInitNoiseSub (pWmaDec);

    // 5.2 Run-Level (Entropy) Decoder
    pWmaDec->entropyMode = SIXTEENS_OB;                     // Huffman-RLC-16
    if (pWmaDec->fltWeightedBitsPerSample < 0.72f) {
        if (pWmaDec->samples_per_second >= 32000)
            pWmaDec->entropyMode = FOURTYFOURS_QB;			// Huffman-RLC-44Q
    }
    else if (pWmaDec->fltWeightedBitsPerSample < 1.16f) {
        if (pWmaDec->samples_per_second >= 32000) {
            pWmaDec->entropyMode = FOURTYFOURS_OB;			// Huffman-RLC-44O
        }
    }
	if (pWmaDec->entropyMode == SIXTEENS_OB) {
		pWmaDec->channelInfo[0].pHuffDecTbl = g_rgiHuffDecTbl16smOb;
		pWmaDec->channelInfo[0].pRunEntry   = gRun16smOb;
        pWmaDec->channelInfo[0].pLevelEntry = gLevel16smOb;
	}
	else if (pWmaDec->entropyMode == FOURTYFOURS_QB) {
		pWmaDec->channelInfo[0].pHuffDecTbl = g_rgiHuffDecTbl44smQb;
		pWmaDec->channelInfo[0].pRunEntry   = gRun44smQb;
        pWmaDec->channelInfo[0].pLevelEntry = gLevel44smQb;
	}
	else if (pWmaDec->entropyMode == FOURTYFOURS_OB ) {
		pWmaDec->channelInfo[0].pHuffDecTbl = g_rgiHuffDecTbl44smOb;
		pWmaDec->channelInfo[0].pRunEntry   = gRun44smOb;
        pWmaDec->channelInfo[0].pLevelEntry = gLevel44smOb;
	}

	WmaPrvAllocate (pWmaDec);
	WmaInitInverseQuadRootTable (pWmaDec);

    //initialize constants for packetization
    pWmaDec->bits_per_packet		= bits_per_packet;
    pWmaDec->bitPackedFrameSize	= LOG2 ((uint32)ROUNDF (pWmaDec->fltBitsPerSample * pWmaDec->frameSampleHalf / 8.0F)) + 2;
    pWmaDec->bitPacketHeader		= NBITS_PACKET_CNT + NBITS_FRM_CNT + pWmaDec->bitPackedFrameSize + 3;

	memset (&gWmaPacketBts, 0, sizeof (WMA_PACKET_BTS));
	gWmaPacketBts.seqNum = 0;
	gWmaPacketBts.firstFrmOffsetLen = pWmaDec->bitPackedFrameSize + 3;
	gWmaPacketBts.packetLen	= bits_per_packet>>3;
	gWmaPacketBts.newPacket = 1;

	pWmaDec->rampUpSize		= 2048;		// init
	pWmaDec->rampDownSize	= 2048;

	pWmaDec->randState.iPrior	= 0;
	pWmaDec->randState.uRand	= 0;

#if defined (SISPKG_IO_AUD_DEC_WMA_DEBUG) && 1
	extern char gDstWavFile[256];

//	pWmaDec->fpWav = fopen ("/wma2.wav", "w+b");
	pWmaDec->fpWav = fopen (gDstWavFile, "w+b");

	//int bytes = (pWmaDec->maxSubFrameDiv + 7)/8;
	int bytes = 2;
	/* quick and dirty, but documented */
	fwrite("RIFF", 1, 4, pWmaDec->fpWav); /* label */
	int temp = 44 -8;
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* length in bytes without header */
	fwrite("WAVEfmt ", 2, 4, pWmaDec->fpWav); /* 2 labels */
	temp = 2 + 2 + 4 + 4 + 2 + 2;
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* length of PCM format declaration area */
	temp = 1;
	fwrite(&temp, 1, 2, pWmaDec->fpWav); /* is PCM? */
	temp = pWmaDec->num_channels;
	fwrite(&temp, 1, 2, pWmaDec->fpWav); /* number of channels */
	temp = pWmaDec->samples_per_second;
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* sample frequency in [Hz] */
	temp = pWmaDec->samples_per_second * bytes * pWmaDec->num_channels;
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* bytes per second */
	temp = pWmaDec->num_channels * bytes;
	fwrite(&temp, 1, 2, pWmaDec->fpWav); /* bytes per sample time */
	//temp = pWmaDec->maxSubFrameDiv;
	VPRINTF(("pWmaDec->maxSubFrameDiv = %d, pWmaDec->fltBitsPerSample = %f\n", pWmaDec->maxSubFrameDiv, pWmaDec->fltBitsPerSample));
	temp = 16;
	assert(temp == 16);
	fwrite(&temp, 1, 2, pWmaDec->fpWav); /* bits per sample */
	fwrite("data", 1, 4, pWmaDec->fpWav); /* label */
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* length in bytes of raw PCM data */

#endif

	/// init
//	myWindow_Init ();
//	myDCT_Init ();

    return ret;
}

int WmaDecDestroy (WmaObject*	pWmaDec)
{
	WMA_FREE (pWmaDec->pCoefRecon[0]);
	WMA_FREE (pWmaDec->pCoefRecon[1]);
	WMA_FREE (pWmaDec->pCoefLowMidRate);

	WMA_FREE (pWmaDec->currOutput[0]);
	WMA_FREE (pWmaDec->currOutput[1]);

	WMA_FREE (pWmaDec->pingPrevOutput[0]);
	WMA_FREE (pWmaDec->pingPrevOutput[1]);
	WMA_FREE (pWmaDec->pongPrevOutput[0]);
	WMA_FREE (pWmaDec->pongPrevOutput[1]);

	WMA_FREE(gpBitBuff); // free the bitBuff
	
#if defined (SISPKG_IO_AUD_DEC_WMA_DEBUG) && 1

	int len = ftell(pWmaDec->fpWav);
	//int bytes = (pWmaDec->maxSubFrameDiv + 7)/8;
	//int bytes = 2;

	/* quick and dirty, but documented */
	int temp = len -8;
	fseek(pWmaDec->fpWav, 4, SEEK_SET);
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* length in bytes without header */

	temp = len - 44;
	fseek(pWmaDec->fpWav, 40, SEEK_SET);
	fwrite(&temp, 1, 4, pWmaDec->fpWav); /* length in bytes of raw PCM data */

	fclose(pWmaDec->fpWav);
	VPRINTF(("wav file closed.\n"));
#endif
	
	return 0;
}

int WmaDecEngine (WmaObject*	pWmaDec)
{
	uint8 seqNum;
    //printf ("entering WmaDecEngine\n");
    VPRINTF (("entering WmaDecEngine 1\n"));
	/*while (1) */
    {
		int i, frmInPacket;

		pWmaDec->getBits	= WmaGetBits;
		pWmaDec->peekBits	= WmaPeekBits;
		pWmaDec->byteAlign	= WmaByteAlign;
		pWmaDec->tellBits	= WmaTellBits;
        
        //VPRINTF (("engine \n")); pDbg[1] = __LINE__;
        //Bkk (40);
		if ( gWmaPacketBts.newPacket ) {
			/// take care of previous packet leftover
			gWmaPacketBts.seqNum = pWmaDec->getBits (4);
			gWmaPacketBts.frmInPacket = pWmaDec->getBits (4);
			gWmaPacketBts.firstFrmOffset = pWmaDec->getBits (gWmaPacketBts.firstFrmOffsetLen);
			gWmaPacketBts.newPacket = 0;
		}

		seqNum = gWmaPacketBts.seqNum;
		frmInPacket = gWmaPacketBts.frmInPacket;

		pWmaDec->currFrameInPacket = 0;
		for (i= 0; i<frmInPacket; i++) {
			//VPRINTF (("-------------------------------------------------------------\n"));
            
            //if ((pWmaDec->currFrame%256) == 0) 
			VPRINTF (("frame %d %d\n", pWmaDec->currFrame, pWmaDec->currFrameInPacket));
			
#if !defined (_MSC_VER) && 0 
            {         
                //AUD_tCtrlBlk *pDecCtrl = pWmaDec->pDecCtrl;
                //VPRINTF (("abv %x\n", (int) &(pDecCtrl->Abv));)
            }
#endif
			WmaDecFrame (pWmaDec);
			//VPRINTF (("offset %d %d\n", (gWmaPacketBts.offset-20)>>3, (gWmaPacketBts.offset-20)&7));
			pWmaDec->currFrame ++;
			pWmaDec->currFrameInPacket ++;
#if !defined (_MSC_VER)            
//            AUD_tCtrlBlk *pDecCtrl = pWmaDec->pDecCtrl;
//            pDecCtrl->Attr.NumFrames ++;
#endif
		}

		if (seqNum == gWmaPacketBts.seqNum) {
			/// leftover for the current packet
			uint32 btsLeft = gWmaPacketBts.packetLen*8 - gWmaPacketBts.offset;
			//VPRINTF (("skip %d bits\n", btsLeft));
			while (btsLeft > 31) {
				WmaGetBits2 (31);
				btsLeft -= 31;
			}
			WmaGetBits2 (btsLeft);
		}
		assert ( ((seqNum+1)&0xf) == gWmaPacketBts.seqNum);
		assert ( gWmaPacketBts.offset == gWmaPacketBts.firstFrmOffset+4+4+gWmaPacketBts.firstFrmOffsetLen );
	}
    return OKAY; 
}

int WmaDecFrame (WmaObject*	pWmaDec)
{
	/// assign
	if (pWmaDec->currFrameInPacket == gWmaPacketBts.frmInPacket-1 ) {
		pWmaDec->getBits	= WmaGetBits2;
		pWmaDec->peekBits	= WmaPeekBits2;
		pWmaDec->byteAlign	= WmaByteAlign2;
		pWmaDec->tellBits	= WmaTellBits2;
	}
	else {
		pWmaDec->getBits	= WmaGetBits;
		pWmaDec->peekBits	= WmaPeekBits;
		pWmaDec->byteAlign	= WmaByteAlign;
		pWmaDec->tellBits	= WmaTellBits;
	}

	/// FIXME to terminate
	pWmaDec->currSubFrame = 0;
	pWmaDec->sizeAccr = 0;
	pWmaDec->hasSubFrame = 0;
	while (pWmaDec->sizeAccr < pWmaDec->frameSampleHalf) {
		WmaDecSubFrame (pWmaDec);
		pWmaDec->currSubFrame ++;
		pWmaDec->sizeAccr += pWmaDec->sizePrev;
	}
	assert (pWmaDec->sizeAccr == pWmaDec->frameSampleHalf);
	return 0;
}

int WmaDecSubFrame (WmaObject*	pWmaDec)
{
	uint8 quant_incr, allZero=0;
	int32 i;

	//VPRINTF ((" === subframe %d of %d\n", pWmaDec->currSubFrame, pWmaDec->currFrame));

#if 0
    if (pWmaDec->currSubFrame ==0 && pWmaDec->currFrame==128) {
        int aaa=1;
		VPRINTF (("break here\n"));

        if (aaa) while (1);
	}
#endif

	/// header
	if (pWmaDec->currFrameInPacket==0 && pWmaDec->currSubFrame == 0) {
		pWmaDec->ratio_log2_prev = pWmaDec->getBits (pWmaDec->log2_sz_raio);
		pWmaDec->ratio_log2_curr = pWmaDec->getBits (pWmaDec->log2_sz_raio);
		pWmaDec->sizePrev = pWmaDec->frameSampleHalf/(1<<pWmaDec->ratio_log2_prev);
		pWmaDec->sizeCurr =	pWmaDec->frameSampleHalf/(1<<pWmaDec->ratio_log2_curr);
	}
	if (pWmaDec->ratio_log2_curr)
		pWmaDec->hasSubFrame = 1;
	pWmaDec->ratio_log2_next = pWmaDec->getBits (pWmaDec->log2_sz_raio);
	pWmaDec->sizeNext = pWmaDec->frameSampleHalf/(1<<pWmaDec->ratio_log2_next);

	assert (pWmaDec->ratio_log2_prev < 5);
	assert (pWmaDec->ratio_log2_curr < 5);
	assert (pWmaDec->ratio_log2_next < 5);

	WmaUpdateSubFrameConfig (pWmaDec);

	/// stereo and coded
	if (pWmaDec->num_channels == 1) {
		//assert (0);
		VPRINTF(("num_channels is 1.\n"));
		pWmaDec->channelInfo[0].power = pWmaDec->getBits (1);
		allZero = !pWmaDec->channelInfo[0].power;
	}
	else if (pWmaDec->num_channels == 2) {
		pWmaDec->stereo_mode = pWmaDec->getBits (1);
		pWmaDec->channelInfo[0].power = pWmaDec->getBits (1);
		pWmaDec->channelInfo[1].power = pWmaDec->getBits (1);
		allZero = (!pWmaDec->channelInfo[0].power) && (!pWmaDec->channelInfo[1].power);
	} else {
		assert(0);
	}

	/// quant
	if (allZero) {		
		if (pWmaDec->weightingMode == LPC_MODE) {
			pWmaDec->update_mask = pWmaDec->currSubFrame==0? 1: 0;
			pWmaDec->channelInfo[0].cSubbandActual = 0;
			if (pWmaDec->num_channels == 2)
				pWmaDec->channelInfo[1].cSubbandActual = 0;
			WmaDecodeFrameHeaderLpc (pWmaDec);		
		}
	}
	else {
		pWmaDec->quantStepSize = 1;
		do {
			quant_incr = pWmaDec->getBits (7);
			pWmaDec->quantStepSize += quant_incr;
		} while (quant_incr == MAX_QUANT_INCR);
		WmaCalcQuantStep (pWmaDec->quantStepSize, &pWmaDec->fQuantStep);

		/// compute max_escape_size
		WmaUpdateMaxEsc (pWmaDec);

		/// noise
		if ( pWmaDec->noiseSub ) {
			WmaDecodeFrameHeaderNoiseSub(pWmaDec);
		}

		/// update mask
		pWmaDec->update_mask = 1;
		if (pWmaDec->allowSuperFrame && pWmaDec->hasSubFrame) {
			pWmaDec->update_mask = pWmaDec->getBits (1);
		}

		if (pWmaDec->weightingMode == BARK_MODE/* && !pWmaDec->noiseSub*/) {
			/// bark mode and high rate
			WmaDecodeFrameHeaderBark (pWmaDec);
		}
		else {
			/// LPC 
			WmaDecodeFrameHeaderLpc (pWmaDec);
			if (pWmaDec->noiseSub) {
				WmaGetBandWeightLowRate (pWmaDec);
			}
			else {
				assert (0);
			}
		}
	}	
	
	/// run-level decoding
	if (pWmaDec->weightingMode == BARK_MODE && !pWmaDec->noiseSub) {
		WmaDecodeCoefHighRate (pWmaDec);
	}
	else {
		//int32 ch;
		WmaDecodeCoefLowMidRate (pWmaDec);
		//for (ch = 0; ch< pWmaDec->num_channels; ch++)
			
	}
	
	/// stereo L/R
	if (pWmaDec->num_channels ==2 && pWmaDec->stereo_mode == STEREO_SUMDIFF &&
		( pWmaDec->channelInfo[0].power || pWmaDec->channelInfo[1].power) ) {
		int32 temp, *pRecon0 = pWmaDec->channelInfo[0].pCoefRecon, *pRecon1 = pWmaDec->channelInfo[1].pCoefRecon;
		int32 maxCoef = (pWmaDec->weightingMode == BARK_MODE && !pWmaDec->noiseSub)? 
			pWmaDec->highCutOff: pWmaDec->cSubBand;

		for (i=0; i<maxCoef; i++) {
			temp = *pRecon0;
			*pRecon0++ = temp + *pRecon1;
			*pRecon1   = temp - *pRecon1; pRecon1++;
		}

		// now both chnl has power !!!
		pWmaDec->channelInfo[0].power = 1;
		pWmaDec->channelInfo[1].power = 1;
	}

	/// DCT
#if defined (_DEBUG) && 0
	{
		extern FILE *fpRecon;
		int32 ch, size, i, val;
		uint8 temp[4];
		if (!fpRecon) {
			fpRecon = fopen ("/recon.bin", "wb");
		}
		
		for (ch=0; ch<2; ch++) {
			for (i=0; i<(pWmaDec->subBand >> pWmaDec->ratio_log2_curr); i++) {
				val = pWmaDec->channelInfo[ch].pCoefRecon[i];
				temp[0] = (val>>24) & 0xff;
				temp[1] = (val>>16) & 0xff;
				temp[2] = (val>>8 ) & 0xff;
				temp[3] = (val>>0 ) & 0xff;
				fwrite (temp, 4, 1, fpRecon);
			}
		}		
	}
#endif	//defined (_DEBUG) && 1
	for (i=0; i<pWmaDec->num_channels; i++) {
		int *pCoef = pWmaDec->channelInfo[i].pCoefRecon;
		if (pWmaDec->channelInfo[i].power)
			myDctIV(pWmaDec, pCoef, 0, 0);
		myWindowing (pWmaDec, i);
	}

#if 0
	{
		static FILE *fp0=0, *fp1=0;
		int32 ch;
		if (!fp0) {
			fp0 = fopen ("dct.0", "w"); assert (fp0);
			fp1 = fopen ("dct.1", "w"); assert (fp1);
		}
		
		
		for (i=0; i<(pWmaDec->subBand >> pWmaDec->ratio_log2_curr); i++) {
			fprintf (fp0, "%08X\n", pWmaDec->channelInfo[0].pCoefRecon[i]);
			fprintf (fp1, "%08X\n", pWmaDec->channelInfo[1].pCoefRecon[i]); 
		}
	}
#endif

#if defined (_MSC_VER) && defined (_DEBUG) && 0
	{
		int ch, i;
		for (ch=0; ch<2; ch++) {
			int *p = pWmaDec->channelInfo[ch].pCoefRecon;
			VPRINTF ("\n");
			for (i=0; i< (pWmaDec->subBand >> pWmaDec->ratio_log2_curr); i++) {
				if ( (i&7) == 0 )
					VPRINTF ("**** ");
				VPRINTF ("%08x ", *p++);
				if ( (i&7) == 7 )
					VPRINTF ("\n");
			}
		}
	}
#endif

#if defined (SISPKG_IO_AUD_DEC_WMA_DEBUG) && 1
#define	CLIP(a) ( (a)>=32767? 32767: (a)<=(-32768)? (-32768): (a) )
	{
		int32 i, *p0, *p1;

		assert (pWmaDec->fpWav);
		p0 = pWmaDec->channelInfo[0].pCurrOutput;
		p1 = pWmaDec->channelInfo[1].pCurrOutput;

		for (i=0; i<pWmaDec->currOutputSamples; i++) {
			short sample[2];	// little endian
			sample[0] = CLIP (*p0);
			sample[1] = CLIP (*p1);
			if(pWmaDec->num_channels == 1)
				fwrite (&sample[0], 1, 2, pWmaDec->fpWav);
			else if(pWmaDec->num_channels == 2)
				fwrite (&sample[0], 1, 4, pWmaDec->fpWav);
			else
				assert(0);
			p0 ++;
			p1 ++;
		}
	}
#endif

	/// clear up
	pWmaDec->sizePrev = pWmaDec->sizeCurr;
	pWmaDec->sizeCurr =	pWmaDec->sizeNext;
	pWmaDec->ratio_log2_prev = pWmaDec->ratio_log2_curr;
	pWmaDec->ratio_log2_curr = pWmaDec->ratio_log2_next;
	return 0;
}

static int32 WmaDecodeCoefLowMidRate  (WmaObject* pWmaDec)
{
	uint8 ch;

	WmaSetCoefTable (pWmaDec);
	for (ch = 0; ch < pWmaDec->num_channels; ch++) {
		PerChannelInfo* pChnlInfo = &pWmaDec->channelInfo[ch];

		/// clear
#if 0
		memset (pChnlInfo->pCoefRecon, 0,
			sizeof (int32) * (pWmaDec->subBand >> pWmaDec->ratio_log2_curr));
#endif
		if (pChnlInfo->power ) {
			memset (pWmaDec->pCoefLowMidRate, 0, sizeof(int32) * pWmaDec->highCutOff);
			WmaDecRunLevelLowMidRate ( pWmaDec, ch);

			if(pWmaDec->weightingMode == BARK_MODE) // add by tcx 2008.04.23 for MidRate
				WmaInverseQuantizeMidRate(pWmaDec, ch);
			else
				WmaInverseQuantizeLowRate (pWmaDec, ch);
		}
		else {
			memset (pChnlInfo->pCoefRecon, 0,
			sizeof (int32) * (pWmaDec->subBand >> pWmaDec->ratio_log2_curr));
		}
#if 0
		{
			static int cnt = 0;
			int band;
			PerChannelInfo *ppcinfo = &pWmaDec->channelInfo[ch];
			static FILE *fpIQ = 0;

			if (cnt == 0) {
				fpIQ = fopen ("iq.txt", "w");
				cnt ++;
			}
			fprintf (fpIQ, "## frame %d sub %d, ch %d\n", pWmaDec->currFrame, pWmaDec->currSubFrame, ch);	
			
			for (band = 0; band < pWmaDec->cSubBand; band ++) {
				fprintf (fpIQ, "%08x ", ppcinfo->pCoefRecon[band]);
				if ((band & 7) == 7)
					fprintf (fpIQ, "\n");
			}
		}
#endif
	}
	return 0;
}

static int32 WmaDecodeCoefHighRate  (WmaObject* pWmaDec)
{
	uint8 ch;

	WmaSetCoefTable (pWmaDec);
	for (ch = 0; ch < pWmaDec->num_channels; ch++) {
		PerChannelInfo* pChnlInfo = &pWmaDec->channelInfo[ch];

		/// clear
		memset (pChnlInfo->pCoefRecon, 0,
			sizeof (int32) * (pWmaDec->subBand >> pWmaDec->ratio_log2_curr));
		if (pChnlInfo->power ) {
			WmaDecRunLevelHighRate ( pWmaDec, ch);
		}
	}
	return 0;
}

static int32 WmaSetCoefTable  (WmaObject* pWmaDec)
{
	PerChannelInfo* pChnlInfo = &pWmaDec->channelInfo[0];

	if (pWmaDec->num_channels == 2)
	{
		pChnlInfo = &pWmaDec->channelInfo[1];
		if (pWmaDec->entropyMode == SIXTEENS_OB) {
			if (pWmaDec->stereo_mode == STEREO_SUMDIFF) {
				pChnlInfo->pHuffDecTbl	= g_rgiHuffDecTbl16ssOb;
				pChnlInfo->pRunEntry	= gRun16ssOb;
				pChnlInfo->pLevelEntry	= gLevel16ssOb;
			}
			else {
				pChnlInfo->pHuffDecTbl	= g_rgiHuffDecTbl16smOb;
				pChnlInfo->pRunEntry	= gRun16smOb;
				pChnlInfo->pLevelEntry	= gLevel16smOb;
			}
		}
		else if (pWmaDec->entropyMode == FOURTYFOURS_QB)
		{
			if (pWmaDec->stereo_mode == STEREO_SUMDIFF) {
				pChnlInfo->pHuffDecTbl	= g_rgiHuffDecTbl44ssQb;
				pChnlInfo->pRunEntry	= gRun44ssQb;
				pChnlInfo->pLevelEntry	= gLevel44ssQb;
			}
			else {
				pChnlInfo->pHuffDecTbl	= g_rgiHuffDecTbl44smQb;
				pChnlInfo->pRunEntry	= gRun44smQb;
				pChnlInfo->pLevelEntry	= gLevel44smQb;
			}
		}
		else if (pWmaDec->entropyMode == FOURTYFOURS_OB) {
			if (pWmaDec->stereo_mode == STEREO_SUMDIFF)	{
				pChnlInfo->pHuffDecTbl	= g_rgiHuffDecTbl44ssOb;
				pChnlInfo->pRunEntry	= gRun44ssOb;
				pChnlInfo->pLevelEntry	= gLevel44ssOb;
			}
			else {
				pChnlInfo->pHuffDecTbl	= g_rgiHuffDecTbl44smOb;
				pChnlInfo->pRunEntry	= gRun44smOb;
				pChnlInfo->pLevelEntry	= gLevel44smOb;
			}
		}
	}
	return 0;
}

static int32 WmaDecodeFrameHeaderLpc (WmaObject*	pWmaDec)
{
	uint8 ch;
	int32 lpcCoef [LPCORDER];
	uint8 lpsCoef [LPCORDER];

	/// table 10
	for (ch = 0; ch < pWmaDec->num_channels; ch ++) {
		PerChannelInfo *pChnl = &pWmaDec->channelInfo[ch];
		if (pChnl->power) {
			if (pWmaDec->update_mask) {
				uint32 order;
				for (order = 0; order < LPCORDER; order ++ ){
					if (order==0 || order==8 || order==9) 
						lpsCoef [order] = pWmaDec->getBits (3);
					else 
						lpsCoef [order] = pWmaDec->getBits (4);					
				}
				// convert to spectrum
				WmaLsp2Lpc(lpsCoef, lpcCoef, LPCORDER);
#if defined (_MSC_VER) && 1
				//memset (pChnl->pWeightFactor, 0xff, 4*1024);
#endif
				WmaLpcToSpectrum(pWmaDec, lpcCoef, pChnl);
#if 0
				{
				static int cnt=0;
				if (cnt==0)
				{
					int i;
					FILE *fpLPC, *fpWeight, *fpTrig;

					fpLPC = fopen ("lpc.txt", "w"); assert (fpLPC);
					fpWeight = fopen ("weight.txt", "w"); assert (fpWeight);
					fpTrig = fopen ("trig.txt", "w"); assert (fpTrig);					

										
					for (i=0; i<LPCORDER; i++)
						fprintf (fpLPC, "%08x\n", lpcCoef[i]);
					fprintf (fpLPC, "0\n");
					fprintf (fpLPC, "0\n");
					fprintf (fpLPC, "0\n");
					fprintf (fpLPC, "0\n");
					fprintf (fpLPC, "0\n");
					fprintf (fpLPC, "0\n");

					fprintf (fpLPC, "5a827998\n");
					fprintf (fpLPC, "2d413ccc\n");
					
					fprintf (fpLPC, "%x\n", pWmaDec->frameSampleHalf);

					for (i=0; i<pWmaDec->frameSampleHalf; i++) 
						fprintf (fpWeight, "%08x\n", pChnl->pWeightFactor[i]);
					fprintf (fpWeight, "// max_weight %x\n", pChnl->maxWeight);

					/// FIXME 
					for (i=0; i<1024; i++)
						fprintf (fpTrig, "%08x\n", trig_1024[i]);			


					fclose (fpLPC);
					fclose (fpWeight);
					fclose (fpTrig);					
					//cnt ++;

				}
				cnt ++;
				}
#endif
			}
			else if (pWmaDec->currSubFrame > 0 ) {
				/// resample_weights
				WmaResampleWeightFactorLPC (pWmaDec, pChnl);
			}			
		}
		else if (pWmaDec->update_mask) {
			//artifically set to constants since nothing got sent for this channel; see comments in msaudioenc.c
			uint32* pWeightFactor = pChnl->pWeightFactor;
            int16 band;

			pChnl->maxWeight = WEIGHT_FROM_FLOAT(1.0F);			
			for (band = 0; band < pWmaDec->cSubBand; band++ )
				*pWeightFactor++ = WEIGHT_FROM_FLOAT(1.0F);
		}
		else if (pWmaDec->currSubFrame > 0) {
			///resample_weights
			WmaResampleWeightFactorLPC (pWmaDec, pChnl);
		}		
	}
	//pWmaDec->ratio_log2_curr_with_update = pWmaDec->ratio_log2_curr;
	
#if 0
	{
		static int cnt = 0;
		int band, ch;

		static FILE *fpWeight = 0;
		if (cnt == 0) {
			fpWeight = fopen ("weight.txt", "w");
			cnt ++;
		}
		fprintf (fpWeight, "## frame %d sub %d\n", pWmaDec->currFrame, pWmaDec->currSubFrame);
		for (ch = 0; ch<2; ch++) {
			PerChannelInfo *pChnl = &pWmaDec->channelInfo[ch];
			fprintf (fpWeight, "ch %d, max %08x\n", ch, pChnl->maxWeight);
			for (band = 0; band < pWmaDec->cSubBand; band ++) {
				fprintf (fpWeight, "%08x\n", pChnl->pWeightFactor[band]);
			}
		}
	}
#endif
	return 0;
}

static int32 WmaDecodeFrameHeaderBark (WmaObject*	pWmaDec)
{
	uint8 ch;

	/// table 9
	for (ch = 0; ch < pWmaDec->num_channels; ch ++) {
		uint8 band = 0;
		if (pWmaDec->channelInfo[ch].power) {
			PerChannelInfo *pChnlInfo = &pWmaDec->channelInfo[ch];

			if (pWmaDec->update_mask) {
				//memset (pChnlInfo->barkMaskQ, 0, sizeof (BarkMask) * 25); // modify by tcx 2008.04.17
				memset(pChnlInfo->barkMaskQ, 0, sizeof(int)*25);
				memset(pChnlInfo->fbarkMaskQ, 0, sizeof(FastFloat)*25);
				
				//pChnlInfo->barkMaskQ[0].maskQ = 0; // del by tcx 2008.04.17
				pChnlInfo->maxMaskQ = 0;

				assert (pWmaDec->barkInfo[pWmaDec->ratio_log2_curr].num_bark_bands <= 25);

				for (; band < pWmaDec->barkInfo[pWmaDec->ratio_log2_curr].num_bark_bands; band ++) {
					int32 mask_diff, mask_prev;

					WmaDecHuf (pWmaDec, g_rgiHuffDecTblMsk, &mask_diff);
					mask_diff -= 60;

					/*
					mask_prev = (band ==0)?36:pChnlInfo->barkMaskQ[band-1].maskQ;
					pChnlInfo->barkMaskQ[band].maskQ = mask_diff + mask_prev;
					if (pChnlInfo->barkMaskQ[band].maskQ > pChnlInfo->maxMaskQ)
						pChnlInfo->maxMaskQ = pChnlInfo->barkMaskQ[band].maskQ;
					*/ // modify by tcx 2008.04.17

					mask_prev = (band == 0) ? 36:pChnlInfo->barkMaskQ[band - 1];
					pChnlInfo->barkMaskQ[band] = mask_diff + mask_prev;
					if(pChnlInfo->barkMaskQ[band] > pChnlInfo->maxMaskQ)
						pChnlInfo->maxMaskQ = pChnlInfo->barkMaskQ[band];
				}
				pWmaDec->ratio_log2_curr_with_update = pWmaDec->ratio_log2_curr;

				/// calculate
			}
			else {
			}

			WmaWeightedQuantization(pWmaDec, ch);

#if defined (_MSC_VER) && defined (_DEBUG) && 0
			for (band=0; band< 25; band ++) {
				VPRINTF (("%2d ", pChnlInfo->barkMaskQ[band].maskQ));
				if ( band%5 == 4)
					VPRINTF (("\n"));
			}
#endif

		}
	}
	return 0;
}
// 5.1.1 Bark Band Computation
static void WmaSetBarkIndex (WmaObject*	pWmaDec)
{
    int32 i, iWin;
    float fltSamplingPeriod;

    uint16 *piBarkIndex;
    int32 cFrameSample;
    int8 bark_initialized;

	// 3.1 Samples Per Frame
    fltSamplingPeriod = 1.0F / pWmaDec->samples_per_second;

    if(pWmaDec->version_number == 1) {
		assert (0);
    }
    else
    {
		assert (pWmaDec->possibleWinSize <= 5);
        for (iWin = 0; iWin < pWmaDec->possibleWinSize; iWin++) {
			piBarkIndex = pWmaDec->barkInfo[iWin].bark_index;
            piBarkIndex  [0] = 0;
            cFrameSample = pWmaDec->frameSample / (1 << iWin);
            bark_initialized = 0;

            if (pWmaDec->samples_per_second >= 44100) {
                if(cFrameSample == 1024) { 
                    // winsize = 512
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 17;
                    piBarkIndex[1]  = 5;    piBarkIndex[2]  = 12;
                    piBarkIndex[3]  = 18;   piBarkIndex[4]  = 25;
                    piBarkIndex[5]  = 34;   piBarkIndex[6]  = 46;
                    piBarkIndex[7]  = 54;   piBarkIndex[8]  = 63;
                    piBarkIndex[9]  = 86;   piBarkIndex[10] = 102;
                    piBarkIndex[11] = 123;  piBarkIndex[12] = 149;
                    piBarkIndex[13] = 179;  piBarkIndex[14] = 221;
                    piBarkIndex[15] = 279;  piBarkIndex[16] = 360;
                    piBarkIndex[17] = 512;
                }
                else if(cFrameSample == 512) { 
                    // winsize = 256
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 15;
                    piBarkIndex[1]  = 5;    piBarkIndex[2]  = 11;
                    piBarkIndex[3]  = 17;   piBarkIndex[4]  = 23;
                    piBarkIndex[5]  = 31;   piBarkIndex[6]  = 37;
                    piBarkIndex[7]  = 43;   piBarkIndex[8]  = 51;
                    piBarkIndex[9]  = 62;   piBarkIndex[10] = 74;
                    piBarkIndex[11] = 89;   piBarkIndex[12] = 110;
                    piBarkIndex[13] = 139;  piBarkIndex[14] = 180;
                    piBarkIndex[15] = 256;
                }
                else if(cFrameSample == 256) { 
                    // winsize = 128
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 12;
                    piBarkIndex[1]  = 4;   piBarkIndex[2]  = 9;
                    piBarkIndex[3]  = 12;  piBarkIndex[4]  = 16;
                    piBarkIndex[5]  = 21;  piBarkIndex[6]  = 26;
                    piBarkIndex[7]  = 37;  piBarkIndex[8]  = 45;
                    piBarkIndex[9]  = 55;  piBarkIndex[10] = 70;
                    piBarkIndex[11] = 90;  piBarkIndex[12] = 128;
                }
            }
            else if (pWmaDec->samples_per_second >= 32000) {
                if(cFrameSample == 1024) {
                    // winsize = 512
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 16;
                    piBarkIndex[1]  = 6;   piBarkIndex[2]  = 13;
                    piBarkIndex[3]  = 20;  piBarkIndex[4]  = 29;
                    piBarkIndex[5]  = 41;  piBarkIndex[6]  = 55;
                    piBarkIndex[7]  = 74;  piBarkIndex[8]  = 101;
                    piBarkIndex[9]  = 141; piBarkIndex[10] = 170;
                    piBarkIndex[11] = 205; piBarkIndex[12] = 246;
                    piBarkIndex[13] = 304; piBarkIndex[14] = 384;
                    piBarkIndex[15] = 496; piBarkIndex[16] = 512;
                }
                else if(cFrameSample == 512) { 
                    // winsize = 256
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 15;
                    piBarkIndex[1]  = 5;   piBarkIndex[2]  = 10;
                    piBarkIndex[3]  = 15;  piBarkIndex[4]  = 20;
                    piBarkIndex[5]  = 28;  piBarkIndex[6]  = 37;
                    piBarkIndex[7]  = 50;  piBarkIndex[8]  = 70;
                    piBarkIndex[9]  = 85;  piBarkIndex[10] = 102;
                    piBarkIndex[11] = 123; piBarkIndex[12] = 152;
                    piBarkIndex[13] = 192; piBarkIndex[14] = 248;
                    piBarkIndex[15] = 256;
                }
                else if(cFrameSample == 256) { 
                    // winsize = 128
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 11;
                    piBarkIndex[1]  = 4;   piBarkIndex[2]  = 9;
                    piBarkIndex[3]  = 14;  piBarkIndex[4]  = 19;
                    piBarkIndex[5]  = 25;  piBarkIndex[6]  = 35;
                    piBarkIndex[7]  = 51;  piBarkIndex[8]  = 76;
                    piBarkIndex[9]  = 96;  piBarkIndex[10] = 124;
                    piBarkIndex[11] = 128;
                }
            }
            else if(pWmaDec->samples_per_second >= 22050) {
                if(cFrameSample == 512)	{
                    // winsize = 256
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 14;
                    piBarkIndex[1]  = 5;   piBarkIndex[2]  = 12;
                    piBarkIndex[3]  = 18;  piBarkIndex[4]  = 25;
                    piBarkIndex[5]  = 34;  piBarkIndex[6]  = 46;
                    piBarkIndex[7]  = 63;  piBarkIndex[8]  = 86;
                    piBarkIndex[9]  = 102; piBarkIndex[10] = 123;
                    piBarkIndex[11] = 149; piBarkIndex[12] = 179;
                    piBarkIndex[13] = 221; piBarkIndex[14] = 256;
                }
                else if(cFrameSample == 256) { 
                    // winsize = 128
                    bark_initialized = 1;
                    pWmaDec->barkInfo[iWin].num_bark_bands = 10;
                    piBarkIndex[1]  = 5;   piBarkIndex[2]  = 11;
                    piBarkIndex[3]  = 17;  piBarkIndex[4]  = 23;
                    piBarkIndex[5]  = 31;  piBarkIndex[6]  = 43;
                    piBarkIndex[7]  = 62;  piBarkIndex[8]  = 89;
                    piBarkIndex[9]  = 110; piBarkIndex[10] = 128;
                }
            }

            if(!bark_initialized) {
                float fltTemp = cFrameSample*fltSamplingPeriod;
                int32 iIndex;
                int32 iFreqCurr = 0;
                int32 iCurr = 1;
                while(1) {
                    iIndex = ((int32) ((g_rgiBarkFreq [iFreqCurr++]*fltTemp + 2.0f)/4.0f))*4; // open end

                    if(iIndex > piBarkIndex[iCurr - 1])
                        piBarkIndex[iCurr++] = iIndex;

                    if((iFreqCurr >= NUM_BARK_BAND) ||(piBarkIndex[iCurr - 1] > cFrameSample/2)) {
                        piBarkIndex[iCurr - 1] = cFrameSample/2;
                        pWmaDec->barkInfo[iWin].num_bark_bands = iCurr - 1;
                        break;
                    }
                }
            }
            else
            {
                for (i = 0; i < pWmaDec->barkInfo[iWin].num_bark_bands; i++) {
                    piBarkIndex [i + 1] = ((piBarkIndex [i + 1] + 2) / 4) * 4;  //rounding
                    assert (piBarkIndex [i + 1] > piBarkIndex [i]);
                }
            }

            piBarkIndex +=  NUM_BARK_BAND + 1;
        }
    }

	return;
}

static int32 WmaUpdateSubFrameConfig (WmaObject* pWmaDec)
{
	pWmaDec->cSubBand			= pWmaDec->subBand			>> pWmaDec->ratio_log2_curr;
	pWmaDec->subFrameSample		= pWmaDec->frameSample		>> pWmaDec->ratio_log2_curr;

	pWmaDec->lowCutOff          = pWmaDec->lowCutOffLong	>> pWmaDec->ratio_log2_curr;
    pWmaDec->highCutOff         = pWmaDec->highCutOffLong	>> pWmaDec->ratio_log2_curr;
	
	pWmaDec->channelInfo[0].cSubbandActual = pWmaDec->highCutOff - pWmaDec->lowCutOff;

	if (pWmaDec->num_channels == 2)
		pWmaDec->channelInfo[1].cSubbandActual = pWmaDec->highCutOff - pWmaDec->lowCutOff;

	//update first noise index
    if (pWmaDec->noiseSub) {
        pWmaDec->firstNoiseIndex = (int32) (0.5F + pWmaDec->firstNoiseFreq * pWmaDec->subFrameSample 
            / ((float) pWmaDec->samples_per_second));
        if (pWmaDec->firstNoiseIndex > pWmaDec->cSubBand) 
            pWmaDec->firstNoiseIndex = pWmaDec->cSubBand;

        // use precalculated values
        pWmaDec->currFirstNoiseBand = pWmaDec->firstNoiseBand[pWmaDec->ratio_log2_curr];
    }

	if (pWmaDec->sizeCurr > pWmaDec->sizePrev) {
		pWmaDec->coefRecurQ1 = (pWmaDec->sizeCurr - pWmaDec->sizePrev) / 2;
		pWmaDec->coefRecurQ2 = (pWmaDec->sizeCurr + pWmaDec->sizePrev) / 2;
	}
	else {
		pWmaDec->coefRecurQ1 = 0;
		pWmaDec->coefRecurQ2 = pWmaDec->sizeCurr;
	}

	if (pWmaDec->sizeCurr > pWmaDec->sizeNext) {
		pWmaDec->coefRecurQ3 = pWmaDec->sizeCurr + (pWmaDec->sizeCurr - pWmaDec->sizeNext) / 2;
		pWmaDec->coefRecurQ4 = pWmaDec->sizeCurr + (pWmaDec->sizeCurr + pWmaDec->sizeNext) / 2;
	}
	else {
		pWmaDec->coefRecurQ3 = pWmaDec->sizeCurr;
		pWmaDec->coefRecurQ4 = pWmaDec->sizeCurr + pWmaDec->sizeCurr;
	}

	pWmaDec->rampUpSize		= min2 (pWmaDec->sizePrev, pWmaDec->sizeCurr);
	pWmaDec->rampDownSize	= min2 (pWmaDec->sizeNext, pWmaDec->sizeCurr);
	pWmaDec->rampHorSize	= pWmaDec->coefRecurQ3 - pWmaDec->coefRecurQ2;

#if defined (_MSC_VER) && defined (_DEBUG) && 0
	VPRINTF (("---- Q1 = 0x%x\n", pWmaDec->coefRecurQ1));
	VPRINTF (("---- Q2 = 0x%x\n", pWmaDec->coefRecurQ2));
	VPRINTF (("---- Q3 = 0x%x\n", pWmaDec->coefRecurQ3));
	VPRINTF (("---- Q4 = 0x%x\n", pWmaDec->coefRecurQ4));
#endif


	pWmaDec->channelInfo[0].pCoefRecon = pWmaDec->pCoefRecon[0];

	if (pWmaDec->num_channels == 2)
		pWmaDec->channelInfo[1].pCoefRecon = pWmaDec->pCoefRecon[1];

	pWmaDec->channelInfo[0].pCurrOutput = pWmaDec->currOutput[0];
	if (pWmaDec->num_channels == 2)
		pWmaDec->channelInfo[1].pCurrOutput = pWmaDec->currOutput[1];
	if (pWmaDec->pingpong) {
		pWmaDec->channelInfo[0].pPrevOutput = pWmaDec->pingPrevOutput[0];
		if (pWmaDec->num_channels == 2)
			pWmaDec->channelInfo[1].pPrevOutput = pWmaDec->pingPrevOutput[1];
		pWmaDec->channelInfo[0].pNextOutput = pWmaDec->pongPrevOutput[0];
		if (pWmaDec->num_channels == 2)
			pWmaDec->channelInfo[1].pNextOutput = pWmaDec->pongPrevOutput[1];
	}
	else {
		pWmaDec->channelInfo[0].pPrevOutput = pWmaDec->pongPrevOutput[0];
		if (pWmaDec->num_channels == 2)
			pWmaDec->channelInfo[1].pPrevOutput = pWmaDec->pongPrevOutput[1];
		pWmaDec->channelInfo[0].pNextOutput = pWmaDec->pingPrevOutput[0];
		if (pWmaDec->num_channels == 2)
			pWmaDec->channelInfo[1].pNextOutput = pWmaDec->pingPrevOutput[1];
	}
	pWmaDec->pingpong ^= 1;

	return 0;
}

void    WmaUpdateMaxEsc(WmaObject* pWmaDec)
{
    uint32 quantStepSize = pWmaDec->quantStepSize;
    if(quantStepSize < 5)       pWmaDec->max_escape_size = 13;
    else if(quantStepSize < 15) pWmaDec->max_escape_size = 13;
    else if(quantStepSize < 32) pWmaDec->max_escape_size = 12;
    else if(quantStepSize < 40) pWmaDec->max_escape_size = 11;
    else if(quantStepSize < 45) pWmaDec->max_escape_size = 10;
    else if(quantStepSize < 55) pWmaDec->max_escape_size =  9;
    else                        pWmaDec->max_escape_size =  9;

    pWmaDec->max_escape_level = (1<<pWmaDec->max_escape_size) - 1;
}

static int32 WmaGetSamplePerFrame (int32   samplePerSec,
                              uint32   bitPerSec,
                              int32   num_channels,
                              int32   version)
{
    //3.1
    int32 cSamplePerFrame;

	assert (version == 2);

	if (samplePerSec <= 16000)
        cSamplePerFrame = 512;
    else if (samplePerSec <= 22050)
        cSamplePerFrame = 1024;
    else if (samplePerSec <= 32000) {
        if(version == 1)
            cSamplePerFrame = 1024;
        else
            cSamplePerFrame = 2048;
    }
    else if (samplePerSec <= 48000)
        cSamplePerFrame = 2048;
    else {
		assert (0);
        return 0;
	}

    return cSamplePerFrame;
}

static int32 WmaPrvAllocate (WmaObject* pWmaDec)
{
    int32   ret = 0, size;

	/// FIXME no need in embedded!
	size = sizeof (int32) * pWmaDec->subBand *3 /2;
	pWmaDec->currOutput[0] = (int32*) WMA_ALLOC (size); assert (pWmaDec->currOutput[0]);
	pWmaDec->currOutput[1] = (int32*) WMA_ALLOC (size); assert (pWmaDec->currOutput[1]);

	size = sizeof (int32) * pWmaDec->subBand ;
	pWmaDec->pingPrevOutput [0] = (int32*) WMA_ALLOC (size); assert (pWmaDec->pingPrevOutput[0]);
	pWmaDec->pingPrevOutput [1] = (int32*) WMA_ALLOC (size); assert (pWmaDec->pingPrevOutput[1]);
	pWmaDec->pongPrevOutput [0] = (int32*) WMA_ALLOC (size); assert (pWmaDec->pongPrevOutput[0]);
	pWmaDec->pongPrevOutput [1] = (int32*) WMA_ALLOC (size); assert (pWmaDec->pongPrevOutput[1]);
	memset (pWmaDec->pingPrevOutput [0], 0, size);
	memset (pWmaDec->pingPrevOutput [1], 0, size);
	memset (pWmaDec->pongPrevOutput [0], 0, size);
	memset (pWmaDec->pongPrevOutput [1], 0, size);

	/// SW huff decoding, no flush needed
	size = sizeof (int32) * pWmaDec->subBand;
    pWmaDec->pCoefRecon[0] = (int32*) WMA_ALLOC (size); assert (pWmaDec->pCoefRecon[0]);
	pWmaDec->pCoefRecon[1] = (int32*) WMA_ALLOC (size); assert (pWmaDec->pCoefRecon[1]);

	/// FIXME may add qualifier
	pWmaDec->pCoefLowMidRate = (int32*) WMA_ALLOC (size); assert (pWmaDec->pCoefLowMidRate);

	if (pWmaDec->weightingMode == LPC_MODE)	{
		int32 ch;
	
		pWmaDec->pWeightFactor = WMA_ALLOC (sizeof (uint32) * pWmaDec->subBand * pWmaDec->num_channels); assert (pWmaDec->pWeightFactor);		

		for (ch=0; ch< pWmaDec->num_channels; ch++ ) {
			PerChannelInfo *pChnl = &pWmaDec->channelInfo[ch];
			pChnl->pWeightFactor = pWmaDec->pWeightFactor + pWmaDec->subBand*ch;

			pChnl->maxWeight = 0; // add by tcx 2008.04.17
		}
	}
	else { // add by tcx 2008.04.17
		int32 ch;
		for (ch=0; ch< pWmaDec->num_channels; ch++ ) {
			PerChannelInfo *pChnl = &pWmaDec->channelInfo[ch];
			pChnl->pWeightFactor = (uint*)pChnl->barkMaskQ;
			pChnl->maxWeight = 0;
		}
	}
    return ret;
}

static int32 WmaCalcQuantStep(int32 quant_step, FastFloat *pQuantStep)
{
    if ( quant_step < DBPOWER_TABLE_OFFSET ) {
		//assert (0);
		// Wei Zhong, 22 is the least accoriding to MSFT
#if 1
        if ( quant_step < 0 )
        {   // negative values of iQSS are being generated in the V5 encoder (LowRate in particular)
            //pQuantStep->iFraction = (int32)(0.382943866392*(1<<QUANTSTEP_FRACT_BITS)),      // Average Fraction
			pQuantStep->iFraction = 0x62089bf;
            pQuantStep->iFracBits = 27 - ((-quant_step>>3));                // Approximate Exponent
        }
        else
        {
            //pQuantStep->iFraction = (int32)(0.869439785679*(1<<QUANTSTEP_FRACT_BITS));     // Average Fraction
			pQuantStep->iFraction = 0xde939b1;
            pQuantStep->iFracBits = 27 - ((quant_step>>3));                // Approximate Exponent
        }
#endif
    }
    else if ( quant_step < (DBPOWER_TABLE_OFFSET+DBPOWER_TABLE_SIZE) ) {
        // *** normal case ***
        pQuantStep->iFraction = rgDBPower10[ quant_step - DBPOWER_TABLE_OFFSET ];
        pQuantStep->iFracBits = QUANTSTEP_FRACT_BITS - ((quant_step>>3)+4);    // implied FractBit scale for rgiDBPower table
        NormU32( (uint32*)(&pQuantStep->iFraction), &pQuantStep->iFracBits, 0x3FFFFFFF );
    }
    else {
		assert (0); // wei zhong
	}
    return 0;
}

static int32 WmaWeightedQuantization(WmaObject *pWmaDec, int32 ch)
{
	PerChannelInfo* pChnlInfo = &pWmaDec->channelInfo[ch];
	int32 band;

	for (band = 0; band < 25/*pWmaDec->barkInfo[pWmaDec->ratio_log2_curr].num_bark_bands*/; band ++) {
		/*
		int32 iIndex = pChnlInfo->maxMaskQ - pChnlInfo->barkMaskQ[band].maskQ;
		FastFloat *pMaskQ = &pChnlInfo->barkMaskQ[band].fMaskQ;
		*/ // modify by tcx 2008.04.17
		int32 iIndex = pChnlInfo->maxMaskQ - pChnlInfo->barkMaskQ[band];
		FastFloat *pMaskQ = &pChnlInfo->fbarkMaskQ[band];

		assert( 0<=iIndex );

		if (iIndex >= MASK_MINUS_POWER_TABLE_SIZE)
			iIndex = MASK_MINUS_POWER_TABLE_SIZE-1;
		
        {
			int32 uiFraction, iFracBits;
			uiFraction = rgiMaskMinusPower10[ iIndex ];     // with MASK_POWER_FRAC_BITS==28 fractional bits
			iFracBits = MASK_POWER_FRAC_BITS+(iIndex>>2);

			// 11/14/06 Wei Zhong, both are positive
			pMaskQ->iFraction = ( MULT_HI_UDWORD(pWmaDec->fQuantStep.iFraction,uiFraction)) << 1;
			pMaskQ->iFracBits = pWmaDec->fQuantStep.iFracBits + iFracBits - 31;

			Norm4FastFloat( pMaskQ );
		}
    }

    return 0;
}

void NormU32( uint32* puiValue, uint32* pcFracBits, const uint32 uiTarget )
{
    const uint32 uiTarget2 = uiTarget>>1;
    register uint32 uiV = *puiValue;
    register uint32 cFB = *pcFracBits;

	assert (uiTarget == 0x3fffffff);	// wzhong 11/09/07

    assert( uiV > 0 );
    if (uiV == 0)
		return; // useful if asserts are disabled

    while ( uiV < uiTarget2 ) {
        uiV <<= 2;
        cFB += 2;
    }
    if ( uiV < uiTarget ) {
        uiV <<= 1;
        cFB += 1;
    }
    *puiValue = uiV;
    *pcFracBits = cFB;
}

void Norm4FastFloat( FastFloat* pfflt )
{   // use the faster Norm4FastfloatU when you know the value is positive
	register uint32 uiF = abs(pfflt->iFraction);
	register uint32 iFB = 0;
    
	if ( uiF == 0 ) {
		pfflt->iFracBits = 0;
		return;
	}
	while ( uiF < 0x1FFFFFFF ) {
		uiF <<= 2;
		iFB +=  2;
	}
	if ( uiF < 0x3FFFFFFF ) {
		iFB +=  1;
	}
	pfflt->iFraction <<= iFB;
	pfflt->iFracBits += iFB;
}

//3.9 Parameters for Noise Substitution
static int32 WmaInitNoiseSub (WmaObject *pWmaDec)
{
    int32 iWin, iBand;
    float fltSamplingPeriod;

    int32 *piBarkIndex;
    int32 cFrameSample;
    int32 iNoiseIndex;

    pWmaDec->noiseSub = 1;
    pWmaDec->firstNoiseFreq = (float)(pWmaDec->samples_per_second*0.5f);
    
	if(pWmaDec->version_number == 1) 
	{
        // version 1 was incorrectly using the inequalities...
        // do not change!!!
		assert (0);
        if(pWmaDec->samples_per_second == 22050) 
		{
            // somewhat different parameters...
            if (pWmaDec->fltWeightedBitsPerSample >= 1.16f)
                pWmaDec->noiseSub = 0;
            else if(pWmaDec->fltWeightedBitsPerSample >= 0.72f)
                pWmaDec->firstNoiseFreq *= (float)0.70;
            else
                pWmaDec->firstNoiseFreq *= (float)0.60;
        }
        else if (pWmaDec->samples_per_second == 44100) 
		{
            if (pWmaDec->fltWeightedBitsPerSample >= 0.61f)
                pWmaDec->noiseSub = 0;
            else
                pWmaDec->firstNoiseFreq *= (float) 0.4;
        }
        else if (pWmaDec->samples_per_second == 16000) 
		{
            if (pWmaDec->fltBitsPerSample <= 0.5f)
                pWmaDec->firstNoiseFreq *= (float) 0.30;
            else {
                pWmaDec->firstNoiseFreq *= (float) 0.50;
                pWmaDec->noisePeakIgnoreBand = 3;
            }
        }
        else if (pWmaDec->samples_per_second == 11025) 
		{
            assert(1 == pWmaDec->noisePeakIgnoreBand);
            pWmaDec->firstNoiseFreq *= (float) 0.70;
            if (pWmaDec->fltBitsPerSample >= 0.9f)
                pWmaDec->noisePeakIgnoreBand = 3;
        }
        else if (pWmaDec->samples_per_second == 8000) 
		{
            assert(1 == pWmaDec->noisePeakIgnoreBand);
            if (pWmaDec->fltBitsPerSample <=0.625f)
                pWmaDec->firstNoiseFreq *= (float) 0.50;
            else if (pWmaDec->fltBitsPerSample <= 0.75f)
                pWmaDec->firstNoiseFreq *= (float) 0.65;
            else
                pWmaDec->noiseSub = 0;
        }
        else 
		{
            if(pWmaDec->fltBitsPerSample >= 0.8f)
                pWmaDec->firstNoiseFreq *= (float)0.75;
            else if(pWmaDec->fltBitsPerSample >= 0.6f)
                pWmaDec->firstNoiseFreq *= (float)0.60;
            else
                pWmaDec->firstNoiseFreq *= (float)0.5;
        }
    }
    else 
	{
        if (pWmaDec->samples_per_second >= 44100) 
		{
            if (pWmaDec->fltWeightedBitsPerSample >= 0.61f)
                pWmaDec->noiseSub = 0;
            else
                pWmaDec->firstNoiseFreq *= (float) 0.4;
        }
        else if(pWmaDec->samples_per_second >= 22050) 
		{        
            if (pWmaDec->fltWeightedBitsPerSample >= 1.16f)
                pWmaDec->noiseSub = 0;
            else if(pWmaDec->fltWeightedBitsPerSample >= 0.72f)
                pWmaDec->firstNoiseFreq *= (float)0.70;
            else
                pWmaDec->firstNoiseFreq *= (float)0.60;
        }
        else if (pWmaDec->samples_per_second >= 16000) 
		{
            if (pWmaDec->fltBitsPerSample <= 0.5f)
                pWmaDec->firstNoiseFreq *= (float) 0.30;
            else 
			{
                pWmaDec->firstNoiseFreq *= (float) 0.50;
                pWmaDec->noisePeakIgnoreBand = 3;
            }
        }
        else if (pWmaDec->samples_per_second >= 11025) 
		{
            assert(1 == pWmaDec->noisePeakIgnoreBand);
            pWmaDec->firstNoiseFreq *= (float) 0.70;
            if (pWmaDec->fltBitsPerSample >= 0.9f)
                pWmaDec->noisePeakIgnoreBand = 3;
        }
        else if (pWmaDec->samples_per_second >= 8000) 
		{
            assert(1 == pWmaDec->noisePeakIgnoreBand);
            if (pWmaDec->fltBitsPerSample <=0.625f)
                pWmaDec->firstNoiseFreq *= (float) 0.50;
            else if (pWmaDec->fltBitsPerSample <= 0.75f)
                pWmaDec->firstNoiseFreq *= (float) 0.65;
            else
                pWmaDec->noiseSub = 0;
        }
        else 
		{
            if(pWmaDec->fltBitsPerSample >= 0.8f)
                pWmaDec->firstNoiseFreq *= (float)0.75;
            else if(pWmaDec->fltBitsPerSample >= 0.6f)
                pWmaDec->firstNoiseFreq *= (float)0.60;
            else
                pWmaDec->firstNoiseFreq *= (float)0.5;
        }
    }

	if (!pWmaDec->noiseSub)
        return OKAY;
    
    //calculate index of each bark freq
    fltSamplingPeriod = 1.0F / pWmaDec->samples_per_second;
    
    // for the v1 compatibility
    if(pWmaDec->version_number == 1) {
		assert (0);
    }
    else
    {
        for (iWin = 0; iWin < pWmaDec->possibleWinSize; iWin++)    
		{
            // precalculate the first noise bands
            pWmaDec->firstNoiseBand[iWin] = pWmaDec->barkInfo[iWin].num_bark_bands - 1;// init to max...
            cFrameSample = pWmaDec->frameSample / (1 << iWin);
            iNoiseIndex = (int32)(pWmaDec->firstNoiseFreq*cFrameSample*fltSamplingPeriod + 0.5f);
            for(iBand = 1; iBand < pWmaDec->barkInfo[iWin].num_bark_bands; iBand++) 
			{
                if(pWmaDec->barkInfo[iWin].bark_index[iBand] > iNoiseIndex) 
				{
                    pWmaDec->firstNoiseBand[iWin] = iBand - 1;
                    break;
                }
            }
            piBarkIndex +=  NUM_BARK_BAND + 1;
        }
    }
    
    return OKAY;
}

static int32 WmaDecodeFrameHeaderNoiseSub (WmaObject*	pWmaDec)
{    
    int32 iStart, iEnd, iBand;
    int8 ch = 0;

    //noise band indicator
    for (ch = 0; ch < pWmaDec->num_channels; ch ++)   
	{
        PerChannelInfo* pChnl = &pWmaDec->channelInfo [ch];
        
        if (pChnl->power != 0) 
		{
			BarkIndex *pBarkIndex = &pWmaDec->barkInfo[pWmaDec->ratio_log2_curr];            
            
			pChnl->numBandNotCoded = 0;
            pChnl->cSubbandActual = pWmaDec->firstNoiseIndex - pWmaDec->lowCutOff;

            iStart = 0, iEnd = 0, iBand = pWmaDec->currFirstNoiseBand;
            while (1) 
			{
                iStart = max2 (pWmaDec->firstNoiseIndex, pBarkIndex->bark_index[iBand]);
                if (iStart >= pWmaDec->highCutOff)
                    break;

				assert (iBand < pBarkIndex->num_bark_bands);

                iEnd   = min2 (pWmaDec->highCutOff, pBarkIndex->bark_index [iBand + 1]);

                pChnl->pBandNotCoded[iBand] = pWmaDec->getBits (1);

				if ( pChnl->pBandNotCoded[iBand] )
					pChnl->numBandNotCoded ++;
                else 
                    pChnl->cSubbandActual += iEnd - iStart;
                iBand++;
			}
			if (iBand < pBarkIndex->num_bark_bands ) 
				pChnl->pBandNotCoded[iBand] = 0;		// protect from debug verify code looking one band past end  
        }
        else {
            pChnl->cSubbandActual = 0;
			pChnl->numBandNotCoded = 0;
		}
        assert ( pChnl->cSubbandActual <= pWmaDec->highCutOff - pWmaDec->lowCutOff);        
    }
    
    //noise power
    for (ch = 0; ch < pWmaDec->num_channels; ch++)  
	{
		uint32 val;
        PerChannelInfo* pChnl = &pWmaDec->channelInfo [ch];
        if (pChnl->power != 0) {
			if (pChnl->numBandNotCoded){				
				pChnl->pNoisePower[0] = pWmaDec->getBits (7) - 19;
								
				for (iBand = 1; iBand < pChnl->numBandNotCoded; iBand++)	
				{
					WmaDecHuf (pWmaDec, g_rgiHuffDecTblNoisePower, &val);
                    pChnl->pNoisePower[iBand] = val - 18 + pChnl->pNoisePower[iBand-1];						
				}
			}
        }        
    }
	return OKAY;
}

#define INV_MAX_WEIGHT_FRAC_BITS 30
#define MORE_WF_FRAC_BITS 0

static uint32 uiInverseMaxWeight(uint32 wtMaxWeight)
{   
    uint32 uiMSF = wtMaxWeight;
    uint32 iExp = (32-INV_MAX_WEIGHT_FRAC_BITS)+(32-WEIGHTFACTOR_FRACT_BITS);  // (32-30)+(32-21)=13
    int32 iMSF8;
    uint32 uiFrac1;
    if ( uiMSF < ((uint32)(1<<((32-INV_MAX_WEIGHT_FRAC_BITS)+(32-WEIGHTFACTOR_FRACT_BITS)))) ) 
        return 0xFFFFFFFF ;
    // normalize the fractional part
    while( (uiMSF & 0xF0000000)==0 ) 
	{
        iExp -= 4;
        uiMSF <<= 4;
    }
    while( (uiMSF & 0x80000000)==0 ) 
	{
        iExp--;
        uiMSF <<= 1;
    }
    // discard the most significant one bit (it's presence is built into g_InverseFraction)
    iExp--;
    uiMSF <<= 1;
    assert( iExp >= 0 );
    if ( iExp < 0 )
        return 0xFFFFFFFF ;
    
	// split into top INVERSE_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
    iMSF8 = uiMSF>>(32-INVERSE_FRACTION_TABLE_LOG2_SIZE);
    uiMSF <<= INVERSE_FRACTION_TABLE_LOG2_SIZE;
    // lookup and interpolate - tables are set up to return correct binary point for uint32 (we hope)
    uiFrac1  = g_InverseFraction[iMSF8++];
    uiFrac1 -= MULT_HI_UDWORD( uiMSF, uiFrac1 - g_InverseFraction[iMSF8] );
    return uiFrac1 >> iExp;
}

int32 quickRand(tRandState* ptRandState)
{
    const uint32 a = 0x19660d;
    const uint32 c = 0x3c6ef35f;
	uint32 halfRand;

    int32 iTemp, iTemp1;
    //a*x + c has to be done with unsigned 32 bit

	halfRand = ptRandState->uRand >> 1;

	/// wzhong 11/08/07, adjust to 31 bit unsigned mul
    //ptRandState->uRand =  a * ptRandState->uRand + c;
	if (ptRandState->uRand & 1) 
		ptRandState->uRand =  a * halfRand * 2 + a+ c;
	else
		ptRandState->uRand =  a * halfRand * 2 + c;

    // uiRand values starting from a 0 seed are: 0x3c6ef35f, 0x47502932, 0xd1ccf6e9, 0xaaf95334, 0x6252e503, 0x9f2ec686, 0x57fe6c2d, ...

    // do not change the above - this reference generator has been extensively tested and has excellent randomness properties
    // a truism in the world of random number generator theory and practice is:
    // "any change, no matter how small, can and will change the properties of the generator and must be fully tested"
    // In case you don't know, it can easily take a person-month to fully test a generator.

    // Notwithstanding the above, it is fair to take a function of a random number to shape its range or distribution.
    // This we do below to give it a triangular distrbution between -2.5 and 2.5 to roughly approximate a Guassian distribution.

    // cast and shift to make the range (-1, 1) with Binary Point 3.
    iTemp = ((int32)(ptRandState->uRand)) >> 2; 

    // *1.25 to make the range (-1.25, 1.25) 
    iTemp += ((int32)(ptRandState->uRand)) >> 4;

    // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
    // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
    iTemp1 = iTemp - ptRandState->iPrior;

    // Save first term of this difference for next time.
    ptRandState->iPrior = iTemp;

    //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution	
    return iTemp1;
}

static int32 WmaInverseQuantizeLowRate (WmaObject*	pWmaDec, uint8 ch)
{
	PerChannelInfo *pChnlInfo =  &pWmaDec->channelInfo[ch];
    uint32 uiMaxWeight, cMaxWeightFracBits, uiQuantStepXInvMaxWeight;

	int32 iRecon = 0;
    uint32 uiWeightFactor;
    int32 QuantStepXMaxWeightXWeightFactor, qrand, iNoise;
	int32 cFracBits, cFracBits2, cWFFracBits;

    uint8  cNoiseBand;
    int32 cQSIMWFracBits;
    
	const int32 iDitherFactor = 0x51EB851F;		// LPC  0.04 * 2^35 
	uint32 uiInvMaxWeight = uiInverseMaxWeight (pChnlInfo->maxWeight);
	BarkIndex *pBarkIndex  = &pWmaDec->barkInfo[pWmaDec->ratio_log2_curr];
	
//	tRandState saveRand = pWmaDec->randState;
	/// wzhong 11/08/07
	/// array use DSP flavor increment addressing

	uint16 *pBark = &pBarkIndex->bark_index [1];			// AR1

	uint8  *pBarkNotCoded = pChnlInfo->pBandNotCoded;		// AR2
	int32 *pCoefQ = pWmaDec->pCoefLowMidRate;				// AR3
	uint32 *pWeight = pChnlInfo->pWeightFactor;				// AR4
	int32 *pRecon = pChnlInfo->pCoefRecon;					// AR5
	int8 *pNoisePower = pChnlInfo->pNoisePower;				// AR6
	FastFloat *pBWRatio = pChnlInfo->pffltSqrtBWRatio;		// AR&

	int32 toDump = 0;
	static int32 cnt = 0;

	if ( (cnt==0) && (pWmaDec->currFrame==0x00) && (pWmaDec->currSubFrame==0)) {
		toDump = 1;
		cnt ++;
	}
	else
		toDump = 0;


	//Calculate QuantStep X invMaxWeight
    cQSIMWFracBits  = pWmaDec->fQuantStep.iFracBits;                           
    uiQuantStepXInvMaxWeight = MULT_HI_DWORD(pWmaDec->fQuantStep.iFraction,uiInvMaxWeight);
	cQSIMWFracBits += (INV_MAX_WEIGHT_FRAC_BITS-32);
	// float QSIMWF = uiQuantStepXInvMaxWeight*1.0F/(1<<cQSIMWFracBits)
	NormU32( &uiQuantStepXInvMaxWeight, &cQSIMWFracBits, 0x3FFFFFFF );

	// since all weights are less than MaxWeight, fast scale below by MaxWeight's FracBits
	uiMaxWeight = pChnlInfo->maxWeight<<MORE_WF_FRAC_BITS;;
	cMaxWeightFracBits = MORE_WF_FRAC_BITS;		// really should be WEIGHTFACTOR_FRACT_BITS+MORE_WF_FRAC_BITS but this way is for shift delta

	NormU32( &uiMaxWeight, &cMaxWeightFracBits, 0x3FFFFFFF );

    if (!pWmaDec->noiseSub) {
		assert (0);
		return 0;
    }

    cNoiseBand = pChnlInfo->numBandNotCoded;

	/// after this Aura

	if (iRecon < pWmaDec->lowCutOff ) 
	{
		/*
		// not integerized since cLowCutOff is typically 0, so this is here for compatability with V1 
		double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pWmaDec->fQuantStep);
		while  (iRecon < pWmaDec->lowCutOff) 	{
			float fltNoise = pWmaDec->fltDitherLevel * ((float) quickRand (&(pWmaDec->randState)) / (float) 0x20000000);//rgfltNoise [iRecon];
			float fltWeightFactor = ((float)rgiWeightFactor [pWmaDec->lowCutOff])/(1<<WEIGHTFACTOR_FRACT_BITS);
			// SH4 warning CBE4717 on the next line is ignorable - appraently a compiler mistake
			fltCoefRecon  = (float) (fltNoise * fltWeightFactor * dblQuantStep * (float)(1<<WEIGHTFACTOR_FRACT_BITS)/((float)pChnlInfo->maxWeight) );
			rgiCoefRecon [iRecon] = (int32)(fltCoefRecon * (1<<TRANSFORM_FRACT_BITS));
						
			iRecon++;
		}*/

	}

	assert (iRecon == 0);
	assert (pWmaDec->firstNoiseIndex);

	// wzhong, 11/09/08
	do //while  (iRecon < pWmaDec->firstNoiseIndex) 
	{
        int iCoef,iCoefScaled,iCoefRecon,iNoiseScaled,iNoiseQuant;         
        assert(TRANSFORM_FRACT_BITS==5);
        
		if (iRecon >= *pBark) {			
			pBark ++;
			pBarkNotCoded ++;
		}

	    // Since weight factors became unsigned, the following assert is not
	    // valid. Other wrap-around detection would have to be performed elsewhere.
		assert( *pWeight <= (uint32)(0x7FFFFFFF>>cMaxWeightFracBits) );
        uiWeightFactor = *pWeight++ << cMaxWeightFracBits; 
		//assert ((pWeight ++) == &rgiWeightFactor [iRecon]);

        cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
		//// float WeightFactor = uiWeightFactor/(1.0F*(1<<cWFFracBits))
	    // Since weight factors became unsigned, the following assert is not
	    // valid. Other wrap-around detection would have to be performed elsewhere.
	    assert( uiWeightFactor <= 0x7FFFFFFF );
		NormU32( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		// weightFactor with cWFFracBits fractional bits
		        
        QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);		
        cFracBits = cQSIMWFracBits+cWFFracBits-31 - 11;
        		
        iCoef = MULT_HI(((int32)(*pCoefQ ++)<<20),QuantStepXMaxWeightXWeightFactor);
				
        //Rescale to TRANSFORM_FRACT_BITS for outputing to the inverse transform so that (float)iCoefScaled/(1<<TRANSFORM_FRACT_BITS)
		iCoefScaled = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );
		//// float CoefScaled = iCoefScaled/32.0F

		qrand = quickRand (&(pWmaDec->randState));
        iNoise = MULT_HI(iDitherFactor, qrand);
		
		iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);

		cFracBits2 = 35+29-31;											// == 33
		cFracBits2 += ((cQSIMWFracBits+cWFFracBits-31)-31);		
		
		assert( (cFracBits+(33-20)) == cFracBits2 );

        //rescale iNoiseQuant so that (float)iNoiseScaled/(1<<TRANSFORM_FRACT_BITS)
		iNoiseScaled = Align2FracBits( iNoiseQuant, cFracBits2, TRANSFORM_FRACT_BITS );
		
        iCoefRecon = iCoefScaled + iNoiseScaled;
        
		*pRecon ++ = iCoefRecon;
		assert (pWeight-pChnlInfo->pWeightFactor == pRecon-pChnlInfo->pCoefRecon); // step in tandem
		iRecon ++;		
    }
	while  (iRecon < pWmaDec->firstNoiseIndex); // wzhong 11/09/07

	assert (iRecon < pWmaDec->highCutOff);
    do //while  (iRecon < pWmaDec->highCutOff) // wzhong 11/09/07
	{
		if (iRecon >= *pBark) {			
			pBark++;
			pBarkNotCoded ++;
		}
        
		if (*pBarkNotCoded == 1)
        {
			FastFloat ffltNoisePower;
            uint32 uiNoisePowerXinvMaxWeight;
			int32 iUBLimitOniRecon = min2 (*pBark, pWmaDec->highCutOff);

			WmaCalcQuantStep( *pNoisePower++, &ffltNoisePower);
			//// float Noise Power = ffltNoisePower.iFraction/(1.0F*(1<<ffltNoisePower.iFracBits))

			ffltNoisePower = ffltMultiply( ffltNoisePower, *pBWRatio++ );
			//// float Noise Power = ffltNoisePower.iFraction/(1.0F*(1<<ffltNoisePower.iFracBits))

            uiNoisePowerXinvMaxWeight = MULT_HI(ffltNoisePower.iFraction, uiInvMaxWeight>>1)<<1;
			cFracBits = ffltNoisePower.iFracBits + (INV_MAX_WEIGHT_FRAC_BITS-31);
			//// float NoisePower/MaxWeight = uiNoisePowerXinvMaxWeight/(1.0F*(1<<cFracBits))
			NormU32( &uiNoisePowerXinvMaxWeight, &cFracBits, 0x3FFFFFFF );

			assert (iRecon< iUBLimitOniRecon);
            do //while (iRecon < iUBLimitOniRecon)
            {
                int32 iNoiseRand,iNoiseWeighted,iCoefRecon;
                
				assert( *pWeight <= (uint32)(0x7FFFFFFF>>cMaxWeightFracBits) );
				uiWeightFactor = *pWeight++ << cMaxWeightFracBits; 
				//assert ((pWeight ++) == rgiWeightFactor [iRecon]);
				cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
				
				assert( uiWeightFactor <= 0x7FFFFFFF );
				NormU32( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		// uiWeightFactor with cWFFracBits fractional bits

				qrand = quickRand (&(pWmaDec->randState));										// FB = 29												
                iNoiseRand = MULT_HI(uiNoisePowerXinvMaxWeight,qrand);
                iNoiseWeighted = MULT_HI(iNoiseRand,uiWeightFactor);	
				
				cFracBits2 = cFracBits-2;
				cFracBits2 += (cWFFracBits-31);
				
				iCoefRecon = Align2FracBits( iNoiseWeighted, cFracBits2, TRANSFORM_FRACT_BITS );  //scale so that (float)iCoefRecon/(1<<TRANSFORM_FRACT_BITS)
                *pRecon ++ = iCoefRecon;
				assert (pWeight-pChnlInfo->pWeightFactor == pRecon-pChnlInfo->pCoefRecon); // step in tandem
				iRecon ++;				
			}
			while (iRecon < iUBLimitOniRecon); // wzhong 110907
		}
		else 
		{		
            int32 iCoef,iNoiseQuant,iCoefScaled,iCoefRecon;
			//assert (0);	// wzhong, lpc.wma frame 1, subframe 1

            if (iRecon >= *pBark) {
				assert (0);
				pBark ++;
				pBarkNotCoded ++;
			}
            
			assert( *pWeight <= (uint32) (0x7FFFFFFF>>cMaxWeightFracBits) );
			uiWeightFactor = *pWeight ++ << cMaxWeightFracBits; 
			
			cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
			
			assert( uiWeightFactor <= 0x7FFFFFFF );
			NormU32( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		// uiWeightFactor with cWFFracBits fractional bits
			
            QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);
            cFracBits = cQSIMWFracBits+cWFFracBits-31;
			
			qrand = quickRand (&(pWmaDec->randState));
            iNoise = MULT_HI(iDitherFactor,qrand); 
			cFracBits2 = 35+29-31;							// FP = 33
			
            iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);
			cFracBits2 += (cFracBits-31);

            iCoef = MULT_HI( ((int32) *pCoefQ ++)<<20, QuantStepXMaxWeightXWeightFactor );
			cFracBits += (20-31);

			iCoefScaled = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );  

            assert(cFracBits2>=TRANSFORM_FRACT_BITS);
            iCoefRecon = iCoefScaled + (iNoiseQuant>>(cFracBits2-TRANSFORM_FRACT_BITS));
            *pRecon ++ = iCoefRecon;
			assert (pWeight-pChnlInfo->pWeightFactor == pRecon-pChnlInfo->pCoefRecon); // step in tandem
			iRecon ++;			
		}
	}
	while  (iRecon < pWmaDec->highCutOff) ; // wzhong 11/09/07

	//Calculate from highCutOff to m_cSubband
	{	
		uint32 QuantStepXMaxWeightXWeightFactorXDither;
		int32 i;
		
	    assert( pChnlInfo->pWeightFactor [pWmaDec->highCutOff - 1] <= (uint32) (0x7FFFFFFF>>cMaxWeightFracBits) );
		uiWeightFactor = pChnlInfo->pWeightFactor [pWmaDec->highCutOff - 1] << MORE_WF_FRAC_BITS;
		cWFFracBits = WEIGHTFACTOR_FRACT_BITS+MORE_WF_FRAC_BITS;
		//// float WeightFactor = uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))
		NormU32( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		

		QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);
		cFracBits = cQSIMWFracBits+cWFFracBits-31;
		//// float QuantStep*WeightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1024.0F*(1<<(cFracBits-10)))
		NormU32( (uint32 *)&QuantStepXMaxWeightXWeightFactor, &cFracBits, 0x3FFFFFFF );

		QuantStepXMaxWeightXWeightFactorXDither = MULT_HI(QuantStepXMaxWeightXWeightFactor,iDitherFactor);
		cFracBits += 4;
		//// float QS * WF/MaxWF * Dither = QuantStepXMaxWeightXWeightFactorXDither/(1024.0F*(1<<(cFracBits-10)))
		NormU32( &QuantStepXMaxWeightXWeightFactorXDither, &cFracBits, 0x3FFFFFFF );

		///while (iRecon < pWmaDec->cSubBand)
		assert (pWmaDec->cSubBand > iRecon);
		cFracBits -= 2;
		for (i=0; i< pWmaDec->cSubBand-iRecon; i++) {
			int32 iCoefScaled;
			int32 iCoefRecon;

			qrand = quickRand (&(pWmaDec->randState));
			iCoefRecon = MULT_HI(QuantStepXMaxWeightXWeightFactorXDither, qrand);
						
			iCoefScaled = Align2FracBits( iCoefRecon, cFracBits, TRANSFORM_FRACT_BITS );
			*pRecon ++ = iCoefScaled;							
		}
	}

#if 0
	{		
		static FILE *fpIQInput = 0,  *fpBark=0, *fpBarkNotCoded, *fpWeight, *fpCoefQ, *fpRecon, *fpNoisePower, *fpBWRatio, *fpDBPower10;
		if (toDump) {
			int i;

			pBarkNotCoded = pChnlInfo->pBandNotCoded;		// AR2
			pCoefQ = pWmaDec->pCoefLowMidRate;				// AR3
			pWeight = pChnlInfo->pWeightFactor;				// AR4
			pRecon = pChnlInfo->pCoefRecon;					// AR5
			pNoisePower = pChnlInfo->pNoisePower;				// AR6
			pBWRatio = pChnlInfo->pffltSqrtBWRatio;		// AR&
			
			fpIQInput = fopen ("iqinput.txt", "w");
			fprintf (fpIQInput, "%08x	//uiQuantStepXInvMaxWeight\n", uiQuantStepXInvMaxWeight);	/// const
			fprintf (fpIQInput, "%08x	// cQSIMWFracBits\n", cQSIMWFracBits);
			fprintf (fpIQInput, "%08x	// cMaxWeightFracBits\n", cMaxWeightFracBits);
			fprintf (fpIQInput, "%08x	//iDitherFactor\n", iDitherFactor);
			fprintf (fpIQInput, "%08x\n", 1664525);
			fprintf (fpIQInput, "%08x\n", 1013904223);
			fprintf (fpIQInput, "%08x	// iPrior\n", saveRand.iPrior);
			fprintf (fpIQInput, "%08x	// uRand\n", saveRand.uRand);
			fprintf (fpIQInput, "0x3fffffff\n");
			fprintf (fpIQInput, "0x1fffffff\n");
			fprintf (fpIQInput, "0x62089bf\n");
			fprintf (fpIQInput, "0xde939b1\n");
			fprintf (fpIQInput, "%08x	// uiInvMaxWeight\n", uiInvMaxWeight);
			fprintf (fpIQInput, "%08x	// pWeightFactor [highCutOff - 1]\n", pChnlInfo->pWeightFactor [pWmaDec->highCutOff - 1]);

			for (i=0; i<2; i++)
				fprintf (fpIQInput, "0\n");

			fprintf (fpIQInput, "%08x	//firstNoiseIndex\n", pWmaDec->firstNoiseIndex);
			fprintf (fpIQInput, "%08x	// highCutOff\n", pWmaDec->highCutOff);
			fprintf (fpIQInput, "%08x	// cSubBand\n", pWmaDec->cSubBand);
/*
			for (i=0; i<13; i++) {
				fprintf (fpIQInput, "0\n");
			}
			*/
			
			fpBark = fopen ("bark.txt", "w");
			for (i=0; i<25; i++){
				fprintf (fpBark, "%08x\n", pBarkIndex->bark_index[i]);
			}

			fpBarkNotCoded = fopen ("barknotcoded.txt", "w");
			for (i=0; i<25; i++){
				fprintf (fpBarkNotCoded, "%08x\n", pBarkNotCoded[i]);
			}

			fpWeight = fopen ("weight.txt", "w");
			for (i=0; i<pWmaDec->cSubBand; i++ ) {
				fprintf (fpWeight, "%08x\n", pWeight[i]);
			}
			
			fpRecon = fopen ("recon.txt", "w");
			for (i=0; i< pWmaDec->cSubBand; i++ ) {
				fprintf (fpRecon, "%08x\n", pRecon[i]);
			}

			fpCoefQ = fopen ("coefq.txt", "w");
			for (i=0; i< pWmaDec->cSubBand; i++ ) {
				fprintf (fpCoefQ, "%x\n", pCoefQ[i]);
			}

			fpNoisePower = fopen ("noisepower.txt", "w");
			for (i=0; i<25; i++) {
				fprintf (fpNoisePower, "%08x\n", pNoisePower[i]);
			}

			fpBWRatio = fopen ("bwratio.txt", "w");
			for (i=0; i<25; i++) {
				fprintf (fpBWRatio, "%08x\n", pBWRatio[i].iFraction);
				fprintf (fpBWRatio, "%08x\n", pBWRatio[i].iFracBits);
			}

			fpDBPower10 = fopen ("dbpower10.txt", "w");
			for (i=0; i<128; i++) {
				fprintf (fpDBPower10, "0x%08x,\n", rgDBPower10[i]); 
			}
			fclose (fpDBPower10);
			fclose (fpBark);
			fclose (fpIQInput);
			fclose (fpWeight);
			fclose (fpCoefQ);
			fclose (fpRecon);
			fclose (fpNoisePower);
			fclose (fpBWRatio);
			fclose (fpBarkNotCoded);			
		}		
	}
#endif
	return 0;
}


// **********************************************************************
// Macros for DecodeCoefsMidRate with combined INTEGER and INT_FLOAT code

#define MASKQ_RESAMPLE_OFFSET 6
typedef struct MaskResampleInfo
{
	int iMaskResampleRatio;
	int iMaskResampleRatioPow;
	int cValidBarkBandLatestUpdate;
} MaskResampleInfo;

static inline int iResampleIndex( int iRecon, const MaskResampleInfo MRI )
{	// return resampled linear index suitable for frame that was last updated with MaskQ
	// equvilent to iRecon shifted left or right by (MRI.iMaskResampleRatioPow-MASKQ_RESAMPLE_OFFSET)
	return ( (((iRecon) << MRI.iMaskResampleRatioPow) >> MASKQ_RESAMPLE_OFFSET) );
}
static inline int iUnResampleIndex( int iResampled, const MaskResampleInfo MRI )
{	// return the reverse of iResampleIndex()
	// that is, convert from resampled indexes of MaskQ or WeightFactor to natural index for the current frame
    if (MRI.iMaskResampleRatioPow > MASKQ_RESAMPLE_OFFSET){
		// Rounding for down shift 
		// Although V4 did not round when resampling, this inverse funtion needs to round ???
        return (iResampled + (1<<(MRI.iMaskResampleRatioPow-(MASKQ_RESAMPLE_OFFSET+1)))) >> (MRI.iMaskResampleRatioPow-MASKQ_RESAMPLE_OFFSET);
        // return iResampled >> (MRI.iMaskResampleRatioPow-MASKQ_RESAMPLE_OFFSET);
    } else {
        return iResampled << (MASKQ_RESAMPLE_OFFSET-MRI.iMaskResampleRatioPow);
    }
}


//*****************************************************************************************
// Look up (10^(1/16)) ^ iMaskQ
//*****************************************************************************************
static inline FastFloat ffltMaskPower10( int iMaskQ )
{	// (10^(1/16)) ^ iMaskQ
	FastFloat fflt;

    // There is, unfortunately, no easy way to keep this assert with our new
    // combined decoder/encoder build.
//#if !defined (ENCODER)
//  assert( -MASK_MINUS_POWER_TABLE_SIZE < iMaskQ && iMaskQ <= MASK_PLUS_POWER_TABLE_SIZE );
//#endif

    if ( iMaskQ <= 0 )
	{
		if (iMaskQ < -MASK_MINUS_POWER_TABLE_SIZE)
			iMaskQ = -(MASK_MINUS_POWER_TABLE_SIZE-1);
		fflt.iFraction = rgiMaskMinusPower10[ -iMaskQ ];	// with MASK_POWER_FRAC_BITS==28 fractional bits
		fflt.iFracBits = MASK_POWER_FRAC_BITS+(-iMaskQ>>2);
	}
	else
	{
		if (iMaskQ >= MASK_PLUS_POWER_TABLE_SIZE)
			iMaskQ = MASK_PLUS_POWER_TABLE_SIZE;
		fflt.iFraction = rgiMaskPlusPower10[ iMaskQ-1 ];	// with MASK_POWER_FRAC_BITS==28 fractional bits
		fflt.iFracBits = MASK_POWER_FRAC_BITS-(iMaskQ>>2);
	}
	return fflt;
}

static inline FastFloat FastFloatFromFloat(float flt) 
{
	FastFloat fflt;
	float fltScale = (float)(1<<(31-24));
	fflt.iFracBits = 24;

	while( flt < -fltScale || fltScale < flt )
	{
		fflt.iFracBits -= 1;
		fltScale *= 2;
	}

	fflt.iFraction = (int32)(flt*(1<<fflt.iFracBits));
	Norm4FastFloat( &fflt );
	return fflt;
}   

static inline float FloatFromFastFloat( FastFloat fflt )
{
	assert( 0<= fflt.iFracBits && fflt.iFracBits <= 50 );
	if ( fflt.iFracBits > 30 )
		return fflt.iFraction/(1048576.0F*(1<<(fflt.iFracBits-20)));
	else
		return fflt.iFraction/((float)(1<<fflt.iFracBits));
}


//******************************************************************************************
//******************************************************************************************
// prvScanForNextBarkIndex scans for the next resampled bark index and then returns its unresampled linear index
//   iRecon and iHighLimit are linear indexes in the current frames natural scale.
//   iBarkResampled is the bark index in the resampled bark scale (appropriate for the frame when the MaskQ's were last updated)
//   rgiBarkIndex is the resampled bark index scale from that last updated MaskQ subframe
static inline int prvScanForNextBarkIndex( const int iRecon, int* piBarkResampled, const int iHighLimit, 
										  const int16* const rgiBarkIndexResampled, const MaskResampleInfo MRI )
{   //Scan for the next resampled bark index
	int iNextBarkIndex, iBarkResampled, iReconResampled;
	iReconResampled = iResampleIndex(iRecon, MRI);

	while ( iReconResampled >= rgiBarkIndexResampled [*piBarkResampled+1]) 
		++(*piBarkResampled);

	iBarkResampled = *piBarkResampled;

	if ( iBarkResampled >= MRI.cValidBarkBandLatestUpdate )
	{
		assert( iBarkResampled < MRI.cValidBarkBandLatestUpdate );
		return( MRI.cValidBarkBandLatestUpdate );
	}
	
	iNextBarkIndex = iUnResampleIndex( rgiBarkIndexResampled [iBarkResampled + 1], MRI );
	if (iNextBarkIndex > iHighLimit) 
		iNextBarkIndex = iHighLimit;
	return iNextBarkIndex;
}

#if 0
//******************************************************************************************
//******************************************************************************************
// SCALE_COEF_RECON shifts CoefRecon to give it TRANSFORM_FRACT_BITS==5 fractional bits
	// When branching is not a high penaty activity, do it the simplier way
	//   iCoefRecon = (t=iFracBits-5)<0 ? iCoefRecon>>-t : iCoefRecon<<t
	// This SCALE_COEF_RECON requires 3 ops plus 1 branch or 2 ops plus 1 branch.  
	// This SETUP_FOR_SCALE_COEF_RECON requires 2 ops
	// SCALE_COEFFICENT gets executed 25x as often as SETUP_FOR_SCALE_COEF_RECON, 
	// so this method requires 2.58 ops plus 0.04 branches per SCALE_COEFFICENT
	// On one test on a 500 MHz Pentium 686, this way saves 1% execution time over masking.
#	define SCALE_COEF_RECON(iCR) (iShift<0) ? (iCR)<<-iShift : (iCR)>>iShift
//#	define SETUP_FOR_SCALE_COEF_RECON(iFB) iShift=iFB-TRANSFORM_FRACT_BITS
// This more complex setup (with pre-normalization) is required to deal with 56_WMAv2.wma which ends
// with 1.5 seconds of DC bias at -890.  This results in a single large coef at 0 and the rest 0.
// Then in the noise band, iShift tries to be == 33...
#	define SETUP_FOR_SCALE_COEF_RECON(fftQ) \
    while( fftQ.iFracBits > (30+TRANSFORM_FRACT_BITS) ) { fftQ.iFracBits--; fftQ.iFraction>>=1; } \
    iShift=fftQ.iFracBits-TRANSFORM_FRACT_BITS;

#define UNNORMED_MULT(fflt,c,shift) \
	fflt.iFraction = MULT_HI( fflt.iFraction, c );	\
	fflt.iFracBits += shift;
#endif

static const int32 cDitherFactorMR = 0x28F5C28F;		// BARK 0.02 * 2^35 (but I am not sure how to derive the 35)

#define MASK_X_QUANT(iLevel,ffltQuantizer) MULT_HI(((iLevel)<<16),ffltQuantizer.iFraction)
#define RAND_X_QUANT(rnd,ffltQuantizer) MULT_HI((rnd),ffltQuantizer.iFraction)
#define UNNORMED_MULT(fflt,c,shift) \
	fflt.iFraction = MULT_HI( fflt.iFraction, c );	\
	fflt.iFracBits += shift;

// SCALE_COEF_RECON shifts CoefRecon to give it TRANSFORM_FRACT_BITS==5 fractional bits
	// When branching is not a high penaty activity, do it the simplier way
	//   iCoefRecon = (t=iFracBits-5)<0 ? iCoefRecon>>-t : iCoefRecon<<t
	// This SCALE_COEF_RECON requires 3 ops plus 1 branch or 2 ops plus 1 branch.  
	// This SETUP_FOR_SCALE_COEF_RECON requires 2 ops
	// SCALE_COEFFICENT gets executed 25x as often as SETUP_FOR_SCALE_COEF_RECON, 
	// so this method requires 2.58 ops plus 0.04 branches per SCALE_COEFFICENT
	// On one test on a 500 MHz Pentium 686, this way saves 1% execution time over masking.
//#	define SCALE_COEF_RECON(iCR) (iShift<0) ? (iCR)<<-iShift : (iCR)>>iShift

//#	define SETUP_FOR_SCALE_COEF_RECON(iFB) iShift=iFB-TRANSFORM_FRACT_BITS
// This more complex setup (with pre-normalization) is required to deal with 56_WMAv2.wma which ends
// with 1.5 seconds of DC bias at -890.  This results in a single large coef at 0 and the rest 0.
// Then in the noise band, iShift tries to be == 33...
#	define SETUP_FOR_SCALE_COEF_RECON(fftQ) \
    while( fftQ.iFracBits > (30+TRANSFORM_FRACT_BITS) ) { fftQ.iFracBits--; fftQ.iFraction>>=1; } \
    iShift=fftQ.iFracBits-TRANSFORM_FRACT_BITS;

#define COEF_PLUS_NOISE_FRAC_BITS 22
#define DITHER_FRAC_BITS 35
#define RAND_FRAC_BITS 29
#define MORE_WF_FRAC_BITS 0

#define RAND_TO_NOISE(qr) qr

// Inverse Quantize for "normal" case of CoefQ and Noise Dithering
static inline void CoefPlusNoiseInvQuant( const int iRecon, const int iCoefQ, int32 qrand, 
                                         const int32* rgiCoefQ, int32* rgiCoefRecon, FastFloat ffltQuantizer, int iShift )
{
	int iNoise, iCoefPlusNoise, iCoefRecon;
	
	// Multiply by Dither and align iNoise fractional bits to be COEF_PLUS_NOISE_FRAC_BITS == 22
	iNoise = MULT_HI(cDitherFactorMR,qrand)>>((DITHER_FRAC_BITS+RAND_FRAC_BITS-31)-COEF_PLUS_NOISE_FRAC_BITS);
	//// Float Noise = iNoise/(1.0F*(1<<22))
	if ( abs(rgiCoefQ[iCoefQ]) >= (1<<(31-COEF_PLUS_NOISE_FRAC_BITS))  )
	{	// rare, but it does happen occasionally (e.g. tough_32m_32)
		int iFB = 0;
		uint uiCoefQ = abs(rgiCoefQ[iCoefQ]);
		while( uiCoefQ >= (1<<(31-COEF_PLUS_NOISE_FRAC_BITS)) )
		{
			uiCoefQ >>= 1;
			iFB++;
		}
		iCoefPlusNoise = (rgiCoefQ[iCoefQ]<<(COEF_PLUS_NOISE_FRAC_BITS-iFB)) + (iNoise>>iFB);
		//// Float Coef+Noise = iCoefPlusNoise/(1.0F*(1<<(22-iFB)))
		iCoefRecon = MULT_HI( iCoefPlusNoise, ffltQuantizer.iFraction );
		//// Float qrand = qrand/(1024.0F*(1<<25))

		ffltQuantizer.iFracBits -= iFB;
		SETUP_FOR_SCALE_COEF_RECON(ffltQuantizer);
		ffltQuantizer.iFracBits += iFB;

		rgiCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
		//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
		SETUP_FOR_SCALE_COEF_RECON(ffltQuantizer);
	}
	else
	{
		iCoefPlusNoise = (rgiCoefQ[iCoefQ]<<COEF_PLUS_NOISE_FRAC_BITS) + iNoise;	
		//// Float Coef+Noise = iCoefPlusNoise/(1.0F*(1<<22))
		iCoefRecon = MULT_HI( iCoefPlusNoise, ffltQuantizer.iFraction );
		//// Float qrand = qrand/(1024.0F*(1<<25))
		rgiCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
		//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
	}
} // CoefPlusNoiseInvQuant


//**************************************************************************************************
//
// prvInverseQuantizeMidRate handles only MidRate inverse quantization
//
// rgiWeightFactor is not used.
//
//**************************************************************************************************
//WMARESULT prvInverseQuantizeMidRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor)
static int32 WmaInverseQuantizeMidRate (WmaObject*	pWmaDec, uint8 ch) // add by tcx 2008.04.21
{
	int iBark = 0;					// index barks using natural scale for this cSubband (at this sampling frequency)
	int iBarkResampled = 0;			// index barks using resampled scale from cSubbands when the MaskQ's were last updated
	int iCoefQ = 0;
	int iRecon = 0;

	PerChannelInfo *pChnlInfo =  &pWmaDec->channelInfo[ch];
	
	//    const I32* rgiCoefQ				= ppcinfo->m_rgiCoefQ;
	int32 *pCoefQ = pWmaDec->pCoefLowMidRate;

	//const U8*  rgbBandNotCoded		= ppcinfo->m_rgbBandNotCoded;
	uint8  *pBardNotCoded = pChnlInfo->pBandNotCoded;

	//const Int* rgiNoisePower		= ppcinfo->m_rgiNoisePower;
	int8 *pNoisePower = pChnlInfo->pNoisePower;

	
	//const FastFloat* rgffltSqrtBWRatio	= ppcinfo->m_rgffltSqrtBWRatio;
	FastFloat *pBWRatio = pChnlInfo->pffltSqrtBWRatio;

	
	//const Int* rgiMaskQ				= ppcinfo->m_rgiMaskQ;
	int* pMaskQ = pChnlInfo->barkMaskQ;
	
	//const Int  iMaxMaskQ			= ppcinfo->m_iMaxMaskQ;
	int maxMaskQ = pChnlInfo->maxMaskQ;
	
    	const int16 *rgiBarkIndex;
	const int16 *rgiBarkIndexResampled;
    	//CoefType* rgiCoefRecon			= (CoefType*)ppcinfo->m_rgiCoefRecon;
	int32* pCoefRecon = pChnlInfo->pCoefRecon;
		
	//CoefType iCoefRecon; 
	int32 iCoefRecon;
	
    	int qrand, iReconTarget;
    	//U8  cNoiseBand = rgbBandNotCoded [0];
    	uint8 cNoiseBand = pChnlInfo->numBandNotCoded;
	
	int iNoiseBand = 0;
	FastFloat qfltQuantizer;
	
	MaskResampleInfo MRI;

	int iShift = 0;
	bool *rgfMaskNeededForBark;
	// A mask value of a bark should be preserved if that bark:
	// (1) is needed for mask resampling,
	// (2) not coded (noise substituted),
	// (3) has a coded coeff with non-zero value,
	// (4) or has the maximum mask value used in normalization.
    

	rgfMaskNeededForBark = NULL;

	assert (pWmaDec->noiseSub  &&  pWmaDec->weightingMode == BARK_MODE);
	assert(TRANSFORM_FRACT_BITS==5);
	assert(pWmaDec->sizeCurr != 0);
	assert (pWmaDec->sizeCurr == pWmaDec->cSubBand);

	// assert (pau->m_cFrameSampleHalf <= (1<<MASKQ_RESAMPLE_OFFSET));       // why??? 
	//assert (pau->m_iSubFrameSizeWithUpdate != 0); 
	assert( pWmaDec->ratio_log2_curr <= MASKQ_RESAMPLE_OFFSET );

	assert (pWmaDec->cSubBand > 0);

#if 0
	// Initially, no bark-mask needs to be preserved.
	if (NULL != rgfMaskNeededForBark)
		memset(rgfMaskNeededForBark, 0, 25 * sizeof(Bool));
#endif

	// handle changing subFrame window sizes by resampling the indexes
	//    MRI.iMaskResampleRatio = (pau->m_iSubFrameSizeWithUpdate << MASKQ_RESAMPLE_OFFSET) /                           
	//                              ppcinfo->m_cSubFrameSampleHalf;    
	MRI.iMaskResampleRatio = ((pWmaDec->frameSampleHalf / (1<<pWmaDec->ratio_log2_curr_with_update))
		<<MASKQ_RESAMPLE_OFFSET)/pWmaDec->cSubBand;
    
	MRI.iMaskResampleRatioPow = LOG2(MRI.iMaskResampleRatio);
	MRI.cValidBarkBandLatestUpdate = pWmaDec->barkInfo[pWmaDec->ratio_log2_curr_with_update].num_bark_bands; 
	// MRI.cValidBarkBandLatestUpdate = pau->m_rgcValidBarkBand [LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate)];

	rgiBarkIndexResampled = pWmaDec->barkInfo[pWmaDec->ratio_log2_curr_with_update].bark_index;
	//		pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) *  
	//                                  LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate);   

	//rgiBarkIndex  = pau->m_rgiBarkIndex;  
	rgiBarkIndex = pWmaDec->barkInfo[pWmaDec->ratio_log2_curr].bark_index;

	prvGetBandWeightMidRate (pWmaDec, ch, rgiBarkIndex, rgiBarkIndexResampled, MRI);

    //Scan for the first bark index = note iRecon is 0 and rgiBarkIndex[1] ==0 for 16000 Hz and 11025 Hz frames with 128 or 64 samples
	prvScanForNextBarkIndex( iRecon, &iBarkResampled, pWmaDec->lowCutOff, rgiBarkIndexResampled, MRI );
	while( iRecon >= rgiBarkIndex[iBark+1] )
		iBark++;

#if 0
	if (iRecon < pau->m_cLowCutOff ) {
		// not integerized since cLowCutOff is typically 0, so this is here for compatability with V1 
		Double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
		Float fltCoefRecon;
		while  (iRecon < pau->m_cLowCutOff) 	{
			Float fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);//rgfltNoise [iRecon];
			Float fltWeightRatio = (float)pow(10.0f,(rgiMaskQ[iBarkResampled] - iMaxMaskQ)/16.0f);
			fltCoefRecon  = (Float) (fltNoise * fltWeightRatio * dblQuantStep);
			rgiCoefRecon [iRecon] = COEF_FROM_FLOAT(fltCoefRecon);
			MONITOR_COUNT(gMC_IQ_Float,9);
			iRecon++;
		}
	}
#endif

	while  (iRecon < pWmaDec->firstNoiseIndex) 
	{
		iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, pWmaDec->firstNoiseIndex, rgiBarkIndexResampled, MRI );

		//qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
		qfltQuantizer = pChnlInfo->fbarkMaskQ[iBarkResampled];

		qfltQuantizer.iFracBits += COEF_PLUS_NOISE_FRAC_BITS - 31 ;      // Account for MULT_HI in loop below

		//// Float Quantizer = qfltQuantizer.iFraction/(512.0F*(1<<qfltQuantizer.iFracBits)
		SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);              

		while ( iRecon < iReconTarget ) 
		{   
			//  CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * (10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
			qrand = quickRand (&(pWmaDec->randState));       

			//// Float qrand = qrand/(1024.0F*(1<<25))
			CoefPlusNoiseInvQuant( iRecon, iCoefQ, qrand, pCoefQ, pCoefRecon, qfltQuantizer, iShift );
			//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
			
#if 0
			VERIFY_COEF_RECON_MR(iRecon,qrand,-1,iBarkResampled);

			if (NULL != rgfMaskNeededForBark && rgiCoefQ[iCoefQ])
				rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif
			iRecon++;
			iCoefQ++;
		}

		if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
			iBarkResampled++;       // normal to increment except when hitting FirstNoiseIndex

		while( iRecon >= rgiBarkIndex[iBark+1] )
			iBark++;
	}
    
	while  (iRecon < pWmaDec->highCutOff)   // These Bands may or may not be coded, treat as appropriate
	{

		if ( (pBardNotCoded [iBark] == 1)) 
		{   

			int iLoopMax;    

			// CoefRecon[iR] = rand() * ((10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * sqrt( BandWeight[iN]/BandWeight[cN-1] ) * ((10^(1/20)) ^ NoisePower[iN])
			// Note BandsNotCoded span whole "natural" bark bands, which are not resampled and are not limited by m_cHighCutOff
			FastFloat ffltMaskPower, ffltNoisePower;
#if 0
			if (NULL != rgfMaskNeededForBark)
				rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif
			assert( iNoiseBand < cNoiseBand );
            
			// auCalcQuantStep( rgiNoisePower[iNoiseBand], &ffltNoisePower.iFraction, &ffltNoisePower.iFracBits );
			//ffltNoisePower = FASTFLOAT_FROM_QUANTSTEPTYPE( qstCalcQuantStep( rgiNoisePower[iNoiseBand] ) );
			WmaCalcQuantStep(pNoisePower[iNoiseBand], &ffltNoisePower);			
			//ffltNoisePower = FASTFLOAT_MULT( ffltNoisePower, rgffltSqrtBWRatio[iNoiseBand] );
			ffltNoisePower = ffltMultiply(ffltNoisePower, pBWRatio[iNoiseBand]);
			//// Float Noise Power = ffltNoisePower.iFraction/(4.0F*(1<<ffltNoisePower.iFracBits))
            
			iLoopMax = min2(rgiBarkIndex[iBark+1], pWmaDec->highCutOff);
			while(iRecon < iLoopMax) 
			{
				ffltMaskPower = ffltMaskPower10( pMaskQ[iBarkResampled] - maxMaskQ );
				//// Float Mask Power = ffltMaskPower.iFraction/(1.0F*(1<<ffltMaskPower.iFracBits))

				qfltQuantizer = ffltMultiply( ffltMaskPower, ffltNoisePower );
				qfltQuantizer.iFracBits += (RAND_FRAC_BITS - 31);       // Account for MULT_HI in loop below
				//// Float Quantizer = qfltQuantizer.iFraction/(4.0F*(1<<qfltQuantizer.iFracBits))
				SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);

				iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, iLoopMax, rgiBarkIndexResampled, MRI );

				while ( iRecon < iReconTarget ) 
				{
					qrand = quickRand (&(pWmaDec->randState));
					//// Float qrand = qrand/(1024.0F*(1<<25))

					iCoefRecon = RAND_X_QUANT( RAND_TO_NOISE(qrand), qfltQuantizer );
					//// Float CoefRecon = iCoefRecon/(1.0F*(1<<qfltQuantizer.iFracBits))
					pCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
					//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F

					//VERIFY_COEF_RECON_MR(iRecon,qrand,cNoiseBand,iBarkResampled);
					iRecon++;
				}

				if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled[iBarkResampled+1]) 
					iBarkResampled++;       // normal to increment except when hitting end of Bark Band
			}

			iNoiseBand++;
		}
		else 
		{   // This Band is Coded (just like those before FirstNoiseIndex)

			// CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * 10^(MaskQ[iB]-Max(MaskQ[]))*2.5*0.5/20 * 10^(QuantStepSize/20)
			iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, pWmaDec->highCutOff, rgiBarkIndexResampled, MRI );

			if ( iReconTarget > rgiBarkIndex [iBark + 1] )
				iReconTarget = rgiBarkIndex [iBark + 1];        

			//qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
			qfltQuantizer = pChnlInfo->fbarkMaskQ[iBarkResampled];
			//// Float Quantizer = qfltQuantizer.iFraction/(1.0F*(1<<qfltQuantizer.iFracBits))

			// account for MULT_HI in loop below
			qfltQuantizer.iFracBits += COEF_PLUS_NOISE_FRAC_BITS - 31;

			//// Float Quantizer = qfltQuantizer.iFraction/(512.0F*(1<<qfltQuantizer.iFracBits))
            		SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);

			while ( iRecon < iReconTarget )
			{   
				//  CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * (10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
				qrand = quickRand (&(pWmaDec->randState));
				//// Float qrand = qrand/(1024.0F*(1<<25))

				CoefPlusNoiseInvQuant( iRecon, iCoefQ, qrand, pCoefQ, pCoefRecon, qfltQuantizer, iShift );
				//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
#if 0
				if (NULL != rgfMaskNeededForBark && rgiCoefQ[iCoefQ])
					rgfMaskNeededForBark[iBark] = WMAB_TRUE;

				VERIFY_COEF_RECON_MR(iRecon,qrand,-1,iBarkResampled);
#endif                
				iRecon++;
				iCoefQ++;
			}
		}

		if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
			iBarkResampled++;       // normal to increment except when hitting HighCutOff

		while( iRecon >= rgiBarkIndex[iBark+1] )
			iBark++;
	}

	iReconTarget = pWmaDec->cSubBand;
	if  (iRecon < iReconTarget) 
	{   //  CoefRecon[iR] = rand() * DitherLevel * (10^(1/16)) ^ (MaskQ[iB for HighCutOff-1]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
		// We may have scaned past m_cHighCutOff doing a BandNotCoded, so search back to find it.
		while ( (((pWmaDec->highCutOff - 1) << MRI.iMaskResampleRatioPow) >> MASKQ_RESAMPLE_OFFSET) < rgiBarkIndexResampled[iBarkResampled]) 
			--iBarkResampled;

		//qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
		qfltQuantizer = pChnlInfo->fbarkMaskQ[iBarkResampled];
		//// Float Quantizer = qfltQuantizer.iFraction/(1.0F*(1<<qfltQuantizer.iFracBits))

		UNNORMED_MULT( qfltQuantizer, cDitherFactorMR, (DITHER_FRAC_BITS - 31) + (RAND_FRAC_BITS - 31) );
		//// Float Quantizer = qfltQuantizer.iFraction/(4.0F*(1<<qfltQuantizer.iFracBits))
		SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);

		while ( iRecon < iReconTarget )
		{   
			qrand = quickRand (&(pWmaDec->randState));
			//// Float qrand = qrand/(1024.0F*(1<<25))

			iCoefRecon = RAND_X_QUANT( RAND_TO_NOISE(qrand), qfltQuantizer );
			//// Float CoefRecon = iCoefRecon/(1.0F*(1<<qfltQuantizer.iFracBits))
			pCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
			//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F

			//VERIFY_COEF_RECON_MR(iRecon,qrand,cNoiseBand,iBarkResampled);

			iRecon++;
		}
	}

#if 0
        // rgfMaskNeededForBark[bark corresponding to HighCutOff-1 should be made true. 
        if (NULL != rgfMaskNeededForBark)
        {
            for (iBark = pau->m_cValidBarkBand-1; iBark >= 0; iBark--) {
                if ((pau->m_cHighCutOff >= rgiBarkIndex[iBark]) && (pau->m_cHighCutOff < rgiBarkIndex[iBark+1])) {
                    rgfMaskNeededForBark[iBark] = WMAB_TRUE;
                    break; 
                }
            }
        }
#endif
    
	return 0;
} // prvInverseQuantizeMidRate


//*********************************************************************************
// prvGetBandWeightMidRate
// Calculate a band weight for bands which are not coded
// 
// BW = average( 10^(2*MaskQ[iBarkResampled]/16) ) over each linear frequency bin
// For most sampling rates, the bark scale when resampled does not equal the un-resampled bark scale
// But 32000 does have the nice property that the resampled bark scale = un-resampled in the noise substitution region
// So for 32000 Hz to 44099 Hz:  
//		BW = 10^((2/16)*MaskQ[iBarkResampled])
// For all other sampling frequencies:
//		BW = ( N1 * 10^(2*MarkQ[iBarkResampled]) + N2 * 10^(2*MarkQ[iBarkResampled+1]/16) ) / (N1 + N2)
//		where N1 is the number of linear frequency bins in the first resampled bark band
//		and   N2 is the number of linear frequency bins in the second resampled bark band
// BandWeights are only used as the sqrt of the ratio to the last noise BandWeight
// So for 32000 Hz to 44099 Hz
//		SqrtBWRatio = 10^((MaskQ[iBarkResampled] - MaskQ[iLast])/16)
// And for other sampling frequencies
//		SqrtBWRatio = sqrt( BW[i] / BW[last] )
// And for all cases where SqrtBWRatio[last] = 1
// Note that log to the base 10^(1/16) of the MaskQ are integers for 32000 but are not integers for non-32000Hz
// So doing all the inverse quantization in the exponent domain does not have the advantage of using integer exponents
// For this reason, SqrtBWRatio is stored and used as a FastFloat.
// TODO:  rgffltSqrtBWRatio seems to have a narrow range between 1/4 and 4. Consider making it a fixed point int.
// TODO:  Avoid recalculating when MaskQ's have not been updated and the resample ratio is the same as the previous subframe
//*********************************************************************************
void prvGetBandWeightMidRate (WmaObject*	pWmaDec, uint8 ch, 
							  const int16* const rgiBarkIndex, const int16* const rgiBarkIndexResampled, MaskResampleInfo MRI)
{
	PerChannelInfo *pChnlInfo =  &pWmaDec->channelInfo[ch];

	uint8*	pBandNotCoded = pChnlInfo->pBandNotCoded;
	FastFloat* pffltSqrtBWRatio = pChnlInfo->pffltSqrtBWRatio;
	int* pMaskQ = pChnlInfo->barkMaskQ;
	uint8 cNoiseBand	= 0;
	FastFloat fflt;
	float flt;

	int iCurrStart, iCurrBand, iCurrEnd;	// indexes for the current subframe
	int iMaskStart, iMaskBand, iMaskEnd;	// indexes in the subframe where the MaskQ's were last updated
	int iRsmpStart, iRsmpBand, iRsmpEnd;	// indexes for the current subframe resampled to the subframe where the MaskQ's were last updated
	int fAllBandsSynced = (MRI.iMaskResampleRatioPow == MASKQ_RESAMPLE_OFFSET) 
		|| (pWmaDec->version_number !=1 && 32000 <= pWmaDec->samples_per_second && pWmaDec->samples_per_second < 44100 );
	int rgiMaskQ4BandNotCoded[10];			// used if bands Syncronized
	float fltBandWeight[10];				// used if bands are no Syncronized
	int cMaskHighCutOff;

	iCurrBand = pWmaDec->currFirstNoiseBand;
	iMaskBand = iRsmpBand = 0;
	if (!fAllBandsSynced)
		cMaskHighCutOff = iResampleIndex(pWmaDec->cSubBand, MRI) * pWmaDec->highCutOffLong/ pWmaDec->frameSampleHalf;

	while (1)
	{
		assert (iCurrBand <= pWmaDec->barkInfo[pWmaDec->ratio_log2_curr].num_bark_bands);
		iCurrStart = max2( rgiBarkIndex [iCurrBand], pWmaDec->firstNoiseIndex);
		if (iCurrStart >= pWmaDec->highCutOff)
			break;

		if (pBandNotCoded [iCurrBand] == 1)
		{
			assert( cNoiseBand < 10 );		// probably (cNoiseBand < 5) would be OK

			// Determine the band for accessing the possibly resampled MaskQ's
			iMaskStart = iResampleIndex( iCurrStart, MRI );
			while ( rgiBarkIndexResampled [iMaskBand+1] <= iMaskStart ) 
				++iMaskBand;

			if ( fAllBandsSynced )
			{	// Current subFrame bands synced to band were MaskQ's were last updated
				rgiMaskQ4BandNotCoded[cNoiseBand] = pMaskQ[iMaskBand];
			}
			else
			{	// Have to check in detail about whether one or two MaskQ's are used by this BandNotCoded
				iCurrEnd   = min2(pWmaDec->highCutOff, rgiBarkIndex [iCurrBand + 1]);
				assert (iCurrEnd >= iCurrStart);
				iRsmpStart = iMaskStart;
				iMaskEnd = min2( rgiBarkIndexResampled [ iMaskBand+1 ], cMaskHighCutOff );
				iRsmpEnd = min2( iResampleIndex( iCurrEnd, MRI ), cMaskHighCutOff );
				while ( rgiBarkIndexResampled [iRsmpBand+1] <= (iRsmpEnd-1) ) 
					++iRsmpBand;
				assert( iMaskBand == iRsmpBand || (iMaskBand+1) == iRsmpBand );
				if ( iRsmpBand == iMaskBand )
				{	// just a constant MaskQ for the whole NoiseBand
					fflt = ffltMaskPower10( pMaskQ[iMaskBand] );
					flt = FloatFromFastFloat( fflt );
					fltBandWeight[cNoiseBand] = flt*flt;
					//MONITOR_COUNT(gMC_GBW_floats,4);
				}
				else
				{	// Two different MaskQ's for this NoiseBand
					// BW = ( N1 * 10^(2*MarkQ[iBarkResampled]) + N2 * 10^(2*MarkQ[iBarkResampled+1]/16) ) / (N1 + N2)
					// This is not suited to FastFloat - leave in float.
					float fltP1, fltP2;
					int iUnRsmpEnd = iUnResampleIndex( iMaskEnd, MRI );
					fflt  = ffltMaskPower10( pMaskQ[iMaskBand] );
					fltP1 = FloatFromFastFloat( fflt );
					fflt  = ffltMaskPower10( pMaskQ[iRsmpBand] );
					fltP2 = FloatFromFastFloat( fflt );
					flt   = ( (iUnRsmpEnd-iCurrStart)*fltP1*fltP1 + (iCurrEnd-iUnRsmpEnd)*fltP2*fltP2 ) / (iCurrEnd - iCurrStart);
					fltBandWeight[cNoiseBand] = flt;
					//MONITOR_COUNT(gMC_GBW_floats,13);
				}
			}
			cNoiseBand++;
		}

		iCurrBand++;
	}

	for( iCurrBand = 0; iCurrBand < (cNoiseBand-1); iCurrBand++ )
	{	
		if ( fAllBandsSynced )
		{
			fflt = ffltMaskPower10( rgiMaskQ4BandNotCoded[iCurrBand] - rgiMaskQ4BandNotCoded[cNoiseBand-1] );
			flt  = FloatFromFastFloat( fflt );
			pffltSqrtBWRatio[iCurrBand] = fflt;
			//// float SqrtBWRatio = fflt.iFraction*1.0F/(1<<fflt.iFracBits)
		}
		else
		{
			// This calculation is not ideally suited to FastFloat, leave in float for time being
			// But note that limited range of the result probably means it can be done more quickly
			flt = (float)sqrt( fltBandWeight[iCurrBand] / fltBandWeight[cNoiseBand-1] );
			fflt = FastFloatFromFloat( flt );
			pffltSqrtBWRatio[iCurrBand] = fflt;
			//// float SqrtBWRatio = fflt.iFraction*1.0F/(1<<fflt.iFracBits)
		}

		//MONITOR_RANGE(gMR_rgffltSqrtBWRatio,FLOAT_FROM_FASTFLOAT(fflt));
		//MONITOR_COUNT(gMC_GBW_floats,14);		// count sqrt as 10.

	}

	if (cNoiseBand > 0)
	{	// last band has a ratio of 1.0

		pffltSqrtBWRatio[cNoiseBand-1].iFraction = 0x40000000;
		pffltSqrtBWRatio[cNoiseBand-1].iFracBits = 30;
		//MONITOR_RANGE(gMR_rgffltSqrtBWRatio,1.0F);
	}

	assert (cNoiseBand < pWmaDec->barkInfo[pWmaDec->ratio_log2_curr].num_bark_bands);
	assert (pWmaDec->currFirstNoiseBand > 0);
	//pBandNotCoded [0] = cNoiseBand;				// used as cNoiseBand
	pChnlInfo->numBandNotCoded = cNoiseBand;
} // prvGetBandWeightMidRate



// *****************************************************************************
//
// Conversion from LSF thru Parcor to LPC
//
// *****************************************************************************
// Parcor to LPC conversion

/* symmetric convolution */
static void WmaConvolve_odd(int32* in1, int32 l1, int32* in2, int32 l2, int32* out, int32 *lout)
{
	// In BUILD_INTEGER mode, in1 assumed to have FRACT_BITS_LSP and in2 to have FRACT_BITS_LP. Output will have
	// FRACT_BITS_LP.
	int32 i,j;
	int32 ret[100];
 
	for (i=0; (i<l1)&&(i<l2); i++) {
		ret[i]=0;
		for (j=0; j<=i; j++) {
			ret[i] += MULT_LSP(in1[j],in2[i-j]);
		}
	}
	
	for (i=l1; i<(l1+l2)/2; i++) {
		ret[i]=0;
		for (j=0; j<l1; j++) {
			ret[i] += MULT_LSP(in1[j],in2[i-j]);
		}
	}

	*lout=(l1+l2)-1;
	for (i=0; i<(*lout)/2; i++)	{
		out[i]=ret[i];
		out[(*lout-1)-i]=ret[i];
	}
	out[*lout/2]=ret[*lout/2];
}

/* symmetric convolution */
static void WmaConvolve(int32 *in1, int32 l1, int32 *in2, int32 l2, int32 *out, int32 *lout)
{
  // In BUILD_INTEGER mode, in1 assumed to have FRACT_BITS_LSP and in2 to have FRACT_BITS_LP. Output will have
  // FRACT_BITS_LP.
	int32   i,j;
	int32 ret[100];
  
	for (i=0; (i<l1)&&(i<l2); i++) 
	{
		ret[i]=0;
		for (j=0; j<=i; j++) {
			ret[i] += MULT_LSP(in1[j],in2[i-j]);
		}
	}
  
	for (i=l1; i<(l1+l2)/2; i++) {
		ret[i]=0;
		for (j=0; j<l1; j++) {
			ret[i] += MULT_LSP(in1[j],in2[i-j]);
		}
	}
	
	*lout=(l1+l2)-1;
	for (i=0; i<(*lout)/2; i++) {
		out[i]=ret[i];
		out[(*lout-1)-i]=ret[i];
	}
}

#define LSF_DECODE(i,lsfQ) g_rgiLsfReconLevel[i][lsfQ[i]]
//#define LP_DECODE(i,lsfQ) LSF_DECODE((i),(lsfQ))
#define LP_DECODE(i,lsfQ) (g_rgiLsfReconLevel[i][lsfQ[i]] >> (FRACT_BITS_LSP-FRACT_BITS_LP))


// Only updates
#define ARRAY_RANGE_FMAX(a,b,c,d,init,i) if ((init)) d = -FLT_MAX; \
for ((i)=(b);(i)<(c);(i)++) (d) = ((d) < (a)[i]) ? (a)[i] : (d);
#define ARRAY_RANGE_FMIN(a,b,c,d,init,i) if ((init)) d = FLT_MAX; \
for ((i)=(b);(i)<(c);(i)++) (d) = ((d) > (a)[i]) ? (a)[i] : (d);

// 5.2.3.3
static void WmaLsp2pq (uint8 *lsfQ, int32 *p, int32 *q, int32 order)
{
    int32	i;
    int32	long_seq[100];
    int32	short_seq[3];
    int32   long_length, short_length;
   
    short_length = 3; 
    long_length  = 2;
    
    short_seq[0] = short_seq[2] = LSP_FROM_FLOAT(1);
    long_seq[0]  = long_seq[1]  = LP_FROM_FLOAT(1);
    
    for (i = 0; i < order; i+= 2) {
        short_seq[1]= LSF_DECODE(i,lsfQ);   // -2.0F*(Float)cos(2.0*PI*lsp[i])
        WmaConvolve (short_seq, short_length, long_seq, long_length, 
            long_seq, &long_length);
    }
    
    for (i=1; i<=order/2; i++) {
        p[i] = long_seq[i];
    }
    
    long_length = 3;
    long_seq[0] = LP_FROM_FLOAT(1);
    long_seq[1] = LP_DECODE(1,lsfQ);             // -2.0F*(Float)cos(2.0*PI*lsp[1])
    long_seq[2] = LP_FROM_FLOAT(1);
    
    for (i = 3; i < order; i += 2) {
        short_seq[1]= LSF_DECODE(i,lsfQ);   // -2.0F*(Float)cos(2.0*PI*lsp[i])
        WmaConvolve_odd (short_seq, short_length, long_seq, long_length, 
            long_seq, &long_length);
    }
    
    for (i=1; i<=order/2; i++) {
        q[i]=long_seq[i]-long_seq[i-1];
    }
}

// 5.2.3.3
static void WmaPq2lpc (int32* p, int32* q, int32* lpc, int32 order)
{
	int32 i;

    for(i=1; i<=order/2; i++) 
	{
		lpc[i-1]= -DIV2(p[i]+q[i]);
		lpc[order-i]= -DIV2(p[i]-q[i]);
	}
}

// 5.2.3.3
// Quantized LSF to LPC
static void WmaLsp2Lpc(uint8 *lsfQ, int32 *lpc, int32 order)
{
	int32 p[LPCORDER+2],q[LPCORDER+2];
	
	WmaLsp2pq (lsfQ, p, q, order);
	WmaPq2lpc (p, q, lpc, order);
}



// ************************************************************************************
//
// InverseQuadRoot(x) = (1/x)^(1/4) 
// where the 1/4 is one squareroot combined with the flattenfactor
//
// Do this three different ways:  Encoder, I32eger Decoder, I32Float Decoder
//
// ************************************************************************************




ALIGN_32 (uint32 gLZLTable[128]);
#if 0	// Wei Zhong, simplified 11/01/07
// 8 | 7<<4,		0
// 6 | 6<<4,		1
// 5 | 5<<4,		2-3
// 4 | 4<<4,		4-7
// 3 | 3<<4,		8-15
// 2 | 2<<4,		16-31
// 1 | 1<<4,		32-63
// 0,				64-127
void gLZLTableInit(Void)
{
   int32 i,j;
   uint8 mask;

   for(i=0;i<128;i++) {
	   mask=0x80;
	   for(j=0;j<8;j++) {
		   if((2*i) &mask)
			   break;
		   mask>>=1;
	   }
	   
	   gLZLTable[i]=j;
       mask=0x80;
	   for(j=0;j<8;j++) {
		   if((2*i+1)&mask)
			   break;
		   mask>>=1;
	   }
       gLZLTable[i]|=j<<4;
   }
   return;
}
#else
// 7 ,		0
// 6 ,		1
// 5 ,		2-3
// 4 ,		4-7
// 3 ,		8-15
// 2 ,		16-31
// 1 ,		32-63
// 0,		64-127
static int32 getMSB (int32 a)
{
	int32 i=0;
	if (a==0) return 0;
	while (a>>=1) {
		i ++;
	}
	return i;
}

static void gLZLTableInit(void)
{
	int32 i;
//	uint8 mask;
    VPRINTF (("gLZL at %x\n", gLZLTable));	
	gLZLTable [0] = 7;
	for(i=1;i<128;i++) {
		gLZLTable [i] = 6- getMSB(i);
	}

	return;
}
#endif

static void WmaInitInverseQuadRootTable (WmaObject* pWmaDec)
{
	// 11/08/06 Wei Zhong, lpc only?
	if (pWmaDec->weightingMode == LPC_MODE) {
		gLZLTableInit();
	}
	return;
}

//	input:
//		original [10]
//		iSize
//	
//	output:
//		pWeightFactor [2048]
//		maxWeight
static int32 WmaLpcToSpectrum(WmaObject* pWmaDec, const int32* rgLpcCoef, PerChannelInfo* ppcinfo)
{
    // put often used variables near the top for easier access in platforms like the SH3 and SH4

    int32 original[LPCORDER];
    int32 *pFb;      
    int32 i, j;
    int32 tS2x; 
    int32 tCpS1x, tCmS1x, tCpS2x, tCmS2x;
    int32 CpS1, CmS1, CpS2, CmS2;
    int32 D, E, F, G, H, I, J;
    
    int32* rgwtLpcSpec;
    uint32 wtLpcSpecMax, wtTemp;
    int32 iFreq,/* iF,*/ iLoopLimit;
    //int32 iShrink/*, iStride*/; 
    int32 Tmp[32];
    uint32* pWF = (uint32*) (int32 *)ppcinfo->pWeightFactor; 

    int32 S4, C4;
    int32 S1, C1;

	int32 SL16, CL16, SL8, CL8, SL3by16, CL3by16;
    int32 iSizeBy2, iSizeBy4, iSizeBy8, iSizeBy16;

    int32 iSize;
	const int32	*trig_table;

    // 5.4.2
    if (pWmaDec->fV5Lpc)
        iSize = pWmaDec->cSubBand;
    else 
        iSize = pWmaDec->frameSampleHalf;
	
	switch (iSize) {
	case 2048:
		trig_table = trig_2048; break;
	case 1024:
		trig_table = trig_1024; break;
	case 512:
		trig_table = trig_512; break;
	case 256:
		trig_table = trig_256; break;
	case 128:
		trig_table = trig_128; break;
	default:
		trig_table = 0;
		assert (0);
		break;
	}
#define MAX_LP_SPEC_SIZE 2048

    //iShrink = LOG2(MAX_LP_SPEC_SIZE/iSize);     // for smaller transforms, shrink or expand indexing
    //iStride = MAX_LP_SPEC_SIZE/iSize;           // for smaller transforms, stride past unused (lpc_compare only)
    iSizeBy2 = iSize>>1;
    iSizeBy4 = iSizeBy2>>1;
    iSizeBy8 = iSizeBy4>>1;
    iSizeBy16 = iSizeBy8>>1;
    // iSize can be 2048, 1024, 512, 256, or 128.
    //      at 32000 Hz: 2048, 1024, 512 and 256
    //      at 22050 Hz: 1024, 512, 256 and 128
    //      at 16000 Hz: 512, 256, and 128
    //      at 11025 Hz: 512, 256, and 128
    //      at  8000 Hz: 512.

#if 0
	{
		int a = 
		InverseQuadRootOfSumSquares (0x14a286c, 0xff760626);
	}
#endif
        
    // DEBUG_ONLY( if (pWmaDec->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    //assert(pWmaDec->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    
    //for (i = 0; i < LPCORDER; i++) 
    //fprintf(stdout, "%.20lf\n", (double) FLOAT_FROM_LP(rgLpcCoef[i])); 
    
    for (i = 0; i < LPCORDER; i++) {
		// wzhong, for clarity
		original[i] = -(rgLpcCoef[i] >> 2);
        //original[i] = -LP_SPEC_FROM_LP(rgLpcCoef[i]); 
	}
    
	/// limit variable scope
	{
		int32 t1pO7, t1mO7, tO1pO9, tO1mO9, tO0pO8, tO0mO8, tO4pO6, tO4mO6;
		int32 t1pO7pO3, t1pO7mO3, tO1pO5pO9, tO0pO2pO4pO6pO8;
                                        // F[128]
		//printf ("%x\n", SQRT2);
		Tmp[14]  = (t1pO7 = LP_SPEC_FROM_FLOAT(1) + original[7]) + (tS2x = MULT_BP2X(SQRT2,original[3]));
											// F[256]  
		Tmp[22]  = (t1mO7 = LP_SPEC_FROM_FLOAT(1) - original[7]) + original[3];
		Tmp[30]  = t1mO7;                   // F[384]
		Tmp[31]  = t1pO7 - tS2x;            // F[640]
		Tmp[23]  = t1mO7 - original[3];     // F[768]
		Tmp[15]  = t1mO7;                   // F[896]
    
											// F[1152] 
		Tmp[8]  = (tO1pO9 = original[1] + original[9]) + (tS2x = MULT_BP2X(SQRT2,original[5]));
											// F[1280] 
		Tmp[16]  = (tO1mO9 = original[1] - original[9]) + original[5];
		Tmp[24]  = tO1mO9;                  // F[1408]
		Tmp[25]  = tO1pO9 - tS2x;           // F[1664]
		Tmp[17]  = tO1mO9 - original[5];    // F[1792]
		Tmp[9]  = tO1mO9;                   // F[1920] 
    
											// F[2176] 
		Tmp[10]  = (tO0pO8 = original[0] + original[8]) + (tS2x = MULT_BP2X(SQRT2,original[4]));
											// F[2304] 
		Tmp[18]  = (tO0mO8 = original[0] - original[8]) + original[4];
		Tmp[26]  = tO0mO8;                  // F[2432]
		Tmp[27]  = tO0pO8 - tS2x;           // F[2688]
		Tmp[19]  = tO0mO8 - original[4];    // F[2816] 
		Tmp[11]  = tO0mO8;                  // F[2944]
    
											// F[3200] 
		Tmp[12]  = original[2] + (tS2x = MULT_BP2X(SQRT2,original[6]));
		Tmp[20]  = original[2] + original[6]; // F[3328]
		Tmp[28]  = original[2];             // F[3456] 
		Tmp[29]  = original[2] - tS2x;      // F[3712]
		Tmp[21]  = original[2] - original[6]; // F[3840]
		Tmp[13]  = original[2];             // F[3968]
    
		tO4pO6 = original[4] + original[6];
		tO4mO6 = original[4] - original[6];
		t1pO7pO3 = t1pO7 + original[3];
		t1pO7mO3 = t1pO7 - original[3];
		tO1pO5pO9 = tO1pO9 + original[5];
		tO0pO2pO4pO6pO8 = tO0pO8 + tO4pO6 + original[2];

		D  = t1pO7pO3 + tO1pO5pO9 + tO0pO2pO4pO6pO8;    // F[0]
		E  = t1pO7mO3 + (tS2x = MULT_BP2X(SQRT2_2, tO0pO8 - tO4mO6 - original[2]));     // F[512]
		F  = t1pO7pO3 - tO1pO5pO9;                      // F[1024]
		G  = t1pO7mO3 - tS2x;                           // F[1536]
		H  = -tO1pO9 + original[5] + (tS2x = MULT_BP2X(SQRT2_2,tO0pO8 - tO4pO6 + original[2]));  // F[2560]
		I  =  tO0pO8 + tO4mO6 - original[2];            // F[3072]
		J  =  tO1pO9 - original[5] + tS2x;              // F[3584]

	}
    
	//printf ("%x %x %x %x %x %x %x, %x\n", D, E, F, G, H, I, J, SQRT2_2);
    // j==0 and i==0 below.
    pFb   =  (int32*) &pWF[0];      
    *pFb  =  InverseQuadRootOfSumSquares(D, 0);     // F[0]
    pFb  +=  iSizeBy4;   
    *pFb  =  InverseQuadRootOfSumSquares(E, J);     // F[512]    b: F[512]    a: F[3584]
    pFb  +=  iSizeBy4;    
    *pFb  =  InverseQuadRootOfSumSquares(F, I);     // F[1024]    b: F[1024]   a: F[3072]
    pFb  +=  iSizeBy4;    
    *pFb  =  InverseQuadRootOfSumSquares(G, H);     // F[1536]   b: F[1536]   a: F[2560]

 	SL16 = TRIGTBL(iSizeBy16,0);
	CL16 = TRIGTBL(iSizeBy16,1);
    WmaDoLpc4( iSizeBy16, Tmp+8, pWF, iSizeBy2, SL16, CL16 );
	
	SL8 = TRIGTBL(iSizeBy8,0);
	CL8 = TRIGTBL(iSizeBy8,1);
    WmaDoLpc4( iSizeBy8,  Tmp+16, pWF, iSizeBy2, SL8,  CL8 );

	SL3by16 = TRIGTBL(iSizeBy8 + iSizeBy16,0);
	CL3by16 = TRIGTBL(iSizeBy8 + iSizeBy16,1);
    WmaDoLpc4( iSizeBy8 + iSizeBy16, Tmp+24, pWF, iSizeBy2, SL3by16, CL3by16 );

    assert( 64 <= iSize && iSize <= 2048 );

    for (j=1; j<iSizeBy16; j++ )
    {
		int32 S2, C2;
        int32 C8, S8;

		S4 = TRIGTBL (4*j ,0);
		C4 = TRIGTBL (4*j ,1);
		//assert( fabs(FLOAT_FROM_BP2(S4)-sin(PI*4*j/iSize)) < 0.0001 
		//	&& fabs(FLOAT_FROM_BP2(C4)-cos(PI*4*j/iSize)) < 0.0001 );

		CmS1 = -C4 + S4;                    // cnst3[i*(4<<iShrink)+3];
		CpS1 =  C4 + S4;                    // cnst3[i*(4<<iShinrk)+1];
		
		//S8 = MUL2(MULT_BP2X(C4,S4));                        // sin(2x)		
		//C8 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S4,S4)); // cos(2x)
		S8 = TRIGTBL (8*j ,0);
		C8 = TRIGTBL (8*j ,1);
		CmS2 = -C8 + S8;                    // cnst3[i*(4<<iShrink)+2];
		CpS2 =  C8 + S8;                    // cnst3[i*(4<<iShink)];

        assert( BP2_FROM_FLOAT(1) <= CpS2 && CpS2 <= BP2_FROM_FLOAT(1.5) );     
                                                        // F[j]
        Tmp[6]  = LP_SPEC_FROM_FLOAT(1) + (tCpS2x = MULT_BP2X(CpS2,original[7])) + (tCpS1x = MULT_BP2X(CpS1,original[3]));
                                                        // F[256-j]
        Tmp[14] = LP_SPEC_FROM_FLOAT(1) + (tCmS2x = MULT_BP2X(CmS2,original[7])) + tCpS1x;
                                                        // F[256+j]
        Tmp[22] = LP_SPEC_FROM_FLOAT(1) - tCpS2x - (tCmS1x = MULT_BP2X(CmS1,original[3]));    
        Tmp[30] = LP_SPEC_FROM_FLOAT(1) - tCmS2x + tCmS1x;  // F[512-j]
        Tmp[31] = LP_SPEC_FROM_FLOAT(1) + tCpS2x - tCpS1x;  // F[512+j]
        Tmp[23] = LP_SPEC_FROM_FLOAT(1) + tCmS2x - tCpS1x;  // F[768-j]
        Tmp[15] = LP_SPEC_FROM_FLOAT(1) - tCpS2x + tCmS1x;  // F[768+j]
        Tmp[7]  = LP_SPEC_FROM_FLOAT(1) - tCmS2x - tCmS1x;  // F[1024-j]
        
                                                        // F[1024+j]
        Tmp[0]  = original[1] + (tCpS2x = MULT_BP2X(CpS2,original[9])) + (tCpS1x = MULT_BP2X(CpS1,original[5]));
                                                        // F[1280-j]  
        Tmp[8]  = original[1] + (tCmS2x = MULT_BP2X(CmS2,original[9])) + tCpS1x;
                                                        // F[1280+j]  
        Tmp[16] = original[1] - tCpS2x - (tCmS1x = MULT_BP2X(CmS1,original[5]));
        Tmp[24] = original[1] - tCmS2x + tCmS1x;            // F[1536-j]
        Tmp[25] = original[1] + tCpS2x - tCpS1x;            // F[1536+j]
        Tmp[17] = original[1] + tCmS2x - tCpS1x;            // F[1792-j]
        Tmp[9]  = original[1] - tCpS2x + tCmS1x;            // F[1792+j]
        Tmp[1]  = original[1] - tCmS2x - tCmS1x;            // F[2048-j
        
                                                        // F[2048+j]  
        Tmp[2]  = original[0] + (tCpS2x = MULT_BP2X(CpS2,original[8])) + (tCpS1x = MULT_BP2X(CpS1,original[4]));
                                                        // F[2304-j]  
        Tmp[10] = original[0] + (tCmS2x = MULT_BP2X(CmS2,original[8])) + tCpS1x;
                                                        // F[2304+j]  
        Tmp[18] = original[0] - tCpS2x - (tCmS1x = MULT_BP2X(CmS1,original[4]));
        Tmp[26] = original[0] - tCmS2x + tCmS1x;            // F[2560-j]
        Tmp[27] = original[0] + tCpS2x - tCpS1x;            // F[2560+j]
        Tmp[19] = original[0] + tCmS2x - tCpS1x;            // F[2816-j]
        Tmp[11] = original[0] - tCpS2x + tCmS1x;            // F[2816+j]
        Tmp[3]  = original[0] - tCmS2x - tCmS1x;            // F[3072-j]
        
                                                        // F[3072+j] 
                                                        // F[3328-j]
        Tmp[4]  =  Tmp[12]  = original[2] + (tCpS1x = MULT_BP2X(CpS1,original[6]));
                                                        // F[3328+j] 
        Tmp[20] = original[2] - (tCmS1x = MULT_BP2X(CmS1,original[6]));
        Tmp[28] = original[2] + tCmS1x;                   // F[3584-j]
        Tmp[29] = original[2] - tCpS1x;                   // F[3584+j]
        Tmp[21] = original[2] - tCpS1x;                   // F[3840-j]
        Tmp[13] = original[2] + tCmS1x;                   // F[3840+j]
        Tmp[5]  = original[2] - tCmS1x;                   // F[4096-j]
        
        //** this block uses 52 adds and 14 mults??
       

        // example:  iSize = 0x100 = 256.
        //            j==1         |   j==2          |  j==3
        //       i:  0   1   2   3 |   0   1   2   3 |  0
        //    k      1  1f  21  3f |   2  1e  22  3e |  3 ...
        // 80+k     81  9f  a1  bf |  82  9e  a2  be | 83 ...
        // 80-k     7f  61  5f  41 |  7e  62  5e  42 | 7e ...
        //100-k     ff  e1  df  c1 |  fe  e2  de  c2 | fd ...

		S1 = TRIGTBL(j, 0);
		C1 = TRIGTBL(j, 1);
        WmaDoLpc4( j, Tmp, pWF, iSizeBy2, S1, C1 );

        
		S2 = TRIGTBL(iSizeBy8-j,0);
		C2 = TRIGTBL(iSizeBy8-j,1);
        WmaDoLpc4( iSizeBy8 - j, Tmp+8,  pWF, iSizeBy2, S2, C2 );

		S2 = TRIGTBL(iSizeBy8+j,0);
		C2 = TRIGTBL(iSizeBy8+j,1);
        WmaDoLpc4( iSizeBy8 + j, Tmp+16, pWF, iSizeBy2, S2, C2 );

		S2 = TRIGTBL(iSizeBy4-j,0);
		C2 = TRIGTBL(iSizeBy4-j,1);
        WmaDoLpc4( iSizeBy4 - j, Tmp+24, pWF, iSizeBy2, S2, C2 );

    }
#if 0
		//if (j == 32)
		{
			FILE *fpWeight;
			int i;
			fpWeight = fopen ("h:\\hddec\\wma\\weight.ref", "w"); assert (fpWeight);
			fprintf (fpWeight, "# %d\n", j);
			for (i=0; i<1024; i++) {
				fprintf (fpWeight, "%08x\n", pWF[i]);
			}
			fclose (fpWeight);
		}
#endif
    //** this loop uses (512>>iShrink) * (36 adds and 24 mults)
    
    //** total to here: 
    //**   (46 adds and 14 mults) + (N/16) * (52 adds and 14 mults) + (N/4) * (36 adds and 24 mults)
    //** TA = 46 + 3.25N + 9N      = 46 + 12.250N
    //** TM = 14 + (14/16)N + 6*N  = 14 +  6.875N
    //** An FFT takes order N*log(N)
    //** so we may need to substitute an optimized FFT for this on some platforms.
    
    // The inverse quad root has already been computed. Copy to destination, find max etc remain here.
    
    rgwtLpcSpec  = (int32 *)ppcinfo->pWeightFactor; 
    wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    iLoopLimit   = pWmaDec->cSubBand;
    
    for (iFreq = 0 /*, iF = 0*/; iFreq < iLoopLimit; iFreq++/*, iF += iStride*/) {
        wtTemp = pWF[iFreq];

        if ( wtTemp > wtLpcSpecMax )
            wtLpcSpecMax = wtTemp;
		
        assert( wtTemp>= 0 ); 
    }
    
    if (wtLpcSpecMax == WEIGHT_FROM_FLOAT(0.0F)) {        
        return -1;
    }  

    ppcinfo->maxWeight = wtLpcSpecMax; 

    return OKAY;
}

// wzhong 11/01/07
// use Aura
static uint32 InverseQuadRootOfSumSquares (int32 F1, int32 F2 )
{
    // LpSpecPowerType is uint64.
	uint64 fOrig = SUM_SQUARES( F1, F2 );
    //uint64 f = fOrig >> (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER);
	
	//int32 iExpNt=0;
    uint32 uiMSF;
    int32 iExp = 0;
    int32 iMSF8;
    uint32 uiFrac1;
	//uint8 index;
	//uint64 temp64;

	uiMSF = (uint32)(SFTR_64 (fOrig, 52) );
    if ( uiMSF==0 ) {
		//assert (0);
        iExp = 32;
        uiMSF = (uint32)(SFTR_64 (fOrig, 20 ) );
    }
	else {
		//printf ("here\n");
	}
	
    if (uiMSF==0) {
		assert (0);
        return(0xFFFFFFFF);
	}
    // normalize the most significant fractional part
    while( (uiMSF & 0xFf000000)==0 ) {
        iExp += 8;
        uiMSF <<= 8;
    }

	//index=(uint8)(uiMSF>>24);
	assert (0<(uiMSF>>24) && (uiMSF>>24) < 256);

	// wzhong, 11/01/07
	// seem when index !=0, therefore can simply to
	// gLZLTable is simplified as well
#if 0
	iExp+=((gLZLTable[index>>1]>>((index&1)<<2)))&0xf;
#else
	iExp+=((gLZLTable[(uiMSF>>25)])); //&0xf;

	//assert ( (((gLZLTable[index>>1]>>((index&1)<<2)))&0xf )	== (((gLZLTable[index>>1]>>4))&0xf ) );
#endif

    // discard the most significant one bit (it's presence is built into g_InvQuadRootFraction)
    iExp++;
    
    //iExpNt=(iExp-(2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER));   
	//iExpNt = iExp- 20;   
	//uiMSF = (iExpNt>32) ? (uint32)(fOrig<<(iExpNt-32)) : (uint32)(fOrig>>(32-iExpNt));
	uiMSF = (iExp > 52) ? (uint32)( SFTL_64 (fOrig, iExp-52)) : (uint32)( SFTR_64(fOrig, 52-iExp));
	

    // split into top INVQUADROOT_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
    iMSF8 = uiMSF>>(32-INVQUADROOT_FRACTION_TABLE_LOG2_SIZE);
    uiMSF <<= INVQUADROOT_FRACTION_TABLE_LOG2_SIZE;

    // lookup and interpolate - tables are set up to return correct binary point for WeightType
    uiFrac1  = g_InvQuadRootFraction[iMSF8++];
	// wzhong, 11/01/07
	// doing linear interpolation
    uiFrac1 -= MULT_HI_UDWORD2( uiMSF, uiFrac1 - g_InvQuadRootFraction[iMSF8] );

#if 0
	{
		static int cnt = 0;
		static FILE *fpLZL, *fpInvQuadRootFrac, *fpInvQuadRootExp;
		if (cnt == 100) {
			int i;
			fpLZL = fopen ("lzl.txt", "w"); assert (fpLZL);
			fpInvQuadRootFrac = fopen ("invquadrootfrac.txt", "w"); assert (fpInvQuadRootFrac);
			fpInvQuadRootExp = fopen ("invquadrootExp.txt", "w"); assert (fpInvQuadRootExp);
			
			// 128
			for (i=0; i<128; i++) {
				fprintf (fpLZL, "%08x\n", gLZLTable[i]);
			}

			// 257, 
			for (i=0; i<1+INVQUADROOT_FRACTION_TABLE_SIZE; i++) {
				fprintf (fpInvQuadRootFrac, "0x%08x,	// %d\n", g_InvQuadRootFraction[i] , i);
			}			

			/// 65,
			for (i=0; i<1+64; i++) {
				fprintf (fpInvQuadRootExp, "0x%08x,	// %d\n", g_InvQuadRootExponent[i] , i);
			}

			fclose (fpLZL);
			fclose (fpInvQuadRootFrac);
			fclose (fpInvQuadRootExp);
			//cnt = 1;
		}
		cnt ++;
	}

#endif

    return MULT_HI_UDWORD2( uiFrac1, g_InvQuadRootExponent[ iExp ] );
}

static void WmaDoLpc4( const int32 k, const int32* pTmp, uint32* pWF, const int32 iSizeBy2, const int32 S1, const int32 C1 ) 
{
    int32    CmS, CpS;
    int32 T2, T4, T6, T7, T8, T9, TA, TB;
    int32 D4, E4;

    int32    C2, S2, C3, S3;

    CmS = C1 - S1;                              // cnst4[i*6+4];
    CpS = C1 + S1;                              // cnst4[i*6+5];
    assert( BP2_FROM_FLOAT(1.0) <= CpS && CpS < BP2_FROM_FLOAT(1.5) );
    T8 = MULT_BP2X(CmS,pTmp[2]) + MULT_BP2X(CpS,pTmp[3]);     // F[2048+j]    F[3072-j]
    T6 = MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3]);     // F[2048+j]    F[3072-j]

    S2 = (MULT_BP2X(C1,S1)) << 1;                        // sin(2x)
    //C2 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S1,S1)); // cos(2x)
	C2 = (0x20000000 - (MULT_BP2X(S1,S1))) << 1; // cos(2x)
    CmS = C2 - S2;                              // cnst4[i*6+2];
    CpS = C2 + S2;                              // cnst4[i*6+3];
    T7 = MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1]);     // F[1024+j]    F[2048-j]
    T4 = MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1]);     // F[1024+j]    F[2048-j]

    S3 = MULT_BP2X(S1,C2) + MULT_BP2X(C1,S2);   // sin(3x) = sin(x+2x)
    C3 = MULT_BP2X(C1,C2) - MULT_BP2X(S1,S2);   // cos(2x) = cos(x+2x)
    CmS = C3 - S3;                              // old cnst4[i*6];
    CpS = C3 + S3;                              // old cnst4[i*6+1];
    T9 = MULT_BP2X(CmS,pTmp[4]) + MULT_BP2X(CpS,pTmp[5]);     // F[3072+j]    F[4096-j]
    T2 = MULT_BP2X(CpS,pTmp[4]) - MULT_BP2X(CmS,pTmp[5]);     // F[3072+j]    F[4096-j]

    TA = pTmp[6] + pTmp[7];                                   // F[j]      +  F[1024-j];
    TB = pTmp[6] - pTmp[7];                                   // F[j]      -  F[1024-j];

    D4  = DIV2(+ T7  + T8 + T9 + TA);
    E4  = DIV2(+ T4  + T6 + T2 + TB);
    pWF[k]           = InverseQuadRootOfSumSquares(D4, E4);              // F[j]

    D4  = DIV2(- T7  + T6 - T2 + TA);
    E4  = DIV2(+ T4  + T8 - T9 - TB);
    pWF[iSizeBy2-k]  = InverseQuadRootOfSumSquares(D4, E4);              // F[1024-j]

    D4  = DIV2(- T7  - T6 + T2 + TA);
    E4  = DIV2(- T4  + T8 - T9 + TB);
    pWF[iSizeBy2+k]  = InverseQuadRootOfSumSquares(D4, E4);              // F[1024+j]

    D4  = DIV2(- T4  + T6 + T2 - TB);
    E4  = DIV2(+ T7  - T8 - T9 + TA);
    pWF[(iSizeBy2<<1)-k]  = InverseQuadRootOfSumSquares(D4, E4);         // F[2048-j]

    //** This block uses 36 adds and 20 mults plus either 4 more mults or 4 shifts
}

static void WmaResampleWeightFactorLPC (WmaObject* pWmaDec, PerChannelInfo* ppcinfo)
{
    int32 iRatio;
    int32 i, j;

    uint32*  rguiWeightFactor = ppcinfo->pWeightFactor;
    
	// This should only be called for LPC mode. Bark mode resamples weight factor
    // from the original weight factor array to avoid losing information.
    assert(LPC_MODE == pWmaDec->weightingMode);

    if (pWmaDec->ratio_log2_prev < pWmaDec->ratio_log2_curr) {
        //downsample
        iRatio = pWmaDec->ratio_log2_curr - pWmaDec->ratio_log2_prev;
        i = 0;
        while (i < pWmaDec->cSubBand) {
            rguiWeightFactor [i] = rguiWeightFactor [i << iRatio];
            i++;
        }
    }
    else if (pWmaDec->ratio_log2_prev > pWmaDec->ratio_log2_curr) {
        //upsample
        iRatio = pWmaDec->ratio_log2_prev - pWmaDec->ratio_log2_curr;
        i = (pWmaDec->cSubBand >> iRatio) - 1;
        while (i >= 0) {
            for (j = 0; j < (1<<iRatio); j++)    {
                rguiWeightFactor [(i << iRatio) + j] = rguiWeightFactor [i];
            }
            i--;
        }                
    }    

    return;
}

static int32 Align2FracBits( const int32 iValue, const int32 cFracBits, const int32 cAlignFracBits )
{
    const int32 iShift = cFracBits-cAlignFracBits;
	assert (cAlignFracBits == TRANSFORM_FRACT_BITS) ; // wzhong 110907
    if (iShift < 0) {
        return (iValue << -iShift);
    }
	else if (iShift < 32) {
		return (iValue >> iShift);
    }
	else {
        return 0;
    }
}

static FastFloat ffltMultiply( FastFloat ffltA, FastFloat ffltB )
{
	FastFloat ffltP;

	ffltP.iFraction = MULT_HI( ffltA.iFraction, ffltB.iFraction );
	ffltP.iFracBits = (ffltA.iFracBits + ffltB.iFracBits - 31);
#if 1
	NormU32 (&ffltP.iFraction, &ffltP.iFracBits, 0x3fffffff);		// wzhong, 110907, essentailly same normalization
#else
	Norm4FastFloat( &ffltP );
#endif
	return ffltP;
}

// wzhong 11/01/07
// use RISC
// upto 25 bark band, no optimization needed
static FastFloat FastFloatFromU64(uint64 u64, int32 cExp) 
{
    FastFloat fflt;
    uint32 uiMSF = (uint32)(u64>>32);
    int32 iExp = 0;

    if ( uiMSF==0 ) {
        iExp = 32;
        uiMSF = (uint32)u64;
    }
    if (uiMSF==0) {
        fflt.iFracBits = 0;
        fflt.iFraction = 0;
        return fflt;
    }
    // normalize the most significant fractional part
    while( (uiMSF & 0xF0000000)==0 ) {
        iExp += 3;
        uiMSF <<= 3;
    }
    while( (uiMSF & 0xC0000000)==0 ) {
        iExp++;
        uiMSF <<= 1;
    }
    // number of fractional bits
    fflt.iFracBits = iExp+cExp-32;

    fflt.iFraction = (iExp>32) ? (uint32)(u64<<(iExp-32)) : (uint32)(u64>>(32-iExp));

    return fflt;
}   

void Norm4FastFloatU( FastFloat* pfflt )
{   // same as above when we know value is positive (which we often do)
    register uint32 uiF = pfflt->iFraction;
    register int32 iFB = 0;
	
    assert( uiF > 0 );

    while ( uiF < 0x1FFFFFFF ) {
        uiF  <<= 2;
        iFB +=  2;
    }
    if ( uiF < 0x3FFFFFFF ) {
        uiF  <<= 1;
        iFB +=  1;
    }
    pfflt->iFraction = uiF;
    pfflt->iFracBits += iFB;
}

// 5.4.3
// wzhong 11/01/07
// use RISC, upto 25 bark band
#define U64_SQUARE_I32(x) (((uint64)(x)) * ((uint64)(x))) 
static FastFloat ffltAvgSquareWeights(uint32* pwtWeightFactor, int32 cN )
{
    uint64 u64Sum = 0;
    FastFloat fflt;
    int32 i = cN;
	for ( ; i > 0; i--, pwtWeightFactor++) {
        u64Sum += U64_SQUARE_I32(*pwtWeightFactor);
    }
    // convert to a fastfloat and then divide.  This avoids an int64 division which is
    // really slow on the sh 3 and quite slow on other risc/dsp machines
    // cN varies from 1 to 328 so this can lose 8.3 bits of precision.
    // but we are comparing to float implementation which has only 24 bits of mantissa.
    fflt = FastFloatFromU64( u64Sum, (WEIGHTFACTOR_FRACT_BITS<<1) );
    fflt.iFraction /= cN;
    Norm4FastFloatU( &fflt );
    return fflt;
}

// wzhong, 11/1/07
// upto 25 bark band, use RISC, no futher optimization needed
static FastFloat ffltSqrtRatio( FastFloat fflt1, FastFloat fflt2 )
{
    uint64 u64Ratio;
    FastFloat fflt;
    int32 iMSF8;
    uint32 uiMSF;
    uint32 uiFrac1;
    int32 iExp = 0;

    if ( fflt2.iFraction==0 ) {
        assert( fflt2.iFraction != 0 ); // divide by 0
        fflt.iFraction = 0x7FFFFFFF;
        fflt.iFracBits = 0;
        return fflt;
    }
    //// fflt1.iFraction*0.5F/(1<<(fflt1.iFracBits-1))
    //// fflt2.iFraction*0.5F/(1<<(fflt2.iFracBits-1))

	// wzhong, how to do 64 bit division?
    u64Ratio = (((uint64)fflt1.iFraction)<<32)/fflt2.iFraction;
    uiMSF = (uint32)(u64Ratio>>32);
    if ( uiMSF==0 ) {
        iExp = 32;
        uiMSF = (uint32)u64Ratio;
    }
    assert( uiMSF != 0 ) ;
    // normalize the most significant fractional part
    while( (uiMSF & 0xF0000000)==0 ) {
        iExp += 4;
        uiMSF <<= 4;
    }
    while( (uiMSF & 0x80000000)==0 ) {
        iExp++;
        uiMSF <<= 1;
    }
    // discard the most significant one bit (it's presence is built into g_InvQuadRootFraction)
    iExp++;
    // get all 32 bits from source

	uiMSF = (iExp>32) ? (uint32)(u64Ratio<<(iExp-32)) : (uint32)(u64Ratio>>(32-iExp));

    // split into top SQRT_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
    iMSF8 = uiMSF>>(32-SQRT_FRACTION_TABLE_LOG2_SIZE);
    uiMSF <<= SQRT_FRACTION_TABLE_LOG2_SIZE;
    // lookup and interpolate
    uiFrac1  = g_SqrtFraction[iMSF8++];   // BP2
    uiFrac1 += MULT_HI_UDWORD( uiMSF,  g_SqrtFraction[iMSF8] - uiFrac1 );
    // adjust by sqrt(1/2) if expoenent is odd
    if ( (iExp+fflt1.iFracBits-fflt2.iFracBits) & 1 ) {
        // multiply by 1/sqrt(2) and adjust fracbits by 1/2
		uint32 temp = UBP0_FROM_FLOAT(0.70710678118654752440084436210485);
        uiFrac1 = MULT_HI_UDWORD( uiFrac1,  temp);
        fflt.iFracBits = ((fflt1.iFracBits-(fflt2.iFracBits+1))>>1)+(iExp-3);
    } else {
        fflt.iFracBits = ((fflt1.iFracBits-fflt2.iFracBits)>>1)+(iExp-3);
    }
    fflt.iFraction = uiFrac1>>1;    // make sure sign is positive
    //// fflt.iFraction*0.5F/(1<<(fflt.iFracBits-1))
	Norm4FastFloatU( &fflt );
    return fflt;
}

// 5.4.3
// wzhong 11/01/07
// use RISC, upto 25 bark band
static void WmaGetBandWeightLowRate (WmaObject* pWmaDec)
{
	uint8*	rgbBandNotCoded;
    int32 iBand, iStart, iEnd, ch;
	uint8 cNoiseBand;
    
    //calculating band weight
    for (ch = 0; ch < pWmaDec->num_channels; ch++) {
		PerChannelInfo *pChnl =  &pWmaDec->channelInfo[ch];
        if (pChnl->power) {
		    FastFloat rgfltBandWeight[25];
			FastFloat *rgffltSqrtBWRatio = pChnl->pffltSqrtBWRatio;
            uint32 *rgxWeightFactor  = pChnl->pWeightFactor;
			BarkIndex *pBarkIndex = &pWmaDec->barkInfo[pWmaDec->ratio_log2_curr];            

		    rgbBandNotCoded = pChnl->pBandNotCoded;

		    cNoiseBand = 0;
            iStart = 0, iEnd = 0;
            iBand = pWmaDec->currFirstNoiseBand;
            while (1) 
			{
                iStart = max2(pWmaDec->firstNoiseIndex, pBarkIndex->bark_index [iBand]);
                if (iStart >= pWmaDec->highCutOff)
                    break;

                assert (iBand < pBarkIndex->num_bark_bands);
                iEnd   = min2 (pWmaDec->highCutOff, pBarkIndex->bark_index [iBand+1]);
                assert (iEnd >= iStart);

			    if (rgbBandNotCoded [iBand]) {
                    // This may be slower than straight divide, but we must do this to
                    // remain consistent with prvGetNoiseLikeBandsMono
                    rgfltBandWeight [cNoiseBand] = ffltAvgSquareWeights( rgxWeightFactor+iStart, iEnd-iStart );
                    //// rgfltBandWeight[cNoiseBand].iFraction*1.0F/(1<<rgfltBandWeight[cNoiseBand].iFracBits)
					
				    cNoiseBand++;
			    }
                //SHOW_NOISE_BANDS_ONCE;
                iBand++;
		    }
		    assert (cNoiseBand < pBarkIndex->num_bark_bands);
		    assert (pWmaDec->firstNoiseBand > 0);

			// wzhong, 
			// sqrt (...) / last_uncoded_band_weight
			for( iBand = 0; iBand < (cNoiseBand-1); iBand++ ) {
				// This calculation is not ideally suited to FastFloat, leave in float for time being
				// But note that limited range of the result probably means it can be done more quickly
                rgffltSqrtBWRatio[iBand] = ffltSqrtRatio( rgfltBandWeight[iBand], rgfltBandWeight[cNoiseBand-1] );
				//// float SqrtBWRatio = fflt.iFraction*1.0F/(1<<fflt.iFracBits)
			}

			if (cNoiseBand > 0) {
				// last band has a ratio of 1.0
				rgffltSqrtBWRatio[cNoiseBand-1].iFraction = 0x40000000;
				rgffltSqrtBWRatio[cNoiseBand-1].iFracBits = 30;				
			}
		    //rgbBandNotCoded [0] = cNoiseBand; //used as cNoiseBand
			assert (pChnl->numBandNotCoded == cNoiseBand);
        }
    }
    return;
}
