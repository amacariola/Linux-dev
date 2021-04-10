/***********************************/
/*     LKM TIMER TEST MODULE       */
/*                                 */  
/***********************************/


#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TEST LKM TIMER MODULE");



static struct timer_list my_timer;

void timer_callback(struct timer_list * data) {
	gpio_set_value(4, 0); /* Turn LED off */
}

/***********************************/
/*         START MODULE            */
/*                                 */  
/***********************************/


static int __init ModuleStart(void) {
	printk("Module attached to kernel..\n");
	/* GPIO 4 init */
	if(gpio_request(4, "rpi-gpio-4")) {
		printk("Can not allocate GPIO 4\n");
		return -1;
	}

	/* Set GPIO 4 direction */
	if(gpio_direction_output(4, 0)) {
		printk("Can not set GPIO 4 to output!\n");
		gpio_free(4);
		return -1;
	}

	/* Turn LED on */
	gpio_set_value(4, 1);

	/* Initialize timer */
	timer_setup(&my_timer, timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));


	return 0;
}

/***********************************/
/*          STOP MODULE            */
/*                                 */  
/***********************************/

static void __exit ModuleStop(void) {
	printk("Module detached to kernel..exiting\n");
	gpio_free(4);
	del_timer(&my_timer);
}

module_init(ModuleStart);
module_exit(ModuleStop);


