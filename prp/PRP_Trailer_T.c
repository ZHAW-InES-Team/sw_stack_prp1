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

#include "PRP_Trailer_T.h"
#include "PRP_LogItf_T.h"


/************************************************************/
/*       PRP_Trailer_T_add_trailer                          */
/************************************************************/
void PRP_Trailer_T_add_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length, PRP_RedundancyControlTrailer_T* trailer) 
{
	uinteger16 size;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	if((data[12] == 0x81) && (data[13] == 0x00))
	{
		// calculate LSDU_size for VLAN-Tagged frame
		size = *length - 18 + PRP_RCT_LENGTH;
	}
	else
	{
		// calculate LSDU_size for not VLAN-Tagged frame
		size = *length - 14 + PRP_RCT_LENGTH;
	}
	
	/* can only be done because frame is in a much bigger buffer */
	*((uinteger16*)(&(data[(*length)]))) = prp_htons(trailer->seq_);
	*((uinteger16*)(&(data[((*length) + 2)]))) = prp_htons((((trailer->lan_id_ & 0x000F) << 12) | (size & 0x0FFF)));
	
	*length = *length + PRP_RCT_LENGTH;
}

/************************************************************/
/*       PRP_Trailer_T_get_trailer                          */
/************************************************************/
PRP_RedundancyControlTrailer_T* PRP_Trailer_T_get_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length) 
{
	integer32 i;
	uinteger16 size_offset;
	uinteger32 trailer_offset;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return(NULL_PTR);
	}
			
	if(*length < ((2*PRP_ETH_ADDR_LENGTH)+2))
	{
		return(NULL_PTR);
	}
	
	if((data[12] == 0x81) && (data[13] == 0x00)) /* VLAN tagged LSDU starts at offset 18 */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "vlan tagged frame");
		size_offset = 18;
	}
	else /* otherwise at offset 14 */
	{
		PRP_PRP_LOGOUT(2, "%s\n", "no vlan tagged frame");
		size_offset = 14;
	}
	
	trailer_offset = 0;
	
	if(*length <= 64) /* for small frames the trailer probably is not at the end */
	{
		for(i=*length; i>=(size_offset+PRP_RCT_LENGTH); i--)  /* search trailer */
		{
			if((*((uinteger16*)(&(data[(i-2)]))) == prp_htons((0xA000 | ((i-size_offset) & 0x0FFF)))) ||
			   (*((uinteger16*)(&(data[(i-2)]))) == prp_htons((0xB000 | ((i-size_offset) & 0x0FFF)))))
			{
				trailer_offset = i;
			}	
		}
	}
	else if(*length > 64)
	{
		if((*((uinteger16*)(&(data[(*length-2)]))) == prp_htons((0xA000 | ((*length-size_offset) & 0x0FFF)))) ||
		   (*((uinteger16*)(&(data[(*length-2)]))) == prp_htons((0xB000 | ((*length-size_offset) & 0x0FFF)))))
		{
			trailer_offset = *length;
		}	
	}
		
	if(trailer_offset != 0)
	{
		me->redundancy_control_trailer_.seq_ = prp_ntohs((*((uinteger16*)(&(data[(trailer_offset-4)])))));
		me->redundancy_control_trailer_.lan_id_ = (0xf000 & prp_ntohs((*((uinteger16*)(&(data[(trailer_offset-2)])))))) >> 12;
		return(&(me->redundancy_control_trailer_)); 
	}
		
	PRP_PRP_LOGOUT(2, "frame had no trailer, last 4 byte: %x %x %x %x\n", data[(*length-4)],data[(*length-3)], data[(*length-2)], data[(*length-1)]);
	return(NULL_PTR); /* No trailer found */
}

/************************************************************/
/*       PRP_Trailer_T_remove_trailer                       */
/************************************************************/
void PRP_Trailer_T_remove_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length) 
{
	integer32 i;
	uinteger16 size_offset;
	
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
		
	if(*length < ((2*PRP_ETH_ADDR_LENGTH)+2))
	{
		return;
	}
	
	if((data[12] == 0x81) && (data[13] == 0x00)) /* VLAN tagged LSDU starts at offset 18 */
	{
		size_offset = 18;
	}
	else /* otherwise at offset 14 */
	{
		size_offset = 14;
	}
	
	for(i=*length; i>=(size_offset+PRP_RCT_LENGTH); i--) /* search trailer */
	{
		if(*((uinteger16*)(&(data[(i-2)]))) == prp_htons((0xA000 | ((i-size_offset) & 0x0FFF)))) /* redundancy control trailer with identifier LAN_A */
		{
			*length = i-PRP_RCT_LENGTH; /* just set length field to new value */
			return; 
		}
		else if(*((uinteger16*)(&(data[(i-2)]))) == prp_htons((0xB000 | ((i-size_offset) & 0x0FFF)))) /* redundancy control trailer with identifier LAN_B */
		{
			*length = i-PRP_RCT_LENGTH; /* just set length field to new value */
			return; 
		}
	}
		
	return; /* No trailer found */
}

/************************************************************/
/*       PRP_Trailer_T_init                                 */
/************************************************************/
void PRP_Trailer_T_init(PRP_Trailer_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_RedundancyControlTrailer_T_init(&(me->redundancy_control_trailer_));
}

/************************************************************/
/*       PRP_Trailer_T_cleanup                              */
/************************************************************/
void PRP_Trailer_T_cleanup(PRP_Trailer_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR)
	{
		return;
	}
	
	PRP_RedundancyControlTrailer_T_cleanup(&(me->redundancy_control_trailer_));
}

