#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/delay.h>
#include <linux/delay.h>
#include <asm/system.h>
#include <linux/module.h>

#include "sram.h"

#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
static devfs_handle_t sram_devfs_handle = NULL;
const char devfsname[]=SRAMNAME;
#endif

static u32* sram_physaddr  = ((u32 *) SRAMBASE);
static u32* sram_virtbase;
static int opened=0;
static int majornum;

static int sram_read  (struct file *filp, char *buf, size_t count, loff_t *unused_loff_t);
static int sram_write (struct file *filp, const char *buf, size_t count, loff_t *unused_loff_t);
static int sram_open (struct inode *inode, struct file *filp);
static int sram_release (struct inode *inode, struct file *filp);

static struct file_operations sram_fops = {
  owner   :   THIS_MODULE,
  read	  :   sram_read,
  write   :   sram_write,
  open    :   sram_open,
  release : sram_release,
};

MODULE_AUTHOR("Jochen Klein");
MODULE_DESCRIPTION("sram module");
MODULE_LICENSE("GPL");

static int sram_read(struct file *filp, char *buf, size_t count, loff_t *unused_loff_t) {
  sram_data_s sram_data;

  if (count != sizeof(sram_data_s)) {
    printk(KERN_ERR "pio_read: wrong size\n");
    return -EINVAL;
  }
  copy_from_user((unsigned char *) &sram_data, buf, sizeof(sram_data_s));

  sram_data.in = readl(sram_virtbase + sram_data.adr);
  unsigned long j = jiffies + 2;
  while(jiffies < j); /*wait for one jiffie, 10ms*/

  sram_data.in = readl(sram_virtbase + sram_data.adr);

  copy_to_user(buf, (unsigned char *) &sram_data,sizeof(sram_data_s));
  return count;
}

static int sram_write (struct file *filp, const char *buf, size_t count, loff_t *unused_loff_t)
{
  sram_data_s sram_data;

  if (count != sizeof(sram_data_s)) {
    printk(KERN_ERR "sram_write: wrong size\n");
    printk("count: %d,  datas: %d \n", count, sizeof(sram_data_s));
    return -EINVAL;
  }
  copy_from_user((unsigned char *) &sram_data, buf, sizeof(sram_data_s));

  writel(sram_data.out, sram_virtbase + sram_data.adr);

  unsigned long j = jiffies + 2;
  while(jiffies < j); /*wait for 10ms*/

  return count;
}


static int sram_open (struct inode *inode, struct file *filp)
{
  int minor = MINOR(inode->i_rdev);
  if (minor != 0) {
    printk("Only minor 0 actual\n");
    return -ENODEV;
  }
  if (opened)
    return -EBUSY;
  MOD_INC_USE_COUNT;
  opened=1;
  return 0;
}

static int sram_release (struct inode *inode, struct file *filp)
{
  MOD_DEC_USE_COUNT;
  opened=0;
  return 0;
}

static int sram_init ()
{
  int result;
  result = devfs_register_chrdev(SRAMMAJO, SRAMNAME, &sram_fops);
  if (result < 0) {
    printk(KERN_ERR "Can not get major number %d\n", SRAMMAJO);
    return result;
  } else {
    majornum = result;
    printk(KERN_INFO "Major number of sram-Module: %d\n", majornum);
  }

  sram_devfs_handle = devfs_register(NULL,
                                     devfsname,
                                     DEVFS_FL_DEFAULT,
                                     result,
                                     0,
                                     S_IFCHR | S_IRUGO | S_IWUGO,
                                     &sram_fops,
                                     NULL );

  sram_virtbase  = (u32*) ioremap_nocache((u32)sram_physaddr,SRAMSIZE);
  opened=0;
  return 0;
}

int init_module(void)
{
  printk(KERN_INFO "sram module init (compiled "__DATE__", "__TIME__")\n");
  return sram_init();
}

void cleanup_module(void)
{
  devfs_unregister(sram_devfs_handle);
  iounmap((void *)sram_virtbase);
  unregister_chrdev(majornum, SRAMNAME);
}
