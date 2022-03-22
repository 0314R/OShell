#include "processManagement.h"

int executeCommand(char *executable, FlexArray *args)
{
	int fdIn, status;

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
	{									// Child: actually execute the executable.
		add(NULL, args);				// The last "argument" should be NULL for execvp to work

		fdIn = open("README.txt", O_RDONLY);
		dup2(fdIn, STDIN_FILENO);		// Replace standard input by specified input file
		close(fdIn);

		if( strcmp(executable, "exit") == 0){
			exit(EXIT_COMMAND);					// Special exit value
		}
		else if( execvp(executable, args->arr) == -1){
			printf("Error: command not found!\n");
			exit(EXIT_FAILURE);			// The process exits anyway, but lets the parent know it was unsuccesful.
		}

	}
	return WEXITSTATUS(status);
}

char *removeQuotes(char *quotedInput){
	char *output;
	int newLen, oldLen = strlen(quotedInput);

	// printf("removing quotes from string of length %d\noriginal string: ", oldLen);
	// for(int i=0 ; i<(oldLen+1) ; i++)
	// 	printf("%d ", quotedInput[i]);
	// putchar('\n');

	newLen = oldLen - 2; 							//As per the two removed quotes.
	output = malloc( (newLen+1) * sizeof(char));  	// +1 for the null character
	memmove(output, quotedInput+1, newLen);			// +1 to ignore the first character (quote)
	output[newLen] = '\0';							// ensure the new string has the right length.
	free(quotedInput);
	// printf("new string length: %d %d\n", newLen, (int)strlen(output));
	// for(int i=0 ; i<(newLen+1) ; i++)
	// 	printf("%d ", output[i]);
	// putchar('\n');

	return output;
}
