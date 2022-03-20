#include "processManagement.h"

int executeCommand(char *executable, FlexArray *args)
{
	int status;
	pid = fork();

	if(pid < 0)
	{
		fprintf(stderr, "fork fail\n");
	}
	else if( pid > 0)
	{
		wait(&status);	// Parent waits until child terminates. Return value 0 if success
		free(executable);				// The executable needs to be freed too.
	}
	else
	{									// Child: actually execute the code.
		add(NULL, args);				// The last "argument" should be NULL for execvp to work

		if( execvp(executable, args->arr) == -1){
			printf("Error: command not found!\n");
			exit(EXIT_FAILURE);			// The process exits anyway, but lets the parent know it was unsuccesful.
		}

	}
	return status;
}
