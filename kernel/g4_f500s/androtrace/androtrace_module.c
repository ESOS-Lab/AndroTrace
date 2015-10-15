#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/mutex.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/input.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include "androtrace_module.h"

/*
****************************************************************
@date:   2014-04-05
@author: ryoung 
@desc:   [androtrace] collect real-time io trace, file life-span 
****************************************************************
*/

#define DRIVER_NAME "androtrace_driver"
#define DEVICE_NAME "androtrace_device"

/* char class device*/
struct class* androtrace_class = NULL;
static struct cdev ctrl_dev; 
static int major_num;

/* io trace*/
struct cbuf *io_buf_head = NULL;
struct cbuf *io_buf_hand = NULL;
DEFINE_RAW_SPINLOCK(io_buf_lock);

/*file life-span*/
struct cbuf *fls_buf_head = NULL;
struct cbuf *fls_buf_hand = NULL;
DEFINE_RAW_SPINLOCK(fls_buf_lock);

/*wait_queue for user daemon(polling)*/
wait_queue_head_t wait_queue;

/*fsync id, transaction id = jbd.trnasaction id*/
unsigned int at_fs_id;

/*kernel module*/
static int androtrace_open(struct inode *inode, struct file *filp)
{
	unsigned long io_buf_addr[IO_BUF_COUNT];
	unsigned long filels_buf_addr[FILELS_BUF_COUNT];
	struct cbuf *buf = NULL;
	int i = 0;

	if (filp->private_data != NULL) {
		printk(KERN_INFO "[androtrace] %s() kernel module device has already opened\n", __func__);
		return 0;
	}

	printk(KERN_INFO "[androtrace] %s()\n", __func__);

	/* -- file io trace -- */
	memset(io_buf_addr, 0, IO_BUF_COUNT * sizeof(unsigned long));
	for (i = 0; i < IO_BUF_COUNT; ++i) {
		io_buf_addr[i] = (unsigned long)kzalloc(sizeof(struct cbuf), GFP_KERNEL);
	}
	for (i = 0; i < IO_BUF_COUNT; ++i) {
		buf = (struct cbuf*)io_buf_addr[i]; 
		buf->id = i+1;
		buf->full = false;
		buf->data = kzalloc(IO_BUF_SIZE, GFP_KERNEL);
		if(buf->data == NULL)
			printk(KERN_INFO "[androtrace] %s() failed to alloc io buffer\n", __func__);
		else
			memset(buf->data, 0, IO_BUF_SIZE);	
		buf->offset = 0; 
		if (i == IO_BUF_COUNT-1) //check it is final entry or not
			buf->next  = (struct cbuf*)io_buf_addr[0];		
		else
			buf->next = (struct cbuf*)io_buf_addr[i+1]; 
		io_buf_head = (struct cbuf*)io_buf_addr[0];
		io_buf_hand = (struct cbuf*)io_buf_addr[0];
	}
	filp->private_data = io_buf_head;

 	/* -- file life-span -- */
	memset(filels_buf_addr, 0, FILELS_BUF_COUNT * sizeof(unsigned long));
	for (i = 0; i < FILELS_BUF_COUNT; ++i)
		filels_buf_addr[i] = (unsigned long)kzalloc(sizeof(struct cbuf), GFP_KERNEL);
	for (i = 0; i < FILELS_BUF_COUNT; ++i) {
		buf = (struct cbuf*)filels_buf_addr[i]; 
		buf->id = i+1;
		buf->full = false;
		buf->data = kzalloc(FILELS_BUF_SIZE, GFP_KERNEL);
		if (buf->data == NULL)
			printk(KERN_INFO "[androtrace] %s() failed to alloc file life-span buffer\n", __func__);
		else
			memset(buf->data, 0, FILELS_BUF_SIZE);
		buf->offset = 0; 
		if (i == FILELS_BUF_COUNT-1) /*check it is final entry or not*/
			buf->next  = (struct cbuf*)filels_buf_addr[0];		
		else
			buf->next = (struct cbuf*)filels_buf_addr[i+1]; 		
	}
	fls_buf_head = (struct cbuf*)filels_buf_addr[0];
	fls_buf_hand = (struct cbuf*)filels_buf_addr[0];
	/* init wati_queue */
	init_waitqueue_head(&wait_queue);

	/* +ryoung 150125 (fsync id, transaction id) */
	at_fs_id = 0;
	
	return 0;
}

