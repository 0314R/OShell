#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct FlexArray{
	char **arr;
	int len;
	int max_len;
} FlexArray;

typedef struct Pipeline{
	FlexArray *argArrays;
	int len;
} Pipeline;

FlexArray newFlexArray();

FlexArray staticToFlexArray(char matrixRow[20][256], int len);

Pipeline newPipeline();

void newCommandEntry(Pipeline *pl);

void flex(FlexArray *fa);

void addToPipeline(char *input, Pipeline *pl);
void add(char *input, FlexArray *fa);

void printFlexArray(FlexArray fa);

void emptyFlexArray(FlexArray *fa);

void emptyPipeline(Pipeline pl);
