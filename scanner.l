%option noyywrap

%{
#include "parser.tab.h"
%}

%%

[^ \t\n]+           { return IDENTIFIER; }
\n					{ return EOL; }
[ \t]            	{  }

%%
/*
int main(int argc, char **argv)
{
	int toknr;

	while(toknr = yylex()){
		printf("%d\n", toknr);
	}

    return 0;
}
*/
