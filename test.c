#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define PORT 9444

int main () {
	int server_fd, valread;
	struct sockaddr_in address, client_address;
	int address_len = sizeof(address);
	int client_len = sizeof(client_address);
	bzero((char *) &address, address_len);
	int opt = 1;
	int client_socket;
	char buffer[1024];
	char re_buffer[1024];
	strcpy(re_buffer, "Got your Message!");



	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error! Socket Failed.\n");
		return -1;
	}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		printf("Error! Set socket Failed.\n");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if(bind(server_fd,(struct sockaddr*) &address, sizeof(address))){
		printf("Error! Unable to bind server_fd to server addresss.\n");
		return -1;
	}

	if(listen(server_fd, 5) < 0){
		printf("Error! Listening.");
		return -1;
	}


	if((client_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*)&client_len))<0){
		printf("Error! Client Socket.\n");
		return -1;
	}

	valread = read(client_socket, buffer, 1024);
	printf("%s\n", buffer);
	send (client_socket, re_buffer, strlen(re_buffer), 0);
	return 0;


}
