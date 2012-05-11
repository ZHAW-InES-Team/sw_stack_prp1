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
************|**********|*********************************************/
/* Implements a discard table with TABLE_SIZE entries.
*  Discard table:
*  octets:  | 0 	| 1-6 | 7-8   |
*  		    | valid | mac | SeqNr |
* */

#include "PRP_DiscardAlgorithm_PRP1_T.h"
#include "PRP_LogItf_T.h"
#include <stdio.h>

static struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T discardalgorithm_items_[DISCARD_ITEM_COUNT];
static struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *discardalgorithm_list_[DISCARD_LIST_ENTRY_COUNT];


/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_print                  */
/************************************************************/
/** Print the discard table
 * */
void PRP_DiscardAlgorithm_PRP1_T_print(PRP_DiscardAlgorithm_PRP1_T* const me, uinteger32 level)
{
	int i;
	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item;

	PRP_PRP_LOGOUT(level, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) return;

	//PRP_PRP_LOGOUT(level, "%s\n", "======= Discard Table ================");

	for ( i=0; i<DISCARD_LIST_ENTRY_COUNT; i++ ) {
		if ( me->hash_list[i] != 0 ) {
			//prp_printf( "entry %i: ", i );
			item = me->hash_list[i];
			while ( item != 0 ) {
				//prp_printf( "[0x%x] ", item );
				item = item->next;
			}
			//prp_printf( "\n", i );
		}
	}

	/*
	for (i=0;i<TABLE_SIZE;i++){
		PRP_PRP_LOGOUT(level,"%d ",i);
		for (j=0;j<9;j++){
			PRP_PRP_LOGOUT(level,"%02x ", me->table[i][j]);
		}
		PRP_PRP_LOGOUT(level,"\n");
	}
	*/

}

/************************************************************/
/* 		PRP_DiscardAlgorithm_PRP1_T_search_entry            */
/************************************************************/
/** Scan the table for an entry
 *  If the entry was found, return discard
 *  If the entry was not found, make new entry, return keep
 * */
integer32 PRP_DiscardAlgorithm_PRP1_T_search_entry(PRP_DiscardAlgorithm_PRP1_T* const me, octet* mac, octet* seq_nr)
{

	unsigned short seqnr_corr;
	unsigned short hash;
	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

	if(me == NULL_PTR) return(-PRP_ERROR_NULL_PTR);

	seqnr_corr = seq_nr[1];
	seqnr_corr |= (seq_nr[0]<<8);
	hash = seqnr_corr & DISCARD_HASH_MASK;

	// search entry

	PRP_DISCARD_LOGOUT( 1, "New frame received, seqnr=%i, hash=%i\n", seqnr_corr, hash );

	item = me->hash_list[hash];
	while ( item != 0 ) {

		if ( seqnr_corr == item->seq_nr ) {
			if (    (item->src_mac[0]==mac[0]) && 
				(item->src_mac[1]==mac[1]) &&
				(item->src_mac[2]==mac[2]) &&
				(item->src_mac[3]==mac[3]) &&
				(item->src_mac[4]==mac[4]) &&
				(item->src_mac[5]==mac[5])    )
			{

				// duplicate found

				// unlink from hash table

				if ( item->next != 0 )  {
					item->next->previous = item->previous;
				}

				if ( item->previous != 0 )  {
					item->previous->next = item->next;
				} else {
					me->hash_list[hash] = item->next;
				}

				// unlink from chronology

				if ( item->next_alt != 0 )  {
					item->next_alt->previous_alt = item->previous_alt;
				} else {
					me->newest = item->previous_alt;
				}

				if ( item->previous_alt != 0 )  {
					item->previous_alt->next = item->next_alt;
				} else {
					me->chronology = item->next_alt;
				}

				// add to free list

				item->next_alt = me->free_list;
				me->free_list = item;

				#ifdef PRP_DEBUG_LOG
				me->used_item_count--;
				#endif

				//PRP_DiscardAlgorithm_PRP1_T_print

				PRP_DISCARD_LOGOUT( 1, "DROP, %i items used\n", me->used_item_count );

				return PRP_DROP;

			}
		}

		item = item->next;
	}

	// get item

	item = me->free_list;
	if ( item != 0 ) {

		me->free_list = item->next_alt;
		#ifdef PRP_DEBUG_LOG
		me->used_item_count++;
		#endif

	} else {

		item = me->chronology;
		me->chronology = item->next_alt;
		me->chronology->previous_alt = 0;
		// me->newest != me->chronology I dont't have to check this

		item->previous->next = 0;
		// item->next must be 0 in this case because this is the oldest item of all

	}

	// add item to hash table

	if ( me->hash_list[hash] != 0 ) {
		me->hash_list[hash]->previous = item;
	}
	item->next = me->hash_list[hash];
	me->hash_list[hash] = item;
	item->previous = 0;

	// add item to chronology

	if ( me->chronology != 0 ) {
		// me->newest is not 0 too
		me->newest->next_alt = item;
	} else {
		me->chronology = item;
	}
	item->next_alt = 0;
	item->previous_alt = me->newest;
	me->newest = item;

	// populate item

	item->seq_nr = seqnr_corr;
	item->src_mac[0] = mac[0];
	item->src_mac[1] = mac[1];
	item->src_mac[2] = mac[2];
	item->src_mac[3] = mac[3];
	item->src_mac[4] = mac[4];
	item->src_mac[5] = mac[5];

	gettimeofday( &item->tv, 0 );

	item->hash = hash;

	PRP_DISCARD_LOGOUT( 1, "KEEP, %i items used\n", me->used_item_count );

	return PRP_KEEP;
}

