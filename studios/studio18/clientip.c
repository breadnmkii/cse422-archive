#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 33333

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char * argv[]) {
	int cfd;
	int i;
	struct sockaddr_in serv_addr;

	if (argc != 2 && argc != 3) {
		printf("usage: %s <server ip address> (optional: 'quit')\n", argv[0]);
		return -1;
	}


	/* create client socket */
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd == -1) {
		handle_error("socket");
	}
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	inet_aton(argv[1], &serv_addr.sin_addr);

	/* connect to server socket */
	printf("Attempting to connect to server...\n");
	if (connect(cfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in)) == -1) {
		handle_error("connect");
	}


	// i accidentally modified this is studio19...
	// basically idea is to prepare your entire data with:
	// - core data
	// - flush delimiter (to signal to server to stop reading, like EOL)
	// this way server block on client write, then read until all data sent
	//
	// this assignment also checked for "quit" to send a special quit code
	// that terminated server.

	/* close comms */
	printf("Closing client...\n");
	// fclose(cfp);
	close(cfd);
	printf("Client closed!\n");

	return 0;
}
