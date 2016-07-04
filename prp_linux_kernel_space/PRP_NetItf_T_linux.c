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
*  30.11.15 | beti     | adopted for Linux version 4.2 and prp-1
*********************************************************************/

#define __NO_VERSION__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/mii.h>
#include <linux/crc32.h>
#include <linux/version.h>

#include "PRP_Dev_T.h"
#include "../prp/PRP_T.h"
#include "../prp/PRP_LogItf_T.h"
#include "../prp/PRP_NetItf_T.h"

#define NET_RX_BAD -2 /* packet dropped due to kernel error */

/********************************************************************/
/*			device operations			    						*/
/********************************************************************/
static void PRP_Dev_T_init(struct net_device *dev);
static int __init PRP_Dev_T_init_module(void);
static void __exit PRP_Dev_T_exit_module(void);
static int PRP_Dev_T_open(struct net_device *dev);
static int PRP_Dev_T_stop(struct net_device *dev);
static int PRP_Dev_T_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);
static int PRP_Dev_T_receive(struct sk_buff *skb, struct net_device *dev, struct packet_type* ptype, struct net_device *orig_dev);
static int PRP_Dev_T_do_ioctl(struct net_device* dev, struct ifreq* req, int cmd);
static int PRP_Dev_T_set_mac_address(struct net_device* dev, void *addr);
static struct net_device_stats* PRP_Dev_T_get_stats(struct net_device *dev);
static void PRP_Dev_T_set_multicast_list(struct net_device *dev);
static int PRP_Dev_T_change_mtu(struct net_device *dev, int new_mtu);
static void PRP_Dev_T_timer(unsigned long data);


/********************************************************************/
/*			local vars and functions					    		*/
/********************************************************************/
static char dev_name_A[IFNAMSIZ];
static char dev_name_B[IFNAMSIZ];
static int dev_promisc = 0;

struct net_device *prp_dev;

struct prp_dev_priv
{
	struct net_device *dev_A;
	struct net_device *dev_B;
	struct sockaddr addr_A;
	struct sockaddr addr_B;
	struct sockaddr addr_supervision_multi;
	struct sockaddr addr_bridging_multi;
	u8 open_A;
	u8 open_B;
	u8 promisc_A;
	u8 promisc_B;
	int link_A;
	int link_B;
	struct timer_list alive_timer;
	struct net_device_stats stats;
};

static struct packet_type prp_packet_type;

/* dummy hook to fix a kernel "bug"/"feature" */
static struct packet_type dummy_packet_type;
/* dummy function to fix a kernel "bug"/"feature" */
static int PRP_Dev_T_receive_dummy(struct sk_buff *skb, struct net_device *dev, struct packet_type* ptype, struct net_device *orig_dev);

/********************************************************************/
/*			modul parameters			    						*/
/********************************************************************/

module_param_string(dev_name_A, dev_name_A, sizeof(dev_name_A), 0);
module_param_string(dev_name_B, dev_name_B, sizeof(dev_name_B), 0);
module_param(dev_promisc, int, 0);

MODULE_PARM_DESC(dev_name_A, "Name of the interface that will be prp interface A");
MODULE_PARM_DESC(dev_name_B, "Name of the interface that will be prp interface B");
MODULE_PARM_DESC(dev_promisc, "If set to a value, the cards will be set in promiscous mode");


#ifdef PRP_DEBUG_LOG
	extern int debug_level;
	extern int net_itf_debug_level;
	extern int prp_debug_level;

	module_param(debug_level, int, 0);
	module_param(net_itf_debug_level, int, 0);
	module_param(prp_debug_level, int, 0);
	
	MODULE_PARM_DESC(debug_level, DRV_NAME "general debug log level");
	MODULE_PARM_DESC(net_debug_level, DRV_NAME "Ethernet debug log level");
	MODULE_PARM_DESC(prp_debug_level, DRV_NAME "PRP debug log level");
	
#endif
/********************************************************************/
/*			function prototypes			    						*/
/********************************************************************/
/************************************************************/
/*       PRP_NetItf_T_set_active_A                          */
/************************************************************/
integer32 PRP_NetItf_T_set_active_A(boolean value)
{
	struct prp_dev_priv *dev_priv;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
			
	dev_priv = netdev_priv(prp_dev);
	
	netif_tx_disable(prp_dev);
	
/* TODO locks down the system     	
	if(value == TRUE)
	{
		if(dev_priv->open_A == 0)
		{
			dev_open(dev_priv->dev_A);   
			dev_priv->open_A = 1;
		}
	}
	else
	{
		if(dev_priv->open_A == 1)
		{
 			dev_close(dev_priv->dev_A);   
			dev_priv->open_A = 0;
		}
	} */
	
	netif_wake_queue(prp_dev);
	
	return(0);
}

