%{
#include "mysh.h"
#include <stdio.h>
#include <string.h>
#define YYERROR_VERBOSE 1
node_t *root = NULL;

int yyerror(char *msg) {
    fprintf(stderr, "Syntax error: %s\n", msg);
    root = NULL;
    return 1;
}
%}

%union {
    node_t *node;
    alist_t *alist;
    char *string;
}

%start root

%type  <node>   root exp logical_exp piped_exp redirect_exp cmd_exp
%type  <alist>  strs
%type  <string> str

%token <int> tANDAND tOROR tGTGT tSCOLON tAND tOR tLPAREN tRPAREN tLT tGT tGT_ERR
%token <string> tBARE tQSTRING tQQSTRING

%%
root
    :     { root = NULL; }
    | exp { root = $1; }
    ;

exp /* foo ; bar */
    : logical_exp tSCOLON exp
      { $$ = make_node(N_SEQUENCE, $1, $3, NULL, NULL); }
    | logical_exp
    ;

logical_exp /* foo && bar, foo |\ bar */
    : piped_exp tANDAND logical_exp
      { $$ = make_node(N_AND, $1, $3, NULL, NULL); }
    | piped_exp tOROR logical_exp
      { $$ = make_node(N_OR, $1, $3, NULL, NULL); }
    | piped_exp
    ;


piped_exp /* foo | bar */
    : redirect_exp tOR piped_exp
      { $$ = make_node(N_PIPE, $1, $3, NULL, NULL); }
    | redirect_exp
    ;

redirect_exp /* foo < bar > baz */
    : redirect_exp tLT str
      { $$ = make_node(N_REDIRECT_IN, $1, NULL, NULL, $3); }
    | redirect_exp tGT str
      { $$ = make_node(N_REDIRECT_OUT, $1, NULL, NULL, $3); }
    | redirect_exp tGT_ERR str
      { $$ = make_node(N_REDIRECT_ERR, $1, NULL, NULL, $3); }
    | redirect_exp tGTGT str
      { $$ = make_node(N_REDIRECT_APPEND, $1, NULL, NULL, $3); }
    | cmd_exp
    ;

cmd_exp /* foo bar baz or ( exp ) */
    : tLPAREN exp tRPAREN
      { $$ = make_node(N_SUBSHELL, $2, NULL, NULL, NULL); }
    | strs
      { $$ = make_node(N_COMMAND, NULL, NULL, convert_to_argv($1), NULL); }
    ;

strs: str strs { $$ = make_alist($1, $2); }
    | str      { $$ = make_alist($1, NULL); }
    ;

str : tBARE     { $$ = strdup($1); }
    | tQSTRING  { $1[strlen($1) - 1] = '\0'; $$ = strdup($1 + 1); }
    | tQQSTRING { $1[strlen($1) - 1] = '\0'; $$ = strdup($1 + 1); }

%%

node_t *yacc_parse(char *source) {
    lex_set_source(source);
    yyparse();
    return root;
}
