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




bank_accounts * account_list;
//pthread_mutex_t temp_lock = PTHREAD_MUTEX_INITIALIZER;

int main () {
    
	int server_fd, *valread, client_fd;        //valread will store the client_fd as pointer to be send to the thread function
	struct sockaddr_in address, client_address;    //struct to store the information about addresss
	int address_len = sizeof(address);         
	int client_len = sizeof(client_address);
	bzero((char *) &address, address_len);
	bzero((char *) &client_address, client_len);
	//int opt = 1;
	
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

    //listening
	if(listen(server_fd, 100) < 0){
		fprintf(stderr, "Error! Listening.");
		return -1;
	}


	//accepting all the coming client signals
	//this while loop will never stop until CTRL + C
	while(1) {
		client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
		
		if(client_fd < 0) {
		fprintf(stderr, "Error! Unable to accept Client Socket.\n");
		continue;
		}		
		
		pthread_t thread;
		valread = malloc(sizeof(int));
		*valread = client_fd;
		
		if(pthread_create(&thread, NULL, &connection_handler, (void *) valread) < 0) {
			fprintf(stderr, "Error! Unable to create thread for this client.\n");
			return -1;
		}	
	}
		
	
	//pthread_join(thread, NULL);
	return 0;

}






void * connection_handler (void * socket_fd) {
    //typedef struct bank_accounts bank_accounts;
    
	int sock_fd = *(int*)socket_fd;
	int val_read;
		
	char * buff = (char *) malloc (sizeof(char) * 300);	//this will store the message sent by Client
	bzero(buff, sizeof(buff));	
	int string_handler_return;		//stores the return value of string_handler			
	
	printf("\t*\tClient's connection is accepted by the Server\t*.\n");

	while ((val_read = read(sock_fd, buff, 300)) >= 0) {
		string_handler_return = string_handler(&sock_fd, buff);
		if(string_handler_return == -1) {	    //this means that user entered quit and wants to close the connection
			break;
		}
        bzero(buff, sizeof(buff));
	} 
	
	if (string_handler_return == -1) {
		printf("Server is disconnected from the client.\n");
		close(sock_fd);
	}else if(val_read < 0) {
		fprintf(stderr, "Error! Couldn't read message from Client.\n");
	}
    free(buff);
	pthread_exit(0);
}







int string_handler (int * sock_fd, char * buffer) {
	int client_fd = * sock_fd; 
	int func_return_value;			//store the values of function returns
	int write_return;
	char * buffer_to_send = (char *) malloc (sizeof(char) * 200);

	if(buffer[0] == 'c') {
        //create
        //printf("%s\n\n", buffer);
		func_return_value = add_account((buffer + 1));	//account list is the main linked list storing all the accounts		
		if(func_return_value == -1) {	
				bzero(buffer_to_send, sizeof(buffer_to_send));
				strcpy(buffer_to_send, "ERError! Unable to create account. User with same username already exists.");
				write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
		}
        else {
				bzero(buffer_to_send, sizeof(buffer_to_send));
				strcpy(buffer_to_send, "Account Added Successfully");
				write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
        }
        free(buffer_to_send);
		return 0;
	}
	if(buffer[0] == 's') {
		//serve
        printf("%s\n", buffer);
        bzero(buffer_to_send, sizeof(buffer_to_send));
        strcpy(buffer_to_send, "In session");
        write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
        func_return_value = service_func(client_fd, (buffer+1));	//service_func will handle everything when the account is in service mode
	    free(buffer_to_send);
        return func_return_value;  //if it's quit service_func returns -1 else it returns 0
    }
	
	if(strcmp(buffer, "quit") == 0){
		//quit
        //printf("%s\n\n", buffer);
            bzero(buffer_to_send, sizeof(buffer_to_send));
            strcpy(buffer_to_send, "Connection Closed");
            write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            free(buffer_to_send);
            return -1;       //connection_handler will close the connection
	}
    else {
        bzero(buffer_to_send, sizeof(buffer_to_send));
        strcpy(buffer_to_send, "ERError! Can't perform these actions if not in session mode.");
        write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
        free(buffer_to_send);
        return 0;
    }
    return 0;
}




