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

#include "PRP_RedundancyControlTrailer_T.h"
#include "PRP_LogItf_T.h"

/**
 * @fn void PRP_RedundancyControlTrailer_T_print(PRP_RedundancyControlTrailer_T* const me, uinteger32 level)
 * @brief Print PRP_RedundancyControlTrailer status information
 * @param   me PRP_RedundancyControlTrailer_T this pointer
 * @param   level uinteger32 importance
 */
void PRP_RedundancyControlTrailer_T_print(PRP_RedundancyControlTrailer_T* const me, const char* str)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }
    PRP_USERLOG(user_log.trailer_, "======== RedundancyControlTrailer (%s) ================\n",str);
    PRP_USERLOG(user_log.trailer_, "lan_id_:\t%x\n", me->lan_id_);
    PRP_USERLOG(user_log.trailer_, "seq_:\t\t%u\n", me->seq_);
    PRP_USERLOG(user_log.trailer_, "%s\n", "=======================================================");
}

/**
 * @fn void PRP_RedundancyControlTrailer_T_init(PRP_RedundancyControlTrailer_T* const me)
 * @brief Initialize the PRP_RedundancyControlTrailer interface
 * @param   me PRP_RedundancyControlTrailer_T this pointer
 */
void PRP_RedundancyControlTrailer_T_init(PRP_RedundancyControlTrailer_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    me->lan_id_ = 0;
    me->seq_ = 0;
}

/**
 * @fn void PRP_RedundancyControlTrailer_T_cleanup(PRP_RedundancyControlTrailer_T* const me)
 * @brief Clean up the PRP_RedundancyControlTrailer interface
 * @param   me PRP_RedundancyControlTrailer_T this pointer
 */
void PRP_RedundancyControlTrailer_T_cleanup(PRP_RedundancyControlTrailer_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    me->lan_id_ = 0;
    me->seq_ = 0;
}

