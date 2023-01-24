#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <sys/select.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT 33333
#define LISTEN_QUEUE 10
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main() {
	/* setup server socket */
	int sfd, cfd;	// server socket and accepted client socket
	struct sockaddr_in addr;

	if ( -1 == (sfd = socket(AF_INET, SOCK_STREAM, 0))) {
		handle_error("socket");
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (-1 == bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
		handle_error("bind");
	}
	printf("Created server socket\n");

	if (-1 == (listen(sfd, LISTEN_QUEUE))) {
		handle_error("listen");
	}
	printf("Listening for clients...\n");


	/* setup keyboard */
	fd_set readfds, writefds;
	int ready, nfds, fd, numRead, j;

	/* setup nfds */
	// Q: nfds doesn't have to worry about cfd right, we're multiplexing over sfd and stdin only
	nfds = STDIN_FILENO + 1;
	if (sfd > nfds)
		nfds = sfd + 1;


	/* SELECT MULTIPLEXING HERE */
	printf("Waiting for select multiplex...\n");
	while(1) {
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		// Set stdin and sfd
		FD_SET(STDIN_FILENO, &readfds);
		FD_SET(sfd, &readfds);

		ready = select(nfds, &readfds, &writefds, NULL, NULL);
		if (ready == -1) {
			perror("select");
			return -1;
		}

		printf("select ready!\n");
		
		// iterate through for any read events
		for (fd = 0; fd < nfds; ++fd) {
			if (FD_ISSET(fd, &readfds)) {
				// read event detected for sfd
				if (fd == sfd) {				
					FILE * cfp;

					printf("Client connected!\n");
					if (-1 == (cfd = accept(sfd, NULL, NULL))) {
						handle_error("accept");
					}
					cfp = fdopen(cfd, "w");

					printf("Sending data...\n");
					fprintf(cfp, "1+1=%d", 1+1);
					printf("Sent data\n");
					printf("Closing comms...\n");
					fclose(cfp);
					close(cfd);
					printf("Closed comms...\n");
				}
				// read event detected for stdin
				if (fd == STDIN_FILENO) {
					char buf[100];
					
					fscanf(stdin, "%s", buf);
					printf("Received: %s\n", buf);

					if (buf[0]=='q' && buf[1]=='u' && buf[2]=='i' && buf[3]=='t') {
						printf("Received quit, quitting...\n");
						return 0;
					}
				}

			}
		
		}

	}
}
