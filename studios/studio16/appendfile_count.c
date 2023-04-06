#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char * argv[]) {
  if (argc-1 < 2) {
    printf("usage: %s <filename> <args to append...>\n", argv[0]);
    return -1;
  }

  FILE * fp;
  unsigned long len = 0;
  unsigned long i;
  
  fp = fopen(argv[1], "a");

  // calculate write len
  for (i=2; i<argc; ++i) {
    len += strlen(argv[i])+1; // +1 for space char
  }
  len += 1; // +1 for newline
   
  // write to file
  fprintf(fp, "%lu ", len);
  for (i=2; i<argc; ++i) {    
    fprintf(fp, "%s ", argv[i]);
  }
  fprintf(fp, "\n");

  fclose(fp);

  return 0;
}