/*
@return value: the number of bytes put into user buffer
@buf_type: which buffer user want to read(io trace or file life-span)
*/
static ssize_t androtrace_read(struct file *file, char *user_buf, size_t buf_type, loff_t *offset)
{
	int retval = 0;	
	if (file->private_data == NULL) {
		printk(KERN_INFO "[androtrace] %s() kernel module device's data is null\n", __func__);
		return 0;
	}
	if (buf_type == IO_LOG) {
		raw_spin_lock_irq(&io_buf_lock);
		retval = io_buf_head->offset;
		if (copy_to_user(user_buf, io_buf_head->data, IO_BUF_SIZE)) 
			return -EFAULT;	      
		memset(io_buf_head->data, 0, IO_BUF_SIZE); 
		io_buf_head->offset = 0;
		io_buf_head->full = false;
		io_buf_head = io_buf_head->next;
		raw_spin_unlock_irq(&io_buf_lock);
	} else if (buf_type == FILELS_LOG) {
		raw_spin_lock_irq(&fls_buf_lock);
		retval = fls_buf_head->offset;
		if (copy_to_user(user_buf, fls_buf_head->data, FILELS_BUF_SIZE)) 
			return -EFAULT;	      
		memset(fls_buf_head->data, 0, FILELS_BUF_SIZE); 
		fls_buf_head->offset = 0;
		fls_buf_head->full = false;
		fls_buf_head = fls_buf_head->next;
		raw_spin_unlock_irq(&fls_buf_lock);
	}

	return retval;	
}

unsigned int androtrace_poll(struct file *file, poll_table *wait)
{	
	if (file->private_data == NULL) {
		printk(KERN_INFO "[androtrace] %s() kernel module device's data is null\n", __func__);
		return 0;
	}

	/* check io/file life-span head buffer is full or not */
	if (io_buf_head->full && fls_buf_head->full)
		return ALL_LOG;
	else if (io_buf_head->full)
		return IO_LOG;
	else if (fls_buf_head->full)
		return FILELS_LOG;
	
	/*io/file life-span head buffer, both are not full
	  then it waits on wait_queue */
	if (wait_event_interruptible(wait_queue, io_buf_head->full || fls_buf_head->full))
		return -ERESTARTSYS;
	/*check which head buffer became full*/
	if (io_buf_head->full && fls_buf_head->full)
		return ALL_LOG;
	else if (io_buf_head->full)
		return IO_LOG;
	else if (fls_buf_head->full)
		return FILELS_LOG;
		
	return 	POLLERR;
}

static int androtrace_release(struct inode *inode, struct file *file)
{
	struct cbuf *buf, *buf_next = NULL;
	int i = 0;

	if (file->private_data == NULL) {
		printk(KERN_INFO "[androtrace] %s() kernel module device's data is null\n", __func__);
		return 0;
	}
	
	
	/* release io buffer*/
	raw_spin_lock_irq(&io_buf_lock);
	buf = io_buf_head; 
	buf_next = buf->next;
	for (i = 0; i < IO_BUF_COUNT; ++i) {
		kfree(buf->data);
		buf_next = buf->next;
		kfree(buf);
		buf = buf_next;
	}
	io_buf_head = NULL;
	io_buf_hand = NULL;
	raw_spin_unlock_irq(&io_buf_lock);
	
	/* release file life-span buffer*/
	raw_spin_lock_irq(&fls_buf_lock);
	buf = fls_buf_head; 
	buf_next = buf->next;
	for (i = 0; i < FILELS_BUF_COUNT; ++i) {
		kfree(buf->data);
		buf_next = buf->next;
		kfree(buf);
		buf = buf_next;
	}
	fls_buf_head = NULL;
	fls_buf_hand = NULL;
	raw_spin_unlock_irq(&fls_buf_lock);
	file->private_data = NULL;
	return 0;
}

static struct file_operations ctrl_fops = {
 	.owner = THIS_MODULE,
	.open = androtrace_open,
	.read = androtrace_read,
	.poll = androtrace_poll,
	.release = androtrace_release
};

static int __init androtrace_module_init(void)
{
	dev_t dev = MKDEV(0, 0);
	int retval;
	androtrace_class = class_create(THIS_MODULE, DRIVER_NAME); 
	retval = alloc_chrdev_region(&dev, 0, 1, DRIVER_NAME);

	major_num = MAJOR(dev);
	dev = MKDEV(major_num, 0); 
	cdev_init(&ctrl_dev, &ctrl_fops);
	ctrl_dev.owner = THIS_MODULE;
	ctrl_dev.ops = &ctrl_fops;
	cdev_add(&ctrl_dev, dev, 1);

	device_create(androtrace_class, NULL, dev, NULL, DEVICE_NAME);
	return 0;
}
static void __exit androtrace_module_exit(void)
{
	dev_t dev = 0;
	dev = MKDEV(major_num, 0);
	
	printk(KERN_INFO "[androtrace] %s()", __func__);

	unregister_chrdev_region(MKDEV(major_num, 0) , 1);
	cdev_del(&ctrl_dev);
	device_destroy(androtrace_class, dev);
	class_destroy(androtrace_class);
}

MODULE_LICENSE("GPL");
module_init(androtrace_module_init);
module_exit(androtrace_module_exit);
