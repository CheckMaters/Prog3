#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include "bankingClient.h"


//int service_mode = -1;      //this is set to 0 if the server response that certain account is already in session.
pthread_t input_thread;
pthread_t output_thread;
//int are_we_in_session = -1; //-1 means we are not in session, 0 means we are in session



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

	printf("Trying to connect to server. Please stand by...\n");
	int con;   //return value of connect()
    //this while loop will keep going around until client is able to connect to the server
	while(1){
		con = connect(client_fd, (struct sockaddr *) &serv_address, sizeof(serv_address));
		if (con>=0) {	
			fprintf(stdout, "\n\n** Successfully connected to the Server **\n\n");
			break;
		}
	}

	int thread_result; //will store the return of thread_create
	int * client_fd_pointer;   //pointer to the client_fd as thread only takes (void *) as parameter
	client_fd_pointer = &client_fd;    //setting client_fd as int *
    
    //creating thread to take all the inputs from user and send it to server
	thread_result = pthread_create(&input_thread, NULL,first_thread, (void *)client_fd_pointer);
	if(thread_result < 0){
		fprintf(stderr, "Error! Unable to create a thread.\n");
		return -1;
	}
	
    //creating thread to get all the response from server and output it to the user
	thread_result = pthread_create (&output_thread, NULL, second_thread, (void*)client_fd_pointer);
	if(thread_result < 0) {
		fprintf(stderr, "Error! Unable to create a thread.\n");
		return -1;
	}

    //waiting for all the threads to return
	pthread_join(input_thread, NULL);
	pthread_join(output_thread, NULL);
	close(client_fd);
	
	return 0;

}




void * first_thread (void * args){
	int client_fd = * (int *) args;    //pointer to the client_fd
	char * buffer_to_send = (char *) malloc (sizeof(char) * 265);          //this will be the modified buffer that we are going to send to the server
	char * buffer = (char *) malloc (sizeof(char) * 280);                  //this is the buffer we are going to read from the STDIN
	bzero(buffer, sizeof(buffer)); 
	int server_write_response;			//output of write()	
	int input_handler_result;          //this will store the return value of input_hadler
	int service_func_result;           //will store the value of service_func return; only when user wants to start session

    //this while loop will keep going around until user quits
	while(1){
         //printing helping statements for user, for the activities he/she can perform
		fgets(buffer, 280, stdin);    //reading a line from stdin
		
		//just an extra check that buffer reads from the stdin
		if(buffer == NULL){
				fprintf(stderr, "Error! Unable to read user's input. Try again!");
				bzero(buffer, sizeof(buffer));
				continue;
		}
		//	

		input_handler_result = input_handler((void *) buffer);    //this will check the input line by user, to make sure it is correctly formatted 
		if(input_handler_result == 1){
			//create
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'c';
			strncpy((buffer_to_send + 1), (buffer + 7), 255);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            //if less than 0 means it wasn't able to write it to the server
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server. Try Again!\n");
			}
			bzero(buffer, sizeof(buffer));
			continue;
		}
		else if(input_handler_result == 2){ 
			//serve
            service_func_result = 0;
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 's';
			strncpy((buffer_to_send + 1), (buffer + 6), 255);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            //if less than 0 means it wasn't able to write it to the server
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
					continue;
			}
            
			bzero(buffer, sizeof(buffer));
			continue;
		}
        else if(input_handler_result == 3){ 
			//serve
            service_func_result = 0;
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'd';
			strncpy((buffer_to_send + 1), (buffer + 8), 255);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            //if less than 0 means it wasn't able to write it to the server
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
					continue;
			}
            
			bzero(buffer, sizeof(buffer));
			continue;
		}
        else if(input_handler_result == 4){ 
			//serve
            service_func_result = 0;
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'w';
			strncpy((buffer_to_send + 1), (buffer + 9), 255);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            //if less than 0 means it wasn't able to write it to the server
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
					continue;
			}
            
			bzero(buffer, sizeof(buffer));
			continue;
		}
        else if(input_handler_result == 5){ 
			//serve
            service_func_result = 0;
			bzero(buffer_to_send, sizeof(buffer_to_send));
			strcpy(buffer_to_send, "query");
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            //if less than 0 means it wasn't able to write it to the server
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
					continue;
			}
            
			bzero(buffer, sizeof(buffer));
			continue;
		}
        else if(input_handler_result == 3){ 
			//serve
            service_func_result = 0;
			bzero(buffer_to_send, sizeof(buffer_to_send));
			strcpy(buffer_to_send, "end");
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            //if less than 0 means it wasn't able to write it to the server
			if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server\n.");
					continue;
			}
            
			bzero(buffer, sizeof(buffer));
			continue;
		}
		else if(input_handler_result == 7){
				//quit
                bzero(buffer_to_send, sizeof(buffer_to_send));
                strcpy(buffer_to_send, "quit");
                    while(1){
                        server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
                        if(server_write_response < 0){
                            continue;
                        }
                        else {
                            break;
                        }
                    }
				printf("\t*\tThank You For Your Business. Have a Nice Day.\t*\n");
                free(buffer_to_send);
                free(buffer);
				pthread_exit(0);
					
		}
		else {
				//fprintf(stderr, "Error! Invalid activity Option. Try Again.\n");
                bzero(buffer, sizeof(buffer));
				continue;
		}
	}   
}






