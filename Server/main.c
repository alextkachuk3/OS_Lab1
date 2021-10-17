#include "stdio.h"
#include "server_pipe.h"

#include "../trialfuncs.h"

int main(int argc, char agrv[])
{
    compfunc_status_t status;
    double fresult;
    int iresult;
    printf("f(0) and g(0): \n");
    status = trial_f_imul(0, &iresult);
    printf("f_imul(0): %s\n", symbolic_status(status));
    if (status == COMPFUNC_SUCCESS)
        printf("f_imul(0): %d\n", iresult);
    printf("f(0): %d\n", trial_f_imul(0, &iresult));
    PROCESS_FUNC(f, imul, 0);
    printf("g(0): %d\n", trial_g_imul(0, &iresult));
    PROCESS_FUNC(g, imul, 0);
    printf("g(1): %d\n", trial_g_imul(1, &iresult));
    PROCESS_FUNC(g, imul, 1);
    printf("f(2): %d\n", trial_f_imul(2, &iresult));
    PROCESS_FUNC(f, imul, 2);
    printf("g(3): %d\n", trial_g_imul(3, &iresult));
    PROCESS_FUNC(g, imul, 3);

    PROCESS_FUNC(g, or , -1);
    PROCESS_FUNC(g, or , 0);
    PROCESS_FUNC(g, imin, 0);
       

	printf("Hello, Server!\n");
	InitServerNamedPipes();	
	InitPipesHandlers();
	CloseServerPipes();
	return 0;
}