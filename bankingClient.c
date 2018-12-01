#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>


int main(int argc, char ** argv){
	
	if(argc != 3) {
		fprintf(stderr, "Error! Incorrect Arguments.\n");
		return -1;
	}
	
	int client_fd, val_trans;
	struct sockaddr_in serv_address;
	char buffer[1024];
	bzero(buffer, sizeof(buffer));
	strcpy(buffer, "This is message from client");
	struct hostent* server;
	int PORT = atoi(argv[2]);

//creating a socket for client using socket()
	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error! Client Socket Failed.\n");
		return -1;
	}

	server = gethostbyname(argv[1]);
//checking if the server name is correct or not
	if(server == NULL) {
		fprintf(stderr, "Error! Server name is not correct!\n");
		return -1;
	}	


//setting every single byte of serv_address to '0' at first
	memset(&serv_address, '0' , sizeof(serv_address));

	serv_address.sin_family = AF_INET;
	serv_address.sin_port = htons(PORT);
	bcopy((char*)server->h_addr, (char*) &serv_address.sin_addr.s_addr, server->h_length);


	if(connect(client_fd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0) {
		fprintf(stderr, "Error! Unable to connect to Server.\n");
		return -1;
	}
	
	val_trans = write(client_fd, buffer, strlen(buffer));	
	if(val_trans < 0){
		fprintf(stderr, "Error! Unable to write a buffer to the network socket.\n");
		return -1;
	}
	bzero(buffer, sizeof(buffer));
	val_trans = read(client_fd, buffer, 1024);
	if(val_trans < 0) {
		fprintf(stderr, "Error! Unable to read a buffer from the network socket.\n");
		return -1;
	}
	
	printf("%s\n", buffer);

	return 0;

}
