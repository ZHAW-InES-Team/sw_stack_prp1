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
************|**********|*********************************************
*  14.01.08 | mesv     | added some comments
*********************************************************************/

#include "PRP_Node_T.h"
#include "PRP_LogItf_T.h"


/************************************************************/
/*       PRP_Node_T_print                                   */
/************************************************************/
void PRP_Node_T_print(PRP_Node_T* const me, uinteger32 level)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_PRP_LOGOUT(level, "%s\n", "====Node============================");	
	
	PRP_PRP_LOGOUT(level, "%s\n", "mac_address_A_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->mac_address_A_[i]);
		
	}
	PRP_PRP_LOGOUT(level, "%s\n", "mac_address_B_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->mac_address_B_[i]);
		
	}
	
	PRP_PRP_LOGOUT(level, "cnt_received_A_:\t%u\n", me->cnt_received_A_);
	PRP_PRP_LOGOUT(level, "cnt_received_B_:\t%u\n", me->cnt_received_B_);
	PRP_PRP_LOGOUT(level, "cnt_sent_A_:\t%u\n", me->cnt_sent_A_);
	PRP_PRP_LOGOUT(level, "cnt_sent_B_:\t%u\n", me->cnt_sent_B_);
	PRP_PRP_LOGOUT(level, "cnt_keept_A_:\t%u\n", me->cnt_keept_A_);
	PRP_PRP_LOGOUT(level, "cnt_keept_B_:\t%u\n", me->cnt_keept_B_);
	PRP_PRP_LOGOUT(level, "cnt_err_out_of_sequence_A_:\t%u\n", me->cnt_err_out_of_sequence_A_);
	PRP_PRP_LOGOUT(level, "cnt_err_out_of_sequence_B_:\t%u\n", me->cnt_err_out_of_sequence_B_);
	PRP_PRP_LOGOUT(level, "cnt_err_wrong_lan_A_:\t%u\n", me->cnt_err_wrong_lan_A_);
	PRP_PRP_LOGOUT(level, "cnt_err_wrong_lan_B_:\t%u\n", me->cnt_err_wrong_lan_B_);
	PRP_PRP_LOGOUT(level, "time_last_seen_A_:\t%016llu\n", me->time_last_seen_A_);
	PRP_PRP_LOGOUT(level, "time_last_seen_B_:\t%016llu\n", me->time_last_seen_B_);
	PRP_PRP_LOGOUT(level, "san_A_:\t%u\n", me->san_A_);
	PRP_PRP_LOGOUT(level, "san_B_:\t%u\n", me->san_B_);
	PRP_PRP_LOGOUT(level, "time_last_seen_sf_:\t%lld\n", me->time_last_seen_sf_);
	PRP_PRP_LOGOUT(level, "received_sf_:\t%u\n", me->received_sf_);
	PRP_PRP_LOGOUT(level, "send_seq_:\t%u\n", me->send_seq_);
	PRP_PRP_LOGOUT(level, "multi_broadcast_:\t%u\n", me->multi_broadcast_);
	PRP_PRP_LOGOUT(level, "failed_:\t%u\n", me->failed_);
	PRP_PRP_LOGOUT(level, "failed_A_:\t%u\n", me->failed_A_);
	PRP_PRP_LOGOUT(level, "failed_B_:\t%u\n", me->failed_B_);
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	

}

/************************************************************/
/*       PRP_Node_T_init                                    */
/************************************************************/
void PRP_Node_T_init(PRP_Node_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->next_node_ = NULL_PTR; /* unconnected node */
	me->previous_node_= NULL_PTR;
	prp_memset(me->mac_address_A_, 0, PRP_ETH_ADDR_LENGTH);
	prp_memset(me->mac_address_B_, 0, PRP_ETH_ADDR_LENGTH);
	me->cnt_received_A_ = 0;
	me->cnt_received_B_ = 0;
	me->cnt_sent_A_ = 0;
	me->cnt_sent_B_ = 0;
	me->cnt_keept_A_ = 0;
	me->cnt_keept_B_ = 0;
	me->cnt_err_out_of_sequence_A_ = 0;
	me->cnt_err_out_of_sequence_B_ = 0;
	me->cnt_err_wrong_lan_A_ = 0;
	me->cnt_err_wrong_lan_B_ = 0;
	me->time_last_seen_A_ = 0;
	me->time_last_seen_B_ = 0;
	me->san_A_ = FALSE;
	me->san_B_ = FALSE;
	me->time_last_seen_sf_ = 0;
	me->received_sf_ = FALSE;
	me->send_seq_ = 1;
	me->multi_broadcast_ = FALSE;
	me->failed_ = FALSE;
	me->failed_A_ = FALSE;
	me->failed_B_ = FALSE;
}

/************************************************************/
/*       PRP_Node_T_cleanup                                 */
/************************************************************/
void PRP_Node_T_cleanup(PRP_Node_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->next_node_ = NULL_PTR;
	me->previous_node_= NULL_PTR;
	prp_memset(me->mac_address_A_, 0, PRP_ETH_ADDR_LENGTH);
	prp_memset(me->mac_address_B_, 0, PRP_ETH_ADDR_LENGTH);
	me->cnt_received_A_ = 0;
	me->cnt_received_B_ = 0;
	me->cnt_sent_A_ = 0;
	me->cnt_sent_B_ = 0;
	me->cnt_keept_A_ = 0;
	me->cnt_keept_B_ = 0;
	me->cnt_err_out_of_sequence_A_ = 0;
	me->cnt_err_out_of_sequence_B_ = 0;
	me->cnt_err_wrong_lan_A_ = 0;
	me->cnt_err_wrong_lan_B_ = 0;
	me->time_last_seen_A_ = 0;
	me->time_last_seen_B_ = 0;
	me->san_A_ = FALSE;
	me->san_B_ = FALSE;
	me->time_last_seen_sf_ = 0;
	me->received_sf_ = FALSE;
	me->send_seq_ = 0;
	me->multi_broadcast_ = FALSE;
	me->failed_ = FALSE;
	me->failed_A_ = FALSE;
	me->failed_B_ = FALSE;
}


/************************************************************/
/*       PRP_Node_T_create                                  */
/************************************************************/
PRP_Node_T* PRP_Node_T_create(void)
{
	PRP_Node_T* node;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
    node = (PRP_Node_T*)prp_malloc(sizeof(PRP_Node_T));
    
	if(node != NULL_PTR)
	{
		PRP_Node_T_init(node);
	}	
	
    return(node);
}

/************************************************************/
/*       PRP_Node_T_destroy                                 */
/************************************************************/
void PRP_Node_T_destroy(PRP_Node_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_Node_T_cleanup(me);
	prp_free(me);
}

