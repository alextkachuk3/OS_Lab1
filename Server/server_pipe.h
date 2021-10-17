#pragma once

#include "stdio.h"
#include <Windows.h>

#define BUFSIZE 512

#define FPIPE_NAME TEXT("\\\\.\\pipe\\fnamedpipe")
#define GPIPE_NAME TEXT("\\\\.\\pipe\\gnamedpipe")

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

void InitServerNamedPipes();

int InitNamedPipe(HANDLE* handle_named_pipe, LPCTSTR PIPE_NAME);

int InitPipeHandler(HANDLE* handle_named_pipe);

void CloseServerPipes();