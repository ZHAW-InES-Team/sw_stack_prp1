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

#ifndef PRP_T_H 
#define PRP_T_H 

#include "PRP_Package_T.h"


typedef struct PRP_MergeLayerInfo_T PRP_MergeLayerInfo_T;
struct PRP_MergeLayerInfo_T 
{
	octet node_[PRP_NODE_NAME_LENGTH];
	octet manufacturer_[PRP_MANUFACTURER_NAME_LENGTH];
    octet version_[PRP_VERSION_LENGTH];
	octet mac_address_A_[PRP_ETH_ADDR_LENGTH];
	octet mac_address_B_[PRP_ETH_ADDR_LENGTH];
	boolean adapter_active_A_;
	boolean adapter_active_B_;
	boolean duplicate_discard_;
    boolean transparent_reception_;
	boolean bridging_;
	boolean node_table_empty_;
	boolean clear_node_table_;
	octet supervision_address_[PRP_ETH_ADDR_LENGTH];
	uinteger16 life_check_interval_;
	uinteger16 node_forget_time_;
	uinteger16 link_time_out_;
	uinteger32 cnt_total_sent_A_;
	uinteger32 cnt_total_sent_B_;
	uinteger32 cnt_total_errors_A_;
	uinteger32 cnt_total_errors_B_;
	uinteger32 cnt_nodes_;
};

typedef struct PRP_NodeTableEntry_T PRP_NodeTableEntry_T;
struct PRP_NodeTableEntry_T 
{
	octet mac_address_A_[PRP_ETH_ADDR_LENGTH];
	octet mac_address_B_[PRP_ETH_ADDR_LENGTH];
	uinteger32 cnt_received_A_;
	uinteger32 cnt_received_B_;
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
	uinteger16 send_seq_;
	boolean failed_A_;
	boolean failed_B_;
};

integer32 PRP_T_receive(octet* data, uinteger32* length, octet lan_id);
integer32 PRP_T_transmit(octet* data, uinteger32* length);
void PRP_T_timer(void);

void PRP_T_link_down_A(void);
void PRP_T_link_down_B(void);

/* merge layer */
integer32 PRP_T_get_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer_info);
integer32 PRP_T_set_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer_info);

/* node table */
integer32 PRP_T_go_to_first_node_table_entry(void);
integer32 PRP_T_go_to_next_node_table_entry(void);
integer32 PRP_T_get_node_table_entry(PRP_NodeTableEntry_T* node_table_entry);

void PRP_T_delete_node_table_entry(PRP_Node_T* node); /* private for the stack */

void PRP_T_init(void);
void PRP_T_cleanup(void);

#endif  
