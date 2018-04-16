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

#include <assert.h>

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
	int len = 0;
	len = strlen(filename);

	if(len == 0){
		extension[0] = '/';
		extension[1] = '\0';
		return;
	}

	int i = 0;
	while((filename[len - (i) -1] != '.') && len - i > 0){
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
	send(sock, "\r\n\r\n", sizeof("\r\n\r\n")-1, 0);

}

void sendchar(FILE* file, int sock, char* ext){
	assert(file != NULL);
	//initialise this to a size
	int send_buff_size = MAXDATASIZE;
	char* send_buff = malloc(send_buff_size*sizeof(char));

	// hard-coding this for ease and control, but this could easily be 
	// made automatic by pulling ext and modifying the final 4 chars

	//ideally this functionality could be abstracted out into a separate function

	char* http_message;
	char* content_type;
	if (strcmp(ext, "css") == 0){
		//char http_message[] = "HTTP/1.0 200 OK\nContent-Type: text/css\r\n\r\n";
		content_type =  malloc(sizeof("text/css"));
		strcpy(content_type, "text/css");
	} else if (strcmp(ext, "js") == 0){
		//char http_message[] = "HTTP/1.0 200 OK\nContent-Type: text/javascript\r\n\r\n";
		content_type =  malloc(sizeof("text/javascript"));
		strcpy(content_type, "text/javascript");
	} else {
		//char http_message[] = "HTTP/1.0 200 OK\nContent-Type: text/html\r\n\r\n";
		content_type = malloc(sizeof("text/html"));
		strcpy(content_type, "text/html");
	}

	http_message = (char*)malloc((sizeof("HTTP/1.0 200 OK\nContent-Type: \r\n\r\n") 
									+ strlen(content_type))*sizeof(char));
	sprintf(http_message, "HTTP/1.0 200 OK\nContent-Type: %s\r\n\r\n", content_type);


	int i = 0;
	int just_read;
	while((just_read = fgetc(file)) != EOF){
		if(i >= send_buff_size){
			send_buff_size *= 2;
			send_buff = realloc(send_buff, send_buff_size);
			if(send_buff == NULL){
				exit(1);
			}
		}
		send_buff[i++] = just_read;
		printf("%c", just_read);
	}
	send_buff[i] = '\0';
	char* concat_message = malloc((strlen(http_message) + send_buff_size + 1) * sizeof(char));
	concat_message[0] = '\0';
	strcat(concat_message, http_message);
	strcat(concat_message, send_buff);
	printf("\n");
	printf("Sending %s", concat_message);


	send(sock, concat_message, (size_t)strlen(concat_message), 0);

	//send(sock, "\r\n\r\n", sizeof("\r\n\r\n")-1, 0);

	free(send_buff);
}

void sendbinary(FILE* file, int sock){
	// TODO: Make this
}

//process and respond to a caught request.
void processRequest(char *request, int socket, char* pre_path){
	int request_size = strlen(request);
	char* s = malloc((request_size + 1) * sizeof(char));
	strcpy(s, request); // copy it to local so nothing can mess with it.
	int sock = socket;


	char** req = malloc(MAXDATASIZE*sizeof(char*));
	char* token;
	char separators[] = " \n";
	char path[MAXDATASIZE] = "\0";
	char ext[6] = "\0";
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
		char* temppath = malloc(strlen(path)*sizeof(char));
		strcpy(temppath, path+1);
		strcpy(path, temppath);
		free(temppath);
	}

	getExt(ext, path); 	// function to get a str pointer to just the
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

		sendchar(file, sock, "html");
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

		sendchar(file, sock, ext);
	}
	free(s);
	free(req);
	fclose(file);
}
