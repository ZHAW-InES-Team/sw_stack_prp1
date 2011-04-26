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

#include "PRP_Supervision_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_Environment_T.h"
#include "PRP_SupervisionFrame_T.h"
#include "PRP_Node_T.h"

octet supervision_frame_[64]; /* Supervision frame with VLAN and trailer -> avoids dynamic memory allocation*/

/************************************************************/
/*       PRP_Supervision_T_print                            */
/************************************************************/
void PRP_Supervision_T_print(PRP_Supervision_T* const me, uinteger32 level)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_PRP_LOGOUT(level, "%s\n", "====Supervision=====================");	
	PRP_PRP_LOGOUT(level, "life_check_interval_:\t%u\n", me->life_check_interval_);
	PRP_PRP_LOGOUT(level, "link_time_out_:\t%u\n", me->link_time_out_);
	PRP_PRP_LOGOUT(level, "node_forget_time_:\t%u\n", me->node_forget_time_);
	
	PRP_PRP_LOGOUT(level, "%s\n", "supervision_address_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->supervision_address_[i]);
		
	}
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	
	
}

/************************************************************/
/*       PRP_Supervision_T_supervise                        */
/************************************************************/
integer32 PRP_Supervision_T_supervise(PRP_Supervision_T* const me)
{
	uinteger64 current_time;
	
	PRP_Node_T* node;	
	PRP_Node_T* temp_node;
		
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
		
	current_time = prp_time();

	node = PRP_NodeTable_T_get_first_node(&(me->environment_->node_table_));

	while(node != NULL_PTR) /* go thru the whole list */
	{
		temp_node = PRP_NodeTable_T_get_next_node(&(me->environment_->node_table_), node);
		
		if(((node->san_A_ == node->san_B_) || ((node->san_B_ == FALSE) && (node->san_A_ == TRUE))) && ((current_time - node->time_last_seen_A_) > ((uinteger64)me->link_time_out_ * PRP_SECOND_IN_NANOSECONDS)))
		{
			/* TODO signal that link is down */
			node->failed_A_ = TRUE;
		}
		else
		{
			node->failed_A_ = FALSE;
		}
		
		if(((node->san_A_ == node->san_B_) || ((node->san_A_ == FALSE) && (node->san_B_ == TRUE))) && ((current_time - node->time_last_seen_B_) > ((uinteger64)me->link_time_out_ * PRP_SECOND_IN_NANOSECONDS)))
		{
			/* TODO signal that link is down */
			node->failed_B_ = TRUE;
		}
		else
		{
			node->failed_B_ = FALSE;
		}
		
		if(node->san_A_ == node->san_B_)
		{
			if((node->failed_A_ == TRUE) && (node->failed_B_ == TRUE))
			{
				node->failed_ = FALSE;
			}
			else
			{
				node->failed_ = FALSE;
			}
		}
		else if((node->san_B_ == FALSE) && (node->san_A_ == TRUE))
		{
			node->failed_ = node->failed_A_;
		}
		else if((node->san_A_ == FALSE) && (node->san_B_ == TRUE))
		{
			node->failed_ = node->failed_B_;
		}		
		
		if((current_time - node->time_last_seen_sf_) > ((uinteger64)me->node_forget_time_ * PRP_SECOND_IN_NANOSECONDS))
		{
			node->received_sf_ = FALSE;
		}

		if((current_time - node->time_last_seen_A_) > ((uinteger64)me->node_forget_time_ * PRP_SECOND_IN_NANOSECONDS) &&
		   (current_time - node->time_last_seen_B_) > ((uinteger64)me->node_forget_time_ * PRP_SECOND_IN_NANOSECONDS)) /* node timed out => delete */
		{
			/* TODO signal that node gets deleted */
			PRP_NodeTable_T_remove_node(&(me->environment_->node_table_), node); /* remove node */
		}
				
		
		node = temp_node;
	}	
	
	return(0);
}

