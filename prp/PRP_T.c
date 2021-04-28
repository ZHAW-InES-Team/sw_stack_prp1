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
*  17.12.07 | mesv     | file created, this is the API
************|**********|*********************************************
*  14.01.08 | mesv     | added some comments
*********************************************************************
*  30.11.15 | beti     | added new statistic values
*********************************************************************/

#include "PRP_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_NetItf_T.h"
#include "PRP_OsItf_T.h"
#include "PRP_Environment_T.h"
#include "PRP_Lock_T.h"

/* All API functions are using this lock to have atomic access */
static PRP_Environment_T environment_;
/* Instance of the protocol engine */
static PRP_Lock_T lock_;
/* Indicates if the stack is initialized or not */
static boolean initialized_ = FALSE;


/**
 * @fn integer32 PRP_T_receive(octet* data, uinteger32* length, octet lan_id)
 * @brief Receive function for all incoming frames of the PRP network adapters
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 length in bytes of the frame
 * @param   lan_id octet on which LAN it was received
 * @retval  1 integer32 DROP
 * @retval  0 integer32 KEEP
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_T_receive(octet* data, uinteger32* length, octet lan_id)
{
    integer32 ret = PRP_KEEP;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (initialized_ != TRUE) {
        return(-PRP_ERROR_NOT_INITIALIZED);
    }

    PRP_Lock_T_down(&lock_); /* API calls are mutex */
    ret = PRP_Environment_T_process_rx(&environment_, data, length, lan_id);
    PRP_Lock_T_up(&lock_);

    return(ret);
}

/**
 * @fn integer32 PRP_T_transmit(octet* data, uinteger32* length)
 * @brief Transmit function for all frames of the PRP network adapters
 * @param   data octet pointer to the beginning of the frame (dest mac)
 * @param   length uinteger32 length in bytes of the frame
 * @retval  0 integer32 OK
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_T_transmit(octet* data, uinteger32* length)
{
    integer32 ret = 0;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (initialized_ != TRUE) {
        return(-PRP_ERROR_NOT_INITIALIZED);
    }

    /* API calls are mutex */
    PRP_Lock_T_down(&lock_);
    ret = PRP_Environment_T_process_tx(&environment_, data, length, 0x00);
    PRP_Lock_T_up(&lock_);

    return(ret);
}

/**
 * @fn void PRP_T_timer(void)
 * @brief Timer (1s) for all periodical jobs
 */
void PRP_T_timer(void)
{
    if (initialized_ != TRUE) {
        return;
    }

    PRP_Lock_T_down(&lock_); /* API calls are mutex */
    PRP_Environment_T_process_timer(&environment_);
    PRP_Lock_T_up(&lock_);
}

/**
 * @fn void PRP_T_link_down_A(void)
 * @brief Function to set link PRP A down
 */
void PRP_T_link_down_A(void)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (initialized_ != TRUE) {
        return;
    }

    PRP_Lock_T_down(&lock_); /* API calls are mutex */
    /* not implemented */
    PRP_Lock_T_up(&lock_);
}

/**
 * @fn void PRP_T_link_down_B(void)
 * @brief Function to set link PRP B down
 */
void PRP_T_link_down_B(void)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (initialized_ != TRUE) {
        return;
    }

    PRP_Lock_T_down(&lock_); /* API calls are mutex */
    /* not implemented */
    PRP_Lock_T_up(&lock_);
}

