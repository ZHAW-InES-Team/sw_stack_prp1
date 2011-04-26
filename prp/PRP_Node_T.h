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

#ifndef PRP_Node_T_H 
#define PRP_Node_T_H 

#include "PRP_Package_T.h"
#include "PRP_DropWindowTable_T.h"

struct PRP_Node_T 
{
	PRP_Node_T* next_node_;
	PRP_Node_T* previous_node_;
	octet mac_address_A_[PRP_ETH_ADDR_LENGTH];
	octet mac_address_B_[PRP_ETH_ADDR_LENGTH];
	uinteger32 cnt_received_A_;
	uinteger32 cnt_received_B_;
	uinteger32 cnt_sent_A_;
	uinteger32 cnt_sent_B_;
	uinteger32 cnt_keept_A_;
	uinteger32 cnt_keept_B_;
	uinteger32 cnt_err_out_of_sequence_A_;
	uinteger32 cnt_err_out_of_sequence_B_;
	uinteger32 cnt_err_wrong_lan_A_;
	uinteger32 cnt_err_wrong_lan_B_;
	uinteger64 time_last_seen_A_;
	uinteger64 time_last_seen_B_;
	boolean san_A_;
	boolean san_B_;
	uinteger64 time_last_seen_sf_;
	boolean received_sf_;
	uinteger16 send_seq_;
	boolean multi_broadcast_;
	boolean failed_;
	boolean failed_A_;
	boolean failed_B_;
	PRP_DropWindowTable_T drop_window_table_;
};

void PRP_Node_T_print(PRP_Node_T* const me, uinteger32 level);

void PRP_Node_T_init(PRP_Node_T* const me);
void PRP_Node_T_cleanup(PRP_Node_T* const me);

PRP_Node_T* PRP_Node_T_create(void);
void PRP_Node_T_destroy(PRP_Node_T* const me);

#endif  
