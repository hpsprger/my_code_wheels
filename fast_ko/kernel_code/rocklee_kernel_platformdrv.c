#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>


typedef struct tag_rocklee_priv_device {
    int devnum;
}rocklee_priv_device;

#define DEV_NUMS (1)

static int major = 0;
static int minor = 0;
const  int dev_count = DEV_NUMS;

static struct class *cls = NULL;
static struct cdev  *dev_obj = NULL;
static struct device *devp[DEV_NUMS] = {NULL};

#define DEVNAME "rocklee_cdev"

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

static int rocklee_driver_probe(struct platform_device *pdv)
{
    int ret;
    int i;
    int j;
    rocklee_priv_device *pDev = NULL;
    dev_t devt;

    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);

    dev_obj = cdev_alloc();
    if (dev_obj == NULL) {
    	printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        return -ENOMEM;
    }

    //character device init  with  fops
    cdev_init(dev_obj, &rocklee_fops);
    
    //DEVNAME:cat /proc/devices ==> show name 
    ret = alloc_chrdev_region(&devt, minor, dev_count, DEVNAME"_device");
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed(%d)...\n",__func__,__LINE__, ret);
        goto ERR_STEP;
    }
    major = MAJOR(devt);
    printk(KERN_EMERG"MAJOR(devt)=%d  MINOR(devt)=%d \n", MAJOR(devt), MINOR(devt));
    
    //insert cdev using devt  into system
    ret = cdev_add(dev_obj, devt, dev_count);
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

    //create device class, ready for the next steps
    cls = class_create(THIS_MODULE, DEVNAME"_class"); 
    if(NULL == cls) {
        ret = -EBUSY;
        printk(KERN_EMERG "Fn:%s Ln:%d  class_create failed...\n",__func__,__LINE__);
        goto ERR_STEP1;
    }
   
    //create device node belong to device class
    for (i = 0; i < dev_count; i++) {
        devp[i] = device_create(cls, NULL, MKDEV(major, minor + i), NULL, "%s%d", DEVNAME, i);
        if (IS_ERR(devp[i])) {
            ret = PTR_ERR(devp[i]);
            goto ERR_STEP2;
        }
        //create sysfs ==>/sys/
        for(j = 0; j < ARRAY_SIZE(rocklee_sysfs_attribute); j++) {
            ret = device_create_file(devp[i], &rocklee_sysfs_attribute[j]); //add sysfs entry
            if (ret) {
                printk(KERN_EMERG"add sysfs entry[%d][%d]  failed, ret = %d\n", i, j, ret);
                goto ERR_STEP3;
            }
        }
        
        //ret = request_irq(36, rocklee_interrupt, IRQ_TYPE_EDGE_FALLING, DEVNAME"_intrupt", devp[i]);
        //if (ret < 0) {
        //    printk("Request IRQ %d failed, %d\n", i, ret);
        //    goto ERR_STEP4;
        //}
    }

    //for passing paramter to other function
    pdv->dev.platform_data = pDev;  

    return 0;
ERR_STEP4:
    for (i = 0; i < dev_count; i++) {
        for(j = 0; j < ARRAY_SIZE(rocklee_sysfs_attribute); j++) {
            //rm sysfs entry
            device_remove_file(devp[i], &rocklee_sysfs_attribute[j]);
        }
    }
ERR_STEP3:
    for (i = 0; i <= minor; i++) {
        device_destroy(cls, MKDEV(major, i));
    }
ERR_STEP2:
    class_destroy(cls);
ERR_STEP1:
    unregister_chrdev_region(devt, dev_count);
    kfree(pDev);
ERR_STEP:
    cdev_del(dev_obj);
    printk(KERN_EMERG "Fn:%s Ln:%d  failed(%d)...\n",__func__,__LINE__, ret);
    return ret; 
}

static int rocklee_driver_remove(struct platform_device *pdv)
{
    int i;
    int j;
    
    //free_irq(xxx, pdv);

    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    
    for (i = 0; i < dev_count; i++) {
        for(j = 0; j < ARRAY_SIZE(rocklee_sysfs_attribute); j++) {
            //rm sysfs entry
            device_remove_file(devp[i], &rocklee_sysfs_attribute[j]);
        }
    }
    
    for (i = minor; i < (minor + dev_count); i++) {
        device_destroy(cls, MKDEV(major, i));
    }
    
    class_destroy(cls);
    unregister_chrdev_region(MKDEV(major, minor), dev_count);
    cdev_del(dev_obj);
        
    kfree(pdv->dev.platform_data);
    pdv->dev.platform_data = 0;
    return 0;
}


static void rocklee_driver_shutdown(struct platform_device *pdv)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
}

static int rocklee_driver_suspend(struct platform_device *pdv,pm_message_t pmt)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    return 0;
}


static int rocklee_driver_resume(struct platform_device *pdv)
{
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    return 0;
}

struct platform_driver rocklee_driver = {
    .probe = rocklee_driver_probe,
    .remove = rocklee_driver_remove,
    .shutdown = rocklee_driver_shutdown,
    .suspend = rocklee_driver_suspend,
    .resume = rocklee_driver_resume,
    .driver = {
        .name = DEVNAME"_platform",//platform_device'name  and platform_driver'name must be the same
        .owner = THIS_MODULE,
    }
};

void rocklee_device_release(struct device *dev)
{
    printk(KERN_EMERG "rocklee_device_release~~~~\n");
}

struct platform_device rocklee_device = {
    .name = DEVNAME"_platform",//platform_device'name  and platform_driver'name must be the same
    .id = -1,
    .dev.release = rocklee_device_release,
};


static int rocklee_init(void)
{
    int ret;
    
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    ret = platform_driver_register(&rocklee_driver);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d failed(%d)...\n",__func__,__LINE__, ret);
        return ret;
    }
    ret = platform_device_register(&rocklee_device);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d failed...\n",__func__,__LINE__, ret);
        return ret;
    }
    return 0;
}

static void rocklee_exit(void)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    platform_driver_unregister(&rocklee_driver);
    platform_device_unregister(&rocklee_device);
}

module_init(rocklee_init);
module_exit(rocklee_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rocklee");

