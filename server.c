/*
*	sever.c -- a simple HTTP/1.0 server.
*	Hopefully should be easy to adapt to newer versions of HTTP and expand to
*	more MIME types.
*
*	A large portion of this program is based off information from beej's guide
*	http://beej.us/guide/bgnet/
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#define QUEUE 10	// How many connections can be queued 
					// (How many simultaneous incoming connections)

int main(int argc, char *argv[]){

	int sockfd, new_fd;	// sockfd is our listening socket, new_fd for new connections.
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;	// address information for new connections.
	socklen_t addr_len;
	struct sigaction sa;
	int yes = 1;
	char s[INET_ADDRSTRLEN];
	int rv;

	if(argc < 3){
		fprintf(stderr, "not enough arguments\nusage: server port path_to_root\n");
		return 1;
	}

	char* port = argv[1];
	char* root = argv[2];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// loads up the nitty gritty of *servinfo
	if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// find a socket to bind to
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("server: socket");
			continue;
		}

		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}

		// we'll break as soon as we find one
		break;
	}

	// Here we've either got a bound socket, or no socket, so need to check.
	if (p == NULL){
		fprintf(stderr, "server: failed to bind socket\n");
		perror("recvfrom");
		exit(1);
	}

	freeaddrinfo(servinfo); // don't need this anymore.

	if(listen(sockfd, QUEUE) == -1){
		// listen errored
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");


	printf("lolnope jk\n");
	return 0;
}