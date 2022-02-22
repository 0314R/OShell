%option noyywrap

%{
#include "parser.tab.h"
%}

%%

"||"                { return OR; }
"&&"                { return AND; }
"|"                 { return PIPELINE; }
"&"                 { return SINGLEAND; }
";"                 { yylval.end = return (*yytext); }

"exit"				{ return EXIT; }
[^ \t\n|&;<>]+           { yylval.id = yytext; return IDENTIFIER; }
\n					{ return EOL; }
[ \t]            	{  }

%%
/* code for testing
int main(int argc, char **argv)
{
	int toknr;

	while(toknr = yylex()){
		printf("%d\n", toknr);
	}

    return 0;
}
*/
