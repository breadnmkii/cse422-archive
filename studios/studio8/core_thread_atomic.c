#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/types.h>

#define iters 1000000

// array of static task_structs
static struct task_struct * arr_taskptr[4] = {0, 0, 0, 0};

// atomic typed data that should be race condition safe
atomic_t shared_data;
static int i;

static int boring_func(void * foo) {
	pid_t tid = current->pid;
	printk(KERN_INFO "thread %d starting iteration\n", tid);
	for (i=0; i<iters; i++) {
		atomic_add(1, &shared_data);
//		printk(KERN_INFO "%d", atomic_read(&shared_data));
	}
	printk(KERN_INFO "thread %d ending iteration\n", tid);
	while(!kthread_should_stop()) {
		schedule();	
	}
	printk(KERN_INFO "stopped thread\n");	

	return 0;
}

static int mod_init(void) {
	printk(KERN_ALERT "initializing core_thread_atomic module\n");
	for (i=0; i<4; ++i) {
		printk(KERN_ALERT "THIS IS ONE LOOP ITERATION %d\n", i);
		arr_taskptr[i] = kthread_create(boring_func, NULL, "kthread %d", i);
		if (!IS_ERR(arr_taskptr[i])) {
			printk(KERN_INFO "waking thread...\n");
			wake_up_process(arr_taskptr[i]);
			printk(KERN_INFO "binding thread to cpu %d\n", i);
			kthread_bind(arr_taskptr[i], i);
		}
		else {
			printk(KERN_ALERT "could not create thread %d\n", i);
		}
	}
	printk(KERN_INFO "initializing atomic int shared_data to zero\n");
	atomic_set(&shared_data, 0);
	return 0;
}

static void mod_exit(void) {
	printk(KERN_INFO "shared data value: %d\n", atomic_read(&shared_data));
	
	printk(KERN_ALERT "removing core_thread module\n");
	for (i=0; i<4; ++i) {
		if (!IS_ERR(arr_taskptr[i])) {
			kthread_stop(arr_taskptr[i]);
		}
	}	
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("b.z.yang");
MODULE_DESCRIPTION("Studio 8 module");
