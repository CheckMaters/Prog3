#ifndef BANKINGSERVER_H
#define BANKINGSERVER_H


typedef struct _bank_accounts {
	
	char username[256];
	float balance;	        //Ever accout starts off with 0
	int session_flag;       //if it's 0 then account is in session, if -1 then account isn't in session. No other values
	struct _bank_accounts * next;

} bank_accounts;

 
/*
	this function will handle the client when client
	is in service session with his account. This 
	function will call upon other functions to check
	if the account exists and is in session or not.
	NOTE: This fuction return 1 to close the service session
		as it found out that the accound doesn't exists
		or the account is in service.
*/
int service_func(int * sock_fd, char * account_name_char);






/*
	this is the first function called by the thread.
	this function has a while loop that keeps going
	around and read stuff from client until there is
	no message coming from client. 
*/
void * connection_handler (void * socket_fd);



/*
	this function is the second function called by the 
	thread. This fuction creates a new account and 
	calls another function if user wants to serve the 
	account. 
*/
int string_handler (int * sock_fd, char * buffer);





/*
	this function looks for the account 
	based on the char * provided. If
	couldn't locate the account in the 
	main_list, the it will return NULL.

	NOTE: this function won't check if the 
		account is in session or not.
*/
bank_accounts * search_for_account (char * account_name_char); 




//this function checks if the account is in session or nor
//-1 yes in session, 0 it's not in session
//int in_session (bank_accounts * account);

#endif
