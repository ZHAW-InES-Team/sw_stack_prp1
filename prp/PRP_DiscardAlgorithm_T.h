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
*  11.05.12 | asdo     | discard algorithm modification
*  30.11.15 | beti     | adopted for Linux version 4.2
*  30.11.15 | beti     | added new statistic values
*********************************************************************/

#ifndef PRP_DiscardAlgorithm_T_H
#define PRP_DiscardAlgorithm_T_H

#include "PRP_Package_T.h"
#include "PRP_RedundancyControlTrailer_T.h"
#include "PRP_DataTypes_T.h"

#define DISCARD_ITEM_COUNT          1024
#define DISCARD_LIST_ENTRY_COUNT    256      /* 2^n, n is 8 in this case */
#define DISCARD_HASH_MASK           0x00FF   /* Must select n bit in a range from 1 to 16 */
#define DISCARD_TICK_COUNT          20       /* 20ms -> 400ms */
#define AGING_COUNT					5		 /* do aging all 100ms */


struct PRP_DiscardAlgorithm_T
{
    /* first list orded by hash, second list orded from newest to oldest
     * because we dont want to process dead items all the time */
    struct PRP_DiscardAlgorithm_DiscardItem_T **hash_list;
    /* list of free items to use */
    struct PRP_DiscardAlgorithm_DiscardItem_T *free_list;
    /* from oldest to newest */
    struct PRP_DiscardAlgorithm_DiscardItem_T *chronology;
    /* shortcut to newest item */
    struct PRP_DiscardAlgorithm_DiscardItem_T *newest;

    uinteger32 time;
    uinteger8 ageing_counter;

    int used_item_count;

    PRP_Environment_T* environment_;
};

struct PRP_DiscardAlgorithm_DiscardItem_T
{
    unsigned short seq_nr;
    unsigned char src_mac[6];

    uinteger32 timestamp;
    unsigned short hash;

    struct PRP_DiscardAlgorithm_DiscardItem_T *previous;       /* Used only for hash_list */
    struct PRP_DiscardAlgorithm_DiscardItem_T *next;           /* Used only for hash_list */

    struct PRP_DiscardAlgorithm_DiscardItem_T *previous_alt;   /* Used only for chronology */
    struct PRP_DiscardAlgorithm_DiscardItem_T *next_alt;       /* Used only for free_list and chronology */

    unsigned short cnt_duplicates_arrived_;
};

void PRP_DiscardAlgorithm_T_print(PRP_DiscardAlgorithm_T* const me, const char* drop_or_keep);
void PRP_DiscardAlgorithm_T_check_consistency(PRP_DiscardAlgorithm_T* const me);

integer32 PRP_DiscardAlgorithm_T_search_entry(PRP_DiscardAlgorithm_T* const me, octet* mac, octet* seq_nr);
void PRP_DiscardAlgorithm_T_do_aging(PRP_DiscardAlgorithm_T* const me);
void PRP_DiscardAlgorithm_T_init(PRP_DiscardAlgorithm_T* const me, PRP_Environment_T* const environment);
void PRP_DiscardAlgorithm_T_cleanup(PRP_DiscardAlgorithm_T* const me);

#endif /* PRP_DiscardAlgorithm_T_H */

