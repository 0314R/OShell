
%{
#include "processManagement.h"

int yylex(void);
int yyerror(char* s);
int yylex_destroy();

Pipeline pipeline;
char commandLine[10][20][256];
int r = 0, c = 0; //row, column
int rowLen[10];
int skip = false;
int io[2] = {0,1};

void exitWrapper(){
	//printf("exitWrapper\n");
	yylex_destroy();
	exit(EXIT_SUCCESS);
}

void printCommandLine(){
	int i, j, len;
	for(i=0 ; i<r ; i++){
		for(j=0 ; j<rowLen[i] ; j++){
			len = (int)strlen(commandLine[i][j]);
			printf("[%d,%d,%d](%s) ", i, j, len, commandLine[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}

void resetCommandLine(){
	for(int i=0 ; i<r ; i++)
		rowLen[i] = 0;
	r = c = 0;
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

chain       : pipeline redirections		{ if(strcmp($1, "cd") == 0){
											printf("CASE CD\n");
											$$ = cd( &(pipeline.argArrays[0]));
										  }
										  else if(skip == false)
										  {
											//$$ = executeCommand( &(pipeline.argArrays[0]), io);
											printf("CASE COMMAND\n");
											$$ = executeCommands( pipeline, io);
										  } else { //skip == true
											$$ = 0;	//make sure $$ is initialized
										  }
										  free($1);

										  printCommandLine();
										  resetCommandLine();
										  printCommandLine();


										  //emptyFlexArray( &(pipeline.argArrays[0]) ) ; // Clean array of arguments for next command.
										  emptyPipeline(pipeline);
										  /* for(int i=0 ; i<pipeline.len ; i++){
											  free(pipeline.argArrays[i]);
										  } */
										  pipeline.len = 1;
										  //newCommandEntry(&pipeline);
										  /* $$ = 100; */
										  if($$ == EXIT_COMMAND) exitWrapper();

										  skip = false;
										  io[0] = 0;
										  io[1] = 1;
									    }
            ;

pipeline    : pipedCommand pipeline		 { /*printf("PIPELINE\n")*/; }
			| command  					 { ; }
            ;

pipedCommand : command SINGLEOR			 { /*printf("PIPED COMMAND\n");*/ rowLen[r]=c; r++; c=0; newCommandEntry(&pipeline); }
			 ;

redirections : '<' fileName '>' fileName { openInput($2, io); openOutput($4, io);}
			 | '>' fileName '<' fileName { openOutput($2, io); openInput($4, io); }
			 | '<' fileName				 { openInput($2, io); }
			 | '>' fileName				 { openOutput($2, io); }
			 |							 { ; }
			 ;

command     : executable options 		{ /*printf("COMMAND\n") */;  }
            ;

executable  : IDENTIFIER	        	{ /*/printf("EXECUTABLE\n"); */ strcpy(commandLine[r][c], $1); c++; add($1, &pipeline); }
			| QUOTED_STRING				{ $$ = removeQuotes($1); strcpy(commandLine[r][c], $$); c++; add($$, &pipeline); }
            ;

options     :           				{ ; }
            | option options			{ ; }
            ;

option      : IDENTIFIER				{ strcpy(commandLine[r][c], $1); c++; add($1, &pipeline); free($1); }
			| QUOTED_STRING				{ $1 = removeQuotes($1); strcpy(commandLine[r][c], $1); c++; add($1, &pipeline); free($1);}

fileName    : IDENTIFIER				{ $$ = $1; }
			| QUOTED_STRING				{ $1 = removeQuotes($1); $$ = $1; }

%%

int main(int argc, char **argv)
{
	pipeline = newPipeline();
	newCommandEntry(&pipeline);

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
