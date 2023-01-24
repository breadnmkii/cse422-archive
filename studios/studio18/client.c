#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SOCK_PATH "/tmp/422_serversock"
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char * argv[]) {
	int cfd;
	unsigned int i;
	struct sockaddr_un serv_addr;

	/* create client socket */
	cfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (cfd == -1) {
		handle_error("socket");
	}
	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path, SOCK_PATH, sizeof(serv_addr.sun_path) -1);

	/* connect to server socket */
	if (connect(cfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_un)) == -1) {
		handle_error("connect");
	}
	printf("Successfully connected to server!\n");

	/* fopen fd for sending data */
	FILE * cfp;
	cfp = fdopen(cfd, "w");

	printf("Sending data...\n");
	/* send data to server socket */
	for (i=0; i<10; ++i) {
		if (fprintf(cfp, "%d", i) == -1) {
			handle_error("fprintf");
		}
	}
	fprintf(cfp, "\n", 0); // flush integer writes

	// printf("%d %d\n", argc==2, strcmp(argv[1],"quit")==0);
	if (argc == 2 && strcmp(argv[1],"quit") == 0) {
		printf("Sending terminate...\n");
		fprintf(cfp, "%d", 418);
	}
	
	/* close comms */
	printf("Closing client...\n");
	fclose(cfp);
	close(cfd);
	printf("Client closed!\n");

	return 0;
}
