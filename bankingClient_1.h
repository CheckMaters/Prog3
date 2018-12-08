#ifndef BANKINGCLIENT_1_H
#define BANKINGCLIENT_1_H


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
	This function will read (from stdin) 
	whatever activities user wants to 
	perform and write it to server to
	actually carry out that activity
*/
void * first_thread (void * args);



/*
	This function will basically print
	options for entry level of activity
	that user could perform after
	successfully connecting with server
*/
void print_info();




/*
	This function will be called by a  thread
	that will read the response back from
	the server's end and thus provide
	satisfactory response to the user.
*/
void * second_thread (void * args);

#endif