int service_func (int sock_fd, char * account_name_char) {
	int client_fd = sock_fd;
	int func_return;
    int read_return;
	int write_return;
    pthread_mutex_t temp_lock;
    char * buffer_read = (char *) malloc (sizeof(char) * 270);;
	char * buffer_to_send = (char *) malloc (sizeof(char) * 100);
	bank_accounts * this_account;
    printf("I am in servie Func - %s\n", account_name_char);
    this_account = search_for_account(account_name_char);
	
	if(this_account == NULL) {
		bzero(buffer_to_send, sizeof(buffer_to_send));
		strcpy(buffer_to_send, "ERError! This account doesn't exist!");	
        write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
        free(buffer_read);
        free(buffer_to_send);
		return 0;
	}

	if((in_session(this_account)) == 0) {
		bzero(buffer_to_send, sizeof(buffer_to_send));
		strcpy(buffer_to_send, "ERError! This account is already in service!");
        write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
        free(buffer_to_send);
        free(buffer_read);
        return 0;
	}
    
    printf("In session with this account -> %s\n", this_account->username);
    //At this point we have the the account that can be served well.
    
    //pthread_mutex_lock(&temp_lock);
    this_account->session_flag = 0;
    //pthread_mutex_unlock(&temp_lock);
    int return_value_d_w;
    float amount_d_w;
    while ((read_return = read(client_fd, buffer_read, 270)) >= 0) {
        printf("Inside the while loop in service_func");
		if(buffer_read[0] == 'd'){
            //deposit
            printf("%s\n", buffer_read);
            amount_d_w = atof((buffer_read + 1));
            return_value_d_w = deposit(amount_d_w, this_account);
            
            if(return_value_d_w == -1){
                bzero(buffer_to_send, sizeof(buffer_to_send));
                strcpy(buffer_to_send, "ERError! Unable to deposit the specified amount. Try Again.");
                write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));       
            }
            else {
                bzero(buffer_to_send, sizeof(buffer_to_send));
                strcpy(buffer_to_send, "Money Deposited.");
                write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            }
            //if return_value_d_w is not -1 (i.e., 0) then the amount was successfully deposited
            bzero(buffer_read, sizeof(buffer_read));
            continue;
            
        }
        if(buffer_read[0] == 'w'){
            //withdraw
            printf("%s\n", buffer_read);
            amount_d_w = atof((buffer_read + 1));
            return_value_d_w = withdraw(amount_d_w, this_account);
                
            if(return_value_d_w == -1){
                bzero(buffer_to_send, sizeof(buffer_to_send));
                strcpy(buffer_to_send, "ERError! Not enought balance in account. Try Again.");
                write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            }
            else {
                bzero(buffer_to_send, sizeof(buffer_to_send));
                strcpy(buffer_to_send, "Successfully Withdrawn");
                write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            }
            //if return_value_d_w is not -1 (i.e., 0) then the amount was successfully deposited
            bzero(buffer_read, sizeof(buffer_read));
            continue;
        }
        if(strcmp(buffer_read, "query") == 0){
            printf("%s\n", buffer_read);
            bzero(buffer_to_send, sizeof(buffer_to_send));
            sprintf(buffer_to_send, "Current Balance is :- %f", this_account->balance);
            write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            bzero(buffer_read, sizeof(buffer_read));
            continue;
            
        }
        if(strcmp(buffer_read, "quit") == 0){
            printf("%s\n", buffer_read);
            //pthread_mutex_lock(&temp_lock);
            this_account->session_flag = -1;        //setting session to -1 as we are leaving the session
            //pthread_mutex_unlock(&temp_lock);
     
            bzero(buffer_to_send, sizeof(buffer_to_send));
            strcpy(buffer_to_send, "Connection Closed");
            write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            free(buffer_read);
            free(buffer_to_send);
            return -1;
            
        }
        if(buffer_read[0] == 'e') {
            printf("%s\n", buffer_read);
            //pthread_mutex_lock(&temp_lock);
            this_account->session_flag = -1;    //setting session to -1 as we are leaving the session
            //pthread_mutex_unlock(&temp_lock);
            bzero(buffer_to_send, sizeof(buffer_to_send));
            strcpy(buffer_to_send, "Session Ended");
            write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            free(buffer_read);
            free(buffer_to_send);
            return 0;
        }
        else {
            bzero(buffer_to_send, sizeof(buffer_to_send));
            strcpy(buffer_to_send, "ERError! Incorrect Actions, while in Session Mode.");
            write_return = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            bzero(buffer_read, sizeof(buffer_read));
        }
		
	}//while loop ends 
    return 0;
}



bank_accounts* search_for_account(char * account_name_char) {
	bank_accounts * temp = account_list;
	
	while (temp != NULL) {
        printf("Searching...\n\n");
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







int withdraw (float amount, bank_accounts * account) {
	if (account->balance < amount) {
		printf("Error! Not enought money in the account.\n\n");
		return -1;
	}
	account->balance = account->balance - amount;
        printf("Successfully Withdrawed.\n\n");
	return 0;
}



int deposit (float amount, bank_accounts * account) {
	if(amount <= 0) {
		printf("Error! Deposit amount needs to be greater than 0.\n");
		return -1;
	}
	account->balance = account->balance + amount;
        printf("Successfully Deposited.\n\n");
	return 0;
}


int add_account (char * new_account) {
	bank_accounts * ptr = account_list;
	printf("We are in add_account - %s\n\n", new_account);
	if (ptr == NULL) {
		account_list = (bank_accounts *) malloc (sizeof(bank_accounts) + (sizeof(char) * 256));
		strcpy(account_list->username, new_account);
		account_list->balance = 0.0;
		account_list->session_flag = -1;
		account_list->next = NULL;
        printf("Account added successfully.\n\n");
		return 0;		
	}
	
	bank_accounts * temp = account_list->next;
	//checking if the new_accout is same as the first account's username
	if(strcmp(new_account, ptr->username) == 0) {
        printf("while loop - %s\n", ptr->username);
		printf("Error! Account with this username already exists. -> %s\n", ptr->username);
		return -1;
	}
	

	//checking if the there is any account with similar username as new_accout, if then error message and return -1
	while (temp != NULL) {
        printf("%s\n", temp->username);
		if(strcmp(new_account, temp->username) == 0)	 {
			printf("Error! Account with this username already exists. -> %s\n", temp->username);
			return -1;
		}
		ptr = temp;
		temp = temp->next;
	}

	
	//getting to this point means there is no account with same username as new_accout, and thus creating a new
	//bank account and adding it to the end of the linked list 
	if(temp == NULL) {
		bank_accounts * another_account = (bank_accounts *) malloc (sizeof(bank_accounts) + (sizeof(char) * 256));
		strcpy(another_account->username, new_account);
		another_account->balance = 0.0;
		another_account->session_flag = -1;
		another_account->next = NULL;
		ptr->next = another_account;
        printf("Account added successfully to the list.\n\n");
	}	
    
	return 0;
}

