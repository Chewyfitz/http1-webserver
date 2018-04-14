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

#define MAXDATASIZE 256 // the maximum size we can get at once

// Macro for getting the address of the client
void *get_in_addr(struct sockaddr *sockaddr){
	if(sockaddr->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sockaddr)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sockaddr)->sin6_addr);
}

int main(int argc, char *argv[]){

	//socket stuff
	int sockfd, new_fd;	// sockfd is our listening socket, new_fd for new connections.
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;	// address information for new connections.
	socklen_t addr_len;
	struct sigaction sa;
	int yes = 1;
	char s[INET_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//client stuff
	char buf[MAXDATASIZE];
	int numbytes; // for recieved data

	if(argc < 3){
		fprintf(stderr, "not enough arguments\nusage: server port path_to_root\n");
		return 1;
	}

	char* port = argv[1];
	char* root = argv[2];

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
	
	while(1){ // Time to accept() some requests
		addr_len = sizeof(their_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_len);
		if(new_fd == -1){
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
		printf("server: got connection from %s\n", s);

		if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
			perror("recv");
			continue;
		}
		buf[numbytes] = '\0';

		printf("data recieved: %s\n", buf);

		close(new_fd);
	}
	

	printf("lolnope jk\n");
	return 0;
}