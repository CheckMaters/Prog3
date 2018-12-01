#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>


int main(int argc, char ** argv){
	
//Error if the arguments are more or less than expected
	if(argc != 3) {
		fprintf(stderr, "Error! Unexpected total number of Arguments.\n");
		return -1;
	}
	
	int client_fd, val_trans;			//client_fd -> socket descriptor, val_trans -> returning int value from read and write
	struct sockaddr_in serv_address;		//struct used to store the server address/hostname and port number
	char buffer[1024];				//buffer to send message back and forth from client and server
	bzero(buffer, sizeof(buffer));			//setting every byte of buffer to '0' in the beginning 
	strcpy(buffer, "This is message from client");	//First message from client to server
	struct hostent* server;				//this will store the human readable hostname as actual hostname by converting it using gethostbyname(char*)
	int PORT = atoi(argv[2]);			//getting the port number from STDIN
	
//checking the PORT number
	if(PORT != 9444) {
		fprintf(stderr, "Error! Incorrect Port Number.\n");
		return -1;
	}


//checking if the server name is correct or not; Server name must be same as the server name where bankingServer is executing
	server = gethostbyname(argv[1]);
	if(server == NULL) {
		fprintf(stderr, "Error! Server name is not correct!\n");
		return -1;
	}	


//Opening a socket for client using socket()
	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error! Client Socket Failed.\n");
		return -1;
	}


//setting every single byte of serv_address to '0' at first
	memset(&serv_address, '0' , sizeof(serv_address));


//filling up the struct sockaddr_in with port number, server address and sin_family
	serv_address.sin_family = AF_INET;
	serv_address.sin_port = htons(PORT);
	bcopy((char*)server->h_addr, (char*) &serv_address.sin_addr.s_addr, server->h_length);


	if(connect(client_fd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0) {
		fprintf(stderr, "Error! Unable to connect to Server.\n");
		return -1;
	} else {
		fprintf(stderr, "\n\n** Successfully connected to the Server **\n\n");
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
