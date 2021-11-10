#include "stdio.h"
#include "server_pipe.h"

#include "../trialfuncs.h"


#define client_f TEXT("ClientF")


char* getline() {
    char* line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0) {
            len = lenmax;
            char* linen = realloc(linep, lenmax *= 2);

            if (linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}

int main(int argc, char agrv[])
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

    const char calc_command[] = "calc";
    const char exit_command[] = "exit\n";    

    while (true)
    {
        char* command = getline();

        char seps[] = " ";

        char* token = NULL;
        char* next_token = NULL;

        token = strtok_s(command, seps, &next_token);        

        if (strcmp(token, calc_command) == 0)
        {
            token = strtok_s(NULL, seps, &next_token);

            int value = atoi(token);

            int iresultf;
            compfunc_status_t statusf;
            statusf = trial_f_imul(value, &iresultf);
            printf("f_imul(%d%s%s%s", value, ") : ", symbolic_status(statusf), "\n");
            if (statusf == COMPFUNC_SUCCESS)
                printf("f_imul(1): %d\n", iresultf);

            int iresult;
            compfunc_status_t status;
            status = trial_g_imul(value, &iresult);
            printf("f_imul(%d%s%s%s", value, ") : ", symbolic_status(status), "\n");
            if (status == COMPFUNC_SUCCESS)
                printf("f_imul(1): %d\n", iresult);
        }
        else if (strcmp(token, exit_command) == 0)
        {
            TerminateProcess(fpi.hProcess, 0);
            TerminateProcess(gpi.hProcess, 0);
            CloseHandle(fpi.hProcess);
            CloseHandle(fpi.hThread);
            CloseHandle(gpi.hProcess);
            CloseHandle(gpi.hThread);
            return 0;
        }
        else
        {
            printf("Wrong command\n");
        }
    }

	printf("Hello, Server!\n");
	InitServerNamedPipes();	
	InitPipesHandlers();
	CloseServerPipes();
	return 0;
}