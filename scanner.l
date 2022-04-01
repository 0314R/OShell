%option noyywrap
%option always-interactive

%{
#include "parser.tab.h"
%}

%%

"||"                { return OR; }
"&&"                { return AND; }
"|"                 { return SINGLEOR; }
"&"                 { return SINGLEAND; }
";"                 { return (*yytext); }
"<"					{ return (*yytext); }
">"					{ return (*yytext); }
\"[^"]*\"			{ yylval.id = strdup(yytext); printf("Allo QS %s\n", yytext); return QUOTED_STRING; }
[^ \t\n;<>]+        { yylval.id = strdup(yytext); printf("Allo ID %s\n", yytext); return IDENTIFIER; }
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
