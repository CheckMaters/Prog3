#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define PORT 9444
#include "bankingServer.h"
#include "bankActivities.h"



bank_accounts * account_list;


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
		
		if(pthread_create(&thread, NULL, &connection_handler, (void *) valread) < 0) {
			fprintf(stderr, "Error! Unable to create thread for this client.\n");
			return -1;
		}	
	}
		
	if(client_fd < 0) {
		fprintf(stderr, "Error! Uable to accept Client Socket.\n");
		return -1;
	}
	pthread_join(thread, NULL);
	return 0;

}






void * connection_handler (void * socket_fd) {		
	int sock_fd = *(int*)socket_fd;
	int val;
		
	char buff[1024];	//this will store the message sent by Client
	bzero(buff, 1024);	
	char response[1024];	//this will store the message that Server will send in response to the Client's message that it received
	bzero(response, 1024);
	int func_return;
	
	printf("\t*\tClient's connection is accepted by the Server\t*.\n");

	while ((val = read(sock_fd, buff, 1024)) >= 0) {
		func_return = string_handler(*sock_fd, buff);
		if(func_return == -1) {	
			break;
		}
	} 
	
	if (val >= 0) {
		printf("Server is disconnected from the client.\n");
		close(sock_fd);
	}
	if(val < 0) {
		fprintf(stderr, "Error! Couldn't read message from Client.\n");
	}
	pthread_exit(0);
}







int string_handler (int * sock_fd, char * buffer) {
	int client_fd = * sock_fd; 
	int func_return_value;
	int write_return;
	char buffer_to_send[200];

	if(buffer[0] == 'c') {
		func_return_value = add_account ((buffer + 1), account_list);
		if(func_return_value == -1) {	
			while(1) {
				bzero(buffer, sizeof(buffer));
				strcpy(buffer, "ERError! Unable to create account. User with same username already exists.");
				write_return = write(client_fd, buffer, strlen(buffer));
			//if write is unable to write the error to client for some reason, it will keep writing until it successfully writes it on the client end	
				if(write_return < 0) {
					continue;
					//wasn't able to write it, so it will continue to the loop
				}else {	
					break;
				}
			}
		}
		return 0;	
	}
	if(buffer[0] == 's') {
	}
}




int service_func (int * sock_fd, char * account_name_char) {
	int client_fd = * sock_fd;
	int func_return;
	int write_return;
	char buffer_to_send[100];
	bank_accounts * this_account = (account_name_char);
	
	if(this_accounts == NULL) {
		bzero(buffer, sizeof(buffer));
		strcpy(buffer, "ERError! This account doesn't exist!");	
		while(1){
			write_return = write(client_fd, buffer, strlen(buffer));
			if(write_return < 0) {	
				continue;
			}else {
				break;
			}
		}	
		return 1;
	}

	if((in_session(this_accout)) == 0) {
		bzero(buffer, sizeof(buffer));
		strcpy(buffer, "ERErro! This account is already in service!");
		while(1) {
			write_return = write(client_fd, buffer, strlen(buffer));
			if(write_return < 0) {
				continue;
			}else {	
				break;
			} 
		}
	}

	//At this point we have the the accout that can be served well.
	
}





bank_accounts * search_for_account (char * account_name_char) {
	bank_accounts * temp = main_list;
	
	while (temp != NULL) {
		if(strcmp(temp->username, account_name_char) == 0) {	
			return temp;
		}
		temp = temp->next;	
	}
	return temp;
}



int in_session (bank_accounts * account) {
	if(account->session_flag == 0) {
		return 0;		//the account is in session
	}
	return -1;	//the account isn't in session
}
