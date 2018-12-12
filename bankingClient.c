#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>
#include "bankingClient.h"


int service_mode = 0;
pthread_t input_thread;
pthread_t output_thread;

pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;


typedef struct account{
	
	int sessionflag; // 0 is not in session and 1 is in session
	int fd;

} account;


int input_handler(void* args){
	int counter = 0;
	int checker = 0;
	
	char* first = (char*)args;
	char* str = malloc(sizeof(char)*256);
	char* aname = malloc(sizeof(char)*256);
	//printf("%s\n", first);
	char *token = strtok(first, " ");
	//printf("%s\n", first);
	int j;
	int lastspace = 0;
	
	
	for(j = 0; j< strlen(token); j++){
		if(token[j] == ' ' || token[j] == '\n'){
			//printf("hellopoop\n");
			lastspace = 1;
		}
		//printf("here: %c\n", token[j]);
	}
	
	if(lastspace == 1){
		//printf("got in\n");
		for(j=0; j < strlen(token)-1;j++){
			str[j]=token[j];
			//printf("in the string %c\n", token[j]);
		}
		token = str;
		//printf("new length %d\n", strlen(str));
	}
	
	//printf("%s-----------first\n", first);
	str = first + (strlen(token)+1);
	//printf("%d--token\n", strlen(str));
	if(strlen(str) == 0 || ((strlen(str) == 1) && str[0] == '\n')){
		
		if ((strcmp(token, "query")==0) && (strlen(str) < 1)){
			return 5;
			
		} else if ((strcmp(token, "end")==0) && (strlen(str) < 1)){
			return 6;
			
		} else if ((strcmp(token, "quit")==0) && (strlen(str) < 1)) {
			return 7;			
		} else {
			fprintf(stderr, "Check Account name or amount entered. Try again.\n");
			return -1;
		}
	}
	//printf("%s %d---str\n", str, strlen(str));
	if(strlen(str) >= 255){
		for(j=0; j < 255; j++){
			aname[j] = str[j];
		}
	} else if (strlen(str) < 255){
		for(j=0; j < (strlen(str)-1); j++){
			aname[j] = str[j];
		}
	}
	
	printf("%s %d\n", aname, strlen(aname));

	if(token != NULL) {
		
		//printf("%s %d\n", token, strlen(token));
		
		//printf("%d---\n", checker);
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
			if (strlen(str)>0){
				fprintf(stderr,"Error with arguments. Try again.\n");
				return -1;
			}
			
		} else if (strcmp(token, "end")==0){
			checker = 6;
			if (strlen(str)>0){
				fprintf(stderr,"Error with arguments. Try again.\n");
				return -1;
			}
			
		} else if (strcmp(token, "quit")==0){
			checker = 7;
			if (strlen(str)>0){
				fprintf(stderr,"Error with arguments. Try again.\n");
				return -1;
			}
			
		} else {
			fprintf(stderr, "Incorrect spelling or invalid command. Try again.\n");
			return -1;
		}
		
		if (checker > 2 && checker < 5){
			int dotCounter = 0;
			int i = 0;
			//printf("%s\n", aname);
			for(i;i<strlen(aname)-1;i++){
				//printf("%c %d %d\n", aname[i], dotCounter, strlen(aname));
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
		/*
		else if (checker == 5 && strlen(holder) > 5) {
			fprintf(stderr, "Invalid arguments. Try again.\n");
			return -1;
		} else if ((checker == 6 && strlen(holder) > 5)){
			fprintf(stderr, "Invalid arguments. Try again.\n");
			return -1;
		} else if ((checker == 7 && strlen(holder) > 5)) {
			fprintf(stderr, "Invalid arguments. Try again.\n");
			return -1;
		}
		*/
	}
	
	//printf("%s\n", first);
	return checker;
}





void throttle(){

    struct timespec timeToWait;
    struct timeval now;
    int rt;

    gettimeofday(&now,NULL);


    timeToWait.tv_sec = now.tv_sec+10;
    //timeToWait.tv_nsec = (now.tv_usec+1000UL*timeInMs)*1000UL;

    pthread_mutex_lock(&fakeMutex);
    rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    pthread_mutex_unlock(&fakeMutex);
    printf("\nDone\n");
}





void * first_thread (void * args){
	
	account * holder2 = args;
	int client_fd = holder2->fd; 
	char buffer_to_send[257];
	char buffer[280];
	bzero(buffer, sizeof(buffer));
	//int thread_counter = 0;			//keeps counting number of threads created for output, so that it can wait for output thread when we are writing more than one time
	int server_write_response;			//output of write()	
	int input_handler_result;
	int service_func_result;

	while(1){

		if(holder2->sessionflag == 0){ //not in session 
			print_info();
			fgets(buffer, 280, stdin);
		
			//just an extra check that buffer reads from the stdin
			if(buffer == NULL){
				fprintf(stderr, "Error! Unable to read user's input. Try again!");
				bzero(buffer, sizeof(buffer));
				continue;
			}


			input_handler_result = input_handler((void *) buffer);


			if(input_handler_result == 1){
				//create
				service_func_result = 0;
				bzero(buffer_to_send, sizeof(buffer_to_send));
				buffer_to_send[0] = 'c';
				strncpy((buffer_to_send + 1), (buffer + 7), 255);
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
				if(server_write_response < 0){
					fprintf(stderr, "Error! Unable to write to the Server. Try Again!\n");
				}
				bzero(buffer, sizeof(buffer));
				sleep(2);
				continue;
			} else if(input_handler_result == 2){ 
		
		
				//serve

		
				bzero(buffer_to_send, sizeof(buffer_to_send));
				buffer_to_send[0] = 's';
				strncpy((buffer_to_send + 1), (buffer + 6), 255);
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));

				//continuing because user entered end
				bzero(buffer, sizeof(buffer));
				sleep(2);
				continue;

			} else if(input_handler_result == 7){
				//quit
				//buffer_to_send[0] = 'q';
				strcpy((buffer_to_send), "quit");
				
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
				printf("\t*\tThank You For Your Business. Have a Nice Day.\t*\n");
				pthread_exit(0);

			} else if (input_handler_result == 3 || input_handler_result == 4 || input_handler_result == 5 || input_handler_result == 6){
				fprintf(stderr, "Error! Unable to perform these activities before serving an account.\n");
				continue;
			} else {
				fprintf(stderr, "Error! Invalid activity Option. Try Again.\n");
				continue;
			}

		} else if(holder2->sessionflag == 1) {
		
			print_service_info();
			fgets(buffer, 200, stdin);
			input_handler_result = input_handler ((void *)buffer);

			if(input_handler_result == 1 || input_handler_result == 2) {
				fprintf(stderr, "Error! Can't perform this activities while serving an account. Try Again.\n");
				continue;
			}

			if(input_handler_result == 3) {
				bzero(buffer_to_send, sizeof(buffer_to_send));
				buffer_to_send[0] = 'd';
				strncpy( (buffer_to_send + 1), (buffer + 8), 255);
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
				
				if(server_write_response < 0) {
					fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
				}

				bzero(buffer, sizeof(buffer));
				sleep(2);
				continue;
			}

			if(input_handler_result == 4) {
				bzero(buffer_to_send, sizeof(buffer_to_send));
				buffer_to_send[0] = 'w';
				strncpy( (buffer_to_send + 1), (buffer + 9), 255);
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
				
				if(server_write_response < 0) {
					fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
				}

				bzero(buffer, sizeof(buffer));
				sleep(2);
				continue;
			
			}

			if(input_handler_result == 5) {
				
				bzero(buffer_to_send, sizeof(buffer_to_send));
				strcpy(buffer_to_send, "query");
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			
				if(server_write_response < 0) {
					fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
				}

				bzero(buffer, sizeof(buffer));
				sleep(2);
				continue;
			}

			if(input_handler_result == 6) {
				//end
				bzero(buffer_to_send, sizeof(buffer_to_send));
				buffer_to_send[0] = 'e';
				server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
				
                        	printf("You have successfully ended your current service session.\n");
				sleep(2);
				continue;
				
			}

			if(input_handler_result == 7) {
				//exit the client		
				pthread_exit(0);
			}

		}

	}
   
}



