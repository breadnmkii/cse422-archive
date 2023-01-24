/*    Simulated workload using OpenMP
 *
 * This program will create some number of seconds of work on each processor
 * on the system.
 *
 * This program requires the use of the OpenMP compiler flag, and that 
 * optimizations are turned off, to build correctly. E.g.: 
 * gcc -fopenmp workload.c -o workload
 */

#define _GNU_SOURCE
#include <stdio.h> // for printf()
#include <sched.h> // for sched_getcpu()
#include <unistd.h>
#include <sys/syscall.h> // for syscalls
#include <linux/futex.h> // for futex
#include <sys/time.h> 	 // for futex
#include <limits.h>	 // for INT_MAX constant

// 500 million iterations should take several seconds to run
#define ITERS 500000000
#define UNLOCKED 1
#define LOCKED 0

volatile int lock_state = UNLOCKED;

int lock(volatile int* state) {
	int ret_val;
	while (LOCKED > (ret_val = __atomic_sub_fetch(state, 1, __ATOMIC_ACQ_REL))) {
		// lock already claimed, sleep thread
		syscall(SYS_futex, state, FUTEX_WAIT, ret_val, NULL);
	}
	return 0;
}

int unlock(volatile int* state) {
	int ret_val;
	ret_val = __atomic_add_fetch(state, 1, __ATOMIC_ACQ_REL);
	if (ret_val != UNLOCKED) {
		// threads are sleeping for lock
		__atomic_store_n(state, 1, __ATOMIC_RELEASE);	// indicate lock is unlocked
		syscall(SYS_futex, state, FUTEX_WAKE, INT_MAX);	// wake sleepers
	}
	return 0;
}

void critical_section( void ){
	int index = 0;
	while(index < ITERS){ index++; }
}

int main (int argc, char* argv[]){

	// Create a team of threads on each processor
	#pragma omp parallel
	{
		// Each thread executes this code block independently
		lock(&lock_state);
		critical_section();
		unlock(&lock_state);

		printf("CPU %d finished!\n", sched_getcpu());
	}

	return 0;
}
