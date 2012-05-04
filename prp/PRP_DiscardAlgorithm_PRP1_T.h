/********************************************************************
*  
*  Copyright (c) 2011, Institute of Embedded Systems at
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
*  Version     : 1.1
*  Author      : itin
* 
*********************************************************************
*  Change History
*
*  Date     | Name     | Modification
************|**********|*********************************************
*  17.06.11 | itin     | file created
*********************************************************************/

#ifndef PRP_DiscardAlgorithm_PRP1_T_H
#define PRP_DiscardAlgorithm_PRP1_T_H
#define TABLE_SIZE 100

#include "PRP_Package_T.h"
#include "PRP_RedundancyControlTrailer_T.h"
#include "PRP_DataTypes_T.h"

struct PRP_DiscardAlgorithm_PRP1_T
{
	uinteger16 entry_pointer;
	uinteger16 entry_pointer_old;
	uinteger16 overflow;
	octet table[TABLE_SIZE][9];
};

void PRP_DiscardAlgorithm_PRP1_T_print(PRP_DiscardAlgorithm_PRP1_T* const me, uinteger32 level);
integer32 PRP_DiscardAlgorithm_PRP1_T_search_entry(PRP_DiscardAlgorithm_PRP1_T* const me, octet* mac, octet* seq_nr);
void PRP_DiscardAlgorithm_PRP1_T_do_aging(PRP_DiscardAlgorithm_PRP1_T* const me);
void PRP_DiscardAlgorithm_PRP1_T_init(PRP_DiscardAlgorithm_PRP1_T* const me);
void PRP_DiscardAlgorithm_PRP1_T_cleanup(PRP_DiscardAlgorithm_PRP1_T* const me);

#endif  
