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
	{ 0x229f7496, "filp_open" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0x2eae86e3, "get_pid_task" },
	{ 0xb3cf6dd7, "sock_from_file" },
	{ 0x4302d0eb, "free_pages" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x92997ed8, "_printk" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x6a5cb5ee, "__get_free_pages" },
	{ 0xa2e9ed7e, "init_task" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x235bb942, "pid_task" },
	{ 0xb60450de, "dentry_path_raw" },
	{ 0x9166fada, "strncpy" },
	{ 0xf9aa7421, "kern_path" },
	{ 0xf9c0b663, "strlcat" },
	{ 0xb4b19daa, "param_ops_charp" },
	{ 0x75a40ab, "kernel_read" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xf4c04097, "filp_close" },
	{ 0xfff5afc, "time64_to_tm" },
	{ 0x6a53721c, "find_vpid" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "4FAC344766743281DE5CCCE");
