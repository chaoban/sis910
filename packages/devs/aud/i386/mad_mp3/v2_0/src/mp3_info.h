#ifndef MP3INFO_H_INCLUDED
#define MP3INFO_H_INCLUDED
#include <cyg/io/aud/adecctrl.h>
#include <pkgconf/io_aud.h>
#include <pkgconf/devs_mad_mp3.h>
#include "mad.h"

bool LoadID3(AUD_tCtrlBlk *pAcb,int id3Version);
bool DisplayID3(AUD_tCtrlBlk *pAcb,int id3Version);
#endif