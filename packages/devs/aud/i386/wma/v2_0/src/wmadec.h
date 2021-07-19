#ifndef WMA_DEC_H
#define	WMA_DEC_H

#include <math.h>
#include <cyg/io/aud/adecctrl.h>

//#define	VPRINTF(a) {printf a;}
#define	ALIGN_32(a) a __attribute__ ((aligned (32)))

#define MAX_SUBFRAMES       16
#define	own
#define	nonown

#ifdef  PI
#undef  PI
#endif

#define PI					3.1415926535897932384626433832795
#define	ROOT2				1.4142135623730950488016887242097
#define	HALF_ROOT2			0.70710678118654752440084436210485

#ifdef	BITS_PER_BYTE
#undef	BITS_PER_BYTE
#endif
#define BITS_PER_BYTE		8

#ifdef	BITS_PER_DWORD
#undef	BITS_PER_DWORD
#endif
#define BITS_PER_DWORD      32

#ifdef	NBITS_PACKET_CNT
#undef	NBITS_PACKET_CNT
#endif
#define NBITS_PACKET_CNT    4

#ifdef	NBITS_FRM_CNT
#undef	NBITS_FRM_CNT
#endif
#define NBITS_FRM_CNT       4


#ifdef  NUM_BARK_BAND
#undef  NUM_BARK_BAND
#endif
#define NUM_BARK_BAND		25

#ifdef  LOW_CUTOFF_V1
#undef  LOW_CUTOFF_V1
#endif
#define LOW_CUTOFF_V1		3        //TBD

#ifdef  LOW_CUTOFF
#undef  LOW_CUTOFF
#endif
#define LOW_CUTOFF					0


#define MIN_THRESHOLD_IN_DB_V1		25
#define MASK_QUANT 2.5

#define MIN_MASKQ_IN_DB_V1 (int32) (MIN_THRESHOLD_IN_DB_V1 / MASK_QUANT)

#define NBITS_FIRST_MASKQUANT		5

#define FIRST_V2_MASKQUANT			36

#define MAX_MASKQUANT_DIFF			60  // limited by the huffman table

#define NUM_BITS_QUANTIZER			7

#define MAX_QUANT					129	/* 138 has been seen by decoder */

#define MIN_QUANT					1		/*	was once 10; 22 is lowest value seen by decoder */

#define FOURTYFOURS_QB				1
#define FOURTYFOURS_OB				2
#define SIXTEENS_OB					3

#define MIN_AUDIBLE					0   // should be fixed...


#define MASK_POWER_FRAC_BITS 28
#define MASK_POWER_SCALE (1<<MASK_POWER_FRAC_BITS)
#define MASK_POWER_FROM_FLOAT(flt) ((int32)((flt)*MASK_POWER_SCALE))
#define DOUBLE_FROM_QUANTSTEPTYPE(qst) DOUBLE_FROM_FASTFLOAT(qst)

// 7 Table
// 7.1 constant Table 14 Constants used by the decoder
#define ENCOPT_BARK					0x0001
#define ENCOPT_V5LPC				0x0020
#define ENCOPT_SUPERFRAME			0x0002
#define ENCOPT_SUBFRAME				0x0004

#define ENCOPT_SUBFRAMEDIVMASK		0x0018
#define ENCOPT_SUBFRAMEDIVSHR		3

#define LPCORDER					10
#define MASK_RESAMPLE_OFFSET		6
#define MAX_QUANT_INCR				127
#define MIN_FRAME_SIZE				128
#define STEREO_WEIGHT			1.6


// Constants
#define QUANTSTEP_DENOMINATOR		20
#define QUANTSTEP_FRACT_BITS		28

#define TRANSFORM_FRACT_BITS		5

// fix-point binary points conversion factors
// convert float or double to BP1 integer ( -1.0 <= x < +1.0 )
#define NF2BP1						0x7FFFFFFF
// convert float or double to BP2 integer ( -2.0 <= x < +2.0 )
#define NF2BP2						0x3FFFFFFF

#define HI(a)	( ((uint32) (a)) >>16 )
#define LO(a)	( (uint32) (a) & 0xffff)



#define MULT_HI_UDWORD(a,b) (uint32)((((uint64)(a))*((uint64)(b)))>>32)	
#define MULT_HI_UDWORD2(a,b) (uint32)((((uint64)(a)>>1)*((uint64)(b)>>1))>>30)	

//#define MULT_HI(a,b) (MULT_HI_DWORD(a,b)<<1)
#define MULT_HI(a,b)  ((int32)((((int64)(a))*((int64)(b)))>>31 >> 1)) << 1

