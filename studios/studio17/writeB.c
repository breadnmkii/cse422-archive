#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main () {
	FILE * fp;
	int i;
	
	for (i=1; i<21; i+=2) {
		sleep(1);
		fp = fopen("activeobj.fifo", "w");
		fprintf(fp, "%d\n", i);
		fclose(fp);
	}


	return 0;
}
