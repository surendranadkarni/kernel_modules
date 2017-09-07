/*
 * * chardev.c − Create an input/output character device
 * */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "chardev_ioctl.h"

#define SUCCESS 0
#define DEVICE_NAME "char_dev"
#define BUF_LEN 80
#define DEBUG 1

static int Device_Open = 0;
static char Message[BUF_LEN];
static char *Message_Ptr = "Hello";

static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
    printk("device_open(%p)\n", file);
#endif
    if (Device_Open)
        return -EBUSY;
    Device_Open++;
    /*
     *  * Initialize the message
     *      */
    Message_Ptr = Message;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
    printk("device_release(%p,%p)\n", inode, file);
#endif
    /*
     *  * We're now ready for our next caller
     *      */
    Device_Open--;
    module_put(THIS_MODULE);
    return SUCCESS;
}


static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
    int bytes_read = 0;
#ifdef DEBUG
    printk("device_read(%p,%p,%d)\n", file, buffer, length);
#endif
    if (*Message_Ptr == 0)
    {
        printk("Message_Ptr = 0\n");
        return 0;
    }
    while (length && *Message_Ptr) 
    {
        put_user(*(Message_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
    //copy_to_user(buffer,Message_Ptr, strlen(Message_Ptr));
#ifdef DEBUG
    printk("Read %d bytes, %d left\n", bytes_read, length);
#endif
    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{
    int i = 0;
#ifdef DEBUG
    printk("device_write\n");
#endif
    for (i = 0; i < length && i < BUF_LEN; i++)
    {
        get_user(Message[i], buffer + i);
    }
    //copy_from_user(Message, buffer, length);
    Message_Ptr = Message;
    return i;
}

long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int i;
    char *temp;
    char ch;
    switch (ioctl_num) 
    {
        case IOCTL_SET_MSG:
            temp = (char *)ioctl_param;
            get_user(ch, temp);
            //copy_from_user(ch, temp, 1);
            for (i = 0; ch && i < BUF_LEN; i++, temp++)
                get_user(ch, temp);
                //copy_from_user(ch, temp, 1);
            device_write(file, (char *)ioctl_param, i, 0);
            #ifdef DEBUG
                printk("device_write(%p)", file);
            #endif
                
            break;
        case IOCTL_GET_MSG:
            i = device_read(file, (char *)ioctl_param, 99, 0);
            put_user('\0', (char *)ioctl_param + i);
            #ifdef DEBUG
                printk("device_read(%p)", file);
            #endif

            break;
        case IOCTL_GET_NTH_BYTE:
               #ifdef DEBUG
                printk("IOCTL_GET_NTH_BYTE");
               #endif

            return Message[ioctl_param];
            break;
    }
    return SUCCESS;
}

struct file_operations Fops = 
{
    .read   = device_read,
    .write  = device_write,
    .unlocked_ioctl  = device_ioctl,
    .open   = device_open,
    .release = device_release,
};

int init_module()
{
    int ret_val;
    /*
     *  * Register the character device (atleast try)
     *      */
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);
    /*
     *  * Negative values signify an error
     *      */
    if (ret_val < 0) {
        printk("%s failed with %d\n",
                "Sorry, registering the character device ", ret_val);
        return ret_val;
    }
    printk("%s The major device number is %d.\n",
            "Registeration is a success", MAJOR_NUM);
    printk("If you want to talk to the device driver,\n");
    printk("you'll have to create a device file. \n");
    printk("We suggest you use:\n");
    printk("mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
    printk("The device file name is important, because\n");
    printk("the ioctl program assumes that's the\n");
    printk("file you'll use.\n");
    return 0;
}

void cleanup_module()
{
    int ret;
    /*
     *  * Unregister the device
     *      */
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    /*
     *  * If there's an error, report it
     *      */
    /*if (ret < 0)
        printk("Error in module_unregister_chrdev: %d\n", ret);*/
}
