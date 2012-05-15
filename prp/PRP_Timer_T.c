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

#include "PRP_Timer_T.h"
#include "PRP_LogItf_T.h"

/**
 * @fn void PRP_Timer_T_start(PRP_Timer_T* const me, uinteger16 timeout) 
 * @brief Starts or restarts a timer
 * @param   me PRP_Timer_T this pointer
 * @param   timeout uinteger16 timeout in milliseconds
 */
void PRP_Timer_T_start(PRP_Timer_T* const me, uinteger16 timeout) 
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    me->enabled_ = TRUE;

    if ((timeout % PRP_TIMER_TICK_INTERVAL) != 0) {
        PRP_PRP_LOGOUT(3, "%s\n", "Timerinterval to coarse, normalised timeout is not an integer");
    }

    /* logical timer in number of timer ticks */
    me->timer_ = timeout / PRP_TIMER_TICK_INTERVAL;
}

/**
 * @fn void PRP_Timer_T_stop(PRP_Timer_T* const me) 
 * @brief Stops a running timer
 * @param   me PRP_Timer_T this pointer
 */
void PRP_Timer_T_stop(PRP_Timer_T* const me) 
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    me->enabled_ = FALSE;
}

/**
 * @fn boolean PRP_Timer_T_tick(PRP_Timer_T* const me)  
 * @brief Decrements the value_ variable until timeout happened
 * @param   me PRP_Timer_T this pointer
 * @retval  TRUE Timer expired
 * @retval  FALSE Timer decremented
 */
boolean PRP_Timer_T_tick(PRP_Timer_T* const me)  
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return(FALSE);
    }

    if (me->enabled_ == FALSE) {
        return(FALSE);
    }

    /* countdown */
    if ( me->timer_ >  0 ) {
        me->timer_--;
    }

    /* timeout */
    if ( me->timer_ <= 0 ) {
        me->enabled_ = FALSE;
        return(TRUE);
    }

    return(FALSE);
}

/**
 * @fn void PRP_Timer_T_init(PRP_Timer_T* const me) 
 * @brief Initialize the PRP_Timer interface
 * @param   me PRP_Timer_T this pointer
 */
void PRP_Timer_T_init(PRP_Timer_T* const me) 
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    me->enabled_ = FALSE;
    me->timer_ = 0;
}

/**
 * @fn void PRP_Timer_T_cleanup(PRP_Timer_T* const me) 
 * @brief Clean up the PRP_Timer interface
 * @param   me PRP_Timer_T this pointer
 */
void PRP_Timer_T_cleanup(PRP_Timer_T* const me) 
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR){
        return;
    }
    me->enabled_ = FALSE;
    me->timer_ = 0;
}

