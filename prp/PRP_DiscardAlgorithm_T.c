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
*  14.01.08 | mesv     | added some comments and a printout
*********************************************************************/

#include "PRP_DiscardAlgorithm_T.h"
#include "PRP_LogItf_T.h"


/************************************************************/
/*       PRP_DiscardAlgorithm_T_print                       */
/************************************************************/
void PRP_DiscardAlgorithm_T_print(PRP_DiscardAlgorithm_T* const me, uinteger32 level)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}

	PRP_PRP_LOGOUT(level, "%s\n", "====DiscardAlgorithm================");	
	PRP_PRP_LOGOUT(level, "drop_window_size_:\t%u\n", me->drop_window_size_);
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	

}


/************************************************************/
/*       PRP_DiscardAlgorithm_T_drop_window_decision        */
/************************************************************/
integer32 PRP_DiscardAlgorithm_T_drop_window_decision(PRP_DiscardAlgorithm_T* const me, PRP_RedundancyControlTrailer_T* trailer, PRP_DropWindow_T* drop_window)
{
	integer32 res;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
		
	if(trailer->lan_id_ == PRP_ID_LAN_A) /* received frame from LAN A */
	{
		if(drop_window->expected_seq_B_ < drop_window->start_seq_B_) /* drop window at wrap around? */
		{
	
			if((trailer->seq_ >= drop_window->start_seq_B_) || (trailer->seq_ <= (drop_window->expected_seq_B_ - 1))) /* drop? */
			{
				drop_window->start_seq_A_ = trailer->seq_ + 1; /* reset drop window to 0*/
				drop_window->start_seq_B_ = trailer->seq_ + 1; /* shrink drop window */		
				PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_DROP");			
				res = PRP_DROP;
			}
			else /* keep */
			{
				drop_window->start_seq_B_ = drop_window->expected_seq_B_; /* reset drop window to 0*/	
					
				if(trailer->seq_ != drop_window->expected_seq_A_) /* out of order */
				{
					drop_window->start_seq_A_ = trailer->seq_; /* reset drop window to 1*/
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_OUT_OF_ORDER");			
					res = PRP_KEEP_OUT_OF_ORDER;
					
				}
				else /* in order */
				{
					if((drop_window->expected_seq_A_ + ((PRP_DROP_WINDOW_SIZE + 1) - drop_window->start_seq_A_)) >= me->drop_window_size_) /* drop window max size reached */
					{
						drop_window->start_seq_A_ = (trailer->seq_ + 1) - me->drop_window_size_; /* maximum size */
					}
					else
					{
						/* keep start sequence */
					}
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_IN_ORDER");			
					res = PRP_KEEP_IN_ORDER;
				}
			}
		}
		else /* not at wrap around */
		{
			if((trailer->seq_ >= drop_window->start_seq_B_) && (trailer->seq_ <= (drop_window->expected_seq_B_ - 1))) /* drop? */
			{
				drop_window->start_seq_A_ = trailer->seq_ + 1; /* reset drop window to 0*/
				drop_window->start_seq_B_ = trailer->seq_ + 1; /* shrink drop window */		
				PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_DROP");			
				res = PRP_DROP;
			}
			else /* keep */
			{
				drop_window->start_seq_B_ = drop_window->expected_seq_B_; /* reset drop window to 0*/		
								
				if(trailer->seq_ != drop_window->expected_seq_A_) /* out of order */
				{
					drop_window->start_seq_A_ = trailer->seq_; /* reset drop window to 1*/
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_OUT_OF_ORDER");			
					res = PRP_KEEP_OUT_OF_ORDER;
				}
				else /* in order */
				{			
					if((drop_window->expected_seq_A_ - drop_window->start_seq_A_) >= me->drop_window_size_) /* drop window max size reached */
					{
						drop_window->start_seq_A_ = (trailer->seq_ + 1) - me->drop_window_size_; /* maximum size */
					}
					else
					{
						/* keep start sequence */
					}					
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_IN_ORDER");			
					res = PRP_KEEP_IN_ORDER;
				}
			}			
		}
		
		drop_window->expected_seq_A_ = trailer->seq_ + 1; /* in every case */
	}
	else if(trailer->lan_id_ == PRP_ID_LAN_B) /* received frame from LAN B */
	{
		if(drop_window->expected_seq_A_ < drop_window->start_seq_A_) /* drop window at wrap around? */
		{
	
			if((trailer->seq_ >= drop_window->start_seq_A_) || (trailer->seq_ <= (drop_window->expected_seq_A_ - 1))) /* drop? */
			{
				drop_window->start_seq_B_ = trailer->seq_ + 1; /* reset drop window to 0*/
				drop_window->start_seq_A_ = trailer->seq_ + 1; /* shrink drop window */		
				PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_DROP");			
				res = PRP_DROP;
			}
			else /* keep */
			{
				drop_window->start_seq_A_ = drop_window->expected_seq_A_; /* reset drop window to 0*/	
				
				if(trailer->seq_ != drop_window->expected_seq_B_) /* out of order */
				{
					drop_window->start_seq_B_ = trailer->seq_; /* reset drop window to 1*/
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_OUT_OF_ORDER");			
					res = PRP_KEEP_OUT_OF_ORDER;
					
				}
				else /* in order */
				{
					if((drop_window->expected_seq_B_ + ((PRP_DROP_WINDOW_SIZE + 1) - drop_window->start_seq_B_)) >= me->drop_window_size_) /* drop window max size reached */
					{
						drop_window->start_seq_B_ = (trailer->seq_ + 1) - me->drop_window_size_; /* maximum size */
					}
					else
					{
						/* keep start sequence */
					}				
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_IN_ORDER");			
					res = PRP_KEEP_IN_ORDER;
				}
			}
		}
		else /* not at wrap around */
		{
			if((trailer->seq_ >= drop_window->start_seq_A_) && (trailer->seq_ <= (drop_window->expected_seq_A_ - 1))) /* drop? */
			{
				drop_window->start_seq_B_ = trailer->seq_ + 1; /* reset drop window to 0*/
				drop_window->start_seq_A_ = trailer->seq_ + 1; /* shrink drop window */		
				PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_DROP");			
				res = PRP_DROP;
			}
			else /* keep */
			{
				drop_window->start_seq_A_ = drop_window->expected_seq_A_; /* reset drop window to 0*/
				
				if(trailer->seq_ != drop_window->expected_seq_B_) /* out of order */
				{
					drop_window->start_seq_B_ = trailer->seq_; /* reset drop window to 1*/
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_OUT_OF_ORDER");			
					res = PRP_KEEP_OUT_OF_ORDER;
				}
				else /* in order */
				{			
					if((drop_window->expected_seq_B_ - drop_window->start_seq_B_) >= me->drop_window_size_) /* drop window max size reached */
					{
						drop_window->start_seq_B_ = (trailer->seq_ + 1) - me->drop_window_size_; /* maximum size */
					}
					else
					{
						/* keep start sequence */
					}		
					
					PRP_PRP_LOGOUT(2, "%s\n", "decision: PRP_KEEP_IN_ORDER");			
					res = PRP_KEEP_IN_ORDER;
				}
			}			
		}		
		
		drop_window->expected_seq_B_ = trailer->seq_ + 1;	/* in every case */
		
	}
	else
	{
		return(-PRP_ERROR_WRONG_VAL);		
	}

	return(res);
}


/************************************************************/
/*       PRP_DiscardAlgorithm_T_init                        */
/************************************************************/
void PRP_DiscardAlgorithm_T_init(PRP_DiscardAlgorithm_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->drop_window_size_ = PRP_DROP_WINDOW_MAX_SIZE; /* half the drop window value range */
}


/************************************************************/
/*       PRP_DiscardAlgorithm_T_cleanup                     */
/************************************************************/
void PRP_DiscardAlgorithm_T_cleanup(PRP_DiscardAlgorithm_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->drop_window_size_ = PRP_DROP_WINDOW_MAX_SIZE;
}
