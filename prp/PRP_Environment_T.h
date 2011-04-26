/********************************************************************
*  
*  Copyright (c) 2007, Institute of Embedded Systems at 
*                      Zurich University of Applied Sciences 
*                      (http://ines.zhaw.ch)
*  
*  All rights reserved.
* 
* 
*  Redistribution and use in source and binary forms, with or  
*  without modification, are permitted provided that the 
*  following conditions are met:
*  
*  - Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer. 
*
*  - Redistributions in binary form must reproduce the above 
*    copyright notice, this list of conditions and the following 
*    disclaimer in the documentation and/or other materials provided
*    with the distribution. 
*
*  - Neither the name of the Zurich University of Applied Sciences
*    nor the names of its contributors may be used to endorse or 
*    promote products derived from this software without specific 
*    prior written permission. 
*
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
*  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
*  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
*  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
*  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*  
*********************************************************************/


/********************************************************************
*   _____       ______  _____                                       *
*  |_   _|     |  ____|/ ____|                                      *
*    | |  _ __ | |__  | (___    Institute of Embedded Systems       *
*    | | | '_ \|  __|  \___ \   Zurich University of Applied        *
*   _| |_| | | | |____ ____) |  Sciences                            *
*  |_____|_| |_|______|_____/   8401 Winterthur, Switzerland        *
*                                                                   *
*********************************************************************
* 
*  Project     : Parallel Redundancy Protocol
* 
*  Version     : 1.0
*  Author      : Sven Meier
* 
*********************************************************************
*  Change History
*
*  Date     | Name     | Modification
************|**********|*********************************************
*  17.12.07 | mesv     | file created
*********************************************************************/

#ifndef PRP_Environment_T_H 
#define PRP_Environment_T_H 

#include "PRP_Package_T.h"
#include "PRP_Timer_T.h"
#include "PRP_EnvironmentConfiguration_T.h"
#include "PRP_NodeTable_T.h"
#include "PRP_Supervision_T.h"
#include "PRP_Bridging_T.h"
#include "PRP_DiscardAlgorithm_T.h"
#include "PRP_FrameAnalyser_T.h"

struct PRP_Environment_T 
{
	PRP_Timer_T bridging_timer_;
	PRP_Timer_T supervise_timer_;
	PRP_Timer_T supervision_tx_timer_;
	PRP_EnvironmentConfiguration_T environment_configuration_;
	PRP_Supervision_T supervision_;
	PRP_Bridging_T bridging_;
	PRP_NodeTable_T node_table_;
	PRP_DiscardAlgorithm_T discard_algorithm_;
	PRP_FrameAnalyser_T frame_analyser_;

};

void PRP_Environment_T_process_timer(PRP_Environment_T* const me);
integer32 PRP_Environment_T_process_rx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id);
integer32 PRP_Environment_T_process_tx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id);

void PRP_Environment_T_init(PRP_Environment_T* const me);
void PRP_Environment_T_cleanup(PRP_Environment_T* const me);

#endif  

