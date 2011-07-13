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


/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_print                  */
/************************************************************/
/** Print the discard table
 * */
void PRP_DiscardAlgorithm_PRP1_T_print(PRP_DiscardAlgorithm_PRP1_T* const me, uinteger32 level)
{
	int i, j;

	PRP_PRP_LOGOUT(level, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) return;

	PRP_PRP_LOGOUT(level, "%s\n", "======= Discard Table ================");
	for (i=0;i<TABLE_SIZE;i++){
		PRP_PRP_LOGOUT(level,"%d ",i);
		for (j=0;j<9;j++){
			PRP_PRP_LOGOUT(level,"%02x ", me->table[i][j]);
		}
		PRP_PRP_LOGOUT(level,"\n");
	}

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
	//new_entry: array[10]  --> [0-5]:mac; [6-9]:SeqNr
	int i;

	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) return(-PRP_ERROR_NULL_PTR);

	//search entry
	for (i=0;i<TABLE_SIZE;i++){
		if (me->table[i][0] == 0x01){  				//entry valid?
			if (seq_nr[1] == me->table[i][8]) { 	//last byte of SeqNr equal?
				 if (seq_nr[0]==me->table[i][7]){	//Sequence Number is equal, check mac
					 if ((mac[5]==me->table[i][6])&&(mac[4]==me->table[i][5])&&(mac[3]==me->table[i][4])&&
					 (mac[2]==me->table[i][3])&&(mac[1]==me->table[i][2])&&(mac[0]==me->table[i][1])){
						 PRP_PRP_LOGOUT(3,"\n---------- DROP ----------\n");
						 return PRP_DROP;
					 }
				 }
			}
		}
	}

	//entry not found, make new entry
	if (me->entry_pointer >= TABLE_SIZE){
		me->overflow++;
		me->entry_pointer = 0;
	}

    for (i=0;i<6;i++){					//set mac
    	me->table[me->entry_pointer][i+1]=mac[i];
    }

    me->table[me->entry_pointer][7]=seq_nr[0];
    me->table[me->entry_pointer][8]=seq_nr[1];


    me->table[me->entry_pointer][0]=0x01; //set valid flag

    me->entry_pointer++;
    PRP_PRP_LOGOUT(stderr,"\n---------- KEEP ----------\n");
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
	int delta, i;
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

	if(me == NULL_PTR) return;

    switch(me->overflow){
        case 0:
        	delta = me->entry_pointer - me->entry_pointer_old;
        break;
        case 1:
        	delta = TABLE_SIZE - me->entry_pointer_old + me->entry_pointer;
        break;
        	//if overflow > 1: no aging needed, whole table has been passed through at least once
        	delta = TABLE_SIZE; //this makes sure that the aging loop will be skipped
        break;
    }

    //do aging if necessary
	if (delta < TABLE_SIZE/4) {
		PRP_PRP_LOGOUT(3,"\nAGING is necessary\n");
		//make the necessary number of invalid entries
		for (i=0;i<(TABLE_SIZE/4-delta);i++){
			//deal with table overflow
			if ((me->entry_pointer + i)<TABLE_SIZE){
				me->table[me->entry_pointer + i][0]=0x00;
			}
			else{
				me->table[me->entry_pointer + i-TABLE_SIZE][0]=0x00;
			}
		}
		//adapt the entry pointer to the new value
		if (me->entry_pointer+i < TABLE_SIZE){
			me->entry_pointer = me->entry_pointer + (TABLE_SIZE/4-delta);
		}
		else{
			me->entry_pointer = me->entry_pointer + (TABLE_SIZE/4-delta)-TABLE_SIZE;
		}
	}

	me->entry_pointer_old = me->entry_pointer;
	me->overflow = 0;
	PRP_PRP_LOGOUT(3,"\nNew entry pointer: %d\n",me->entry_pointer);
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

	me->entry_pointer = 0;
	me->entry_pointer_old = 0;
	for (i=0;i<TABLE_SIZE;i++){
	    me->table[i][0]=0x00;
	    me->table[i][7]=(octet)(i);		// initialize the sequence numbers
	    me->table[i][8]=(octet)(i);		// to different values, this speeds up
	}									// search algorithm

}

/************************************************************/
/*       PRP_DiscardAlgorithm_PRP1_T_cleanup                */
/************************************************************/
void PRP_DiscardAlgorithm_PRP1_T_cleanup(PRP_DiscardAlgorithm_PRP1_T* const me)
{
	PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(me == NULL_PTR) return;

}
