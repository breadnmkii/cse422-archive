#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <poll.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT 33333
#define LISTEN_QUEUE 10
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


/* struct pollfd {
 * 	int fd;
 * 	short events;
 * 	short revents
 * };
 */

int main() {
	int i;
	
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

	/* setup poll shittake */
	int numfds = 2;
	int fds[3];	// three fds for keyboard, server, and client
	struct pollfd * polls;

	polls = calloc(3, sizeof(struct pollfd));
	if (polls == NULL)
		handle_error("malloc");

	// open fd pointers
	fds[0] = STDIN_FILENO;
	fds[1] = sfd;

	// build polls fd list
	for (i=0; i<2; ++i) {
		polls[i].fd = fds[i];
		polls[i].events = POLLIN;
	}

	/* POLL MULTIPLEXING HERE */
	int ready;
	printf("Waiting for poll multiplex...\n");
	while(1) {
		FILE * cfp;
		char buf[100];

		ready = poll(polls, numfds, -1);
		if (ready == -1) {
			perror("poll");
			return -1;
		}
		printf("poll ready!\n");
		
		// iterate through for any read events
		for (i = 0; i < numfds; ++i) {
			if (polls[i].revents & POLLIN) {
				// stdin read event active
				if (polls[i].fd == STDIN_FILENO) {	
					printf("poll event: stdin read\n");
					fscanf(stdin, "%s", buf);
					printf("Received: %s\n", buf);

					if (buf[0]=='q' && buf[1]=='u' && buf[2]=='i' && buf[3]=='t') {
						printf("Received quit, quitting...\n");

						/* cleanup */
						close(sfd);
						if (cfd) {
							fclose(cfp);	
							close(cfd);
						}
						return 0;
					}
				}
				else if (polls[i].fd == sfd) {				
					// client connection event active
					printf("poll event: Client connected!\n");
					
					if (-1 == (cfd = accept(sfd, NULL, NULL))) {
						handle_error("accept");
					}
					// register cfd with polls
					polls[2].fd = cfd;
					polls[2].events = POLLIN;
					++numfds;
					
					// open cfd for reading
					cfp = fdopen(cfd, "r");
				}
				else if (polls[i].fd == cfd) {
					// client read event active
					printf("poll event: Reading client\n");

					fscanf(cfp, "%s", buf);
					printf("Received: %s\n", buf);

					/* test getline */
					//int n = 100;
					//char * lineptr = malloc(sizeof(char) * n);
					//getline(&lineptr, &n, cfp);
					//printf("Received: %s\n", lineptr);
						
					if (buf[0]=='q' && buf[1]=='u' && buf[2]=='i' && buf[3]=='t') {
						printf("Received quit, quitting...\n");

						/* cleanup */
						fclose(cfp);
						close(cfd);
						close(sfd);

						return 0;
					}
				}
			}

			// Q: THIS ISNT DETECTING THE CLIENT DISCONENCT??
			if (polls[i].revents & POLLRDHUP) {
				// client disconnect event active
				if (polls[i].fd == cfd) {
					// close comms
					fclose(cfp);
					close(cfd);
					--numfds;

					printf("Closed comms...\n");
				}
			}	
		
		}

	}
	
	/* close server fd */
	close(sfd);

	return 0;
}