#define	MAX_DCTIV_SIZE			2048
#define	MAX_DCTIV_SIZE_LOG2		11

#define	MAX_FFT_SIZE			(MAX_DCTIV_SIZE/2)			// 1024
#define	MAX_FFT_SIZE_LOG2		(MAX_DCTIV_SIZE_LOG2-1)		// 10

#define COEF_FRAC_BITS 5
#define COEF_FRAC_SCALE (1<<COEF_FRAC_BITS)
#define COEF_FROM_FLOAT(flt) ((CoefType)(flt*COEF_FRAC_SCALE))
#define FLOAT_FROM_COEF(coef) (coef/((Float)COEF_FRAC_SCALE))
#define COEF_FROM_INT(i) ((i)<<COEF_FRAC_BITS)
#define INT_FROM_COEF(cf) ((cf)>>COEF_FRAC_BITS)


//  11/14/06 Wei Zhong, both positive
#define MULT_HI_DWORD(a,b) (int32)((((int64)(a))*((int64)(b)))>>32)
#define MULT_QUANT(iLevel,ffltQuantizer) ((MULT_HI_DWORD (((iLevel)<<16),ffltQuantizer.iFraction)) << 1)


#define SCALE_COEF_RECON(iCR) (iShift<0) ? (iCR)<<-iShift : (iCR)>>iShift
#define MAKE_MASK_FOR_SCALING(iFB) iShift=iFB+(16-31-TRANSFORM_FRACT_BITS)

#define MULT_HI_DWORD_DOWN(a,b) (int32)((((int64)(a))*((int64)(b)))>>30)			// 30 is because all sin cos is in the range of 1<<30
#define MULT_BP2(a,b) MULT_HI_DWORD_DOWN((a),(b))

#   define DIV2(a) ((a)>>1)
#   define MUL2(a) ((a)<<1)
#   define DIV4(a) ((a)>>2)
#   define MUL4(a) ((a)<<2)
#   define MUL8(a) ((a)<<3)

/// LPC

typedef struct 
{
    int32 sin_PIby4cSB;		// sin( pi/(4*cSB) ) * nF2BP1 for DCT and inverseTransform
    int32 cos_PIby4cSB;		// cos( pi/(4*cSB) ) * nF2BP1 for DCT and inverseTransform
	int32 sin_3PIby4cSB;		// sin( 3*pi/(4*cSB) ) * nF2BP1 for DCT
	int32 cos_3PIby4cSB;		// cos( 3*pi/(4*cSB) ) * nF2BP1 for DCT
	int32 sin_PIbycSB;		// sin( pi/cSB ) * nF2BP1 for DCT
	int32 cos_PIbycSB;		// cos( pi/cSB ) * nF2BP1 for DCT
	int32 sin_2PIbycSB;		// sin( pi/(cSB/2) ) * nF2BP1 for FFT
	int32 cos_2PIbycSB;		// cos( pi/(cSB/2) ) * nF2BP1 for FFT
	int32 sin_PIby2cSB;		// sin( pi/(2*cSB) ) * nF2BP1 for inverseTransform
	int32 cos_PIby2cSB;		// cos( pi/(2*cSB) ) * nF2BP1 for inverseTransform
	int32 two_sin_PIbycSB;	// 2 * sin( pi/cSB ) * nF2BP1 for DCT
    int32 sin_4PIbycSB;       // sin( 4*pi/cSB ) * nF2BP2 for LPC
    int32 cos_4PIbycSB;       // sin( 4*pi/cSB ) * nF2BP2 for LPC
}SinCosTable;

#   define FRACT_BITS_LSP 30
#   define LSP_SCALE (1<<FRACT_BITS_LSP)
#   define LSP_FROM_FLOAT(a) ((int32)((a)*LSP_SCALE))
#   define MULT_LSP(a,b) (int32)((((int64)(a))*((int64)(b)))>>FRACT_BITS_LSP)

#define FRACT_BITS_LP 27

#   define LP_SCALE (1<<FRACT_BITS_LP)
#   define FLOAT_FROM_LP(a) ((float)((a)*(1.0f/LP_SCALE)))
#   define LP_FROM_FLOAT(a) ((int32)((a)*LP_SCALE))

#define LSF_DECODE(i,lsfQ) g_rgiLsfReconLevel[i][lsfQ[i]]
//#define LP_DECODE(i,lsfQ) LSF_DECODE((i),(lsfQ))
#define LP_DECODE(i,lsfQ) (g_rgiLsfReconLevel[i][lsfQ[i]] >> (FRACT_BITS_LSP-FRACT_BITS_LP))


