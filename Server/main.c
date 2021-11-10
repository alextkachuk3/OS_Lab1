#include "stdio.h"
#include "server_pipe.h"

#include "../trialfuncs.h"
#include "queue.h"

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
    int* run = (int*)malloc(sizeof(int));

    *run = 1;

    int* cancel = (int*)malloc(sizeof(int));

    *cancel = 0;

    PQUEUE q = (PQUEUE*)malloc(sizeof(QUEUE)); 
    init(q);

    PSERVER_PIPE_DATA s_data;
    s_data = (PSERVER_PIPE_DATA*)malloc(sizeof(SERVER_PIPE_DATA));

    s_data->q = q;
    s_data->run = run;
    s_data->cancel = cancel;

    HANDLE pipe_server_thread_handle;
    DWORD thread_id;

    pipe_server_thread_handle = CreateThread(
        NULL,
        0,
        InitPipeServer,
        (LPVOID)s_data,
        0,
        &thread_id);

    const char y_command[] = "y\n";
    const char n_command[] = "n\n";
    const char calc_command[] = "calc";
    const char cancel_command[] = "cancel\n";

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

            insert(q, value);
        }
        else if (strcmp(token, exit_command) == 0)
        {
            if (s_data)
            {
                s_data->run = 0;
            }
            return 0;
        }
        else if (strcmp(token, cancel_command) == 0)
        {
            while (1)
            {
                printf("Please confirm that computation shoud be stopped [y]es, stop/[n]ot yet\n");
                char* confirm = getline();

                if (strcmp(confirm, y_command) == 0)
                {
                    *cancel = 1;
                    break;
                }
                else if (strcmp(confirm, n_command) == 0)
                {
                    break;
                }
                else
                {
                    printf("Type y or n\n");
                }

                free(confirm);
            }
        }
        else
        {
            printf("Wrong command\n");
        }
    }

    WaitForSingleObject(pipe_server_thread_handle, INFINITE);
	printf("Hello, Server!\n");

    free(run);
    free(cancel);
    free(s_data);

	return 0;
}