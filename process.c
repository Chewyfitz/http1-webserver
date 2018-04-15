// Aidan Fitzpatrick (fitzpatricka)

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
	// This funciton will store the appropriate extension (after the .)
	// in char* extension. (or '/' if the root was requested).
	////printf("1: %s\n", filename);
	int len= strlen(filename) -1;
	int i = 0;
	while((filename[len - (i)] != '.') && len - i > 0){
		////printf("2: %c\n", filename[len - (i)]);
		i++;
	}

	strcpy(extension, filename + (len - i));
	////printf("3: %s\n", extension);

	if(strcmp(extension, filename) == 0){
		// no need to call strcpy for this, it's only one character...
		extension[0] = '/';
		extension[1] = '\0';
	}
	////printf("4: %s\n", extension);
}

void sendNotFound(int sock){

	char send_buff[] = "HTTP/1.0 404 Not Found\r\n";
	send(sock, send_buff, (size_t)strlen(send_buff)+1, 0);

}

void sendchar(FILE* file, int sock){
	//initialise this to a size
	int send_buff_size = MAXDATASIZE;
	char* send_buff = malloc(send_buff_size*sizeof(char));

	int i = 0;
	send_buff = "HTTP/1.0 200 OK\r\n";
	i = strlen(send_buff);
	while(!EOF){
		if(i >= send_buff_size){
			send_buff_size *= 2;
			send_buff = realloc(send_buff, send_buff_size);
			if(send_buff == NULL){
				exit(1);
			}
		}
		send_buff[i++] = fgetc(file);
		printf("%c", send_buff[i]);
	}
	printf("\n");
	printf("Sending %s", send_buff);
	send(sock, send_buff, (size_t)strlen(send_buff)+1, 0);
}

void sendbinary(FILE* file, int sock){
	// TODO: Make this
}

//process and respond to a caught request.
void processRequest(char *request, int socket, char* pre_path){
	int request_size = strlen(request);
	char* s = malloc(request_size * sizeof(char));
	strcpy(s, request); // copy it to local so nothing can mess with it.
	int sock = socket;


	char** req = malloc(MAXDATASIZE*sizeof(char*));
	char* token;
	char separators[] = " \n";
	char path[MAXDATASIZE];
	char ext[6];
	strcpy(path, pre_path);

	FILE *file = NULL;

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
	if(path[0] == '/'){
		path = path++;
	}

	getExt(ext, path); 	// custom function to get a str pointer to just the
						// extension (so I know how to open the file)

	// Do a fast comparison here, since it's the first one
	if(ext[0] == '/'){
		// Open root/index.html
		strcat(path, "index.html");
		file = fopen(path, "r");
		printf("%s\n", path);

		if(file == NULL){
			sendNotFound(sock);
			return;
		}

		sendchar(file, sock);
	} else if(strcmp(ext, ".jpg") == 0){
		// Open the file as binary. (.jpg)
		file = fopen(path, "rb");
		printf("%s\n", path);

		if(file == NULL){
			sendNotFound(sock);
			return;
		}

		sendbinary(file, sock);
	} else {
		// Open the file as text. (.html .css .js)
		file = fopen(path, "r");
		printf("%s\n", path);

		if(file == NULL){
			sendNotFound(sock);
			return;
		}

		sendchar(file, sock);
	}
	fclose(file);
}