/************************************************************/
/*       PRP_Supervision_T_supervision_rx                   */
/************************************************************/
integer32 PRP_Supervision_T_supervision_rx(PRP_Supervision_T* const me, octet* data, uinteger32* length, octet lan_id)
{
	integer32 i;
		
	PRP_Node_T temp_node;
	
	PRP_Node_T* node_A;
	PRP_Node_T* node_B;
	
	PRP_Node_T* node;	
	
	PRP_SupervisionFrame_T* supervision_frame_payload;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	if((*length >= 60) && (data[12] == 0x88) && (data[13] == 0xFB)) /* if Supervision frame */
	{
		supervision_frame_payload = (PRP_SupervisionFrame_T*)(&(data[14]));
	}
	else if((*length >= 64) && (data[12] == 0x81) && (data[13] == 0x00) &&
	        (data[16] == 0x88) && (data[17] == 0xFB)) /* if VLAN tagged Supervision frame */
	{
		supervision_frame_payload = (PRP_SupervisionFrame_T*)(&(data[18]));
	}
	else
	{
		PRP_PRP_LOGOUT(0, "%s\n", "detected frame was no supervision frame");
		return(-PRP_ERROR_FRAME_COR);
	}
	
	if(me->environment_->environment_configuration_.bridging_ == TRUE)
	{	
		PRP_Frames_T_normal_rx(&(me->environment_->frame_analyser_.frames_), data, length, lan_id);
		return(PRP_DROP);
	}	
	
	/* Supervision frame detected */
	
	PRP_Node_T_init(&temp_node);
	
	if(prp_ntohs(supervision_frame_payload->prp_version_) != PRP_VERSION) /* correct version of PRP */
	{
		PRP_PRP_LOGOUT(1, "PRP version in supervision frame not %u: %u\n", PRP_VERSION, prp_ntohs(supervision_frame_payload->prp_version_));
		return(-PRP_ERROR_FRAME_COR);
	}
		
	if(supervision_frame_payload->length_ != PRP_LENGTH) /* correct length for PRP Supervision frame */
	{
		PRP_PRP_LOGOUT(1, "PRP length in supervision frame not %u: %u\n", PRP_LENGTH, supervision_frame_payload->length_);
		return(-PRP_ERROR_FRAME_COR);
	}
	
	PRP_SupervisionFrame_T_print(supervision_frame_payload, 3);
	
	/* Fill in Node properties */
	if(supervision_frame_payload->type_ == PRP_TYPE_DUPLICATE_DISCARD) /* what mode is the remote node running on */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "node is running in duplicate discard mode");
		temp_node.san_A_ = FALSE;
		temp_node.san_B_ = FALSE;
	}
	else if(supervision_frame_payload->type_ == PRP_TYPE_DUPLICATE_ACCEPT) /* what mode is the remote node running on */ 
	{
		PRP_PRP_LOGOUT(2, "%s\n", "node is running in duplicate accept mode");
		temp_node.san_A_ = TRUE;
		temp_node.san_B_ = TRUE;
	}
	else
	{
		PRP_PRP_LOGOUT(1, "PRP type in supervision unknown: %u\n", PRP_LENGTH, supervision_frame_payload->type_);
		return(-PRP_ERROR_FRAME_COR);
	}
		
	
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++) /* fill in MAC addresses */
	{
		temp_node.mac_address_A_[i] = supervision_frame_payload->source_mac_A_[i];
		temp_node.mac_address_B_[i] = supervision_frame_payload->source_mac_B_[i];
	}

		
	/* Check Node table for existence of node */
	
	if(0 == prp_memcmp(temp_node.mac_address_A_, temp_node.mac_address_B_, PRP_ETH_ADDR_LENGTH)) /* if remote node has equal mac addresses on the two adapters */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "remote node has equal mac addresses on both adapters");				
		node = PRP_NodeTable_T_get_node(&(me->environment_->node_table_), temp_node.mac_address_A_);

		if(node == NULL_PTR)  /* node not yet in table? */
		{
			PRP_PRP_LOGOUT(2, "%s\n", "node was not in table, adding node");		
			node = PRP_NodeTable_T_add_node(&(me->environment_->node_table_), &temp_node); /* add new node */
		}
			
	}
	else
	{
		PRP_PRP_LOGOUT(2, "%s\n", "remote node has different mac addresses on both adapters");	
		node_A = PRP_NodeTable_T_get_node(&(me->environment_->node_table_), temp_node.mac_address_A_);
		node_B = PRP_NodeTable_T_get_node(&(me->environment_->node_table_), temp_node.mac_address_B_);
		
		if((node_A == NULL_PTR) && (node_B == NULL_PTR)) /* not yet in table? */
		{
				PRP_PRP_LOGOUT(2, "%s\n", "node was not in table, adding node");		
				node = PRP_NodeTable_T_add_node(&(me->environment_->node_table_), &temp_node); /* add a new node */				
		}
		else if((node_A != NULL_PTR) && (node_B != NULL_PTR)) /* entry with mac A and mac B exists*/
		{
			PRP_PRP_LOGOUT(2, "%s\n", "node for both mac addresses found");	
				
			if(node_A == node_B) /* the same entry? */
			{
				PRP_PRP_LOGOUT(2, "%s\n", "same node object for both addresses");	
				node = node_A;
			}
			else 			
			{	
				/* happens only once when a node with two diffrent mac's already has sent traffic but didn't send a Supervision frame till now */
				PRP_PRP_LOGOUT(2, "%s\n", "different node object for both addresses");	
			
				/* remove both entries to get a new valid consistent entry */
				PRP_NodeTable_T_remove_node(&(me->environment_->node_table_), node_A); /* remove first node */				
				PRP_NodeTable_T_remove_node(&(me->environment_->node_table_), node_B); /* remove second node */
				
				node = PRP_NodeTable_T_add_node(&(me->environment_->node_table_), &temp_node); /* add a new node */				
			}
		}
		else if((node_A != NULL_PTR) && (node_B == NULL_PTR)) /* entry with mac A exists but not with mac B*/ 
		{
			/* happens only once when a node with two diffrent mac's already has sent traffic but didn't send a Supervision frame till now */
			PRP_PRP_LOGOUT(2, "%s\n", "node only for mac address A found");	
			node = node_A;
		}
		else if((node_A == NULL_PTR) && (node_B != NULL_PTR)) /* entry with mac B exists but not with mac A*/
		{
			/* happens only once when a node with two diffrent mac's already has sent traffic but didn't send a Supervision frame till now */
			PRP_PRP_LOGOUT(2, "%s\n", "node only for mac address B found");	
			node = node_B;
		}
		else
		{
			PRP_PRP_LOGOUT(0, "%s\n", "never ever do that again!!!!!!");	
			return(-PRP_ERROR_NULL_PTR);
		}		
	}
	
	if(node == NULL_PTR)
	{
		PRP_PRP_LOGOUT(0, "%s\n", "something is very wrong with the node table");	
		return(-PRP_ERROR_NULL_PTR);
	}
	
	PRP_NodeTable_T_set_mac_address_A(&(me->environment_->node_table_), node, temp_node.mac_address_A_); /* copy MAC */	
	PRP_NodeTable_T_set_mac_address_A(&(me->environment_->node_table_), node, temp_node.mac_address_B_); /* copy MAC */	
	node->san_A_ = temp_node.san_A_;
	node->san_B_ = temp_node.san_B_;
	node->time_last_seen_sf_ = prp_time();
	node->received_sf_ = TRUE;
	
	PRP_Node_T_print(node, 3);
	
	PRP_Frames_T_normal_rx(&(me->environment_->frame_analyser_.frames_), data, length, lan_id);
	
	/* TODO probably add some information if a DAN is actualy a 
	VDAN or not (second TLV), not necessary for algorithm */
	
	return(PRP_DROP); /* always drop supervision frames because they are allready proceeded */
}

