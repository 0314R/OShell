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

"cd"				{ return CD; }
"red"				{ return RED; }
"brown"				{ return BROWN; }
"yellow"			{ return YELLOW; }
"green"				{ return GREEN; }
"blue"				{ return BLUE; }
"purple"			{ return PURPLE; }
"cyan"				{ return CYAN; }
"black"				{ return BLACK; }
"grey"				{ return GREY; }
"white"				{ return WHITE; }
"lightgrey"			{ return LIGHTGREY; }
"lightred"			{ return LIGHTRED; }
"lightgreen"		{ return LIGHTGREEN; }
"lightblue"			{ return LIGHTBLUE; }
"lightpurple"		{ return LIGHTPURPLE; }
"lightcyan"			{ return LIGHTCYAN; }
"reset"				{ return RESET; }

[^ \t\n;<>]+".sh"	{ yylval.id = strdup(yytext); return SCRIPT; }
\"[^"]*\"			{ yylval.id = strdup(yytext); return QUOTED_STRING; }
[^ \t\n;<>]+        { yylval.id = strdup(yytext); return IDENTIFIER; }
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
