#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct FlexArray{
	char **arr;
	int len;
	int max_len;
} FlexArray;

FlexArray newFlexArray();

FlexArray staticToFlexArray(char matrixRow[20][256], int len);

void flex(FlexArray *fa);

void add(char *input, FlexArray *fa);

void printFlexArray(FlexArray fa);

void emptyFlexArray(FlexArray *fa);
