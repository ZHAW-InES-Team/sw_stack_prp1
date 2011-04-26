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
*  17.12.07 | mesv     | file created, this is the API
************|**********|*********************************************
*  14.01.08 | mesv     | added some comments
*********************************************************************/

#include "PRP_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_NetItf_T.h"
#include "PRP_OsItf_T.h"
#include "PRP_Environment_T.h"
#include "PRP_Node_T.h"
#include "PRP_Lock_T.h"
#include "PRP_Bridging_T.h"

static PRP_Environment_T environment_;
static PRP_Lock_T lock_;
static PRP_Node_T node_;
static boolean initialized_ = FALSE;


/************************************************************/
/*       PRP_T_receive                                      */
/************************************************************/
integer32 PRP_T_receive(octet* data, uinteger32* length, octet lan_id)
{         
	integer32 ret = PRP_KEEP;	
			
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
		
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
		
  	ret = PRP_Environment_T_process_rx(&environment_, data, length, lan_id);
 	
	PRP_Lock_T_up(&lock_);
	
	return(ret);
}      
   
/************************************************************/
/*       PRP_T_transmit                                     */
/************************************************************/
integer32 PRP_T_transmit(octet* data, uinteger32* length)
{
	integer32 ret = 0;	
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
	
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	 	
 	ret = PRP_Environment_T_process_tx(&environment_, data, length, 0x00);

	PRP_Lock_T_up(&lock_);
	
 	return(ret);
}

/************************************************************/
/*       PRP_T_timer                                        */
/************************************************************/
void PRP_T_timer(void)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return;
	}
	
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	
	PRP_Environment_T_process_timer(&environment_);
	
	PRP_Lock_T_up(&lock_);
	
}

/************************************************************/
/*       PRP_T_link_down_A                                  */
/************************************************************/
void PRP_T_link_down_A(void)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return;
	}
	
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	
	if(environment_.environment_configuration_.bridging_ == TRUE)
	{	
		PRP_Bridging_T_statemachine(&(environment_.bridging_), PRP_RAPID_SPANNING_TREE_LINK_DOWN, NULL_PTR, PRP_ID_LAN_A);
	}
	
	PRP_Lock_T_up(&lock_);
	
}


/************************************************************/
/*       PRP_T_link_down_B                                   */
/************************************************************/
void PRP_T_link_down_B(void)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return;
	}
	
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	
	if(environment_.environment_configuration_.bridging_ == TRUE)
	{	
		PRP_Bridging_T_statemachine(&(environment_.bridging_), PRP_RAPID_SPANNING_TREE_LINK_DOWN, NULL_PTR, PRP_ID_LAN_B);
	}
	
	PRP_Lock_T_up(&lock_);
	
}


/************************************************************/
/*       PRP_T_get_merge_layer_info                         */
/************************************************************/
integer32 PRP_T_get_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
	
	if(merge_layer == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	
	prp_memcpy(merge_layer->node_, environment_.environment_configuration_.node_, PRP_NODE_NAME_LENGTH);
	prp_memcpy(merge_layer->manufacturer_, environment_.environment_configuration_.manufacturer_, PRP_MANUFACTURER_NAME_LENGTH);
	prp_memcpy(merge_layer->version_, environment_.environment_configuration_.version_, PRP_VERSION_LENGTH);
	prp_memcpy(merge_layer->mac_address_A_, environment_.environment_configuration_.mac_address_A_, PRP_ETH_ADDR_LENGTH);
	prp_memcpy(merge_layer->mac_address_B_, environment_.environment_configuration_.mac_address_B_, PRP_ETH_ADDR_LENGTH);
	merge_layer->adapter_active_A_ = environment_.environment_configuration_.adapter_active_A_;
	merge_layer->adapter_active_B_ = environment_.environment_configuration_.adapter_active_B_;
	merge_layer->duplicate_discard_ = environment_.environment_configuration_.duplicate_discard_;
    merge_layer->transparent_reception_ = environment_.environment_configuration_.transparent_reception_;
	merge_layer->bridging_ = environment_.environment_configuration_.bridging_;
	merge_layer->clear_node_table_ = FALSE; /* write only */ 
	merge_layer->node_table_empty_ = environment_.node_table_.node_table_empty_;
	prp_memcpy(merge_layer->supervision_address_, environment_.supervision_.supervision_address_, PRP_ETH_ADDR_LENGTH);
	merge_layer->life_check_interval_ = environment_.supervision_.life_check_interval_;
	merge_layer->node_forget_time_ = environment_.supervision_.node_forget_time_;
	merge_layer->link_time_out_ = environment_.supervision_.link_time_out_;
	merge_layer->drop_window_size_ = environment_.discard_algorithm_.drop_window_size_;	
	merge_layer->cnt_total_sent_A_ = environment_.environment_configuration_.cnt_total_sent_A_;
	merge_layer->cnt_total_sent_B_ = environment_.environment_configuration_.cnt_total_sent_B_;
	merge_layer->cnt_total_errors_A_ = environment_.environment_configuration_.cnt_total_errors_A_;
	merge_layer->cnt_total_errors_B_ = environment_.environment_configuration_.cnt_total_errors_B_;
	merge_layer->cnt_nodes_ = environment_.node_table_.cnt_nodes_;
	
	PRP_Lock_T_up(&lock_);
	
	return(0);
}

