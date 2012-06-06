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
*********************************************************************
*  13.07.11 | itin     | supervision tx for PRP1: only send one mac
*********************************************************************
*  23.05.12 | walh     | no further use for node table handling
*********************************************************************/

#include "PRP_Supervision_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_Environment_T.h"

/* Supervision frame with VLAN and trailer -> avoids dynamic mem allocation
 * prp_version_  PRP version : 0x0000
 * type_         Mode: 0x20 for Duplicate Discard, 0x21 for Duplicate Accept
 * length_       LSPDU: 12
 * source_mac_A_ Source mac of adapter A of the local node
 * source_mac_B_ Source mac of adapter B of the local node
 */
octet supervision_frame_[64];

/**
 * @fn void PRP_Supervision_T_print(PRP_Supervision_T* const me, uinteger32 level)
 * @brief Print PRP_Supervision status information
 * @param   me PRP_Supervision_T this pointer
 * @param   level uinteger32 importance
 */
void PRP_Supervision_T_print(PRP_Supervision_T* const me, uinteger32 level)
{
    integer32 i;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    PRP_PRP_LOGOUT(level, "%s\n", "====Supervision=====================");
    PRP_PRP_LOGOUT(level, "life_check_interval_:\t%u\n", me->life_check_interval_);
    PRP_PRP_LOGOUT(level, "%s\n", "supervision_address_:");
    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        PRP_PRP_LOGOUT(level, "%x\n", me->supervision_address_[i]);
    }
    PRP_PRP_LOGOUT(level, "%s\n", "====================================");
}

/**
 * @fn integer32 PRP_Supervision_T_supervision_rx(PRP_Supervision_T* const me, octet* data, uinteger32* length, octet lan_id)
 * @brief Receives a supervision frame and does the actual processing.
 * @param   me PRP_Supervision_T this pointer
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 pointer to the length in bytes of the frame
 * @param   lan_id octet on which LAN it was received
 * @retval  0 integer32 OK
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_Supervision_T_supervision_rx(PRP_Supervision_T* const me, octet* data, uinteger32* length, octet lan_id)
{
    integer32 i;
    boolean node_valid;
    integer32 pos;
    octet sup_path;
    uinteger16 sup_version;
    uinteger16 sup_sequence_number;
    octet tlv_type;
    octet tlv_length;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return(-PRP_ERROR_NULL_PTR);
    }

    if (*length < 60) {
        PRP_PRP_LOGOUT(0, "%s\n", "frame too short for supervision");
        return(-PRP_ERROR_FRAME_COR);
    }

    pos = 12;
    if (data[pos] == 0x81 && data[pos+1] == 0x00) {
        // skip VLAN tag
        pos += 4;
    }

    if (data[pos] != 0x88 || data[pos+1] != 0xFB) {
        PRP_PRP_LOGOUT(0, "%s\n", "detected frame was no supervision frame");
        return(-PRP_ERROR_FRAME_COR);
    }
    pos += 2;

    sup_path = data[pos] >> 4;
    sup_version = (data[pos++] & 0xF) << 8;
    sup_version |= data[pos++];

    PRP_PRP_LOGOUT(3, "%s\n", "====SupervisionFrame================");
    PRP_PRP_LOGOUT(3, "sup_path:\t\t\t%u\n", (uinteger16)sup_path);
    PRP_PRP_LOGOUT(3, "sup_version:\t\t%u\n", sup_version);
    sup_path = sup_path;    /* suppress unused variable warning */

    sup_sequence_number  = data[pos++] << 8;
    sup_sequence_number |= data[pos++];
    PRP_PRP_LOGOUT(3, "sup_sequence_number:\t\t%u\n", sup_sequence_number);

    if (sup_version == 0) {
        PRP_PRP_LOGOUT(1, "%s\n", "Error: PRP-0 supervision frame received, should not happen in PRP-1 network! Dropping.");
        return(-PRP_ERROR_FRAME_COR);
    }

    if (sup_version > 1) {
        PRP_PRP_LOGOUT(2, "%s\n", "Warning: parsing supervision frame with version more recent than 1.");
        /* "Implementation of version X of the protocol shall interpret
         * version >X as if they were version X" */
    }

    /* Starting to decode TLVs for the rest of this method */
    node_valid = FALSE;
    while (1) {
        tlv_type   = data[pos++];
        tlv_length = data[pos++];
        PRP_PRP_LOGOUT(3, "tlv_type  :\t\t%u\n", tlv_type);
        PRP_PRP_LOGOUT(3, "tlv_length:\t\t%u\n", tlv_length);

        if (tlv_type == 0) {
            /* end of TLV list */
            break;
        }

        /* check TLV: duplicate discard or duplicate accept */
        if (tlv_type == 20 || tlv_type == 21 || tlv_type == 23) {
            if (tlv_type == 20 || tlv_type == 23) {
                /* PRP node with duplicate discard, or HSR node */
                PRP_PRP_LOGOUT(2, "%s\n", "node is running in duplicate discard mode");
            } else {
                /* PRP node with duplicate accept (ONLY USED for debugging) */
                PRP_PRP_LOGOUT(1, "%s\n", "node is running in duplicate accept mode");
            }

            /* HSR or PRP node MAC address */
            if (tlv_length == PRP_ETH_ADDR_LENGTH || tlv_length == 2*PRP_ETH_ADDR_LENGTH) {
                node_valid = TRUE;
                PRP_PRP_LOGOUT(3, "%s\n", "source_mac_A_:");
                for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
                    PRP_PRP_LOGOUT(3, "%x\n", data[pos+i]);
                }

                if (tlv_length > PRP_ETH_ADDR_LENGTH) {
                    PRP_PRP_LOGOUT(3, "%s\n", "source_mac_B_:");
                    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
                        PRP_PRP_LOGOUT(3, "%x\n", data[pos+PRP_ETH_ADDR_LENGTH+i]);
                    }
                }
                else {
                    PRP_PRP_LOGOUT(3, "%s\n", "source_mac_B_: not given (assuming same as source_mac_A_)");
                }
            }
            else {
                PRP_PRP_LOGOUT(1, "%s\n", "Error: supervision with unknown MAC address format, ignoring node.");
            }
        }
        else {
            PRP_PRP_LOGOUT(3, "%s\n", "skipping this tlv");
        }

        pos += tlv_length;
        if (pos >= *length) {
            /* next TLV would start beyond end of frame */
            PRP_PRP_LOGOUT(1, "%s\n", "Error: received supervision with TLV pointing beyond end of frame. Ignoring.");
            break;
        }
    }

    if (node_valid == FALSE) {
        PRP_PRP_LOGOUT(1,"%s\n", "Error: did not find node MAC in supervision TLV. Dropping.");
        return(-PRP_ERROR_FRAME_COR);
    }

    PRP_Frames_T_normal_rx(&(me->environment_->frame_analyser_.frames_), data, length, lan_id);

    /* TODO probably add some information if a DAN is actualy a VDAN or
     * not (second TLV), not necessary for algorithm */

    /* always drop supervision frames because they are allready proceeded */
    return(PRP_DROP);
}