void * second_thread (void * socket_fd){
	int  client_fd = *(int *) socket_fd;            //client_fd so that this function could read the response from server
	int read_response;                          //storing value of read(), if less than 0 means wasn't able to read from the server
	char * buffer = (char *) malloc (sizeof(char) * 100);
	//pthread_mutex_t sample_lock;
    
	while(1) {
    bzero(buffer, sizeof(buffer));
	read_response = read(client_fd,buffer, sizeof(buffer)); 
        if(read_response < 0) {
              continue;  
        } 
        else {    
		  if(strcmp(buffer, "Connection Closed") == 0) {
            //service_mode = 0;
            free(buffer);
            pthread_exit(0);
		  }
		  if(buffer[0] == 'E' && buffer[1] == 'R') {
              /*
              //service_mode will be checked when some user wants to start a session with certain account
			//first two if statements will let service_func know that user can't access the account at the moment so return
			if(strcmp((buffer + 2), "Error! This account doesn't exist!")==0) {
                //pthread_mutex_lock(&sample_lock);
				//service_mode = -1;
                pthread_mutex_unlock(&sample_lock);
                
			}
			else if(strcmp((buffer + 2), "Error! This account is already in service!") == 0){
				pthread_mutex_lock(&sample_lock);
				service_mode = -1;
                pthread_mutex_unlock(&sample_lock);
                
			}
            */
			fprintf(stderr, "%s\n", (buffer + 2));
			continue;
		  }
	       printf("%s\n", buffer);
        }
	}
}







/*

int service_func (int * socket_fd) {
	int client_fd = *socket_fd;
	if(service_mode == 0) {
        service_mode = -1;
		return 0;
	}
	char buffer[200];              //the buffer we read from STDIN, entered by user
	char buffer_to_send[200];      //the buffer we send to the server after modifying
	int input_handler_result;         //return result of input_handler after checking the input of user
	int server_write_response;     //return of write(), if less than 0 then it wasn't able to write to the server

	printf("Hello! You are in service mode now.\n");
		
	while (1) {        //printing the activites user can perform in session
		fgets(buffer, 200, stdin);    //getting the inputs of user
		input_handler_result = input_handler ((void *)buffer);    //checking the inputs of user, if they are correctly formatted
		
        //if user wants to create or serve, he/she can't as they have already started session
		if(input_handler_result == 1 || input_handler_result == 2) {
			fprintf(stderr, "Error! Can't perform this activities while in Service Mode. Try Again.\n");
			continue;
		}
		if(input_handler_result == 3) {
            //deposit
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'd';
			strncpy( (buffer_to_send + 1), (buffer + 8), 195);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0) {
				fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
			}
			bzero(buffer, sizeof(buffer));
			continue;
		}
		if(input_handler_result == 4) {
            //withdraw
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'w';
			strncpy( (buffer_to_send + 1), (buffer + 9), 190);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0) {
				fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
			}
			bzero(buffer, sizeof(buffer));
			continue;
		}
		if(input_handler_result == 5) {	
            //query
			bzero(buffer_to_send, sizeof(buffer_to_send));
			strcpy(buffer_to_send, "query");
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0) {
				fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
			}
			bzero(buffer, sizeof(buffer));
			continue;
		}
		if(input_handler_result == 6) {	
			//end
            bzero(buffer_to_send, sizeof(buffer_to_send));
            strcpy(buffer_to_send, "end");
            server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            if(server_write_response < 0) {
                    fprintf(stderr, "Error! Unable to write to the server. Try Again.\n");
            }
			printf("You have successfully ended your current service session.\n");
            service_mode = 0;   //because this user is ending the session with his/her account
			return 1;
		}
		if(input_handler_result == 7) {
			//quit
            bzero(buffer_to_send, sizeof(buffer_to_send));
            strcpy(buffer_to_send, "quit");
            server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
            if(server_write_response < 0) {
                    fprintf(stderr, "Error! Unable to write to the server. Try Again.\n");
            }
            service_mode = 0;   //because this user is ending the session with his/her account
			return 2;
		}
	}
	
}

*/







