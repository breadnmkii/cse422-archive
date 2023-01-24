#define _GNU_SOURCE
#include <sys/types.h>
//#include <sys/syscall.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/* NOTES:
 * ARM generated syscall table: arch/arm/include/generated/uapi/asm/unistd-common.h
 * (include <asm/unistd.h>
 * __NR_setuid: __NR_SYSCALL_BASE + 23
 * __NR_getuid: __NR_SYSCALL_BASE + 24
 *
 *
 * ARM library syscall table: 
 * (include <sys/syscall.h>)
 *  syscall_base + 23 : SYS_setuid
 *  syscall_base + 24 : SYS_getuid
 *
 */


int main(int argc, char* argv[]) {
  // Read uid via syscall
  uid_t uid = syscall(__NR_getuid);
  printf("UID: %d\n", uid);
  // Attempt to set uid via syscall
  uid_t user_uid = 1000;
  if (syscall(__NR_setuid, user_uid) < 0) {
    printf("Error: %s with errno %d\n", strerror(errno), errno);
    return -1;
  }
  printf("Set UID: %d\n", syscall(__NR_getuid)); 
  
  return 0;
}