#   define BP1_FRAC_BITS 31
#   define BP1_FRAC_SCALE NF2BP1
#   define BP1_FROM_FLOAT(flt) ((int32)(flt*BP1_FRAC_SCALE))
#   define FLOAT_FROM_BP1(bp1) (bp1/((Float)BP1_FRAC_SCALE))
#   define MULT_BP1(a,b) MULT_HI((a),(b))

    
#   define BP2_FRAC_BITS 30
#   define BP2_FRAC_SCALE NF2BP2
#   define BP2_FROM_FLOAT(flt) ((int32)(flt*BP2_FRAC_SCALE))
#   define FLOAT_FROM_BP2(bp2) (((float)bp2)/((float)BP2_FRAC_SCALE))
#   define BP2_FROM_BP1(bp1) ((bp1)>>1)
#   define MULT_BP2(a,b) MULT_HI_DWORD_DOWN((a),(b))

#define MASK_POWER_FRAC_BITS 28
#define MASK_POWER_SCALE (1<<MASK_POWER_FRAC_BITS)
#define MASK_POWER_FROM_FLOAT(flt) ((int32)((flt)*MASK_POWER_SCALE))
#define FLOAT_FROM_MASK_POWER(typ) ((typ)/((float)MASK_POWER_SCALE))


#define DBPOWER_FRAC_BITS QUANTSTEP_FRACT_BITS
#define DBPOWER_FROM_FLOAT(flt) ((int32)((flt)*(1<<DBPOWER_FRAC_BITS)))
#define FLOAT_FROM_DBPOWER(typ) ((typ)*(((float)1.0)/(1<<DBPOWER_FRAC_BITS)))

#define DOUBLE_FROM_FASTFLOAT(fflt) ((double)fflt.iFraction/(1<<fflt.iFracBits))
#       define SQRT2_2  BP2_FROM_FLOAT(0.70710678118654752440084436210485)
#       define SQRT2    BP2_FROM_FLOAT(1.4142135623730950488016887242097)

#define FRACT_BITS_LP_SPEC 25
typedef uint64 LpSpecPowerType;
#define BITS_LP_SPEC_POWER 64
#define FRACT_BITS_LP_SPEC_POWER 30 
#define QR_FRACTION_FRAC_BITS 24
#define QR_EXPONENT_FRAC_BITS 29

#define LP_SPEC_SCALE (1<<FRACT_BITS_LP_SPEC)
#define LP_SPEC_POWER_SCALE ((float)(1L<<FRACT_BITS_LP_SPEC_POWER))
#define LP_SPEC_FROM_FLOAT(a) ((int32)((a)*LP_SPEC_SCALE))
#define LP_SPEC_POWER_FROM_FLOAT(a) ((int32)((a)*LP_SPEC_POWER_SCALE))
#define FLOAT_FROM_LP_SPEC(a) ((float)((a)*(1.0f/LP_SPEC_SCALE)))
#define LP_SPEC_FROM_LP(a) ((int32)((a)>>(FRACT_BITS_LP-FRACT_BITS_LP_SPEC)))

#define WEIGHTFACTOR_FRACT_BITS     21
#define WEIGHT_FROM_FLOAT(a) ((uint32)((a)*(1<<WEIGHTFACTOR_FRACT_BITS)))
#define FLOAT_FROM_WEIGHT(a) ((Float)((a)*(1.0f/(1<<WEIGHTFACTOR_FRACT_BITS))))

#define UBP0_FROM_FLOAT(x) ((uint32)(x*4294967296.0F))


 





typedef enum {
	LPC_MODE	= 0,
	BARK_MODE	= 1,
} WeightMode;

typedef enum {
	STEREO_INVALID		= -1,
	STEREO_LEFTRIGHT	= 0,
	STEREO_SUMDIFF
} StereoMode;

typedef enum
{
	CODEC_NULL,
	CODEC_STEADY,
	CODEC_DONE,
	CODEC_BEGIN,
	CODEC_LAST,
	CODEC_ONHOLD
} Status;

typedef enum
{
	FFT_FORWARD		= 0,
	FFT_INVERSE		= 1,
} FftDirection;

typedef struct
{
	//I8 exponent;  Old exponent was shift from binary point at 24th position
	int32	iFraction;
    int32	iFracBits;     
} FastFloat;

typedef struct _bark_mask_ {
	uint16		maskQ;
	FastFloat	fMaskQ;
} BarkMask;

