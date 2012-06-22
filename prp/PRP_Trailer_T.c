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
*  13.04.11 | reld     | update to PRP-1
*********************************************************************/

#include "PRP_Trailer_T.h"
#include "PRP_LogItf_T.h"

/**
 * @fn void PRP_Trailer_T_add_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length, PRP_RedundancyControlTrailer_T* trailer) 
 * @brief Adds a Trailer to the frame
 * @param   me PRP_Trailer_T this pointer
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 pointer to the length in bytes of the frame
 * @param   trailer PRP_RedundancyControlTrailer_T pointer to the trailer
 * @retval  !NULL OK
 * @retval  NULL_PTR ERROR (or no trailer)
 *
 * This function adds a Trailer to the frame. Changes the values passed as
 * argument into network byte order and changes the field length etc.
 */
void PRP_Trailer_T_add_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length, PRP_RedundancyControlTrailer_T* trailer) 
{
    uinteger16 size;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    if ((data[12] == 0x81) && (data[13] == 0x00)) {
        /* calculate LSDU_size for VLAN-Tagged frame */
        size = *length - 16 + PRP_RCT_LENGTH;
    }
    else {
        /* calculate LSDU_size for not VLAN-Tagged frame */
        size = *length - 12 + PRP_RCT_LENGTH;
    }

    /* pad such that length would be >=60 bytes
     * (without CRC, without VLAN tag, without PRP trailer) */
    while (size < 60-8) {
        data[(*length)++] = 0;
        size++;
    }

    /* can only be done because frame is in a much bigger buffer */
    data[(*length)++] = trailer->seq_ >> 8;
    data[(*length)++] = trailer->seq_;
    data[(*length)++] = (trailer->lan_id_ << 4) | (size >> 8);
    data[(*length)++] = size;
    data[(*length)++] = 0x88;
    data[(*length)++] = 0xFB;
}

/**
 * @fn PRP_RedundancyControlTrailer_T* PRP_Trailer_T_get_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length) 
 * @brief Extracts the Trailer out the frame
 * @param   me PRP_Trailer_T this pointer
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 pointer to the length in bytes of the frame
 * @retval  redundancy_control_trailer_ OK
 * @retval  NULL_PTR ERROR (or no trailer)
 *
 * This function extracts the trailer out the frame, and fills it into a
 * structure in host byte order. Returns a Pointer to a RCT object if
 * there is a Trailer, else NULL.
 */
PRP_RedundancyControlTrailer_T* PRP_Trailer_T_get_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length)
{
    octet lan_id;
    octet src_mac[6];
    uinteger16 i, size_offset, lsdu_size;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return(NULL_PTR);
    }

    for (i=0; i<PRP_ETH_ADDR_LENGTH; i++) {
        src_mac[i] = data[(6 + i)];
    }

    /* PRP-1 frames are not allowed to be shorter than 60 bytes (without CRC) */
    if (*length < 60) {
        PRP_PRP_LOGOUT(2, "%s\n", "short frame");
        return(NULL_PTR);
    }
    if (data[*length-2] != 0x88 || data[*length-1] != 0xFB) {
        PRP_PRP_LOGOUT(2, "%s\n", "frame without PRP-1 suffix 0x88FB");
        return(NULL_PTR);
    }
    if ((data[12] == 0x81) && (data[13] == 0x00)) {
        PRP_PRP_LOGOUT(2, "%s\n", "vlan tagged frame");
        size_offset = 18;
    } else {
        PRP_PRP_LOGOUT(2, "%s\n", "no vlan tagged frame");
        size_offset = 14;
    }

    /* trailer_offset = 0; */
    lan_id = data[*length-4] >> 4;
    lsdu_size = ((data[*length-4] & 0xF) << 8) | data[*length-3];

    if ((lan_id == 0xA || lan_id == 0xB) && (lsdu_size == *length-size_offset)) {
        PRP_PRP_LOGOUT(2, "frame with PRP-1 trailer, last 6 bytes: %02x %02x %02x %02x %02x %02x\n",
                       data[(*length-6)],data[(*length-5)],data[(*length-4)],
                       data[(*length-3)],data[(*length-2)],data[(*length-1)]);

        me->redundancy_control_trailer_.seq_ = (data[*length-6] << 8) | data[*length-5];
        me->redundancy_control_trailer_.seq_octet_[0] = data[*length-6];
        me->redundancy_control_trailer_.seq_octet_[1] = data[*length-5];
        me->redundancy_control_trailer_.lan_id_ = lan_id;
        return(&(me->redundancy_control_trailer_));
    }
    PRP_USERLOG(user_log.verbose_,"frame with PRP-1 suffix, but no valid PRP-1 trailer, last 6 bytes: %x %x %x %x\n(SRC %02x:%02x:%02x:%02x:%02x:%02x)",
                data[(*length-6)],data[(*length-5)], data[(*length-4)],
                data[(*length-3)], data[(*length-2)], data[(*length-1)],
                src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);

    /* no trailer found */
    return(NULL_PTR);
}

/**
 * @fn void PRP_Trailer_T_remove_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length) 
 * @brief Removes a RCT from a frame, if there is one
 * @param   me PRP_Trailer_T this pointer
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 pointer to the length in bytes of the frame
 */
void PRP_Trailer_T_remove_trailer(PRP_Trailer_T* const me, octet* data, uinteger32* length) 
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (PRP_Trailer_T_get_trailer(me, data, length)) {
        /* just set length field to new value */
        *length -= 6;
        /* we don't do padding here if length goes below 60 bytes */
    } else {
        PRP_PRP_LOGOUT(2, "%s\n", "BUG? remove_trailer was called but there is no trailer.");
    }
}

/**
 * @fn void PRP_Trailer_T_init(PRP_Trailer_T* const me)
 * @brief Initialize the PRP_Trailer interface
 * @param   me PRP_Trailer_T this pointer
 */
void PRP_Trailer_T_init(PRP_Trailer_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    PRP_RedundancyControlTrailer_T_init(&(me->redundancy_control_trailer_));
}

/**
 * @fn void PRP_Trailer_T_cleanup(PRP_Trailer_T* const me)
 * @brief Clean up the PRP_Trailer interface
 * @param   me PRP_Trailer_T this pointer
 */
void PRP_Trailer_T_cleanup(PRP_Trailer_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (me == NULL_PTR) {
        return;
    }

    PRP_RedundancyControlTrailer_T_cleanup(&(me->redundancy_control_trailer_));
}