/************************************************************/
/*       PRP_T_set_merge_layer_info                         */
/************************************************************/
integer32 PRP_T_set_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer)
{
	PRP_Node_T* node;
	PRP_DropWindow_T* drop_window;
	PRP_DropWindow_T* temp_drop_window;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
	
	if(merge_layer == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	
	prp_memcpy(environment_.environment_configuration_.node_, merge_layer->node_, PRP_NODE_NAME_LENGTH);
	prp_memcpy(environment_.environment_configuration_.manufacturer_, merge_layer->manufacturer_, PRP_MANUFACTURER_NAME_LENGTH);	
	prp_memcpy(environment_.environment_configuration_.version_, merge_layer->version_, PRP_VERSION_LENGTH);
	if(merge_layer->clear_node_table_ == TRUE) /* clear the table */
	{		
		PRP_NodeTable_T_cleanup(&(environment_.node_table_));
		PRP_NodeTable_T_init(&(environment_.node_table_));
	}
	if(0 != prp_memcmp(environment_.environment_configuration_.mac_address_A_, merge_layer->mac_address_A_, PRP_ETH_ADDR_LENGTH)) /* did^mac address of adapter A change ? */
	{
		if(0 != PRP_NetItf_T_set_mac_address_A(merge_layer->mac_address_A_)) /* change the mac on the card */
		{
			PRP_Lock_T_up(&lock_);
			return(-PRP_ERROR_ADAPTER);
		}
		/* delete all drop windows with the old mac address */		
		node = PRP_NodeTable_T_get_first_node(&(environment_.node_table_)); 
		while(node != NULL_PTR)
		{
			drop_window = PRP_DropWindowTable_T_get_first_drop_window(&(node->drop_window_table_));
			while(drop_window != NULL_PTR)
			{
				temp_drop_window = PRP_DropWindowTable_T_get_next_drop_window(&(node->drop_window_table_), drop_window);
				if(0 == prp_memcmp(drop_window->mac_address_, environment_.environment_configuration_.mac_address_A_, PRP_ETH_ADDR_LENGTH))
				{
					PRP_DropWindowTable_T_remove_drop_window(&(node->drop_window_table_), drop_window);
				}
				
				drop_window = temp_drop_window;			
			}	
			node = PRP_NodeTable_T_get_next_node(&(environment_.node_table_), node);
		}
		
		prp_memcpy(environment_.environment_configuration_.mac_address_A_, merge_layer->mac_address_A_, PRP_ETH_ADDR_LENGTH);
	}
	if(0 !=	prp_memcmp(environment_.environment_configuration_.mac_address_B_, merge_layer->mac_address_B_, PRP_ETH_ADDR_LENGTH))
	{
		if(0 != PRP_NetItf_T_set_mac_address_B(merge_layer->mac_address_B_))
		{
			PRP_Lock_T_up(&lock_);
			return(-PRP_ERROR_ADAPTER);
		}
		
		prp_memcpy(environment_.environment_configuration_.mac_address_B_, merge_layer->mac_address_B_, PRP_ETH_ADDR_LENGTH);
	}
	
	if((merge_layer->adapter_active_A_ == TRUE) || (merge_layer->adapter_active_A_ == FALSE))
	{
		if(0 != PRP_NetItf_T_set_active_A(merge_layer->adapter_active_A_))
		{
			PRP_Lock_T_up(&lock_);
			return(-PRP_ERROR_ADAPTER);
		}
		environment_.environment_configuration_.adapter_active_A_ = merge_layer->adapter_active_A_;
	}
	else
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_ERROR_WRONG_VAL);
	}
	
	if((merge_layer->adapter_active_B_ == TRUE) || (merge_layer->adapter_active_B_ == FALSE))
	{
		if(0 != PRP_NetItf_T_set_active_B(merge_layer->adapter_active_B_))
		{
			PRP_Lock_T_up(&lock_);
			return(-PRP_ERROR_ADAPTER);
		}
		environment_.environment_configuration_.adapter_active_B_ = merge_layer->adapter_active_B_;
	}
	else
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_ERROR_WRONG_VAL);
	}

	if((merge_layer->duplicate_discard_ == TRUE) || (merge_layer->duplicate_discard_ == FALSE))
	{
		environment_.environment_configuration_.duplicate_discard_ = merge_layer->duplicate_discard_;
	}
	else
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_ERROR_WRONG_VAL);
	}
	
	if((merge_layer->duplicate_discard_ == TRUE) || (merge_layer->duplicate_discard_ == FALSE))
	{
	    environment_.environment_configuration_.transparent_reception_ = merge_layer->transparent_reception_;
	}
	else
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_ERROR_WRONG_VAL);
	}
    
	if((merge_layer->bridging_ == TRUE) || (merge_layer->bridging_ == FALSE))
	{
		if((merge_layer->bridging_ == TRUE) && (environment_.environment_configuration_.bridging_ == FALSE))
		{
			PRP_NodeTable_T_cleanup(&(environment_.node_table_));
		}	
		environment_.environment_configuration_.bridging_ = merge_layer->bridging_;
	}
	else
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_ERROR_WRONG_VAL);
	}
	
	
