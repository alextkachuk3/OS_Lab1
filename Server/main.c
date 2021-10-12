#include "stdio.h"
#include "server_pipe.h"

int main(int argc, char agrv[])
{
	printf("Hello, World!");
	InitServerNamedPipes();
	CloseServerPipes();
	return 0;
}