/************************************************************/
/*       PRP_NetItf_T_set_active_B                          */
/************************************************************/
integer32 PRP_NetItf_T_set_active_B(boolean value)
{
	struct prp_dev_priv *dev_priv;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
			
	dev_priv = netdev_priv(prp_dev);
	
	netif_tx_disable(prp_dev);
	
/* TODO locks down the system     		
	if(value == TRUE)
	{
		if(dev_priv->open_B == 0)
		{
 			dev_open(dev_priv->dev_B);   
			dev_priv->open_B = 1;	
		}
	}
	else
	{
		if(dev_priv->open_B == 1)
		{
 			dev_close(dev_priv->dev_B);   
			dev_priv->open_B = 0;	
		}
	} */
	
	netif_wake_queue(prp_dev);
	
	return(0);
}

/************************************************************/
/*       PRP_NetItf_T_set_mac_address_A                     */
/************************************************************/
integer32 PRP_NetItf_T_set_mac_address_A(octet* mac)
{
	struct prp_dev_priv *dev_priv;
	struct sockaddr new_addr;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(mac == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
			
	dev_priv = netdev_priv(prp_dev);
	
	memcpy(new_addr.sa_data, mac, dev_priv->dev_A->addr_len);
	new_addr.sa_family = dev_priv->dev_A->type;
		
	if(is_valid_ether_addr(new_addr.sa_data) == 0)
    {
		PRP_ERROUT("%s\n", "illegal MAC address");
		return(-ENODEV);		
    }	
    
	netif_tx_disable(prp_dev);
	
/* TODO locks down the system     	
 	if(dev_priv->open_A == 1)
   	{
	   	dev_close(dev_priv->dev_A);
	} */
	
	if((NULL == dev_priv->dev_A->netdev_ops->ndo_set_mac_address)  || (dev_promisc != 0))	
	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "setting mac address not supported, setting cards in promiscous mode");
		if(0 == dev_priv->promisc_A)
		{
	   		dev_set_promiscuity(dev_priv->dev_A, 1);
			dev_priv->promisc_A = 1;
		}
 		memcpy(dev_priv->dev_A->dev_addr, new_addr.sa_data, dev_priv->dev_A->addr_len); /* set the mac address */   	 		
		prp_dev->flags |= IFF_PROMISC;
    }
    else if(0 != dev_set_mac_address(dev_priv->dev_A, &new_addr)) /* set the ethernet interface mac address*/
	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "can not set mac address, setting cards in promiscous mode");
		if(0 == dev_priv->promisc_A)
		{
	   		dev_set_promiscuity(dev_priv->dev_A, 1);
			dev_priv->promisc_A = 1;
		}
 		memcpy(dev_priv->dev_A->dev_addr, new_addr.sa_data, dev_priv->dev_A->addr_len); /* set the mac address */   	 		
		prp_dev->flags |= IFF_PROMISC;
   	}	
   	else
   	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "setting mac address on interface A succeeded");
   	}	
    
 	memcpy(prp_dev->dev_addr, new_addr.sa_data, prp_dev->addr_len); /* set the mac address of the prp virtual device */
 	
/* TODO locks down the system
	if(dev_priv->open_A == 1)
   	{
		dev_open(dev_priv->dev_A);   
	} */
	
	netif_wake_queue(prp_dev);
	
	return(0);
}

/************************************************************/
/*       PRP_NetItf_T_set_mac_address_B                     */
/************************************************************/
integer32 PRP_NetItf_T_set_mac_address_B(octet* mac)
{
	struct prp_dev_priv *dev_priv;
	struct sockaddr new_addr;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(mac == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);
	}
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
			
	dev_priv = netdev_priv(prp_dev);
	
	memcpy(new_addr.sa_data, mac, dev_priv->dev_B->addr_len);
	new_addr.sa_family = dev_priv->dev_B->type;
		
	if(is_valid_ether_addr(new_addr.sa_data) == 0)
    {
		PRP_ERROUT("%s\n", "illegal MAC address");
		return(-ENODEV);		
    }	
    
	netif_tx_disable(prp_dev);
	
/* TODO locks down the system
 	if(dev_priv->open_B == 1)
   	{
	    dev_close(dev_priv->dev_B);
    } */
 	
	if((NULL == dev_priv->dev_B->netdev_ops->ndo_set_mac_address) || (dev_promisc != 0))	
	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "setting mac address not supported, setting cards in promiscous mode");
		if(0 == dev_priv->promisc_B)
		{
	   		dev_set_promiscuity(dev_priv->dev_B, 1);
			dev_priv->promisc_B = 1;
		}
 		memcpy(dev_priv->dev_B->dev_addr, new_addr.sa_data, dev_priv->dev_B->addr_len); /* set the mac address */   	 		
		prp_dev->flags |= IFF_PROMISC;
    }
    else if(0 != dev_set_mac_address(dev_priv->dev_B, &new_addr)) /* set the ethernet interface mac address*/
	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "can not set mac address, setting cards in promiscous mode");
		if(0 == dev_priv->promisc_B)
		{
	   		dev_set_promiscuity(dev_priv->dev_B, 1);
			dev_priv->promisc_B = 1;
		}
 		memcpy(dev_priv->dev_B->dev_addr, new_addr.sa_data, dev_priv->dev_B->addr_len); /* set the mac address */   	 		
		prp_dev->flags |= IFF_PROMISC;
   	}	
   	else
   	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "setting mac address on interface B succeeded");
   	}	
   	
