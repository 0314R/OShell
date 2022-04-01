#include "flexArray.h"

FlexArray newFlexArray(){
	FlexArray fa;
	int max_len = 1;

	fa.arr = malloc(max_len * sizeof(char *));
	assert(fa.arr != NULL);

	fa.len = 0;
	fa.max_len = max_len;

	return fa;
}

FlexArray staticToFlexArray(char matrixRow[20][256], int len){
	FlexArray fa = newFlexArray();

	for(int i=0 ; i<len ; i++){
		add(matrixRow[i], &fa);
	}
	add(NULL, &fa);

	return fa;
}

// flexing <=> doubling the array size.
void flex(FlexArray *fa){
	int new_len = 2 * fa->max_len;

	fa->arr = realloc(fa->arr, new_len * sizeof(char *));
	assert( fa->arr != NULL);
	fa->max_len = new_len;
}

void add(char *input, FlexArray *fa){
	if(fa->len == fa->max_len)
		flex(fa);			// to flex <=> to double size of array

	if(input != NULL){
		int in_len = strlen(input);
		fa->arr[fa->len] = malloc( (in_len+1) * sizeof(char));
		strcpy(fa->arr[fa->len], input);
	} else {
		fa->arr[fa->len] = NULL;
	}
	fa->len++;
}

void printFlexArray(FlexArray fa){
	int i;
	putchar('{');
	for(i=0 ; i<fa.len-1 ; i++){
		if(fa.arr[i] != NULL)
			printf("\"%s\", ", fa.arr[i]);
		else
			printf("NULL, ");
	}
	if(fa.arr[i] != NULL)
		printf("\"%s\"}", fa.arr[i]);
	else
		printf("NULL}");
	putchar('\n');
}

void emptyFlexArray(FlexArray *fa){
	for(int i=0 ; i < (fa->len) ; i++ ){
		free( fa->arr[i] );
	}
	fa->len = 0;
}
