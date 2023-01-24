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

// 500 million iterations should take several seconds to run
#define ITERS 500000000
#define UNLOCKED 0
#define LOCKED 1

volatile int lock_state = UNLOCKED;

int lock(volatile int* state) {
	int expected = UNLOCKED;
	int desired = LOCKED;
	// Spinlock, continuously attempt to lock and set desired to LOCKED
	while (!__atomic_compare_exchange(state, &expected, &desired, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
		expected = UNLOCKED;
	}
	return 0;
}

int unlock(volatile int* state) {
	int expected = LOCKED;
	int desired = UNLOCKED;
	// Unlock, if fails return
	if (!__atomic_compare_exchange(state, &expected, &desired, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
		return -1;
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
