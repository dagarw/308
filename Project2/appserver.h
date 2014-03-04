#ifndef APPSERVER
#define APPSERVER

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

#ifndef PTHREAD
#define PTHREAD
#include <pthread.h>
#endif

#ifndef BANK
#define BANK
#include "Bank.h"
#endif

/*structure used to store a mutex lock associated with each bank account*/
typedef struct account_struct
{
	pthread_mutex_t lock;
	int value;
}account;

#endif