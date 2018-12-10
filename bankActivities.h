#ifndef BANKACTIVITIES_H
#define BANKACTIVITIES_H


/*
this struct keeps info about each bank account.
Each account will have unique username, starting
balance of $0.0 and session flag will be set to
0 in the begging and will be changed to 1 if the
account is in session.
*/
typedef struct bank_accounts {
	char username[256];
	float balance;			//Ever accout starts off with 0.0
	int session_flag;		//if it's 0 then account is in session, if -1 then account isn't in session. No other values
	struct bank_accounts * next;
}bank_accounts;






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
int add_account (char * new_account, bank_accounts * main_list);




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