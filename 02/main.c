#include "main.h"
#include "mycat.h"
#include <errno.h>  /* errno */
#include <stdarg.h> /* va_list, ... */
#include <stdio.h>  /* perror, fprintf, sprintf */
#include <stdlib.h> /* exit, atoi */
#include <string.h> /* strerror */
#include <unistd.h> /* getopt */

/* command name */
char *cmdname;

/* quiet mode (-q option) */
int is_quiet = 0;

/* follow mode (-f option) */
int follow = 0;

/* use stdio (-l option) */
int use_stdio = 0;

/* buffer size (-s N option) */
unsigned int buffer_size = 1024;

/* logging feature */

/** Prints log message to stderr. */
int logger_log(const char *file, int line, const char *func,
               const char *message) {
    if (is_quiet) {
        return 0;
    }
    return fprintf(stderr, "%s:%d(%s): %s\n", file, line, func, message);
}

/** Prints log message with parameters to stderr. */
int logger_log_f(const char *file, int line, const char *func,
                 const char *format, ...) {
    if (is_quiet) {
        return 0;
    }
    char message[4096];
    va_list va;
    va_start(va, format);
    vsprintf(message, format, va);
    va_end(va);
    return logger_log(file, line, func, message);
}

/** Prints error message to stderr and exit. */
int logger_die(const char *file, int line, const char *func,
               const char *message) {
    logger_log(file, line, func, message);
    exit(EXIT_FAILURE);
    return 1;
}

/** Prints errno-based error message to stderr and exit. */
int logger_perror_die(const char *file, int line, const char *func,
                      const char *context) {
    char *message;
    if (context != NULL) {
        char buffer[4096];
        sprintf(buffer, "%s: %s", context, strerror(errno));
        message = buffer;
    } else {
        message = strerror(errno);
    }
    return logger_die(file, line, func, message);
}

/* main features */

void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qfls:")) != -1) {
        switch (opt) {
        case 'q':
            /* -q: quiet */
            is_quiet = 1;
            break;
        case 'f':
            /* -f: follow */
            follow = 1;
            break;
        case 'l':
            /* l: use stdio */
            use_stdio = 1;
            break;
        case 's':
            /* -s N: buffer size */
            buffer_size = atoi(optarg);
            if (buffer_size == 0) {
                DIE("Invalid size");
            }
            break;
        case '?':
        default:
            fprintf(stderr, "Usage: %s [-q] [-f] [-s size] file ...\n",
                    cmdname);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv) {
    cmdname = argv[0];
    parse_options(argc, argv);
    return do_cat(argc - optind, argv + optind);
}
