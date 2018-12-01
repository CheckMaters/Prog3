#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define PORT 9444

int main () {
	int server_fd, valread, client_fd;
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

	if(listen(server_fd, 5) < 0){
		fprintf(stderr, "Error! Listening.");
		return -1;
	}


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
	return 0;


}
