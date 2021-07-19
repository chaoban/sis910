#include <stdio.h>
#include <stdlib.h>

//#include "common.h"
#include "common.h"

#include "wmadec.h"
#include "wmabts.h"
#include "wmaentropy.h"


int WmaDecHuf (WmaObject* pWmaDec, const uint16 *pHufTab, uint32 *val)
{
#define	FIRST_LOAD	10
#define	SECOND_LOAD 12

	uint32 ret_value;
	const uint16* node_base = pHufTab;

	uint32 uBits, bitCnt;
	uint32 codeword;
	uint32 i;

	uBits = pWmaDec->peekBits (FIRST_LOAD + SECOND_LOAD + 1);

	uBits <<= (32- (FIRST_LOAD + SECOND_LOAD + 1));
	codeword = uBits;

     // Do first five 2-bit tables
     i = 5;
     do
     {
         node_base += ((codeword & 0xC0000000) >> 30);  // Use top 2 bits as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 2;            // Advance to next 2 bits
         i -= 1;
     } while (i > 0);

     // Do remaining three 2-bit tables
     i = 3;
     do
     {
         node_base += ((codeword & 0xC0000000) >> 30);  // Use top 2 bits as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 2;            // Advance to next 2 bits
         i -= 1;
     } while (i > 0);

     // Do six 1-bit tables
     i = 6;
     do
     {
         node_base += ((codeword & 0x80000000) >> 31);  // Use top bit as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 1;            // Advance to next bit
         i -= 1;
     } while (i > 0);


decode_complete:
     assert(ret_value & 0x8000);
     bitCnt = ((ret_value >> 10) & (0x0000001F));
	 pWmaDec->getBits (bitCnt);
     *val = ret_value & 0x000003FF;

	 if (*val >= 0x03FC)
         *val = *(node_base + (*val & 0x0003) + 1);

	 return 0;
}


int WmaDecRunLevelHighRate (WmaObject* pWmaDec, uint8 ch)
{
	PerChannelInfo* pChnlInfo = &pWmaDec->channelInfo[ch];
	uint16 level, run, localRun=0, last=0;
	int32  iSign, *pCoefRecon = pChnlInfo->pCoefRecon;
	uint32 val;
	int32 iShift;
	FastFloat fQuant;
	BarkIndex	*pBarkIndex = &pWmaDec->barkInfo[pWmaDec->ratio_log2_curr_with_update];
	uint16 currBark=0, NextBarkBand;

	assert (pChnlInfo->power);
	NextBarkBand = pBarkIndex->bark_index[currBark+1];
	run = localRun = pWmaDec->lowCutOff;

	//fQuant = pChnlInfo->barkMaskQ[currBark].fMaskQ; // modify by tcx 2008.04.17
	fQuant = pChnlInfo->fbarkMaskQ[currBark];
	
	MAKE_MASK_FOR_SCALING(fQuant.iFracBits);

	while (localRun < min2 (pWmaDec->highCutOff, pChnlInfo->cSubbandActual) && !last)
	{
		int32 band = 0;

		WmaDecHuf (pWmaDec, pChnlInfo->pHuffDecTbl, &val);

		switch (val) {
		case END_OF_BLOCK:
			last = 1;
			//VPRINTF (("last\n"));
			break;
		case ESCAPE_CODE:
			//VPRINTF (("escape\n"));
			level = pWmaDec->getBits (pWmaDec->max_escape_size);
			run   = pWmaDec->getBits (pWmaDec->bitsSubbandMax);
			iSign  = pWmaDec->getBits (1)-1;
			break;
		default:
			level = pChnlInfo->pLevelEntry [val-2];
			run	  = pChnlInfo->pRunEntry [val-2];
			iSign  = pWmaDec->getBits (1)-1;
			break;
		}
		if (!last) {
			uint32 coefRecon, scaleLocalRun, scaleNextBarkBand;
			//int32 val;

			localRun += run;

			if (pWmaDec->ratio_log2_curr_with_update < pWmaDec->ratio_log2_curr) {
				scaleLocalRun = localRun << (pWmaDec->ratio_log2_curr-pWmaDec->ratio_log2_curr_with_update);
				scaleNextBarkBand = NextBarkBand;
			}
			else {
				scaleLocalRun = localRun;
				scaleNextBarkBand = NextBarkBand << (pWmaDec->ratio_log2_curr_with_update-pWmaDec->ratio_log2_curr);
			}

			while (scaleLocalRun >=  scaleNextBarkBand) {
				currBark ++;
				NextBarkBand = pBarkIndex->bark_index[currBark+1];
				//fQuant = pChnlInfo->barkMaskQ[currBark].fMaskQ; // modify by tcx 2008.04.17
				fQuant = pChnlInfo->fbarkMaskQ[currBark];
				
				MAKE_MASK_FOR_SCALING(fQuant.iFracBits);

				if (pWmaDec->ratio_log2_curr_with_update < pWmaDec->ratio_log2_curr) {
					scaleNextBarkBand = NextBarkBand;
				}
				else {
					scaleNextBarkBand = NextBarkBand << (pWmaDec->ratio_log2_curr_with_update-pWmaDec->ratio_log2_curr);
				}
			}

			coefRecon = MULT_QUANT(level, fQuant);
			coefRecon = SCALE_COEF_RECON(coefRecon);

			//VPRINTF ((">> [%d %d], local_run = %d, IQ = %d\n", run, level, localRun, coefRecon));
			pCoefRecon [localRun] = (coefRecon ^ iSign) - iSign;
			if (pWmaDec->currFrame >= 1050) {
				//VPRINTF (("run level sign %d %d %d\n", run, level, sign));
			}
			localRun ++;
			assert (localRun < 2048);
		}
		assert (band < 25);
	}

	/// reset
	//VPRINTF (("localRun -1 = %d\n", localRun-1));
	return 0;
}

