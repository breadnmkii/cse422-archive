// SPDX-License-Identifier: GPL-2.0
/*
 * Sample kobject implementation
 *
 * Copyright (C) 2004-2007 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2007 Novell Inc.
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>

/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "foo", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */

static int pidme;

/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t pidme_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", pidme);
}

static ssize_t pidme_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int ret;
	int maxDepth;	// maximum number of times to attempt to find parent of pid
	struct pid* rpid;
	struct task_struct* curr_task;
        printk("pidme before update: %d\n", pidme);
	ret = kstrtoint(buf, 10, &pidme);
        printk("pidme after update: %d\n", pidme);
	if (ret < 0)
		return ret;
        
        // attempt to print ancestry of supplied pidme pid
        rpid = find_vpid(pidme);
        printk("Family pids of %d\n", pidme);
	if (rpid != NULL) {
		curr_task = get_pid_task(rpid, PIDTYPE_PID);
		if (curr_task != NULL) {
			pid_t this_pid = curr_task->pid;
			maxDepth = 500;
			while ((this_pid > 1) && (maxDepth > 0)) {
				printk("pid %d with command %s\n", this_pid, curr_task->comm);
				curr_task = curr_task->real_parent;
				this_pid = curr_task->pid;
				--maxDepth;
			}
			printk("(init/root) pid %d with command %s\n", this_pid, curr_task->comm);
		}
        }        

	return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute pidme_attribute =
	__ATTR(fam, 0664, pidme_show, pidme_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
	&pidme_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *example_kobj;

static int __init example_init(void)
{
	int retval;

	/*
	 * Create a simple kobject with the name of "kobject_example",
	 * located under /sys/kernel/
	 *
	 * As this is a simple directory, no uevent will be sent to
	 * userspace.  That is why this function should not be used for
	 * any type of dynamic kobjects, where the name and number are
	 * not known ahead of time.
	 */
	example_kobj = kobject_create_and_add("fam_reader", kernel_kobj);	// modified with 'fam_reader' kobject
	if (!example_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");
