#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]){
	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET_ADDRSTRLEN];

	if(argc != 3){
		fprintf(stderr, "usage: server port path_to_root");
		return 1;
	}

	
	printf("Dev Branch\n");
	return 0;
}