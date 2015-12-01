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
*  15.03.10 | muth     | angepasst Linux version 2.6.31-20-generic
*  30.11.15 | beti     | copied from prp-0
*********************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/mii.h>
#include <linux/crc32.h>
#include <linux/semaphore.h>

#include "../prp/PRP_OsItf_T.h"
#include "../prp/PRP_LogItf_T.h"

/*atomic_t malloc_test = ATOMIC_INIT(0);*/
/********************************************************************/
/*			device operations			    						*/
/********************************************************************/

/********************************************************************/
/*			local vars					    						*/
/********************************************************************/


/********************************************************************/
/*			function prototypes			    						*/
/********************************************************************/


/********************************************************************/
/*			local function				    						*/
/********************************************************************/


/********************************************************************/
/*			device functions			    						*/
/********************************************************************/


uinteger64 prp_time(void)
{
	uinteger64 time;
	struct timeval temp_time;
	
	do_gettimeofday(&temp_time);
	time = ((uinteger64)temp_time.tv_sec * 1000000000) + ((uinteger64)temp_time.tv_usec * 1000);
	
	return(time);
}


uinteger32 prp_htonl(uinteger32 host)
{
	return htonl(host);
}

	
uinteger16 prp_htons(uinteger16 host)
{
	return htons(host);
}

 
uinteger32 prp_ntohl(uinteger32 net)
{
	return ntohl(net);
}

 
uinteger16 prp_ntohs(uinteger16 net)
{
	return ntohs(net);
}

 

integer32 prp_printf(const char *format, ...)
{
	static octet buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	
	return printk(buffer);
}


void* prp_malloc(uinteger32 size)
{
/*
	atomic_inc(&malloc_test);
	PRP_PRP_LOGOUT(5, "mallocs: %i\n", atomic_read(&malloc_test));
*/	
	return kmalloc(size, GFP_ATOMIC);
}

		
void prp_free(void* ptr)
{
/*
	atomic_dec(&malloc_test);
	PRP_PRP_LOGOUT(5, "mallocs: %i\n", atomic_read(&malloc_test));
*/
	
	kfree(ptr);
}

void prp_memcpy(void *dest_ptr, const void *src_ptr, uinteger32 size)
{
	memcpy(dest_ptr, src_ptr, size);
}

integer32 prp_memcmp( const void *left_ptr, const void *right_ptr, uinteger32 size)
{
	return memcmp(left_ptr, right_ptr, size);
}

void prp_memset(void *ptr, octet set, uinteger32 size)
{
	memset(ptr, set, size);
}


struct PRP_SpinLock_T 
{
	spinlock_t lock;
	unsigned long flags;
};


void* create_lock(void)
{
	struct PRP_SpinLock_T* lock;
	
	lock = (struct PRP_SpinLock_T*)kmalloc(sizeof(struct PRP_SpinLock_T), GFP_ATOMIC);
	spin_lock_init(&(lock->lock));
	
	return((void*)lock);
}

void destroy_lock(void* lock)
{
	kfree(lock);
}

void lock_down(void* lock)
{
	struct PRP_SpinLock_T* temp_lock;
	temp_lock = (struct PRP_SpinLock_T*)lock;
	
	if(in_interrupt())
	{
		spin_lock(&(temp_lock->lock));
	}
	else
	{
		spin_lock_irqsave(&(temp_lock->lock), temp_lock->flags);
	}
}

void lock_up(void* lock)
{
	struct PRP_SpinLock_T* temp_lock;
	temp_lock = (struct PRP_SpinLock_T*)lock;
	if(in_interrupt()) /* the dev_hard_start_xmit requires that it gets called in a not irq disabled environment */
	{
		spin_unlock(&(temp_lock->lock));
	}
	else
	{
		spin_unlock_irqrestore(&(temp_lock->lock), temp_lock->flags);
	}
}

