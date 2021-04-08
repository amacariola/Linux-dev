#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/***********************************************/
/*        Test LCD Driver(Linux module)        */
/*					       */
/*    Special thanks to Johannes4Linux for     */
/*     a very good introduction to Linux       */
/*            module programming               */
/*     https://github.com/Johannes4Linux       */
/***********************************************/

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TEST LINUX MODULE FOR LCD DISPLAY");

/* LCD PINOUT CONFIGURATION */

unsigned int gpiopins[] = {3,2,4,17,27,22,10,9,11,5};
static char lcd_buffer[17];

/* DEVICE CLASS/VARIABLES */

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "LCD Driver"
#define DRIVER_CLASS "LCDModuleClass"

#define REGISTER_SELECT gpiopins[1]

/**
 * @brief generates a pulse on the enable signal
 */
void lcd_enable(void) {
	gpio_set_value(gpiopins[0], 1);
	msleep(5);
	gpio_set_value(gpiopins[0], 0);
}

/**
 * @brief set the 8 bit data bus
 * @param data: Data to set
 */
void lcd_send_byte(char data) {
	int i;
	for(i=0; i<8; i++)
		gpio_set_value(gpiopins[i+2], ((data) & (1<<i)) >> i);
	lcd_enable();
	msleep(5);
}

/**
 * @brief send a command to the LCD
 *
 * @param data: command to send
 */
void lcd_command(uint8_t data) {
 	gpio_set_value(REGISTER_SELECT, 0);	/* RS to Instruction */
	lcd_send_byte(data);
}

/**
 * @brief send a data to the LCD
 *
 * @param data: command to send
 */
void lcd_data(uint8_t data) {
 	gpio_set_value(REGISTER_SELECT, 1);	/* RS to data */
	lcd_send_byte(data);
}


/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta, i;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(lcd_buffer));

	/* Copy data to user */
	not_copied = copy_from_user(lcd_buffer, user_buffer, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	/* Set the new data to the display */
	lcd_command(0x1);

	for(i=0; i<to_copy; i++)
		lcd_data(lcd_buffer[i]);

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
	.write = driver_write
};

static int __init ModuleInit(void) {
	int i;
	char *names[] = {"ENABLE_PIN", "REGISTER_SELECT", "DATA_PIN0", "DATA_PIN1", "DATA_PIN2", "DATA_PIN3", "DATA_PIN4", "DATA_PIN5", "DATA_PIN6", "DATA_PIN7"};
	printk("LCD Module loaded to the kernel\n");

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
		printk("lcd-driver - Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* Initialize GPIOs */
	printk("lcd-driver - GPIO Init\n");
	for(i=0; i<10; i++) {
		if(gpio_request(gpiopins[i], names[i])) {
			printk("lcd-driver - Error Init GPIO %d\n", gpiopins[i]);
			goto GpioInitError;
		}
	}

	printk("lcd-driver - Set GPIOs to output\n");
	for(i=0; i<10; i++) {
		if(gpio_direction_output(gpiopins[i], 0)) {
			printk("lcd-driver - Error setting GPIO %d to output\n", i);
			goto GpioDirectionError;
		}
	}

	/* Init the display */
	lcd_command(0x30);	/* Set the display for 8 bit data interface */

	lcd_command(0xf);	/* Turn display on, turn cursor on, set cursor blinking */

	lcd_command(0x1);

	char text[] = "This is a test. If you see this, it means the driver runs good";
	for(i=0; i<sizeof(text)-1;i++)
		lcd_data(text[i]);

	return 0;
GpioDirectionError:
	i=9;
GpioInitError:
	for(;i>=0; i--)
		gpio_free(gpiopins[i]);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}


static void __exit ModuleExit(void) {
	int i;
	lcd_command(0x1);	/* Clear the display */
	for(i=0; i<10; i++){
		gpio_set_value(gpiopins[i], 0);
		gpio_free(gpiopins[i]);
	}
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Module detached to the kernel..\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


