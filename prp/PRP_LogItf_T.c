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
*********************************************************************
*  14.05.12 | asdo     | prp-1 modification
*********************************************************************
*  21.06.12 | walh     | logging implemented
*********************************************************************
*  30.11.15 | beti     | adopted for Linux version 4.2
*********************************************************************/

#include "PRP_LogItf_T.h"
#include "PRP_Environment_T.h"

/* pointer to the prp environment */
static PRP_Environment_T* environment_;

#ifdef PRP_DEBUG_LOG
    integer32 debug_level           = 0;
    integer32 net_itf_debug_level   = 0;
    integer32 prp_debug_level       = 0;
    integer32 discard_debug_level   = 0;
#endif

/* global struct for user interface */
struct PTP_LogItf_T user_log;

/**
 * @fn void PRP_LogItf_T_print_config(void)
 * @brief Help about implemented functions
 */
void PRP_LogItf_T_print_config(void)
{
    PRP_EnvironmentConfiguration_T_print(&(environment_->environment_configuration_), 0);
}

/**
 * @fn void PRP_LogItf_T_print_counters(void)
 * @brief Function to print the statistic counters
 */
void PRP_LogItf_T_print_counters(void)
{
    uinteger32 tx_diff;
    uinteger32 rx_diff;
    if (environment_->environment_configuration_.cnt_total_sent_A_>
        environment_->environment_configuration_.cnt_total_sent_B_
    ) {
        tx_diff = environment_->environment_configuration_.cnt_total_sent_A_
        - environment_->environment_configuration_.cnt_total_sent_B_;
    } else {
        tx_diff = environment_->environment_configuration_.cnt_total_sent_B_
        - environment_->environment_configuration_.cnt_total_sent_A_;
    }
    if (environment_->environment_configuration_.cnt_total_received_A_>
        environment_->environment_configuration_.cnt_total_received_B_
    ) {
        rx_diff = environment_->environment_configuration_.cnt_total_received_A_
                - environment_->environment_configuration_.cnt_total_received_B_;
    } else {
        rx_diff = environment_->environment_configuration_.cnt_total_received_B_
                - environment_->environment_configuration_.cnt_total_received_A_;
    }

    PRP_PRINTF( "%s\n", "======== Statistic counters ===========================");
    PRP_PRINTF( "cnt_total_sent_A_:\t%u\n", environment_->environment_configuration_.cnt_total_sent_A_);
    PRP_PRINTF( "cnt_total_sent_B_:\t%u\n", environment_->environment_configuration_.cnt_total_sent_B_);
    PRP_PRINTF( "cnt_tx_difference:\t%u\n", tx_diff);
    PRP_PRINTF( "cnt_total_received_A_:\t%u\n", environment_->environment_configuration_.cnt_total_received_A_);
    PRP_PRINTF( "cnt_total_received_B_:\t%u\n", environment_->environment_configuration_.cnt_total_received_B_);
    PRP_PRINTF( "cnt_rx_difference:\t%u\n", rx_diff);
    PRP_PRINTF( "cnt_total_errors_A_:\t%u\n", environment_->environment_configuration_.cnt_total_errors_A_);
    PRP_PRINTF( "cnt_total_errors_B_:\t%u\n", environment_->environment_configuration_.cnt_total_errors_B_);
    PRP_PRINTF( "%s\n", "=======================================================");
}

/**
 * @fn void PRP_LogItf_T_reset(void)
 * @brief Reset all log settings
 */
void PRP_LogItf_T_reset(void)
{
    if (user_log.counter_) {
        user_log.counter_       = FALSE;
        PRP_PRINTF("%s\n","==== Statistic counter logging disabled");
    }
    if (user_log.consistency_) {
        user_log.consistency_   = FALSE;
        PRP_PRINTF("%s\n","==== Discard consistency check disabled");
    }
    if (user_log.discard_) {
        user_log.discard_       = FALSE;
        PRP_PRINTF("%s\n","==== Discard list logging disabled");
    }
    if (user_log.frame_) {
        user_log.frame_         = FALSE;
        PRP_PRINTF("%s\n","==== RX frame content logging disabled");
    }
    if (user_log.sf_) {
        user_log.sf_            = FALSE;
        PRP_PRINTF("%s\n","====  RX supervision status logging disabled");
    }
    if (user_log.trailer_) {
        user_log.trailer_       = FALSE;
        PRP_PRINTF("%s\n","====  RTC logging disabled");
    }
    if (user_log.verbose_) {
        user_log.verbose_       = FALSE;
        PRP_PRINTF("%s\n","====  Verbose log disabled");
    }
}

/**
 * @fn void PRP_LogItf_T_init(PRP_Environment_T* const environment)
 * @brief Initialize pointer to environment and reset status flags
 * @param   environment PRP_Environment_T pointer to the environment
 */
void PRP_LogItf_T_init(PRP_Environment_T* const environment)
{
    environment_        = environment;
    user_log.counter_       = FALSE;
    user_log.consistency_   = FALSE;
    user_log.discard_       = FALSE;
    user_log.frame_         = FALSE;
    user_log.sf_            = FALSE;
    user_log.trailer_       = FALSE;
    user_log.verbose_       = FALSE;
}













    


    
