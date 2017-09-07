#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define procfs_name "my_sched"
#define procfs_string "Hello World\n"
#define MY_WORK_QUEUE_NAME "WQsched.c"

#define MIN(a,b) (a)<(b)?(a):(b)
#define SUCCESS 0
#define BUF_LEN 80
int len = 0;
struct proc_dir_entry *Our_Proc_File;

static int TimerIntrpt = 0;
static int die = 0;
static struct workqueue_struct *my_workqueue;
static struct delayed_work my_task;



static int count;
static char msg[BUF_LEN];
static char *msg_Ptr;


static void intrpt_routine(struct work_struct *work)
{

    /* Increment the counter*/
    TimerIntrpt++;

    /* If cleanup wants us to die*/
    if (die == 0)
        queue_delayed_work(my_workqueue, &my_task, 100);
}


static ssize_t procfile_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
    unsigned long p = *offset;
    printk(KERN_INFO "offset %lx %d \n timer called %d\n", p, count++, TimerIntrpt);
    if(p == 0)
    {
        int bytes_read = len+1;
        copy_to_user(buffer, msg, bytes_read); 
        *offset = bytes_read;
        return bytes_read;
    } 
    else
    {
        return 0;
    }
}

static ssize_t procfile_write(struct file *filp, const char __user *buffer, size_t length, loff_t * offset)
{
    len = MIN(length,BUF_LEN);
    copy_from_user(msg, buffer, len);
    msg_Ptr = msg;
    printk(KERN_INFO "procfs_write: write %lu bytes\n", len);
    return length;
}


static int procfile_open(struct inode *inode, struct file *file)
{
    return SUCCESS;
}

static const struct file_operations proc_file_fops = {
 .owner = THIS_MODULE,
 .open  = procfile_open,
 .read  = procfile_read,
 .write = procfile_write
};

int init_module()
{
    INIT_DELAYED_WORK(&my_task, intrpt_routine);
    my_workqueue = create_workqueue(MY_WORK_QUEUE_NAME);
    queue_delayed_work(my_workqueue, &my_task, 100);

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
    die = 1;
    cancel_delayed_work(&my_task);
    flush_workqueue(my_workqueue);
    destroy_workqueue(my_workqueue);

    printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}


MODULE_LICENSE("GPL");
