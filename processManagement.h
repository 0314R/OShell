#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "flexArray.h"

#define EXIT_COMMAND 2

pid_t pid;

int executeCommand(char *executable, FlexArray *args);

char *removeQuotes(char *quotedInput);
