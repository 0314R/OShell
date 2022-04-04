
%{
#include "processManagement.h"
#include "colors.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

Pipeline pipeline;
int skip = false;
int io[2] = {0,1};

void exitWrapper(){
	//printf("exitWrapper\n");
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

%}

%union {int num; char *id; }
%token <id> IDENTIFIER QUOTED_STRING SCRIPT
%token OR AND SINGLEOR SINGLEAND EOL CD RED RESET YELLOW LIGHTBLUE LIGHTCYAN
GREEN BROWN BLUE PURPLE CYAN BLACK GREY WHITE LIGHTRED LIGHTGREY LIGHTGREEN LIGHTPURPLE
%type <id> executable options option fileName path
%type <num> chain

%%
inputline   : composition inputline		{ ; }
			| SCRIPT 		 			{ char cmd[100] = "bash ./";
										  strcat(cmd, $1);
										  system(cmd); 
										}
			  EOL inputline
			| RED 						{ red(); }
			  EOL inputline	
			| BROWN						{ brown(); }
			  EOL inputline	
			| YELLOW 					{ yellow(); }
			  EOL inputline	
			| GREEN 					{ green(); }
			  EOL inputline	
			| BLUE	 					{ blue(); }
			  EOL inputline	
			| PURPLE 					{ purple(); }
			  EOL inputline	
			| CYAN	 					{ cyan(); }
			  EOL inputline	
			| BLACK 					{ black(); }
			  EOL inputline	
			| GREY	 					{ grey(); }
			  EOL inputline	
			| WHITE 					{ white(); }
			  EOL inputline	
			| LIGHTRED 					{ lightred(); }
			  EOL inputline	
			| LIGHTGREY					{ lightgrey(); }
			  EOL inputline	
			| LIGHTGREEN				{ lightgreen(); }
			  EOL inputline	
			| LIGHTBLUE					{ lightblue(); }
			  EOL inputline	
			| LIGHTPURPLE				{ lightpurple(); }
			  EOL inputline	
			| LIGHTCYAN					{ lightcyan(); }
			  EOL inputline	
			| RESET 					{ reset(); }
			  EOL inputline
			|
            ;

composition : chain AND					{ if($1 != 0) skip = true; }
			| chain OR					{ if($1 == 0) skip = true; }
			| chain EOL					{ ; }
			| chain ';'					{ ; }
			;

chain       : pipeline redirections		{ if(skip == false) $$ = executeCommand( &(pipeline.argArrays[0]), io);
										  emptyFlexArray( &(pipeline.argArrays[0]) ) ; // Clean array of arguments for next command.
										  if($$ == EXIT_COMMAND) exitWrapper();

										  skip = false;
										  io[0] = 0;
										  io[1] = 1;
									    }
            ;

pipeline    : command					{ ; }
            ;

redirections : '<' fileName '>' fileName { openInput($2, io); openOutput($4, io);}
			 | '>' fileName '<' fileName { openOutput($2, io); openInput($4, io); }
			 | '<' fileName				 { openInput($2, io); }
			 | '>' fileName				 { openOutput($2, io); }
			 |							 { ; }
			 ;

command     : CD path		 			{ if ($2 == NULL) { printf("Error: cd requires folder to navigate to!\n"); } //{chdir(getenv("HOME"));} 
										  else { 
											if(chdir($2) != 0) {printf("Error: cd directory not found!\n"); } 
											else { free($2);} } }
            | executable options 		{ ; }
			;

path		: 							{ $$ = NULL; }
			| IDENTIFIER
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
