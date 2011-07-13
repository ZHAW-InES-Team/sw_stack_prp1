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

#include "PRP_DropWindowTable_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_DropWindow_T.h"



/************************************************************/
/*       PRP_DropWindowTable_T_print                        */
/************************************************************/
void PRP_DropWindowTable_T_print(PRP_DropWindowTable_T* const me, uinteger32 level)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}

	PRP_PRP_LOGOUT(level, "%s\n", "====DropWindowTable=================");	
	PRP_PRP_LOGOUT(level, "cnt_drop_windows_:\t%u\n", me->cnt_drop_windows_);
	PRP_PRP_LOGOUT(level, "drop_window_table_empty_:\t%u\n", me->drop_window_table_empty_);
	PRP_PRP_LOGOUT(level, "%s\n", "====================================");	
}


/************************************************************/
/*       PRP_DropWindowTable_T_add_drop_window              */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindowTable_T_add_drop_window(PRP_DropWindowTable_T* const me, PRP_DropWindow_T* drop_window)
{
	PRP_DropWindow_T* new_drop_window;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
	
	new_drop_window = PRP_DropWindow_T_create(); /* create a new drop window */
	
	if(new_drop_window == NULL_PTR) /* memory full? */
	{
		return(NULL_PTR);
	}
		
	prp_memcpy(new_drop_window, drop_window, sizeof(PRP_DropWindow_T)); /* copy the values to the new drop window */
	
	/* here you could add the drop windows sorted, to make a binary search or so */
	if(me->last_drop_window_ == NULL_PTR) /* table empty ? */
	{
		me->first_drop_window_ = new_drop_window; /* the first entry in table */
	}	
	else
	{
		me->last_drop_window_->next_drop_window_ = new_drop_window; /* reconnect drop windows */
	}
	
	new_drop_window->previous_drop_window_ = me->last_drop_window_; /* added to the end */
	new_drop_window->next_drop_window_ = NULL_PTR; /* added to the end */
	me->last_drop_window_ = new_drop_window; /* new last entry in the table */
	
	me->cnt_drop_windows_++; /* increment the number of entries in the table */	
	me->drop_window_table_empty_ = FALSE; /* table not empty for sure */
		
	return(new_drop_window);
}

/************************************************************/
/*       PRP_DropWindowTable_T_remove_drop_window           */
/************************************************************/
void PRP_DropWindowTable_T_remove_drop_window(PRP_DropWindowTable_T* const me, PRP_DropWindow_T* drop_window)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
		
	if(drop_window->previous_drop_window_ == NULL_PTR)  /* was deleted drop window first drop window in table? */
	{
		me->first_drop_window_ = drop_window->next_drop_window_; /* new first entry */
	}
	else
	{
		drop_window->previous_drop_window_->next_drop_window_ = drop_window->next_drop_window_; /* reconnect drop windows */
	}
	
	if(drop_window->next_drop_window_ == NULL_PTR) /* was deleted drop window last drop window in table? */
	{
		me->last_drop_window_ = drop_window->previous_drop_window_; /* new last entry */
	}
	else
	{
		drop_window->next_drop_window_->previous_drop_window_ = drop_window->previous_drop_window_; /* reconnect drop windows */
	}
		
	PRP_DropWindow_T_destroy(drop_window); /* Release memory */
	
	drop_window = NULL_PTR;
	
	me->cnt_drop_windows_--; /* decrement the number of entries in the table */	
	if(me->cnt_drop_windows_ <= 0) /* check if table is empty */
	{
		me->drop_window_table_empty_ = TRUE;
	}	
}

/************************************************************/
/*       PRP_DropWindowTable_T_get_drop_window              */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindowTable_T_get_drop_window(PRP_DropWindowTable_T* const me, octet* drop_window_mac)
{
	PRP_DropWindow_T* drop_window;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
	
	drop_window = me->first_drop_window_; /* beginning of the table */
	
	/* here you could replace the linear search with a binary search or so */

	while(NULL_PTR != drop_window) /* check in worst case whole table, linear search*/
	{
		if(0 == prp_memcmp(drop_window->mac_address_, drop_window_mac, PRP_ETH_ADDR_LENGTH)) /* check if mac are equal => found */
		{
			return(drop_window);
		}
		
		drop_window = drop_window->next_drop_window_; /* go to the next drop window */
	}	
	
	return(NULL_PTR); /* not found */
}

/************************************************************/
/*       PRP_DropWindowTable_T_get_first_drop_window        */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindowTable_T_get_first_drop_window(PRP_DropWindowTable_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
	
	return(me->first_drop_window_);
}

/************************************************************/
/*       PRP_DropWindowTable_T_get_last_drop_window         */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindowTable_T_get_last_drop_window(PRP_DropWindowTable_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
	
	return(me->last_drop_window_);
}

/************************************************************/
/*       PRP_DropWindowTable_T_get_previous_drop_window     */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindowTable_T_get_previous_drop_window(PRP_DropWindowTable_T* const me, PRP_DropWindow_T* drop_window)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
	
	return(drop_window->previous_drop_window_);
}

/************************************************************/
/*       PRP_DropWindowTable_T_get_next_drop_window         */
/************************************************************/
PRP_DropWindow_T* PRP_DropWindowTable_T_get_next_drop_window(PRP_DropWindowTable_T* const me, PRP_DropWindow_T* drop_window)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
	
	return(drop_window->next_drop_window_);
}


/************************************************************/
/*       PRP_DropWindowTable_T_init                         */
/************************************************************/
void PRP_DropWindowTable_T_init(PRP_DropWindowTable_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	me->first_drop_window_ = NULL_PTR;
	me->last_drop_window_ = NULL_PTR;
	me->cnt_drop_windows_ = 0;
	me->drop_window_table_empty_ = TRUE;
}

/************************************************************/
/*       PRP_DropWindowTable_T_cleanup                      */
/************************************************************/
void PRP_DropWindowTable_T_cleanup(PRP_DropWindowTable_T* const me)
{
	PRP_DropWindow_T* drop_window;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	drop_window = me->first_drop_window_; /* beginning of the table */
	
	while(NULL_PTR != drop_window) /* go through whole list */
	{
		PRP_DropWindowTable_T_remove_drop_window(me, drop_window); /* deletes first drop window and sets the new first pointer*/
		drop_window = me->first_drop_window_; /* new beginning of the table */
	}	
}

