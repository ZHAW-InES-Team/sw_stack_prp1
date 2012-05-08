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
*  14.01.08 | mesv     | added some comments, changed the cost ptr
*********************************************************************/

#include "PRP_Bridging_T.h"
#include "PRP_BridgingFrame_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_Environment_T.h"

octet bridging_frame[53];       /* rapid spanning tree frame */

/* FLAGS */
#define AG_FW_LR_ROOT           0x78
#define AG_FW_LR_ROOT_TC        0x79
#define AG_ALT                  0x44
#define AG_ALT_TC               0x45


/**
 * @fn void PRP_Bridging_T_print(PRP_Bridging_T* const me, uinteger32 level)
 * @brief Function to print bridging status information
 *
 * @param   me PRP_Bridging_T this pointer
 * @param   level uinteger32 importance
 */
void PRP_Bridging_T_print(PRP_Bridging_T* const me, uinteger32 level)
{
    integer32 i;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    PRP_PRP_LOGOUT(level, "%s\n", "====RapidSpanningTree=====================");
    PRP_PRP_LOGOUT(level, "time_last_seen_A_:\t%016llu\n", me->time_last_seen_A_);
    PRP_PRP_LOGOUT(level, "time_last_seen_B_:\t%016llu\n", me->time_last_seen_B_);
    if(me->state_A_ == PRP_RAPID_SPANNING_TREE_UNKOWN)
    {
        PRP_PRP_LOGOUT(level, "%s\n", "state_A_:\t\tUNKOWN\n");
    }
    else if(me->state_A_ == PRP_RAPID_SPANNING_TREE_ALTERNATE)
    {
        PRP_PRP_LOGOUT(level, "%s\n", "state_A_:\t\tALTERNATE\n");
    }
    else if(me->state_A_ == PRP_RAPID_SPANNING_TREE_ROOT)
    {
        PRP_PRP_LOGOUT(level, "%s\n", "state_A_:\t\tROOT\n");
    }

    if(me->state_B_ == PRP_RAPID_SPANNING_TREE_UNKOWN)
    {
        PRP_PRP_LOGOUT(level, "%s\n", "state_B_:\t\tUNKOWN\n");
    }
    else if(me->state_B_ == PRP_RAPID_SPANNING_TREE_ALTERNATE)
    {
        PRP_PRP_LOGOUT(level, "%s\n", "state_B_:\t\tALTERNATE\n");
    }
    else if(me->state_B_ == PRP_RAPID_SPANNING_TREE_ROOT)
    {
        PRP_PRP_LOGOUT(level, "%s\n", "state_B_:\t\tROOT\n");
    }

    PRP_PRP_LOGOUT(level, "%s\t\n", "full_path_costs_:");
    for(i=0; i<22; i++)
    {
        PRP_PRP_LOGOUT(level, "%x\n ", me->full_path_costs_[i]);
    }
    PRP_PRP_LOGOUT(level, "%s\n", "====================================");
}

/**
 * @fn integer32 PRP_Bridging_T_compare(PRP_Bridging_T* const me, octet* full_path_costs)
 * @brief Compare the path costs
 *
 * @param   me PRP_Bridging_T this pointer
 * @param   full_path_costs octet pointer to the path
 * @return  int 0 : OK
 *          int <0 : ERROR (code)
 */
