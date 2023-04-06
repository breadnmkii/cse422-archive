#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#define iters 1000000

// array of static task_structs
static struct task_struct * arr_taskptr[4] = {0, 0, 0, 0};

// volatile int that can change outside of this execution context
volatile int shared_data = 0;
static int i;

static int boring_func(void * foo) {
	for (i=0; i<iters; i++) {
		shared_data++;
	}
		
	while(!kthread_should_stop()) {
		schedule();	
	}
	printk(KERN_INFO "stopped thread\n");	

	return 0;
}

static int mod_init(void) {	
	printk(KERN_ALERT "initializing core_thread module\n");
	for (i=0; i<4; ++i) {
		arr_taskptr[i] = kthread_create(boring_func, NULL, "kthread %d", i);
		if (!IS_ERR(arr_taskptr[i])) {
			printk(KERN_INFO "waking thread...\n");
			wake_up_process(arr_taskptr[i]);
			printk(KERN_INFO "binding thread to cpu %d\n", i);
			kthread_bind(arr_taskptr[i], i);
		}
	}	
	return 0;
}

static void mod_exit(void) {
	printk(KERN_INFO "shared data value: %d\n", shared_data);
	
	printk(KERN_ALERT "removing core_thread module\n");
	for (i=0; i<4; ++i) {
		kthread_stop(arr_taskptr[i]);
	}	
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("b.z.yang");
MODULE_DESCRIPTION("Studio 8 module");