/**
 * @fn integer32 PRP_Supervision_T_supervision_tx(PRP_Supervision_T* const me)
 * @brief Transmission of a supervision frame.
 * @param   me PRP_Supervision_T this pointer
 * @retval  0 integer32 OK
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_Supervision_T_supervision_tx(PRP_Supervision_T* const me)
{
    integer32 i;
    uinteger32 length;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return(-PRP_ERROR_NULL_PTR);
    }

    length = 0;

    /*  ethernet DST */
    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        supervision_frame_[length++] = me->supervision_address_[i];
    }
    /* ethernet SRC */
    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        supervision_frame_[length++] = me->environment_->environment_configuration_.mac_address_A_[i];
    }
    /* ethertype */
    supervision_frame_[length++] = 0x88;
    supervision_frame_[length++] = 0xFB;

    /* supervision frame payload */
    supervision_frame_[length++] = 0; // SupPath and MSB of SupVersion
    supervision_frame_[length++] = 1; // LSB of SupVersion

    /* SupSequenceNumber */
    supervision_frame_[length++] = me->supervision_seqno_ >> 8;
    supervision_frame_[length++] = me->supervision_seqno_;
    me->supervision_seqno_++;

    /* Supervision TLV1 (20 or 21) */
    /* what mode are we running on */
    if (me->environment_->environment_configuration_.duplicate_discard_ == TRUE) {
        supervision_frame_[length++] = 20; // duplicate accept
    }
    else {
        /* duplicate accept is expected to disappear in PRP-1 */
        supervision_frame_[length++] = 21; // duplicate discard
    }
    supervision_frame_[length++] = PRP_ETH_ADDR_LENGTH;
    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        supervision_frame_[length++] = me->environment_->environment_configuration_.mac_address_A_[i];
    }
    /*for(i=0; i<PRP_ETH_ADDR_LENGTH; i++)
    {
        supervision_frame_[length++] = me->environment_->environment_configuration_.mac_address_B_[i];
    }*/

    // Supervision TLV2 (only for RedBox)
    // We are not a RedBox.

    // Supervision TLV3 (only for RedBox)
    // We are not a RedBox.

    // Supervision TLV4 (end of TLVs)
    supervision_frame_[length++] = 0;
    supervision_frame_[length++] = 0;

    PRP_PRP_LOGOUT(2, "%s\n", "sending supervision frame");

    return(PRP_Environment_T_process_tx(me->environment_, supervision_frame_, &length, 0x00));
}

/**
 * @fn void PRP_Supervision_T_init(PRP_Supervision_T* const me, PRP_Environment_T* const environment)
 * @brief Initialize the PRP_Supervision interface
 * @param   me PRP_Supervision_T this pointer
 * @param   environment PRP_Environment_T pointer to the environment type
 */
void PRP_Supervision_T_init(PRP_Supervision_T* const me, PRP_Environment_T* const environment)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    prp_memset(supervision_frame_, 0, sizeof(supervision_frame_));
    me->environment_ = environment;
    me->life_check_interval_ = PRP_LIFE_CHECK_INTERVAL;
    me->check_interval_aging_ = PRP_CHECK_INTERVAL_AGING;

    me->supervision_address_[0] = 0x01;
    me->supervision_address_[1] = 0x15;
    me->supervision_address_[2] = 0x4e;
    me->supervision_address_[3] = 0x00;
    me->supervision_address_[4] = 0x01;
    me->supervision_address_[5] = 0x00; /* default */

    me->supervision_seqno_ = 0;
}

/**
 * @fn void PRP_Supervision_T_cleanup(PRP_Supervision_T* const me)
 * @brief Clean up the PRP_Supervision interface
 * @param   me PRP_Supervision_T this pointer
 */
void PRP_Supervision_T_cleanup(PRP_Supervision_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    prp_memset(supervision_frame_, 0, sizeof(supervision_frame_));
    me->environment_ = NULL_PTR;
    me->life_check_interval_ = PRP_LIFE_CHECK_INTERVAL;
    me->check_interval_aging_ = PRP_LIFE_CHECK_INTERVAL;
    prp_memset(me->supervision_address_, 0, PRP_ETH_ADDR_LENGTH);
    me->supervision_seqno_ = 0;
}

