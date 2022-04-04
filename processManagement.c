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

int cd(char command[20][256], int len){
	//cd expects exactly one option/argument, namely the path.
	switch( len - 1){		// The first "argument" in the array is the cd command itself. Therefore -1.
		case 0: printf("Error: cd requires folder to navigate to!\n"); return 0;
		case 1: break;
		default: printf("Error: cd directory not found!\n"); return 0;
	}

	FlexArray args = staticToFlexArray(command, len);

	char *path = args.arr[1];
	int status = chdir(path);
	if(status != 0)
		printf("Error: cd directory not found!\n");

	emptyFlexArray(&args);
	free(args.arr);
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

// This closes all fds that a process has (inherited), except the ones relevant to itself.
void closePipeFds(int **pipes, int np, int callingProcess, int io[2]){
	int isParent = (callingProcess == -5);
	int exceptionIn = callingProcess-1, exceptionOut=callingProcess;
	if(callingProcess == 0){
		exceptionIn = -2; //no exception, because first file not using a pipe for input
	} else if(!isParent){ //this is not the first process, so we can close io[0] if it's not 0 or 1
		close(io[0]);
	}
	if(callingProcess == np){
		exceptionOut = -2; //no exception, because last file not using a pipe for output
	} else if(!isParent){ //this is not the last process, so we can close io[1] if it's not 0 or 1
		close(io[1]);
	}

	// Close all fds except for the exceptions relevant to the calling Process.
	for(int i=0 ; i < np ; i++){
		if( (i != exceptionIn) )
			close(pipes[i][0]);
		if( (i != exceptionOut) )
			close(pipes[i][1]);
	}
}

int executePipeline(char commands[10][20][256], int nc, int *rowLens, int io[2]){
	FlexArray argArr;

	int **pipes = initializePipes(nc-1);
	if(pipes == NULL)
		return EXIT_FAILURE;

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

			// set input file, different for first process
			if(p == 0){
				if(dup2(io[0], STDIN_FILENO) < 0){
					printf("Error with dup2 io[0]\n");
					exit(EXIT_FAILURE);
				}
				if (io[0] != STDIN_FILENO)
					close(io[0]);
			} else {
				if(dup2(pipes[p-1][0], STDIN_FILENO) < 0){
					printf("Error with dup2 pipes[%d][0]\n", p-1);
					exit(EXIT_FAILURE);
				}
				if (pipes[p-1][0] != STDIN_FILENO)
					close(pipes[p-1][0]);
			}

			// set output file, different for last process
			if(p == nc-1){
				if(dup2(io[1], STDOUT_FILENO) < 0){
					printf("Error with dup2 io[1]\n");
					exit(EXIT_FAILURE);
				}
				if (io[1] != STDOUT_FILENO)
					close(io[1]);
			} else {
				if(dup2(pipes[p][1], STDOUT_FILENO) < 0){
					printf("Error with dup2 pipes[%d][1]\n", p);
					exit(EXIT_FAILURE);
				}
				if (pipes[p][1] != STDOUT_FILENO)
					close(pipes[p][1]);
			}

			// Obtain arguments relevant for this process.
			argArr = staticToFlexArray(commands[p], rowLens[p]);
			// Execute the command corresponding to this process.
			if( execvp(argArr.arr[0], argArr.arr) == -1){
				printf("Error: command not found!\n");
				emptyFlexArray(&argArr);
				free(argArr.arr);
				exit(EXIT_FAILURE);			// The process exits anyway, but lets the parent know it was unsuccesful.
			}
			emptyFlexArray(&argArr);
			free(argArr.arr);

			exit(EXIT_SUCCESS);
		}
	}


	// Only the parent process can get here.
	closePipeFds(pipes, nc-1, -5, io); //-5 as fake index so that there will be no exceptions in closing fds, unlike children based on their actual index.
	for(int p=0 ; p<nc ; p++){
		waitpid(pids[p], &status[p], 0);
		if(status[p] != EXIT_SUCCESS){
			parentStatus = EXIT_FAILURE;
		}
	}

	freePipes(pipes, nc-1);
	free(pids);
	free(status);

	return parentStatus;
}

void executeBackgroundPipeline(char commands[10][20][256], int nc, int *rowLens, int io[2]){
	printf("executing %s(%s) in the bg\n", commands[0][0], commands[0][1]);
	FlexArray argArr;

	pid_t *pids = malloc(nc * sizeof(pid_t));
	assert(pids != NULL);

	for(int p=0 ; p<nc ; p++){
		pids[p] = fork();

		if(pids[p] < 0){
			printf("Error: failed to fork\n");
			exit(EXIT_FAILURE);
		}
		else if(pids[p] == 0){
			// for the first process: set input file if it is not the inherited STDIN. Else close STDIN.
			if( p==0 && (io[0] != STDIN_FILENO) ){

				if( dup2(io[0], STDIN_FILENO) < 0 ){
					printf("Error with dup2 pipes[%d][0]\n", p-1);
					exit(EXIT_FAILURE);
				}
				close(io[0]);
			} else {
				close(io[0]);
				close(STDIN_FILENO);
			}

			// for the last process: set the output file if it is not the inherited STDOUT.
			if( io[1] != STDOUT_FILENO ){
				if( (p==nc-1) && dup2(io[1], STDOUT_FILENO) < 0 ){
					printf("Error with dup2 pipes[%d][1]\n", p);
					exit(EXIT_FAILURE);
				}
				printf("my STDOUT before\n");
				close(io[1]);
				printf("my STDOUT after\n");
			}

			// Obtain arguments relevant for this process.
			argArr = staticToFlexArray(commands[p], rowLens[p]);
			// Execute the command corresponding to this process.
			if( execvp(argArr.arr[0], argArr.arr) == -1){
				printf("Error: command not found!\n");
				emptyFlexArray(&argArr);
				free(argArr.arr);
				exit(EXIT_FAILURE);			// The process exits anyway, but lets the parent know it was unsuccesful.
			}

			//I don't think this code is ever actually executed...
			//...
			//...
			emptyFlexArray(&argArr);
			free(argArr.arr);

			exit(EXIT_SUCCESS);
		}
	}


	// Only the parent process can get here.
	free(pids);
}

char *removeQuotes(char *quotedInput){
	char *output;
	int newLen, oldLen = strlen(quotedInput);

	newLen = oldLen - 2; 							//As per the two removed quotes.
	output = malloc( (newLen+1) * sizeof(char));  	// +1 for the null character
	memmove(output, quotedInput+1, newLen);			// +1 to ignore the first character (quote)
	output[newLen] = '\0';							// ensure the new string has the right length.
	free(quotedInput);

	return output;
}
