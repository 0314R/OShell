
%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int yylex(void);
int yyerror(char* s);

//char *executable;
//char *first_option;
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

command: executable options { printf("executable: %soptions %s", $1, $2); }
;

executable: IDENTIFIER		{ $$ = $1; printf("EX %s\n", $$); }
| EXIT						{ printf("EXIT\n"); exit(EXIT_SUCCESS); }
;

options: EOL				{ $$ = ""; }
| option options			{ $$ = $1; }
;

option: IDENTIFIER			{ $$ = $1; printf("OP1 %s\n", $$); }

%%

int main(int argc, char **argv)
{
    yyparse();

    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);

	return 0;
}
