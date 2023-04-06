#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char * argv[]) {

	pid_t tgt_pid;
	struct rlimit limit;
	union sigval data;

	getrlimit(RLIMIT_SIGPENDING, &limit);
	printf("soft limit %d\n", limit.rlim_cur);

	if (argc != 3) {
		printf("usage: %d <PID> <sig_nr>\n", argv[0]);
		return -1;
	}

	if (strtol(argv[2], NULL, 0) > limit.rlim_cur) {	
		printf("Number of RT sigs exceed soft limit\n");
		return -1;
	}

	long argv1 = strtol(argv[1], NULL, 0);
	tgt_pid = (pid_t) argv1;

	int i;
	data.sival_int = 0;
	printf("sending to pid %d for %d times\n", strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10));
	for (i=0; i<strtol(argv[2], NULL, 0); ++i) {
		printf("%d\n", i);
		sigqueue(tgt_pid, SIGRTMIN, data);
	}

	data.sival_int = 1;
	sigqueue(tgt_pid, SIGRTMIN, data);

	return 0;
}