int WmaDecRunLevelLowMidRate (WmaObject* pWmaDec, uint8 ch)
{
	PerChannelInfo* pChnlInfo = &pWmaDec->channelInfo[ch];
	uint16 level, run, localRun=0, last=0;
	int32  iSign, *pCoef = pWmaDec->pCoefLowMidRate;
	uint32 val;
	int32 iShift;
	FastFloat fQuant;
	BarkIndex	*pBarkIndex = &pWmaDec->barkInfo[pWmaDec->ratio_log2_curr_with_update];
	uint16 currBark=0, NextBarkBand;

	assert (pChnlInfo->power);
	NextBarkBand = pBarkIndex->bark_index[currBark+1];
	run = localRun = pWmaDec->lowCutOff;

	//fQuant = pChnlInfo->barkMaskQ[currBark].fMaskQ; // modify by tcx 2008.04.17
	fQuant = pChnlInfo->fbarkMaskQ[currBark];
	
	MAKE_MASK_FOR_SCALING(fQuant.iFracBits);

	while (localRun < min2 (pWmaDec->highCutOff, pChnlInfo->cSubbandActual) && !last)
	{
		int32 band = 0;

		WmaDecHuf (pWmaDec, pChnlInfo->pHuffDecTbl, &val);

		switch (val) {
		case END_OF_BLOCK:
			last = 1;
			//VPRINTF (("last\n"));
			break;
		case ESCAPE_CODE:
			//VPRINTF (("escape\n"));
			level = pWmaDec->getBits (pWmaDec->max_escape_size);
			run   = pWmaDec->getBits (pWmaDec->bitsSubbandMax);
			iSign  = pWmaDec->getBits (1)-1;
			break;
		default:
			level = pChnlInfo->pLevelEntry [val-2];
			run	  = pChnlInfo->pRunEntry [val-2];
			iSign  = pWmaDec->getBits (1)-1;
			break;
		}
		if (!last) {
			
			//uint32 scaleLocalRun, scaleNextBarkBand;
			//int32 val;

			localRun += run;

			//VPRINTF ((">> [%d %d], local_run = %d, IQ = %d\n", run, level, localRun, coefRecon));
			pCoef [localRun] = (level ^ iSign) - iSign;
						
			localRun ++;
			assert (localRun < 2048);
		}
		assert (band < 25);
	}

	/// reset
	//VPRINTF (("localRun -1 = %d\n", localRun-1));
	return 0;
}
