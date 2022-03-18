#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock_types.h>
#include <linux/if_arp.h>

#define DEVNAME "rocklee"
#define ROCKLEE_MAX_PKT_LEN      1500
#define ETH_MAC_LEN 6

static int major = 0;
static int minor = 0;
static struct class *cls = NULL;
static struct cdev  *dev_obj = NULL;
static struct device *devp = NULL;
static const char *servername = NULL;
static int tcp_port = 0;

module_param(servername,charp,S_IRUGO);
module_param(tcp_port,int,S_IRUGO);  

typedef struct tag_rocklee_net_priv {
    spinlock_t lock;
    struct net_device_stats stats;
}rocklee_net_priv;

typedef struct tag_rocklee_priv_device {
    int devnum;
}rocklee_priv_device;


/***********************************************802.11***********************************************************/





/***********************************************802.3***********************************************************/

static int rocklee_ndev_init(struct net_device *ndev)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    netif_start_queue(ndev);
    //netif_stop_queue(ndev);

    return 0;
}

static int rocklee_ndev_open(struct net_device *ndev)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return 0;
}

static int rocklee_ndev_stop(struct net_device *ndev)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return 0;
}

static int rocklee_ndev_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    printk(KERN_INFO "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    return 0;
}

static int rocklee_ndev_change_mtu(struct net_device *dev, int new_mtu)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return -EINVAL;
}

static int rocklee_ndev_validate_addr(struct net_device *dev)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    if (!is_valid_ether_addr(dev->dev_addr))
        return -EADDRNOTAVAIL;

    return 0;
}

static void rocklee_ndev_tx_timeout(struct net_device *dev, unsigned int txqueue)
{
    printk(KERN_INFO "Fn:%s Ln:%d ...\n",__func__,__LINE__);
}

static int rocklee_ndev_set_mac_address(struct net_device *dev, void *addr)
{
    rocklee_net_priv *priv = netdev_priv(dev);
    struct sockaddr *hwaddr = addr;

    printk(KERN_EMERG "Fn:%s Ln:%d ...[0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]\n",__func__,__LINE__, hwaddr->sa_data[0], hwaddr->sa_data[1], hwaddr->sa_data[2], hwaddr->sa_data[3], hwaddr->sa_data[4], hwaddr->sa_data[5]);

    if (!is_valid_ether_addr(hwaddr->sa_data))
        return -EADDRNOTAVAIL;
    spin_lock_bh(&priv->lock);
    memcpy(dev->dev_addr, hwaddr->sa_data, ETH_MAC_LEN);
    spin_unlock_bh(&priv->lock);
    return 0;
}

//该函数系统后台会隔一段时间 调用一次
static struct net_device_stats *rocklee_ndev_get_stats(struct net_device *dev)
{
    rocklee_net_priv *priv = netdev_priv(dev);
    printk(KERN_INFO "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return &priv->stats;
}

//ifconfig -a 把所有up 与 没有up的网卡都打印出来   
//sudo ifconfig rockllee0 up 
//sudo ifconfig rockllee0 down 
//sudo insmod rocklee_kernel_platformdrv_netdevice.ko
//sudo rmsmod rocklee_kernel_platformdrv_netdevice
//sudo ifconfig rockllee0  192.168.4.66
//sudo ifconfig rockllee0  hw ether fc:aa:14:e3:e2:88
//sudo dmesg -c  
//ifconfig
//有线网卡接收数据流  ==> 看 github 的 网络包收发流程.xlsx 
//无线网卡接收数据流  ==> 看 github 的 网络包收发流程.xlsx 
static const struct net_device_ops g_rocklee_netdev_ops = {
    .ndo_init = rocklee_ndev_init,
    .ndo_open = rocklee_ndev_open,
    .ndo_stop = rocklee_ndev_stop,
    .ndo_start_xmit = rocklee_ndev_start_xmit,
    .ndo_change_mtu = rocklee_ndev_change_mtu,
    .ndo_validate_addr = rocklee_ndev_validate_addr, //sudo ifconfig rockllee0  192.168.4.66
    .ndo_tx_timeout = rocklee_ndev_tx_timeout,
    .ndo_set_mac_address = rocklee_ndev_set_mac_address, //sudo ifconfig rockllee0  hw ether fc:aa:14:e3:e2:88
    .ndo_get_stats = rocklee_ndev_get_stats,
};

#if 0
static struct ethtool_ops g_rocklee_ethtool_ops = {
    .nway_reset = NULL,
    .get_link = NULL,
    .get_settings = NULL,
    .set_settings = NULL,
    .get_strings = NULL,
    .get_sset_count = NULL,
    .get_ethtool_stats = NULL,
    .get_drvinfo = NULL,    
    .self_test = NULL,        
};
#endif

//const struct iw_handler_def *wireless_handlers;
//无线网卡相关的调试函数
#if 0
static struct iw_handler_def g_rocklee_wireless_ops = {
    NULL,
};
#endif

//参考 drivers/net/wireless/cisco/airo.c
static void rocklee_ndev_setup(struct net_device *dev)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    //dev->wireless_handlers = &g_rocklee_wireless_ops;
    dev->netdev_ops = &g_rocklee_netdev_ops;
    //dev->ethtool_ops = &g_rocklee_ethtool_ops;
    dev->mtu                = ROCKLEE_MAX_PKT_LEN;
    dev->type = ARPHRD_ETHER ; //没有赋值的话，默认是(AMPR NET/ROM), 不能更改MAC地址，配置为 这个域段赋值后ARPHRD_ETHER 变为 Link encap:Ethernet 
    dev->addr_len = ETH_ALEN;  //ETH_ALEN 配置了这个长度后，dev->dev_addr 这个指针貌似才会被分配空间，修改MAC地址才能正常
    dev->flags = IFF_BROADCAST|IFF_MULTICAST;
#if 0
    dev->min_mtu            = 68;
    dev->max_mtu            = MIC_MSGLEN_MAX;
    dev->tx_queue_len       = 100;
    eth_broadcast_addr(dev->broadcast);
#endif
}

