#pragma once

#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>

#include "queue.h"

#define BUFSIZE 512

#define FPIPE_NAME TEXT("\\\\.\\pipe\\fnamedpipe")
#define GPIPE_NAME TEXT("\\\\.\\pipe\\gnamedpipe")

typedef struct server_pipe_data
{
    PQUEUE q;
    int* run;
    int* cancel;

}SERVER_PIPE_DATA, * PSERVER_PIPE_DATA;

enum STATUS
{
    success,
    soft_fail,
    hard_fail
};

typedef struct PipeData {
    HANDLE pipe_handle;
    enum STATUS status;
    int x;
    int result;
} PIPEDATA, * PPIPEDATA;

PPIPEDATA f_pipe_data;
PPIPEDATA g_pipe_data;

DWORD f_thread_id;
DWORD g_thread_id;

HANDLE f_thread_handle;
HANDLE g_thread_handle;

DWORD WINAPI InitPipeServer(LPVOID param);

void InitServerNamedPipes();

int InitNamedPipe(HANDLE* handle_named_pipe, LPCTSTR PIPE_NAME);

int InitPipeHandler(PPIPEDATA pipe_data, HANDLE* thread_handle, DWORD* thread_id);

int InitPipesHandlers(PSERVER_PIPE_DATA server_pipe_data);

DWORD WINAPI PipeInstanceThread(LPVOID param);

void CloseServerPipes();

int CheckInputRowIsEmpty();