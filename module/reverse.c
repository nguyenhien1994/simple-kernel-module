#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "reverse"
#define BUFFER_SIZE 1024

static int major_number;
static char buffer[BUFFER_SIZE];

static int reverse_open(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t reverse_read(struct file *file, char *data, size_t length, loff_t *offset)
{
    int i;
    int j = 0;
    char reverse[BUFFER_SIZE];
    for (i = strlen(buffer) - 1; i >= 0; i--)
    {
        reverse[j++] = buffer[i];
    }
    reverse[j] = '\0';
    if (copy_to_user(data, reverse, strlen(reverse) + 1))
    {
        return -EFAULT;
    }
    return strlen(reverse) + 1;
}

static ssize_t reverse_write(struct file *file, const char *data, size_t length, loff_t *offset)
{
    if (copy_from_user(buffer, data, length))
    {
        return -EFAULT;
    }
    return length;
}

static int reverse_release(struct inode *inode, struct file *file)
{
    return 0;
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
    if (major_number < 0)
    {
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
