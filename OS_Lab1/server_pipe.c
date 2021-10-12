#include "server_pipe.h"

void InitNamedPipe(HANDLE handle_named_pipe, LPCTSTR PIPE_NAME)
{
    handle_named_pipe = CreateNamedPipe(
        PIPE_NAME,             // pipe name 
        PIPE_ACCESS_DUPLEX,       // read/write access 
        PIPE_TYPE_MESSAGE |       // message type pipe 
        PIPE_READMODE_MESSAGE |   // message-read mode 
        PIPE_WAIT,                // blocking mode 
        PIPE_UNLIMITED_INSTANCES, // max. instances  
        BUFSIZE,                  // output buffer size 
        BUFSIZE,                  // input buffer size 
        0,                        // client time-out 
        NULL);                    // default security attribute 
    if (handle_named_pipe == INVALID_HANDLE_VALUE)
    {
        printf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
        return -1;
    }
}

void InitServerNamedPipes()
{
    InitNamedPipe(fhandle_named_pipe, FPIPE_NAME);
    InitNamedPipe(ghandle_named_pipe, GPIPE_NAME);
}

void CloseServerPipes()
{
    CloseHandle(fhandle_named_pipe);
    CloseHandle(ghandle_named_pipe);
}
