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
cat /sys/devices/virtual/rocklee_cdev_class/rocklee_cdev0/rocklee_dbg_0
cat /sys/devices/virtual/rocklee_cdev_class/rocklee_cdev0/rocklee_dbg_1
sudo insmod rocklee_kernel_rbtree.ko
sudo rmmod rocklee_kernel_rbtree.ko
sudo dmesg -c
#endif 

struct test_node {
    u32 key;
    struct rb_node rb;

    /* following fields used for testing augmented rbtree functionality */
    u32 val;
    u32 augmented;
};
///////////////////////////////display brtree////////////////////////////////////////////
#include <linux/export.h>
#include <linux/math.h>
#include <linux/types.h>

#define BUFFER_ROW 20
#define BUFFER_COL 2000

unsigned char g_disp_buffer[BUFFER_ROW][BUFFER_COL] = {0};

#define SUCCESS (0x0)
#define FAIL    (0x1)
#define EMPTY   (0x2)
#define FULL    (0x3)
#define NORMAL  (0x4)
#define FIFO_DATA_LEN (0x1000)
//BFS遍历的时候，使用占位法(也就是某个子节点不存在，推栈的时候，用空指针占一个位置，
//这样可以通过栈元素的编号就可以知道这个元素是哪个节点的左还是右孩子，甚至可以使用与多子节点的非二叉树)
#define MAX_SUBNODE (2)

typedef struct _FIFO {
    unsigned long data[FIFO_DATA_LEN];
    unsigned int rIndex;
    unsigned int wIndex;
    unsigned int status;
} FIFO;

int FifoInit(FIFO *pFifo)
{
    if (NULL == pFifo) {
        return FAIL;
    }

    (void)memset(pFifo, 0, sizeof(FIFO));
    return SUCCESS;
}

int FifoPush(FIFO *pFifo, unsigned long data)
{
    if (NULL == pFifo) {
        return FAIL;
    }

    if (FULL != pFifo->status) {
        pFifo->data[pFifo->wIndex++] = data;
        pFifo->wIndex %= FIFO_DATA_LEN;
        if (pFifo->wIndex == pFifo->rIndex) {
            pFifo->status = FULL;  
        } else {
            pFifo->status = NORMAL;
        }
    } else {
        return FAIL;
    }

    return SUCCESS;
}

int FifoPop(FIFO *pFifo, unsigned long *pData)
{
    if (NULL == pFifo) {
        return FAIL;
    }

    if (EMPTY != pFifo->status) {
        *pData=pFifo->data[pFifo->rIndex++];
        pFifo->rIndex %= FIFO_DATA_LEN;  
        if (pFifo->wIndex == pFifo->rIndex) {
            pFifo->status = EMPTY;  
        } else {
           pFifo->status = NORMAL;
        }
    } else {
        return FAIL;
    }

    return SUCCESS;
}

int IsFifoEmpty(FIFO *pFifo)
{
    if (NULL == pFifo) {
        return EMPTY;
    }	
    return pFifo->status;
}

//using BFS with FIFO for displaying rbtree
void show_rbtree(struct rb_root_cached *root)
{
    unsigned long level;  //标识是树的第几层，层数从1开始
	unsigned long index;//这是该层的第几号节点，空节点为了打印方便，也会占位，所以打印的时候通过占位法，变为一棵满二叉树 
    float test; //验证下除法的实现
	unsigned long node_count = 0;
	int ret;
    FIFO rbtree_fifo;
    struct rb_node *root_node;
    struct rb_node *cur_process_node;
	
    if (root == NULL) {
        printk(KERN_ALERT "root null\n");
        return;
    }

    root_node = root->rb_root.rb_node;
    if (root_node == NULL) {
        printk(KERN_ALERT "rb_root null\n");
        return;
    }
#if 0
    node_count ++;
    node_count +=3;
    test = node_count / 3;
    printk(KERN_ALERT "\n test ======>%e \n", test);
    printk(KERN_ALERT "\n test ======>%d \n", test);
    printk(KERN_ALERT "\n test ======>%u \n", (unsigned int)test);
    return;
#endif 
    FifoInit(&rbtree_fifo);
    ret = FifoPush(&rbtree_fifo, (unsigned long)root_node);
    if (ret != SUCCESS) {
        printk(KERN_ALERT "FifoPush faile:%d\n", ret);
        return;
    }
	
    while (IsFifoEmpty(&rbtree_fifo) != EMPTY) {
		ret = FifoPop(&rbtree_fifo, (unsigned long *)cur_process_node);
        if (ret != SUCCESS) {
            printk(KERN_ALERT "FifoPop faile:%d\n", ret);
            return;
        }
		
		node_count++;
		
		if (cur_process_node == NULL) {
			continue;
		}
		
		level = int_sqrt(node_count + 1); //层编号从1开始, 第n层，节点的总个数: node_count = 2的n次方 + 1 
		index = node_count - int_pow(2, level - 1);//每一层的节点编号从0开始 
		sprintf(g_disp_buffer[level][index * 10], "%10u", rb_entry(cur_process_node, struct test_node, rb)->key);
		
        if (cur_process_node->rb_left != NULL) {
            FifoPush(&rbtree_fifo, (unsigned long)cur_process_node->rb_left);
        } else {
            FifoPush(&rbtree_fifo, (unsigned long)NULL);
        }
        if (cur_process_node->rb_right != NULL) {
            FifoPush(&rbtree_fifo, (unsigned long)cur_process_node->rb_right);
        } else {
            FifoPush(&rbtree_fifo, (unsigned long)NULL);
        }
    }
    return;
}

