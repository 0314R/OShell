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

void flex(FlexArray *fa){
	int new_len = 2 * fa->max_len;
	//printf("Flexing from %d to %d\n", fa->max_len, new_len);

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

	// Now print the last string, which should be a NULL but if/else just in case.
	if(fa.arr[i] != NULL)
		printf("\"%s\"}", fa.arr[i]);
	else
		printf("NULL}");
}

void emptyFlexArray(FlexArray *fa){
	printf("freeing array of size %d\n", fa->len);
	for(int i=0 ; i < (fa->len) ; i++ ){
		free( fa->arr[i] );
	}
	fa->len = 0;
}
