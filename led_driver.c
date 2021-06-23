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
#include <linux/gpio.h>


int device_file_major_number = 0;
static const char device_name[] = "customled";


static ssize_t device_file_write(struct file *, const char *, size_t, loff_t *);
static int device_file_open(struct inode *, struct file *);

dev_t dev = 0;
static struct cdev dev_cdev;
static struct class *dev_class;


/* Define GPIOs for LEDs */
static struct gpio leds[] = {{  21, GPIOF_OUT_INIT_LOW, "LED"}, };



static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.write = device_file_write,
	.open = device_file_open,
};

void unregister_device(void){
	printk( KERN_ALERT "Led driver: unregister_device() is called\n" );
   	if(device_file_major_number != 0)
    	{
        	unregister_chrdev_region(dev, 1);
    	}
}

int register_device(void)
{
	int result = 0;

	printk(KERN_ALERT "Led driver: register_device() is called.\n" );

	// dynamic allocation (0)

	// register LED gpios
	result = gpio_request_array(leds, ARRAY_SIZE(leds));

	if (result) {
		printk(KERN_ERR "Unable to request GPIOs for LEDs: %d\n", result);
		return result;
	}


	result = alloc_chrdev_region(&dev, 0,1, device_name);
	if( result < 0 )
	{
        	printk(KERN_ALERT "Led:  can\'t register character device with errorcode = %i\n", result );
	        return -1;
    	}
	

	device_file_major_number = MAJOR(dev);
	printk(KERN_ALERT "Led: registered character device with major number = %i and minor numbers 0...255\n", device_file_major_number );


	//assign file operations to dev
	cdev_init(&dev_cdev, &fops);

	//telling kernel about the cdev structure
	if(cdev_add(&dev_cdev, dev,1) < 0){
		printk(KERN_ALERT "Failed to add the device to the system");
	}


	//creating class
	if((dev_class = class_create(THIS_MODULE,"customled_class")) == NULL){
            printk(KERN_ALERT "Cannot create the struct class for device\n");
            return -1;
        }	
	printk(KERN_ALERT "Created class!\n");

	/*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"customled")) == NULL){
            printk(KERN_ALERT "Cannot create the Device\n");
            return -1;
        }
        printk(KERN_ALERT "Kernel device created Successfully...\n");

	return 0;
}


static int hello_init(void){
	int result = 0;

	printk(KERN_ALERT "Simple driver: Initialization started");

	// registering device
	result = register_device();
	return 0;
}

static void hello_exit(void){
	gpio_set_value(leds[0].gpio,0);
	gpio_free_array(leds, ARRAY_SIZE(leds));
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
	cdev_del(&dev_cdev);
	unregister_device();	
	printk(KERN_ALERT "Simple driver: Exiting!\n");
	printk(KERN_ALERT "Goodbye!\n");
}


static ssize_t device_file_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	char led_mode = '\0';
	printk(KERN_ALERT "Writing to the device");
	
	copy_from_user(&led_mode,&buffer[0],1);
	
	if(led_mode == '1' && len == 2){
		gpio_set_value(leds[0].gpio, 1);
	}
	else if(led_mode == '0' && len == 2){
		gpio_set_value(leds[0].gpio, 0);
	}
	else{
		printk(KERN_ALERT "Custom led driver: WRONG INPUT!\nWrite '1' for turning LED on or '0' for turning LED off!\n");
	}
	
	return len;
}


static int device_file_open(struct inode *inode, struct file *file){
	printk(KERN_ALERT "Device opened!\n");
	return 0;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
