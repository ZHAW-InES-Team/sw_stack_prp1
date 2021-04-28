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
*********************************************************************
*  30.11.15 | beti     | added new statistic values
*********************************************************************/

#ifndef PRP_EnvironmentConfiguration_T_H 
#define PRP_EnvironmentConfiguration_T_H 

#include "PRP_Package_T.h"

struct PRP_EnvironmentConfiguration_T 
{
    /* Name of the node */
    octet node_[PRP_NODE_NAME_LENGTH];
    /* Name of the manufacturer */
    octet manufacturer_[PRP_MANUFACTURER_NAME_LENGTH];
    /* Version of the implementation */
    octet version_[PRP_VERSION_LENGTH];
    /* MAC address of adapter A */
    octet mac_address_A_[PRP_ETH_ADDR_LENGTH];
    /* MAC address of adapter B */
    octet mac_address_B_[PRP_ETH_ADDR_LENGTH];
    /* Status of adapter A */
    boolean adapter_active_A_;
    /* Status of adapter B */
    boolean adapter_active_B_;
    /* Duplicate discard modus enabled flag */
    boolean duplicate_discard_;
    /* Transparent reception modus enabled flag */
    boolean transparent_reception_;
    /* Total number of frames sent over adapter A */
    uinteger32 cnt_total_sent_A_;
    /* Total number of frames sent over adapter B */
    uinteger32 cnt_total_sent_B_;
    /* Total number of frames received over adapter A */
    uinteger32 cnt_total_received_A_;
    /* Total number of frames received over adapter B */
    uinteger32 cnt_total_received_B_;
    /* Total number of errors on adapter A */
    uinteger32 cnt_total_errors_A_;
    /* Total number of errors on adapter B */
    uinteger32 cnt_total_errors_B_;
    /* Total missing duplicate frames */
    uinteger32 cnt_total_missing_duplicates_;
    /* Max duplicate delay */
    uinteger32 max_duplicate_delay_;
};

void PRP_EnvironmentConfiguration_T_print(PRP_EnvironmentConfiguration_T* const me, uinteger32 level);
void PRP_EnvironmentConfiguration_T_init(PRP_EnvironmentConfiguration_T* const me);
void PRP_EnvironmentConfiguration_T_cleanup(PRP_EnvironmentConfiguration_T* const me);

#endif /* PRP_EnvironmentConfiguration_T_H */

