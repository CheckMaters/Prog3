#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include "bankingClient_1.h"


pthread_t input_thread;
pthread_t output_thread;

int input_handler(void* args){
	int counter = 0;
	int checker = 0;
	char* first = (char*)args;
	char* str = malloc(sizeof(char)*256);
	char *token = strtok(first, " ");
	int j;
	int lastspace = 0;
	
	while(token != NULL) {	
		printf("counter %d\n", counter);
		
		for(j = 0; j< strlen(token); j++){
			if(token[j] == ' ' || token[j] == '\n'){
				printf("hellopoop\n");
				lastspace = 1;
			}
			printf("here: %c\n", token[j]);
		}
		if(lastspace == 1){
			printf("got in\n");
			for(j=0; j < strlen(token)-1;j++){
				str[j]=token[j];
				printf("in the string %c\n", token[j]);
			}
			token = str;
			printf("new length %d\n", strlen(str));
		}
		
		printf("%s %d\n", token, strlen(token));
		if(counter == 0){
			printf("%d---\n", checker);
			if(strcmp(token, "create")==0){
				checker = 1;
					//printf("hello sir");
			} else if(strcmp(token, "serve")==0) {
				checker = 2;
				
			} else if (strcmp(token, "deposit") == 0){
				checker = 3;
			} else if (strcmp(token, "withdraw") == 0){
				checker = 4;
			} else if (strcmp(token, "query")==0){
				checker = 5;
				
			} else if (strcmp(token, "end")==0){
				checker = 6;
				
			} else if (strcmp(token, "quit")==0){
				checker = 7;
				
			} else {
				fprintf(stderr, "Incorrect spelling or invalid command. Try again.\n");
				return -1;
			}
			
		} else if (counter == 1){
			if (checker == 1||checker==2){
				if(strlen(token)>255){
					fprintf(stderr, "Too long of a username. Try again.\n");
					return -1;
				}
				
			} else if (checker > 2 && checker < 5){
				if(strlen(token)>255){
					fprintf(stderr, "Too long of a username. Try again.\n");
					return -1;
				}
				int dotCounter = 0;
				int i = 0;
				printf("%s\n", token);
				for(i;i<strlen(token)-1;i++){
					printf("%c %d %d\n", token[i], dotCounter, strlen(token));
					if(token[i] == '.'){
						dotCounter++;
						continue;
					}
					if(isdigit((int)token[i]) == 0){
						fprintf(stderr, "Invalid amount. Try again.\n");
						return -1;
					}
				}
				if(dotCounter > 1){
					fprintf(stderr, "Invalid amount. Try again.\n");
					return -1;
				}
				
			} else if(checker == 0){
				fprintf(stderr, "Invalid argument. Try again.\n");
				return -1;
			} else if (checker == 5||checker == 6|| checker == 7) {
				fprintf(stderr, "Invalid arguments. Try again.\n");
				return -1;
			}
			
			
		} else if(counter >= 2) {
			fprintf(stderr, "Incorrect format of the input to many spaces. Try again.\n");
			return -1;
		}
		printf("counter right before %d\n", counter);
		counter+=1;
		token = strtok(NULL, " ");
		printf("counter %d\n", counter);
	}
	printf("counter %d\n", counter);
	if(counter == 0 || counter > 2){
		fprintf(stderr,"Invalid number of arguments. Try again.\n");
		return -1;
	} else if(counter == 1){
		return checker;
		
	} else {
		return checker;
	}
	fprintf(stderr,"Check arguments. Try again.\n");
	return -1;
	
}


