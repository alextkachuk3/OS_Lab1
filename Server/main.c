#include "stdio.h"
#include "server_pipe.h"

int main(int argc, char agrv[])
{
	printf("Hello, Server!\n");
	InitServerNamedPipes();	
	InitPipesHandlers();
	CloseServerPipes();
	return 0;
}