/*	environment_.node_table_.node_table_empty_ = merge_layer->node_table_empty_; read only */
	if(0 !=	prp_memcmp(environment_.supervision_.supervision_address_, merge_layer->supervision_address_, PRP_ETH_ADDR_LENGTH))
	{
		if((merge_layer->supervision_address_[0] != 0x01) ||
		   (merge_layer->supervision_address_[1] != 0x15) ||
		   (merge_layer->supervision_address_[2] != 0x4e) ||
		   (merge_layer->supervision_address_[3] != 0x00) ||
		   (merge_layer->supervision_address_[4] != 0x01)) /* is it a valid supervision frame address? */
		{
			PRP_Lock_T_up(&lock_);
			return(-PRP_ERROR_WRONG_VAL);
		}
		
		if(0 != PRP_NetItf_T_set_supervision_address(merge_layer->supervision_address_)) /* add the address to the multicast addresses of the adapters */
		{
			PRP_Lock_T_up(&lock_);
			return(-PRP_ERROR_ADAPTER);
		}
		/* delete all drop windows with the old mac address */		
		node = PRP_NodeTable_T_get_first_node(&(environment_.node_table_));
		while(node != NULL_PTR)
		{
			drop_window = PRP_DropWindowTable_T_get_first_drop_window(&(node->drop_window_table_));
			while(drop_window != NULL_PTR)
			{
				temp_drop_window = PRP_DropWindowTable_T_get_next_drop_window(&(node->drop_window_table_), drop_window);
				if(0 == prp_memcmp(drop_window->mac_address_, environment_.supervision_.supervision_address_, PRP_ETH_ADDR_LENGTH))
				{
					PRP_DropWindowTable_T_remove_drop_window(&(node->drop_window_table_), drop_window);
				}
				
				drop_window = temp_drop_window;			
			}	
			node = PRP_NodeTable_T_get_next_node(&(environment_.node_table_), node);
		}
		prp_memcpy(environment_.supervision_.supervision_address_, merge_layer->supervision_address_, PRP_ETH_ADDR_LENGTH);
	}
	environment_.supervision_.life_check_interval_ = merge_layer->life_check_interval_;
	environment_.supervision_.node_forget_time_ = merge_layer->node_forget_time_;
	environment_.supervision_.link_time_out_ = merge_layer->link_time_out_;
	if(merge_layer->drop_window_size_ <= PRP_DROP_WINDOW_MAX_SIZE)
	{
		environment_.discard_algorithm_.drop_window_size_ = merge_layer->drop_window_size_;	
	}
	else
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_ERROR_WRONG_VAL);
	}
	
/*	environment_.environment_configuration_.cnt_total_sent_A_ = merge_layer->cnt_total_sent_A_; read only */
/*	environment_.environment_configuration_.cnt_total_sent_B_ = merge_layer->cnt_total_sent_B_; read only */
/*	environment_.environment_configuration_.cnt_total_errors_A_ = merge_layer->cnt_total_errors_A_; read only */
/*	environment_.environment_configuration_.cnt_total_errors_B_ = merge_layer->cnt_total_errors_B_; read only */
/*	environment_.node_table_.cnt_nodes_ = merge_layer->cnt_nodes_; read only */
	
	PRP_Lock_T_up(&lock_);
	
	return(0);
}





