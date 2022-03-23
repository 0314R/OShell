#include "processManagement.h"

void openInput(char *fileName, int *io){
	printf("before opening input %d %d %d %d\n", io[0], STDIN_FILENO, io[1], STDOUT_FILENO);

	io[0] = open(fileName, O_RDONLY);
	if(io[0] < 0)
        printf("Error opening the file\n");

	printf("after opening input %d %d %d %d\n", io[0], STDIN_FILENO, io[1], STDOUT_FILENO);
	free(fileName);
}

void openOutput(char *fileName, int *io){
	printf("before opening output %d %d %d %d\n", io[0], STDIN_FILENO, io[1], STDOUT_FILENO);

	io[1] = open(fileName, O_RDWR | O_CREAT);
	if(io[1] < 0)
        printf("Error opening the file\n");

	printf("after opening output %d %d %d %d\n", io[0], STDIN_FILENO, io[1], STDOUT_FILENO);
	free(fileName);
}

int executeCommand(FlexArray *args, int inAndOutput[2])
{
	int fdIn=inAndOutput[0], fdOut = inAndOutput[1], status, dupIn, dupOut;
	char *executable = args->arr[0];
	//printf("fdIn = %d, fdOut = %d\n", inAndOutput[0], inAndOutput[1]);

	pid = fork();

	if(pid < 0)
	{
		fprintf(stderr, "fork fail\n");
	}
	else if( pid > 0)
	{
		wait(&status);	// Parent waits until child terminates. Return value 0 if success
		//free(executable);				// The executable needs to be freed too.
	}
	else
	{									// Child: actually execute the executable.
		addToFlexArray(NULL, args);				// The last "argument" should be NULL for execvp to work


		printf("before dup %d %d %d %d\n", fdIn, STDIN_FILENO, fdOut, STDOUT_FILENO);
		dupIn = dup2(fdIn, STDIN_FILENO);		// Replace standard input by specified input file
		if(dupIn < 0)
	        printf("Error opening the input file\n");
		dupOut = dup2(fdOut, STDOUT_FILENO);
		if(dupOut < 0)
	        printf("Error opening the output file\n");
		printf("after dup %d %d %d %d\n", fdIn, STDIN_FILENO, fdOut, STDOUT_FILENO);

		if(fdIn>1) close(fdIn);
		if(fdOut>1) close(fdOut);

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
