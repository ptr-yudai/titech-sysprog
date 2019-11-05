#include "mysh.h"
#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* exit */
#include <string.h> /* strchr */
#include <unistd.h> /* getopt */

/* シェル入力1行の最大文字数（NULL終端含む） */
#define MAX_LINE 8192

char *cmdname;

void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "q")) != -1) {
        switch (opt) {
        case 'q':
            /* -q: quiet */
            l_set_quiet(1);
            break;
        case '?':
        default:
            fprintf(stderr, "Usage: %s [-q] [cmdline ...]\n", cmdname);
            exit(EXIT_FAILURE);
        }
    }
}

int invoke_line(char *line) {
    LOG("Input line='%s'", line);
    node_t *node = yacc_parse(line);
    if (node == NULL) {
        LOG("Obtained empty line: ignored");
        return 0;
    }
    if (!l_get_quiet()) {
        dump_node(node, stdout);
    }
    int exit_status = invoke_node(node);
    free_node(node);
    return exit_status;
}

int main(int argc, char **argv) {
    cmdname = argv[0];
    parse_options(argc, argv);

    init_shell();

    if (optind < argc) {
        /* 引数があれば，引数に渡された文字列をそのまま実行 */
        int exit_status;
        for (int i = optind; i < argc; i++) {
            exit_status = invoke_line(argv[i]);
        }
        return exit_status;
    }

    for (int history_id = 1;; history_id++) {
        char line[MAX_LINE];
        /* プロンプトを表示 */
        printf("mysh[%d]%% ", history_id);

        /* 1行読み込み */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* EOF: Ctrl-D (^D) が送信された */
            return EXIT_SUCCESS;
        }
        /* 改行があれば潰しておく */
        char *p = strchr(line, '\n');
        if (p) {
            *p = '\0';
        }
        /* 入力された行を実行 */
        invoke_line(line);
    }
}
