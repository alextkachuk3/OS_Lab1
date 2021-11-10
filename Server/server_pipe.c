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


//Initialization of thread for handling each pipe
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


// Function which control timeout, complete status and compution result
int InitPipesHandlers(PSERVER_PIPE_DATA server_pipe_data)
{
    g_thread_id = 0;
    f_thread_id = 0;

    g_thread_handle = NULL;
    f_thread_handle = NULL;

    f_pipe_data->x = remove(server_pipe_data->q);
    g_pipe_data->x = f_pipe_data->x;

    InitPipeHandler(f_pipe_data, &f_thread_handle, &f_thread_id);
    InitPipeHandler(g_pipe_data, &g_thread_handle, &g_thread_id);

    int f_attemp = 1;
    int g_attemp = 1;

    int f_complete = 0;
    int g_complete = 0;

    int current_canceled = 0;

    // 200 * 50 = 10 seconds
    for (int i = 0; i < 200; i++)
    {
        if (*server_pipe_data->cancel == 1)
        {
            TerminateThread(f_thread_handle, 0);
            TerminateThread(g_thread_handle, 0);
            server_pipe_data->cancel = 0;
            current_canceled = 1;
            break;
        }
        DWORD fstatus = WaitForSingleObject(f_thread_handle, 50);
        DWORD gstatus = WaitForSingleObject(g_thread_handle, 50);
        if (fstatus == WAIT_OBJECT_0 && f_complete == 0) 
        {
            if (f_pipe_data->status == hard_fail)
            {
                printf("%s%i%s", "f(", f_pipe_data->x, ") - hard-fail\n");
                f_complete = 1;
            }
            else if (f_pipe_data->status == soft_fail)
            {
                if (f_attemp < 5)
                {
                    InitPipeHandler(f_pipe_data, &f_thread_handle, &f_thread_id);
                    f_attemp++;
                }
                else
                {
                    printf("%s%i%s", "f(", f_pipe_data->x, ") - hard-fail\n");
                    f_complete = 1;
                }
            }
            else if (f_pipe_data->status == success)
            {
                printf("%s%i%s%i%s", "f(", f_pipe_data->x, ") = ", f_pipe_data->result, "\n");
                f_complete = 1;
            }
        }

        if (gstatus == WAIT_OBJECT_0 && g_complete == 0)
        {
            if (g_pipe_data->status == hard_fail)
            {
                printf("%s%i%s", "g(", g_pipe_data->x, ") - hard-fail\n");
                g_complete = 1;
            }
            else if (g_pipe_data->status == soft_fail)
            {
                if (g_attemp < 5)
                {
                    InitPipeHandler(g_pipe_data, &g_thread_handle, &g_thread_id);
                    g_attemp++;
                }
                else
                {
                    printf("%s%i%s", "g(", g_pipe_data->x, ") - hard-fail\n");
                    g_complete = 1;
                }
            }
            else if (g_pipe_data->status == success)
            {
                printf("%s%i%s%i%s", "g(", g_pipe_data->x, ") = ", g_pipe_data->result, "\n");
                g_complete = 1;
            }
        }
    }

    if (current_canceled == 0)
    {
        if (f_complete == 0)
        {
            printf("%s%i%s", "f(", f_pipe_data->x, ") - calc out of time\n");
        }
        if (g_complete == 0)
        {
            printf("%s%i%s", "g(", f_pipe_data->x, ") - calc out of time\n");
        }
    }
    

    return 0;
}

//Function which start computing f(x) and g(x) in other process, if queue is not empty. Run in other thread cause main thread need for handling command without lock
DWORD __stdcall InitPipeServer(LPVOID param)
{
    if (param == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   InitPipeServer got an unexpected NULL value in param.\n");
        printf("   InitPipeServer exitting.\n");
        return (DWORD)-1;
    }

    PSERVER_PIPE_DATA data = (PSERVER_PIPE_DATA)param;

    InitServerNamedPipes();

    while (data->run)
    {
        if (isempty(data->q) == 0)
        {
            STARTUPINFO gsi;
            PROCESS_INFORMATION gpi;

            ZeroMemory(&gsi, sizeof(gsi));
            gsi.cb = sizeof(gsi);
            ZeroMemory(&gpi, sizeof(gpi));

            STARTUPINFO fsi;
            PROCESS_INFORMATION fpi;

            ZeroMemory(&fsi, sizeof(fsi));
            fsi.cb = sizeof(fsi);
            ZeroMemory(&fpi, sizeof(fpi));

            TCHAR clientf_exec[] = TEXT("clientf");
            TCHAR clientg_exec[] = TEXT("clientg");

            if (!CreateProcess(
                NULL,
                clientg_exec,
                NULL,
                NULL,
                TRUE,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL,
                &gsi,
                &gpi)
                )
            {
                printf("CreateProcess failed (%d).\n", GetLastError());
                return;
            }

            if (!CreateProcess(
                NULL,
                clientf_exec,
                NULL,
                NULL,
                TRUE,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL,
                &fsi,
                &fpi)
                )
            {
                printf("CreateProcess failed (%d).\n", GetLastError());
                return;
            }
            InitPipesHandlers(data);
        }
        
    }   

    CloseServerPipes();
    return 0;
}

//Init named pipes
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

//Close named pipes
void CloseServerPipes()
{
    CloseHandle(f_pipe_data->pipe_handle);
    CloseHandle(g_pipe_data->pipe_handle);

    HeapFree(GetProcessHeap(), 0, f_pipe_data);
    HeapFree(GetProcessHeap(), 0, g_pipe_data);
}

//Function in which current process send param to child process and receive result
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
        printf("   PipeInstanceThread got an unexpected NULL value in oaram.\n");
        printf("   PipeInstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    if (pchRequest == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PipeInstanceThread got an unexpected NULL heap allocation.\n");
        printf("   PipeInstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if (pchReply == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PipeInstanceThread got an unexpected NULL heap allocation.\n");
        printf("   PipeInstanceThread exitting.\n");
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
            wchar_t* result = (wchar_t*)malloc(sizeof(wchar_t) * 10);
            swprintf(result, 10, L"%d", pipe_data->x);
            if (FAILED(StringCchCopy(pchReply, BUFSIZE, result)))
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
                    
                }
                else
                {
                    _tprintf(TEXT("ReadFile failed, GLE=%d.\n"), GetLastError());
                }
                break;
            }

            char szString[BUFSIZE];
            size_t nNumCharConverted;
            wcstombs_s(&nNumCharConverted, szString, BUFSIZE,
                pchRequest, BUFSIZE);

            const char success_status[] = "value";
            const char soft_fail_status[] = "soft-fail";
            const char hard_fail_status[] = "hard-fail";

            char seps[] = " ";

            char* token = NULL;
            char* next_token = NULL;

            token = strtok_s(szString, seps, &next_token);
            if (strcmp(token, success_status) == 0)
            {
                pipe_data->status = success;
                token = strtok_s(NULL, seps, &next_token);
                pipe_data->result = atoi(token);
            }
            else if (strcmp(token, soft_fail_status) == 0)
            {
                pipe_data->status = soft_fail;
            }
            else if (strcmp(token, hard_fail_status) == 0)
            {
                pipe_data->status = hard_fail;
            }

            free(result);
        }

    }

    FlushFileBuffers(pipe_data->pipe_handle);
    DisconnectNamedPipe(pipe_data->pipe_handle);

    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);

    return (DWORD)0;
}
