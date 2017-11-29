#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/timer.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include <linux/miscdevice.h> 
#include<linux/string.h>

typedef struct sync_dev
{
  struct timer_list timer;
  struct fasync_struct *fasync_queue;
}sync_dev;


#define DEVICE_NAME "syncdev"


struct sync_dev *sync_devp; 

void timer_funtion(unsigned long para)
{
    if (&sync_devp->fasync_queue)
    {
        kill_fasync(&sync_devp->fasync_queue, SIGIO, POLL_IN);
        printk("send sync !\n");
    }
    mod_timer(&sync_devp->timer,jiffies + HZ * 2);
}
int sync_open(struct inode *inode, struct file *filp)
{
    filp->private_data = sync_devp;
    printk("open sync ok!\n");
    return 0; 
}

static int sync_fasync(int fd, struct file * filp, int on) 
{
    int retval;
    struct sync_dev *dev = filp->private_data;

    retval=fasync_helper(fd,filp,on,&dev->fasync_queue);
    if(retval<0)
      return retval;
    return 0;
}

int sync_release(struct inode *inode, struct file *filp)
{
    struct sync_dev *dev = filp->private_data;

    sync_fasync(-1, filp, 0);     
    return 0;
}

static const struct file_operations sync_fops =
{
    .owner = THIS_MODULE,
    .open = sync_open,
    .release = sync_release,
    .fasync = sync_fasync, /* 必须要的  */
};

static struct miscdevice misc = {
 .minor = MISC_DYNAMIC_MINOR,
 .name = DEVICE_NAME,
 .fops = &sync_fops,
};

static int synccdev_init(void)
{

    misc_register(&misc);
	

    sync_devp = kmalloc(sizeof(struct sync_dev),GFP_KERNEL);    
    if(!sync_devp)
    {
        return -1;
    }
    memset(sync_devp,0,sizeof(struct sync_dev));

    
    
    init_timer(&sync_devp->timer);
    sync_devp->timer.expires = jiffies + HZ*2;
    sync_devp->timer.function = timer_funtion;
    add_timer(&sync_devp->timer);

    //mod_timer(&sync_devp->timer,jiffies + 10);
    
    return 0;
}

static void synccdev_exit(void)
{

    kfree(sync_devp);  
	misc_deregister(&misc);   	
    del_timer(&sync_devp->timer);
}



module_init(synccdev_init);
module_exit(synccdev_exit);