/* TODO locks down the system
 	if(dev_priv->open_B == 1)
   	{
	 	dev_open(dev_priv->dev_B); 
 	} */
 	
	netif_wake_queue(prp_dev);
	
	return(0);
}

/************************************************************/
/*       PRP_NetItf_T_set_supervision_address               */
/************************************************************/
integer32 PRP_NetItf_T_set_supervision_address(octet* mac)
{
	struct prp_dev_priv *dev_priv;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
		
	if(mac == NULL_PTR)
	{
		return(-PRP_ERROR_NULL_PTR);		
	}
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
			
	dev_priv = netdev_priv(prp_dev);
	
	dev_mc_del(dev_priv->dev_A, dev_priv->addr_supervision_multi.sa_data);
    dev_mc_del(dev_priv->dev_B, dev_priv->addr_supervision_multi.sa_data);

    memcpy(dev_priv->addr_supervision_multi.sa_data, mac, dev_priv->dev_A->addr_len);	
    
    dev_mc_add(dev_priv->dev_A, dev_priv->addr_supervision_multi.sa_data); /* multicast address for supervision frames */
    dev_mc_add(dev_priv->dev_B, dev_priv->addr_supervision_multi.sa_data); /* multicast address for supervision frames */
    
	return(0);
}


/************************************************************/
/*       PRP_NetItf_T_transmit                              */
/************************************************************/
integer32 PRP_NetItf_T_transmit(octet* data, uinteger32* length, octet lan_id)
{
	/* TODO check efficiency */
	struct sk_buff *skb = NULL;
	struct prp_dev_priv *dev_priv;
	
	dev_priv = netdev_priv(prp_dev);
	
	skb = alloc_skb(*length, GFP_ATOMIC );
	
	if(skb == NULL)
	{
		PRP_ERROUT("%s\n", "can not allocate skb struct");
		return(-PRP_ERROR_NULL_PTR);		
	}
	
	rcu_read_lock();	
	
	skb->len = *length;
	memcpy(skb->data, data, *length);

	skb->network_header = skb->data;
	
	if((lan_id == PRP_ID_LAN_A) && (dev_priv->open_A == 1))
	{
		skb->dev = dev_priv->dev_A;
	}
	else if((lan_id == PRP_ID_LAN_B) && (dev_priv->open_B == 1))
	{
		skb->dev = dev_priv->dev_B;
	}
	else
	{
		rcu_read_unlock();		
	    dev_kfree_skb_any(skb);
	}

	skb->priority = 1;
	/* Fix against message – “protocol xxx is buggy”  */
	skb_set_network_header(skb, 0);
	skb_set_mac_header(skb, 0);
	skb->mac_len = ETH_HLEN;
	rcu_read_unlock();		
		
	dev_priv->stats.tx_packets++;
	dev_priv->stats.tx_bytes += skb->len;	

	dev_queue_xmit(skb);
	
	prp_dev->trans_start = jiffies;

	return(0);
}




/********************************************************************/
/*			local function				    						*/
/********************************************************************/
static void PRP_Dev_T_timer(unsigned long data)
{
	struct net_device *dev = (struct net_device*)data;
	struct prp_dev_priv *dev_priv;
	int link_A;
	int link_B;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
 	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return;		
	} 	
		
	dev_priv = netdev_priv(dev);
	
	PRP_T_timer(); /* core timer function*/
	
	link_A = netif_carrier_ok(dev_priv->dev_A);
	if((dev_priv->link_A) && !link_A)
	{
		PRP_ERROUT("%s\n", "link down A");
	}
	dev_priv->link_A = link_A;
	
	link_B = netif_carrier_ok(dev_priv->dev_B);
	if((dev_priv->link_B) && !link_B)
	{
		PRP_ERROUT("%s\n", "link down B");
	}
	dev_priv->link_B = link_B;
	
	
    mod_timer(&(dev_priv->alive_timer), (jiffies + (HZ/10)));
}

/********************************************************************/
/*			device functions			    						*/
/********************************************************************/

