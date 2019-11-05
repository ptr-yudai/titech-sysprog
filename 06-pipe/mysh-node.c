#include "mysh.h"
#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* malloc */

/* create a node object. */
node_t *make_node(type_t type, node_t *lhs, node_t *rhs, char **argv,
                  char *filename) {
    node_t *result = malloc(sizeof(node_t));
    result->type = type;
    result->lhs = lhs;
    result->rhs = rhs;
    result->argv = argv;
    result->filename = filename;
    return result;
}

/* free node objects recursively. */
void free_node(node_t *node) {
    if (node == NULL) {
        return;
    }
    if (node->lhs != NULL) {
        free_node(node->lhs);
    }
    if (node->rhs != NULL) {
        free_node(node->rhs);
    }
    if (node->argv != NULL) {
        for (int i = 0; node->argv[i] != NULL; i++) {
            free(node->argv[i]);
        }
        free(node->argv);
    }
    if (node->filename != NULL) {
        free(node->filename);
    }
    free(node);
}

/* free node objects recursively. */
#define TO_STR(t) #t
char *inspect_node_type(type_t type) {
    switch (type) {
    case N_COMMAND:
        return TO_STR(N_COMMAND);
    case N_SEQUENCE:
        return TO_STR(N_SEQUENCE);
    case N_AND:
        return TO_STR(N_AND);
    case N_OR:
        return TO_STR(N_OR);
    case N_PIPE:
        return TO_STR(N_PIPE);
    case N_REDIRECT_IN:
        return TO_STR(N_REDIRECT_IN);
    case N_REDIRECT_OUT:
        return TO_STR(N_REDIRECT_OUT);
    case N_REDIRECT_ERR:
        return TO_STR(N_REDIRECT_ERR);
    case N_REDIRECT_APPEND:
        return TO_STR(N_REDIRECT_APPEND);
    case N_SUBSHELL:
        return TO_STR(N_SUBSH);
    }
}

/**
 * Stringaze the given node.
 * It returns a static variable: An invocation will break the previous result.
 */
char *inspect_node(node_t *node) {
    static char buf[4096];
    char *p = buf;
    p += sprintf(p, "[%s]", inspect_node_type(node->type));
    if (node->argv != NULL) {
        p += sprintf(p, " argv: %s", inspect_tokens(node->argv));
    }
    if (node->filename != NULL) {
        p += sprintf(p, " filename: {%s}", node->filename);
    }
    return buf;
}

/**
 * Inspect tokens.
 * It returns a static variable: An invocation will break the previous result.
 */
char *inspect_tokens(char **tokens) {
    static char buf[4096];
    char *p = buf;
    int i;
    for (i = 0; tokens[i] != NULL; i++) {
        int length = sprintf(p, "{%s} ", tokens[i]);
        p += length;
    }
    sprintf(p, "(size=%d)", i);
    return buf;
}

/**
 * Find a cmd name in a node hierarchy.
 */
char **node_argv(node_t *node) {
    while (node->argv == NULL) {
        node = node->lhs;
    }
    return node->argv;
}

void dump_node_impl(node_t *node, FILE *out, char *label, int level) {
    /* indent */
    for (int i = 0; i < level; i++) {
        fprintf(out, "  ");
    }
    if (label != NULL) {
        fprintf(out, "(%s) ", label);
    }
    fprintf(out, "%s\n", inspect_node(node));
    if (node->lhs != NULL) {
        dump_node_impl(node->lhs, out, "lhs", level + 1);
    }
    if (node->rhs != NULL) {
        dump_node_impl(node->rhs, out, "rhs", level + 1);
    }
}
void dump_node(node_t *node, FILE *out) {
    dump_node_impl(node, out, "root", 0);
}

/* === alist === */

/* add arg to the tail of arglist */
alist_t *make_alist(char *arg, alist_t *next) {
    alist_t *result = malloc(sizeof(alist_t));
    result->arg = arg;
    result->next = next;
    return result;
}

/* free an alist object recursively. */
void free_alist(alist_t *alist) {
    while (alist != NULL) {
        alist_t *next = alist->next;
        free(alist);
        alist = next;
    }
}

/* count the elements of an alist. */
int len_alist(alist_t *alist) {
    int result = 0;
    while (alist != NULL) {
        result++;
        alist = alist->next;
    }
    return result;
}

/* convert an arg_list to 'char **argv' format. */
char **convert_to_argv(alist_t *alist) {
    int len = len_alist(alist);
    char **result = malloc(sizeof(char *) * (len + 1));
    for (int i = 0; i < len; i++) {
        result[i] = alist->arg;
        alist = alist->next;
    }
    result[len] = NULL; /* NULL-terminated */
    free_alist(alist);
    return result;
}
