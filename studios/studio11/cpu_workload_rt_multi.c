#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct sched_param param;

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("usage: %s <cpunum> <prioritynum> <numtasks>\n", argv[0]);
		return -1;
	}
	int cpu = atoi(argv[1]);
	int priority = atoi(argv[2]);
	int tasks = atoi(argv[3]);

	// guard input range
	if (cpu < 0 || cpu > 3) {
		printf("cpu must be within 0-3 inclusive\n");
		return -1;
	}
	if (priority < sched_get_priority_min(SCHED_RR) || priority > sched_get_priority_max(SCHED_RR)) {
		printf("priority must be within %d-%d for SCHED_RR policy\n", sched_get_priority_min(SCHED_RR), sched_get_priority_max(SCHED_RR));
		return -1;
	}
	if (tasks < 1 || tasks > 10) {
		printf("number of tasks to spawn must be 1-10\n");
		return -1;
	}

	// set param priority and set sched
	param.sched_priority = priority;
	if (-1 == sched_setscheduler(getpid(), SCHED_RR, &param)) {
		perror("Could not schedule: ");
		return -1;
	}

	// spawn multiple tasks
	while (tasks > 1) {
		if (fork() == 0) {
			// child process
			break;
		}
		--tasks;
	}

	cpu_set_t *cpuset = CPU_ALLOC(sizeof(cpu_set_t));
	printf("alloced cpuset\n");
	CPU_ZERO(cpuset);
	printf("init cpuset\n");
	CPU_SET(atoi(argv[1]), cpuset);
	printf("added cpus\n");
	sched_setaffinity(getpid(), sizeof(cpu_set_t), cpuset);
	printf("Running on cpu %d...\n", atoi(argv[1]));

	int i;
	int workload;
	for (i=0; i<500000000; ++i) {
		workload = (21 + 1) * 7;
	}

	return 0;
}
