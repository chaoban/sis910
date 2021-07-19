/* $Id: adeccmd.h 4964 2007-05-02 00:04:47Z echung $ */

#ifndef ADECCMD_H
#define ADECCMD_H

#include "adec.h"
#include "msg_def.h"


/*
  *		Parameter:
  *			@id	The unique identification for message queue. 
  *		Description:
  *			Before you use the message queue to exchange information with
  *			other thread, you should call MsgQ_Create() firstly. The ID
  *			must be unique with other queue's.
  */
int MsgQ_Create(unsigned id);


void MsgQ_Delete(unsigned id);

/*
  *		Parameter:
  *			@id	The unique identification for message queue.
  *			@Msg	The message to be sent.
  *		Return value:
  *			0	successful
  *			-1 failed
  */
int MsgQ_Send(unsigned id, tMsg Msg);


/*
  *		Parameter:
  *			@id	The unique identification for message queue.
  *			@Msg	The pointer of structure "tMsg".
  *		Return value:
  *			0	successful
  *			-1 failed
  */
int MsgQ_Rcv(unsigned id, tMsg *Msg);

#endif /* ADECCMD_H */
