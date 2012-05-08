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

#ifndef PRP_NodeTable_T_H 
#define PRP_NodeTable_T_H 

#include "PRP_Package_T.h"

struct PRP_NodeTable_T 
{
    /* Pointer to the first node. If NULL, table is empty */
    PRP_Node_T* first_node_;
    /* Pointer to the last node. If NULL, table is empty */
    PRP_Node_T* last_node_;
    /* Number of nodes in the table */
    uinteger32 cnt_nodes_;
    /* Shows whether table is empty or not */
    boolean node_table_empty_;
};

void PRP_NodeTable_T_print(PRP_NodeTable_T* const me, uinteger32 level);
void PRP_NodeTable_T_set_mac_address_A(PRP_NodeTable_T* const me, PRP_Node_T* node, octet* mac);
void PRP_NodeTable_T_set_mac_address_B(PRP_NodeTable_T* const me, PRP_Node_T* node, octet* mac);
PRP_Node_T* PRP_NodeTable_T_add_node(PRP_NodeTable_T* const me, PRP_Node_T* node);
PRP_Node_T* PRP_NodeTable_T_get_first_node(PRP_NodeTable_T* const me);
PRP_Node_T* PRP_NodeTable_T_get_last_node(PRP_NodeTable_T* const me);
PRP_Node_T* PRP_NodeTable_T_get_previous_node(PRP_NodeTable_T* const me, PRP_Node_T* node);
PRP_Node_T* PRP_NodeTable_T_get_next_node(PRP_NodeTable_T* const me, PRP_Node_T* node);
PRP_Node_T* PRP_NodeTable_T_get_node(PRP_NodeTable_T* const me, octet* node_mac);
void PRP_NodeTable_T_remove_node(PRP_NodeTable_T* const me, PRP_Node_T* node);
void PRP_NodeTable_T_init(PRP_NodeTable_T* const me);
void PRP_NodeTable_T_cleanup(PRP_NodeTable_T* const me);

#endif /* PRP_NodeTable_T_H */

