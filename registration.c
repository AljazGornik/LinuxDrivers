#include <linux/fs.h> 	    /* file stuff */
#include <linux/kernel.h>   /* printk() */
#include <linux/errno.h>    /* error codes */
#include <linux/module.h>   /* THIS_MODULE */
#include <linux/cdev.h>     /* char device stuff */
#include <linux/uaccess.h>  /* copy_to_user() */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>

static char* whom = "World";
static int howmany = 1;

int device_file_major_number = 0;
static const char device_name[] = "Simple driver with new registration";

static ssize_t device_file_read(struct file *, char *, size_t , loff_t *);
static ssize_t device_file_write(struct file *, const char *, size_t, loff_t *);
static int device_file_open(struct inode *, struct file *);

dev_t dev = 0;
static struct cdev dev_cdev;
static struct class *dev_class;


static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.read = device_file_read,
	.write = device_file_write,
	.open = device_file_open,
};

void unregister_device(void){
	printk( KERN_ALERT "Simple driver: unregister_device() is called\n" );
   	if(device_file_major_number != 0)
    	{
        	unregister_chrdev_region(dev, 1);
    	}
}

int register_device(void)
{
	int result = 0;

	printk(KERN_ALERT "Simple driver: register_device() is called.\n" );

	// dynamic allocation (0)


	result = alloc_chrdev_region(&dev, 0,1, device_name);
	if( result < 0 )
	{
        	printk(KERN_ALERT "Simple driver:  can\'t register character device with errorcode = %i\n", result );
	        return -1;
    	}
	

	device_file_major_number = MAJOR(dev);
	printk(KERN_ALERT "Simple driver: registered character device with major number = %i and minor numbers 0...255\n", device_file_major_number );


	//assign file operations to dev
	cdev_init(&dev_cdev, &fops);

	//telling kernel about the cdev structure
	if(cdev_add(&dev_cdev, dev,1) < 0){
		printk(KERN_ALERT "Failed to add the device to the system");
	}


	//creating class
	if((dev_class = class_create(THIS_MODULE,"simple_device_driver_class")) == NULL){
            printk(KERN_ALERT "Cannot create the struct class for device\n");
            return -1;
        }	
	printk(KERN_ALERT "Created class!\n");

	/*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"simple_registration_device")) == NULL){
            printk(KERN_ALERT "Cannot create the Device\n");
            return -1;
        }
        printk(KERN_ALERT "Kernel device created Successfully...\n");

	return 0;
}


static int hello_init(void){
	int i;
	int result = 0;

	for(i = 0; i<howmany; i++){
		printk(KERN_ALERT "Hello, %s!\n", whom);
	}
	printk(KERN_ALERT "The process is \"%s\" (pid %i)\n", current->comm, current->pid);
	printk(KERN_ALERT "Simple driver: Initialization started");

	// registering device
	result = register_device();
	return 0;
}

static void hello_exit(void){
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
	cdev_del(&dev_cdev);
	unregister_device();	
	printk(KERN_ALERT "Simple driver: Exiting!\n");
	printk(KERN_ALERT "Goodbye %s!\n", whom);
}


static ssize_t device_file_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	printk(KERN_ALERT "Writing to the device");
	return 0;
}

static ssize_t device_file_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	printk(KERN_ALERT "Reading from the device");
	return 0;
}


static int device_file_open(struct inode *inode, struct file *file){
	printk(KERN_ALERT "Device opened!\n");
	return 0;
}

module_init(hello_init);
module_exit(hello_exit);
module_param(whom,charp,S_IRUGO);
module_param(howmany,int,S_IRUGO);
MODULE_LICENSE("GPL");
