#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>

#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/nsproxy.h>
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/dcache.h>
#include <linux/pid.h>

static struct task_struct * kthread = NULL;
static int tgt_pid;
module_param(tgt_pid, int, 0);

static int
thread_fn (void * data) {
  struct dentry * d;
  char * msg;
  int len;

  struct pid * tgtpid_task;
  struct task_struct * pidtask;
  struct files_struct * files;
  struct file * file;
  const struct file_operations * fileop;
  loff_t offset = 0;


  printk("fs: %x\n", (unsigned int)(current->fs));
  printk("files: %x\n", (unsigned int)(current->files));
  printk("nsproxy: %x\n", (unsigned int)(current->nsproxy));

  printk("pwd: %x\n", (unsigned int)(current->fs->pwd.dentry));
  printk("root: %x\n", (unsigned int)(current->fs->root.dentry));

  if (current->fs->pwd.dentry == current->fs->root.dentry) {
    printk("same d_iname: %s\n", current->fs->pwd.dentry->d_iname);
  } else {
    printk("pwd d_iname: %s\n", current->fs->pwd.dentry->d_iname);
    printk("root d_iname: %s\n", current->fs->root.dentry->d_iname);
  }

  printk("printing subdirs of root dir:\n");
  list_for_each_entry(d, &(current->fs->root.dentry->d_subdirs), d_child) {
    if (!list_empty(&(d->d_subdirs))) {
      printk("%s\n", d->d_iname);
    } 
  }

  // get target pid struct
  printk("targetting pid of %d\n", tgt_pid);
  tgtpid_task = find_get_pid(tgt_pid);
  if (tgtpid_task == NULL) {
    printk(KERN_ERR "pidtask is null!\n");
    return -1;
  }
  // get target pid task
  pidtask = get_pid_task(tgtpid_task, PIDTYPE_PID);
  printk("got pidtask\n");
  if (pidtask == NULL) {
    printk( KERN_ERR "Could not get pid\n");
    return -1;
  }
  // set files_struct to task_struct member
  printk("setting files_struct\n");
  files = pidtask->files;
  // set file
  printk("setting file\n");
  file = fcheck_files(files, 1);
  if (file == NULL) {
    printk(KERN_ERR "file is null!\n");
    return -1;
  }
  // set fileop to file member
  printk("setting fileop\n");
  fileop = file->f_op;
  if (fileop == NULL) {
    printk(KERN_ERR "fileop is null!\n");
    return -1;
  }
  // write to target pid stdout
  printk("writing to tgt stdout\n");
  msg = "Hello targeted process!";
  len = strlen(msg) + 1;
  printk("error?\n");
  fileop->write(file, msg, len, &offset);

  return 0;
}


static int
vfs_module_init (void) {
  // create thread
  printk(KERN_INFO "Loaded vfs_thread module\n");

  kthread = kthread_create(thread_fn, NULL, "vfs_thread_name");
  if (IS_ERR(kthread)) {
    printk( KERN_ERR "Failed to create kernel thread\n");
    return PTR_ERR(kthread);
  }

  wake_up_process(kthread); 

  return 0;
}


static void
vfs_module_exit (void) {
  printk(KERN_INFO "Unloaded vfs_thread module\n");
}


// Module binding
module_init(vfs_module_init);
module_exit(vfs_module_exit);

MODULE_LICENSE ("GPL");
