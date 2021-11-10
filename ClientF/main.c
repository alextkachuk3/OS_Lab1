#include <Windows.h>

#include "../pipe_client.h"
#include "../trialfuncs.h"

#define f_pipe_name TEXT("\\\\.\\pipe\\fnamedpipe")

int main(int argc, char argv[])
{
	printf("Hello, Client!");
	HANDLE pipe_handle = NULL;
	InitClientNamedPipe(&pipe_handle, f_pipe_name);
	CloseHandle(pipe_handle);
	getch();
	return 0;
}