#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* atoi */
#include <unistd.h> /* sleep */

int main(int argc, char **argv) {
    int sec = atoi(argv[1]);
    for (int i = sec; i > 0; i--) {
        if (i <= 3) {
            if (argc > 2) {
                fprintf(stderr, "%s-%d ", argv[2], i);
            } else {
                fprintf(stderr, "%d ", i);
            }
        }
        sleep(1);
    }
    return 0;
}