/************************************************************/
/* 		PRP_DiscardAlgorithm_PRP1_T_do_aging	            */
/************************************************************/
/** Call this function every 100ms (aging time 400ms)
 *  If the entry pointer has not increased for at least 1/4
 *  of the table, delete the valid flags of the next quarter
 *  of the table
 * */
void PRP_DiscardAlgorithm_PRP1_T_do_aging(PRP_DiscardAlgorithm_PRP1_T* const me)
{

	struct timeval tv_now;
	struct timeval tv_delta;
	struct timeval tv;
	int i;

	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item;
	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *new_oldest;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

	tv_delta.tv_sec = 0;
	tv_delta.tv_usec = 400000;
	gettimeofday( &tv_now, 0 );
	timersub( &tv_now, &tv_delta, &tv );

	#ifdef PRP_DEBUG_LOG
	i = 0;
	#endif

	new_oldest = 0;
	item = me->chronology;
	while ( item != 0 ) {

		if ( timercmp( &item->tv, &tv, > ) ) {
			new_oldest = item;
			break;
		} else {

			// unlink from hash table

			if ( item->previous != 0 )  {
				// It is allowed to set item->previous->next to 0 instead of item->next because 
				// item->next is older and was already unlinked
				item->previous->next = 0;
			} else {
				// Because we move from the oldest to the newest and me->hash_list[x] is sorted
				// from the newest to the oldest we did not set item->previous to 0 by accident
				// item->previous still means - this is the first entry of the hash entry
				me->hash_list[item->hash] = 0;
			}

			#ifdef PRP_DEBUG_LOG
			i++;
			#endif

		}

		item = item->next_alt;
	}

	if ( new_oldest != 0 ) {

		if ( me->chronology != new_oldest ) {

			#ifdef PRP_DEBUG_LOG
			me->used_item_count -= i;	
			#endif
			PRP_DISCARD_LOGOUT( 1, "Free %i items, %i still used\n", i, me->used_item_count );
			
			// add to free list
			new_oldest->previous_alt->next_alt = me->free_list;
			me->free_list = me->chronology;

			// unlink from chronology
			me->chronology = new_oldest;
			new_oldest->previous_alt = 0;

		}

	} else {

		if ( me->chronology != 0 ) {

			#ifdef PRP_DEBUG_LOG
			me->used_item_count -= i;
			#endif
			PRP_DISCARD_LOGOUT( 1, "Free all %i items, %i still used\n", i, me->used_item_count );

			// add to free list
			me->newest->next_alt = me->free_list;
			me->free_list = me->chronology;

			// unlink from chronology
			me->chronology = 0;
			me->newest = 0;

		}

	}

}

/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_init                   */
/************************************************************/
/** Set table valid to zero
 *  Set table entry pointer and entry pointer old to first
 *  entry
 * */
void PRP_DiscardAlgorithm_PRP1_T_init(PRP_DiscardAlgorithm_PRP1_T* const me)
{
	int i;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

	if(me == NULL_PTR) return;

	// init hash list
	for (i=0; i<DISCARD_LIST_ENTRY_COUNT; i++) {
		discardalgorithm_list_[i] = 0;
	}
	me->hash_list = discardalgorithm_list_;

	// init discard items
	for (i=0; i<(DISCARD_ITEM_COUNT-1); i++) {
		discardalgorithm_items_[i].next_alt = &discardalgorithm_items_[i+1];
	}
	discardalgorithm_items_[DISCARD_ITEM_COUNT-1].next_alt = 0;
	me->free_list = &discardalgorithm_items_[0];

	// init chronology
	me->chronology = 0;
	me->newest = 0;

	me->used_item_count = 0;

}

/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_cleanup                */
/************************************************************/
void PRP_DiscardAlgorithm_PRP1_T_cleanup(PRP_DiscardAlgorithm_PRP1_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) return;

}
