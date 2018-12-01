#ifndef BANKACTIVITY_H
#define BANKACTIVITY_H


/*
this struct keeps info about each bank account.
Each account will have unique username, starting
balance of $0.0 and session flag will be set to
0 in the begging and will be changed to 1 if the
account is in session.
*/
typedef struct  _bank_accounts {
	char username[256];
	float balance;
	int session_flag;
	struct _bank_accounts * next;
} bank_accounts;






/*
this function will check if 
the username provided by client 
to make new username already exists 
or not. If the account with the same 
username exists, then it will printout
an error and return -1; if doesn't exist
then will make a new account with that
username with starting balance of $0.0
*/
int add_account (char * new_account, bank_account * main_list);

#endif