void * second_thread (void * args){

	account * holder = args;
	pthread_mutex_t sesh;
	int  client_fd = holder->fd;
	int response;
	char buffer [500];
	
	while(1) {

		response = read(client_fd,buffer, sizeof(buffer)); 
		
		if(strcmp(buffer, "Connection Closed")==0) {
			pthread_exit(0);
		}
		if(strcmp(buffer, "In session")==0){
			pthread_mutex_lock(&sesh);	
			holder->sessionflag = 1;	
			pthread_mutex_unlock(&sesh);
			printf("Now serving account.\n");
			continue;
		}
		if(strcmp(buffer, "Session Ended")==0){	
			pthread_mutex_lock(&sesh);
			holder->sessionflag = 0;
			pthread_mutex_unlock(&sesh);
			printf("Ended current session.\n");
			continue;
		}
		
		if(buffer[0] == 'E' && buffer[1] == 'R') {
				
			//first two if statements will let service_func know that user can't access the account at the moment so return
			if(strcmp((buffer + 2), "Error! This account doesn't exist!")==0) {
				pthread_mutex_lock(&sesh);	
				holder->sessionflag = 0;
				pthread_mutex_unlock(&sesh);	
			}
			else if(strcmp((buffer + 2), "Error! This account is already in service!") == 0){
				pthread_mutex_lock(&sesh);	
				holder->sessionflag = 0;
				pthread_mutex_lock(&sesh);	
			}

			printf("%s\n", (buffer + 2));
			bzero(buffer, sizeof(buffer));
			continue;
		}
		
		printf("%s\n", buffer);
		bzero(buffer, sizeof(buffer));
	
	  }
	
}

