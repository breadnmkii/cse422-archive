#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void fork_tree(int curr_gen, int generations);

int main(int argc, char **argv) {
  // Input sentinel
  if (argc != 2 || ((int)atoi(argv[1]) < 1 || (int)atoi(argv[1]) > 10)) {
    printf("Usage: supply one integer parameter between 1-10 inclusive\n");
    return -1;
  }
  // Tree fork code
  int generations = (int)atoi(argv[1]);
  int gen = 0;

  while (gen < generations) {
    pid_t parent = getpid();
    pid_t left, right;
    printf("Gen %d  PID %d\n", gen+1, parent);
    ++gen;
    if (!(gen < generations))
      break;

    left = fork();
    if (left < 0) {
      perror("fork");
      exit(-1);
    }
    else if (left == 0) {
      // Left child
      continue;
    }

    right = fork();
    if (right < 0) {
      perror("fork");
      exit(-1);
    }
    else if (right == 0) {
      // Right child
      continue;
    }
    break;
  }
  wait(NULL);
  exit(0);
}