/* the real init function called by the register_netdev function per device*/
/*****************************************************************************************/
static void PRP_Dev_T_init(struct net_device *dev)
{	
	struct prp_dev_priv *dev_priv;
	struct net_device *dev_A;
	struct net_device *dev_B;

	static const struct net_device_ops prp_netdev_ops = {
		.ndo_open = PRP_Dev_T_open,
		.ndo_stop = PRP_Dev_T_stop,
		.ndo_get_stats = PRP_Dev_T_get_stats,
		.ndo_set_mac_address = PRP_Dev_T_set_mac_address,
		.ndo_set_rx_mode = PRP_Dev_T_set_multicast_list,
		.ndo_change_mtu = PRP_Dev_T_change_mtu,
		.ndo_start_xmit = PRP_Dev_T_hard_start_xmit,
		.ndo_do_ioctl = PRP_Dev_T_do_ioctl
	};
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
 	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return;		
	}   
		
	PRP_NET_ITF_LOGOUT(0, "opening device A: [%s]\n", dev_name_A);
	dev_A = dev_get_by_name(&init_net, dev_name_A);
	PRP_NET_ITF_LOGOUT(0, "opening device B: [%s]\n", dev_name_B);
	dev_B = dev_get_by_name(&init_net, dev_name_B);
	
	if((dev_A == NULL) || (dev_B == NULL))
	{
		PRP_ERROUT("%s\n", "can not find the needed ethernet interfaces");
	   	return;
	}
		
	/* tell the kernel the device parameters */
	ether_setup(dev); /* assign some of the fields */	
	
 	memcpy(dev->dev_addr, dev_A->dev_addr, dev_A->addr_len); /* set the mac address of the prp virtual device to the same */

	dev->netdev_ops = &prp_netdev_ops;

	dev->tx_queue_len = 0;
	if(((dev_A->flags & IFF_MULTICAST) != 0) && ((dev_B->flags & IFF_MULTICAST) != 0))
	{
		dev->flags |= IFF_MULTICAST;
	}
	else
	{
		dev->flags &= ~IFF_MULTICAST;
	}
	
	dev->flags |= IFF_MASTER;
	dev->flags |= IFF_DYNAMIC;
	dev->mtu = PRP_MTU;
	if((dev_A->netdev_ops->ndo_change_mtu != NULL) && (dev_B->netdev_ops->ndo_change_mtu != NULL))
	{
		dev_A->netdev_ops->ndo_change_mtu(dev_A, PRP_MTU);
		dev_B->netdev_ops->ndo_change_mtu(dev_B, PRP_MTU);
	}
	
	dev_priv = netdev_priv(dev);
	memset(dev_priv, 0, sizeof(struct prp_dev_priv));	
	dev_priv->dev_A = dev_A;
	dev_priv->dev_B = dev_B;
	dev_priv->promisc_A = 0;
	dev_priv->promisc_B = 0;
	dev_priv->open_A = 0;	
	dev_priv->open_B = 0;	
	
    init_timer(&(dev_priv->alive_timer));
    dev_priv->alive_timer.data = (unsigned long)dev;
    dev_priv->alive_timer.function = PRP_Dev_T_timer;
				   	
	PRP_INFOOUT("%s\n", "module loaded");

	return;
}


/* the module init function called once for all device( only one device available )*/
/*****************************************************************************************/
static int __init PRP_Dev_T_init_module(void)
{
		
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
    /* allocate a net_device structure */
    if((prp_dev = alloc_netdev(sizeof(struct prp_dev_priv), "prp%d", NET_NAME_UNKNOWN, PRP_Dev_T_init)) == NULL)
    {
		PRP_ERROUT("%s\n", "can not allocate net device structure, out of memory?");
        return(-ENOMEM);
    }   
	
	if(0 != register_netdev(prp_dev))
	{
		PRP_ERROUT("%s\n", "can not register the device");
		return(-ENODEV);		
	}
	
	return(0);
}


/* the module exit function */
/*****************************************************************************************/
static void __exit PRP_Dev_T_exit_module(void)
{	
	struct prp_dev_priv *dev_priv;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
				
	dev_priv = netdev_priv(prp_dev);
	
	dev_put(dev_priv->dev_A); /* give the lock back */
	dev_put(dev_priv->dev_B); /* give the lock back */
	unregister_netdev(prp_dev); /* removes the device from the netdevice list */
	free_netdev(prp_dev); /* release memory*/
	
	PRP_INFOOUT("%s\n", "module unloaded");
    
	return;
}