/**
 * @fn integer32 PRP_T_get_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer)
 * @brief Copies the Merge Layer information to the object passed as argument.
 * @param   merge_layer PRP_MergeLayerInfo_T pointer to a Merge Layer Info object where the info shall be copied to
 * @retval  0 integer32 OK
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_T_get_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (initialized_ != TRUE) {
        return(-PRP_ERROR_NOT_INITIALIZED);
    }

    if (merge_layer == NULL_PTR) {
        return(-PRP_ERROR_NULL_PTR);
    }

    PRP_Lock_T_down(&lock_); /* API calls are mutex */

    prp_memcpy(merge_layer->node_, environment_.environment_configuration_.node_, PRP_NODE_NAME_LENGTH);
    prp_memcpy(merge_layer->manufacturer_, environment_.environment_configuration_.manufacturer_, PRP_MANUFACTURER_NAME_LENGTH);
    prp_memcpy(merge_layer->version_, environment_.environment_configuration_.version_, PRP_VERSION_LENGTH);
    prp_memcpy(merge_layer->mac_address_A_, environment_.environment_configuration_.mac_address_A_, PRP_ETH_ADDR_LENGTH);
    prp_memcpy(merge_layer->mac_address_B_, environment_.environment_configuration_.mac_address_B_, PRP_ETH_ADDR_LENGTH);
    merge_layer->adapter_active_A_ = environment_.environment_configuration_.adapter_active_A_;
    merge_layer->adapter_active_B_ = environment_.environment_configuration_.adapter_active_B_;
    merge_layer->duplicate_discard_ = environment_.environment_configuration_.duplicate_discard_;
    merge_layer->transparent_reception_ = environment_.environment_configuration_.transparent_reception_;
    prp_memcpy(merge_layer->supervision_address_, environment_.supervision_.supervision_address_, PRP_ETH_ADDR_LENGTH);
    merge_layer->life_check_interval_ = environment_.supervision_.life_check_interval_;
    merge_layer->link_time_out_ = environment_.supervision_.link_time_out_;
    merge_layer->cnt_total_sent_A_ = environment_.environment_configuration_.cnt_total_sent_A_;
    merge_layer->cnt_total_sent_B_ = environment_.environment_configuration_.cnt_total_sent_B_;
    merge_layer->cnt_total_received_A_ = environment_.environment_configuration_.cnt_total_received_A_;
    merge_layer->cnt_total_received_B_ = environment_.environment_configuration_.cnt_total_received_B_;
    merge_layer->cnt_total_errors_A_ = environment_.environment_configuration_.cnt_total_errors_A_;
    merge_layer->cnt_total_errors_B_ = environment_.environment_configuration_.cnt_total_errors_B_;
    merge_layer->cnt_total_missing_duplicates_ = environment_.environment_configuration_.cnt_total_missing_duplicates_;
    merge_layer->max_duplicate_delay_ = environment_.environment_configuration_.max_duplicate_delay_;

    PRP_Lock_T_up(&lock_);

    return(0);
}

/**
 * @fn integer32 PRP_T_set_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer)
 * @brief Sets the Merge Layer info parameters to the values passed as argument.
 * @param   merge_layer PRP_MergeLayerInfo_T pointer to a Merge Layer Info object where the info shall be copied from
 * @retval  0 integer32 OK
 * @retval  <0 integer32 ERROR (code)
 */
