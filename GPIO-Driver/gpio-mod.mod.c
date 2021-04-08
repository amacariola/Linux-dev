#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xd1656c72, "module_layout" },
	{ 0xc01ea588, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xdbefea67, "class_destroy" },
	{ 0xc2219b3e, "device_destroy" },
	{ 0xfe990052, "gpio_free" },
	{ 0x886b3749, "gpiod_direction_input" },
	{ 0x21b8bbe9, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xb2b81575, "cdev_add" },
	{ 0xcb58c8a0, "cdev_init" },
	{ 0x87484f76, "device_create" },
	{ 0xf14f99d5, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xdf9d8a59, "gpiod_set_raw_value" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x14dedb67, "gpiod_get_raw_value" },
	{ 0x1bde9398, "gpio_to_desc" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");

