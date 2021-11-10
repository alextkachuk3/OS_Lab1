#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#include "../trialfuncs.h"

#define BUFSIZE 512

int _tmain(int argc, TCHAR* argv[])
{
    HANDLE hPipe;
    wchar_t lpvMessage[100];
    TCHAR  chBuf[BUFSIZE];
    BOOL   fSuccess = FALSE;
    DWORD  cbRead, cbToWrite, cbWritten, dwMode;
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\fnamedpipe");

    while (1)
    {
        hPipe = CreateFile(
            lpszPipename,
            GENERIC_READ | 
            GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            _tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
            return -1;
        }

        if (!WaitNamedPipe(lpszPipename, 20000))
        {
            printf("Could not open pipe: 20 second wait timed out.");
            return -1;
        }
    }

    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(
        hPipe,
        &dwMode,
        NULL,
        NULL);
    if (!fSuccess)
    {
        _tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
        return -1;
    }

    do
    {
        fSuccess = ReadFile(
            hPipe,
            chBuf,
            BUFSIZE * sizeof(TCHAR),
            &cbRead,
            NULL);

        if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
            break;

        _tprintf(TEXT("\"%s\"\n"), chBuf);
    } while (!fSuccess);

    if (!fSuccess)
    {
        _tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
        return -1;
    }

    int value = _wtoi(&chBuf);

    int iresult;
    compfunc_status_t status;
    status = trial_f_imul(value, &iresult);
    printf("f_imul(%d%s%s%s%i%s", value, ") : ", symbolic_status(status), " ", iresult, "\n");
    size_t size = strlen(symbolic_status(status)) + 1;
    wchar_t* wc = (wchar_t*)malloc(sizeof(wchar_t) * size);
    size_t outSize;
    mbstowcs_s(&outSize, wc, size, symbolic_status(status), size-1);
    wcscpy_s(lpvMessage, 100, wc);
    wcscat_s(lpvMessage, 100, L" ");
    wchar_t* result = (wchar_t*)malloc(sizeof(wchar_t) * 16);    
    swprintf_s(result, 16, L"%d", iresult);
    wcscat_s(lpvMessage, 100, result);

    cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
    _tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

    fSuccess = WriteFile(
        hPipe,
        lpvMessage,
        cbToWrite,
        &cbWritten,
        NULL);

    if (!fSuccess)
    {
        _tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
        return -1;
    }

    CloseHandle(hPipe);

    return 0;
}