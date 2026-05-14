 /**************************************************************
 * Class: CSC-415-02 Spring 2026
 * Name: Junxi Lin
 * Student ID: 923696927
 * GitHub Name: Junxi-Lin
 * Project: Assignment 6 - Device Driver
 *
 * File: secret.c
 *
 * Description: A device driver that performs text encryption
 *              and decryption  using a Caesar cipher with
 *              a user-defined key.
 *
 **************************************************************/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define MY_MAJOR 415
#define MY_MINOR 0
#define DEVICE_NAME "secret"

/* ioctl commands */
#define MODE_ENCRYPT 'e'
#define MODE_DECRYPT 'd'
/* to set the Caesar cipher key */
#define SET_KEY      'k'

#define MAX_SIZE 512
#define ASCII_RANGE 256

MODULE_AUTHOR("Junxi Lin");
MODULE_DESCRIPTION("Secret device driver with Caesar cipher");
MODULE_LICENSE("GPL");

static ssize_t my_read(struct file *fs, char __user *buf, size_t hsize, loff_t *off);
static ssize_t my_write(struct file *fs, const char __user *buf, size_t hsize, loff_t *off);
static int my_open(struct inode *inode, struct file *fs);
static int my_close(struct inode *inode, struct file *fs);
static long my_ioctl(struct file *fs, unsigned int command, unsigned long data);

/* Character device structure */
struct cdev my_cdev;

/* Data structure stored for open file */
typedef struct myds
{
    char *text;        // buffer storing user text
    size_t size;       // size of stored text
    int key;           // Caesar cipher shift value
    int isEncrypted;   // state flag
} myds;

/* File operations map */
struct file_operations fops = {
    .open = my_open,
    .release = my_close,
    .write = my_write,
    .read = my_read,
    .unlocked_ioctl = my_ioctl,
    .owner = THIS_MODULE
};

/* Open that allocate memory for this instance */
static int my_open(struct inode *inode, struct file *fs)
{
    myds *ds;

    ds = vmalloc(sizeof(myds));
    if (!ds)
        return -ENOMEM;

    ds->text = vmalloc(MAX_SIZE);
    if (!ds->text)
    {
        vfree(ds);
        return -ENOMEM;
    }

    memset(ds->text, 0, MAX_SIZE);

    ds->size = 0;
    ds->key = 5;
    ds->isEncrypted = 0;

    fs->private_data = ds;

    printk(KERN_INFO "secret: device opened\n");
    return 0;
}

/* Close that  free allocated memory */
static int my_close(struct inode *inode, struct file *fs)
{
    myds *ds = (myds *)fs->private_data;

    if (ds)
    {
        if (ds->text)
            vfree(ds->text);

        vfree(ds);
    }

    printk(KERN_INFO "secret: device closed\n");
    return 0;
}

/* Copy data from user into kernel buffer */
static ssize_t my_write(struct file *fs, const char __user *buf, size_t hsize, loff_t *off)
{
    myds *ds = (myds *)fs->private_data;
    size_t len;

    if (!ds || !ds->text)
        return -EFAULT;

    /* Prevent buffer overflow */
    if (hsize >= MAX_SIZE)
        len = MAX_SIZE - 1;
    else
        len = hsize;

    memset(ds->text, 0, MAX_SIZE);

    if (copy_from_user(ds->text, buf, len))
        return -EFAULT;

    ds->text[len] = '\0';
    ds->size = len;
    ds->isEncrypted = 0;

    printk(KERN_INFO "secret: wrote: %s\n", ds->text);

    return len;
}

/* send processed data back to the user */
static ssize_t my_read(struct file *fs, char __user *buf, size_t hsize, loff_t *off)
{
    myds *ds = (myds *)fs->private_data;
    size_t len;

    if (!ds || !ds->text)
        return -EFAULT;

    len = strlen(ds->text);

    if (copy_to_user(buf, ds->text, len + 1))
        return -EFAULT;

    printk(KERN_INFO "secret: read: %s\n", ds->text);

    return len;
}

/* ioctl: control device behavior like encrypt,decrypt, or set key */
static long my_ioctl(struct file *fs, unsigned int command, unsigned long data)
{
    myds *ds = (myds *)fs->private_data;
    int i;

    if (!ds || !ds->text)
        return -EFAULT;

    switch (command)
    {
        case MODE_ENCRYPT:
            for (i = 0; i < strlen(ds->text); i++)
                ds->text[i] = (ds->text[i] + ds->key) % ASCII_RANGE;

            ds->isEncrypted = 1;
            printk(KERN_INFO "secret: encrypted -> %s\n", ds->text);
            break;

        case MODE_DECRYPT:
            for (i = 0; i < strlen(ds->text); i++)
                ds->text[i] = (ds->text[i] - ds->key + ASCII_RANGE) % ASCII_RANGE;

            ds->isEncrypted = 0;
            printk(KERN_INFO "secret: decrypted -> %s\n", ds->text);
            break;

        case SET_KEY:
            ds->key = (int)data;
            printk(KERN_INFO "secret: key set to %d\n", ds->key);
            break;

        default:
            printk(KERN_ERR "secret: invalid ioctl command\n");
            return -EINVAL;
    }

    return 0;
}


int init_module(void)
{
    int result;
    dev_t devno = MKDEV(MY_MAJOR, MY_MINOR);

    result = register_chrdev_region(devno, 1, DEVICE_NAME);
    if (result < 0)
        return result;

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    result = cdev_add(&my_cdev, devno, 1);
    if (result < 0)
    {
        unregister_chrdev_region(devno, 1);
        return result;
    }

    printk(KERN_INFO "secret: driver loaded\n");
    return 0;
}

void cleanup_module(void)
{
    dev_t devno = MKDEV(MY_MAJOR, MY_MINOR);

    cdev_del(&my_cdev);
    unregister_chrdev_region(devno, 1);

    printk(KERN_INFO "secret: driver unloaded\n");
}
