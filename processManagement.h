#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "flexArray.h"

pid_t pid;

int executeCommand(char *executable, FlexArray *args);