integer32 PRP_T_set_merge_layer_info(PRP_MergeLayerInfo_T* merge_layer)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if (initialized_ != TRUE) {
        return(-PRP_ERROR_NOT_INITIALIZED);
    }

    if (merge_layer == NULL_PTR) {
        return(-PRP_ERROR_NULL_PTR);
    }

    PRP_Lock_T_down(&lock_); /* API calls are mutex */

    prp_memcpy(environment_.environment_configuration_.node_, merge_layer->node_, PRP_NODE_NAME_LENGTH);
    prp_memcpy(environment_.environment_configuration_.manufacturer_, merge_layer->manufacturer_, PRP_MANUFACTURER_NAME_LENGTH);
    prp_memcpy(environment_.environment_configuration_.version_, merge_layer->version_, PRP_VERSION_LENGTH);

    /* did mac address of adapter A change ? */
    if (0 != prp_memcmp(environment_.environment_configuration_.mac_address_A_, merge_layer->mac_address_A_, PRP_ETH_ADDR_LENGTH)) {
        /* change the mac on the card */
        if (0 != PRP_NetItf_T_set_mac_address_A(merge_layer->mac_address_A_)) {
            PRP_Lock_T_up(&lock_);
            return(-PRP_ERROR_ADAPTER);
        }
        prp_memcpy(environment_.environment_configuration_.mac_address_A_, merge_layer->mac_address_A_, PRP_ETH_ADDR_LENGTH);
    }
    if (0 != prp_memcmp(environment_.environment_configuration_.mac_address_B_, merge_layer->mac_address_B_, PRP_ETH_ADDR_LENGTH)) {
        if (0 != PRP_NetItf_T_set_mac_address_B(merge_layer->mac_address_B_)) {
            PRP_Lock_T_up(&lock_);
            return(-PRP_ERROR_ADAPTER);
        }
        prp_memcpy(environment_.environment_configuration_.mac_address_B_, merge_layer->mac_address_B_, PRP_ETH_ADDR_LENGTH);
    }

    if ((merge_layer->adapter_active_A_ == TRUE) || (merge_layer->adapter_active_A_ == FALSE)) {
        if(0 != PRP_NetItf_T_set_active_A(merge_layer->adapter_active_A_)) {
            PRP_Lock_T_up(&lock_);
            return(-PRP_ERROR_ADAPTER);
        }
        environment_.environment_configuration_.adapter_active_A_ = merge_layer->adapter_active_A_;
    } else {
        PRP_Lock_T_up(&lock_);
        return(-PRP_ERROR_WRONG_VAL);
    }

    if ((merge_layer->adapter_active_B_ == TRUE) || (merge_layer->adapter_active_B_ == FALSE)) {
        if (0 != PRP_NetItf_T_set_active_B(merge_layer->adapter_active_B_)) {
            PRP_Lock_T_up(&lock_);
            return(-PRP_ERROR_ADAPTER);
        }
        environment_.environment_configuration_.adapter_active_B_ = merge_layer->adapter_active_B_;
    } else {
        PRP_Lock_T_up(&lock_);
        return(-PRP_ERROR_WRONG_VAL);
    }

    if ((merge_layer->duplicate_discard_ == TRUE) || (merge_layer->duplicate_discard_ == FALSE)) {
        environment_.environment_configuration_.duplicate_discard_ = merge_layer->duplicate_discard_;
    } else {
        PRP_Lock_T_up(&lock_);
        return(-PRP_ERROR_WRONG_VAL);
    }

    if ((merge_layer->duplicate_discard_ == TRUE) || (merge_layer->duplicate_discard_ == FALSE)) {
        environment_.environment_configuration_.transparent_reception_ = merge_layer->transparent_reception_;
    } else {
        PRP_Lock_T_up(&lock_);
        return(-PRP_ERROR_WRONG_VAL);
    }

    /* environment_.node_table_.node_table_empty_ = merge_layer->node_table_empty_; read only */
    if (0 != prp_memcmp(environment_.supervision_.supervision_address_, merge_layer->supervision_address_, PRP_ETH_ADDR_LENGTH)) {
        if ((merge_layer->supervision_address_[0] != 0x01) ||
            (merge_layer->supervision_address_[1] != 0x15) ||
            (merge_layer->supervision_address_[2] != 0x4e) ||
            (merge_layer->supervision_address_[3] != 0x00) ||
            (merge_layer->supervision_address_[4] != 0x01)) { /* is it a valid supervision frame address? */
            PRP_Lock_T_up(&lock_);
            return(-PRP_ERROR_WRONG_VAL);
        }
        /* add the address to the multicast addresses of the adapters */
        if (0 != PRP_NetItf_T_set_supervision_address(merge_layer->supervision_address_)) {
            PRP_Lock_T_up(&lock_);
            return(-PRP_ERROR_ADAPTER);
        }
        prp_memcpy(environment_.supervision_.supervision_address_, merge_layer->supervision_address_, PRP_ETH_ADDR_LENGTH);
    }
    environment_.supervision_.life_check_interval_ = merge_layer->life_check_interval_;
    environment_.supervision_.link_time_out_ = merge_layer->link_time_out_;

    PRP_Lock_T_up(&lock_);

    return(0);
}

/**
 * @fn void PRP_T_init(void)
 * @brief Initialize the PRP interface
 */
void PRP_T_init(void)
{
    if (initialized_ != FALSE) {
        return;
    }
    PRP_Lock_T_init(&lock_); /* API calls are mutex */
    PRP_Environment_T_init(&environment_);
    initialized_ = TRUE;
}

/**
 * @fn void PRP_T_cleanup(void)
 * @brief Clean up the PRP interface
 */
void PRP_T_cleanup(void)
{
    if (initialized_ != TRUE) {
        return;
    }
    PRP_Lock_T_down(&lock_); /* API calls are mutex */
    initialized_ = FALSE;
    PRP_Lock_T_cleanup(&lock_);

    PRP_Environment_T_cleanup(&environment_);
}

