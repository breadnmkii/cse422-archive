#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
  if (argc-1 < 2) {
    printf("usage: %s <filename> <args to write...>\n", argv[0]);
    return -1;
  }

  int fd, i;
  const char * newline = "\n";

  fd = open(argv[1], O_WRONLY | O_CREAT);

  for (i=2; i<argc; ++i) {
    write(fd, argv[i], strlen(argv[i]));
    write(fd, newline, 1);
  }

  close(fd);

  return 0;
}
