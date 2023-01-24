#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  int fd[2];
  FILE * fp;
  
  if (pipe(fd) != 0) {
  	perror("error");
  	return -1;
  }
  if (fork() == 0) {	
	close(fd[0]); // close reader
	fp = fdopen(fd[1], "w"); // open writer
	fprintf(fp, "%s", "Hello, world!\n");
	fprintf(fp, "%s", "Are you getting this?\n");
	fclose(fp); // close filepointer writer
  }
  else {
	char buffer[128];
	close(fd[1]); // close writer
	fp = fdopen(fd[0], "r"); // open reader
	
	/* while buffer still has data */
	while (fgets(buffer, 128, fp) != NULL ) {
	  printf("%s", buffer);
	};
	fclose(fp); // close filepointer reader
  }

  return 0;
}
