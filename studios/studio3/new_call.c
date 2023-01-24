#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/* NOTES:
 * ARM syscall table:
 *  syscall_base + 441 : SYS_noargs
 *  syscall_base + 442 : SYS_onearg
 *
 */


int main(int argc, char* argv[]) {
  // call sys_noargs
  uid_t uid = syscall(__NR_noargs);
  // call sys_onearg with int
  uid_t arg = 422;
  syscall(__NR_onearg, arg);
  
  return 0;
}
