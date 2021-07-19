#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "wmadec.h"
#include "wmabts.h"

#define gGetBits(a)     ExtractBits(a)
#define gPeekBits(a)    PeekBits(a)

WMA_PACKET_BTS gWmaPacketBts = {0};

uint32 WmaGetBits	(uint a)
{
	uint32 val;
	val = gGetBits (a);
	gWmaPacketBts.offset += a;
	assert (gWmaPacketBts.offset <=gWmaPacketBts.packetLen * 8);
	//V16_printf ("%d, %x\n", a, val);
	return val;
}

uint32 WmaPeekBits	(uint a)
{
	uint32 val;
	val = gPeekBits (a);
	assert (gWmaPacketBts.offset <=gWmaPacketBts.packetLen * 8);
	return val;
}

uint32 WmaByteAlign	(void)
{
	uint32 cnt = (8-gWmaPacketBts.offset) & 7;
	assert (gWmaPacketBts.offset <gWmaPacketBts.packetLen * 8);
	return WmaGetBits (cnt);
}

uint32 WmaTellBits	(void)
{
	assert (gWmaPacketBts.offset <gWmaPacketBts.packetLen * 8);
	return gWmaPacketBts.offset;
}


/// last frame within a frame may
uint32 WmaGetBits2	(uint a)
{
	uint32 val;

	if ((a + gWmaPacketBts.offset < gWmaPacketBts.packetLen *8) && gWmaPacketBts.bitTopLen==0) {
	//if ((a + gWmaPacketBts.offset <= gWmaPacketBts.packetLen *8) && gWmaPacketBts.bitTopLen==0) {//modify by tcx
		/// internal
		val = gGetBits (a);
	}
	else {
		/// force to load top
		if ( !gWmaPacketBts.bitTopLen ) {
			WmaPeekBits2 (a);
		}
		assert (gWmaPacketBts.bitTopLen);

		if (gWmaPacketBts.bitTopLen > a) {
			/// still previous packet left over is enough
			val = gWmaPacketBts.bitTop >> (gWmaPacketBts.bitTopLen-a);

			gWmaPacketBts.bitTopLen -= a;
			gWmaPacketBts.bitTop &= (1<<gWmaPacketBts.bitTopLen) -1;
		}
		else {
			/// just cross packet boundary
			val = gWmaPacketBts.bitTop << (a - gWmaPacketBts.bitTopLen);
			val |=gGetBits (a - gWmaPacketBts.bitTopLen);

			/// parse packet header
			assert (gWmaPacketBts.seqNum == (((gWmaPacketBts.packetTop >> (gWmaPacketBts.firstFrmOffsetLen + 4)) -1) & 0xf) );
			gWmaPacketBts.seqNum = (gWmaPacketBts.packetTop >> (gWmaPacketBts.firstFrmOffsetLen + 4)) & 0xf;
			gWmaPacketBts.frmInPacket = (gWmaPacketBts.packetTop >> gWmaPacketBts.firstFrmOffsetLen) & 0xf;
			gWmaPacketBts.firstFrmOffset = gWmaPacketBts.packetTop & ((1<<gWmaPacketBts.firstFrmOffsetLen)-1);

			/// check gWmaPacketBts.packetTop
			gWmaPacketBts.bitTopLen = 0;
			gWmaPacketBts.offset += (4+4+gWmaPacketBts.firstFrmOffsetLen) - gWmaPacketBts.packetLen*8;
		}
	}
	gWmaPacketBts.offset += a;
	return val;
}

uint32 WmaPeekBits2	(uint a)
{
	uint32 val;

	assert (a<32);
	if ( gWmaPacketBts.bitTopLen ) {
		/// has left over from last peek
		if (gWmaPacketBts.bitTopLen >= a) {
			val = gWmaPacketBts.bitTop >> (gWmaPacketBts.bitTopLen-a);
		}
		else {
			val = gWmaPacketBts.bitTop << (a - gWmaPacketBts.bitTopLen);
			val |=gPeekBits (a - gWmaPacketBts.bitTopLen);
		}
	}
	else if (a + gWmaPacketBts.offset < gWmaPacketBts.packetLen *8) {
	//else if (a + gWmaPacketBts.offset <= gWmaPacketBts.packetLen *8) {//modify by tcx 
		/// internal
		val = gPeekBits (a);
	}
	else {
		uint32 hiCnt = gWmaPacketBts.packetLen *8 - gWmaPacketBts.offset;
		uint32 loCnt = a-hiCnt;

		/// automatically load BitTop
		val =  gGetBits(hiCnt);
		gWmaPacketBts.bitTop = val;
		gWmaPacketBts.bitTopLen = hiCnt;

		val <<= loCnt;
		gWmaPacketBts.packetTop = gGetBits (4+4+gWmaPacketBts.firstFrmOffsetLen);
		val |= gPeekBits (loCnt);
	}
	return val;
}

uint32 WmaByteAlign2	(void)
{
	uint32 cnt = (8-gWmaPacketBts.offset) & 7;
	return WmaGetBits2 (cnt);
}

uint32 WmaTellBits2	(void)
{
	return gWmaPacketBts.offset;
}
