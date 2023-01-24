#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <sys/select.h>
#include <sys/epoll.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT 33333
#define LISTEN_QUEUE 10
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define MAX_BUF 1000
#define MAX_EVENTS 5

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


	/* setup epoll shittake */
	int epfd, ready;
	struct epoll_event ev;
	struct epoll_event evList[MAX_EVENTS];
	char buf[MAX_BUF];

	int numfds = 2;
	// create epoll instance with  epoll_create1()
	epfd = epoll_create1(0);
	if (epfd == -1)
		handle_error("epoll_create");

	// add stdin and server socket fds to epoll instance	
	int fds[numfds];
	fds[0] = STDIN_FILENO;
	fds[1] = sfd;
	
	for (i=0; i<numfds; ++i) {
		ev.events = EPOLLIN & EPOLLET;	// Q: modified with epollet results in epoll_wait never triggering?
		ev.data.fd = fds[i];
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[i], &ev) == -1)
			handle_error("epoll_ctl");
	}


	/* EPOLL MULTIPLEXING HERE */
	/* Notes:
	 *  Epoll_wait returns number of ready events, and evList
	 *  will be dynamically modified to represent ONLY the
	 *  available events.
	 *
	 */

	while(1) {
		ready = epoll_wait(epfd, evList, MAX_EVENTS, -1);
		if (ready == -1) {
			perror("poll");
			return -1;
		}
		printf("epoll ready!\n");
		
		// iterate through for any read events
		for (i = 0; i < ready; ++i) {
			if (evList[i].events & EPOLLIN & EPOLLET) {
				// stdin read event active
				if (evList[i].data.fd == STDIN_FILENO) {	
					printf("epoll event: stdin read\n");
				}
				else if (evList[i].data.fd == sfd) {				
					// client connection event active
					printf("epoll event: Client connected!\n");
					
					if (-1 == (cfd = accept(sfd, NULL, NULL))) {
						handle_error("accept");
					}
					// register cfd with epoll
					ev.events = EPOLLIN & EPOLLET;	// listen for only read events
					ev.data.fd = cfd;
					if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev) == -1)
						handle_error("epoll_ctl");
				}
				else if (evList[i].data.fd == cfd) {
					// client read event active
					printf("epoll event: Reading client\n");
				}
			}
			else if (evList[i].events && EPOLLRDHUP) {
				// client disconnect event active
				printf("epoll event: Disconnect client\n");
				if (i == 2) {
					// close comms
					close(cfd);

					printf("Closed comms...\n");
				}
			}	
		
		}

	}
	
	/* close server fd */
	close(sfd);

	return 0;
}
