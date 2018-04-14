#ifndef HEAD_INCLUDE
//Libraries
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

//Custom Files

#define QUEUE 10	// How many connections can be queued 
					// (How many simultaneous incoming connections)

#define MAXDATASIZE 256 // the maximum size we can get at once

#define HEAD_INCLUDE 1
#endif

void processRequest(char *request, int socket){
	int request_size = strlen(request);
	char* s = malloc(request_size * sizeof(char));
	strcpy(s, request); // copy it to local so nothing can mess with it.
	char** req = malloc(MAXDATASIZE*sizeof(char*));
	char* token;
	char separators[] = " \n";

	int i = 0;
	req[0] = (token = strtok(s, separators));
	while(token != NULL){
		req[++i] = (token = strtok(NULL, separators));
	}
	
}