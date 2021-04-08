#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/***********************************************/
/*        Test GPIO driver(Linux module)       */
/*					       */
/*    Special thanks to Johannes4Linux for     */
/*     a very good introduction to Linux       */
/*            module programming               */
/*     https://github.com/Johannes4Linux       */
/***********************************************/

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple gpio driver");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_gpio_driver"
#define DRIVER_CLASS "MyModuleClass"

/*************************************/
/*                                   */
/*        READ DATA ON DRIVER        */
/*                                   */
/*************************************/
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char tmp[3] = " \n";

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	/* Read value of button */
	printk("Value of button: %d\n", gpio_get_value(17));
	tmp[0] = gpio_get_value(17) + '0';

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, &tmp, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/*************************************/
/*                                   */
/*       WRITE DATA ON DRIVER        */
/*                                   */
/*************************************/
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	/* Setting the LED */
	switch(value) {
		case '0':
			gpio_set_value(4, 0);
			break;
		case '1':
			gpio_set_value(4, 1);
			break;
		default:
			printk("Invalid Input!\n");
			break;
	}

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}


static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}


static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

/*************************************/
/*                                   */
/*            MODULE START           */
/*                                   */
/*************************************/
static int __init ModuleStart(void) {
	printk("GPIO Driver module loaded into kernel..!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not e created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* GPIO 4 init */
	if(gpio_request(4, "rpi-gpio-4")) {
		printk("Can not allocate GPIO 4\n");
		goto AddError;
	}

	/* Set GPIO 4 direction */
	if(gpio_direction_output(4, 0)) {
		printk("Can not set GPIO 4 to output!\n");
		goto Gpio4Error;
	}

	/* GPIO 17 init */
	if(gpio_request(17, "rpi-gpio-17")) {
		printk("Can not allocate GPIO 17\n");
		goto Gpio4Error;
	}

	/* Set GPIO 17 direction */
	if(gpio_direction_input(17)) {
		printk("Can not set GPIO 17 to input!\n");
		goto Gpio17Error;
	}


	return 0;
Gpio17Error:
	gpio_free(17);
Gpio4Error:
	gpio_free(4);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/*************************************/
/*                                   */
/*           MODULE STOP             */
/*                                   */
/*************************************/
static void __exit ModuleTerminate(void) {
	gpio_set_value(4, 0);
	gpio_free(17);
	gpio_free(4);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Module sucessfully detached to the kernel..\n");
}

module_init(ModuleStart);
module_exit(ModuleTerminate);

/* end of gpio.mod.c file */