typedef struct _PerChannelInfo_
{
//	BarkMask			barkMaskQ [25]; // modify by tcx 2008.04.17
	int				barkMaskQ[25];
	FastFloat			fbarkMaskQ[25];

	uint16				maxMaskQ;

	
	uint8				numBandNotCoded;    
	uint8				pBandNotCoded[25];			//band not coded, maxim is 25 bands
													// total sum of pBandNotCoded[i] equal to numBandNotCoded;

	/////////////////////////////////////////////////////////////////////
	// for the 2 elements
	// if the numBandNotCoded == 2
	// then only pffltSqrtBWRatio[0..1] and pNoisePower[0..1] are valid

	FastFloat			pffltSqrtBWRatio[25];		// max is 25 band													
	int8				pNoisePower[25];			// max is 25 band
	/////////////////////////////////////////////////////////////////////
	
    nonown const uint16*		pHuffDecTbl;                //decoder only
    nonown const uint16*		pRunEntry;                  //decoder only
    nonown const uint16*		pLevelEntry;                //decoder only
    uint16				cSubbandActual;
    uint8				power;                      //channel power, is_channel_coded
//    int32				actualPower;                // Power as suggested by CoefQ/CoefRecon
    nonown uint32*		pWeightFactor;				//weighting factor

	nonown int32*		pCoefRecon;					// reconstructed coef
	nonown int32*		pNextOutput;				// max 3072 * 32-bit, eg prev output
	nonown int32*		pCurrOutput;				// max 3072 * 32
	nonown int32*		pPrevOutput;

#if !defined (_MSC_VER)
    // use int32 instead of int32*
    int32               upOut[2];
    int32               upOutSample[2];
    int32               horOut[2];
    int32               horOutSample[2];
#endif
    uint32				maxWeight;
	
	/// LPC
	
} PerChannelInfo, *PPerChannelInfo;

/*
typedef struct _SubFrameConfigInfo_
{
    int32 subFrame;			// how many valid sub frames, for example,
							// if total 16 subframe, then count from 0 to 16
    int32 subFrameSize		[MAX_SUBFRAMES];	// == subband
    int32 subFrameStart		[MAX_SUBFRAMES];
    int32 maskUpdate		[MAX_SUBFRAMES];
} SubFrameConfigInfo, *PSubFrameConfigInfo;
*/
typedef struct tagRandState {
	int32 iPrior;         // prior value
	uint32 uRand;         // current value
} tRandState;

typedef struct {
	uint8	num_bark_bands;
	uint16	bark_index [25];
} BarkIndex;

