
%{
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

char pl[10][20][256]; //this static array will contain all the args of all commands of the current pipeline.
int rowLens[10];	  //this goes together with the above: says how long each row is, i.e. how many args per command.
int r, c, nOfBgPls = 0; //rows, columns of the current pipeline ; number of background pipelines so far.
int skip = false;
int bg = false;
int io[2] = {0,1};

void resetPipeline(){
	for(int i=0 ; i<10 ; i++){
		rowLens[i] = 0;
	}
	r = c = 0;
}

void printPipeline(){
	printf("PIPELINE:\n");
	for(int i=0 ; i<r ; i++){
		printf("[%d] ", rowLens[i]);
		for(int j=0 ; j<rowLens[i] ; j++){
			printf("%s ", pl[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}

void exitWrapper(){
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

int executeChain(char *value){
	int status;

	if(skip == false){
		if(strcmp(value, "exit") == 0){
		  free(value);
		  exitWrapper();
		}
		if(strcmp(value, "cd") == 0)
			status = cd(pl[0], rowLens[0]);
		else{
			if(bg){
				executeBackground(pl, r, rowLens, io, nOfBgPls);
				nOfBgPls++;
				//executeBackgroundCommand(pl[0], rowLens[0], io);
				status = EXIT_SUCCESS;
			}
			else{
				status = executePipeline(pl, r, rowLens, io);
			}
		}

	}
	free(value);
	resetPipeline();

	skip = false;
	bg = false;
	resetIo(io);

	return status;
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER QUOTED_STRING
%token OR AND SINGLEOR SINGLEAND EOL
%type <id> executable options option fileName pipeline chain
%type <num> composition

%%
inputline   : composition inputline		{ ; }
			|
            ;

composition : chain AND					{ if(executeChain($1) != 0) skip = true; }
			| chain OR					{ if(executeChain($1) == 0) skip = true; }
			| chain SINGLEAND			{ bg = true; $$ = executeChain($1); }
			| chain SINGLEAND EOL		{ bg = true; $$ = executeChain($1); }
			| chain EOL					{ $$ = executeChain($1); }
			| chain ';'					{ $$ = executeChain($1); }
			;

chain       : pipeline redirections		{ ; }
            ;

pipeline 	: pipedCommand pipeline		{ free($2); }
			| command					{ rowLens[r] = c; r++; c=0; /*printf("finished reading command, [%d,%d, %d]\n", r, c, rowLens[r-1]);*/ }
            ;

pipedCommand : command SINGLEOR			 { rowLens[r] = c; r++;  c = 0; /*printf("finished reading command, [%d,%d, %d]\n", r, c, rowLens[r-1]);*/ }
			 ;

redirections : '<' fileName '>' fileName { openInput($2, io); openOutput($4, io); }
			 | '>' fileName '<' fileName { openOutput($2, io); openInput($4, io); }
			 | '<' fileName				 { openInput($2, io); }
			 | '>' fileName				 { openOutput($2, io); }
			 | '<' '>' fileName			 { printf("Error: invalid syntax!\n"); free($3); skip = true; }
			 | '>' '<' fileName			 { printf("Error: invalid syntax!\n"); free($3); skip = true; }
			 | '<' fileName '>'			 { printf("Error: invalid syntax!\n"); free($2); skip = true; }
			 | '>' fileName '<'			 { printf("Error: invalid syntax!\n"); free($2); skip = true; }
			 | '<' '>'					 { printf("Error: invalid syntax!\n"); skip = true; }
			 | '>' '<'					 { printf("Error: invalid syntax!\n"); skip = true; }
			 | '<'						 { printf("Error: invalid syntax!\n"); skip = true; }
			 | '>'						 { printf("Error: invalid syntax!\n"); skip = true; }
			 |							 { ; }
			 ;

command     : executable options 		{ ; }
            ;

executable  : IDENTIFIER	        	{ strcpy(pl[r][c], $1); c++; }
			| QUOTED_STRING				{ $$ = removeQuotes($1); strcpy(pl[r][c], $$); c++; }
            ;

options     :           				{ ; }
            | option options			{ ; }
            ;

option      : IDENTIFIER				{ strcpy(pl[r][c], $1); c++; free($1); }
			| QUOTED_STRING				{ $1 = removeQuotes($1); strcpy(pl[r][c], $1); c++; free($1);}
			;
fileName    : IDENTIFIER				{ $$ = $1; }
			| QUOTED_STRING				{ $1 = removeQuotes($1); $$ = $1; }
			;
%%

int main(int argc, char **argv)
{
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

    yyparse();

    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);

	return 0;
}
