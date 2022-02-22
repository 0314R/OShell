#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct FlexArray{
	char **arr;
	int len;
	int max_len;
} FlexArray;

FlexArray newFlexArray(){
	FlexArray fa;
	int max_len = 1;

	fa.arr = malloc(max_len * sizeof(char *));
	assert(fa.arr != NULL);

	fa.len = 0;
	fa.max_len = max_len;

	return fa;
}

void flex(FlexArray *fa){
	int new_len = 2 * fa->max_len;
	//printf("Flexing from %d to %d\n", fa->max_len, new_len);

	fa->arr = realloc(fa->arr, new_len * sizeof(char *));
	assert( fa->arr != NULL);
	fa->max_len = new_len;
}

void add(char *input, FlexArray *fa){
	if(fa->len == fa->max_len)
		flex(fa);

	int in_len = strlen(input);
	fa->arr[fa->len] = malloc( (in_len+1) * sizeof(char));
	strcpy(fa->arr[fa->len], input);
	fa->len++;
}

void printFlexArray(FlexArray fa){
	for(int i=0 ; i<fa.len ; i++){
		printf("arg[%d]: %s\n", i, fa.arr[i]);
	}
}

void emptyFlexArray(FlexArray *fa){
	//printf("freeing array of size %d\n", fa->len);
	for(int i=0 ; i < (fa->len) ; i++ ){
		free( fa->arr[i] );
	}
	fa->len = 0;
}
