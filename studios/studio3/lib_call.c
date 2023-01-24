#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main() {
  // Read uid
  uid_t uid = getuid();
  printf("UID: %d\n", uid);
  // Attempt to set uid
  uid_t user_uid = 1000;
  if (setuid(user_uid) < 0) {
    printf("Error: %s with errno %d\n", strerror(errno), errno);
    return -1;
  }
  printf("SET_UID: %d\n", getuid());

  return 0;
}
