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
*  21.06.12 | walh     | logging implemented
*********************************************************************
*  30.11.15 | beti     | added new statistic values
*********************************************************************/

#include "PRP_EnvironmentConfiguration_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_NetItf_T.h"


/**
 * @fn void PRP_EnvironmentConfiguration_T_print(PRP_EnvironmentConfiguration_T* const me, uinteger32 level)
 * @brief Print the PRP_EnvironmentConfiguration_T status information
 * @param   me PRP_Environment_T this pointer
 * @param   level uinteger32 importance
 */
void PRP_EnvironmentConfiguration_T_print(PRP_EnvironmentConfiguration_T* const me, uinteger32 level)
{
    integer32 i;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    PRP_PRINTF( "%s\n", "======== EnvironmentConfiguration =====================");
    PRP_PRINTF( "node_:\t\t\t%s\n", me->node_);
    PRP_PRINTF( "manufacturer_:\t\t%s\n", me->manufacturer_);
    PRP_PRINTF( "version_:\t\t%s\n", me->version_);
    PRP_PRINTF( "%s", "mac_address_A_:\t\t");
    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        if (i < 5) {
            PRP_PRINTF("%02x:", me->mac_address_A_[i]);
        } else {
            PRP_PRINTF("%02x\n", me->mac_address_A_[i]);
        }
    }
    PRP_PRINTF( "%s", "mac_address_B_:\t\t");
    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        if (i < 5) {
            PRP_PRINTF("%02x:", me->mac_address_B_[i]);
        } else {
            PRP_PRINTF("%02x\n", me->mac_address_B_[i]);
        }
    }
    PRP_PRINTF( "adapter_active_A_:\t%u\n", me->adapter_active_A_);
    PRP_PRINTF( "adapter_active_B_:\t%u\n", me->adapter_active_B_);
    PRP_PRINTF( "duplicate_discard_:\t%u\n", me->duplicate_discard_);
    PRP_PRINTF( "transparent_reception_:\t%u\n", me->transparent_reception_);
    PRP_PRINTF( "%s\n", "=======================================================");
}

/**
 * @fn void PRP_EnvironmentConfiguration_T_init(PRP_EnvironmentConfiguration_T* const me)
 * @brief Initialize the PRP_EnvironmentConfiguration interface
 * @param   me PRP_Environment_T this pointer
 */
void PRP_EnvironmentConfiguration_T_init(PRP_EnvironmentConfiguration_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    prp_memset(me->node_, 0, PRP_NODE_NAME_LENGTH);
    prp_memset(me->manufacturer_, 0, PRP_MANUFACTURER_NAME_LENGTH);
    prp_memset(me->version_, 0, PRP_VERSION_LENGTH);
    prp_memset(me->mac_address_A_, 0, PRP_ETH_ADDR_LENGTH);
    prp_memset(me->mac_address_B_, 0, PRP_ETH_ADDR_LENGTH);
    me->adapter_active_A_ = FALSE;
    me->adapter_active_B_ = FALSE;
    me->duplicate_discard_ = TRUE;
    me->transparent_reception_ = FALSE;
    me->cnt_total_sent_A_ = 0;
    me->cnt_total_sent_B_ = 0;
    me->cnt_total_received_A_ = 0;
    me->cnt_total_received_B_ = 0;
    me->cnt_total_errors_A_ = 0;
    me->cnt_total_errors_B_ = 0;
    me->cnt_total_missing_duplicates_ = 0;
    me->max_duplicate_delay_ = 0;
}

/**
 * @fn void PRP_EnvironmentConfiguration_T_cleanup(PRP_EnvironmentConfiguration_T* const me)
 * @brief Clean up the PRP_EnvironmentConfiguration interface
 * @param   me PRP_Environment_T this pointer
 */
void PRP_EnvironmentConfiguration_T_cleanup(PRP_EnvironmentConfiguration_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    prp_memset(me->node_, 0, PRP_NODE_NAME_LENGTH);
    prp_memset(me->manufacturer_, 0, PRP_MANUFACTURER_NAME_LENGTH);
    prp_memset(me->version_, 0, PRP_VERSION_LENGTH);
    prp_memset(me->mac_address_A_, 0, PRP_ETH_ADDR_LENGTH);
    prp_memset(me->mac_address_B_, 0, PRP_ETH_ADDR_LENGTH);
    me->adapter_active_A_ = FALSE;
    me->adapter_active_B_ = FALSE;
    me->duplicate_discard_ = TRUE;
    me->transparent_reception_ = FALSE;
    me->cnt_total_sent_A_ = 0;
    me->cnt_total_sent_B_ = 0;
    me->cnt_total_received_A_ = 0;
    me->cnt_total_received_B_ = 0;
    me->cnt_total_errors_A_ = 0;
    me->cnt_total_errors_B_ = 0;
}