int main(int argc, char ** argv){
	
//Error if the arguments are more or less than expected
	if(argc != 3) {
		fprintf(stderr, "Error! Unexpected total number of Arguments.\n");
		return -1;
	}
	int num;
	int client_fd, val_trans;			//client_fd -> socket descriptor, val_trans -> returning int value from read and write
	struct sockaddr_in serv_address;		//struct used to store the server address/hostname and port number
	struct hostent* server;				//this will store the human readable hostname in the form of actual hostname by converting it using gethostbyname(char*)
	int PORT = atoi(argv[2]);			//getting the port number from STDIN
	
//checking the PORT number
	if(PORT != 9444) {
		fprintf(stderr, "Error! Incorrect Port Number.\n");
		return -1;
	}


//checking if the server name is correct or not; Server name must be same as the server name where bankingServer is executing
	server = gethostbyname(argv[1]);
	if(server == NULL) {
		fprintf(stderr, "Error! Unable to locate the defined Server!\n");
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

	printf("Trying to connect to server. Please stand by.\n");
	int con;
	while(1){
		con = connect(client_fd, (struct sockaddr *) &serv_address, sizeof(serv_address));
		if (con>=0) {	
			fprintf(stdout, "\n\n** Successfully connected to the Server **\n\n");
			break;
		}
	}

	int thread_result;
	thread_result = pthread_create(&input_thread, NULL,first_thread, NULL);
	
	if(thread_result < 0){
		fprintf(stderr, "Error! Unable to create a thread\n");
		return -1;
	}
	
	/*

	//printf("To create a new Bank Account, TYPE IN :- create <accountname>\n");
	//printf("To start a session with your existing account, TYPE IN :- server <accountname>\n");
	//printf("To close this connection, TYPE IN :- quit\n");
	print_Info();
	
	fgets(buffer, 1024, stdin);

	
		
	
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

	
	
	char buffer[265];				//buffer to send message back and forth from client and server
	bzero(buffer, sizeof(buffer));			//setting every byte of buffer to '0' in the beginning
	int tokenizer_result;
	
	while(1){
		print_Info();
		fgets(buffer, 265, stdin);
		
		//if unable to read the user's input this will allow them to retype the input by repeating the while loop
		if (buffer == NULL){
				fprintf(stderr, "Error! Unable to read you input, Please try again\n");
				bzero(buffer, sizeof(buffer));
				continue;
		}
		//
		tokenizer_result = input_Handler( (void *)buffer);
		
		//checking the user's input 
		if(tokenzier_result == )
		
	}
	*/
	
	/*
	while(1) {
		printf("Client: Enter Data for Server:\n");
		fgets(buffer2,1024,stdin);
		printf("Buffer-> %s -----\n", buffer2);
		if ((send(client_fd,buffer2, strlen(buffer2),0))== -1) {
			fprintf(stderr, "Failure Sending Message\n");
			close(client_fd);
			exit(1);
		} else {
			printf("Client: Message being sent: %s\n",buffer2);
			num = recv(client_fd, buffer2, sizeof(buffer2),0);
			if ( num <= 0 ){
				printf("Either Connection Closed or Error\n");
				//Break from the While
				break;
			}
			buff[num] = '\0';
			printf("Client: Message Received From Server -  %s\n",buffer2);
		}
	}
	*/
	pthread_join(input_thread, (void *) &client_fd);
	//pthread_join(&output_thread, NULL);
	close(client_fd);
	
	return 0;

}




void * first_thread (void * args){
	int client_fd = * (int *) args; 
	char buffer[280];
	bzero(buffer, sizeof(buffer));
	//int thread_counter = 0;				//keeps counting number of threads created for output, so that it can wait for output thread when we are writing more than one time
	int server_write_response;			//output of write()	
	
	
	int input_handler_result;
	while(1){
		print_info();
		fgets(buffer, 256, stdin);
		
		//just an extra check that buffer reads from the stdin
		if(buffer == NULL){
				fprintf(stderr, "Error! Unable to read user's input. Try again!");
				bzero(buffer, sizeof(buffer));
				continue;
		}
		//	
		input_handler_result = input_handler((void *) buffer);
		if(input_handler_result == 1){
			//create
			char buffer_to_send[257];
			buffer_to_send[0] = 'c';
			strncpy((buffer_to_send + 1), (buffer + 7), 255);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
			}
			bzero(buffer, sizeof(buffer));
			
		}
		else if(input_handler_result == 2){ 
				//server
			char buffer_to_send[257];
			buffer_to_send[0] = 's';
			strncpy((buffer_to_send + 1), (buffer + 6), 255);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
			}
			bzero(buffer, sizeof(buffer));
		}
		else if(input_handler_result == 7){
				//quit
				printf("\t*\tThank You For Your Business\t*\n");
				pthread_exit(0);
					
		}
		else if (input_handler_result == 3 || input_handler_result == 4 || input_handler_result == 5 || input_handler_result == 6){
				fprintf(stderr, "Error! Unable to perform these activities as User isn't in serve mode.\n");
				continue;
		}
		else {
				fprintf(stderr, "Error! Invalid activity Option. Try Again.\n");
				continue;
		}
	}   
}







void print_info(){
	printf("To create a new Bank Account, TYPE IN :- create <accountname>\n");
	printf("To start a session with your existing account, TYPE IN :- server <accountname>\n");
	printf("To close this connection, TYPE IN :- quit\n");
	return;	
}
