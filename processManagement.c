#include "processManagement.h"

void openInput(char *fileName, int *io){
	io[0] = open(fileName, O_RDONLY);
	if(io[0] < 0)
        printf("Error: no such input file\n");

	free(fileName);
}

void openOutput(char *fileName, int *io){
	io[1] = open(fileName, O_RDWR | O_CREAT);
	if(io[1] < 0)
        printf("Error: could not open/create output file\n");

	free(fileName);
}

int cd(FlexArray *args){
	//cd expects exactly one option/argument, namely the path.
	switch( args->len - 1){		// The first "argument" in the array is the cd command itself. Therefore -1.
		case 0: printf("Error: cd requires folder to navigate to!\n"); return 0;
		case 1: break;
		default: printf("Error: cd directory not found!\n"); return 0;
	}

	char *path = args->arr[1];
	int status = chdir(path);
	if(status != 0)
		printf("Error: cd directory not found!\n");
	//printf("cd status %d\n", status);
	// printf("cd to %s\n", path);

	return status;
}

int executeCommands(char commands[10][20][256], int nc, int *rowLens, int io[2]){
	printf("EXECUTING %d COMMANDS\n", nc); //nC = number of Commands

	FlexArray argArr;

	int **pipes = malloc( (nc-1) * sizeof(int *));
	for(int i=0 ; i < (nc-1) ; i++){
		pipes[i] = malloc( 2 * sizeof(int));
		if( pipe(pipes[i]) == -1){
			printf("Error: failed to pipe.\n");
			return(EXIT_FAILURE);
		} else {
			printf("Pipe %d: [%d,%d]\n", i, pipes[i][0], pipes[i][1]);
		}
	}

	pid_t *pids = malloc(nc * sizeof(pid_t));
	assert(pids != NULL);
	int *status = malloc(nc * sizeof(int)), parentStatus = EXIT_SUCCESS;
	assert(status != NULL);

	for(int p=0 ; p<nc ; p++){
		pids[p] = fork();

		if(pids[p] < 0){
			printf("Error: failed to fork\n");
			return(EXIT_FAILURE);
		}
		else if(pids[p] == 0){
			printf("I am child %d\n", p);

			argArr = staticToFlexArray(commands[p], rowLens[p]);
			//printFlexArray(argArr);
			emptyFlexArray(&argArr);
			free(argArr.arr);

			exit(EXIT_SUCCESS);
		}
	}
	for(int p=0 ; p<nc ; p++){
		waitpid(pids[p], &status[p], 0);
		printf("child %d returned with %d==%d\n", p, status[p], WEXITSTATUS(status[p]));
		if(status[p] != EXIT_SUCCESS){
			parentStatus = EXIT_FAILURE;
		}
	}
	printf("All children done\n");

	for(int i=0 ; i < (nc-1) ; i++)
		free(pipes[i]);
	free(pipes);
	free(pids);
	free(status);

	return parentStatus;
}

int executeCommand(FlexArray *args, int io[2])
{
	int fdIn=io[0], fdOut = io[1], status, dupIn, dupOut;
	char *executable = args->arr[0];
	//printf("fdIn = %d, fdOut = %d\n", io[0], io[1]);

	if(io[0] == -1 || io[1] == -1)
		return EXIT_FAILURE;

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
		add(NULL, args);				// The last "argument" should be NULL for execvp to work


		dupIn = dup2(fdIn, STDIN_FILENO);		// Replace standard input by specified input file
		if(dupIn < 0)
	        printf("Error opening the input file\n");
		dupOut = dup2(fdOut, STDOUT_FILENO);
		if(dupOut < 0)
	        printf("Error opening the output file\n");

		if(fdIn != 0 && fdIn != -1) close(fdIn);
		if(fdOut != 1 && fdOut != -1) close(fdOut);

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
