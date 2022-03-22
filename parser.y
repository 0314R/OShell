
%{
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

Pipeline pipeline;
int skip = false;
int inAndOutput[2] = {0,1};

void exitWrapper(){
	//printf("exitWrapper\n");
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER QUOTED_STRING
%token OR AND SINGLEOR SINGLEAND EOL
%type <id> executable options option fileName
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

chain       : pipeline redirections		{ if(skip == false) $$ = executeCommand( &(pipeline.argArrays[0]), inAndOutput);
										  emptyFlexArray( &(pipeline.argArrays[0]) ) ; // Clean array of arguments for next command.
										  if($$ == EXIT_COMMAND) exitWrapper();
										  skip = false;
									    }
            ;

pipeline    : command					{ ; }
            ;

redirections : '<' fileName				{ inAndOutput[0] = open($2, O_RDONLY); }
			 | '>' fileName				{ inAndOutput[1] = open($2, O_WRONLY | O_CREAT); }
			 |							{ ; }
			 ;

command     : executable options 		{ ; }
            ;

executable  : IDENTIFIER	        	{ add($1, &pipeline); free($1); }
			| QUOTED_STRING				{ $$ = removeQuotes($1); add($$, &pipeline); }
            ;

options     :           				{ ; }
            | option options			{ ; }
            ;

option      : IDENTIFIER				{ add($1, &pipeline); free($1); }
			| QUOTED_STRING				{ $1 = removeQuotes($1); add($1, &pipeline); free($1);}

fileName    : IDENTIFIER				{ $$ = $1; }
			| QUOTED_STRING				{ $1 = removeQuotes($1); $$ = $1; }

%%

int main(int argc, char **argv)
{
	pipeline = newPipeline();
	newCommandEntry(&pipeline);

    yyparse();

    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);

	return 0;
}
