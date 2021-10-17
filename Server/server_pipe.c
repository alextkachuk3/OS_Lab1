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
    return 0;
}

int InitPipeHandler(PPIPEDATA pipe_data, HANDLE* thread_handle, DWORD* thread_id)
{
    *thread_handle = CreateThread(
        NULL,
        0,
        PipeInstanceThread,
        (LPVOID)pipe_data,
        0,
        thread_id);

    if (thread_handle == NULL)
    {
        printf("CreateThread failed, GLE=%d.\n", GetLastError());
        return -1;
    }
    return 0;
}

int InitPipesHandlers()
{
    g_thread_id = 0;
    f_thread_id = 0;

    g_thread_handle = NULL;
    f_thread_handle = NULL;

    InitPipeHandler(f_pipe_data, &f_thread_handle, &f_thread_id);
    InitPipeHandler(g_pipe_data, &g_thread_handle, &g_thread_id);

    WaitForSingleObject(f_thread_handle, INFINITE);
    WaitForSingleObject(g_thread_handle, INFINITE);

    return 0;
}

void InitServerNamedPipes()
{
    f_pipe_data = (PPIPEDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(PIPEDATA));
    g_pipe_data = (PPIPEDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(PIPEDATA));
    f_pipe_data->pipe_handle = INVALID_HANDLE_VALUE;
    g_pipe_data->pipe_handle = INVALID_HANDLE_VALUE;

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

DWORD __stdcall PipeInstanceThread(LPVOID param)
{
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
    TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    PPIPEDATA pipe_data = NULL;

    if (param == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
        printf("   InstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    if (pchRequest == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InstanceThread got an unexpected NULL heap allocation.\n");
        printf("   InstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if (pchReply == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InstanceThread got an unexpected NULL heap allocation.\n");
        printf("   InstanceThread exitting.\n");
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    pipe_data = (PPIPEDATA)param;

    BOOL connected = FALSE;

    connected = ConnectNamedPipe(pipe_data->pipe_handle, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (connected)
    {
        while (TRUE)
        {
            if (FAILED(StringCchCopy(pchReply, BUFSIZE, TEXT("test value"))))
            {
                cbReplyBytes = 0;
                pchReply[0] = 0;
                printf("StringCchCopy failed, no outgoing message.\n");
                return;
            }
            cbReplyBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);

            fSuccess = WriteFile(
                pipe_data->pipe_handle,
                pchReply,
                cbReplyBytes,
                &cbWritten,
                NULL);

            if (!fSuccess || cbReplyBytes != cbWritten)
            {
                _tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
                break;
            }

            fSuccess = ReadFile(
                pipe_data->pipe_handle,
                pchRequest,
                BUFSIZE * sizeof(TCHAR),
                &cbBytesRead,
                NULL);

            if (!fSuccess || cbBytesRead == 0)
            {
                if (GetLastError() == ERROR_BROKEN_PIPE)
                {
                    _tprintf(TEXT("InstanceThread: client disconnected.\n"));
                }
                else
                {
                    _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
                }
                break;
            }

            _tprintf(TEXT("Client Request String:\"%s\"\n"), pchRequest);
        }
    }

    FlushFileBuffers(pipe_data->pipe_handle);
    DisconnectNamedPipe(pipe_data->pipe_handle);

    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);

    return (DWORD)0;
}
