#ifndef WMA_ENTROPY_H
#define	WMA_ENTROPY_H

#define	ESCAPE_CODE		0
#define	END_OF_BLOCK	1

int WmaDecHuf (WmaObject* pWmaDec, const uint16 *pHufTab, uint32 *val);
int WmaDecRunLevelHighRate (WmaObject* pWmaDec, uint8 ch);	/// high rate
int WmaDecRunLevelLowMidRate (WmaObject* pWmaDec, uint8 ch);	/// high rate

#endif //WMA_ENTROPY_H

