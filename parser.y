
%{
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

Pipeline pipeline;
char pl[10][20][256];
int rowLens[10];
int r, c;
int skip = false;
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
	//printf("exitWrapper\n");
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER QUOTED_STRING
%token OR AND SINGLEOR SINGLEAND EOL
%type <id> executable options option fileName pipeline
%type <num> chain

%%
inputline   : composition inputline		{ ; }
			|
            ;

composition : chain AND					{ if($1 != 0) skip = true; }
			| chain OR					{ if($1 == 0) skip = true; }
			| chain EOL					{ ; }
			| chain ';'					{ ; }
			;

chain       : pipeline redirections		{ /*if(strcmp($1, "cd") == 0)
											$$ = cd( &(pipeline.argArrays[0]));
										  else *if(skip == false)
											$$ = executeCommand( &(pipeline.argArrays[0]), io);
										  free($1);*/
										  $$ = executeCommands(pl, r, rowLens, io);
										  printf("PARENT RETURNED WITH STATUS %d\n", $$);
										  printPipeline();
										  resetPipeline();

										  resetIo(io);

										  /*
										  emptyFlexArray( &(pipeline.argArrays[0]) ) ; // Clean array of arguments for next command.
										  if($$ == EXIT_COMMAND) exitWrapper();

										  skip = false;
										  io[0] = 0;
										  io[1] = 1;
										  */
									    }
            ;

pipeline 	: pipedCommand pipeline		{ ; }
			| command					{ rowLens[r] = c; r++; c=0; /*printf("finished reading command, [%d,%d, %d]\n", r, c, rowLens[r-1]);*/ }
            ;

pipedCommand : command SINGLEOR			 { rowLens[r] = c; r++;  c = 0; /*printf("finished reading command, [%d,%d, %d]\n", r, c, rowLens[r-1]);*/ }
			 ;

redirections : '<' fileName '>' fileName { openInput($2, io); openOutput($4, io);}
			 | '>' fileName '<' fileName { openOutput($2, io); openInput($4, io); }
			 | '<' fileName				 { openInput($2, io); }
			 | '>' fileName				 { openOutput($2, io); }
			 |							 { ; }
			 ;

command     : executable options 		{ printf("COM\n"); }
            ;

executable  : IDENTIFIER	        	{ printf("EXE\n"); strcpy(pl[r][c], $1); c++; free($1);/*add($1, &pipeline);*/ }
| QUOTED_STRING							{ printf("EXE\n"); $$ = removeQuotes($1); strcpy(pl[r][c], $$); c++; free($$); /*add($$, &pipeline);*/ }
            ;

options     :           				{ ; }
            | option options			{ ; }
            ;

option      : IDENTIFIER				{ printf("OPT\n"); strcpy(pl[r][c], $1); c++; /*add($1, &pipeline); */free($1); }
			| QUOTED_STRING				{ printf("OPT\n"); $1 = removeQuotes($1); strcpy(pl[r][c], $1); c++; /*add($1, &pipeline);*/ free($1);}

fileName    : IDENTIFIER				{ $$ = $1; }
			| QUOTED_STRING				{ $1 = removeQuotes($1); $$ = $1; }

%%

int main(int argc, char **argv)
{
	//pipeline = newPipeline();
	//newCommandEntry(&pipeline);

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