integer32 PRP_Bridging_T_compare(PRP_Bridging_T* const me, octet* full_path_costs)
{
    integer32 i;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    if(full_path_costs == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    for(i=0; i<22; i++) /* compare the path costs */
    {
        if(me->full_path_costs_[i] > full_path_costs[i])
        {
            return(1);
        }
        else if(me->full_path_costs_[i] < full_path_costs[i])
        {
            return(-1);
        }
    }

    return(0);
}

/**
 * @fn integer32 PRP_Bridging_T_statemachine(PRP_Bridging_T* const me, octet event, void* event_data, octet lan_id)
 * @brief Processes the events to the statemachine
 *
 * @param   me PRP_Bridging_T this pointer
 * @param   event octet event type
 * @param   event_data void pointer to the data that comes with this event
 * @param   lan_id octet on which LAN it was received
 * @return  integer32 0 : OK
 *          integer32 <0 : ERROR (code)
 */
integer32 PRP_Bridging_T_statemachine(PRP_Bridging_T* const me, octet event, void* event_data, octet lan_id)
{
    PRP_BridgingFrame_T* bridging_frame_payload;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    if(((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
        (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
        (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))&&
        (event_data == NULL_PTR)) /* if data is expectetd, is there some? */
    {
        return(-PRP_ERROR_NULL_PTR);
    }
    else
    {
        bridging_frame_payload = (PRP_BridgingFrame_T*)event_data;
    }

    /* statemachine */
    if((me->state_A_ == PRP_RAPID_SPANNING_TREE_UNKOWN) &&  (me->state_B_ == PRP_RAPID_SPANNING_TREE_UNKOWN))
    {
        if(lan_id == PRP_ID_LAN_A)
        {
            if((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
                (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                me->topologie_change_A_ = FALSE;
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ROOT;
                /*send BPDU A {AG, FW, LR, root, TC} wait 0.5s and repeat*/
                bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
            }
        }
        else
        {
            if((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
                (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                me->topologie_change_B_ = FALSE;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ROOT;
                /*send BPDU B {AG, FW, LR, root, TC} wait 0.5s and repeat*/
                bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
            }
        }
    }
    else if((me->state_A_ == PRP_RAPID_SPANNING_TREE_ROOT) &&  (me->state_B_ == PRP_RAPID_SPANNING_TREE_UNKOWN))
    {
        if(lan_id == PRP_ID_LAN_A)
        {
            if(event == PRP_RAPID_SPANNING_TREE_LINK_DOWN)
            {
                me->topologie_change_A_ = FALSE;
                me->state_A_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
            }
            else if((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                if(me->topologie_change_A_ == TRUE)
                {
                    me->topologie_change_A_ = FALSE;
                    /*send BPDU A {AG, FW, LR, root, TC} wait 0.5s and repeat */
                    bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                }
                else
                {
                    /* proposal flag not set */
                    if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) && (0 == (bridging_frame_payload->flags_ & 0x2)))
                    { }
                    else
                    {
                        /*send BPDU A {AG, FW, LR, root} */
                        bridging_frame_payload->flags_ = AG_FW_LR_ROOT;
                        PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                    }
                }
            }
        }
        else
        {
            if(event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED)
            {
                me->topologie_change_A_ = TRUE;/* wait for the next BPDU */
                me->topologie_change_B_ = FALSE;
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ALTERNATE;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ROOT;
                /*send BPDU B {AG, FW, LR, root, TC} wait 0.5s and repeat*/
                bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
            }
            else if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))

            {
                me->topologie_change_B_ = FALSE;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ALTERNATE;
                /*send BPDU B {AG, alt, TC} wait 0.5s and repeat*/
                bridging_frame_payload->flags_ = AG_ALT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
            }
        }
    }
    else if((me->state_A_ == PRP_RAPID_SPANNING_TREE_UNKOWN) &&  (me->state_B_ == PRP_RAPID_SPANNING_TREE_ROOT))
    {
        if(lan_id == PRP_ID_LAN_A)
        {
            if(event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED)
            {
                me->topologie_change_A_ = FALSE;
                me->topologie_change_B_ = TRUE; /* wait for the next BPDU */
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ROOT;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ALTERNATE;
                /*send BPDU A {AG, FW, LR, root, TC} wait 0.5s and repeat*/
                bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
            }
            else if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))

            {
                me->topologie_change_A_ = FALSE;
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ALTERNATE;
                /*send BPDU A {AG, alt, TC} wait 0.5s and repeat*/
                bridging_frame_payload->flags_ = AG_ALT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
            }
        }
        else
        {
            if(event == PRP_RAPID_SPANNING_TREE_LINK_DOWN)
            {
                me->topologie_change_B_ = FALSE;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
            }
            else if((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                if(me->topologie_change_B_ == TRUE)
                {
                    me->topologie_change_B_ = FALSE;
                    /*send BPDU B {AG, FW, LR, root, TC} wait 0.5s and repeat*/
                    bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                }
                else
                {
                    /* proposal flag not set */
                    if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) && (0 == (bridging_frame_payload->flags_ & 0x2)))
                    {
                    }
                    else
                    {
                        /*send BPDU B {AG, FW, LR, root} */
                        bridging_frame_payload->flags_ = AG_FW_LR_ROOT;
                        PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                    }
                }
            }
        }
    }
    else if((me->state_A_ == PRP_RAPID_SPANNING_TREE_ROOT) &&  (me->state_B_ == PRP_RAPID_SPANNING_TREE_ALTERNATE))
    {
        if(lan_id == PRP_ID_LAN_A)
        {
            if(event == PRP_RAPID_SPANNING_TREE_LINK_DOWN)
            {
                me->state_A_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ROOT;
                me->topologie_change_A_ = FALSE;
                me->topologie_change_B_ = TRUE; /* wait for the next BPDU */
            }
            else if((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                if(me->topologie_change_A_ == TRUE)
                {
                    me->topologie_change_A_ = FALSE;
                    /*send BPDU A {AG, FW, LR, root, TC} and repeat */
                    bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                }
                else
                {
                    /* proposal flag not set */
                    if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) && (0 == (bridging_frame_payload->flags_ & 0x2)))
                    {
                    }
                    else
                    {
                        /*send BPDU A {AG, FW, LR, root} */
                        bridging_frame_payload->flags_ = AG_FW_LR_ROOT;
                        PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                    }
                }
            }
        }
        else
        {
            if(event == PRP_RAPID_SPANNING_TREE_LINK_DOWN)
            {
                me->topologie_change_B_ = FALSE;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
            }
            else if(event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED)
            {
                me->topologie_change_A_ = TRUE; /* wait for the next BPDU */
                me->topologie_change_B_ = FALSE;
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ALTERNATE;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ROOT;
                /*send BPDU B {AG, FW, LR, root, TC} and repeat*/
                bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
            }
            else if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                if(me->topologie_change_B_ == TRUE)
                {
                    me->topologie_change_B_ = FALSE;
                    /*send BPDU B {AG, alt, TC}and repeat */
                    bridging_frame_payload->flags_ = AG_ALT_TC;
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                }
                else
                {
                    /* proposal flag not set */
                    if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) && (0 == (bridging_frame_payload->flags_ & 0x2)))
                    {
                    }
                    else
                    {
                        /*send BPDU B {AG, alt} */
                        bridging_frame_payload->flags_ = AG_ALT;
                        PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                    }
                }
            }
        }
    }
    else if((me->state_A_ == PRP_RAPID_SPANNING_TREE_ALTERNATE) &&  (me->state_B_ == PRP_RAPID_SPANNING_TREE_ROOT))
    {
        if(lan_id == PRP_ID_LAN_A)
        {
            if(event == PRP_RAPID_SPANNING_TREE_LINK_DOWN)
            {
                me->topologie_change_A_ = FALSE;
                me->state_A_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
            }
            else if(event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED)
            {
                me->topologie_change_A_ = FALSE;
                me->topologie_change_B_ = TRUE; /* wait for the next BPDU */
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ROOT;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_ALTERNATE;
                /*send BPDU A {AG, FW, LR, root, TC} and repeat*/
                bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
            }
            else if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                    (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                if(me->topologie_change_A_ == TRUE)
                {
                    me->topologie_change_A_ = FALSE;
                    /*send BPDU A {AG, alt, TC}and repeat */
                    bridging_frame_payload->flags_ = AG_ALT_TC;
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                }
                else
                {
                    /* proposal flag not set */
                    if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) && (0 == (bridging_frame_payload->flags_ & 0x2)))
                    {
                    }
                    else
                    {
                        /*send BPDU A {AG, alt} */
                        bridging_frame_payload->flags_ = AG_ALT;
                        PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_A);
                    }
                }
            }
        }
        else
        {
            if(event == PRP_RAPID_SPANNING_TREE_LINK_DOWN)
            {
                me->state_A_ = PRP_RAPID_SPANNING_TREE_ROOT;
                me->state_B_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
                me->topologie_change_A_ = TRUE; /* wait for the next BPDU */
                me->topologie_change_B_ = FALSE;

            }
            else if((event == PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED) ||
                (event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) ||
                (event == PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED))
            {
                if(me->topologie_change_B_ == TRUE)
                {
                    me->topologie_change_B_ = FALSE;
                    /*send BPDU B {AG, FW, LR, root, TC}and repeat */
                    bridging_frame_payload->flags_ = AG_FW_LR_ROOT_TC;
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                    PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                }
                else
                {
                    /* proposal flag not set */
                    if((event == PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED) && (0 == (bridging_frame_payload->flags_ & 0x2)))
                    {
                    }
                    else
                    {
                        /*send BPDU B {AG, FW, LR, root} */
                        bridging_frame_payload->flags_ = AG_FW_LR_ROOT;
                        PRP_Bridging_T_bridging_tx(me, bridging_frame_payload, PRP_ID_LAN_B);
                    }
                }
            }
        }
    }
    PRP_Bridging_T_print(me, 0);
    return(0);
}

