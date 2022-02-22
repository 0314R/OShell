%option noyywrap

%{
#include "parser.tab.h"
%}

%%

"||"                { return OR; }
"&&"                { return AND; }
"|"                 { return SINGLEOR; }
"&"                 { return SINGLEAND; }
";"                 { yylval.end = return (*yytext); }
"<"					{ return (*yytext); }
">"					{ return (*yytext); }

"exit"				{ return EXIT; }
[^ \t\n|&;<>]+      { yylval.id = yytext; return IDENTIFIER; }
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
