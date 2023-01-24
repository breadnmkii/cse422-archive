#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage: %s number_of_bytes\n", argv[0]);
    return -1;
  }

  int bytes = atoi(argv[1]);
  char* mem[10];

  for (int i=0; i<10; ++i) {
    mem[i] = (char *) malloc(bytes * sizeof(char));
  }

  for (int i=0; i<10; ++i) {
    free(mem[i]);
  }

  return 0;
}