/*
int service_func (int * args) {
	int client_fd = *args;
	
	if(service_mode < 0) {
		return 0;
	}
	
	char buffer[200];
	char buffer_to_send[200];
	int input_handler_result;
	int server_write_response;

	printf("Hello! You are in service mode now.\n");
		
	while (1) {
		print_service_info();
		fgets(buffer, 200, stdin);
		input_handler_result = input_handler ((void *)buffer);
		
		if(input_handler_result == 1 || input_handler_result == 2) {
			fprintf(stderr, "Error! Can't perform this activities while in Service Mode. Try Again.\n");
			continue;
		}
		
		if(input_handler_result == 3) {
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'd';
			strncpy( (buffer_to_send + 1), (buffer + 8), 195);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0) {
				fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
			}
			bzero(buffer, sizeof(buffer));
			throttle();
			continue;
		}
		
		if(input_handler_result == 4) {
			bzero(buffer_to_send, sizeof(buffer_to_send));
			buffer_to_send[0] = 'w';
			strncpy( (buffer_to_send + 1), (buffer + 9), 190);
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0) {
				fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
			}
			bzero(buffer, sizeof(buffer));
			throttle();
			continue;
		}
		
		if(input_handler_result == 5) {	
			bzero(buffer_to_send, sizeof(buffer_to_send));
			strcpy(buffer_to_send, "query");
			server_write_response = write(client_fd, buffer_to_send, strlen(buffer_to_send));
			if(server_write_response < 0) {
				fprintf(stderr, "Error! Unable to write it to the server. Try Again.\n");
			}
			bzero(buffer, sizeof(buffer));
			throttle();
			continue;
		}
		
		if(input_handler_result == 6) {	
			//end
			printf("You have successfully ended your current service session.\n");
			throttle();
			return 1;
		}
		
		if(input_handler_result == 7) {
			//quit
			return 2;
		}
		
	}
	
}
*/



void print_info(){
	printf("To create a new Bank Account, TYPE IN :- create <accountname>\n");
	printf("To start a session with your existing account, TYPE IN :- server <accountname>\n");
	printf("To close this connection, TYPE IN :- quit\n\n");
	return;	
}




void print_service_info(){
	printf("To deposit money, TYPE IN :- deposit <amount>\n");
	printf("To withdraw money, TYPE IN :- withdraw <amount>\n");
	printf("To check balance, TYPE IN :- query\n");
	printf("To end the session, TYPE IN :- end\n");
	printf("To close this connection, TYPE IN :- quit\n\n");
	return;
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

	printf("Trying to connect to server. Please stand by...\n");
	int con;
	while(1){
		con = connect(client_fd, (struct sockaddr *) &serv_address, sizeof(serv_address));
		if (con>=0) {	
			fprintf(stdout, "\n\n** Successfully connected to the Server **\n\n");
			break;
		}
	}
	account * accstruct = malloc(sizeof(account)); //not in session
	accstruct->fd = client_fd;
	accstruct->sessionflag = 0;
	int thread_result;
	thread_result = pthread_create(&input_thread, NULL,first_thread, (void *)accstruct);
	
	if(thread_result < 0){
		fprintf(stderr, "Error! Unable to create a thread.\n");
		return -1;
	}
	
	thread_result = pthread_create (&output_thread, NULL, second_thread, (void *)accstruct);

	if(thread_result < 0) {
		fprintf(stderr, "Error! Unable to create a thread.\n");
		return -1;
	}

		
	pthread_join(input_thread, NULL);
	pthread_join(output_thread, NULL);
	close(client_fd);
	
	return 0;

}