/************************************************************/
/*       PRP_Supervision_T_supervision_tx                   */
/************************************************************/
integer32 PRP_Supervision_T_supervision_tx(PRP_Supervision_T* const me)
{
	integer32 i;
	uinteger32 length;
	
	PRP_SupervisionFrame_T* supervision_frame_payload;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	if(me->environment_->environment_configuration_.bridging_ == TRUE)
	{	
		return(0);
	}	
	
	/* ethernet header */
	for(i= 0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		supervision_frame_[i] = me->supervision_address_[i];
		supervision_frame_[(6+i)] = me->environment_->environment_configuration_.mac_address_A_[i];
	}
		
	*((uinteger16*)(&(supervision_frame_[(2*PRP_ETH_ADDR_LENGTH)]))) = prp_htons(PRP_ETHERTYPE);
	
	/* supervision frame payload */
	supervision_frame_payload = (PRP_SupervisionFrame_T*)(&(supervision_frame_[14]));
	
	supervision_frame_payload->prp_version_ = prp_htons(PRP_VERSION); /* version is 0x0000 */
	
	if(me->environment_->environment_configuration_.duplicate_discard_ == TRUE) /* what mode are we running on */
	{
		supervision_frame_payload->type_ = PRP_TYPE_DUPLICATE_DISCARD; /* 20 for dd */
	}
	else
	{
		supervision_frame_payload->type_ = PRP_TYPE_DUPLICATE_ACCEPT; /* 21 for da */
	}
	
	supervision_frame_payload->length_ = PRP_LENGTH; /* length is always 12 */
	
	for(i= 0; i<PRP_ETH_ADDR_LENGTH; i++) /* fill in mac addresses */
	{
		supervision_frame_payload->source_mac_A_[i] = me->environment_->environment_configuration_.mac_address_A_[i];
		supervision_frame_payload->source_mac_B_[i] = me->environment_->environment_configuration_.mac_address_B_[i];
	}
	
	supervision_frame_payload->type2_ = 0; /* we are no red box so this is 0 */
	supervision_frame_payload->length2_ = 0; /* we are no red box so this is 0 */	
	for(i= 0; i<PRP_ETH_ADDR_LENGTH; i++) /* we are no red box so this is 0 */
	{
		supervision_frame_payload->red_box_mac_[i] = 0;
	}
	
	length = 56; /* 56 to avoid padding in switches, buffer is big enough that no memory has to be allocated dynamically*/
	

	PRP_PRP_LOGOUT(2, "%s\n", "supervision frame to send");	
	PRP_SupervisionFrame_T_print(supervision_frame_payload, 2);
		
	return(PRP_Environment_T_process_tx(me->environment_, supervision_frame_, &length, 0x00));
}