/************************************************************/
/*       PRP_T_get_node_table_entry                         */
/************************************************************/
integer32 PRP_T_get_node_table_entry(PRP_NodeTableEntry_T* node_table_entry)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
	
	PRP_Lock_T_down(&lock_);
	
	if(node_.previous_node_ == NULL_PTR)
	{
		PRP_Lock_T_up(&lock_);
		return(-PRP_NODE_DELETED);
	}
	
	prp_memcpy(&node_, node_.previous_node_, sizeof(PRP_Node_T));	
	
	prp_memcpy(node_table_entry->mac_address_A_, node_.mac_address_A_, PRP_ETH_ADDR_LENGTH);
	prp_memcpy(node_table_entry->mac_address_B_, node_.mac_address_B_, PRP_ETH_ADDR_LENGTH);
	node_table_entry->cnt_received_A_ = node_.cnt_received_A_;
	node_table_entry->cnt_received_B_ = node_.cnt_received_B_;
	node_table_entry->cnt_keept_A_ = node_.cnt_keept_A_;
	node_table_entry->cnt_keept_B_ = node_.cnt_keept_B_;
	node_table_entry->cnt_err_out_of_sequence_A_ = node_.cnt_err_out_of_sequence_A_;
	node_table_entry->cnt_err_out_of_sequence_B_ = node_.cnt_err_out_of_sequence_B_;
	node_table_entry->cnt_err_wrong_lan_A_ = node_.cnt_err_wrong_lan_A_;
	node_table_entry->cnt_err_wrong_lan_B_ = node_.cnt_err_wrong_lan_B_;
	node_table_entry->time_last_seen_A_ = node_.time_last_seen_A_;
	node_table_entry->time_last_seen_B_ = node_.time_last_seen_B_;
	node_table_entry->san_A_ = node_.san_A_;
	node_table_entry->san_B_ = node_.san_B_;
	node_table_entry->send_seq_ = node_.send_seq_;
	node_table_entry->failed_A_ = node_.failed_A_;
	node_table_entry->failed_B_ = node_.failed_B_;
	
	PRP_Lock_T_up(&lock_);
	return(0);
}

/************************************************************/
/*       PRP_T_go_to_first_node_table_entry                 */
/************************************************************/
integer32 PRP_T_go_to_first_node_table_entry(void)
{         
	PRP_Node_T* temp_node;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
	
	PRP_Lock_T_down(&lock_);
	
	temp_node = PRP_NodeTable_T_get_first_node(&(environment_.node_table_));
	
	if(temp_node == NULL_PTR)
	{
		PRP_Lock_T_up(&lock_);
		return(PRP_NODETABLE_END);
	}
	
	prp_memcpy(&node_, temp_node, sizeof(PRP_Node_T));	
	
	node_.previous_node_ = temp_node; /* to detect if a node gets deleted if it was the current one */
		
	PRP_Lock_T_up(&lock_);
	return(0);
}         

          
/************************************************************/
/*       PRP_T_go_to_next_node_table_entry                  */
/************************************************************/
integer32 PRP_T_go_to_next_node_table_entry(void)
{    
	PRP_Node_T* temp_node;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return(-PRP_ERROR_NOT_INITIALIZED);
	}
	
	PRP_Lock_T_down(&lock_);
	
	temp_node = node_.next_node_;
	
	if(temp_node == NULL_PTR)
	{
		PRP_Lock_T_up(&lock_);
		return(PRP_NODETABLE_END);
	}
	
	prp_memcpy(&node_, temp_node, sizeof(PRP_Node_T));	
	
	node_.previous_node_ = temp_node; /* to detect if a node gets deleted if it was the current one */
	
	PRP_Lock_T_up(&lock_);	
	return(0);
}         

/************************************************************/
/*       PRP_T_delete_node_table_entry                      */
/************************************************************/
void PRP_T_delete_node_table_entry(PRP_Node_T* node)
{         
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(initialized_ != TRUE)
	{
		return;
	}
	
	if(node == NULL_PTR)
	{
		return;
	}
	
	if(node_.next_node_ == node)
	{
		node_.next_node_ = 	node->next_node_;
	}
	if(node_.previous_node_ == node)
	{
		node_.previous_node_ = 	NULL_PTR;		
	}
}


/************************************************************/
/*       PRP_T_init                                         */
/************************************************************/
void PRP_T_init(void)
{
	if(initialized_ != FALSE)
	{
		return;
	}
	PRP_Lock_T_init(&lock_); /* API calls are mutex */
	PRP_Environment_T_init(&environment_);
	PRP_Node_T_init(&node_);
	initialized_ = TRUE;
}

/************************************************************/
/*       PRP_T_cleanup                                      */
/************************************************************/
void PRP_T_cleanup(void)
{
	if(initialized_ != TRUE)
	{
		return;
	}
	PRP_Lock_T_down(&lock_); /* API calls are mutex */
	
	initialized_ = FALSE;
	
	PRP_Lock_T_cleanup(&lock_);
	PRP_Environment_T_cleanup(&environment_);
	PRP_Node_T_cleanup(&node_);
}

