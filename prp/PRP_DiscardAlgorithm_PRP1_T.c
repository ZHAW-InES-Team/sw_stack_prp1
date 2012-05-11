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


#ifdef PRP_DEBUG_LOG

/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_print                  */
/************************************************************/
/** Print the discard table
 * */
void PRP_DiscardAlgorithm_PRP1_T_print(PRP_DiscardAlgorithm_PRP1_T* const me)
{
	int i;
	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) return;

	PRP_DISCARD_LOGOUT(2, "%s\n", "======= Discard Table ================");

	for ( i=0; i<DISCARD_LIST_ENTRY_COUNT; i++ ) {
		if ( me->hash_list[i] != 0 ) {
			PRP_DISCARD_LOGOUT( 2, "entry %i: ", i );
			item = me->hash_list[i];
			while ( item != 0 ) {
				PRP_DISCARD_S_LOGOUT( 2, "[0x%x] ", item );
				item = item->next;
			}
			PRP_DISCARD_S_LOGOUT( 2, "\n", i );
		}
	}

	if ( discard_debug_level >= 3 ) {

		i=0;
		item = me->free_list;
		while ( item != 0 ) {
			i++;
			item = item->next_alt;
		}
		PRP_DISCARD_LOGOUT( 3, "free_list: %i items\n", i );

		i=0;
		item = me->chronology;
		while ( item != 0 ) {
			i++;
			item = item->next_alt;
		}
		PRP_DISCARD_LOGOUT( 3, "chronology: %i items\n", i );

	}

	PRP_DISCARD_LOGOUT(2, "%s\n", "======= END Discard Table ============");

}

/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_check_consistency      */
/************************************************************/
/** Check consistency of data structures
 * */
void PRP_DiscardAlgorithm_PRP1_T_check_consistency(PRP_DiscardAlgorithm_PRP1_T* const me)
{

	int i;

	int hash_item_count;
	int chronology_count;
	int freelist_count;

	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item;
	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item2;

	unsigned char b;

	if ( discard_debug_level < 4 ) return;
	
	// check hash table

	hash_item_count = 0;

	for ( i=0; i<DISCARD_LIST_ENTRY_COUNT; i++ ) {
		if ( me->hash_list[i] != 0 ) {
			item = me->hash_list[i];

			if ( item != 0 ) {
				if ( item->previous != 0 ) {
					PRP_DISCARD_LOGOUT( 4, "!!! item->previous of first hash-entry is not 0\n", i );
				}
			}

			while ( item != 0 ) {
				hash_item_count++;
				
				item2 = me->free_list;
				while ( item2 != 0 ) {
					if ( item == item2 ) {
						PRP_DISCARD_LOGOUT( 4, "!!! hash item is in free list\n", i );
						break;
					}
					item2 = item2->next_alt;
				}

				b = 0;
				item2 = me->chronology;
				while ( item2 != 0 ) {
					if ( item == item2 ) {
						b = 1;
						break;
					}
					item2 = item2->next_alt;
				}
				if ( b == 0 ) {
					PRP_DISCARD_LOGOUT( 4, "!!! hash item is not in chronology\n", i );
				}
 
				item = item->next;

			}
		}
	}

	// chronology

	chronology_count = 0;
	
	if ( me->chronology != 0 ) {
		
		if ( me->chronology->previous_alt != 0 ) {
			PRP_DISCARD_LOGOUT( 4, "!!! me->chronology->previous_alt is not 0\n", i );
		}

		item = me->chronology;
		while ( item != 0 ) {
			chronology_count++;
			
			if ( item->next_alt != 0 ) {
				if ( timercmp( &item->tv, &item->next_alt->tv, > ) ) {
					PRP_DISCARD_LOGOUT( 4, "!!! wrong chronology order\n", i );
				}
			} else {
				if ( item != me->newest ) {
					PRP_DISCARD_LOGOUT( 4, "!!! last item in chronology is not me->newest\n", i );
				}
			}
			
			item = item->next_alt;
		}

	} else {
		if ( hash_item_count != 0 ) {
			PRP_DISCARD_LOGOUT( 4, "!!! no chronology but hash items\n", i );
		}
	}

	// free list

	freelist_count = 0;
	
	if ( me->free_list != 0 ) {
		
		item = me->free_list;
		while ( item != 0 ) {
			freelist_count++;
			item = item->next_alt;
		}

	} else {

		if ( hash_item_count != DISCARD_ITEM_COUNT ) {
			PRP_DISCARD_LOGOUT( 4, "!!! no free items but not all items used\n", i );
		}

		if ( chronology_count != DISCARD_ITEM_COUNT ) {
			PRP_DISCARD_LOGOUT( 4, "!!! no free items but not all items in chronology\n", i );
		}

	}

	// item count

	if ( hash_item_count != chronology_count ) {
		PRP_DISCARD_LOGOUT( 4, "!!! hash item count and chronology count is not equal\n", i );
	}
	
	if ( (freelist_count + hash_item_count) != DISCARD_ITEM_COUNT ) {
		PRP_DISCARD_LOGOUT( 4, "!!! items got lost somewhere in space!?\n", i );
	}

}

#endif

/************************************************************/
/* 		PRP_DiscardAlgorithm_PRP1_T_search_entry    */
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
					item->previous_alt->next_alt = item->next_alt;
				} else {
					me->chronology = item->next_alt;
				}

				// add to free list

				item->next_alt = me->free_list;
				me->free_list = item;

				#ifdef PRP_DEBUG_LOG
				me->used_item_count--;
				#endif

				#ifdef PRP_DEBUG_LOG
				PRP_DiscardAlgorithm_PRP1_T_print( me );
				PRP_DiscardAlgorithm_PRP1_T_check_consistency( me );
				#endif

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

	#ifdef PRP_DEBUG_LOG
	PRP_DiscardAlgorithm_PRP1_T_print( me );
	PRP_DiscardAlgorithm_PRP1_T_check_consistency( me );
	#endif

	return PRP_KEEP;
}

/************************************************************/
/* 		PRP_DiscardAlgorithm_PRP1_T_do_aging	            */
/************************************************************/
/** Call this function every 100ms (aging time 400ms)
 *  Aging time callcullation: Count(seq_nr) / Max Frames per ms
 *  -> 2^16 / ~144 = ~450 ms 
 * */
void PRP_DiscardAlgorithm_PRP1_T_do_aging(PRP_DiscardAlgorithm_PRP1_T* const me)
{

	struct timeval tv_now;
	struct timeval tv_delta;
	struct timeval tv;
	#ifdef PRP_DEBUG_LOG
	int i;
	#endif

	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *item;
	struct PRP_DiscardAlgorithm_DiscardItem_PRP1_T *new_oldest;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

	tv_delta.tv_sec = DISCARD_AGE_SEC;
	tv_delta.tv_usec = DISCARD_AGE_USEC;
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

	#ifdef PRP_DEBUG_LOG
	PRP_DiscardAlgorithm_PRP1_T_check_consistency( me );
	#endif

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
