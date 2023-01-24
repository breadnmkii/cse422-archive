#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
	if (argc != 2) {
		printf("usage: %s <filename>\n", argv[0]);
		return -1;
	}

	char *buffer;
	FILE *fp, *ao;
	int len = 100;

	buffer = malloc(sizeof(char)*len);
	fp = fopen(argv[1], "r");
	ao = fopen("activeobj.fifo", "w");

	if (fp == NULL || ao == NULL) {
		perror("error");
		return -1;
	}

	while (!feof(fp)) {
		getline(&buffer, &len, fp);	// read all data until eof 
		printf("%s\n", buffer);
		fprintf(ao, "%s\n", buffer);
	}

	free(buffer);
	fclose(fp);
	fclose(ao);	

	return 0;
}
