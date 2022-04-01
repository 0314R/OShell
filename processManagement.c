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

void resetIo(int *io){
	if(io[0] > 1) // not STDIN or STDOUT
		close(io[0]);
	if(io[1] > 1)
		close(io[1]);

	io[0] = 0;
	io[1] = 1;
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

int **initializePipes(int np){
	int **pipes = malloc( np * sizeof(int *));
	assert(pipes != NULL);

	for(int i=0 ; i < np ; i++){
		pipes[i] = malloc( 2 * sizeof(int));

		if( pipe(pipes[i]) == -1){
			printf("Error: failed to pipe.\n");
			return NULL;
		}
	}

	return pipes;
}

void printPipes(int **pipes, int np){
	for(int i=0 ; i<np ; i++){
		printf("Pipe %d: [%d,%d]\n", i, pipes[i][0], pipes[i][1]);
	}
}

void freePipes(int **pipes, int np){
	for(int i=0 ; i < np ; i++)
		free(pipes[i]);
	free(pipes);
}

void closePipeFds(int **pipes, int np, int callingProcess, int io[2]){
	//int isParent = (callingProcess == -5);
	int exceptionIn = callingProcess-1, exceptionOut=callingProcess;
	if(callingProcess == 0){
		exceptionIn = -2; //no exception, because first file not using a pipe for input
	} else { //this is not the first process, so we can close io[0] if it's not 0 or 1
		if(io[0] != STDIN_FILENO)
			close(io[0]);
	}
	if(callingProcess == np){
		exceptionOut = -2; //no exception, because last file not using a pipe for output
	} else { //this is not the last process, so we can close io[1] if it's not 0 or 1
		if(io[1] != STDOUT_FILENO)
			close(io[1]);
	}

	//if(np > 0) printf("For process %d, I'm closing all fds but pipes[%d][0] and pipes[%d][1]\n", callingProcess, exceptionIn, exceptionOut);
	for(int i=0 ; i < np ; i++){
		if( (i != exceptionIn) && (pipes[i][0] > 1) )
			close(pipes[i][0]);
		if( (i != exceptionOut) && (pipes[i][1] > 1) )
			close(pipes[i][1]);
	}
}

int executeCommands(char commands[10][20][256], int nc, int *rowLens, int io[2]){
	//printf("EXECUTING %d COMMANDS WITH %d PIPES\n", nc, nc-1); //nC = number of Commands

	FlexArray argArr;

	int **pipes = initializePipes(nc-1);
	if(pipes == NULL)
		return EXIT_FAILURE;
	//printPipes(pipes, nc-1);

	pid_t *pids = malloc(nc * sizeof(pid_t));
	assert(pids != NULL);
	int *status = malloc(nc * sizeof(int)), parentStatus = EXIT_SUCCESS;
	assert(status != NULL);

	for(int p=0 ; p<nc ; p++){
		pids[p] = fork();

		if(pids[p] < 0){
			printf("Error: failed to fork\n");
			exit(EXIT_FAILURE);
		}
		else if(pids[p] == 0){
			//close unused file descriptors
			closePipeFds(pipes, nc-1, p, io);

			//printf("I am child %d\n", p);

			// set input file, different for first process
			if(p == 0){
				if(dup2(io[0], STDIN_FILENO) < 0){
					printf("Error with dup2 io[0]\n");
					exit(EXIT_FAILURE);
				}
				if (io[0] > 1)
					close(io[0]);
			} else {
				if(dup2(pipes[p-1][0], STDIN_FILENO) < 0){
					printf("Error with dup2 pipes[%d][0]\n", p-1);
					exit(EXIT_FAILURE);
				}
				if (pipes[p-1][0] > 1)
					close(pipes[p-1][0]);
			}

			// set output file, different for last process
			if(p == nc-1){
				if(dup2(io[1], STDOUT_FILENO) < 0){
					printf("Error with dup2 io[1]\n");
					exit(EXIT_FAILURE);
				}
				if (io[1] > 1)
					close(io[1]);
			} else {
				if(dup2(pipes[p][1], STDOUT_FILENO) < 0){
					printf("Error with dup2 pipes[%d][1]\n", p);
					exit(EXIT_FAILURE);
				}
				if (pipes[p][1] > 1)
					close(pipes[p][1]);
			}

			argArr = staticToFlexArray(commands[p], rowLens[p]);

			if( execvp(argArr.arr[0], argArr.arr) == -1){
				printf("Error: command not found!\n");
				emptyFlexArray(&argArr);
				free(argArr.arr);
				exit(EXIT_FAILURE);			// The process exits anyway, but lets the parent know it was unsuccesful.
			}
			//printFlexArray(argArr);
			emptyFlexArray(&argArr);
			free(argArr.arr);

			exit(EXIT_SUCCESS);
		}
	}

	// Only the parent process can get here.
	closePipeFds(pipes, nc-1, -2, io); //-2 because there will be no exceptions in closing the pipes.
	for(int p=0 ; p<nc ; p++){
		waitpid(pids[p], &status[p], 0);
		//printf("child %d returned with %d==%d\n", p, status[p], WEXITSTATUS(status[p]));
		if(status[p] != EXIT_SUCCESS){
			parentStatus = EXIT_FAILURE;
		}
	}
	//printf("All children done\n");

	freePipes(pipes, nc-1);
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