/////////////////////////参考代码:https://elixir.bootlin.com/linux/latest/source/lib/rbtree_test.c////////////////////////////////////////

#define __param(type, name, init, msg)        \
    static type name = init;        \
    module_param(name, type, 0444);        \
    MODULE_PARM_DESC(name, msg);

__param(int, nnodes, 100, "Number of nodes in the rb-tree");
__param(int, perf_loops, 1000, "Number of iterations modifying the rb-tree");
__param(int, check_loops, 100, "Number of iterations modifying and verifying the rb-tree");

static struct rb_root_cached root = RB_ROOT_CACHED;
static struct test_node *nodes = NULL;

static struct rnd_state rnd;

static void insert(struct test_node *node, struct rb_root_cached *root)
{
    struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
    u32 key = node->key;

    while (*new) {
        parent = *new;
        if (key < rb_entry(parent, struct test_node, rb)->key)
            new = &parent->rb_left;
        else
            new = &parent->rb_right;
    }

    rb_link_node(&node->rb, parent, new);
    rb_insert_color(&node->rb, &root->rb_root);
}

static void insert_cached(struct test_node *node, struct rb_root_cached *root)
{
    struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
    u32 key = node->key;
    bool leftmost = true;

    while (*new) {
        parent = *new;
        if (key < rb_entry(parent, struct test_node, rb)->key)
            new = &parent->rb_left;
        else {
            new = &parent->rb_right;
            leftmost = false;
        }
    }

    rb_link_node(&node->rb, parent, new);
    rb_insert_color_cached(&node->rb, root, leftmost);
}

static inline void erase(struct test_node *node, struct rb_root_cached *root)
{
    rb_erase(&node->rb, &root->rb_root);
}

static inline void erase_cached(struct test_node *node, struct rb_root_cached *root)
{
    rb_erase_cached(&node->rb, root);
}


#define NODE_VAL(node) ((node)->val)

RB_DECLARE_CALLBACKS_MAX(static, augment_callbacks,
             struct test_node, rb, u32, augmented, NODE_VAL)

static void insert_augmented(struct test_node *node,
                 struct rb_root_cached *root)
{
    struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
    u32 key = node->key;
    u32 val = node->val;
    struct test_node *parent;

    while (*new) {
        rb_parent = *new;
        parent = rb_entry(rb_parent, struct test_node, rb);
        if (parent->augmented < val)
            parent->augmented = val;
        if (key < parent->key)
            new = &parent->rb.rb_left;
        else
            new = &parent->rb.rb_right;
    }

    node->augmented = val;
    rb_link_node(&node->rb, rb_parent, new);
    rb_insert_augmented(&node->rb, &root->rb_root, &augment_callbacks);
}

static void insert_augmented_cached(struct test_node *node,
                    struct rb_root_cached *root)
{
    struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
    u32 key = node->key;
    u32 val = node->val;
    struct test_node *parent;
    bool leftmost = true;

    while (*new) {
        rb_parent = *new;
        parent = rb_entry(rb_parent, struct test_node, rb);
        if (parent->augmented < val)
            parent->augmented = val;
        if (key < parent->key)
            new = &parent->rb.rb_left;
        else {
            new = &parent->rb.rb_right;
            leftmost = false;
        }
    }

