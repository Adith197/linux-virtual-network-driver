#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithyaa");
MODULE_DESCRIPTION("Virtual Network Driver");
static struct net_device *vnet_dev;
static int vnet_open(struct net_device *dev){
    pr_info("net device opened\n");
    netif_start_queue(dev);
    return 0;
}

static int vnet_stop(struct net_device *dev){
    pr_info("net device closed\n");
    netif_stop_queue(dev);
    return 0;
}

static int vnet_start_xmit( struct sk_buff *skb,struct net_device *dev){
    pr_info("packet transmitted\n");
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}
static const struct net_device_ops vnet_ops = {
    .ndo_open = vnet_open,
    .ndo_stop = vnet_stop,
    .ndo_start_xmit = vnet_start_xmit,
};
static int __init vnet_init(void)
{
    pr_info("vnet: module inserted\n");
    vnet_dev =alloc_netdev(0,"vnet%d",NET_NAME_UNKNOWN,ether_setup);
    if(!vnet_dev){
        pr_err("vnet: failed to allocate net device\n");
        return -ENOMEM;
    }
    vnet_dev->netdev_ops = &vnet_ops;
    int ret;
    ret = register_netdev(vnet_dev);
    if(ret){
        pr_err("vnet: failed to regiister net device\n");
        free_netdev(vnet_dev);
        vnet_dev = NULL;
        return ret;
    }
    pr_info("vnet: device registered\n");
    return 0;
}

static void __exit vnet_exit(void)
{
    if (vnet_dev) {
        unregister_netdev(vnet_dev);
        free_netdev(vnet_dev);
        vnet_dev = NULL;
    }
    pr_info("vnet: module removed from kernel\n");
}

module_init(vnet_init);
module_exit(vnet_exit);