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
void flex(FlexArray *fa);
void add(char *input, FlexArray *fa);
void printFlexArray(FlexArray fa);
void emptyFlexArray(FlexArray *fa);