static int rocklee_net_driver_probe(struct platform_device *pdv)
{
    int ret;
    int i;
    int j;
    rocklee_priv_device *pDev = NULL;
    dev_t devt;
    struct device *dev = &pdv->dev;
    struct net_device *ndev = NULL;
    rocklee_net_priv *ndev_priv = NULL;
    
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);

    dev_obj = cdev_alloc();
    if (dev_obj == NULL) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        return -ENOMEM;
    }

    //character device init  with  fops
    cdev_init(dev_obj, &rocklee_fops);
    
    //DEVNAME:cat /proc/devices ==> show name 
    ret = alloc_chrdev_region(&devt, minor, 1, DEVNAME"_device");
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed(%d)...\n",__func__,__LINE__, ret);
        goto ERR_STEP;
    }
    major = MAJOR(devt);
    printk(KERN_EMERG"MAJOR(devt)=%d  MINOR(devt)=%d \n", MAJOR(devt), MINOR(devt));
    
    //insert cdev using devt  into system
    ret = cdev_add(dev_obj, devt, 1);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed(%d)...\n",__func__,__LINE__, ret);
        goto ERR_STEP;
    }
 
    pDev = kmalloc(sizeof(rocklee_priv_device), GFP_KERNEL);
    if (NULL == pDev) {
        ret = -ENOMEM;
        printk(KERN_EMERG "Fn:%s Ln:%d  kmalloc failed...\n",__func__,__LINE__);
        return -ENOMEM;
    }
    memset(pDev, 0, sizeof(rocklee_priv_device));

    pdv->dev.platform_data = pDev;//dev->platform_data = data;
    
    //create device class, ready for the next steps
    cls = class_create(THIS_MODULE, DEVNAME"_class"); 
    if(NULL == cls) {
        ret = -EBUSY;
        printk(KERN_EMERG "Fn:%s Ln:%d  class_create failed...\n",__func__,__LINE__);
        goto ERR_STEP1;
    }
   
    //create device node belong to device class
    devp = device_create(cls, NULL, MKDEV(major, minor), NULL, "%s%d", DEVNAME, minor);
    if (IS_ERR(devp)) {
        ret = PTR_ERR(devp);
        goto ERR_STEP2;
    }
    //create sysfs ==>/sys/
    for(j = 0; j < ARRAY_SIZE(rocklee_sysfs_attribute); j++) {
        ret = device_create_file(devp, &rocklee_sysfs_attribute[j]); //add sysfs entry
        if (ret) {
            printk(KERN_EMERG"add sysfs entry[%d][%d]  failed, ret = %d\n", i, j, ret);
            goto ERR_STEP3;
        }
    }
    
    //ret = request_irq(36, rocklee_interrupt, IRQ_TYPE_EDGE_FALLING, DEVNAME"_intrupt", devp);
    //if (ret < 0) {
    //    printk("Request IRQ %d failed, %d\n", i, ret);
    //    goto ERR_STEP4;
    //}

    ndev = alloc_netdev(sizeof(rocklee_net_priv), "rockllee%d", NET_NAME_UNKNOWN, rocklee_ndev_setup);
    if (ndev == NULL) {
        printk(KERN_EMERG"alloc_netdev failed, ndev = %d\n", ndev);
        goto ERR_STEP4;
    }
    
    SET_NETDEV_DEV(ndev, dev);//#define SET_NETDEV_DEV(net, pdev) ((net)->dev.parent = (pdev))
    
    ndev_priv = netdev_priv(ndev);
    memset(ndev_priv, 0, sizeof(*ndev_priv));
    /* 然后可以给ndev_priv 各个子域段赋值 */
    spin_lock_init(&ndev_priv->lock);
    
    ret = register_netdev(ndev);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed(%d)...\n",__func__,__LINE__, ret);
        goto ERR_STEP5;
    }  
    
    platform_set_drvdata(pdv, ndev); //dev->driver_data = data;
    
    return 0;
ERR_STEP5:    
    free_netdev(ndev);
