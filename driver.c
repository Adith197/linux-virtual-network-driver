#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithyaa");
MODULE_DESCRIPTION("Virtual Network Driver");

static struct net_device *vnet_dev;
static struct net_device *vnet1_dev;

struct vnet_priv{
    struct net_device *peer;
};

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

static netdev_tx_t vnet_start_xmit( struct sk_buff *skb,struct net_device *dev){
    struct vnet_priv *priv;
    struct net_device *peer;
    pr_info("packet transmitted\n");
    pr_info("packet length - %u\n",skb->len);
    pr_info("protocol - 0x%04x\n",ntohs(skb->protocol));//ntosh - network to host short
    priv = netdev_priv(dev);
    peer = priv->peer;
    if(!peer){
        pr_err("vnet: peer device not set\n");
        dev_kfree_skb(skb);
        return NETDEV_TX_OK;
    }
    pr_info("vnet: delivered packet to peer device\n");
    pr_info("vnet: peer device name - %s\n",peer->name);
    int ret;
    ret = dev_forward_skb(peer, skb);
    if(ret == NET_RX_DROP){
        pr_err("vnet: failed to forward packet to peer device\n");
    }
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
    vnet_dev =alloc_netdev(sizeof(struct vnet_priv),"vnet%d",NET_NAME_UNKNOWN,ether_setup);
    vnet1_dev =alloc_netdev(sizeof(struct vnet_priv),"vnet%d",NET_NAME_UNKNOWN,ether_setup);
    if(!vnet_dev){
        pr_err("vnet: failed to allocate net device\n");
        return -ENOMEM;
    }
    if(!vnet1_dev){
        pr_err("vnet1: failed to allocate net device\n");
        free_netdev(vnet_dev);
        vnet_dev = NULL;
        return -ENOMEM;
    }
    unsigned char mac_addr[] = {0x02,0x00,0x00,0x00,0x00,0x01};
    eth_hw_addr_set(vnet_dev, mac_addr);
    unsigned char mac_addr1[] = {0x02,0x00,0x00,0x00,0x00,0x02};
    eth_hw_addr_set(vnet1_dev,mac_addr1);
    vnet_dev->netdev_ops = &vnet_ops;
    vnet1_dev->netdev_ops=&vnet_ops;
    struct vnet_priv *priv0;
    struct vnet_priv *priv1;
    priv0=netdev_priv(vnet_dev);
    priv1=netdev_priv(vnet1_dev);
    priv0->peer=vnet1_dev;
    priv1->peer=vnet_dev;
    int ret,ret1;
    ret = register_netdev(vnet_dev);
    ret1=register_netdev(vnet1_dev);
    if(ret){
        pr_err("vnet: failed to register tx net device\n");
        free_netdev(vnet_dev);
        vnet_dev = NULL;
        free_netdev(vnet1_dev);
        vnet1_dev=NULL;
        return ret;
    }
    if(ret1){
        pr_err("vnet1: failed to register rx net device\n");
        free_netdev(vnet1_dev);
        vnet1_dev = NULL;
        free_netdev(vnet_dev);
        vnet_dev=NULL;
        return ret1;
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
    if (vnet1_dev){
        unregister_netdev(vnet1_dev);
        free_netdev(vnet1_dev);
        vnet1_dev=NULL;
    }
    pr_info("vnet: module removed from kernel\n");
    pr_info("vnet1: module removed from kernel\n");
}

module_init(vnet_init);
module_exit(vnet_exit);