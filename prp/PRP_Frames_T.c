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
*  17.12.07 | mesv     | file created, this is the core
************|**********|*********************************************
*  14.01.08 | mesv     | added some comments
************|**********|*********************************************
*  13.07.11 | itin     | integration of discard algorithm for PRP1
*********************************************************************/

#include "PRP_Frames_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_FrameAnalyser_T.h"
#include "PRP_Trailer_T.h"
#include "PRP_Node_T.h"
#include "PRP_NodeTable_T.h"
#include "PRP_RedundancyControlTrailer_T.h"
#include "PRP_Environment_T.h"
#include "PRP_EnvironmentConfiguration_T.h"
#include "PRP_NetItf_T.h"


/************************************************************/
/*       PRP_Frames_T_replace_src_mac                       */
/************************************************************/
void PRP_Frames_T_print(PRP_Frames_T* const me, octet* data, uinteger32* length, uinteger32 level)
{
	uinteger32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	if(data == NULL_PTR)
	{
		return;
	}

	if(length == NULL_PTR)
	{
		return;
	}
	
	PRP_PRP_LOGOUT(level, "%s\n", "====Frame============================");	
	PRP_PRP_LOGOUT(level, "length: %u\n", *length);		
	PRP_PRP_LOGOUT(level, "%s\n", "data");	
	for(i=0; i<*length; i++)
	{
		PRP_PRP_LOGOUT(level, "%u: %x\n", i, data[i]);		
	}
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	
	
}


/************************************************************/
/*       PRP_Frames_T_replace_src_mac                       */
/************************************************************/
integer32 PRP_Frames_T_replace_src_mac(PRP_Frames_T* const me, octet* data, uinteger32* length, octet* mac)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
		
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		data[(6 + i)] = mac[i];
	}
	
	return(0);
}


/************************************************************/
/*       PRP_Frames_T_replace_dest_mac                       */
/************************************************************/
integer32 PRP_Frames_T_replace_dest_mac(PRP_Frames_T* const me, octet* data, uinteger32* length, octet* mac)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		data[i] = mac[i];
	}
	
	return(0);
}


