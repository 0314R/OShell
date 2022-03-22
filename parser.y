
%{
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

FlexArray args;
Pipeline pipeline;
int skip = false;

void exitWrapper(){
	free(args.arr);
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER QUOTED_STRING
%token OR AND SINGLEOR SINGLEAND EOL
%type <id> executable options option fileName
%type <num> command pipeline chain

%%
inputline   : composition inputline		{ ; }
			|
            ;

composition : chain AND					{ if($1 != 0) skip = true; }
			| chain OR					{ if($1 == 0) skip = true; }
			| chain EOL					{ ; }
			| chain ';'					{ ; }
			;

chain       : pipeline redirections		{ $$ = $1; skip = false; }
            ;

pipeline    : command					{ $$ = $1; }
            ;

redirections : '<' fileName				{ printf("input: %s\n", $2); }
			 |							{ printf("no input"); }
			 ;

command     : executable options 		{ if(skip == false) $$ = executeCommand($1, &args);
										  emptyFlexArray(&args);		// Clean array of arguments for next command.
										  if($$ == EXIT_COMMAND) exitWrapper();			}
            ;

executable  : IDENTIFIER	        	{ add($1, &args); }
			| QUOTED_STRING				{ $$ = removeQuotes($1); add($$, &args); }
            ;

options     :           				{ ; }
            | option options			{ ; }
            ;

option      : IDENTIFIER				{ add($1, &args); free($1); }
			| QUOTED_STRING				{ $1 = removeQuotes($1); add($1, &args); free($1);}

fileName    : IDENTIFIER				{ $$ = $1; }
			| QUOTED_STRING				{ $1 = removeQuotes($1); $$ = $1; }

%%

int main(int argc, char **argv)
{
	args = newFlexArray();
	pipeline = newPipeline();

    yyparse();

    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);

	return 0;
}
