%{
#include "mysh.h"
#include "y.tab.h"
#define RETURN(t) LOG("New token: %s {%s}", #t, yytext); return (t);
%}

%option noinput
%option nounput
%option noyywrap

%%
[ \t\r]+ ;
"&&" { RETURN(tANDAND); }
"||" { RETURN(tOROR); }
">>" { RETURN(tGTGT); }
";"  { RETURN(tSCOLON); }
"|"  { RETURN(tOR); }
"&"  { RETURN(tAND); }
"("  { RETURN(tLPAREN); }
")"  { RETURN(tRPAREN); }
"<"  { RETURN(tLT); }
">"  { RETURN(tGT); }
"2>"  { RETURN(tGT_ERR); }
'[^']*' { yylval.string = yytext; RETURN(tQSTRING); }
\"[^\"]*\" { yylval.string = yytext; RETURN(tQQSTRING); }
[^ \t\r\n&|;()<>]+ { yylval.string = yytext; RETURN(tBARE); }
\n   { return 0; }
<<EOF>> { return 0; }

%%

void lex_set_source(char *source) {
    yy_scan_string(source);
}

int lex_main(int argc, char** argv) {
    yy_scan_string(argv[1]);
    int t;
    while ((t = yylex()) != 0) { printf("[%s]\n", yytext); }
    return 0;
}
