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

#include "flexArray.h"

#define EXIT_COMMAND 2

pid_t pid;

void openInput(char *fileName, int *inAndOutput);
void openOutput(char *fileName, int *inAndOutput);
void resetIo(int *io);

int cd(char command[20][256], int len);

int executePipeline(char commands[10][20][256], int r, int *rowLens, int io[2]);
void executeBackgroundPipeline(char commands[10][20][256], int r, int *rowLens, int io[2]);
void executeBackgroundCommand(char command[20][256], int len, int io[2]);

char *removeQuotes(char *quotedInput);
