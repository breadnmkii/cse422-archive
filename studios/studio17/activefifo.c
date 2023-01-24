#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
	FILE * fp, * fpw;
	char * fifopath = "activeobj.fifo";
	int len = 100;
	char * buffer = malloc(sizeof(char)*len);
	int data;

	if (-1 == mkfifo(fifopath, S_IRUSR | S_IWUSR)) {
		perror("error");
		return -1;
	}
	fp = fopen(fifopath, "r");
	fpw = fopen(fifopath, "w");

	while (!feof(fp)) {
		getline(&buffer, &len, fp);
		printf("%s\n", buffer);
		// fscanf(fp, "%s", &data);
		// printf("original: %d, modified: %d\n", data, data*2); 
	}

	free(buffer);
	fclose(fp);
	fclose(fpw);
}
