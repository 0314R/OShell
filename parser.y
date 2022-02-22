
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
int yyerror(char* s);
%}

%token IDENTIFIER
%token EOL

%%

inputline   : chain AND inputline
            | chain SINGLEAND inputline
            | chain OR inputline
            | chain ';'
            | chain
            | 
            ;

chain       : pipeline redirections
            //| buitlin options 
            ; 

redirections: '<' IDENTIFIER '>' IDENTIFIER
            | '>' IDENTIFIER '<' IDENTIFIER
            | '>' IDENTIFIER
            | '<' IDENTIFIER
            |
            ;

pipeline    : command SINGLEOR pipeline
            | command
            ;

command     : IDENTIFIER options
            ;

options     : IDENTIFIER options
            | IDENTIFIER
            |
            ;


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
