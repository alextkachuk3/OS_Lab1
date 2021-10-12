#include "pipe_client.h"

int InitClientNamedPipe(HANDLE* pipe_handle, LPCTSTR pipe_name)
{
    while (1)
    {
        pipe_handle = CreateFile(
            pipe_name,
            GENERIC_READ |
            GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);


        if (pipe_handle != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            printf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
            return -1;
        }

        if (!WaitNamedPipe(pipe_name, 20000))
        {
            printf("Could not open pipe: 20 second wait timed out.");
            return -1;
        }        
    }

    return 0;
}
