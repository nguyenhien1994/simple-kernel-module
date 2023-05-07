#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "reverse"

static int major_number; // we don't need to know major number in advance
static int device_open = 0;
static char *input = NULL;
static size_t input_size = 0;

static int reverse_open(struct inode *inode, struct file *file)
{
    if (device_open) {
        return -EBUSY;
    }
    device_open++;
    try_module_get(THIS_MODULE);

    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int reverse_release(struct inode *inode, struct file *file)
{
    
    device_open--;
    module_put(THIS_MODULE);

    // reset input buffer
    kfree(input);
    input = NULL;

    printk(KERN_INFO "Device closed\n");
    return 0;
}

static void reverse_str(char* buffer, size_t length)
{
    int start = 0, end = length - 1;
    while(start < end) {
        input[start] ^= input[end];
        input[end] ^= input[start];
        input[start++] ^= input[end--];
    }
}

static ssize_t reverse_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    if (!input) {
        return 0; // no data to read
    }

    reverse_str(input, input_size);

    if (copy_to_user(buffer, input, input_size)) {
        return -EFAULT;
    }

    return input_size;
}

static ssize_t reverse_write(struct file *file, const char *data, size_t length, loff_t *offset)
{
    if (!input) {
        input = kmalloc(length, GFP_KERNEL);
        if (!input) {
            return -ENOMEM;
        }
        input_size = length;
    } else {
        // resize buffer if necessary
        if (length > input_size) {
            input = krealloc(input, length, GFP_KERNEL);
            if (!input) {
                return -ENOMEM;
            }
            input_size = length;
        }
    }

    if (copy_from_user(input, data, length)) {
        return -EFAULT;
    }

    input[length] = '\0';

    return length;
}

static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .open = reverse_open,
    .read = reverse_read,
    .write = reverse_write,
    .release = reverse_release,
};

static int __init reverse_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Registered successfully with major number %d\n", major_number);
    return 0;
}

static void __exit reverse_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Unregistered successfully\n");
}

module_init(reverse_init);
module_exit(reverse_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HienNM");
MODULE_DESCRIPTION("A simple module to reverse a string");
