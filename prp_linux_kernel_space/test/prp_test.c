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
*  30.11.15 | beti     | file copied from prp-0 and adapted for prp-1
*********************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>    
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/ioctl.h>	/* for ioctl's*/
#include <linux/if.h>
#include <semaphore.h>

#include "../prp/PRP_T.h"
#include "../PRP_Dev_T.h"


int update = 100;
sem_t sem;

int main(int argc, char* argv[])
{
	int verbosity = 0;
	int i;
	PRP_MergeLayerInfo_T merge_layer_info;

	int fd 		= 0;
	struct ifreq ifr;

	/* read verbosity argument */
	if(argc > 1){
		if(sscanf(argv[1], "%i", &verbosity) !=1){
			verbosity = 0;
		}
	}

	/** open socket, necessary to get a file descriptor */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	strcpy(ifr.ifr_name, "prp0");	

    if( sem_init(&sem, 0, 1) != 0)
	{
       	printf("Can't create semaphore, abort\n");
		exit(-1);
	}    
		
	system("clear");
	while(TRUE)
	{
        sem_wait(&sem);
		system("clear");
        
		ifr.ifr_data = &merge_layer_info;
		if(0 > ioctl(fd, PRPDEV_IOCTL_GET_MERGE_LAYER_INFO, &ifr))
		{
			return(-1);
		}
		printf("MERGE_LAYER_INFO==========================================================\n");		
		printf("node_:\t\t\t\t\t%s\n", merge_layer_info.node_);
		printf("manufacturer_:\t\t\t\t%s\n", merge_layer_info.manufacturer_);
		printf("version_:\t\t\t\t%s\n", merge_layer_info.version_);
		
		if(verbosity >= 1)
		{
			printf("%s\t\t\t\t", "mac_address_A_:");
			for(i=0; i<6; i++)
			{
				printf("%x ", merge_layer_info.mac_address_A_[i]);
				
			}
			printf("\n");
			printf("%s\t\t\t\t", "mac_address_B_:");
			for(i=0; i<6; i++)
			{
				printf("%x ", merge_layer_info.mac_address_B_[i]);
				
			}
			printf("\n");
		}
		if(verbosity >= 2)
		{
			
			printf("adapter_active_A_:\t\t\t%s\n", merge_layer_info.adapter_active_A_ ? "TRUE" : "FALSE");
			printf("adapter_active_B_:\t\t\t%s\n", merge_layer_info.adapter_active_B_ ? "TRUE" : "FALSE");
			printf("duplicate_discard_:\t\t\t%s\n", merge_layer_info.duplicate_discard_ ? "TRUE" : "FALSE");
			printf("transparent_reception_:\t\t\t%s\n", merge_layer_info.transparent_reception_ ? "TRUE" : "FALSE");
			printf("%s\t\t\t", "supervision_address_:");
			for(i=0; i<6; i++)
			{
				printf("%x ", merge_layer_info.supervision_address_[i]);
				
			}
			printf("\n");		
			printf("life_check_interval_:\t\t\t%u\n", merge_layer_info.life_check_interval_);
			printf("link_time_out_:\t\t\t\t%u\n", merge_layer_info.link_time_out_);
			printf("cnt_total_sent_A_:\t\t\t%u\n", merge_layer_info.cnt_total_sent_A_);
			printf("cnt_total_sent_B_:\t\t\t%u\n", merge_layer_info.cnt_total_sent_B_);
			printf("cnt_total_received_A_:\t\t\t%u\n", merge_layer_info.cnt_total_received_A_);
			printf("cnt_total_received_B_:\t\t\t%u\n", merge_layer_info.cnt_total_received_B_);
			printf("cnt_total_errors_A_:\t\t\t%u\n", merge_layer_info.cnt_total_errors_A_);
			printf("cnt_total_errors_B_:\t\t\t%u\n", merge_layer_info.cnt_total_errors_B_);
			printf("cnt_total_missing_duplicates_:\t\t%u\n", merge_layer_info.cnt_total_missing_duplicates_);
			printf("max_duplicate_delay_:\t\t\t%ums\n", merge_layer_info.max_duplicate_delay_);
		}
		printf("==========================================================================\n");		

		sem_post(&sem);				
		usleep((update*10000));
	}

   	sem_destroy(&sem);
	
	close(fd);
	
	return(0);
}



