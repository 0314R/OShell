#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "flexArray.h"

//for storing background pipeline pids, 10 pids per pipeline (not continuous to avoid conflicts between different background pipelines.)
//it has 10 spots corresponding to 10 background pipelines, but in the parser code you see it loops over the 10 spots (FIFO basically).
pid_t bgPlPids[10][10];

void installHandlers();

void openInput(char *fileName, int *inAndOutput);
void openOutput(char *fileName, int *inAndOutput);
void resetIo(int *io);

int cd(char command[20][256], int len);

void exitWrapper();

int executePipeline(char commands[10][20][256], int r, int *rowLens, int io[2]);
void executeBackground(char commands[10][20][256], int nc, int *rowLens, int io[2], int bgPlId);
char *removeQuotes(char *quotedInput);
