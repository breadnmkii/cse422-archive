#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int getLineLen(FILE * fp);

int main(int argc, char * argv[]) {
  if (argc-1 != 1) {
    printf("usage: %s <filename>\n", argv[0]);
    return -1;
  }

  FILE * fp;
  char * data;
  ssize_t read;
  unsigned long len, maxlen = 0;
  unsigned long i;
  
  fp = fopen(argv[1], "r");

  while (!feof(fp)) {
    len = getLineLen(fp); 
    if (len > maxlen) {
      if (maxlen != 0) {
        data = (char *)realloc(data, sizeof(char) * len);
      } else {
        data =(char *)malloc(sizeof(char) * len);
      }
      maxlen = len;
    }
    getline(&data, &len, fp); 
    printf("%s", data);
  } 

  fclose(fp);
  free(data);

  return 0;
}

int getLineLen(FILE * const fp) {
  int len = 0;
  int c;
  
  while (1) {
    c = fgetc(fp);
    if (c == EOF || c == ' ') {
      break;
    }
    
    len *= 10;
    len += c - '0'; 
  }
  
  return len;
}