/************************************************************/
/*       PRP_Frames_T_normal_rx                             */
/************************************************************/
integer32 PRP_Frames_T_normal_rx(PRP_Frames_T* const me, octet* data, uinteger32* length, octet lan_id)
{
	PRP_Node_T* node;
	PRP_Node_T temp_node;
	PRP_RedundancyControlTrailer_T* trailer;
	octet real_lan_id;
	integer32 i, ret;
	octet temp_mac[6];
	octet src_mac[6];
	octet dest_mac[6];

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	if(*length < (2*PRP_ETH_ADDR_LENGTH))
	{
		return(-PRP_ERROR_WRONG_VAL);
	}

	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		dest_mac[i] = data[i];
		src_mac[i] = data[(6 + i)];
	}
	
	PRP_PRP_LOGOUT(2, "%s\n", "received frame");
	PRP_Frames_T_print(me, data, length, 4);	

	/* substitute local mac */		
	if((lan_id == PRP_ID_LAN_B) && 
	   (prp_memcmp(dest_mac, me->frame_analyser_->environment_->environment_configuration_.mac_address_B_, PRP_ETH_ADDR_LENGTH) == 0) &&
	   (prp_memcmp(me->frame_analyser_->environment_->environment_configuration_.mac_address_A_, me->frame_analyser_->environment_->environment_configuration_.mac_address_B_, PRP_ETH_ADDR_LENGTH) != 0)) /* if mac addresses of adapters are different, the address is our address and it is received from LAN B*/
	{
		PRP_PRP_LOGOUT(2, "%s\n", "substituting local mac B with mac A");
		prp_memcpy(dest_mac, me->frame_analyser_->environment_->environment_configuration_.mac_address_A_, PRP_ETH_ADDR_LENGTH);
		PRP_Frames_T_replace_dest_mac(me, data, length, me->frame_analyser_->environment_->environment_configuration_.mac_address_A_); /* replaces dest mac */
	}
	
	/* disable the ports by software */
	if((me->frame_analyser_->environment_->environment_configuration_.adapter_active_A_ == FALSE) && (lan_id == PRP_ID_LAN_A))
	{
		return(PRP_DROP);
	}
	
	if((me->frame_analyser_->environment_->environment_configuration_.adapter_active_B_ == FALSE) && (lan_id == PRP_ID_LAN_B))
	{
		return(PRP_DROP);
	}
	
	if(me->frame_analyser_->environment_->environment_configuration_.bridging_ == TRUE) /* if in bridging mode */
	{
		if(lan_id == PRP_ID_LAN_A)
		{
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_received_A_++;
			if(me->frame_analyser_->environment_->bridging_.state_A_ == PRP_RAPID_SPANNING_TREE_ALTERNATE) /* decide whether to keep frame or not */
			{
				return(PRP_DROP); /* should be only BPDUs */
			}
			else
			{
				return(PRP_KEEP);				
			}
		}
		else
		{
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_received_B_++;
			if(me->frame_analyser_->environment_->bridging_.state_B_ == PRP_RAPID_SPANNING_TREE_ALTERNATE)
			{
				return(PRP_DROP);
			}
			else
			{
				return(PRP_KEEP);				
			}
		}		
	}	
	
	trailer = PRP_Trailer_T_get_trailer(&(me->trailer_rx_), data, length);
	if(trailer != NULL_PTR) /* lan id out of the trailer */
	{
		real_lan_id	= trailer->lan_id_;
	}
	else /* lan_id from which it was received */
	{
		real_lan_id	= lan_id;
	}
	
	/* get node table entry with this src mac */
	node = PRP_NodeTable_T_get_node(&(me->frame_analyser_->environment_->node_table_), src_mac);
	
	if(node == NULL_PTR) /* node not in table */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "node was not in table, adding node");		
		PRP_Node_T_init(&temp_node);
		node = PRP_NodeTable_T_add_node(&(me->frame_analyser_->environment_->node_table_), &temp_node); /* add node to table */
		if(node == NULL_PTR) /* node generation failed */
		{
			return(-PRP_ERROR_NULL_PTR);
		}
	}	

	PRP_PRP_LOGOUT(2, "%s\n", "node before reception");
	PRP_Node_T_print(node, 2);
	
	
	/* update node values */
	if(lan_id == PRP_ID_LAN_A)
	{
		PRP_NodeTable_T_set_mac_address_A(&(me->frame_analyser_->environment_->node_table_), node, src_mac); /* copy MAC */	
		me->frame_analyser_->environment_->environment_configuration_.cnt_total_received_A_++;
		node->cnt_received_A_++;
		node->time_last_seen_A_ = prp_time();	
		
		if(node->received_sf_ == FALSE) /* singly attached node , if a DAN is not sending SF anymore but is still sending over both interfaces, */
		{                               /* the SAN flags will aternate, if only one interface is working it will remain as a SAN*/
			node->san_A_ = TRUE; /* singly attached node on LAN A (could be also a double attached node in duplicate accept mode*) */
			node->san_B_ = FALSE;
			PRP_PRP_LOGOUT(2, "%s\n", "SAN or DAN in Duplicate accpet mode detected on LAN A");
		}
		if((node->san_A_ == FALSE) && (node->san_B_ == FALSE) && (lan_id != real_lan_id)) /* received LAN id not the adapter recieved the frame */
		{
			PRP_PRP_LOGOUT(2, "%s\n", "LAN identifier of RCT different than the adapter(A) the frame was received on");
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_errors_A_++;
			node->cnt_err_wrong_lan_A_++;	
			node->cnt_keept_A_++; 
			return(PRP_KEEP);
		}
		
	}
	else
	{
		PRP_NodeTable_T_set_mac_address_B(&(me->frame_analyser_->environment_->node_table_), node, src_mac); /* copy MAC */	
		me->frame_analyser_->environment_->environment_configuration_.cnt_total_received_B_++;
		node->cnt_received_B_++;
		node->time_last_seen_B_ = prp_time();

		if(node->received_sf_ == FALSE)  /* singly attached node , if a DAN is not sending SF anymore but is still sending over both interfaces, */
		{                               /* the SAN flags will aternate, if only one interface is working it will remain as a SAN*/
			node->san_B_ = TRUE; /* singly attached node on LAN B (could be also a double attached node in duplicate accept mode*) */
			node->san_A_ = FALSE;
			PRP_PRP_LOGOUT(2, "%s\n", "SAN or DAN in Duplicate accpet mode detected on LAN B");
		}
		if((node->san_A_ == FALSE) && (node->san_B_ == FALSE) && (lan_id != real_lan_id)) /* received LAN id not the adapter recieved the frame */
		{
			PRP_PRP_LOGOUT(2, "%s\n", "LAN identifier of RCT different than the adapter(B) the frame was received on");
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_errors_B_++;
			node->cnt_err_wrong_lan_B_++;	
			node->cnt_keept_B_++; 
			return(PRP_KEEP);
		}
	}

	/* substitute remote mac */
	if(((node->san_A_ == TRUE) && (node->san_B_ == TRUE)) ||
	   ((node->san_A_ == FALSE) && (node->san_B_ == FALSE))) /* dual attached node */
	{
		if(real_lan_id == PRP_ID_LAN_B)
		{		
			prp_memset(temp_mac, 0, PRP_ETH_ADDR_LENGTH);
			if(prp_memcmp(temp_mac, node->mac_address_A_, PRP_ETH_ADDR_LENGTH) != 0) /* MAC A available? */
			{
				PRP_PRP_LOGOUT(2, "%s\n", "substituting remote mac B with mac A");
				PRP_Frames_T_replace_src_mac(me, data, length, node->mac_address_A_); /* replaces src mac B with A*/			
			}
		}
	}

	if((trailer == NULL_PTR) || ((node->san_A_ == TRUE) || (node->san_B_ == TRUE)))/* there was no trailer -> keep frame */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "frame had no trailer");		
		if(lan_id == PRP_ID_LAN_A)
		{
			node->cnt_keept_A_++; 
		}
		else
		{
			node->cnt_keept_B_++; 
		}
		return(PRP_KEEP);
	}	

	if(me->frame_analyser_->environment_->environment_configuration_.transparent_reception_ == FALSE) /* transparent mode? */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "removing trailer");
		PRP_Trailer_T_remove_trailer(&(me->trailer_rx_), data, length); /* remove trailer */
	}

	PRP_PRP_LOGOUT(2,"SRC MAC: \n");
	for (i=0;i<6;i++){
		PRP_PRP_LOGOUT(2,"%x",src_mac[i]);
	}
	PRP_PRP_LOGOUT(2,"\nSequ. NR: %d\n",me->trailer_rx_.redundancy_control_trailer_.seq_);

	/*The frame origin is a DAN, search drop table to check if frame has been received previously*/
	ret = PRP_DiscardAlgorithm_PRP1_T_search_entry(&(me->frame_analyser_->environment_->discard_algorithm_prp1_), src_mac, me->trailer_rx_.redundancy_control_trailer_.seq_octet_);
	PRP_DiscardAlgorithm_PRP1_T_print(&(me->frame_analyser_->environment_->discard_algorithm_prp1_),3);
	return ret;
	
}

