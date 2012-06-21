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

#ifndef PRP_Supervision_T_H 
#define PRP_Supervision_T_H 

#include "PRP_Package_T.h"

struct PRP_Supervision_T {
    /* Pointer to the environment object */
    PRP_Environment_T* environment_;
    /* Destination address to send supervision frames to */
    octet supervision_address_[PRP_ETH_ADDR_LENGTH];
    /* Interval to check for the existence of nodes */
    uinteger16 life_check_interval_;
    /* Interval to check for the existence of nodes */
    uinteger16 check_interval_aging_;
    /* Time when a node is considered down and should be removed from the node table */
    uinteger16 node_forget_time_;
    /* Time when the link is considered down */
    uinteger16 link_time_out_;
    /* The sequenz number for the sent supervisions */
    uinteger16 supervision_seqno_;
};

struct PRP_Supervision_T_params {
    uinteger16 sup_path_;
    uinteger16 sup_version_;
    uinteger16 sup_sequence_number_;
    uinteger16 tlv_type_;
    uinteger16 tlv_length_;
    unsigned char* src_macA_;
    unsigned char* src_macB_;
};

integer32 PRP_Supervision_T_supervision_rx(PRP_Supervision_T* const me, octet* data, uinteger32* length, octet lan_id);
integer32 PRP_Supervision_T_supervision_tx(PRP_Supervision_T* const me);
void PRP_Supervision_T_init(PRP_Supervision_T* const me, PRP_Environment_T* const environment);
void PRP_Supervision_T_cleanup(PRP_Supervision_T* const me);

#endif /* PRP_Supervision_T_H */

