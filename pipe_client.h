#pragma once

#include "stdio.h"
#include <Windows.h>
#include <tchar.h>

#define BUFSIZE 512

int InitClientNamedPipe(HANDLE* pipe_handle, LPCTSTR pipe_name);