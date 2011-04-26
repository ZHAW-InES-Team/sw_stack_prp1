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

#include "PRP_SupervisionFrame_T.h"
#include "PRP_LogItf_T.h"

/************************************************************/
/*       PRP_SupervisionFrame_T_print                       */
/************************************************************/
void PRP_SupervisionFrame_T_print(PRP_SupervisionFrame_T* const me, uinteger32 level)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_PRP_LOGOUT(level, "%s\n", "====SupervisionFrame================");	
	PRP_PRP_LOGOUT(level, "prp_version_:\t\t%u\n", me->prp_version_);
	PRP_PRP_LOGOUT(level, "type_:\t\t\t%u\n", me->type_);
	PRP_PRP_LOGOUT(level, "length_:\t\t%u\n", me->length_);
	
	PRP_PRP_LOGOUT(level, "%s\n", "source_mac_A_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->source_mac_A_[i]);
		
	}
	PRP_PRP_LOGOUT(level, "%s\n", "source_mac_B_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->source_mac_B_[i]);
		
	}
	PRP_PRP_LOGOUT(level, "type2_:\t\t\t%u\n", me->type2_);
	PRP_PRP_LOGOUT(level, "length2_:\t\t%u\n", me->length2_);
	
	PRP_PRP_LOGOUT(level, "%s\n", "red_box_mac_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->red_box_mac_[i]);
		
	}

	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	
	
}



/************************************************************/
/*       PRP_SupervisionFrame_T_init                        */
/************************************************************/
void PRP_SupervisionFrame_T_init(PRP_SupervisionFrame_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	prp_memset(me, 0, sizeof(PRP_SupervisionFrame_T));
}

/************************************************************/
/*       PRP_SupervisionFrame_T_cleanup                     */
/************************************************************/
void PRP_SupervisionFrame_T_cleanup(PRP_SupervisionFrame_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	prp_memset(me, 0, sizeof(PRP_SupervisionFrame_T));
}

