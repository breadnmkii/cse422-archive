#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
  if (argc-1 < 2) {
    printf("usage: %s <filename> <args to append...>\n", argv[0]);
    return -1;
  }

  FILE * fp;
  int i;

  fp = fopen(argv[1], "a");

  for (i=2; i<argc; ++i) {
    fprintf(fp, "%s ", argv[i]);
  }
  fprintf(fp, "\n");

  fclose(fp);

  return 0;
}
