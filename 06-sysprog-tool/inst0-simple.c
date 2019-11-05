#include <stdio.h> /* printf */

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
