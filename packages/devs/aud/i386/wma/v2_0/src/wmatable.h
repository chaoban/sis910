#ifndef WMA_TABLE_H
#define WMA_TABLE_H

#if defined (WMA_TABLE_INIT)
	#define	EXTERN
#else
	#define	EXTERN extern
#endif //WMA_TABLE_INIT

#define MASK_MINUS_POWER_TABLE_SIZE 72

EXTERN const int32 rgiMaskMinusPower10[MASK_MINUS_POWER_TABLE_SIZE];
#define MASK_PLUS_POWER_TABLE_SIZE 50
EXTERN const int32 rgiMaskPlusPower10[MASK_PLUS_POWER_TABLE_SIZE];

#define DBPOWER_TABLE_SIZE 128
#define DBPOWER_TABLE_OFFSET 18

EXTERN const int32 rgDBPower10[DBPOWER_TABLE_SIZE];

EXTERN const uint16		g_rgiBarkFreq[];
EXTERN const uint16		g_rgiHuffDecTblMsk[226];

EXTERN const uint16		g_rgiHuffDecTbl16smOb[];
EXTERN const uint16	    gRun16smOb[];
EXTERN const uint16		gLevel16smOb[];

EXTERN const uint16	    g_rgiHuffDecTbl16ssOb[];
EXTERN const uint16		gRun16ssOb[];
EXTERN const uint16	    gLevel16ssOb[];

EXTERN const uint16		g_rgiHuffDecTbl44smOb[];
EXTERN const uint16		gRun44smOb[];
EXTERN const uint16		gLevel44smOb[];

EXTERN const uint16		g_rgiHuffDecTbl44ssOb[];
EXTERN const uint16		gRun44ssOb[];
EXTERN const uint16		gLevel44ssOb[];

EXTERN const uint16		g_rgiHuffDecTbl44smQb[];
EXTERN const uint16		gRun44smQb[];
EXTERN const uint16		gLevel44smQb[];

EXTERN const uint16		g_rgiHuffDecTbl44ssQb[];
EXTERN const uint16		gRun44ssQb[];
EXTERN const uint16		gLevel44ssQb[];

EXTERN const uint16		g_rgiHuffDecTblNoisePower[];

#define INVQUADROOT_FRACTION_TABLE_LOG2_SIZE	8
#define INVQUADROOT_FRACTION_TABLE_SIZE			(1<<INVQUADROOT_FRACTION_TABLE_LOG2_SIZE)

EXTERN const uint32 g_InvQuadRootFraction[1+INVQUADROOT_FRACTION_TABLE_SIZE];
EXTERN const uint32 g_InvQuadRootExponent[BITS_LP_SPEC_POWER+1];

#define INVERSE_FRACTION_TABLE_LOG2_SIZE		8
#define INVERSE_FRACTION_TABLE_SIZE				(1<<INVERSE_FRACTION_TABLE_LOG2_SIZE)
EXTERN const uint32 g_InverseFraction[1+INVERSE_FRACTION_TABLE_SIZE];

#define SQRT_FRACTION_TABLE_LOG2_SIZE			8
#define SQRT_FRACTION_TABLE_SIZE				(1<<SQRT_FRACTION_TABLE_LOG2_SIZE)
EXTERN const uint32 g_SqrtFraction[1+SQRT_FRACTION_TABLE_SIZE];


EXTERN const int32 g_rgiLsfReconLevel [LPCORDER] [16];

#define PWRTORMS_ZEROPT							60
#define PWRTORMS_SIZE							(PWRTORMS_ZEROPT*2 + 1)
EXTERN const uint32								g_rgfiPwrToRMS[];

#define SINCOSTABLE_ENTRIES 17
EXTERN const SinCosTable *rgSinCosTables[SINCOSTABLE_ENTRIES];

EXTERN const SinCosTable g_sct64;
EXTERN const SinCosTable g_sct128;
EXTERN const SinCosTable g_sct256;
EXTERN const SinCosTable g_sct512;
EXTERN const SinCosTable g_sct1024;
EXTERN const SinCosTable g_sct2048;

EXTERN const int32 trig_2048[]; 
EXTERN const int32 trig_1024[]; 
EXTERN const int32 trig_512[]; 
EXTERN const int32 trig_256[]; 
EXTERN const int32 trig_128[]; 
//#define TRIGTBL(i,j) lpc_trig2048[((i)<<(iShrink+1))+(j)]
#define TRIGTBL(i,j) trig_table[((i)<<1)+(j)]

#endif //WMA_TABLE_H

