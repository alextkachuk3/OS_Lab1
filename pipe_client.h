#pragma once

#include "stdio.h"
#include <Windows.h>

int InitClientNamedPipe(HANDLE* pipe_handle, LPCTSTR pipe_name);