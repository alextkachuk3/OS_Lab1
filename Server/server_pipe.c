#include "server_pipe.h"

int InitNamedPipe(HANDLE* handle_named_pipe, LPCTSTR PIPE_NAME)
{
    *handle_named_pipe = CreateNamedPipe(
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
    if (*handle_named_pipe == INVALID_HANDLE_VALUE)
    {
        printf("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
        return -1;
    }
}

int InitPipeHandler(HANDLE* handle_named_pipe)
{
    BOOL connected = FALSE;

    connected = ConnectNamedPipe(handle_named_pipe, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (connected)
    {
        printf("Client connected, creating a processing thread.\n");
                
        
        return 0;
    }

    return -1;
}

void InitServerNamedPipes()
{
    f_pipe_data = (PPIPEDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(PIPEDATA));
    g_pipe_data = (PPIPEDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(PIPEDATA));
    InitNamedPipe(&f_pipe_data->pipe_handle, FPIPE_NAME);
    InitNamedPipe(&g_pipe_data->pipe_handle, GPIPE_NAME);
}

void CloseServerPipes()
{
    CloseHandle(f_pipe_data->pipe_handle);
    CloseHandle(g_pipe_data->pipe_handle);
    HeapFree(GetProcessHeap(), 0, f_pipe_data);
    HeapFree(GetProcessHeap(), 0, g_pipe_data);
}
