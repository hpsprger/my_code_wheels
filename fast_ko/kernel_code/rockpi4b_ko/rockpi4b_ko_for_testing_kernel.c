#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/rbtree_augmented.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <asm/timex.h>

#if 0
//测试命令
cat /sys/devices/virtual/rockllee_cdev_class/rockllee_cdev0/rockllee_dbg_0
cat /sys/devices/virtual/rockllee_cdev_class/rockllee_cdev0/rockllee_dbg_1
sudo insmod rockpi4b_ko_for_testing_kernel.ko
sudo rmmod rockpi4b_ko_for_testing_kernel.ko
sudo dmesg -c
#endif 

#define DEV_NUMS (1)

static int major = 0;
static int minor = 0;
const  int dev_count = DEV_NUMS;

static struct class *cls = NULL;
static struct cdev  *dev_obj = NULL;
static struct device *devp[DEV_NUMS] = {NULL};

#define DEVNAME "rockllee_cdev"

static irqreturn_t rockllee_interrupt(int irq, void *dev_id)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return IRQ_HANDLED;
}

static ssize_t rockllee_show(struct device *pdevice, struct device_attribute *attr, char *buf)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    rbtree_test_init();

    if (0 == strcmp(attr->attr.name,"rockllee_dbg_0")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    else  if (0 == strcmp(attr->attr.name,"rockllee_dbg_1")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    return 0;
}

static ssize_t rockllee_show_1(struct device *pdevice, struct device_attribute *attr, char *buf)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    rbtree_test_print();

    if (0 == strcmp(attr->attr.name,"rockllee_dbg_0")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    else  if (0 == strcmp(attr->attr.name,"rockllee_dbg_1")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    return 0;
}

static ssize_t rockllee_store(struct device *pdevice, struct device_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    if (0 == strcmp(attr->attr.name,"rockllee_dbg_0")) {

    } else if (0 == strcmp(attr->attr.name,"rockllee_dbg_1")) {

    }
    
    return count;
}

///sys/devices/virtual/rockllee_cdev_class/rockllee_cdev0/rockllee_dbg_0
///sys/devices/virtual/rockllee_cdev_class/rockllee_cdev0/rockllee_dbg_1
static struct device_attribute  rockllee_sysfs_attribute[] = {
    __ATTR(rockllee_dbg_0, 0664, rockllee_show, rockllee_store),
    __ATTR(rockllee_dbg_1, 0664, rockllee_show_1, rockllee_store),
};

static long rockllee_fops_ioctl( struct file *files, unsigned int cmd, unsigned long arg)
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

static int rockllee_fops_release(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return 0;
}

static int rockllee_fops_open(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    return 0;
}

static long rockllee_fops_write(struct file * pfile, const char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "Fn:%s Ln:%d buffer:%s  count:%ld ...\n",__func__,__LINE__, buffer, count);
    return 0;
}

static ssize_t rockllee_fops_read(struct file * pfile, char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "Fn:%s Ln:%d buffer:%s  count:%ld ...\n",__func__,__LINE__, buffer, count);
    return 0;
}

static struct file_operations rockllee_fops = {
    .owner = THIS_MODULE, 
    .open = rockllee_fops_open,
    .release = rockllee_fops_release,
    .write = rockllee_fops_write,
    .read = rockllee_fops_read,
    .unlocked_ioctl = rockllee_fops_ioctl,
};

static int rockllee_init(void)
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
    cdev_init(dev_obj, &rockllee_fops);
    ret = alloc_chrdev_region(&devnum, minor, dev_count, DEVNAME);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        goto ERR_STEP;
    }
    major = MAJOR(devnum);
    
    //step3 register cdev obj 
    ret = cdev_add(dev_obj, devnum, dev_count);
    if (ret) {
        printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
        goto ERR_STEP1;
    }
    
    cls = class_create(THIS_MODULE, DEVNAME"_class");
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
        for(j = 0; j < ARRAY_SIZE(rockllee_sysfs_attribute); j++) {
            ret = device_create_file(devp[i], &rockllee_sysfs_attribute[j]); //add sysfs entry
            if (ret) {
                printk(KERN_EMERG"add sysfs entry[%d][%d]  failed, ret = %d\n", i, j, ret);
                goto ERR_STEP3;
            }
        }
        
        //ret = request_irq(36, rockllee_interrupt, IRQ_TYPE_EDGE_FALLING, DEVNAME"_intrupt", devp[i]);
        //if (ret < 0) {
        //    printk("Request IRQ %d failed, %d\n", i, ret);
        //    goto ERR_STEP4;
        //}
    }

    return 0;
ERR_STEP4:
    for (i = 0; i < dev_count; i++) {
        for(j = 0; j < ARRAY_SIZE(rockllee_sysfs_attribute); j++) {
            //rm sysfs entry
            device_remove_file(devp[i], &rockllee_sysfs_attribute[j]);
        }
    }
ERR_STEP3:
    for (i = 0; i <= minor; i++) {
        device_destroy(cls, MKDEV(major, i));
    }
ERR_STEP2:
    class_destroy(cls);
ERR_STEP1:
    unregister_chrdev_region(devnum, dev_count);
ERR_STEP:
    cdev_del(dev_obj);
    printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
    return ret;   
}

static void rockllee_exit(void)
{
    int i;
    int j;
        
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    
    //free_irq(36, xxx);
    
    for (i = 0; i < dev_count; i++) {
        for(j = 0; j < ARRAY_SIZE(rockllee_sysfs_attribute); j++) {
            //rm sysfs entry
            device_remove_file(devp[i], &rockllee_sysfs_attribute[j]);
        }
    }
    
    for (i = minor; i < (minor + dev_count); i++) {
        device_destroy(cls, MKDEV(major, i));
    }
    
    class_destroy(cls);
    unregister_chrdev_region(MKDEV(major, minor), dev_count);
    cdev_del(dev_obj);
    
    return;
}

module_init(rockllee_init);
module_exit(rockllee_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rockllee");

