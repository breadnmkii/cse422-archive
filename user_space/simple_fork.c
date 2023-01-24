#include <stdio.h>
#include <error.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  printf("As Parent, Parent pid: %d\n", getpid());
  pid_t pid = fork();
  
  if (pid > 0) {
    printf("As Parent, Child pid: %d\n", pid);
  }
  else if (pid == 0) {
    printf("As Child, Child pid: %d\n", getpid());
    printf("As Child, Parent pid: %d\n", getppid());
  }
  else {
    perror("Fork error");
  }

  return 0;
}
