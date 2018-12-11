#ifndef BANKINGCLIENT_H
#define BANKINGCLIENT_H


/*
	this function will tokenize the command
	entered by user and thus create a 
	response accordingly.
	Return Value		Action
	1	:				create account
	2 	:				serve account
	3	:				deposit money
	4	:				withdraw money
	5	:				Query
	6	:				End Session
	7	:				Quit Connection
*/
int input_handler(void * args);







/*
	this function will output whatever it reads 
	from the server's end. if the first two 
	characters of read buffer are 'ER' then it
	will print out the buffer as an ERROR, else 
	it would just print out the straight up 
	message on STDIN.
*/
void * second_thread (void * args);





/*
	This function will read (from stdin) 
	whatever activities user wants to 
	perform and write it to server to
	actually carry out that activity
*/
void * first_thread (void * args);







/*	
	This function will help user when they are 
	in service mode. This function has a while 
	loop that goes around until user quits or ends
	the session. Unless this function returns, 
	first_thread function will wait for this fuctions
	return value.
	Return Value 				Reason
		1				User ended the service session
		2 				User wants to quit the connection		

int service_func(int * socket_fd);

*/




#endif
