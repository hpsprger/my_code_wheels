#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>

#define DRIVER_NAME "gpio_lq"
#define DEVICE_NAME "gpio_lq"
#define DEV_NODE_NAME "rocklee_node"

#define KEY_HOME            EXYNOS4_GPX1(1)
#define NUM_INT_KEY_HOME    IRQ_EINT(9)

typedef  irqreturn_t (*pFunc)(int irq, void *dev_id);

static irqreturn_t eint27_KEY_SLEEP_interrupt(int irq, void *dev_id)
{
    printk("%s(%d)\n", __FUNCTION__, __LINE__);
    return IRQ_HANDLED;
}

static irqreturn_t eint17_KEY_VOLUP_interrupt(int irq, void *dev_id)
{
    printk("%s(%d)\n", __FUNCTION__, __LINE__);
    return IRQ_HANDLED;
}

static irqreturn_t eint16_KEY_VOLDOWN_interrupt(int irq, void *dev_id)
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

static struct device_attribute  rocklee_leds_attribute[] = {
    __ATTR(ctl_led0,0777,leds_show,ctl_leds),
    __ATTR(ctl_led1,0777,leds_show,ctl_leds),
};


static long rocklee_fops_ioctl( struct file *files, unsigned int cmd, unsigned long arg)
{
    printk(KERN_EMERG"cmd is %d,arg is %d\n",cmd,arg);
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
    printk(KERN_EMERG "rocklee release\n");
    return 0;
}

static int rocklee_fops_open(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG "rocklee open\n");
    return 0;
}

static int rocklee_fops_write(struct file * pfile, char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "rocklee_fops_write  buffer: %s  count=%d\n",buffer,count);
    return 0;
}

static int rocklee_fops_read(struct file * pfile, char __user * buffer, size_t count, loff_t  * ppos)
{
    printk(KERN_EMERG "rocklee_fops_read  buffer: %s  count=%d\n",buffer,count);
    return 0;
}

static struct file_operations fops_rocklee_leds = {
    .owner = THIS_MODULE,
    .open = rocklee_fops_open,
    .release = rocklee_fops_release,
    .unlocked_ioctl = rocklee_fops_ioctl,
};

static int rocklee_probe(struct platform_device *pdv)
{
    int ret,i;
    struct rocklee_leds_device  *pDev = NULL;
    dev_t devt;
    printk(KERN_EMERG "rocklee_probe  initializing~~~\n");
    ret = alloc_chrdev_region(&devt,0,1,"rocklee_leds");
    if (ret)
    {
        printk(KERN_EMERG"alloc_chrdev_region  failed, ret = %d\n", ret);
        goto exit;
    }

    printk(KERN_EMERG"MAJOR(devt)=%d  MINOR(devt)=%d \n", MAJOR(devt),MINOR(devt));
    pDev = kmalloc(sizeof(*pDev),GFP_KERNEL);

    if (NULL == pDev)
    {
        ret = -ENOMEM;
        printk(KERN_EMERG"kmalloc  failed \n");
        goto exit_class;
    }

    memset(pDev,0,sizeof(*pDev));

    rocklee_leds_class = class_create(THIS_MODULE,"rocklee_leds_class"); //create device class, ready for the next steps
    if(NULL == rocklee_leds_class)
    {
        ret = -EBUSY;
        printk(KERN_EMERG"class_create  failed \n");
        goto exit_class;
    }

    cdev_init(&pDev->cdev,&fops_rocklee_leds); //character device init  with  fops
    pDev->cdev.owner = THIS_MODULE; //character device init
    ret = cdev_add(&pDev->cdev,devt,1);//insert cdev using devt  into system
    pDev->pdevice = device_create(rocklee_leds_class,NULL,devt,pDev,"rocklee_leds_lq");//create device node belong to device class(rocklee_leds_class)
    if(NULL == pDev->pdevice)
    {
        ret = -EBUSY;
        printk(KERN_EMERG"device_create  failed \n");
        goto exit_cdev;
    }


    pDev->pdevice->platform_data = pDev;  //for passing paramter  to other function
    pdv->dev.platform_data = pDev;  //for passing paramter  to other function

    //create sysfs ==>/sys/
    for(i = 0; i < ARRAY_SIZE(rocklee_leds_attribute); i++)
    {
        ret = device_create_file(pDev->pdevice,&rocklee_leds_attribute[i]); //add sysfs entry
        if (ret)
        {
            printk(KERN_EMERG"add sysfs entry  failed, ret = %d\n", ret);
        }
    }

    for(i=0; i<LED_NUM; i++)
    {
        ret = gpio_request(Leds[i], "LED");
        if (ret) {
            printk("%s: request GPIO %d for LED failed, ret = %d\n", DRIVER_NAME,i, ret);
            goto exit_unavailable;
        }
        else{
            s3c_gpio_cfgpin(Leds[i], S3C_GPIO_OUTPUT);
            gpio_set_value(Leds[i], 1);
        }

    }

    for(i=0; i<GPIO_NUM; i++)
    {
        ret = gpio_request(rocklee[i], "GPIO");
        if (ret) {
            printk("%s: request GPIO %d for GPIO failed, ret = %d\n", DRIVER_NAME,i, ret);
            goto exit_unavailable;
        }
        else{
               s3c_gpio_cfgpin(rocklee[i], S3C_GPIO_SFN(0xF));
               s3c_gpio_setpull(rocklee[i], S3C_GPIO_PULL_UP);
               ret = request_irq(Num_Int_rocklee[i], pIntFunction[i],IRQ_TYPE_EDGE_FALLING /*IRQF_TRIGGER_FALLING*/, IntNameString[i], pdv);
               if (ret < 0) {
                   printk("Request IRQ %d failed, %d\n", Num_Int_rocklee[i], ret);
                   goto exit_unavailable;
               }
        }
    }
    return 0;
exit_unavailable:
    device_destroy(rocklee_leds_class,pDev->pdevice->devt);
exit_cdev:
    cdev_del(&pDev->cdev);
exit_class:
    unregister_chrdev_region(pDev->pdevice->devt,1);
exit:
    return ret;
}

