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
************|**********|*********************************************
*  13.07.11 | itin     | integration of discard algorithm for PRP1
************|**********|*********************************************
*  11.05.12 | asdo     | discard algorithm improvement
************|**********|*********************************************
*  30.11.15 | beti     | added new statistic values
*********************************************************************/

#include "PRP_Environment_T.h"
#include "PRP_LogItf_T.h"


/**
 * @fn void PRP_Environment_T_process_timer(PRP_Environment_T* const me)
 * @brief Runs the timers and if the timer expired calls the respective supervision function
 * @param   me PRP_Environment_T this pointer
 */
void PRP_Environment_T_process_timer(PRP_Environment_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    if (TRUE == PRP_Timer_T_tick(&(me->supervision_tx_timer_))) {
        /* send a new supervision frame */
        PRP_Supervision_T_supervision_tx(&(me->supervision_)); 
        PRP_Timer_T_start(&(me->supervision_tx_timer_), me->supervision_.life_check_interval_);
    }

    if (TRUE == PRP_Timer_T_tick(&(me->aging_timer_))) {
        /* execute aging*/
        PRP_DiscardAlgorithm_T_do_aging(&(me->discard_algorithm_));
        PRP_Timer_T_start(&(me->aging_timer_), me->supervision_.check_interval_aging_);
    }

    if (TRUE == PRP_Timer_T_tick(&(me->statistic_timer_))) {
        /* print log status */
        if (user_log.counter_) {
            PRP_LogItf_T_print_counters();
        }
        PRP_Timer_T_start(&(me->statistic_timer_), PRP_TIMER_STATISTIC_INTERVAL);
    }
}

/**
 * @fn integer32 PRP_Environment_T_process_rx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id)
 * @brief Forwards the API call receive to the frame analyser
 * @param   me PRP_Environment_T this pointer
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 length in bytes of the frame
 * @param   lan_id octet on which LAN it was received
 * @retval  1 integer32 DROP
 * @retval  0 integer32 KEEP
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_Environment_T_process_rx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return(-PRP_ERROR_NULL_PTR);
    }
    /* forward to the frame analyser */
    return(PRP_FrameAnalyser_T_analyse_rx(&(me->frame_analyser_), data, length, lan_id));
}

/**
 * @fn integer32 PRP_Environment_T_process_tx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id)
 * @brief Forwards the API call transmit to the frame analyser
 * @param   me PRP_Environment_T this pointer
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 length in bytes of the frame
 * @param   lan_id octet on which LAN it is going send
 * @retval  0 integer32 OK
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_Environment_T_process_tx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return(-PRP_ERROR_NULL_PTR);
    }
    /* forward to the frame analyser */
    return(PRP_FrameAnalyser_T_analyse_tx(&(me->frame_analyser_), data, length, lan_id));
}


/**
 * @fn void PRP_Environment_T_init(PRP_Environment_T* const me)
 * @brief Initialize the PRP_Environment interface
 * @param   me PRP_Environment_T this pointer
 */
void PRP_Environment_T_init(PRP_Environment_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    /* start all the modules */
    PRP_EnvironmentConfiguration_T_init(&(me->environment_configuration_));
    PRP_Supervision_T_init(&(me->supervision_), me);
    PRP_DiscardAlgorithm_T_init(&(me->discard_algorithm_), me);
    PRP_FrameAnalyser_T_init(&(me->frame_analyser_), me);
    PRP_Timer_T_init(&(me->supervision_tx_timer_));
    PRP_Timer_T_start(&(me->supervision_tx_timer_), me->supervision_.life_check_interval_);
    PRP_Timer_T_init(&(me->aging_timer_));
    PRP_Timer_T_start(&(me->aging_timer_), me->supervision_.check_interval_aging_);
    PRP_Timer_T_init(&(me->statistic_timer_));
    PRP_Timer_T_start(&(me->statistic_timer_), PRP_TIMER_STATISTIC_INTERVAL);
    PRP_LogItf_T_init(me);
}

/**
 * @fn void PRP_Environment_T_cleanup(PRP_Environment_T* const me)
 * @brief Clean up the PRP_Environment interface
 * @param   me PRP_Environment_T this pointer
 */
void PRP_Environment_T_cleanup(PRP_Environment_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    /* stop all the modules */
    PRP_Timer_T_cleanup(&(me->supervision_tx_timer_));
    PRP_Timer_T_cleanup(&(me->aging_timer_));
    PRP_Timer_T_cleanup(&(me->statistic_timer_));
    PRP_EnvironmentConfiguration_T_cleanup(&(me->environment_configuration_));
    PRP_Supervision_T_cleanup(&(me->supervision_));
    PRP_FrameAnalyser_T_cleanup(&(me->frame_analyser_));
}