/*****************************************************************************************/
static int PRP_Dev_T_open(struct net_device *dev)
{
	struct prp_dev_priv *dev_priv;
	PRP_MergeLayerInfo_T merge_layer_info;
	char node_name[] = "ZHW PRP Node";
	char manufacturer_name[] = "ZHW InES";
	char version[] = "the allmost running one";
	char bridging_address[PRP_ETH_ADDR_LENGTH];	
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
			
	dev_priv = netdev_priv(prp_dev);

	/* save old mac addresses */
	memcpy(dev_priv->addr_A.sa_data, dev_priv->dev_A->dev_addr, dev_priv->dev_A->addr_len);
	dev_priv->addr_A.sa_family = dev_priv->dev_A->type;
	memcpy(dev_priv->addr_B.sa_data, dev_priv->dev_B->dev_addr, dev_priv->dev_B->addr_len);
	dev_priv->addr_B.sa_family = dev_priv->dev_B->type;

	memcpy(dev->dev_addr, dev_priv->dev_A->dev_addr, dev_priv->dev_A->addr_len); /* set the mac address of the prp virtual device to the same */
		
    PRP_T_init();
    	
    PRP_T_get_merge_layer_info(&merge_layer_info); 
 	memset(merge_layer_info.node_, '\0' , PRP_NODE_NAME_LENGTH);	
 	memcpy(merge_layer_info.node_, node_name, sizeof(node_name));
 	memset(merge_layer_info.manufacturer_, '\0' , PRP_MANUFACTURER_NAME_LENGTH);	
 	memcpy(merge_layer_info.manufacturer_, manufacturer_name, sizeof(manufacturer_name));
 	memset(merge_layer_info.version_, '\0' , PRP_VERSION_LENGTH);	
 	memcpy(merge_layer_info.version_, version, sizeof(version));
 	memcpy(merge_layer_info.mac_address_A_, dev->dev_addr, PRP_ETH_ADDR_LENGTH); 
 	memcpy(merge_layer_info.mac_address_B_, dev->dev_addr, PRP_ETH_ADDR_LENGTH);  	
	merge_layer_info.adapter_active_A_ = FALSE;
	merge_layer_info.adapter_active_B_ = FALSE;
    PRP_T_set_merge_layer_info(&merge_layer_info);    
    
  
    memcpy(dev_priv->addr_supervision_multi.sa_data, merge_layer_info.supervision_address_, dev_priv->dev_A->addr_len);	
    dev_mc_add(dev_priv->dev_A, dev_priv->addr_supervision_multi.sa_data); /* multicast address for supervision frames */
    dev_mc_add(dev_priv->dev_B, dev_priv->addr_supervision_multi.sa_data); /* multicast address for supervision frames */
    
	bridging_address[0] = 0x01;
	bridging_address[1] = 0x80;
	bridging_address[2] = 0xc2;
	bridging_address[3] = 0x00;
	bridging_address[4] = 0x00;
	bridging_address[5] = 0x00; /* rstp */
	
    memcpy(dev_priv->addr_bridging_multi.sa_data, bridging_address, dev_priv->dev_A->addr_len);	
   	dev_mc_add(dev_priv->dev_A, dev_priv->addr_bridging_multi.sa_data); /* multicast address for rstp frames */
    dev_mc_add(dev_priv->dev_B, dev_priv->addr_bridging_multi.sa_data); /* multicast address for rstp frames */
    	
		
	/* dummy hook to fix a kernel "bug"/"feature" */
	dummy_packet_type.type = __constant_htons(ETH_P_ALL);
	dummy_packet_type.func = PRP_Dev_T_receive_dummy;
	dummy_packet_type.dev = NULL;

	dev_add_pack(&dummy_packet_type); /* has to be called before the other call because list is a lifo*/
	
	prp_packet_type.type = __constant_htons(ETH_P_ALL);
	prp_packet_type.func = PRP_Dev_T_receive;
	prp_packet_type.dev = NULL;
	
	dev_add_pack(&prp_packet_type);

	if(0 != dev_open(dev_priv->dev_A))
	{
		PRP_ERROUT("%s\n", "could not open ethernet device A");
		return(-ENODEV);		
	}	
	dev_priv->open_A = 1;	
	
	if(0 != dev_open(dev_priv->dev_B))
	{	
	   	dev_close(dev_priv->dev_A);
		dev_priv->open_A = 0;	
		PRP_ERROUT("%s\n", "could not open ethernet device B");
		/* set mac address to the old value*/
		if(0 != dev_priv->promisc_B)
		{
			dev_set_promiscuity(dev_priv->dev_B, -1);
	 		dev_priv->promisc_B = 0;
		}		
		if((NULL == dev_priv->dev_B->netdev_ops->ndo_set_mac_address) || (dev_promisc != 0))	
		{
	 		memcpy(dev_priv->dev_B->dev_addr, dev_priv->addr_B.sa_data, dev_priv->dev_B->addr_len); /* reset the mac address */   	 		
	    }
	    else if(0 != dev_set_mac_address(dev_priv->dev_B, &(dev_priv->addr_B))) /* reset the mac address */
		{
	 		memcpy(dev_priv->dev_B->dev_addr, dev_priv->addr_B.sa_data, dev_priv->dev_B->addr_len); /* reset the mac address */   	 		
	   	}	
		if((0 == dev_priv->promisc_A) && (0 == dev_priv->promisc_B))
		{
	 		prp_dev->flags &= ~IFF_PROMISC;
		}		
		return(-ENODEV);		
	}	
	dev_priv->open_B = 1;	

    PRP_T_get_merge_layer_info(&merge_layer_info); 
	merge_layer_info.adapter_active_A_ = TRUE;
	merge_layer_info.adapter_active_B_ = TRUE;
    PRP_T_set_merge_layer_info(&merge_layer_info); 
    
	dev_priv->link_A = netif_carrier_ok(dev_priv->dev_A);
	dev_priv->link_B = netif_carrier_ok(dev_priv->dev_B);
       
    mod_timer(&(dev_priv->alive_timer), (jiffies + (HZ/10)));
		
	/* start the kernel queue */ 
	netif_start_queue(dev);

	return(0);
	
}