int input_handler(void* args){
	int counter = 0;
	int checker = 0;
	
	char* first = (char*)args;
	char* str = malloc(sizeof(char)*256);
	char* aname = malloc(sizeof(char)*256);
	char *token = strtok(first, " ");
    
	int j;
	int lastspace = 0;
	for(j = 0; j< strlen(token); j++){
		if(token[j] == ' ' || token[j] == '\n'){
			
			lastspace = 1;
		}
	}
	
	if(lastspace == 1){
		for(j=0; j < strlen(token)-1;j++){
			str[j]=token[j];
		}
		token = str;
	}
	
	
	str = first + (strlen(token)+1);
	
    //if it's "command" + space + \n, it's an error
	if(strlen(str) == 0 || ((strlen(str) == 1) && str[0] == '\n')){
		
		if ((strcmp(token, "query")==0) && (strlen(str) < 1)){
			return 5;
			
		} else if ((strcmp(token, "end")==0) && (strlen(str) < 1)){
			return 6;
			
		} else if ((strcmp(token, "quit")==0) && (strlen(str) < 1)) {
            free(str);
            free(aname);
            free(token);
			return 7;			
		} else {
			fprintf(stderr, "Error! Invalid Arguments. Try again.\n");
			return -1;
		}
	}
	
	if(strlen(str) >= 255){
		for(j=0; j < 255; j++){
			aname[j] = str[j];
		}
	} else if (strlen(str) < 255){
		for(j=0; j < (strlen(str)-1); j++){
			aname[j] = str[j];
		}
	}
	
	
	if(token != NULL) {
		if(strcmp(token, "create")==0){
			checker = 1;

		} else if(strcmp(token, "serve")==0) {
			checker = 2;
			
		} else if (strcmp(token, "deposit") == 0){
			checker = 3;
		} else if (strcmp(token, "withdraw") == 0){
			checker = 4;
		} else if (strcmp(token, "query")==0){
			checker = 5;
			if (strlen(str)>0){
				fprintf(stderr,"Error! Invalid Arguments. Try again.\n");
				return -1;
			}
			
		} else if (strcmp(token, "end")==0){
			checker = 6;
			if (strlen(str)>0){
				fprintf(stderr,"Error! Invalid Arguments. Try again.\n");
				return -1;
			}
			
		} else if (strcmp(token, "quit")==0){
			checker = 7;
			if (strlen(str)>0){
				fprintf(stderr,"Error! Invalid Arguments. Try again.\n");
				return -1;
			}
			
		} else {
			fprintf(stderr, "Error! Invalid Arguments. Try again.\n");
			return -1;
		}
		
		if (checker > 2 && checker < 5){
			int dotCounter = 0;
			int i = 0;
		
			for(i;i<strlen(aname)-1;i++){
				
				if(aname[i] == '.'){
					dotCounter++;
					continue;
				}
				if(isdigit((int)aname[i]) == 0){
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
		} 
		
	}
	return checker;
}

