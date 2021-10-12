#include "stdio.h"
#include "server_pipe.h"

int main(int argc, char agrv[])
{
	printf("Hello, Server!");
	InitServerNamedPipes();
	CloseServerPipes();
	return 0;
}