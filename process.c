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

void getExt(char* extension, char* filename){
	printf("1: %s\n", filename);
	int len= strlen(filename) -1;
	int i = 0;
	while((filename[len - (i++)] != '.') && len - i > 0) printf("2: %s\n", *(&filename + i));

	strcpy(extension, filename + i);
	printf("3: %s\n", extension);
}

void processRequest(char *request, int socket, char* pre_path){
	int request_size = strlen(request);
	char* s = malloc(request_size * sizeof(char));
	strcpy(s, request); // copy it to local so nothing can mess with it.
	char** req = malloc(MAXDATASIZE*sizeof(char*));
	char* token;
	char separators[] = " \n";
	char path[100];
	char ext[5];
	strcpy(path, pre_path);

	int i = 0;
	req[0] = (token = strtok(s, separators));
	while(token != NULL){
		req[++i] = (token = strtok(NULL, separators));
	}

	// Check if it's a GET request. (Only need to handle GET requests :^) )
	if(!(strcmp(req[0], "GET") == 0)){
		return;
	}

	//add the path of the requested file on to the webroot directory
	strcat(path, req[1]);

	getExt(ext, path); 	// custom function to get a pointer to just the
							// extension (so I know how to open the file)

	if(strcmp(ext, ".jpg") == 0){
		// Open the file as binary.
	} else {
		// Open the file as text.
	}

}