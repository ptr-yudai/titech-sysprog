#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>

void session(int fd, char *cli_addr, int cli_port) {

    FILE *fin, *fout;
    char buf[BUFSIZ];

    fin = fdopen(fd, "r"); fout = fdopen(fd, "w");
    fgets(buf, sizeof(buf), fin);

    fprintf(fout, "HTTP/1.1 200 OK\r\n");
    fprintf(fout, "Content-Type: text/html\r\n");
    fprintf(fout, "\r\n");
    fprintf(fout, "<html><body><h1>Very Simple Web Server</h1>Hello. Your message is:\n<hr><pre>%s", buf);

    while (1) {
        if (fgets(buf, sizeof(buf), fin) == NULL) {
            break;
        }
        fprintf(fout, "%s", buf);
        if (!strcmp(buf, "\r\n")) {
            break;
        }
    }

    fprintf(fout, "</pre><hr>Your connection is: IP address = %s, port = %d\n</body></html>\n", cli_addr, cli_port);
    fflush(fout);
    fclose(fin);
    fclose(fout);
}

int main(int argc, char *argv[]) {

    int listfd, connfd, optval = 1, port = 10000;
    unsigned int addrlen;
    struct sockaddr_in saddr, caddr;

    if (argc >= 2) {
        port = atoi(argv[1]);
    }

    listfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listfd, (struct sockaddr *)&saddr, sizeof(saddr));

    listen(listfd, 10);

    while (1) {
        addrlen = sizeof(caddr);
        connfd = accept(listfd, (struct sockaddr *)&caddr, (socklen_t*)&addrlen);
        session(connfd, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
        close(connfd);
    }

    return 0;
}

