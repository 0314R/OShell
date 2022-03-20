#include "processManagement.h"

void executeCommand(char *executable, FlexArray args)
{
	pid = fork();

	if(pid < 0)
	{
		fprintf(stderr, "fork fail\n");
	}
	else if( pid > 0)
	{
		wait(NULL);						// Parent waits until child terminates.
		emptyFlexArray(&args);			// Clean array of arguments for next command.
		//free(executable);				// The executable needs to be freed too.
	}
	else
	{									// Child: actually execute the code.
		add(NULL, &args);				// The last "argument" should be NULL for execvp to work
		execvp(executable, args.arr);
		exit(EXIT_SUCCESS);				// If execvp does not recognize the executable, the process still exits.
	}
}
