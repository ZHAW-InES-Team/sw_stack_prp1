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
*  30.11.15 | beti     | copied from prp-0
*********************************************************************/

#ifndef PRP_Dev_T
#define PRP_Dev_T


#ifndef __KERNEL__

#define PRP_NODETABLE_END				1
#define PRP_NODE_DELETED				2

#endif


#define PRPDEV_IOCTL_MAGIC SIOCDEVPRIVATE

#define PRPDEV_IOCTL_GET_MERGE_LAYER_INFO			PRPDEV_IOCTL_MAGIC+1
#define PRPDEV_IOCTL_SET_MERGE_LAYER_INFO			PRPDEV_IOCTL_MAGIC+2
#define PRPDEV_IOCTL_GET_NODE_TABLE_ENTRY			PRPDEV_IOCTL_MAGIC+3
#define PRPDEV_IOCTL_GO_TO_FIRST_NODE_TABLE_ENTRY	PRPDEV_IOCTL_MAGIC+4
#define PRPDEV_IOCTL_GO_TO_NEXT_NODE_TABLE_ENTRY	PRPDEV_IOCTL_MAGIC+5

#endif

