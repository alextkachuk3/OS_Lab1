#include <Windows.h>

#define BUFSIZE 512

#define FPIPE_NAME TEXT("\\\\.\\pipe\\fnamedpipe")
#define GPIPE_NAME TEXT("\\\\.\\pipe\\gnamedpipe")

HANDLE fhandle_named_pipe;
HANDLE ghandle_named_pipe;

void InitServerNamedPipes();

void InitNamedPipe(HANDLE handle_named_pipe, LPCTSTR PIPE_NAME);

void CloseServerPipes();