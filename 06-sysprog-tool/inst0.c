#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h> /* dladdr */
#include <stdio.h> /* printf */

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
void __cyg_profile_func_enter(void *addr, void *site_call) {
  printf(">>> %s (%p)\n", addr2name(addr), addr);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *addr, void *site_call) {
  printf("<<< %s (%p)\n", addr2name(addr), addr);
}

void foo(void);
void bar(void);
void baz(void);
void qux(void);

int main(void) {
  printf("main: %p\n", main);
  foo();
  foo();
  return 0;
}

void foo(void) {
  printf("main -> foo: %p\n", foo);
  bar();
  baz();
}

void bar(void) {
  printf("main -> foo -> bar: %p\n", bar);
}

void baz(void) {
  printf("main -> foo -> baz: %p\n", baz);
}

void qux(void) {
  printf("qux?\n");
}

