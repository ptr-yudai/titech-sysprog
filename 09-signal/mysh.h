#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>

/* === logging === */

#define LOG(...) l_logf(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define DIE(...) l_die(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define PERROR_DIE(cxt) l_pdie(__FILE__, __LINE__, __func__, (cxt))

int l_get_quiet(void);
void l_set_quiet(int quiet);
int l_logf(const char *file, int line, const char *func, const char *fmt, ...);
int l_die(const char *file, int line, const char *func, const char *fmt, ...);
int l_pdie(const char *file, int line, const char *func, const char *cxt);

/* === data structure === */

/* node */
typedef enum type {
    N_COMMAND,         /* foo bar baz */
    N_SEQUENCE,        /* foo ; bar */
    N_AND,             /* foo && bar */
    N_OR,              /* foo || bar */
    N_PIPE,            /* foo | bar */
    N_REDIRECT_IN,     /* foo < bar */
    N_REDIRECT_OUT,    /* foo > bar */
    N_REDIRECT_ERR,    /* foo 2> bar */
    N_REDIRECT_APPEND, /* foo >> bar */
    N_SUBSHELL,        /* ( foo... ) */
} type_t;

typedef struct node {
    type_t type;      /* one of enum node_type */
    struct node *lhs; /* two children at most */
    struct node *rhs; /* two children at most */
    char **argv;      /* command */
    char *filename;   /* filename */
    int async;        /* async mode */
} node_t;

node_t *make_node(type_t type, node_t *lhs, node_t *rhs, char **argv,
                  char *filename);
void free_node(node_t *node);
char *inspect_node(node_t *node);
char **node_argv(node_t *node);
void dump_node(node_t *node, FILE *out);
char *inspect_tokens(char **tokens);

/* alist */
typedef struct alist {
    char *arg;
    struct alist *next;
} alist_t;

alist_t *make_alist(char *arg, alist_t *alist);
void free_alist(alist_t *alist);
int len_alist(alist_t *alist);
char **convert_to_argv(alist_t *alist);

int yylex(void);
int yyparse(void);
void lex_set_source(char *source);
node_t *yacc_parse(char *source);

int invoke_node(node_t *node);
void init_shell(void);

#endif
