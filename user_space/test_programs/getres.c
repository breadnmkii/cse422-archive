#include <stdio.h>
#include <time.h>

int main() {

  struct timespec res;

  // Clock realtime resolution
  clock_getres(CLOCK_REALTIME, &res); 
  printf("CLOCK_REALTIME ns: %ld\n", res.tv_nsec); 
  printf("CLOCK_REALTIME s: %d\n", res.tv_sec);
  // Clock realtime coarse resolution
  clock_getres(CLOCK_REALTIME_COARSE, &res);
  printf("CLOCK_REALTIME_COARSE ns: %ld\n", res.tv_nsec);
  printf("CLOCK_REALTIME_COARSE ns: %d\n", res.tv_sec);
  // Clock tai resolution
  clock_getres(CLOCK_TAI, &res);
  printf("CLOCK_TAI ns: %ld\n", res.tv_nsec);
  printf("CLOCK_TAI s: %d\n", res.tv_sec);
  // Clock monotonic raw
  clock_getres(CLOCK_MONOTONIC_RAW, &res);
  printf("CLOCK_MONOTONIC ns: %ld\n", res.tv_nsec);
  printf("CLOCK_MONOTONIC s: %d\n", res.tv_sec);
  return 0;
}
