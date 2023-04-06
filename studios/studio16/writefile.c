#include <stdio.h>
#include <unistd.h>


int main(int argc, char * argv[]) {
  if (argc-1 < 2) {
    printf("usage: %s <filename> <args to write...>\n", argv[0]);
    return -1;
  }

  FILE * fd;
  int i;

  fd = fopen(argv[1], "w");

  for (i=2; i<argc; ++i) {
    fprintf(fd, "%s\n", argv[i]);
  }

  fclose(fd);

  return 0;
}
