#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SOCK_PATH "/tmp/422_serversock"	// idk??
#define LISTEN_QUEUE 10			// number of clients that can wait for connection
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main() {
	int sfd, cfd;
	struct sockaddr_un addr;

	/* create socket with syscall */
	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1) {
		handle_error("socket");
	}
	
	memset(&addr, 0, sizeof(struct sockaddr_un));	// clear struct
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) -1);

	/* associate socket with machine visible add */
	if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
		handle_error("bind");
	}
	printf("Successfully created socket!\n");

	/* listen for client connections */
	if (listen(sfd, LISTEN_QUEUE) == -1) {
		handle_error("listen");
	}
	printf("Listening for clients...\n");


	int clientCount = 0;

	while(1) {
		/* accept incoming conenctions */
		//peer_addr_size = sizeof(struct sockaddr_un);
		cfd = accept(sfd, NULL, NULL);
		if (cfd == -1) {
			handle_error("accept");
		}
		++clientCount;
		printf("Client #%d connected!\n", clientCount);

		/* open stream interface on client fd */
		FILE * cfp;
		unsigned int data;

		cfp = fdopen(cfd, "r");

		while (fscanf(cfp, "%d", &data) != -1) {
			if (data == 418) {
				printf("received coffee terminate\n");
				break;
			}
			printf("received\n");
			printf("%d\n", data);
		};

		/* close stream and client fd */
		fclose(cfp);
		close(cfd);
		printf("Client disconnected!\n");

		if (data == 418) {
			break;
		}
	}

	/* destroy local server socket connection */
	printf("Server closing...\n");
	unlink(SOCK_PATH);
	close(sfd);
	printf("Server closed!\n");

	return 0;
}
