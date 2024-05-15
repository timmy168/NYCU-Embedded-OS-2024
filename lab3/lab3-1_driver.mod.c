#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
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
__used __section("__versions") = {
	{ 0x92997ed8, "_printk" },
	{ 0xf169a864, "gpio_to_desc" },
	{ 0xc28c8517, "gpiod_set_raw_value" },
	{ 0x136a9a25, "gpiod_unexport" },
	{ 0xfe990052, "gpio_free" },
	{ 0xeec9ab2a, "device_destroy" },
	{ 0xfff64c16, "class_destroy" },
	{ 0xe719aa27, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xdcb764ad, "memset" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xf9a482f9, "msleep" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0xb23eacad, "gpiod_direction_output_raw" },
	{ 0xd01e90a0, "gpiod_export" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x8848911d, "cdev_init" },
	{ 0x606edced, "cdev_add" },
	{ 0x3de0fa69, "__class_create" },
	{ 0xaa844fa3, "device_create" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x534337da, "gpiod_get_raw_value" },
	{ 0x244ba1b1, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "832A0390D8E33E54EB8A45C");