/*****************************************************************************************/
static int PRP_Dev_T_stop(struct net_device *dev)
{
	struct prp_dev_priv *dev_priv;
	PRP_MergeLayerInfo_T merge_layer_info;
		
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
	 		
	dev_priv = netdev_priv(dev);
	
	/* stop the kernel queue */ 
	netif_stop_queue(dev);
		
    del_timer_sync(&(dev_priv->alive_timer));
 	
    dev_mc_del(dev_priv->dev_A, dev_priv->addr_supervision_multi.sa_data);
    dev_mc_del(dev_priv->dev_B, dev_priv->addr_supervision_multi.sa_data);
 	
    dev_mc_del(dev_priv->dev_A, dev_priv->addr_bridging_multi.sa_data);
    dev_mc_del(dev_priv->dev_B, dev_priv->addr_bridging_multi.sa_data);
    
	if(dev_priv->open_A == 1)
   	{
	 	dev_close(dev_priv->dev_A);
		dev_priv->open_A = 0;	
	}
 	if(dev_priv->open_B == 1)
   	{
		dev_close(dev_priv->dev_B);
		dev_priv->open_B = 0;	
	}	
 
	dev_remove_pack(&prp_packet_type);
	
	/* dummy hook to fix a kernel "bug"/"feature" */
	dev_remove_pack(&dummy_packet_type);

    PRP_T_get_merge_layer_info(&merge_layer_info); 
	merge_layer_info.adapter_active_A_ = FALSE;
	merge_layer_info.adapter_active_B_ = FALSE;
    PRP_T_set_merge_layer_info(&merge_layer_info);    
	
 	PRP_T_cleanup();

	if(0 != dev_priv->promisc_A)
	{
		dev_set_promiscuity(dev_priv->dev_A, -1);
 		dev_priv->promisc_A = 0;
	}
		
	if((NULL == dev_priv->dev_A->netdev_ops->ndo_set_mac_address)  || (dev_promisc != 0))	
	{
 		memcpy(dev_priv->dev_A->dev_addr, dev_priv->addr_A.sa_data, dev_priv->dev_A->addr_len); /* reset the mac address */   	 		
    }
    else if(0 != dev_set_mac_address(dev_priv->dev_A, &(dev_priv->addr_A))) /* reset the mac address */
	{
 		memcpy(dev_priv->dev_A->dev_addr, dev_priv->addr_A.sa_data, dev_priv->dev_A->addr_len); /* reset the mac address */   	 		
   	}	
   	
   	
	if(0 != dev_priv->promisc_B)
	{
		dev_set_promiscuity(dev_priv->dev_B, -1);
 		dev_priv->promisc_B = 0;
	}
	
	if((NULL == dev_priv->dev_B->netdev_ops->ndo_set_mac_address)  || (dev_promisc != 0))	
	{
 		memcpy(dev_priv->dev_B->dev_addr, dev_priv->addr_B.sa_data, dev_priv->dev_B->addr_len); /* reset the mac address */   	 		
    }
    else if(0 != dev_set_mac_address(dev_priv->dev_B, &(dev_priv->addr_B))) /* reset the mac address */
	{
 		memcpy(dev_priv->dev_B->dev_addr, dev_priv->addr_B.sa_data, dev_priv->dev_B->addr_len); /* reset the mac address */   	 		
   	}	
	
	if((0 == dev_priv->promisc_A) && (0 == dev_priv->promisc_B))
	{
 		prp_dev->flags &= ~IFF_PROMISC;
	}
   	
	return(0);
}

/*****************************************************************************************/
static int PRP_Dev_T_receive(struct sk_buff *skb, struct net_device *dev, struct packet_type* ptype, struct net_device *orig_dev)
{
	int res = -1;
	struct prp_dev_priv *dev_priv;
	unsigned int data_len;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		kfree_skb(skb);
		return(NET_RX_SUCCESS);		
	}
	
	dev_priv = netdev_priv(prp_dev);
	
	rcu_read_lock();

 	if(skb->pkt_type == PACKET_OUTGOING)
 	{
		PRP_NET_ITF_LOGOUT(2, "sending frame to dev: %s \n", orig_dev->name);
		kfree_skb(skb);
		rcu_read_unlock();
		return(NET_XMIT_SUCCESS);
	}
 	 	
	if((orig_dev != dev_priv->dev_A) && (orig_dev != dev_priv->dev_B))
	{
		PRP_NET_ITF_LOGOUT(2, "received frame from non duo dev: %s \n", orig_dev->name);
		kfree_skb(skb);
		rcu_read_unlock();		
		return(NET_RX_SUCCESS);
	}
			
		
 	data_len = (skb->len + ETH_HLEN);
	
	if(orig_dev == dev_priv->dev_A)
	{
		PRP_NET_ITF_LOGOUT(2, "received frame from dev_A: %s \n", orig_dev->name);
		res = PRP_T_receive((skb->data - ETH_HLEN), &data_len, PRP_ID_LAN_A); /* core receive function */
	}		
	else if(orig_dev == dev_priv->dev_B)
	{
		PRP_NET_ITF_LOGOUT(2, "received frame from dev_B: %s \n", orig_dev->name);
		res = PRP_T_receive((skb->data - ETH_HLEN), &data_len, PRP_ID_LAN_B); /* core receive function */
	}
			
	skb->len = (data_len - ETH_HLEN); /*just shift change the length */
	
	/* update stats */ 	
	dev_priv->stats.rx_packets++;
	dev_priv->stats.rx_bytes += skb->len;	
			
	if(res == PRP_KEEP)
	{
		skb->dev = prp_dev;
		netif_rx(skb);	// forward frame to virtual prp device
		rcu_read_unlock();
		return(NET_RX_SUCCESS);
	}
	else if(res == PRP_DROP)
	{
		skb->dev = prp_dev;
		skb->protocol = 0xffff; /* overwrite protocol that no other protocol gets called and frame gets dropped*/
		kfree_skb(skb);
		rcu_read_unlock();
		return(NET_RX_DROP);
	}
	else
	{
		kfree_skb(skb);
		rcu_read_unlock();
		return(NET_RX_BAD);
	}	
}

