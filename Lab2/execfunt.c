#include <stdio.h>

int main()
{
	execl("/bin/ls", "ls", NULL);
	printf("What happened?\n");
}