static int rocklee_remove(struct platform_device *pdv)
{
    int i;
    struct rocklee_leds_device  *pDev = NULL;
    pDev = (struct rocklee_leds_device  *)(pdv->dev.platform_data);
    printk(KERN_EMERG "\tremove\n");

    gpio_free(rocklee[i]);
    free_irq(Num_Int_rocklee[i],pdv);

    for(i = 0; i < ARRAY_SIZE(rocklee_leds_attribute); i++)
    {
        device_remove_file(pDev->pdevice,&rocklee_leds_attribute[i]); //rm sysfs entry
    }

    device_destroy(rocklee_leds_class,pDev->pdevice->devt);
    class_destroy(rocklee_leds_class);
    unregister_chrdev_region(pDev->pdevice->devt,1);
    cdev_del(&pDev->cdev);
    memset(pDev,0,sizeof(*pDev));
    kfree(pDev);
    return 0;
}


static void rocklee_shutdown(struct platform_device *pdv)
{
    ;
}

static int rocklee_suspend(struct platform_device *pdv,pm_message_t pmt)
{
    return 0;
}


static int rocklee_resume(struct platform_device *pdv)
{
    return 0;
}


struct platform_driver rocklee_driver = {
    .probe = rocklee_probe,
    .remove = rocklee_remove,
    .shutdown = rocklee_shutdown,
    .suspend = rocklee_suspend,
    .resume = rocklee_resume,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    }
};




void    rocklee_device_release(struct device *dev)
{
    printk(KERN_EMERG "rocklee_device_release~~~~\n");
}



struct platform_device rocklee_device = {
    .name = DEVICE_NAME,
    .id = -1,
    .dev.release = rocklee_device_release,
};


static int rocklee_init(void)
{
    int DriverState;
    int DeviceState;
    int ret;
    
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    //第一种方式:ko init的时候就创建好 字符设备等
    //第二种方式:平台驱动设备模型，probe的时候再创建字符设备等
    
    //第一种方式
    //step1 alloc cdev obj 
    dev_obj  = 
    
    DriverState = platform_driver_register(&rocklee_driver);
    printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
    DeviceState = platform_device_register(&rocklee_device);
    printk(KERN_EMERG "\tDeviceState is %d\n",DeviceState);
    return 0;
}



static void rocklee_exit(void)
{
    printk(KERN_EMERG "rocklee ko exit~~~~\n");
    platform_driver_unregister(&rocklee_driver);
    platform_device_unregister(&rocklee_device);
}


module_init(rocklee_init);
module_exit(rocklee_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rocklee");

