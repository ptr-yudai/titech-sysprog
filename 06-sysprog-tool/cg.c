#define _GNU_SOURCE
#include <string.h>
#include <stdarg.h> /* va_list */
#include <stdio.h> /* printf */
#include <dlfcn.h> /* dladdr */
#include <stdlib.h> /* atexit, getenv */

#define MAX_DEPTH 32
#define MAX_CALLS 1024

typedef struct CALLSTACK {
  int num;			// number of children
  int called;			// how many times this function was called
  char *name;			// function name
  struct CALLSTACK *parent;	// parent function
  struct CALLSTACK **child;	// child functions
} CALLSTACK;
CALLSTACK *cs = NULL;

__attribute__((no_instrument_function))
int log_to_stderr(const char *file, int line, const char *func, const char *format, ...) {
  char message[4096];
  va_list va;
  va_start(va, format);
  vsprintf(message, format, va);
  va_end(va);
  return fprintf(stderr, "%s:%d(%s): %s\n", file, line, func, message);
}
#define LOG(...) log_to_stderr(__FILE__, __LINE__, __func__, __VA_ARGS__)

__attribute__((no_instrument_function))
const char *addr2name(void* address) {
  Dl_info dli;
  if (dladdr(address, &dli) != 0) {
    return dli.dli_sname;
  } else {
    return NULL;
  }
}

__attribute__((no_instrument_function))
void bye(void) {
  if (cs != NULL) {
    FILE *fp = fopen("cg.dot", "a");
    fprintf(fp, "}\n");
    fclose(fp);
  }
}

__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *addr, void *call_site) {
  FILE *fp = fopen("cg.dot", "a");
  char *label = getenv("SYSPROG_CG_LABEL");
  
  if (cs == NULL) {
    fclose(fp);
    FILE *fp = fopen("cg.dot", "w"); /* uncode */
      
    /* first call */
    cs = (CALLSTACK*)malloc(sizeof(CALLSTACK));
    cs->num = 0;
    cs->called = 1;
    cs->name = (char*)addr2name(addr);
    cs->parent = NULL;
    cs->child = (CALLSTACK**)malloc(sizeof(CALLSTACK*) * MAX_CALLS);
    fprintf(fp, "strict digraph G {\n");

    /* register destructor */
    atexit(bye);
    
  } else {
    
    /* create child */
    char *name = (char*)addr2name(addr);
    if (name == NULL) {
      // 応急処置
      name = malloc(0x18);
      sprintf(name, "\"unk(%p)\"", addr);
    }
    int i, flag = 0;
    for(i = 0; i < cs->num; i++) {
      if (strcmp(cs->child[i]->name, name) == 0) {
	flag = 1;
	break;
      }
    }
    if (flag) {
      cs->child[i]->called++;
      /* move to child */
      cs = cs->child[i];
    } else {
      cs->child[cs->num] = (CALLSTACK*)malloc(sizeof(CALLSTACK));
      cs->child[cs->num]->num = 0;
      cs->child[cs->num]->called = 1;
      cs->child[cs->num]->name = name;
      cs->child[cs->num]->parent = cs;
      cs->child[cs->num]->child = (CALLSTACK**)malloc(sizeof(CALLSTACK*) * MAX_CALLS);
      cs->num++;
      /* move to child */
      cs = cs->child[cs->num - 1];
    }
    
    /* confirm path */
    if (label == NULL || strcmp(label, "1") != 0) {
      fprintf(fp, "  %s -> %s;\n", cs->parent->name, cs->name);
    } else {
      fprintf(fp, "  %s -> %s [label=\"%d\"];\n", cs->parent->name, cs->name, cs->called);
    }
    
  }

  fclose(fp);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *addr, void *call_site) {
  /* go back to parent */
  cs = cs->parent;
  
  if (cs == NULL) {
    /* main */
    FILE *fp = fopen("cg.dot", "a");
    fprintf(fp, "}\n");
    fclose(fp);
  }
}
