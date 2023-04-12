#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

static unsigned int variable=0x12345678;
static struct proc_dir_entry  *proc_entry;

static int rockllee_proc_show(struct seq_file *seq, void *v)
{
	unsigned int *ptr_var = seq->private;
	printk("rockllee_proc_show ....\n");
	seq_printf(seq, "variable:0x%x\n", *ptr_var);         // 内容打印到了seq_file 对应的 buffer中了  ==> 用户态中读取这个proc文件的时候，每次都会首先要打开文件的，所以这里都会准备好数据
	seq_printf(seq, "variable_address:0x%p\n", ptr_var);  // 同上
	return 0;
}

static int rockllee_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, rockllee_proc_show, PDE_DATA(inode)); // 打开文件的时候，为该文件绑定对于的show函数
}

ssize_t	rockllee_proc_write(struct file *file, const char *buffer, size_t len, loff_t *pos)
{
	printk("rockllee_proc_write ....len=%d pos=0x%x data=0x%x \n", len, pos, buffer[0]);
	return len;
}

// seq_file系列函数是为了方便内核导出信息到 sysfs、debugfs、procfs 实现的
// 以往的内核也存在各种形式的实现，但是都无法避免个别实现会产生一些漏洞，
// 这一套函数实现可以让内核导出信息更加简单和统一稳定，维护起来更加方便。
// 它包含了如下一些接口: 
// int     seq_open(struct file *file, const struct seq_operations *op);
// int     seq_release(struct inode *inode, struct file *file);
// ssize_t seq_read(struct file *file, char __user *buf, size_t size, loff_t *ppos);
// int     seq_write(struct seq_file *seq, const void *data, size_t len);
// loff_t  seq_lseek(struct file *file, loff_t offset, int whence)；
// 一般我们需要创建一个 file_operations 结构体去实现不同的 debugfs、 procfs。而以上函数就是用于快速实现这个操作结构体的

//for 5.x kernel 
static const struct proc_ops rockllee_proc_fops =
{
	.proc_open = rockllee_proc_open,
	.proc_read = seq_read,    //读这个文件的buffer里面的内容 到用户空间进行显示
	.proc_write = rockllee_proc_write,  //写用户空间的内容数据到这个文件的buffer中去 
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

//cat /proc/rockllee_test  ==> show variable and variable;s address
static __init int rockllee_proc_init(void)
{
	// 创建 proc接口   /proc/rockllee_test 
	proc_entry = proc_create_data("rockllee_test",0444, NULL, &rockllee_proc_fops, &variable); //&variable 私有数据的指针,如不要为NULL
	if (proc_entry)
		return 0;

	return -ENOMEM;
}

static __exit void rockllee_proc_cleanup(void)
{
	remove_proc_entry("rockllee_test", NULL);
}

module_init(rockllee_proc_init);
module_exit(rockllee_proc_cleanup);

MODULE_AUTHOR("rockllee");
MODULE_DESCRIPTION("proc exmaple");
MODULE_LICENSE("GPL v2");