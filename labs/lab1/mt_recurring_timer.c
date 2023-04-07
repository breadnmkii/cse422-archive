#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>
#include <linux/sched.h>

// module parameters
static unsigned long log_sec = 0;
static unsigned long log_nsec = 1000000000;

// kernel thread taskptrs, one for each core of cpu
static struct task_struct * arr_threads[4];

module_param(log_sec, ulong, 0644);
module_param(log_nsec, ulong, 0644);

struct hrtimer timer;
static ktime_t interval;

// thread pointer

static enum hrtimer_restart timer_expire(struct hrtimer *timer) {
	int i;
	
	// wake threads here
	for (i=0; i<4; ++i) {
		printk(KERN_INFO "waking task %d\n", i);
		wake_up_process(arr_threads[i]);
	}
	printk(KERN_INFO "forwarding timer\n");
	hrtimer_forward_now(timer, interval);
	return HRTIMER_RESTART;
}

static int spawn_func(void * param) {
	int iters;	

	printk(KERN_INFO "ran spawn_func()\n");
	
	iters = 0;
	while(true) {
		++iters;
		printk(KERN_INFO "iters: %d\n", iters);
		printk(KERN_INFO "nvcsw: %lu, nivcsw: %lu\n", current->nvcsw, current->nivcsw);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		if (kthread_should_stop()) {
			printk(KERN_ALERT "terminating thread\n");
			break;
		}
	}
	
	return 0;
}

static int mod_init(void) {
	int i;

	printk(KERN_ALERT "initializing recurring timer module\n");
	interval = ktime_set(log_sec, log_nsec);
	
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer.function = timer_expire;
	
	// create and bind 4 threads here		
	for (i=0; i<4; ++i) {
		printk(KERN_INFO "creating thread %d\n", i);
		arr_threads[i] = kthread_create(spawn_func, NULL, "kthread %d", i);
		if (!IS_ERR(arr_threads[i])) {
			printk(KERN_INFO "binding thread to cpu %d\n", i);
			kthread_bind(arr_threads[i], i);
			printk(KERN_INFO "waking thread...\n");
			wake_up_process(arr_threads[i]);
		}
	}			
	
	printk(KERN_ALERT "starting timer...\n");
	hrtimer_start(&timer, interval, HRTIMER_MODE_REL);

	return 0;
}

static void mod_exit(void) {
	int i;

	printk(KERN_ALERT "stopping timer...\n");
	hrtimer_cancel(&timer);
	
	// stop threads here
	for (i=0; i<4; ++i) {
		printk(KERN_ALERT "stopping thread %d...\n", i);
		kthread_stop(arr_threads[i]);
	}
	
	printk(KERN_ALERT "removing recurring timer module\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("b.z.yang");
MODULE_DESCRIPTION("A module... for recurring timers");
