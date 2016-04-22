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
static unsigned long sram_size = ((unsigned long) SRAMSIZE);
static u32* mmu_physaddr  = ((u32 *) MMUBASE);
static int majornum;

static int sram_read  (struct file *filp, char *buf, size_t count, loff_t *unused_loff_t);
static int sram_write (struct file *filp, const char *buf, size_t count, loff_t *unused_loff_t);
static loff_t sram_llseek  (struct file *filp, loff_t off, int whence);
static int sram_open (struct inode *inode, struct file *filp);
static int sram_release (struct inode *inode, struct file *filp);

static struct file_operations sram_fops = {
  owner   :   THIS_MODULE,
  read	  :   sram_read,
  write   :   sram_write,
  llseek  :   sram_llseek,
  open    :   sram_open,
  release :   sram_release,
};

MODULE_AUTHOR("Jochen Klein");
MODULE_DESCRIPTION("SRAM module");
MODULE_LICENSE("GPL");

static int sram_read(struct file *filp, char *buf, size_t count, loff_t *unused_loff_t)
{
  unsigned long f_pos = filp->f_pos;

  if (f_pos > sram_size)
    return 0;

  if (f_pos + count > sram_size)
    count = sram_size - f_pos;

  copy_to_user(buf, (unsigned char *) sram_virtbase + f_pos, count);

  filp->f_pos += count;
  return count;
}

static int sram_write (struct file *filp, const char *buf, size_t count, loff_t *unused_loff_t)
{
  unsigned long f_pos = filp->f_pos;

  if (f_pos > sram_size)
    return 0;

  if (f_pos + count > sram_size)
    count = sram_size - f_pos;

  copy_from_user((unsigned char *) sram_virtbase + filp->f_pos, buf, count);

  filp->f_pos += count;
  return count;
}

loff_t sram_llseek  (struct file *filp, loff_t off, int whence)
{
  loff_t newpos;

  switch (whence) {
  case 0: /* SEEK_SET */
    newpos = off;
    break;

  case 1: /* SEEK_CUR */
    newpos = filp->f_pos + off;
    break;

  case 2: /* SEEK_END */
    newpos = sram_size + off;
    break;

  default:
    return -EINVAL;
  }

  if (newpos < 0)
    return -EINVAL;

  newpos %= sram_size;

  filp->f_pos = newpos;

  return newpos;
}

static int sram_open (struct inode *inode, struct file *filp)
{
  int minor = MINOR(inode->i_rdev);
  if (minor != 0) {
    printk("Only minor 0 actual\n");
    return -ENODEV;
  }
  if (MOD_IN_USE)
    return -EBUSY;
  MOD_INC_USE_COUNT;
  return 0;
}

static int sram_release (struct inode *inode, struct file *filp)
{
  MOD_DEC_USE_COUNT;
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

  // CHECK: map SRAM to address space
  u32* mmu_virtbase  = (u32*) ioremap_nocache((u32)mmu_physaddr, MMUSIZE);
  mmu_virtbase[0x40] = 1;
  iounmap((void*) mmu_virtbase);

  // CHECK: remapping of SRAM
  sram_virtbase  = (u32*) ioremap_nocache((u32)sram_physaddr, SRAMSIZE);
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
