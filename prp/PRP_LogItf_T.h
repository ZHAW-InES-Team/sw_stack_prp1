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

#ifndef PRP_LogItf_T_H 
#define PRP_LogItf_T_H 

#include "PRP_Package_T.h"
#include "PRP_OsItf_T.h"

#define DRV_NAME "prp_dev"
//#define PRP_DEBUG_LOG

/* log a line to kernel log */
#ifdef PRP_DEBUG_LOG_SRC
    /* add source file name and line number */
    #define PRP_LOG(format, args...) \
            prp_printf("["DRV_NAME"]: " __FILE__ ":%d: " format, __LINE__, args)
#else
    #define PRP_LOG(format, args...) prp_printf("["DRV_NAME"]: " format, args)
#endif


/* info and error log macros */
#define PRP_INFOOUT(format, args...) PRP_LOG("<INFO> "format, args)
#define PRP_ERROUT(format, args...) PRP_LOG("<ERROR> " format, args)

/* general module debug macros */
#ifdef PRP_DEBUG_LOG
    extern int debug_level;
    #define PRP_LOGOUT(level, format, args...) if (debug_level >= level) \
            PRP_LOG("<LOG>" format, args)
#else
    #define PRP_LOGOUT(level, format, args...)
#endif

/* prp debug macros */
#ifdef PRP_DEBUG_LOG
    extern int prp_debug_level;
    #define PRP_PRP_LOGOUT(level, format, args...) if (prp_debug_level >= level) \
            PRP_LOG("<PRP_LOG> " format, args)
#else
    #define PRP_PRP_LOGOUT(level, format, args...)
#endif

/* netdevice debug macros */
#ifdef PRP_DEBUG_LOG
    extern int net_itf_debug_level;
    #define PRP_NET_ITF_LOGOUT(level, format, args...) if (net_itf_debug_level >= level) \
            PRP_LOG("<NET_ITF_LOG> " format, args)
#else
    #define PRP_NET_ITF_LOGOUT(level, format, args...)
#endif


#endif
