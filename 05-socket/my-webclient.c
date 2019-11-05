#include <sys/socket.h>     /* socket, connect */
#include <string.h>         /* memset */
#include <arpa/inet.h>      /* htons, inet_addr */
#include <sys/types.h>      /* read */
#include <sys/uio.h>        /* read */
#include <unistd.h>         /* read, write, close */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTP_REQUEST_GET "GET / HTTP/1.0"
#define HTTP_NEWLINE "\r\n"
#define HTTP_ENDLINE "\r\n\r\n"
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

    int sockfd;
    struct sockaddr_in client_addr;
    int num;
    char buf[BUFFER_SIZE];

    /*

     ソケットの生成，サーバへの接続部分を完成させなさい．

    */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      perror("socket");
      exit(1);
    }
    memset((char*)&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = PF_INET;
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr.sin_port = htons(10000);
    
    if (connect(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) > 0) {
      perror("connect");
      exit(1);
    }

    /*

     HTTPリクエストの送信部分を完成させなさい．

    */
    write(sockfd, HTTP_REQUEST_GET, sizeof(HTTP_REQUEST_GET));
    write(sockfd, HTTP_ENDLINE, sizeof(HTTP_ENDLINE));

    /*

     HTTPレスポンスを受信し，メッセージボディだけを標準出力へ出力しなさい．
  
    */
    char *ptr;
    int writeFlag = 0;
    while((num = read(sockfd, buf, BUFFER_SIZE)) > 0) {
      if ((writeFlag == 0) && ((ptr = strstr(buf, HTTP_ENDLINE)) == NULL)) {
	continue;
      } else if (writeFlag == 0) {
	writeFlag = 1;
	ptr += sizeof(HTTP_ENDLINE) - 1;
	write(STDOUT_FILENO, ptr, num - (int)(ptr - buf));
      } else {
	write(STDOUT_FILENO, buf, num);
      }
    }

    close(sockfd);
    return 0;
}
