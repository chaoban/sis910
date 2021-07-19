#ifndef _WMA_H_
#define _WMA_H_

#include "common.h"
#include "wmadec.h"


typedef struct tagWmaPriv {
    int32       state;
    int32       inited;

    WmaObject   stWmaDec;
    /// abv is from pAcb->Abv
} tWmaPriv;

#endif //_WMA_H_

