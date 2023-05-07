#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>

#define DEVICE_NAME "reverse_string"

static int major_number; // we don't need to know major number in advance
static int device_open = 0;
static char *buffer = NULL;
static size_t buffer_size = 0;

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

    printk(KERN_INFO "Device closed\n");
    return 0;
}

// static void reverse_str(char* buffer, size_t length)
// {
//     int start = 0, end = length - 1;
//     while(start < end) {
//         buffer[start] ^= buffer[end];
//         buffer[end] ^= buffer[start];
//         buffer[start++] ^= buffer[end--];
//     }
// }

static ssize_t reverse_string_read(struct file *file, char *data, size_t length, loff_t *offset)
{
    char* reversed_str = NULL;
    size_t i = 0;
    size_t str_len = 0;
    if (!buffer) {
        return 0; // no data to read
    }

    str_len = strlen(buffer);

    // reverse buffer string
    reversed_str = kmalloc(str_len, GFP_KERNEL);
    if (!reversed_str) {
        return -ENOMEM;
    }

    for (i = 0; i < str_len; i++) {
        reversed_str[i] = buffer[str_len - i - 1];
    }

    if (copy_to_user(data, reversed_str, str_len)) {
        kfree(reversed_str);
        return -EFAULT;
    }

    kfree(reversed_str);

    return str_len;
}

static ssize_t reverse_string_write(struct file *file, const char *data, size_t length, loff_t *offset)
{
    if (!buffer) {
        buffer = kmalloc(length, GFP_KERNEL);
        if (!buffer) {
            return -ENOMEM;
        }
        buffer_size = length;
    } else if (length > buffer_size) {
        // resize buffer if necessary
        buffer = krealloc(buffer, length, GFP_KERNEL);
        if (!buffer) {
            return -ENOMEM;
        }
        buffer_size = length;
    }

    if (copy_from_user(buffer, data, length)) {
        return -EFAULT;
    }

    buffer[length] = '\0';

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
    if (buffer) {
        kfree(buffer);
    }

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
