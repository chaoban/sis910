#ifndef CYGONCE_MP3_DEC_H
#define CYGONCE_MP3_DEC_H


int AUD_MP3_Init(AUD_tCtrlBlk *pAcb);

int AUD_MP3_Decode(AUD_tCtrlBlk *pAcb);

int AUD_MP3_Cleanup(AUD_tCtrlBlk *pAcb);

int AUD_MP3_Get_Config(AUD_tCtrlBlk *pAcb);


#endif