typedef struct _Wma_Object_{    
    //int32		packetCurr;
    uint8		bitPackedFrameSize;
    uint8		bitPacketHeader;
    //int32		bitPacketLength;		// how many audio bits in a ASF packet

    //
	//int32		highCutOffCurr;
    //int32		nextBarkIndex;
    uint8		noiseSub;

	// 3.6
    float		fltBitsPerSample;
    float		fltWeightedBitsPerSample;

    uint8		max_escape_size;				// in bits
    uint16		max_escape_level;				// (1<<max_escape_size)

	Status		codecStatus;

	/// input parameters
    int32		version_number;				// 1 or 2
    int32		samples_per_second;
    uint16		num_channels;
    uint32		bytes_per_second;
	uint32		bits_per_packet;
	uint16		encoder_options;
	uint32		bytes_per_packet;

    uint16		subBand;				// max subBand, eg 2048
	uint16		cSubBand;				// current subBand for sub frame

    uint8		allowSuperFrame;
    uint8		allowSubFrame;

	uint32		currFrame;				/// global frame count
	uint8		currFrameInPacket;
    uint8		currSubFrame;			//which subframe we are doing
	uint8		hasSubFrame;
	uint8		update_mask;
	uint8		ratio_log2_curr_with_update;	// last subfrmame size when mask is updated, == subband

    uint8		maxSubFrameDiv;			//maximum no. division into subwindows

    uint16		minSubFrameSampleHalf;  //min size of subframes in current frame; many change from frame to frame

	uint8		log2_sz_raio;
	uint8		possibleWinSize;		// for 2048, it could be 5, since subframe can varies between 2^(0-4)
    //uint8		incr;					// iIncr could be 0-4

	// note
	// if the subframesize is 2048 (max)
	// subFrameSample		= 4096
	// subFrameSampleHalf	= 2048
    uint16		subFrameSample;			//for long win, same as cFrameSample, updated for each subframe
    uint16		subFrameSampleHalf;
    uint16		subFrameSampleQuad;

    //SubFrameConfigInfo  subfrmconfigPrev;
    //SubFrameConfigInfo  subfrmconfigCurr;
    //SubFrameConfigInfo  subfrmconfigNext;

    //should be reset every subfrm in next version
    uint8		bitsSubbandMax;	    //no. of bits for cSubband;
									// 11 for 2048
    uint16		frameSample;        //no. of input/output samples = 2 * cSubband
    uint16		frameSampleHalf;    //no. of input/output samples = cSubband
    uint16		frameSampleQuad;    //no. of input/output samples = cSubband / 2

    uint16		lowCutOff;          //How many coeffs to zero at the low end of spectrum
    uint16		highCutOff;			//How many coeffs to zero at the high end of spectrum
    uint16		lowCutOffLong;
    uint16		highCutOffLong;

    uint8		weightingMode;
	uint8		fV5Lpc;

    StereoMode  stereo_mode;		// left-right or sum-difference coding
    uint8		entropyMode;        //entropy coding mode
    float		fltDitherLevel;	    //=.12; Relative intensity of Roberts' pseudo-random noise quantization	
	tRandState	randState;


	int16		quantStepSize;
	FastFloat	fQuantStep;

	float		fltDctScale;

	uint16		rampUpSize;			// Min (prev, curr)
	uint16		rampDownSize;		// Min (curr, next)
	uint16		rampHorSize;		// Q3 - Q2

    uint16		sizePrev;			// windows size, == cSubband
    uint16		sizeCurr;
    uint16		sizeNext;
	uint16		sizeAccr;			// accrued with a frame

    uint16		coefRecurQ1;
    uint16		coefRecurQ2;
    uint16		coefRecurQ3;
    uint16		coefRecurQ4;

    PerChannelInfo  channelInfo[2];

	///////////////////////////////////////////////////////////////
	/// buffers, output
	int32		currOutputSamples;		// Q3-Q1
	own int32*	pCoefRecon [2];			// int [2048], store coef, dquantized and DCTIV results
	own	int32	*pCoefLowMidRate;		// coef before inv-quant for low and mid rate

	// max 3*2048/2
	uint8		pingpong;
	own int32*  currOutput[2];
	own int32*	pingPrevOutput [2];		// int [3*2048], store output after windowing
	own int32*	pongPrevOutput [2];		// inte [2048], ramp down windowed

	///////////////////////////////////////////////////////////////

	uint8		ratio_log2_prev, ratio_log2_curr, ratio_log2_next;

	BarkIndex	barkInfo [5];								// 0-4 window

	//int32		validBarkBand;			// current bank band, usually 25?

    float		firstNoiseFreq;
    int8		firstNoiseBand[5];		// maxim 5 window
	int8		currFirstNoiseBand;
    int16		firstNoiseIndex;	
    int32		noisePeakIgnoreBand;	// Num of coefficients to ignore when detecting peaks during noise substitution

    own uint32*		pWeightFactor;			// share with rgiMaskQ when bark
										// int [subband * channel]
    //uint32		frameNumber;			// frame counter to the common code

	// function pointers
	uint32		(*getBits) (uint32);
	uint32		(*peekBits) (uint32);
	uint32		(*byteAlign) (void);
	uint32		(*tellBits) (void);
#if defined (SISPKG_IO_AUD_DEC_WMA_DEBUG) && 1
	FILE		*fpWav;    
#endif //_MSC_VER

//#if !defined (_MSC_VER)
    AUD_tCtrlBlk    *pDecCtrl;
//#endif //!defined (_MSC_VER)
} WmaObject;


int WmaDecInit (WmaObject*	pWmaDec,
                int32		version_number,
                int32		samples_per_second,
                uint16		num_channels,
                int32		bytes_per_second,
                int32		bits_per_packet,		// wzhong, seems constant packet size
                uint16		encoder_options
                );

int WmaDecEngine (WmaObject*	pau);
int WmaDecFrame (WmaObject*	pau);
int WmaDecSubFrame (WmaObject*	pau);

int WmaDecDestroy (WmaObject*	pWmaDec);
//int32 ROUNDF(float f);
int32 LOG2(int32 i);
void NormU32( uint32* puiValue, uint32* pcFracBits, const uint32 uiTarget );
void Norm4FastFloat( FastFloat* pfflt );


///////////////////////////////////////////////////////////////////////////
// for Mid Rate Inverse Quantize
///////////////////////////////////////////////////////////////////////////

#endif	//WMA_DEC_H
