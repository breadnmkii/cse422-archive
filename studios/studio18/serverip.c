#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PORT 33333
#define LISTEN_QUEUE 10			// number of clients that can wait for connection
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main() {
	int sfd, cfd;
	struct sockaddr_in addr;
	
	/* create socket with syscall */
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		handle_error("socket");
	}

	/* print out hostname of network */
	int len = 50;
	char * hostname = malloc(sizeof(char) * len);
	gethostname(hostname, len);
	printf("Hostname: %s\n", hostname);
	
	memset(&addr, 0, sizeof(struct sockaddr_in));	// clear struct
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	/* associate socket with machine visible addr */
	if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
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
		cfd = accept(sfd, NULL, NULL);
		if (cfd == -1) {
			handle_error("accept");
		}
		++clientCount;

		/* print client ip info */
		struct sockaddr_in client_addr;
		unsigned int len = sizeof(struct sockaddr_in);
		getpeername(cfd, (struct sockaddr *)&client_addr, &len);

		printf("sfd: %d cfd: %d\n", sfd, cfd);
		printf("Client #%d connected!\n", clientCount);
		printf("Client IP address: %s\n", inet_ntoa(client_addr.sin_addr));
		printf("Client Port: %d\n", (int)ntohs(client_addr.sin_port));

		/* receive data from client */
		printf("Waiting on client data...\n");
		int recv_len;
		int data_len = 20;
		unsigned int * data = malloc(sizeof(unsigned int) * data_len);

		/* Notes: dealing with short reads */
		// while last byte is not EOF:
		// 	realloc with twice data_len
		//	read
		//	Q: should cfd ptr be at data last left on
		//	   and will read begin at end of old buff? or does it overwrite?
		if ((recv_len = read(cfd, data, sizeof(unsigned int) * data_len)) == -1) {
			handle_error("read");
		}

		// iterate through data
		int i;
		unsigned int recv_data;
		for (i=0; i<data_len; ++i) {
			// receive each unsigned int as host order
			recv_data = ntohs(data[i]);
			
			if (recv_data == '\n') {
				break;
			}
			if (recv_data == 418) {
				printf("received coffee terminate\n");
				break;
			}
			printf("received\n");
			printf("%d\n", recv_data);
		}

		// verify read
		printf("read %d bytes as %d uints\n", recv_len, recv_len/sizeof(unsigned int));

		/* free mem */
		free(data);

		/* close stream and client fd */
		close(cfd);
		printf("Client disconnected!\n");

		if (recv_data == 418) {
			break;
		}
	}

	/* destroy local server socket connection */
	printf("Server closing...\n");
	close(sfd);
	printf("Server closed!\n");

	return 0;
}
