#ifndef WMA_BTS_H
#define	WMA_BTS_H

#include "common.h"

typedef struct _wma_packet_bts_ {
	uint32	offset;			/// within packet, in bits
	uint32	packetLen;		/// in bytes

	/// bit top
	uint32	bitTop;
	uint32	bitTopLen;
	uint32	packetTop;

	/// packet header
	uint8	seqNum;			/// 4 bits
	uint8	frmInPacket;	/// 4 bits
	uint32	firstFrmOffset;	/// variable
	uint32	firstFrmOffsetLen;

	uint8	newPacket;
} WMA_PACKET_BTS;

int32 WmaResetBits	(WMA_PACKET_BTS *pWmaPacketBts);

uint32 WmaGetBits	(uint a);
uint32 WmaPeekBits	(uint a);
uint32 WmaByteAlign	(void);
uint32 WmaTellBits	(void);

uint32 WmaGetBits2	(uint a);
uint32 WmaPeekBits2	(uint a);
uint32 WmaByteAlign2	(void);
uint32 WmaTellBits2	(void);

#endif //WMA_BTS_H

