#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define procfs_name "sleep_test"
#define procfs_string "Hello World\n"
#define MIN(a,b) (a)<(b)?(a):(b)
#define SUCCESS 0

struct proc_dir_entry *Our_Proc_File;

static int count;
static int already_open = 0;

DECLARE_WAIT_QUEUE_HEAD(WaitQ);

static ssize_t procfile_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
    unsigned long p = *offset;
    printk(KERN_INFO "offset %x %d\n", p, count++);
    if(p == 0)
    {
        int bytes_read = strlen(procfs_string)+1;
        copy_to_user(buffer, procfs_string, bytes_read); 
        *offset = bytes_read;
        return bytes_read;
    } 
    else
    {
        return 0;
    }
}


static int procfile_open(struct inode *inode, struct file *file)
{
    if ((file->f_flags & O_NONBLOCK) && already_open)
        return -EAGAIN;

    while(already_open)
    {
        wait_event_interruptible(WaitQ, !already_open);
        
    }
    try_module_get(THIS_MODULE);
    already_open = 1;
    return SUCCESS;
}

static int procfile_close(struct inode *inode, struct file *file)
{
    already_open = 0;
    wake_up(&WaitQ);
    module_put(THIS_MODULE);
    return SUCCESS;
}

static const struct file_operations proc_file_fops = {
 .owner = THIS_MODULE,
 .open  = procfile_open,
 .read  = procfile_read,
 .release = procfile_close,
};

int init_module()
{
    Our_Proc_File = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
    if (Our_Proc_File == NULL) {
        proc_remove(Our_Proc_File);
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", procfs_name);
        return -ENOMEM;
    }

    printk(KERN_INFO "/proc/%s created\n", procfs_name);
    return 0;
    /* everything is ok */
}

void cleanup_module()
{
    proc_remove(Our_Proc_File);
    printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}
