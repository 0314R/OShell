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
\"[^"]*\"			{ yylval.id = strdup(yytext); return QUOTED_STRING; }
[^ \t\n;<>]+        { yylval.id = strdup(yytext); return IDENTIFIER; }
\n					{ return EOL; }
[ \t]            	{  }

%%
