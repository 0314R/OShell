
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

pid_t pid;
FlexArray args;

void exitWrapper(){
	//printFlexArray(args);
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

command: executable options {
	pid = fork();

	if(pid < 0){
		fprintf(stderr, "fork fail\n");
	} else if( pid > 0){
		wait(NULL);						// Parent waits until child terminates.
		//printf("<<child terminated>>\n");
		emptyFlexArray(&args);			// Clean array of arguments for next command.
		free($1);						// The executable needs to be freed too.
	} else {							// Child: actually execute the code.
		//printf("<<child created>>\n");
		add(NULL, &args);

		//printf("execvp(%s, ", $1);
		//printFlexArray(args);
		//printf(")\n");

		execvp($1, args.arr);
		exit(EXIT_SUCCESS);				// In case the execv doesn't get an executable.
	}
}
;

executable: IDENTIFIER		{ add($1, &args); }
| EXIT						{ exitWrapper(); }
;

options: EOL				{ ; }
| option options			{ ; }
;

option: IDENTIFIER			{ add($1, &args); free($1); }

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