/**
 * @fn integer32 PRP_Bridging_T_supervise(PRP_Bridging_T* const me)
 * @brief Supervision of the received BPDU, check for timeout
 *
 * @param   me PRP_Bridging_T this pointer
 * @return  integer32 0 : OK
 *          integer32 <0 : ERROR (code)
 */
integer32 PRP_Bridging_T_supervise(PRP_Bridging_T* const me)
{
    uinteger64 current_time;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    if(me->environment_->environment_configuration_.bridging_ == FALSE)
    {
        return(0);
    }

    current_time = prp_time();

    /* RSTP says 6 times the HELLO time to detect a link down */
    if((current_time - me->time_last_seen_A_) > ((uinteger64)6 * PRP_SECOND_IN_NANOSECONDS))
    {
        PRP_PRP_LOGOUT(0, "%s\n", "link down on LAN A");
        PRP_Bridging_T_statemachine(me, PRP_RAPID_SPANNING_TREE_LINK_DOWN, NULL_PTR, PRP_ID_LAN_A);
    }

    /* RSTP says 6 times the HELLO time to detect a link down */
    if((current_time - me->time_last_seen_B_) > ((uinteger64)6 * PRP_SECOND_IN_NANOSECONDS))
    {
        PRP_PRP_LOGOUT(0, "%s\n", "link down on LAN B");
        PRP_Bridging_T_statemachine(me, PRP_RAPID_SPANNING_TREE_LINK_DOWN, NULL_PTR, PRP_ID_LAN_B);
    }

    return(0);
}

