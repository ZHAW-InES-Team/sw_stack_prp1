/********************************************************************
*
*  Copyright (c) 2008, Institute of Embedded Systems at 
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
*  Author      : Sven Meier/Patrick Staehlin
*
*********************************************************************
*  Change History
*
*  Date     | Name     | Modification
************|**********|*********************************************
*  01.01.08 | mesv     | file created
************|**********|*********************************************
*  05.02.08 | stpa     | support for printf
*********************************************************************/

#include <semaphore.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdarg.h>

#include "../prp/PRP_OsItf_T.h"
#include "../prp/PRP_LogItf_T.h"


/**
 * @fn uinteger64 prp_time(void)
 * @brief Get the system time in nano seconds
 *
 * @retval  uinteger64 >0 : OK
 * @retval  uinteger64 <0 : ERROR
 */
uinteger64 prp_time(void)
{
    uinteger64 time;
    struct timeval temp_time;

    gettimeofday(&temp_time, NULL);
    time =  ((uinteger64)temp_time.tv_sec * 1000000000) +
            ((uinteger64)temp_time.tv_usec * 1000);

    return(time);
}

/**
 * @fn int prp_printf(const char *format, ...)
 * @brief Prints to the output of the Kernel
 *
 * @param   format const char pointer to the string
 * @retval  integer32 >=0 : OK (nr of bytes written)
 * @retval  integer32 <0 : ERROR (code)
 */
int prp_printf(const char *format, ...)
{
    static char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), (const char*)format, args);
    va_end(args);

    return printf("%s",buffer);
}

/**
 * @fn void* prp_malloc(uinteger32 size)
 * @brief Allocates memory and returns a pointer to the memory region
 *
 * @param   size uinteger32 size of memory to allocate
 * @retval  void ptr !NULL : OK (pointer to the memory)
 * @retval  void ptr NULL : ERROR
 */
void* prp_malloc(uinteger32 size)
{
    return malloc(size);
}

/**
 * @fn void prp_free(void* ptr)
 * @brief Frees the dynamicly allocated memory.
 *
 * @param   ptr void pointer to the memory region
 */
void prp_free(void* ptr)
{
    free(ptr);
}

/**
 * @fn void prp_memcpy(void *dest_ptr, const void *src_ptr, uinteger32 size)
 * @brief Copies size bytes from src_ptr to dest_ptr.
 *
 * @param   dest_ptr void pointer to the destination
 * @param   src_ptr void pointer to the source
 * @param   size uinteger32 number of bytes to copy
 */
void prp_memcpy(void *dest_ptr, const void *src_ptr, uinteger32 size)
{
    memcpy(dest_ptr, src_ptr, size);
}

/**
 * @fn integer32 prp_memcmp( const void *left_ptr, const void *right_ptr, uinteger32 size)
 * @brief Compares size bytes from left_ptr with right_ptr.
 *
 * @param   left_ptr left void pointer
 * @param   right_ptr right void pointer
 * @param   size uinteger32 number of bytes to copy
 * @retval  integer32 >0 bigger
 * @retval  integer32 =0 equal
 * @retval  integer32 <0 less
 */
integer32 prp_memcmp( const void *left_ptr, const void *right_ptr, uinteger32 size)
{
    return memcmp(left_ptr, right_ptr, size);
}

/**
 * @fn void prp_memset(void *ptr, octet set, uinteger32 size)
 * @brief Set size values to the set value.
 *
 * @param   ptr void pointer to the destination
 * @param   set octet to set
 * @param   size uinteger32 number of bytes to set
 */
void prp_memset(void *ptr, octet set, uinteger32 size)
{
    memset(ptr, set, size);
}


#ifdef SEMAPHORES
struct PRP_Semaphore_T 
{
    sem_t handle_;
};

/**
 * @fn void* create_lock(void)
 * @brief Creates a semaphore structure
 */
void* create_lock(void)
{
    struct PRP_Semaphore_T* lock;

    lock = (struct PRP_Semaphore_T*)malloc(sizeof(struct PRP_Semaphore_T));
    sem_init(&lock->handle_, 0, 1);

    return((void*)lock);
}

/**
 * @fn void destroy_lock(void* lock)
 * @brief Destroys the semaphore structure
 *
 * @param   lock void pointer to the semaphore
 */
void destroy_lock(void* lock)
{
    struct PRP_Semaphore_T* temp_lock;
    temp_lock = (struct PRP_Semaphore_T*)lock;

    sem_destroy( &temp_lock->handle_ );
    free(temp_lock);
}

/**
 * @fn void lock_down(void* lock)
 * @brief Blocks on the semaphore if in use
 *
 * @param   lock void pointer to the semaphore
 * @retval  integer32 >=0 : OK (nr of bytes written)
 * @retval  integer32 <0 : ERROR (code)
 */
void lock_down(void* lock)
{
    struct PRP_Semaphore_T* temp_lock;
    temp_lock = (struct PRP_Semaphore_T*)lock;

    sem_wait( &temp_lock->handle_ );
}

/**
 * @fn void lock_up(void* lock)
 * @brief Unlocks the semaphore
 *
 * @param   lock void pointer to the semaphore
 * @retval  integer32 >=0 : OK (nr of bytes written)
 * @retval  integer32 <0 : ERROR (code)
 */
void lock_up(void* lock)
{
    struct PRP_Semaphore_T* temp_lock;
    temp_lock = (struct PRP_Semaphore_T*)lock;

    if (sem_post( &temp_lock->handle_ ) != 0 ) {
        printf(" Semaphore reached max Value, semaphore count is unchanged\n");
    }
}

#else /* if not def semaphore */
void* create_lock(void) { return (void *) 1; }
void destroy_lock(void* lock) { }
void lock_down(void* lock) { }
void lock_up(void* lock) { }
#endif

