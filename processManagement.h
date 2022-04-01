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

pid_t pid, pid1, pid2;

void openInput(char *fileName, int *inAndOutput);
void openOutput(char *fileName, int *inAndOutput);

int cd(FlexArray *args);

int executeCommand(FlexArray *args, int inAndOutput[2]);
int executeCommands(Pipeline pl, int inAndOutput[2]);

char *removeQuotes(char *quotedInput);
