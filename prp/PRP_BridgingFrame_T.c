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

#include "PRP_BridgingFrame_T.h"
#include "PRP_LogItf_T.h"

/************************************************************/
/*       PRP_BridgingFrame_T_print                       */
/************************************************************/
void PRP_BridgingFrame_T_print(PRP_BridgingFrame_T* const me, uinteger32 level)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}

	PRP_PRP_LOGOUT(level, "%s\n", "====BridgingFrame================");	
	PRP_PRP_LOGOUT(level, "flags_:\t\t\t0x%01x\n", me->flags_);
	PRP_PRP_LOGOUT(level, "root_prio_and_id_:\t0x%02x\n", prp_ntohs(me->root_prio_and_id_));	
	PRP_PRP_LOGOUT(level, "%s\n", "root_mac_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%01x\n", me->root_mac_[i]);
		
	}
	PRP_PRP_LOGOUT(level, "root_path_costs_:\t%u\n", prp_ntohl(me->root_path_costs_));
	PRP_PRP_LOGOUT(level, "bridge_prio_and_id_:\t0x%02x\n", prp_ntohs(me->bridge_prio_and_id_));		
	PRP_PRP_LOGOUT(level, "%s\n", "bridge_mac_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%01x\n", me->bridge_mac_[i]);
		
	}
	PRP_PRP_LOGOUT(level, "port_prio_and_id_:\t0x%02x\n", prp_ntohs(me->port_prio_and_id_));		
	PRP_PRP_LOGOUT(level, "message_age_:\t\t%u\n", prp_ntohs(me->message_age_));
	PRP_PRP_LOGOUT(level, "max_age_:\t\t%u\n", prp_ntohs(me->max_age_));
	PRP_PRP_LOGOUT(level, "hello_time_:\t\t%u\n", prp_ntohs(me->hello_time_));
	PRP_PRP_LOGOUT(level, "forward_delay_:\t\t%u\n", prp_ntohs(me->forward_delay_));
	PRP_PRP_LOGOUT(level, "v1_length_:\t\t%u\n", me->v1_length_);
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	
	
}



/************************************************************/
/*       PRP_BridgingFrame_T_init                        */
/************************************************************/
void PRP_BridgingFrame_T_init(PRP_BridgingFrame_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	prp_memset(me, 0, sizeof(PRP_BridgingFrame_T)); /* set to all 0 */
}

/************************************************************/
/*       PRP_BridgingFrame_T_cleanup                     */
/************************************************************/
void PRP_BridgingFrame_T_cleanup(PRP_BridgingFrame_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) 
	{
		return;
	}
	
	prp_memset(me, 0, sizeof(PRP_BridgingFrame_T)); /* set to all 0 */
}

