/*
 * memory driver for lab5 (completed part 2)
 * 
 * Adopted from 'Writing Device Drivers in Linux'
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include "uaccess.h" /* copy_from/to_user */
#include <linux/cdev.h>
#include <linux/device.h>

#define MEMORY_SIZE 5

MODULE_LICENSE("Dual BSD/GPL");

/* Necessary structures */
dev_t dev = 0;
static struct class *dev_class;
static struct cdev memory_cdev;

/* Declaration of memory.c functions */
int memory_open(struct inode *inode, struct file *filp);
int memory_release(struct inode *inode, struct file *filp);
ssize_t memory_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void memory_exit(void);
int memory_init(void);


/* access functions */
struct file_operations fops = {
  read: memory_read,
  write: memory_write,
  open: memory_open,
  release: memory_release
};

/* Declaration of the init and exit functions */
module_init(memory_init);
module_exit(memory_exit);


/* Pointer to Buffer to store data */
char *memory_buffer;

/*
 * Code to run on insert of driver to kernel 
 * This should allocate free major device number 
 */
int memory_init(void) {

	int result;

	/*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "memory")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "memory driver: Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

        /*Creating cdev structure*/
        cdev_init(&memory_cdev,&fops);

        /*Adding character device to the system*/
        if((cdev_add(&memory_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }

        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"memory_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }

        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"memory")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }

	  /* Now initialise the buffer for the memory */

	  /* Allocating memory for the buffer */
	  memory_buffer = kmalloc(MEMORY_SIZE + 1, GFP_KERNEL); 
	  if (!memory_buffer) { 
	    result = -ENOMEM;
	    goto fail; 
	  } 

	  memset(memory_buffer, 0, MEMORY_SIZE + 1);

        printk(KERN_INFO "memory driver: Device Driver Insert...Done!!!\n");
        return 0;

r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;

fail: 

    printk(KERN_INFO "memory driver: Device Driver insert FAILED no memory\n");
    memory_exit(); 
    return result;
}

/*
 * exit module called on removing the module from the kernel
 */
void memory_exit(void) {

  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&memory_cdev);
  unregister_chrdev_region(dev, 1);

  /* Freeing buffer memory */
  if (memory_buffer) {
    kfree(memory_buffer);
  }

  printk(KERN_INFO "memory driver: Device Driver Remove...Done!!!\n");

}

/*
 * Called on the open of the driver
 */
int memory_open(struct inode *inode, struct file *filp) {
  /* Success */
  printk(KERN_INFO "memory driver: Device Driver open() called\n");
  return 0;
}

/*
 * Called on the close of the driver
 */
int memory_release(struct inode *inode, struct file *filp) {
  /* Success */
  printk(KERN_INFO "memory driver: Device Driver release() called\n");
  return 0;
}

/*
 * Called on the read of the driver
 */
ssize_t memory_read(struct file *filp, char __user *buf, 
                    size_t count, loff_t *f_pos) { 

  size_t myCount = count;
  printk(KERN_INFO "memory driver: Device Driver read() called\n");
  printk(KERN_INFO "memory driver: read buffer = [%c %c %c %c %c]\n", 
	memory_buffer[0],
	memory_buffer[1],
	memory_buffer[2],
	memory_buffer[3],
	memory_buffer[4]);

  if (count > MEMORY_SIZE) 
  {
	  printk(KERN_INFO "memory driver: warning: read() count %ld is greater than MEMORY_SIZE (%d) reading MEMORY_SIZE bytes\n", count, MEMORY_SIZE);
	myCount = MEMORY_SIZE;
  }

  /* Transfering data to user space */ 
  copy_to_user((char *)buf, memory_buffer, myCount);

  /* Changing reading position as best suits */ 
  if (*f_pos == 0) { 
    *f_pos+=myCount; 
    return myCount; 
  } else { 
    return 0; 
  }
}

/*
 * Called on the write to the driver
 */
ssize_t memory_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {

  char *tmp;
  size_t myCount = count;

  printk(KERN_INFO "memory driver: Device Driver write() called\n");
  printk(KERN_INFO "memory driver: count=%ld, buf=%s\n", count, buf);

  if (count > MEMORY_SIZE) 
  {
	  printk(KERN_INFO "memory driver: warning: write() count %ld is greater than MEMORY_SIZE (%d) writing MEMORY_SIZE bytes\n", count, MEMORY_SIZE);
	myCount = MEMORY_SIZE;
  }

  tmp= (char *)buf;
  copy_from_user(memory_buffer, tmp, myCount);

  printk(KERN_INFO "memory driver: write buffer = [%c %c %c %c %c]\n", 
	memory_buffer[0],
	memory_buffer[1],
	memory_buffer[2],
	memory_buffer[3],
	memory_buffer[4]);

  return myCount;
}

/* End of driver */