/*****************************************************************************************/
/* dummy function to fix a kernel "bug"/"feature" */
static int PRP_Dev_T_receive_dummy(struct sk_buff *skb, struct net_device *dev, struct packet_type* ptype, struct net_device *orig_dev)
{
	kfree_skb(skb);
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	return(NET_RX_SUCCESS);
}


/*****************************************************************************************/
static int PRP_Dev_T_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	unsigned int data_len;

	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
		
	if(skb == NULL)/* means that the kernel had no space for a transmit skbuff (packet dropped in the kernel)*/
	{
		PRP_NET_ITF_LOGOUT(2, "%s\n", "no data to send");
		return(0);		
	}

	skb_pad(skb, PRP_RCT_LENGTH);	/* expand here because there is no expanding in the core */
 	data_len = (skb->len);
				
 	PRP_T_transmit(skb->data, &data_len); /* core send function */
 	 			
    dev_kfree_skb_any(skb);
    
	return(0);
	
}




/*****************************************************************************************/
static int PRP_Dev_T_do_ioctl(struct net_device* dev, struct ifreq* req, int cmd)
{
	PRP_MergeLayerInfo_T merge_layer_info;
	int ret = 0;
	struct prp_dev_priv *dev_priv;
		
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
	
	dev_priv = netdev_priv(dev);
	
	switch(cmd)
	{
		case PRPDEV_IOCTL_GET_MERGE_LAYER_INFO:
			ret = PRP_T_get_merge_layer_info(&merge_layer_info);
            if(copy_to_user(req->ifr_data, &merge_layer_info, sizeof(PRP_MergeLayerInfo_T)))
            {
				ret = -EFAULT;
 				break;
	       	}
			break;
		case PRPDEV_IOCTL_SET_MERGE_LAYER_INFO:
            if(copy_from_user(&merge_layer_info, req->ifr_data, sizeof(PRP_MergeLayerInfo_T)))
            {
				ret = -EFAULT;
				break;
        	}
			ret = PRP_T_set_merge_layer_info(&merge_layer_info);
			break;
		default:
			/* forward ioctl to the other two adpaters */
			if((dev_priv->dev_A->netdev_ops->ndo_do_ioctl != NULL) && (dev_priv->dev_B->netdev_ops->ndo_do_ioctl != NULL))
			{
				ret = dev_priv->dev_A->netdev_ops->ndo_do_ioctl(dev_priv->dev_A , req, cmd);
				ret |= dev_priv->dev_B->netdev_ops->ndo_do_ioctl(dev_priv->dev_B , req, cmd);
			}
			break;
	}
	
	return(ret);
}


/*****************************************************************************************/
static int PRP_Dev_T_set_mac_address(struct net_device* dev, void *addr)
{
	struct prp_dev_priv *dev_priv;
	struct sockaddr new_addr;
	PRP_MergeLayerInfo_T merge_layer_info;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}
	
	dev_priv = netdev_priv(dev);
	memcpy(&new_addr, addr, sizeof(struct sockaddr));
	
	if(is_valid_ether_addr(new_addr.sa_data) == 0)
    {
		PRP_ERROUT("%s\n", "illegal MAC address");
		return(-ENODEV);		
    }	
    
     	
/* TODO locks down the system     	
 	if(dev_priv->open_A == 1)
   	{
	   	dev_close(dev_priv->dev_A);
	} */
		
	if((NULL == dev_priv->dev_A->netdev_ops->ndo_set_mac_address || (dev_promisc != 0)))	
	{
		PRP_NET_ITF_LOGOUT(0, "%s\n", "setting mac address not supported, setting cards in promiscous mode");
		if(0 == dev_priv->promisc_A)
		{
	   		dev_set_promiscuity(dev_priv->dev_A, 1);
			dev_priv->promisc_A = 1;
		}
 		memcpy(dev_priv->dev_A->dev_addr, new_addr.sa_data, dev_priv->dev_A->addr_len); /* set the mac address */   	 		
		dev->flags |= IFF_PROMISC;
    }
    else if(0 != dev_set_mac_address(dev_priv->dev_A, &new_addr)) /* set the ethernet interface mac address*/
	{
		PRP_NET_ITF_LOGOUT(0, "%s\n", "can not set mac address, setting cards in promiscous mode");
		if(0 == dev_priv->promisc_A)
		{
	   		dev_set_promiscuity(dev_priv->dev_A, 1);
			dev_priv->promisc_A = 1;
		}
 		memcpy(dev_priv->dev_A->dev_addr, new_addr.sa_data, dev_priv->dev_A->addr_len); /* set the mac address */   	 		
		dev->flags |= IFF_PROMISC;
   	}	   	
   	