    node->augmented = val;
    rb_link_node(&node->rb, rb_parent, new);
    rb_insert_augmented_cached(&node->rb, root,
                   leftmost, &augment_callbacks);
}


static void erase_augmented(struct test_node *node, struct rb_root_cached *root)
{
    rb_erase_augmented(&node->rb, &root->rb_root, &augment_callbacks);
}

static void erase_augmented_cached(struct test_node *node,
                   struct rb_root_cached *root)
{
    rb_erase_augmented_cached(&node->rb, root, &augment_callbacks);
}

static void init(void)
{
    int i;
    for (i = 0; i < nnodes; i++) {
        nodes[i].key = prandom_u32_state(&rnd);
        nodes[i].val = prandom_u32_state(&rnd);
    }
}

static bool is_red(struct rb_node *rb)
{
    return !(rb->__rb_parent_color & 1);
}

static int black_path_count(struct rb_node *rb)
{
    int count;
    for (count = 0; rb; rb = rb_parent(rb))
        count += !is_red(rb);
    return count;
}

static void check_postorder_foreach(int nr_nodes)
{
    struct test_node *cur, *n;
    int count = 0;
    rbtree_postorder_for_each_entry_safe(cur, n, &root.rb_root, rb)
        count++;

    WARN_ON_ONCE(count != nr_nodes);
}

static void check_postorder(int nr_nodes)
{
    struct rb_node *rb;
    int count = 0;
    for (rb = rb_first_postorder(&root.rb_root); rb; rb = rb_next_postorder(rb))
        count++;

    WARN_ON_ONCE(count != nr_nodes);
}

static void check(int nr_nodes)
{
    struct rb_node *rb;
    int count = 0, blacks = 0;
    u32 prev_key = 0;

    for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
        struct test_node *node = rb_entry(rb, struct test_node, rb);
        WARN_ON_ONCE(node->key < prev_key);
        WARN_ON_ONCE(is_red(rb) &&
                 (!rb_parent(rb) || is_red(rb_parent(rb))));
        if (!count)
            blacks = black_path_count(rb);
        else
            WARN_ON_ONCE((!rb->rb_left || !rb->rb_right) &&
                     blacks != black_path_count(rb));
        prev_key = node->key;
        count++;
    }

    WARN_ON_ONCE(count != nr_nodes);
    WARN_ON_ONCE(count < (1 << black_path_count(rb_last(&root.rb_root))) - 1);

    check_postorder(nr_nodes);
    check_postorder_foreach(nr_nodes);
}

static void check_augmented(int nr_nodes)
{
    struct rb_node *rb;

    check(nr_nodes);
    for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
        struct test_node *node = rb_entry(rb, struct test_node, rb);
        u32 subtree, max = node->val;
        if (node->rb.rb_left) {
            subtree = rb_entry(node->rb.rb_left, struct test_node,
                       rb)->augmented;
            if (max < subtree)
                max = subtree;
        }
        if (node->rb.rb_right) {
            subtree = rb_entry(node->rb.rb_right, struct test_node,
                       rb)->augmented;
            if (max < subtree)
                max = subtree;
        }
        WARN_ON_ONCE(node->augmented != max);
    }
}