/************************************************************/
/*       PRP_Supervision_T_init                             */
/************************************************************/
void PRP_Supervision_T_init(PRP_Supervision_T* const me, PRP_Environment_T* const environment)
{
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}

	prp_memset(supervision_frame_, 0, sizeof(supervision_frame_));
	me->environment_ = environment;
	me->life_check_interval_ = PRP_LIFE_CHECK_INTERVAL;
	me->link_time_out_ = PRP_LINK_TIME_OUT;
	me->node_forget_time_ = PRP_NODE_FORGET_TIME;
	
	me->supervision_address_[0] = 0x01;
	me->supervision_address_[1] = 0x15;
	me->supervision_address_[2] = 0x4e;
	me->supervision_address_[3] = 0x00;
	me->supervision_address_[4] = 0x01;
	me->supervision_address_[5] = 0x00; /* default */
}

/************************************************************/
/*       PRP_Supervision_T_cleanup                          */
/************************************************************/
void PRP_Supervision_T_cleanup(PRP_Supervision_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	prp_memset(supervision_frame_, 0, sizeof(supervision_frame_));
	me->environment_ = NULL_PTR;
	me->life_check_interval_ = PRP_LIFE_CHECK_INTERVAL;
	me->link_time_out_ = PRP_LINK_TIME_OUT;
	me->node_forget_time_ = PRP_NODE_FORGET_TIME;
	prp_memset(me->supervision_address_, 0, PRP_ETH_ADDR_LENGTH);
}
