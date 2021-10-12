#include "server_pipe.h"

void InitNamedPipe(HANDLE handle_named_pipe, LPCTSTR PIPE_NAME)
{
    handle_named_pipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE |
        PIPE_READMODE_MESSAGE |
        PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFSIZE,
        BUFSIZE,
        0,
        NULL);
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