static int rbtree_test_init(void)
{
    int i, j;
    cycles_t time1, time2, time;
    struct rb_node *node;

    nodes = kmalloc_array(nnodes, sizeof(*nodes), GFP_KERNEL);
    if (!nodes)
        return -ENOMEM;

    printk(KERN_ALERT "rbtree testing");

    prandom_seed_state(&rnd, 3141592653589793238ULL);
    init();

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++) {
        for (j = 0; j < nnodes; j++)
            insert(nodes + j, &root);
        for (j = 0; j < nnodes; j++)
            erase(nodes + j, &root);
    }

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk(" -> test 1 (latency of nnodes insert+delete): %llu cycles\n",
           (unsigned long long)time);

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++) {
        for (j = 0; j < nnodes; j++)
            insert_cached(nodes + j, &root);
        for (j = 0; j < nnodes; j++)
            erase_cached(nodes + j, &root);
    }

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk(" -> test 2 (latency of nnodes cached insert+delete): %llu cycles\n",
           (unsigned long long)time);

    for (i = 0; i < nnodes; i++)
        insert(nodes + i, &root);

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++) {
        for (node = rb_first(&root.rb_root); node; node = rb_next(node))
            ;
    }

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk(" -> test 3 (latency of inorder traversal): %llu cycles\n",
           (unsigned long long)time);

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++)
        node = rb_first(&root.rb_root);

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk(" -> test 4 (latency to fetch first node)\n");
    printk("        non-cached: %llu cycles\n", (unsigned long long)time);

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++)
        node = rb_first_cached(&root);

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk("        cached: %llu cycles\n", (unsigned long long)time);

    for (i = 0; i < nnodes; i++)
        erase(nodes + i, &root);

    /* run checks */
    for (i = 0; i < check_loops; i++) {
        init();
        for (j = 0; j < nnodes; j++) {
            check(j);
            insert(nodes + j, &root);
        }
        for (j = 0; j < nnodes; j++) {
            check(nnodes - j);
            erase(nodes + j, &root);
        }
        check(0);
    }

    printk(KERN_ALERT "augmented rbtree testing");

    init();

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++) {
        for (j = 0; j < nnodes; j++)
            insert_augmented(nodes + j, &root);
        for (j = 0; j < nnodes; j++)
            erase_augmented(nodes + j, &root);
    }

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk(" -> test 1 (latency of nnodes insert+delete): %llu cycles\n", (unsigned long long)time);

    time1 = get_cycles();

    for (i = 0; i < perf_loops; i++) {
        for (j = 0; j < nnodes; j++)
            insert_augmented_cached(nodes + j, &root);
        for (j = 0; j < nnodes; j++)
            erase_augmented_cached(nodes + j, &root);
    }

    time2 = get_cycles();
    time = time2 - time1;

    time = div_u64(time, perf_loops);
    printk(" -> test 2 (latency of nnodes cached insert+delete): %llu cycles\n", (unsigned long long)time);

    for (i = 0; i < check_loops; i++) {
        init();
        for (j = 0; j < nnodes; j++) {
            check_augmented(j);
            insert_augmented(nodes + j, &root);
        }
        for (j = 0; j < nnodes; j++) {
            check_augmented(nnodes - j);
            erase_augmented(nodes + j, &root);
        }
        check_augmented(0);
    }

    kfree(nodes);

    return -EAGAIN; /* Fail will directly unload the module */
}

static int rbtree_test_print(void)
{
    int  j;
    cycles_t time1, time2, time;

    nodes = kmalloc_array(nnodes, sizeof(*nodes), GFP_KERNEL);
    if (!nodes)
        return -ENOMEM;

    printk(KERN_ALERT "rbtree testing");

    prandom_seed_state(&rnd, 3141592653589793238ULL);
    init();

    for (j = 0; j < nnodes; j++)
        insert(nodes + j, &root);
		
    time1 = get_cycles();
		
    show_rbtree(&root);

    time2 = get_cycles();
		
    for (j = 0; j < nnodes; j++)
        erase(nodes + j, &root);

    time = time2 - time1;
    time = div_u64(time, perf_loops);
    printk(" -> test 1 (print rbtree): %llu cycles\n",
           (unsigned long long)time);

    kfree(nodes);

    return -EAGAIN; /* Fail will directly unload the module */
}


////////////////////////////////////////////////////////////////

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
    
    rbtree_test_init();

    if (0 == strcmp(attr->attr.name,"rocklee_dbg_0")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    else  if (0 == strcmp(attr->attr.name,"rocklee_dbg_1")) {
        printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    }
    return 0;
}

static ssize_t rocklee_show_1(struct device *pdevice, struct device_attribute *attr, char *buf)
{
    printk(KERN_EMERG "Fn:%s Ln:%d ...\n",__func__,__LINE__);
    
    rbtree_test_print();

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


///sys/devices/virtual/rocklee_cdev_class/rocklee_cdev0/rocklee_dbg_0
///sys/devices/virtual/rocklee_cdev_class/rocklee_cdev0/rocklee_dbg_1
static struct device_attribute  rocklee_sysfs_attribute[] = {
    __ATTR(rocklee_dbg_0, 0664, rocklee_show, rocklee_store),
    __ATTR(rocklee_dbg_1, 0664, rocklee_show_1, rocklee_store),
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
    unregister_chrdev_region(devnum, dev_count);
ERR_STEP:
    cdev_del(dev_obj);
    printk(KERN_EMERG "Fn:%s Ln:%d  failed...\n",__func__,__LINE__);
    return ret;   
}

static void rocklee_exit(void)
{
    int i;
    int j;
        
    printk(KERN_EMERG "Fn:%s Ln:%d...\n",__func__,__LINE__);
    
    //free_irq(36, xxx);
    
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
    
    return;
}

module_init(rocklee_init);
module_exit(rocklee_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rocklee");