/**
 * @fn integer32 PRP_Bridging_T_bridging_rx(PRP_Bridging_T* const me, octet* data, uinteger32* length, octet lan_id)
 * @brief Receives a BPDU and does the actual processing.
 *
 * @param   me PRP_Bridging_T this pointer
 * @param   data octet pointer to the beginning of the frame
 * @param   length uinteger32 pointer to the length in bytes of the frame
 * @param   octet lan_id on which LAN it was received
 * @return  integer32 0 : OK
 *          <0 : ERROR (code)
 */
integer32 PRP_Bridging_T_bridging_rx(PRP_Bridging_T* const me, octet* data, uinteger32* length, octet lan_id)
{
    PRP_BridgingFrame_T bridging_frame_payload;
    integer32 res;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    if((*length >= 53) && (data[12] == 0x00) && (data[13] == 0x27) &&
        (data[14] == 0x42) && (data[15] == 0x42) &&
        (data[16] == 0x03) && (data[17] == 0x00) &&
        (data[18] == 0x00) && (data[19] == 0x02)) /* if rapid spanning tree frame */
    {
        prp_memcpy(&bridging_frame_payload, &(data[21]), sizeof(PRP_BridgingFrame_T));
    }
    else
    {
        PRP_PRP_LOGOUT(2, "%s\n", "detected frame was no rapid spanning tree frame");
        return(-PRP_ERROR_FRAME_COR);
    }

    PRP_PRP_LOGOUT(3, "frame from LAN %X\n", lan_id);
    PRP_BridgingFrame_T_print(&bridging_frame_payload, 3);

    if(lan_id == PRP_ID_LAN_A)
    {
        me->time_last_seen_A_ = prp_time();
    }
    else
    {
        me->time_last_seen_B_ = prp_time();
    }

    if(me->environment_->environment_configuration_.bridging_ == FALSE)
    {
        PRP_Frames_T_normal_rx(&(me->environment_->frame_analyser_.frames_), data, length, lan_id);
        return(PRP_DROP);
    }

    res = PRP_Bridging_T_compare(me, &(data[22])); /* compare costs Byte 22 to 43 */

    prp_memcpy(me->full_path_costs_, &(data[22]), 22); /* store the new path costs in all cases */

    if(res > 0) /* better */
    {
        PRP_PRP_LOGOUT(2, "BPDU with better costs received on LAN %X", lan_id);
        PRP_Bridging_T_statemachine(me, PRP_RAPID_SPANNING_TREE_BETTER_RECEIVED, ((void*)(&bridging_frame_payload)), lan_id);
    }
    else if(res < 0) /* worse */
    {
        PRP_PRP_LOGOUT(2, "BPDU with worse costs received on LAN %X", lan_id);
        PRP_Bridging_T_statemachine(me, PRP_RAPID_SPANNING_TREE_WORSE_RECEIVED, ((void*)(&bridging_frame_payload)), lan_id);
    }
    else /* equal */
    {
        PRP_PRP_LOGOUT(2, "BPDU with equal costs received on LAN %X", lan_id);
        PRP_Bridging_T_statemachine(me, PRP_RAPID_SPANNING_TREE_EQUAL_RECEIVED, ((void*)(&bridging_frame_payload)), lan_id);
    }

    /* just to keep the counters valid */
    PRP_Frames_T_normal_rx(&(me->environment_->frame_analyser_.frames_), data, length, lan_id);

    return(PRP_DROP); /* always drop rapid spanning tree frames because they are already proceeded */
}