ERR_STEP4:
    for(j = 0; j < ARRAY_SIZE(rocklee_sysfs_attribute); j++) {
        //rm sysfs entry
        device_remove_file(devp, &rocklee_sysfs_attribute[j]);
    }
ERR_STEP3:
    device_destroy(cls, MKDEV(major, minor));
ERR_STEP2:
    class_destroy(cls);
ERR_STEP1:
    unregister_chrdev_region(devt, 1);
    kfree(pDev);
ERR_STEP:
    cdev_del(dev_obj);
    printk(KERN_EMERG "Fn:%s Ln:%d  failed(%d)...\n",__func__,__LINE__, ret);
    return ret; 
}

static int rocklee_net_driver_remove(struct platform_device *pdv)
{
    int i;
    int j;
    struct net_device *ndev = (struct net_device *)pdv->dev.driver_data;
    //free_irq(xxx, pdv);

    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    
	for(j = 0; j < ARRAY_SIZE(rocklee_sysfs_attribute); j++) {
		//rm sysfs entry
		device_remove_file(devp, &rocklee_sysfs_attribute[j]);
	}
    
    device_destroy(cls, MKDEV(major, minor));
    class_destroy(cls);
    unregister_chrdev_region(MKDEV(major, minor), 1);
    cdev_del(dev_obj);
    
    unregister_netdev(ndev);
    free_netdev(ndev);
        
    kfree(pdv->dev.platform_data);
    pdv->dev.platform_data = 0;
    return 0;
}


static void rocklee_net_driver_shutdown(struct platform_device *pdv)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
}

static int rocklee_net_driver_suspend(struct platform_device *pdv,pm_message_t pmt)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    return 0;
}


static int rocklee_net_driver_resume(struct platform_device *pdv)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    return 0;
}

void rocklee_net_device_release(struct device *dev)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
}

struct platform_device rocklee_net_device = {
    .name = DEVNAME"_net_device",//platform_device'name  and platform_driver'name must be the same
    .id = -1,
    .dev.release = rocklee_net_device_release,
};


struct platform_driver rocklee_net_driver = {
    .probe = rocklee_net_driver_probe,
    .remove = rocklee_net_driver_remove,
    .shutdown = rocklee_net_driver_shutdown,
    .suspend = rocklee_net_driver_suspend,
    .resume = rocklee_net_driver_resume,
    .driver = {
        .name = DEVNAME"_net_device",//platform_device'name  and platform_driver'name must be the same
        .owner = THIS_MODULE,
    }
};

/***********************************************platform driver & device ********************************************/

static irqreturn_t rocklee_interrupt(int irq, void *dev_id)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return IRQ_HANDLED;
}

static ssize_t rocklee_show(struct device *pdevice, struct device_attribute *attr, char *buf)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    if (0 == strcmp(attr->attr.name,"rocklee_dbg_0")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    else  if (0 == strcmp(attr->attr.name,"rocklee_dbg_1")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    return 0;
}

static ssize_t rocklee_store(struct device *pdevice, struct device_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);

    if (0 == strcmp(attr->attr.name,"rocklee_dbg_0")) {

    } else if (0 == strcmp(attr->attr.name,"rocklee_dbg_1")) {

    }
    
    return count;
}

static struct device_attribute  rocklee_sysfs_attribute[] = {
    __ATTR(rocklee_dbg_0, 0664, rocklee_show, rocklee_store),
    __ATTR(rocklee_dbg_1, 0664, rocklee_show, rocklee_store),
};

static long rocklee_fops_ioctl( struct file *files, unsigned int cmd, unsigned long arg)
{
    printk(KERN_EMERG "Fn:%s Ln:%d  cmd is %d, arg is %ld ...\n",__func__,__LINE__, cmd, arg);
    switch(cmd)
    {
        case 0:
        case 1:
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static int rocklee_fops_release(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return 0;
}

static int rocklee_fops_open(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return 0;
}

static long rocklee_fops_write(struct file * pfile, const char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "Fn:%s Ln:%d buffer:%s  count:%ld ...\n",__func__,__LINE__, buffer, count);
    return 0;
}

static ssize_t rocklee_fops_read(struct file * pfile, char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "Fn:%s Ln:%d buffer:%s  count:%ld ...\n",__func__,__LINE__, buffer, count);
    return 0;
}

static struct file_operations rocklee_fops = {
    .owner = THIS_MODULE, 
    .open = rocklee_fops_open,
    .release = rocklee_fops_release,
    .write = rocklee_fops_write,
    .read = rocklee_fops_read,
    .unlocked_ioctl = rocklee_fops_ioctl,
};

static int rocklee_init(void)
{
    int ret;
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    ret = platform_driver_register(&rocklee_net_driver);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d failed(%d)...\n",__func__,__LINE__, ret);
        return ret;
    }
    ret = platform_device_register(&rocklee_net_device);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d failed...\n",__func__,__LINE__, ret);
        return ret;
    }
    return 0;
}

static void rocklee_exit(void)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    platform_driver_unregister(&rocklee_net_driver);
    platform_device_unregister(&rocklee_net_device);
}
/**********************************************************************************************************/

module_init(rocklee_init);
module_exit(rocklee_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rocklee");

