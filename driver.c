#include <linux/module.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithyaa");
MODULE_DESCRIPTION("Virtual Network Driver");

static int __init vnet_init(void)
{
    pr_info("vnet: module inserted\n");
    return 0;
}

static void __exit vnet_exit(void)
{
    pr_info("vnet: module removed from kernel\n");
}

module_init(vnet_init);
module_exit(vnet_exit);