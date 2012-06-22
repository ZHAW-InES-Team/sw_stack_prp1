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
*********************************************************************/

#ifndef PRP_Package_T_H
#define PRP_Package_T_H

#include "PRP_DataTypes_T.h"

#define PRP_TIMER_TICK_INTERVAL         20      /* in ms */
#define PRP_TIMER_STATISTIC_INTERVAL    1000    /* in ms */

#define PRP_RCT_LENGTH                  4
#define PRP_MTU                         1496

#define PRP_ID_LAN_A                    0xa
#define PRP_ID_LAN_B                    0xb

#define PRP_DROP                        2
#define PRP_KEEP                        0

#define PRP_ERROR_NULL_PTR              1
#define PRP_ERROR_WRONG_VAL             2
#define PRP_ERROR_NOT_SUPPORTED         3
#define PRP_ERROR_ADAPTER               4
#define PRP_ERROR_FRAME_COR             5
#define PRP_ERROR_NOT_INITIALIZED       6

#define PRP_ETH_ADDR_LENGTH             6
#define PRP_NODE_NAME_LENGTH            32
#define PRP_MANUFACTURER_NAME_LENGTH    256
#define PRP_VERSION_LENGTH              32

#define PRP_LIFE_CHECK_INTERVAL         2000    /* in ms */
#define PRP_CHECK_INTERVAL_AGING        20      /* in ms */

#define PRP_SECOND_IN_NANOSECONDS       1000000000
#define PRP_MILLISECOND_IN_NANOSECONDS  1000000


struct PRP_Lock_T;
typedef struct PRP_Lock_T PRP_Lock_T;

struct PRP_Timer_T;
typedef struct PRP_Timer_T PRP_Timer_T;

struct PRP_Trailer_T;
typedef struct PRP_Trailer_T PRP_Trailer_T;

struct PRP_Supervision_T;
typedef struct PRP_Supervision_T PRP_Supervision_T;

struct PRP_RedundancyControlTrailer_T;
typedef struct PRP_RedundancyControlTrailer_T PRP_RedundancyControlTrailer_T;

struct PRP_Frames_T;
typedef struct PRP_Frames_T PRP_Frames_T;

struct PRP_FrameAnalyser_T;
typedef struct PRP_FrameAnalyser_T PRP_FrameAnalyser_T;

struct PRP_EnvironmentConfiguration_T;
typedef struct PRP_EnvironmentConfiguration_T PRP_EnvironmentConfiguration_T;

struct PRP_Environment_T;
typedef struct PRP_Environment_T PRP_Environment_T;

struct PRP_DiscardAlgorithm_T;
typedef struct PRP_DiscardAlgorithm_T PRP_DiscardAlgorithm_T;

#endif /* PRP_Package_T_H */