/**
 * @fn integer32 PRP_Bridging_T_bridging_tx(PRP_Bridging_T* const me, PRP_BridgingFrame_T* bridging_frame_payload, octet lan_id)
 * @brief Transmission of a supervision frame.
 *
 * @param   me PRP_Bridging_T this pointer
 * @param   bridging_frame_payload PRP_Bridging_T pointer to the frame to send
 * @param   lan_id octet to the LAN id
 * @return  integer32 O : OK
 *          integer32 <0 : ERROR (code)
 */
integer32 PRP_Bridging_T_bridging_tx(PRP_Bridging_T* const me, PRP_BridgingFrame_T* bridging_frame_payload, octet lan_id)
{
    uinteger16 temp;
    integer32 i;
    uinteger32 temp_length;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    if(bridging_frame_payload == NULL_PTR)
    {
        return(-PRP_ERROR_NULL_PTR);
    }

    /* RSTP MAC*/
    bridging_frame[0] = 0x01;
    bridging_frame[1] = 0x80;
    bridging_frame[2] = 0xc2;
    bridging_frame[3] = 0x00;
    bridging_frame[4] = 0x00;
    bridging_frame[5] = 0x00;

    /* The "bridge port" adress not the "bridge" address TODO (or solution -> followed)
    for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
    {
        bridging_frame[6+i] = me->environment_->environment_configuration_.mac_address_A_[i];
    }
    */

    /* Approach with administrated mac addresses with gaps of 3 addresses between nodes */
    if(lan_id == PRP_ID_LAN_A)
    {
        temp = 1; /* increment of 1 to the bridge address */
    }
    else
    {
        temp = 2; /* increment of 2 to the bridge address */
    }
    /* upper 3 Bytes are reserved and predefined if locally administrated */
    bridging_frame[6] = me->environment_->environment_configuration_.mac_address_A_[0]; /* dont touch that byte */
    bridging_frame[7] = me->environment_->environment_configuration_.mac_address_A_[1]; /* dont touch that byte */
    bridging_frame[8] = me->environment_->environment_configuration_.mac_address_A_[2]; /* dont touch that byte */

    for(i=PRP_ETH_ADDR_LENGTH-1; i>2; i--) /* add the increment to the lower 3 Bytes */
    {
        temp += me->environment_->environment_configuration_.mac_address_A_[i];
        bridging_frame[6+i] = (octet)temp;
        temp = temp >> 8;
    }

    bridging_frame[12] = 0x00;	// length h */
    bridging_frame[13] = 0x27;	/* length l */
    bridging_frame[14] = 0x42;	/* DSAP */
    bridging_frame[15] = 0x42;	/* SSAP */
    bridging_frame[16] = 0x03;	/* LLC */
    bridging_frame[17] = 0x00;	/* protocol id h */
    bridging_frame[18] = 0x00;	/* protocol id l */
    bridging_frame[19] = 0x02;	/* protocol version */
    bridging_frame[20] = 0x02;	/* BPDU type */
    /* copy the values from the recieved BPDU */
    prp_memcpy(&(bridging_frame[21]), bridging_frame_payload, sizeof(PRP_BridgingFrame_T));

    bridging_frame[34] = 0xf0; /* maximum bridge prio id */
    bridging_frame[35] = 0x00;

    /*	Not the Port MAC, but the Bridge MAC, what probably is MAC A	*/
    for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
    {
        bridging_frame[36+i] = me->environment_->environment_configuration_.mac_address_A_[i];
    }

    if(lan_id == PRP_ID_LAN_A) /* set port prio and id */
    {
        bridging_frame[42] = 0x80;
        bridging_frame[43] = 0x01;
    }
    else
    {
        bridging_frame[42] = 0x80;
        bridging_frame[43] = 0x02;
    }

    PRP_PRP_LOGOUT(2, "frame to LAN %X\n", lan_id);
    PRP_BridgingFrame_T_print(bridging_frame_payload, 3);

    temp_length = sizeof(bridging_frame);

    /* send it to e particular port */
    return(PRP_Environment_T_process_tx(me->environment_, bridging_frame, &temp_length, lan_id));
}

