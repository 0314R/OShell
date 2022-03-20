
%{
#include <stdio.h>
#include <stdlib.h>
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

FlexArray args;

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
%type <id> command executable options option

%%

inputline   : chain EOL inputline
            |
            ;

chain       : pipeline
            ;

pipeline    : command
            ;

command     : executable options 		{ executeCommand($1, &args); }
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
