#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>


#define DEV_NUMS (1)

static int major = 0;
static int minor = 0;
const  int dev_count = DEV_NUMS;

static struct class *cls = NULL;
static struct cdev  *dev_obj = NULL;
static struct device *devp[DEV_NUMS] = {NULL};

#define DEVNAME "rocklee_cdev"

static irqreturn_t KEY_interrupt(int irq, void *dev_id)
{
    printk("%s(%d)\n", __FUNCTION__, __LINE__);
    return IRQ_HANDLED;
}

static ssize_t leds_show(struct device * pdevice,struct device_attribute *attr,const char *buf,size_t count)
{
    struct rocklee_leds_device  *pDev = NULL;
    pDev = (struct rocklee_leds_device  *)(pdevice->platform_data);

    if (0 == strcmp(attr->attr.name,"ctl_led0"))
    {
        printk(KERN_EMERG"led 0: %d\n",gpio_get_value(Leds[0]));
    }
    else  if (0 == strcmp(attr->attr.name,"ctl_led1"))
    {
        printk(KERN_EMERG"led 1: %d\n",gpio_get_value(Leds[1]));
    }
    return 0;
}

static struct device_attribute  rocklee_sysfs_attribute[] = {
    __ATTR(ctl_led0,0777,leds_show,ctl_leds),
    __ATTR(ctl_led1,0777,leds_show,ctl_leds),
};

static long rocklee_fops_ioctl( struct file *files, unsigned int cmd, unsigned long arg)
{
    printk(KERN_EMERG "Fn:%s Ln:%d  cmd is %d, arg is %d ...\n",__func__,__LINE__, cmd, arg);
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

static int rocklee_fops_write(struct file * pfile, char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "Fn:%s Ln:%d buffer:%s  count:%d ...\n",__func__,__LINE__, buffer, count);
    return 0;
}

static int rocklee_fops_read(struct file * pfile, char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "Fn:%s Ln:%d buffer:%s  count:%d ...\n",__func__,__LINE__, buffer, count);
    return 0;
}

static struct file_operations rocklee_fops = {
    .owner = THIS_MODULE, 
    .open = rocklee_fops_open,
    .release = rocklee_fops_release,
    .write = rocklee_fops_write,
    .read = rocklee_fops_write,  
    .unlocked_ioctl = rocklee_fops_ioctl,
};

static int rocklee_init(void)
{
    int ret;
    int i;
    int j;
    dev_t devnum;
        
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    //step1 alloc cdev obj 
    dev_obj  = cdev_alloc();
    if (dev_obj == NULL) {
    	printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        return -ENOMEM;
    }
    //step2 init cdev obj 
    cdev_init(dev_obj, &rocklee_fops);
    ret = alloc_chrdev_region(&devnum, minor, dev_count, DEVNAME);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        goto ERR_STEP;
    }
    major = MAJOR(devnum)
    
    //step3 register cdev obj 
    ret = cdev_add(dev_obj, devnum, dev_count);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        goto ERR_STEP1;
    }
    
    cls = class_create(THIS_MODULE, DEVNAME);
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        goto ERR_STEP1;
    }

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
    }
    
    return 0;
ERR_STEP3:
    for (i = 0; i < dev_count; i++) {
        for(j = 0; j < ARRAY_SIZE(rocklee_leds_attribute); j++) {
            device_remove_file(devp[i], &rocklee_sysfs_attribute[j]); //rm sysfs entry
        }
    }
ERR_STEP2:
    for (--i; i >= minor; i--) {
        device_destroy(cls, MKDEV(major, i));
    }    
    class_destroy(cls);
ERR_STEP1:
    unregister_chrdev_region(devnum, dev_count);
ERR_STEP:
    cdev_del(dev_obj);
    printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
    return ret;   
}

static void rocklee_exit(void)
{
    int i;
    
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    
    for (i = 0; i < dev_count; i++) {
        for(j = 0; j < ARRAY_SIZE(rocklee_leds_attribute); j++) {
            device_remove_file(devp[i], &rocklee_sysfs_attribute[j]); //rm sysfs entry
        }
    }
    
    for (i = minor; i < (minor + dev_count); i++) {
        device_destroy(cls, MKDEV(major, i));
    }
    
    class_destroy(cls);
    unregister_chrdev_region(devnum, dev_count);
    cdev_del(dev_obj);
    
    return;
}

module_init(rocklee_init);
module_exit(rocklee_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rocklee");

