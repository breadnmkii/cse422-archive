#include <time.h>
#include <stdio.h>

int main() {

  struct timespec start;
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("Elapsed time for clock_gettime(): %ld\n", end.tv_nsec-start.tv_nsec);

  return 0;
}
