#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

static unsigned int variable=0x12345678;
static struct proc_dir_entry  *test_entry;

static int test_proc_show(struct seq_file *seq, void *v)
{
	unsigned int *ptr_var = seq->private;
	seq_printf(seq, "variable:0x%x\n", *ptr_var);
	seq_printf(seq, "variable_address:0x%p\n", ptr_var);
	return 0;
}

static int test_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, test_proc_show, PDE_DATA(inode));
}

#if 0
//for 4.x kernel 
static const struct file_operations test_proc_fops =
{
	.owner = THIS_MODULE,
	.open = test_proc_open,
	.read = seq_read, //在需要创建一个由一系列数据顺序组合而成的虚拟文件或一个较大的虚拟文件时，推荐使用seq_file接口
	.llseek = seq_lseek,
	.release = single_release,
};

//cat /proc/stolen_data  ==> show variable and variable;s address
static __init int test_proc_init(void)
{
	printk("variable addr:0x%p\n", &variable);
	printk("variable     :0x%x\n", variable);
	test_entry = proc_create_data("stolen_data",0444, NULL, &test_proc_fops, &variable);
	if (test_entry)
		return 0;

	return -ENOMEM;
}

#endif 

//for 5.x kernel 
static const struct proc_ops test_proc_fops =
{
	.proc_open = test_proc_open,
	.proc_read = seq_read,//在需要创建一个由一系列数据顺序组合而成的虚拟文件或一个较大的虚拟文件时，推荐使用seq_file接口
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

//cat /proc/stolen_data  ==> show variable and variable;s address
static __init int test_proc_init(void)
{
	printk("variable addr:0x%p\n", &variable);
	printk("variable     :0x%x\n", variable);
	test_entry = proc_create_data("stolen_data",0444, NULL, &test_proc_fops, &variable);
	if (test_entry)
		return 0;

	return -ENOMEM;
}

module_init(test_proc_init);

static __exit void test_proc_cleanup(void)
{
	remove_proc_entry("stolen_data", NULL);
}
module_exit(test_proc_cleanup);

MODULE_AUTHOR("Barry Song <baohua@kernel.org>");
MODULE_DESCRIPTION("proc exmaple");
MODULE_LICENSE("GPL v2");
