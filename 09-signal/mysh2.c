#include "mysh.h"
#include <errno.h>    /* errno */
#include <stdbool.h>  /* bool */
#include <string.h>   /* strcmp */
#include <sys/wait.h> /* wait */
#include <unistd.h>   /* fork */

/* うまく表示されない場合は，下記の 1 を 0 に切り替えて下さい */
#if 0
#define FIRE "\xF0\x9F\x94\xA5\n"
#else
#define FIRE "FIRE\n"
#endif

/**
 * 引数の2文字列が等しければ真．NULLの場合は両方ともNULLであれば真．
 */
bool streql(const char *lhs, const char *rhs) {
  return (lhs == NULL && rhs == NULL) ||
    (lhs != NULL && rhs != NULL && strcmp(lhs, rhs) == 0);
}

int stop = 0;
pid_t saved_pid, pid = 0;

int fg(void) {
  int status;
  if (stop == 1) {
    LOG("[CONT(sync)] pid=%d\n", saved_pid);
    kill(saved_pid, SIGCONT);
    stop = 0;
    waitpid(saved_pid, &status, 0);
    write(STDERR_FILENO, FIRE, 5);
    return status;
  }
  return -1;
}

int bg(void) {
  if (stop == 1) {
    LOG("[CONT(async)] pid=%d\n", saved_pid);
    kill(saved_pid, SIGCONT);
    stop = 0;
    return 0;
  }
  return -1;
}

int invoke_node(node_t *node) {
  LOG("Invoke: %s", inspect_node(node));
  //pid_t pid;

  if (streql(node->argv[0], "fg")) {
    return fg();
  }
  if (streql(node->argv[0], "bg")) {
    return bg();
  }

  /* & 付きで起動しているか否か */
  if (node->async) {
    LOG("{&} found: async execution required");
  }

  /* 子プロセスの生成 */
  fflush(stdout);
  pid = fork();
  if (pid == -1) {
    PERROR_DIE(node->argv[0]);
  }

  if (pid == 0) {
    /* 子プロセス側 */
    if (execvp(node->argv[0], node->argv) == -1) {
      PERROR_DIE(node->argv[0]);
    }
    return 0; /* never happen */
  } else {
    /* 親プロセス側 */

    /* 子に独立したプロセスグループを割り振る */
    if (setpgid(pid, pid) == -1) {
      PERROR_DIE(NULL);
    }

    /* 生成した子プロセスを待機 */
    int status, options;
    if (node->async) {
      options = WNOHANG;
    } else {
      options = WUNTRACED;
    }
    
    while(1) {
      pid_t waited_pid;
      waited_pid = waitpid(-1, &status, options);
      
      if (waited_pid == -1) {
	if (errno == ECHILD) {
	  /* すでに成仏していた（何もしない） */
	} else {
	  PERROR_DIE(NULL);
	}
	break;
      } else {
	if (waited_pid == 0) break;
	
	LOG("Waited: pid=%d, status=%d, exit_status=%d", waited_pid, status,
	    WEXITSTATUS(status));
	if (WIFEXITED(status)) {
	  write(STDERR_FILENO, FIRE, 5);
	}
      }

      options = WNOHANG;
    }
    return pid;
  }
}

void stop_handler(int sig)
{
  if (pid > 0 && stop == 0) {
    LOG("[STOP] pid=%d\n", pid);
    kill(pid, sig);
    saved_pid = pid;
    stop = 1;
  }
}

/* A hook point to initialize shell */
void init_shell(void) {
  LOG("Initializing mysh2...");

  signal(SIGTSTP, stop_handler);
}
