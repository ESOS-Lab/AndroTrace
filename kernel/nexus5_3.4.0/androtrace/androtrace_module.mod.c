#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x149063a6, "module_layout" },
	{ 0x59c80094, "class_destroy" },
	{ 0x4ea213da, "device_destroy" },
	{ 0x24cdb706, "cdev_del" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xd3f53596, "device_create" },
	{ 0x828ad9f2, "cdev_add" },
	{ 0x37358781, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x2e8156d0, "__class_create" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xbc3d21af, "finish_wait" },
	{ 0x69ff5332, "prepare_to_wait" },
	{ 0x1000e51, "schedule" },
	{ 0xc4f7fb46, "kmalloc_caches" },
	{ 0xa7ecf324, "__init_waitqueue_head" },
	{ 0x86a4889a, "kmalloc_order_trace" },
	{ 0x6146f21b, "kmem_cache_alloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27e1a049, "printk" },
	{ 0xaea99e9d, "_raw_spin_unlock_irq" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xe551272c, "_raw_spin_lock_irq" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "F68E739FE786AD0FC23AB97");
