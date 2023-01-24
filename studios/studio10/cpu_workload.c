#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s <cpunum>\n", argv[0]);
		return -1;
	}
	printf("received input of %d\n", atoi(argv[1]));
	cpu_set_t *cpuset = CPU_ALLOC(sizeof(cpu_set_t));
	printf("alloced cpuset\n");
	CPU_ZERO(cpuset);
	printf("init cpuset\n");
	CPU_SET(atoi(argv[1]), cpuset);
	printf("added cpus\n");
	sched_setaffinity(getpid(), sizeof(cpu_set_t), cpuset);
	printf("Running on cpu %d...\n", atoi(argv[1]));
	while (1) {}

	return 0;
}
