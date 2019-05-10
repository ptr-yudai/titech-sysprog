#include "mycat.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @param size: オプション処理後の引数の個数
 * @param args: オプション処理後の引数群
 */
int do_cat(int size, char **args) {
  char *filename = args[0];
  LOG("filename = %s", filename);

  if (follow) {
    // Keep-watching mode
    char *buf = malloc(buffer_size);
    int fd = open(filename, O_RDONLY);
    
    if (fd != -1) {
      while(1) {
	int read_size = read(fd, buf, buffer_size);
	write(STDOUT_FILENO, buf, read_size);
      }
      close(fd);
      free(buf);
    }

    return 0;
  }

  if (use_stdio) {
    
    /* -l 有効（課題3）*/
    int i;
    FILE *f;
    char *buf = malloc(buffer_size);

    for(i = 0; i < size || i < 1; i++) {
      if (args[i] == NULL || (args[i][0] == '-' && args[i][1] == '\x00')) {
	// Use stdin
	f = stdin;
      } else {
	// Open file
	f = fopen(args[i], "r");
      }
      if (f == NULL) {
	perror(args[i]);
	continue;
      }

      // Read and write the file
      while(1) {
	int read_size = fread(buf, 1, buffer_size, f);
	fwrite(buf, read_size, 1, stdout);
	if (read_size != buffer_size) {
	  break;
	}
      }
      
      // Close the file
      if (f != stdin) {
	fclose(f);
      }
    }
    
    free(buf);
    
  } else {
    
    /* -l 無効（課題1）*/
    int i;
    int fd;
    char *buf = malloc(buffer_size);
    
    for(i = 0; i < size || i < 1; i++) {
      if (args[i] == NULL || (args[i][0] == '-' && args[i][1] == '\x00')) {
	// Use stdin
	fd = STDIN_FILENO;
      } else {
	// Open file
	fd = open(args[i], O_RDONLY);
      }
      if (fd == -1) {
	// File not found
	perror(args[i]);
	continue;
      }

      // Read and write the file
      while(1) {
	int read_size = read(fd, buf, buffer_size);
	write(STDOUT_FILENO, buf, read_size);
	if (read_size != buffer_size) {
	  break;
	}
      }
      
      // Close the file
      if (fd != STDIN_FILENO) {
	close(fd);
      }
    }
    
    free(buf);
  }
  
  return 0;
}