/************************************************************/
/*       PRP_Frames_T_normal_tx                             */
/************************************************************/
integer32 PRP_Frames_T_normal_tx(PRP_Frames_T* const me, octet* data, uinteger32* length, octet lan_id)
{
	PRP_Node_T* node;
	PRP_Node_T temp_node;	
	PRP_RedundancyControlTrailer_T temp_trailer;
	
	integer32 i;
	octet temp_mac[6];
	octet src_mac[6];
	octet dest_mac[6];

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	if(*length < (2*PRP_ETH_ADDR_LENGTH))
	{
		return(-PRP_ERROR_WRONG_VAL);
	}

	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		dest_mac[i] = data[i];
		src_mac[i] = data[(6 + i)];
	}
	
	if(me->frame_analyser_->environment_->environment_configuration_.bridging_ == TRUE) /* if in bridging mode */
	{
		if((me->frame_analyser_->environment_->bridging_.state_A_ != PRP_RAPID_SPANNING_TREE_ALTERNATE) && ((lan_id == PRP_ID_LAN_A) || (lan_id == 0x00))) /* check if this frame should be sent over LAN A */
		{
			if(me->frame_analyser_->environment_->environment_configuration_.adapter_active_A_ == FALSE)
			{
				return(0);
			}
			PRP_NetItf_T_transmit(data, length, PRP_ID_LAN_A);
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_sent_A_++;
		}
		
		if((me->frame_analyser_->environment_->bridging_.state_B_ != PRP_RAPID_SPANNING_TREE_ALTERNATE) && ((lan_id == PRP_ID_LAN_B) || (lan_id == 0x00))) /* check if this frame should be sent over LAN B */
		{
			if(me->frame_analyser_->environment_->environment_configuration_.adapter_active_B_ == FALSE)
			{
				return(0);
			}

			PRP_NetItf_T_transmit(data, length, PRP_ID_LAN_B);
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_sent_B_++;
		}
		
		return(0);			
	}	
	
			
	/* get node table entry with this dest mac */
	node = PRP_NodeTable_T_get_node(&(me->frame_analyser_->environment_->node_table_), dest_mac);
	
	if(node == NULL_PTR) /* node not in table */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "node was not in table, adding node");		
		PRP_Node_T_init(&temp_node);
		node = PRP_NodeTable_T_add_node(&(me->frame_analyser_->environment_->node_table_), &temp_node); /* add node to table */
		if(node == NULL_PTR) /* node generation failed */
		{
			return(-PRP_ERROR_NULL_PTR);
		}
		
		PRP_NodeTable_T_set_mac_address_A(&(me->frame_analyser_->environment_->node_table_), node, dest_mac); /* copy MAC  we don't know whether the node has two diffren mac addresses or not*/	
		PRP_NodeTable_T_set_mac_address_B(&(me->frame_analyser_->environment_->node_table_), node, dest_mac); /* copy MAC */	
		node->time_last_seen_A_ = prp_time();
		node->time_last_seen_B_ = prp_time();
		
		if((dest_mac[0] & 0x01) == 0x01) /* multicast frames should always sent in dublictae discard mode */
		{
			node->multi_broadcast_ = TRUE;
		}
	}		

	PRP_PRP_LOGOUT(2, "%s\n", "node before transmission");
	PRP_Node_T_print(node, 2);
	
	/* add trailer or not */
	if(((me->frame_analyser_->environment_->environment_configuration_.duplicate_discard_ == TRUE) && ((node->san_A_ == FALSE) && (node->san_B_ == FALSE))) || (node->multi_broadcast_ == TRUE))
	{
		PRP_RedundancyControlTrailer_T_init(&temp_trailer);
		temp_trailer.lan_id_ = PRP_ID_LAN_A;
		temp_trailer.seq_ = node->send_seq_;		
		PRP_PRP_LOGOUT(2, "%s\n", "add trailer for LAN A");		
		PRP_RedundancyControlTrailer_T_print(&temp_trailer,2);
		PRP_Trailer_T_add_trailer(&(me->trailer_tx_), data, length, &temp_trailer); /* add trailer */
	}
	
	/* send it to LAN A or not */
	if((node->san_A_ == TRUE) || ((node->san_A_ == FALSE) && (node->san_B_ == FALSE)) || (node->multi_broadcast_ == TRUE))
	{
		if(me->frame_analyser_->environment_->environment_configuration_.adapter_active_A_ ==  TRUE)
		{
			PRP_PRP_LOGOUT(2, "%s\n", "send frame on LAN A");		
			PRP_NetItf_T_transmit(data, length, PRP_ID_LAN_A);
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_sent_A_++;
			node->cnt_sent_A_++;
			if(node->multi_broadcast_ == TRUE) /* Because we will never see this node as receiving */
			{
				node->time_last_seen_A_ = prp_time();
			}
			PRP_Frames_T_print(me, data, length, 4);	
		}
	}
		
	PRP_Trailer_T_remove_trailer(&(me->trailer_tx_), data, length);  /* add trailer again therefore remove it first*/
	
	/* add trailer or not */
	if(((me->frame_analyser_->environment_->environment_configuration_.duplicate_discard_ == TRUE) && ((node->san_A_ == FALSE) && (node->san_B_ == FALSE))) || (node->multi_broadcast_ == TRUE))
	{
		PRP_RedundancyControlTrailer_T_init(&temp_trailer);
		temp_trailer.lan_id_ = PRP_ID_LAN_B;
		temp_trailer.seq_ = node->send_seq_;		
		PRP_PRP_LOGOUT(2, "%s\n", "add trailer for LAN B");		
		PRP_RedundancyControlTrailer_T_print(&temp_trailer,2);
		PRP_Trailer_T_add_trailer(&(me->trailer_tx_), data, length, &temp_trailer); /* add trailer */
	}
	
	
	/* substitute local mac */
	if(prp_memcmp(me->frame_analyser_->environment_->environment_configuration_.mac_address_A_, me->frame_analyser_->environment_->environment_configuration_.mac_address_B_, PRP_ETH_ADDR_LENGTH) != 0) /* if mac addresses of adapters are different */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "substituting local mac A with mac B");
		PRP_Frames_T_replace_src_mac(me, data, length, me->frame_analyser_->environment_->environment_configuration_.mac_address_B_); /* replaces src mac */
	}
	
	/* substitute remote mac */
	if((((node->san_A_ == TRUE) && (node->san_B_ == TRUE)) || ((node->san_A_ == FALSE) && (node->san_B_ == FALSE))) && 
	   (node->multi_broadcast_ == FALSE)) /* dual attached node */
	{
		if(prp_memcmp(node->mac_address_A_, node->mac_address_B_, PRP_ETH_ADDR_LENGTH) != 0) /* MAC A and B different? */
		{
			prp_memset(temp_mac, 0, PRP_ETH_ADDR_LENGTH);
			if(prp_memcmp(temp_mac, node->mac_address_B_, PRP_ETH_ADDR_LENGTH) != 0) /* MAC B available? */
			{
				PRP_PRP_LOGOUT(2, "%s\n", "substituting node mac A with mac B");
				PRP_Frames_T_replace_dest_mac(me, data, length, node->mac_address_B_); /* replaces dest mac A with B*/			
			}
		}
	}

	/* send it to LAN B or not */
	if((node->san_B_ == TRUE) || ((node->san_A_ == FALSE) && (node->san_B_ == FALSE)) || (node->multi_broadcast_ == TRUE))
	{
		if(me->frame_analyser_->environment_->environment_configuration_.adapter_active_B_ == TRUE)
		{
			PRP_PRP_LOGOUT(2, "%s\n", "send frame on LAN B");		
			PRP_NetItf_T_transmit(data, length, PRP_ID_LAN_B);
			me->frame_analyser_->environment_->environment_configuration_.cnt_total_sent_B_++;
			node->cnt_sent_B_++;
			if(node->multi_broadcast_ == TRUE) /* Because we will never see this node as receiving */
			{
				node->time_last_seen_B_ = prp_time();
			}
			PRP_Frames_T_print(me, data, length, 4);	
		}
	}
	
	if((me->frame_analyser_->environment_->environment_configuration_.adapter_active_A_ == TRUE) ||
	   (me->frame_analyser_->environment_->environment_configuration_.adapter_active_B_ == TRUE))
	{
		node->send_seq_++;
	}
	return(0);
}


/************************************************************/
/*       PRP_Frames_T_init                                  */
/************************************************************/
void PRP_Frames_T_init(PRP_Frames_T* const me, PRP_FrameAnalyser_T* const frame_analyser)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	if(frame_analyser == NULL_PTR)
	{
		return;
	}
	
	me->frame_analyser_ = frame_analyser;
    PRP_Trailer_T_init(&(me->trailer_rx_));
    PRP_Trailer_T_init(&(me->trailer_tx_));
}

/************************************************************/
/*       PRP_Frames_T_cleanup                               */
/************************************************************/
void PRP_Frames_T_cleanup(PRP_Frames_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->frame_analyser_ = NULL_PTR;
    PRP_Trailer_T_cleanup(&(me->trailer_rx_));
    PRP_Trailer_T_cleanup(&(me->trailer_tx_));
}