/* TODO locks down the system     	
 	if(dev_priv->open_A == 1)
   	{
		dev_open(dev_priv->dev_A);   
	} */
	
   	
/* TODO locks down the system     	
 	if(dev_priv->open_B == 1)
   	{
	   	dev_close(dev_priv->dev_B);
	} */
	
	if((NULL == dev_priv->dev_B->netdev_ops->ndo_set_mac_address) || (dev_promisc != 0))	
	{
		PRP_NET_ITF_LOGOUT(0, "%s\n", "setting mac address on interface B not supported, setting interface in promiscuis mode");
		if(0 == dev_priv->promisc_B)
		{
	   		dev_set_promiscuity(dev_priv->dev_B, 1);
			dev_priv->promisc_B = 1;
		}
 		memcpy(dev_priv->dev_B->dev_addr, new_addr.sa_data, dev_priv->dev_B->addr_len); /* set the mac address */   	 		
		dev->flags |= IFF_PROMISC;
    }
    else if(0 != dev_set_mac_address(dev_priv->dev_B, &new_addr)) /* set the ethernet interface mac address*/
	{
		PRP_NET_ITF_LOGOUT(0, "%s\n", "can not set mac address on interface B, setting interface in promiscuis mode");
		if(0 == dev_priv->promisc_B)
		{
	   		dev_set_promiscuity(dev_priv->dev_B, 1);
			dev_priv->promisc_B = 1;
		}
 		memcpy(dev_priv->dev_B->dev_addr, new_addr.sa_data, dev_priv->dev_B->addr_len); /* set the mac address */   	 		
		dev->flags |= IFF_PROMISC;
   	}	
   	
/* TODO locks down the system     	
  	if(dev_priv->open_B == 1)
   	{
		dev_open(dev_priv->dev_B);   
	} */
	
      	
 	memcpy(dev->dev_addr, new_addr.sa_data, dev->addr_len); /* set the mac address of the prp virtual device */

    PRP_T_get_merge_layer_info(&merge_layer_info); 
 	memcpy(merge_layer_info.mac_address_A_, dev_priv->dev_A->dev_addr, PRP_ETH_ADDR_LENGTH); 
 	memcpy(merge_layer_info.mac_address_B_, dev_priv->dev_B->dev_addr, PRP_ETH_ADDR_LENGTH);  	
    PRP_T_set_merge_layer_info(&merge_layer_info);    
 	  	
 	return(0);  
}	


/*****************************************************************************************/
static struct net_device_stats* PRP_Dev_T_get_stats(struct net_device *dev)
{
	struct prp_dev_priv *dev_priv;
	
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(NULL);		
	}	
	
	dev_priv = netdev_priv(dev);
	
	return(&(dev_priv->stats));
}


/*****************************************************************************************/
static void PRP_Dev_T_set_multicast_list(struct net_device *dev)
{
	struct prp_dev_priv *dev_priv;
	
	if(prp_dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return;		
	}
	
	dev_priv = netdev_priv(prp_dev);
	
	/*
	 * Do promisc before checking multicast_mode
	 */
	if((dev->flags & IFF_PROMISC) && !(prp_dev->flags & IFF_PROMISC)) 
	{
		dev_set_promiscuity(dev_priv->dev_A, 1);
	   	dev_set_promiscuity(dev_priv->dev_B, 1);
	}	
	else if(!(dev->flags & IFF_PROMISC) && (prp_dev->flags & IFF_PROMISC)) 
	{
	    dev_set_promiscuity(dev_priv->dev_A, -1);
	    dev_set_promiscuity(dev_priv->dev_B, -1);
	}
	
	/* set allmulti flag to slaves */
	if((dev->flags & IFF_ALLMULTI) && !(prp_dev->flags & IFF_ALLMULTI)) 
	{
	    dev_set_allmulti(dev_priv->dev_A, 1);
	    dev_set_allmulti(dev_priv->dev_B, 1);
	}
	else if(!(dev->flags & IFF_ALLMULTI) && (prp_dev->flags & IFF_ALLMULTI)) 
	{
	    dev_set_allmulti(dev_priv->dev_A, -1);
	    dev_set_allmulti(dev_priv->dev_B, -1);
	}
	
	prp_dev->flags = dev->flags;
	
	/* sync multicast list from dev to devA and devB */
	dev_mc_sync(dev_priv->dev_A, dev);
	dev_mc_sync(dev_priv->dev_B, dev);
}

/*****************************************************************************************/
static int PRP_Dev_T_change_mtu(struct net_device *dev, int new_mtu)
{
	PRP_NET_ITF_LOGOUT(3, "[%s] entering \n", __FUNCTION__);
	
	if(dev == NULL)
	{
		PRP_ERROUT("%s\n", "function call from undefined device");
		return(-ENODEV);		
	}	
	
	if(new_mtu > PRP_MTU)
	{
		return(-EINVAL);
	}
	dev->mtu = new_mtu;
	return(0);	
}

/*****************************************************************************************/
/* tell the kernel the init and exit functions */
module_init(PRP_Dev_T_init_module)
module_exit(PRP_Dev_T_exit_module)

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PRP Virtual Device");
MODULE_AUTHOR("Sven Meier");
