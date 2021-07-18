#include "copy_device.h"
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/mm.h> 
#include <linux/slab.h>

#define ERROR -1
#define BUFFER_SIZE 24
#define BYTES_PER_LINE 8

int major = 0;

struct class* dev_class = NULL;

const char* device_name = "copy";
const char* device_driver_name = "Copy driver";
const char* output_file_path = "/tmp/output";

char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

struct file* open_output_file(const char* file_name)
{
    	struct file * output_file = NULL;
	
	mm_segment_t oldfs;

    	oldfs = get_fs();
    	set_fs(KERNEL_DS); // get_ds() function returns KERNEL_DS

    	output_file = filp_open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0);
    	
	set_fs(oldfs);

	if(IS_ERR(output_file)) {   
		printk(KERN_WARNING "Copy driver: flip_open fails");
		return NULL;
	} 

	return output_file;
}

void convert_to_hex(char* hex_buffer, char* bytes, int size)
{
	for(int i = 0; i < size; ++i) {
		hex_buffer[3*i] = hexmap[(bytes[i] & 0xF0) >> 4];
		hex_buffer[3*i + 1] = hexmap[bytes[i] & 0x0F];
		hex_buffer[3*i + 2] = ' ';
	}
}


int write_hex_buffer_formatted(struct file* output_file, char* buffer, int size)
{
	int res, data_sz, written_bytes;
	char hex_buf[BYTES_PER_LINE*3 + 1];

	written_bytes = 0;
	data_sz = BYTES_PER_LINE;
	
	for(int i = 0; i <= size/BYTES_PER_LINE; ++i) 
	{
		if(i == size/BYTES_PER_LINE) {
			data_sz = size % BYTES_PER_LINE;
			if(data_sz == 0) 
				break;
		}

		hex_buf[data_sz*3] = '\n';
	
		convert_to_hex(&hex_buf[0], buffer+i*BYTES_PER_LINE, data_sz);
	
		res = vfs_write(output_file, hex_buf, data_sz*3 + 1, &output_file->f_pos); 
	
		if(res != (data_sz*3 + 1)) {
			printk(KERN_WARNING "Copy driver: vfs_write writes %d bytes instead of %d", res, data_sz);
		}
		written_bytes += res;
	}
	return written_bytes;
}


static ssize_t device_file_write(struct file *file_ptr, const char __user *user_buffer, size_t size, loff_t *offset)
{

	struct file* output_file;
	mm_segment_t oldfs; 

	int chunk_count, sum, data_size, res;
	char buf[BUFFER_SIZE];
	const char* data_ptr;
	
	
	chunk_count = size/BUFFER_SIZE;
	data_size = BUFFER_SIZE;
	data_ptr = user_buffer;
	sum = 0;
	
	output_file = open_output_file(output_file_path);
	if(output_file == NULL){
		return ERROR;
	}

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	for(int i = 0; i <= chunk_count; ++i) 
	{	
		data_ptr = user_buffer + i*BUFFER_SIZE;
		
		if(i == chunk_count) {
			data_size = size % BUFFER_SIZE;
		}

		res = copy_from_user(&buf, data_ptr, data_size);
		
		if(res != 0) {
			printk(KERN_WARNING "Copy driver: copy_from_user, uncopied bytes: %d", res);
		}

		write_hex_buffer_formatted(output_file, buf, data_size);
	}

	set_fs(oldfs);

	vfs_fsync(output_file, 0);

	filp_close(output_file, NULL);

	return size;
}


static struct file_operations copy_driver_fops = 
{
	.owner = THIS_MODULE,
	.write = device_file_write,
};


int register_device(void)
{
	struct device* chr_dev = NULL;

	printk( KERN_NOTICE "Copy driver: register_device() called\n");

	major = register_chrdev(0, device_driver_name, &copy_driver_fops );
	
	if(major < 0) {
		printk( KERN_WARNING "Copy driver: cannot register device, error: %i\n", major);
		return ERROR;
	}

	dev_class = class_create(THIS_MODULE, device_name);

	if(dev_class == NULL) {
		printk( KERN_WARNING "Copy driver: Class couldn't be created!" );
		return ERROR;
	}

	chr_dev = device_create(dev_class , NULL , MKDEV(major, 0), NULL, device_name);
	
	if(chr_dev == NULL) {
		printk( KERN_WARNING "Copy driver: Device file couldnt be created" );
		return ERROR;
	}

	return 0;
}




void unregister_device(void)
{

	printk( KERN_NOTICE "Copy driver: unregister_device() called\n");
	
	device_destroy(dev_class,MKDEV(major,0));
     
	class_destroy(dev_class);
     
	if(major != 0) {
		unregister_chrdev(major, device_driver_name);
	}

}


