#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "flexArray.h"

pid_t pid;

void executeCommand(char *executable, FlexArray args);
