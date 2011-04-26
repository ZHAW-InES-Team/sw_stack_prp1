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

#include "PRP_Environment_T.h"
#include "PRP_LogItf_T.h"


/************************************************************/
/*       PRP_Environment_T_process_timer                    */
/************************************************************/
void PRP_Environment_T_process_timer(PRP_Environment_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	if(TRUE == PRP_Timer_T_tick(&(me->bridging_timer_)))
	{
		PRP_Bridging_T_supervise(&(me->bridging_)); /* check for link down */
		PRP_Timer_T_start(&(me->bridging_timer_), PRP_TIMER_TICK_INTERVAL); /* to detect link down fast */
	}

	if(TRUE == PRP_Timer_T_tick(&(me->supervise_timer_)))
	{
		PRP_Supervision_T_supervise(&(me->supervision_)); /* check for aged out nodes and failed nodes etc */
		PRP_Timer_T_start(&(me->supervise_timer_), me->supervision_.life_check_interval_);
	}

	if(TRUE == PRP_Timer_T_tick(&(me->supervision_tx_timer_)))
	{
		PRP_Supervision_T_supervision_tx(&(me->supervision_)); /* send a new supervision frame */
		PRP_Timer_T_start(&(me->supervision_tx_timer_), me->supervision_.life_check_interval_);
	}
}

/************************************************************/
/*       PRP_Environment_T_process_rx                       */
/************************************************************/
integer32 PRP_Environment_T_process_rx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
		
	return(PRP_FrameAnalyser_T_analyse_rx(&(me->frame_analyser_), data, length, lan_id)); 	/* forward to the frame analyser */
}

/************************************************************/
/*       PRP_Environment_T_process_tx                       */
/************************************************************/
integer32 PRP_Environment_T_process_tx(PRP_Environment_T* const me, octet* data, uinteger32* length, octet lan_id)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	return(PRP_FrameAnalyser_T_analyse_tx(&(me->frame_analyser_), data, length, lan_id)); 	/* forward to the frame analyser */
}


/************************************************************/
/*       PRP_Environment_T_init                             */
/************************************************************/
void PRP_Environment_T_init(PRP_Environment_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	/* start all the modules */
	PRP_EnvironmentConfiguration_T_init(&(me->environment_configuration_)); 
	PRP_Supervision_T_init(&(me->supervision_), me); 
	PRP_Bridging_T_init(&(me->bridging_), me); 
	PRP_NodeTable_T_init(&(me->node_table_));
	PRP_DiscardAlgorithm_T_init(&(me->discard_algorithm_));
	PRP_FrameAnalyser_T_init(&(me->frame_analyser_), me);
	PRP_Timer_T_init(&(me->bridging_timer_));
	PRP_Timer_T_start(&(me->bridging_timer_), 0);
	PRP_Timer_T_init(&(me->supervise_timer_));
	PRP_Timer_T_start(&(me->supervise_timer_), me->supervision_.life_check_interval_);
	PRP_Timer_T_init(&(me->supervision_tx_timer_));
	PRP_Timer_T_start(&(me->supervision_tx_timer_), me->supervision_.life_check_interval_);
}

/************************************************************/
/*       PRP_Environment_T_cleanup                          */
/************************************************************/
void PRP_Environment_T_cleanup(PRP_Environment_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	/* stop all the modules */
	PRP_Timer_T_cleanup(&(me->bridging_timer_));
	PRP_Timer_T_cleanup(&(me->supervise_timer_));
	PRP_Timer_T_cleanup(&(me->supervision_tx_timer_));
	PRP_EnvironmentConfiguration_T_cleanup(&(me->environment_configuration_));
	PRP_Supervision_T_cleanup(&(me->supervision_));
	PRP_Bridging_T_cleanup(&(me->bridging_));
	PRP_NodeTable_T_cleanup(&(me->node_table_));
	PRP_DiscardAlgorithm_T_cleanup(&(me->discard_algorithm_));
	PRP_FrameAnalyser_T_cleanup(&(me->frame_analyser_));
}
