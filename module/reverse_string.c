#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>

#define DEVICE_NAME "reverse_string"

static int major_number; // we don't need to know major number in advance
static int device_open = 0;
static char *input = NULL;
static size_t input_size = 0;

static struct class *dev_class;
static struct device *dev_device;

static int reverse_string_open(struct inode *inode, struct file *file)
{
    if (device_open) {
        return -EBUSY;
    }
    device_open++;
    try_module_get(THIS_MODULE);

    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int reverse_string_release(struct inode *inode, struct file *file)
{
    device_open--;
    module_put(THIS_MODULE);

    // reset input buffer
    kfree(input);
    input = NULL;

    printk(KERN_INFO "Device closed\n");
    return 0;
}

// static void reverse_str(char* buffer, size_t length)
// {
//     int start = 0, end = length - 1;
//     while(start < end) {
//         input[start] ^= input[end];
//         input[end] ^= input[start];
//         input[start++] ^= input[end--];
//     }
// }

static ssize_t reverse_string_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    char* reversed_str = NULL;
    size_t i = 0;
    if (!input) {
        return 0; // no data to read
    }

    // reverse input string
    reversed_str = kmalloc(input_size, GFP_KERNEL);
    if (!reversed_str) {
        return -ENOMEM;
    }

    for (i = 0; i < input_size; i++) {
        reversed_str[i] = input[input_size - i - 1];
    }

    if (copy_to_user(buffer, reversed_str, input_size)) {
        return -EFAULT;
    }

    return input_size;
}

static ssize_t reverse_string_write(struct file *file, const char *data, size_t length, loff_t *offset)
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
    .open = reverse_string_open,
    .read = reverse_string_read,
    .write = reverse_string_write,
    .release = reverse_string_release,
};

static int __init reverse_string_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    dev_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(dev_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ERR "Failed to create device class\n");
        return PTR_ERR(dev_class);
    }

    dev_device = device_create(dev_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(dev_device)) {
        class_destroy(dev_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ERR "Failed to create device node\n");
        return PTR_ERR(dev_device);
    }

    printk(KERN_INFO "Registered successfully with major number %d\n", major_number);

    return 0;
}

static void __exit reverse_string_exit(void)
{
    device_destroy(dev_class, MKDEV(major_number, 0));
    class_unregister(dev_class);
    class_destroy(dev_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "Unregistered successfully\n");
}

module_init(reverse_string_init);
module_exit(reverse_string_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HienNM");
MODULE_DESCRIPTION("A simple module to reverse a string");
