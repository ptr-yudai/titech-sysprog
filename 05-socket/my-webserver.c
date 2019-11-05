#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

/*----- ここから追加 -----*/
#define DOCUMENT_ROOT "./"
#define RUN_AS "ptr"
/*----- ここまで追加 -----*/

char *not_found_body = "<html><body><center><h1>404 Not Found!</h1></center></body></html>\n";

int find_file(char *path) {
  struct stat info;
  if (stat(path, &info) == -1) {
    return -1;
  }
  return (int)info.st_size;        // file size
}

char *get_content_type(char *filename) {
  if (filename[strlen(filename) - 1] == 'g') {        // jpeg or jpg
    return "image/jpeg";

  } else if (filename[strlen(filename) - 1] == 'f') {        // gif
    return "image/gif";

  } else if (filename[strlen(filename) - 1] == 'l' || filename[strlen(filename) - 1] == 'm') {        // html or htm
    return "text/html";

    /* others */
  } else {
    return "text/plain";
  }
}

void send_content_data(FILE *from, FILE *to) {
  char buf[BUFSIZ];
  int counter = 0;
  while (1) {
    int size = fread(buf, 1, BUFSIZ, from);
    if (size > 0) {
      fwrite(buf, size, 1, to);
    } else {
      break;
    }
    counter++;
  }
}

void session(int fd, char *cli_addr, int cli_port) {

  int keep_alive = 1; // この変数を追加
  FILE *fin, *fout;
  fin = fdopen(fd, "r"); fout = fdopen(fd, "w");

  while(keep_alive) { // このループを追加

    /* read request line */
    char request_buf[BUFSIZ];
    if (fgets(request_buf, sizeof(request_buf), fin) == NULL) {
      fflush(fout);
      fclose(fin);
      fclose(fout);
      close(fd);
      return;        // disconnected
    }
    
    /* parse request line */
    char method[BUFSIZ];
    char uri[BUFSIZ], *path;
    char version[BUFSIZ];
    if (sscanf(request_buf, "%s %s %s", method, uri, version) != 3) {
      close(fd);
      return;
    }
    path = &(uri[1]);
    
    printf("HTTP Request: %s %s %s %s\n", method, uri, path, version);
    fflush(stdout);
    
    /* read/parse header lines */
    while (1) {

      /* read header lines */
      char headers_buf[BUFSIZ];
      if (fgets(headers_buf, sizeof(headers_buf), fin) == NULL) {
	fclose(fin);
	fclose(fout);
	close(fd);
	return;            // disconnected from client
      }

      /* check header end */
      if (strcmp(headers_buf, "\r\n") == 0) {
	break;
      }

      /* parse header lines */
      char header[BUFSIZ];
      char value[BUFSIZ];
      sscanf(headers_buf, "%s %s", header, value);

      /*----- ここから追加 -----*/
      /* close or keep-alive */
      if (strncmp(header, "Connection:", 12) == 0) {
	if (strncmp(value, "close", 6) == 0) {
	  /* bye */
	  keep_alive = 0;
	}
      }
      /*----- ここまで追加 -----*/

      printf("Header: %s %s\n", header, value);
      fflush(stdout);

    } // while


    /*

      送信部分を完成させなさい．

    */
    /*----- ここから追加 -----*/
    if (path[0] == '\x00') {
      strcpy(path, "index.html");
    }
    
    int fsize = find_file(path);
    FILE *fp = fopen(path, "r");
    if (fsize == -1 || fp == NULL) {
      
      /* 404 Not Found */
      fprintf(fout, "%s 404 Not Found\r\n", version);
      if (keep_alive) {
	fprintf(fout, "Connection: Keep-Alive\r\n");
      } else {
	fprintf(fout, "Connection: Close\r\n");
      }
      fprintf(fout, "Content-Type: text/html\r\n");
      fprintf(fout, "Content-Length: %d\r\n\r\n", (int)strlen(not_found_body));
      fprintf(fout, "%s", not_found_body);
      
    } else {
      
      /* 200 Status OK */
      fprintf(fout, "%s 200 OK\r\n", version);
      if (keep_alive) {
	fprintf(fout, "Connection: Keep-Alive\r\n");
      } else {
	fprintf(fout, "Connection: Close\r\n");
      }
      fprintf(fout, "Content-Type: %s\r\n", get_content_type(path));
      fprintf(fout, "Content-Length: %d\r\n\r\n", fsize);
      send_content_data(fp, fout);
      fclose(fp);
      
    }
    fflush(fout);
  }
  /*----- ここまで追加 -----*/

  /* close connection */
  fclose(fin);
  fclose(fout);
  close(fd);
  printf("Connection closed.\n");
  fflush(stdout);
}

int main(int argc, char *argv[]) {

  int listfd, connfd, optval = 1, port = 10000;
  unsigned int addrlen;
  struct sockaddr_in saddr, caddr;

  if (argc >= 2) {
    port = atoi(argv[1]);
  }

  /*----- ここから追加 -----*/
  int rc;
  struct passwd *pwd = calloc(1, sizeof(struct passwd));
  size_t buffer_len = sysconf(_SC_GETPW_R_SIZE_MAX) * sizeof(char);
  char *buffer = malloc(buffer_len);
  getpwnam_r(RUN_AS, pwd, buffer, buffer_len, &pwd);
  free(buffer);
  if (pwd == NULL) {
    perror("getpwnam");
    exit(1);
  }
  // setgid/uidする前にchdir/chrootしておく
  rc = chdir(DOCUMENT_ROOT);
  if (rc < 0) {
    perror("chdir");
    exit(1);
  }
  rc = chroot(DOCUMENT_ROOT);
  if (rc < 0) {
    perror("chroot");
    exit(1);
  }
  // ここでroot権限を失う
  if (setgid(pwd->pw_gid) != 0) {
    perror("setgid");
    exit(1);
  }
  if (setuid(pwd->pw_uid) != 0) {
    perror("setuid");
    exit(1);
  }
  free(pwd);
  //system("whoami"); // DEBUG for B.(3)
  /*----- ここまで追加 -----*/

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
    
    /*----- ここから変更 -----*/
    int pid = fork();
    if (pid == 0) {
      session(connfd, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
      exit(0);
    } else if (pid < 0) {
      perror("fork");
    } else {
      close(connfd);
    }
    /*----- ここまで変更 -----*/
  }

  return 0;
}
