#ifndef BANKINGSERVER_H
#define BANKINGSERVER_H

/*
this struct keeps info about each bank account.
Each account will have unique username, starting
balance of $0.0 and session flag will be set to
0 in the begging and will be changed to 1 if the
account is in session.
*/
typedef struct _bank_accounts {
	char username[256];
	float balance;			//Ever accout starts off with 0.0
	int session_flag;		//if it's 0 then account is in session, if -1 then account isn't in session. No other values
	struct _bank_accounts * next;
}bank_accounts;




/*
	this function will handle the client when client
	is in service session with his account. This 
	function will call upon other functions to check
	if the account exists and is in session or not.
	NOTE: This fuction return 1 to close the service session
		as it found out that the accound doesn't exists
		or the account is in service.
*/
int service_func(int sock_fd, char * account_name_char);






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
int in_session (bank_accounts * account);






/*
this function will check if 
the username provided by client 
to make new username already exists 
or not. If the account with the same 
username exists, then it will printout
an error and return -1; if doesn't exist
then will make a new account with that
username with starting balance of $0.0
	Return Value			Meaning
		0 :-			Account was successfully added
		-1 :-			Account with that user name already exists
*/
int add_account (char * new_account);




/*
this function will withdraw money from
the specified account. If there isn't 
enought money to withdraw the specified 
amount by the client, this function will
print out an error message and return -1, 
else it would subtract the withdrawl amount
from the balance of the specified account.
*/
int withdraw (float amount, bank_accounts * account);




/*
this function will deposit the amount 
specified the client into his/her 
specified bank account. It will check if the amount
specified by the client if correct or not i.e., it 
can't be less than or equal to 0.
*/
int deposit (float amount, bank_accounts * account);


#endif
