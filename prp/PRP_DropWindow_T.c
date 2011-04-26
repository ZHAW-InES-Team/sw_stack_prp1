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

#include "PRP_DropWindow_T.h"
#include "PRP_LogItf_T.h"


/************************************************************/
/*       PRP_DropWindow_T_print                             */
/************************************************************/
void PRP_DropWindow_T_print(PRP_DropWindow_T* const me, uinteger32 level)
{
	integer32 i;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}

	PRP_PRP_LOGOUT(level, "%s\n", "====DropWindow======================");	
	
	PRP_PRP_LOGOUT(level, "%s\n", "mac_address_:");
	for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
	{
		PRP_PRP_LOGOUT(level, "%x\n", me->mac_address_[i]);
		
	}
	
	PRP_PRP_LOGOUT(level, "start_seq_A_:\t\t%u\n", me->start_seq_A_);
	PRP_PRP_LOGOUT(level, "start_seq_B_:\t\t%u\n", me->start_seq_B_);
	PRP_PRP_LOGOUT(level, "expected_seq_A_:\t%u\n", me->expected_seq_A_);
	PRP_PRP_LOGOUT(level, "expected_seq_B_:\t%u\n", me->expected_seq_B_);
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	
	
}
/************************************************************/
/*       PRP_DropWindow_T_init                              */
/************************************************************/
void PRP_DropWindow_T_init(PRP_DropWindow_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->previous_drop_window_ = NULL_PTR; /* unconnected drop window at this point */
	me->next_drop_window_ = NULL_PTR;	/* unconnected drop window at this point */
	prp_memset(me->mac_address_, 0, PRP_ETH_ADDR_LENGTH);	
	me->start_seq_A_ = 0;
	me->start_seq_B_ = 0;
	me->expected_seq_A_ = 0;
	me->expected_seq_B_ = 0;
}

/************************************************************/
/*       PRP_DropWindow_T_cleanup                           */
/************************************************************/
void PRP_DropWindow_T_cleanup(PRP_DropWindow_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->previous_drop_window_ = NULL_PTR;
	me->next_drop_window_ = NULL_PTR;
	
	prp_memset(me->mac_address_, 0, PRP_ETH_ADDR_LENGTH);
	
	me->start_seq_A_ = 0;
	me->start_seq_B_ = 0;
	me->expected_seq_A_ = 0;
	me->expected_seq_B_ = 0;
}


/************************************************************/
/*       PRP_DropWindow_T_create                            */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindow_T_create(void)
{
	PRP_DropWindow_T* drop_window;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
    drop_window = (PRP_DropWindow_T*)prp_malloc(sizeof(PRP_DropWindow_T));
    
	if(drop_window != NULL_PTR)
	{
		PRP_DropWindow_T_init(drop_window);
	}	
	
    return(drop_window);
}

/************************************************************/
/*       PRP_DropWindow_T_destroy                           */
/************************************************************/
void PRP_DropWindow_T_destroy(PRP_DropWindow_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_DropWindow_T_cleanup(me);
	prp_free(me);
}

