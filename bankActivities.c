#include <stdio.h>
#include <stlib.h>
#include <string.h>
#include <linkedList.h>
#include "bankActivities.h"


int withdrawl (float amount, bank_accounts * account) {
	if (account->balance < amount) {
		fprintf("Error! Not enought money in the account.\n");
		return -1;
	}
	account->balance = account->balance - amount;
	return 0;
}



int deposit (float amount, bank_accounts * account) {
	if(amount <= 0) {
		fprintf("Error! Deposit amount needs to be greater than 0.\n");
		return -1;
	}
	account->balance = account->balance + amount;
	return 0;
}


int add_account (char * new_account, bank_accounts * main_list) {
	bank_accounts * temp = main_list->next;
	bank_accounts * ptr = main_list;
	
	//checking if the new_accout is same as the first account's username
	if(strcmp(new_account, ptr->username) == 0) {
		fprintf(stderr, "Error! Account with this username already exists.\n");
		return -1;
	}
	

	//checking if the there is any account with similar username as new_accout, if then error message and return -1
	while (temp != NULL) {
		if(strcmp(new_account, temp->username) == 0)	 {
			fprintf(stderr, "Error! Account with this username already exists.\n");
			return -1;
		}
		ptr = temp;
		temp = temp->next;
	}

	
	//getting to this point means there is no account with same username as new_accout, and thus creating a new
	//bank account and adding it to the end of the linked list 
	if(temp == NULL) {
		bank_accounts * another_account = (bank_accouts *) malloc (sizeof(bank_accounts));
		strcpy(another_account->username, new_account);
		another_account->balance = 0.0;
		another_account->session_flag = 0;
		another_account->next = NULL;
		ptr->next = another_account;
	}	
	return 0;
}

