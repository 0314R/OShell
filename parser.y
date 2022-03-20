
%{
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

FlexArray args;
int skip = false;

void exitWrapper(){
	free(args.arr);
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER
%token OR AND SINGLEOR SINGLEAND EOL
%token EXIT
%type <id> executable options option
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

chain       : pipeline					{ $$ = $1; skip = false; }
            ;

pipeline    : command					{ $$ = $1; }
            ;

command     : executable options 		{ if(skip == false) $$ = executeCommand($1, &args); }
            ;

executable  : IDENTIFIER	        	{ add($1, &args); }
            | EXIT						{ exitWrapper(); }
            ;

options     :           				{ ; }
            | option options			{ ; }
            ;

option      : IDENTIFIER				{ add($1, &args); free($1); }

%%

int main(int argc, char **argv)
{
	args = newFlexArray();

    yyparse();

    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);

	return 0;
}
