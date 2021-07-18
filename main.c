
#include "copy_device.h"
#include <linux/init.h>
#include <linux/module.h>


MODULE_DESCRIPTION("copy driver for linux");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SXX");


static int copy_driver_init(void)
{
	int res;

	printk( KERN_NOTICE "Copy driver: init\n");
	
	res = register_device();
	return res;
}


static void copy_driver_exit(void)
{
	printk( KERN_NOTICE "Copy driver: exit\n");
	unregister_device();
}

module_init(copy_driver_init);
module_exit(copy_driver_exit);



