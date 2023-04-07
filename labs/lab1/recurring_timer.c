#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>

static unsigned long log_sec = 0;
static unsigned long log_nsec = 1000000000;

module_param(log_sec, ulong, 0);
module_param(log_nsec, ulong, 0);

struct hrtimer timer;
static ktime_t interval;

// thread pointer
static struct task_struct * taskptr = 0;

static enum hrtimer_restart timer_expire(struct hrtimer *timer) {
	printk(KERN_INFO "waking task\n");
	wake_up_process(taskptr);	
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
	printk(KERN_ALERT "initializing recurring timer module\n");
	interval = ktime_set(log_sec, log_nsec);
	
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer.function = timer_expire;
	
	taskptr = kthread_run(spawn_func, NULL, "hrtimer thread %d", 333);
	if (!taskptr) {
		printk(KERN_ALERT "could not run thread!\n");
		return -1;
	}
	
	printk(KERN_ALERT "starting timer...\n");
	hrtimer_start(&timer, interval, HRTIMER_MODE_REL);

	return 0;
}

static void mod_exit(void) {
	printk(KERN_ALERT "stopping timer...\n");
	hrtimer_cancel(&timer);
	printk(KERN_ALERT "stopping thread...\n");
	kthread_stop(taskptr);	

	printk(KERN_ALERT "removing recurring timer module\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("b.z.yang");
MODULE_DESCRIPTION("A module... for recurring timers");