/**
 * @fn void PRP_Bridging_T_init(PRP_Bridging_T* const me, PRP_Environment_T* const environment)
 * @brief Initialize the PRP_Bridging interface
 *
 * @param   me PRP_Bridging_T this pointer
 * @param   environment PRP_Environment_T pointer to the environment
 */
void PRP_Bridging_T_init(PRP_Bridging_T* const me, PRP_Environment_T* const environment)
{
    integer32 i;
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    if(environment == NULL_PTR)
    {
        return;
    }

    prp_memset(bridging_frame, 0, sizeof(bridging_frame));
    me->environment_ = environment;
    me->state_A_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
    me->state_B_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
    me->time_last_seen_A_ = prp_time(); /* to avoid a timeout */
    me->time_last_seen_B_ = prp_time(); /* to avoid a timeout */

    for(i=0; i<22; i++)
    {
        me->full_path_costs_[i] = 0xFF;	/* first frame is always better */
    }

    me->topologie_change_A_ = FALSE;
    me->topologie_change_B_ = FALSE;
}

/**
 * @fn void PRP_Bridging_T_cleanup(PRP_Bridging_T* const me)
 * @brief Clean up the PRP_Bridging interface
 *
 * @param   me PRP_Bridging_T this pointer
 */
void PRP_Bridging_T_cleanup(PRP_Bridging_T* const me)
{
    integer32 i;
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    prp_memset(bridging_frame, 0, sizeof(bridging_frame));
    me->environment_ = NULL_PTR;
    me->state_A_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
    me->state_B_ = PRP_RAPID_SPANNING_TREE_UNKOWN;
    me->time_last_seen_A_ = prp_time();
    me->time_last_seen_B_ = prp_time();

    for(i=0; i<22; i++)
    {
        me->full_path_costs_[i] = 0xFF;
    }

    me->topologie_change_A_ = FALSE;
    me->topologie_change_B_ = FALSE;
}

