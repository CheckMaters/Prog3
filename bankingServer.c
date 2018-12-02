#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define PORT 9444



int main () {
	int server_fd, *valread, client_fd;
	struct sockaddr_in address, client_address;
	int address_len = sizeof(address);
	int client_len = sizeof(client_address);
	bzero((char *) &address, address_len);
	bzero((char *) &client_address, client_len);
	int opt = 1;
	char buffer[1024];
	bzero(buffer, 1024);

//opening socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Error! Socket Failed.\n");
		return -1;
	}

/*
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		fprintf(stderr, "Error! Set socket Failed.\n");
		return -1;
	}
*/

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if(bind(server_fd,(struct sockaddr*) &address, address_len)){
		fprintf(stderr, "Error! Unable to bind server_fd to server addresss.\n");
		return -1;
	}

	if(listen(server_fd, 100) < 0){
		fprintf(stderr, "Error! Listening.");
		return -1;
	}


	
	while((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len))) {
		
		pthread_t thread;
		valread = malloc(1);
		*valread = client_fd;
		
		if(pthread_create(&thread, NULL, &connection_handler, (void *) client_fd) < 0) {
			fprintf(stderr, "Error! Unable to create thread for this client.\n");
			return -1;
		}	
	}
		
	if(client_fd < 0) {
		fprintf(stderr, "Error! Uable to accept Client Socket.\n");
		return -1;
	}	
	
/*	
	if((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len))<0){
		fprintf(stderr, "Error! Unable to accept Client Socket.\n");
		return -1;
	}

	valread = read(client_fd, buffer, 1024);
	if(valread < 0) {
		fprintf(stderr, "Error! Reading from Client Socket.\n");	
		return -1;
	}
	printf("%s\n", buffer);
	bzero(buffer, 1024);
	strcpy(buffer, "Got you Message.");
	valread = write(client_fd, buffer, strlen(buffer));
	if(valread < 0){
		fprintf(stderr, "Error! Writing to Client Socket.\n");
	}
*/
	return 0;


}






void * connection_handler (void * socket_fd) {	
	
	int sock_fd = *(int*)socket_fd;
	int val;
	int buffer_checker_int;
		
	char buff[1024];	//this will store the message sent by Client
	bzero(buff, 1024);	
	char response[1024];	//this will store the message that Server will send in response to the Client's message that it received
	bzero(response, 1024);
	
	printf("Client's connection is accepted by the Server.\n");

	while ((val = read(sock_fd, buffer, 1024)) > 0) {
		printf("%s\n", buffer);
		strcpy(response, "Got your message");
		write(sock_fd, response, strlen(response)); 
	} 
	
	if (val == 0) {
		printf("Server is disconnected from the client.\n");
		close(sock_fd);
	}
	if(val < 0) {
		fprintf(stderr, "Error! Couldn't read message from Client.\n");
	}

	pthread_exit(0);
}







int buffer_checker (char * buffer) {
	if(buffer[0] == 'c') {
		if(buffer[1] == 'r') {
			if(buffer[2] == 'e') {
				if(buffer[3] == 'a') {
					if(buffer[4] == 't') {
						if(buffer[5] == 'e' && buffer[6]) {
							return 0;
						}
					}
				}
			}
		}
		return -1;
	}
	else if (buffer[0] == 's') {
		if(buffer[1] == 'e') {
			if(buffer[2] == 'r') {
				if(buffer[3] == 'v') {
					if(buffer[4] == 'e') {
						return 1;
					}
				}
			}
		}
	}
	return -1;
}
