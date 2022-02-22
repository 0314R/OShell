
%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "flexArray.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

FlexArray args;

void exitWrapper(){
	printFlexArray(args);
	emptyFlexArray(&args);
	free(args.arr);
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER
%token EOL
%token EXIT
%type <id> command executable options option

%%

commands:
| commands command			{ ; }
;

command: executable options { add(NULL, &args); printf("execvp(%s, ", $1);
							  printFlexArray(args); printf(")\n");
							  /*execvp($1, $1, $2, NULL); */free($1); }
;

executable: IDENTIFIER		{ add($1, &args); }
| EXIT						{ printf("EXIT\n"); exitWrapper(); }
;

options: EOL				{ ; }
| option options			{ ; }
;

option: IDENTIFIER			{ add($1, &args); printf("OP1 %s\n", $1); free($1); }

%%

int main(int argc, char **argv)
{
	args = newFlexArray();

    yyparse();

	printFlexArray(args);

    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);

	return 0;
}
