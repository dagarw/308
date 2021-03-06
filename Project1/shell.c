#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINPUTSIZE 100

int main(int argc, char **argv)
{
	/*arrays to store the shell prompt, user input,
	directory, environment variable, and evironment
	value*/
	char* prompt = "308sh> ";
	char* backgroundCommand;
	char input[MAXINPUTSIZE];
	char dir[MAXINPUTSIZE];
	char variable[MAXINPUTSIZE];
	char value[MAXINPUTSIZE];
	
	/*status variable for execvp*/
	int status;
	
	/*variable that will be 0 in child processes*/
	int child;
	int child2;
	
	/*counters*/
	int i, k;
	int numArguments;

	/*string to store the current working directory*/
	char cwd[1024];

	/*int that informs program if the process is to be a background process*/
	int backgroundProcess = 0;

	/*INITIALIZATION*/

	/*check for too many or too few arguments. If so display error and exit*/
	if(argc != 3 && argc != 1)
	{
		printf("Invalid command line argument. Exiting.\n");
		return 0;
	}

	/*check for prompt command line argument*/
	if(argc == 3)
	{
		/*if the first argument is not -p than the
		arguments are not supported*/
		if((strlen(argv[1]) != 2) || (strncmp("-p", argv[1], 2) != 0))
		{
			printf("Invalid command line argument. Exiting.\n");
			return 0;
		}
		/*assign prompt argument to prompt variable*/
		prompt = argv[2];
	}

	/*USER INPUT LOOP*/

	while(1)
	{
		/*prompt and wait for input*/
		printf(prompt);

		/*get user input and ensure it is null terminated*/
		fgets (input, MAXINPUTSIZE, stdin);
		if ((strlen(input)>0) && (input[strlen (input) - 1] == '\n'))
		{
        		input[strlen(input) - 1] = '\0';
		}		

		/*BUILTIN COMMAND CHECK*/

		/*if user input is "exit" then exit shell*/
		if((strlen(input) == 4) && (strncmp("exit", input, 4) == 0))
		{
			return 0;
		}

		/*if user input is "pid" then print shell PID*/
		else if((strlen(input) == 3) && (strncmp("pid", input, 3) == 0))
		{
			printf("Process ID of this shell: %d", getpid());
			printf("\n");
		}

		/*if user input is "ppid" then print shell's parent PID*/
		else if((strlen(input) == 4) && (strncmp("ppid", input, 4) == 0))
		{
			printf("Process ID of this shell's parent: %d", getppid());
			printf("\n");
		}

		/*if user input is "cd" then change directory to desired directory*/
		else if(strncmp("cd", input, 2) == 0 && (strlen(input) == 2 || input[2] == ' '))
		{
			if(strlen(input) == 2)
			{
				chdir(getenv("HOME"));
			}
			else
			{
				for(i = 0; i < strlen(input) - 3; i++)
				{
					dir[i] = input[i + 3];
					dir[i+1] = '\0';
				}
				if(chdir(dir) == -1)
				{
					printf("Cannot find directory\n");
				}
			}
		}

		/*if user input is "pwd" then print current working directory*/
		else if((strlen(input) == 3) && (strncmp("pwd", input, 3) == 0))
		{	
			if(getcwd(cwd, 1024) == -1)
			{
				printf("Failed to print current working directory");
			}
			printf("%s\n", cwd);
		}

		/*if user input is "set" then set the given environment variable with the given value*/
		else if(strncmp("set", input, 3) == 0 && input[3] == ' ')
		{
			/*initialize k: at end of for loop k < -1 fails, k = -1 clears environment variable, k > -1 sets environment variable*/
			k = -2;
			for(i=4; i < strlen(input); i++)
			{
				/*if we entered the for loop and the current character isn't a space
				then we have atleast one argument*/
				if(k == -2 && input[i] != ' ')
				{
					k = -1;
				}
				/*figure out argument count at space character*/
				if(input[i] == ' ')
				{
					/*if k is not negative 1 and we have reaches a space character
					we have an invalid number of arguments*/ 
					if(k != -1 || i == strlen(input) - 1)
					{
						printf("Invalid number of arguments for set\n");
						k = -3;
					}
					else
					{
						k=0;
					}
				}
				/*if k = -1 we are filling the variable string*/
				else if(k == -1)
				{
					variable[i-4] = input[i];
				}
				/*if k is > -1 we are filling the variable string*/
				else
				{
					value[k] = input[i];
					k++;
				}
			}
			if(k == -2)
			{
				printf("Invalid number of arguments for set\n");
			}
			else if (k == -1)
			{
				if(setenv(variable, NULL, 1) == -1)
				{
					printf("Failed to clear environment variable\n");
				}
			}
			else
			{
				if(setenv(variable, value, 1) == -1)
				{
					printf("Failed to set environment variable\n");
				}
			}
		}

		/*if user input is "get" then get the given environment variable*/
		else if(strncmp("get", input, 3) == 0 && input[3] == ' ')
		{
			for(i = 0; i < strlen(input - 4); i++)
			{
				variable[i] = input[i+4];
			}
			if(getenv(variable) == NULL)
			{
				printf("Failed to find environment variable\n");
			}
			else
			{
				printf("%s\n", getenv(variable));
			}
		}		

		/*NOT BUILTIN? ATTEMPT TO EXECUTE AS COMMAND*/
		
		else
		{
			/*count number of arguments for args[]*/
			numArguments = 1;
			for(i = 0; i < strlen(input); i++)
			{
				if (input[i] == ' ')
				{
					numArguments++;
				}
			}
			/*check to see if last argument is '&' character if so flag as backgroundProcess*/
			if(input[strlen(input) - 1] == '&')
			{
				numArguments--;
				backgroundProcess = 1;
			}
			/*fill args[] using temp string x for each individual argument*/
			char x[numArguments][MAXINPUTSIZE];
			char* args[numArguments + 1];
			numArguments = 0;
			k = 0;
			for(i = 0; i < strlen(input) + 1; i++)
			{
				if (input[i] == ' ' || input[i] == '\0')
				{
					x[numArguments][k] = '\0';
					args[numArguments] = x[numArguments];
					numArguments++;
					k = 0;
				}
				else if (input[i] == '&' && i == strlen(input) - 1)
				{
					i = strlen(input) + 1;
				}
				else
				{
					x[numArguments][k] = input[i];
					k++;
				}
			}
			/*add null pointer to end of args*/
			args[numArguments] = (char*) NULL;

			/*if background process spawn a child (child2) that will spawn another child (child) to run the process.
			child2 will wait for child to complete and  then print out the completion text. The main process continues
			the loop and re-prompts for user input*/
			if(backgroundProcess == 1)
			{
				backgroundProcess = 0;
				child2 = fork();
				if(child2 == 0)
				{
					child = fork();
					if(child == 0)
					{
						/*print process ID and name of command*/
						printf("[%d] %s\n", getpid(), args[0]);
						/*execute command*/
						execvp(args[0], args);
						/*if we reach this point execution has failed so inform user of
						unrecognized command*/
						perror("\0");
						return 0;
					}
					else
					{
						status = -1;
						waitpid(-1, &status, 0);
						/*print the completed child process ID name and return status*/
						if(1)
						{
							printf("\n[%d] %s Exit %d\n", child, args[0], WEXITSTATUS(status));
							printf(prompt);
						}
						else
						{
							printf("[%d] %s Exit %d\n", child, args[0], WEXITSTATUS(status));
						}
						return 0;
					}
				}
				else
				{
					usleep(1000);
				}
			}

			/*not background process. spawn child (child) that runs command and wait for it to complete before
			accepting more input*/
			else
			{
				/*create child process to run command*/
				child = fork();
				/*if child = 0 we are in the child process*/
				if(child == 0)
				{
					/*print process ID and name of command*/
					printf("[%d] %s\n", getpid(), args[0]);
					/*execute command*/
					execvp(args[0], args);
					/*if we reach this point execution has failed so inform user of
					unrecognized command*/
					perror("\0");
					return 0;
				}
				/*parent process*/
				else
				{
					usleep(1000);
					/*initialize status and wait for child process to complete*/
					status = -1;
					waitpid(child, &status, 0);
					/*print the completed child process ID name and return status*/
					printf("[%d] %s Exit %d\n", child, args[0], WEXITSTATUS(status));
				}
			}
		}
	}
}
