#include "mysh.h"

/** Run a node and obtain an exit status. */
int invoke_node(node_t *node) {
  int status, pid, pid2;
  int pipe_io[2];
  
  LOG("Invoke: %s", inspect_node(node));
  switch (node->type) {
  case N_COMMAND:
    for (int i = 0; node->argv[i] != NULL; i++) {
      LOG("node->argv[%d]: \"%s\"", i, node->argv[i]);
    }

    /* Fork the process */
    pid = fork();
    if (pid < 0) {
      LOG("process creation failed");
      return -1;
    } else if (pid == 0) {
      execvp(node->argv[0], node->argv);
    } else {
      waitpid(pid, &status, 0);
    }

    return status;

  case N_PIPE: /* foo | bar */
    LOG("node->lhs: %s", inspect_node(node->lhs));
    LOG("node->rhs: %s", inspect_node(node->rhs));

    /* Create new PIPEs */
    if (pipe(pipe_io) < 0) {
      LOG("pipe_io creation failed.");
      return -1;
    }
    
    /* Fork the right process */
    pid = fork();
    
    if (pid < 0) {
      
      LOG("process creation failed");
      close(pipe_io[0]);
      close(pipe_io[1]);
      return -1;
      
    } else if (pid == 0) {

      /* Right node */
      dup2(pipe_io[0], 0);
      close(pipe_io[0]);
      close(pipe_io[1]);
      exit(invoke_node(node->rhs));
      
    } else {
      /* Parent */
      pid2 = fork();
      
      if (pid2 < 0) {
	
	LOG("process creation failed");
	close(pipe_io[0]);
	close(pipe_io[1]);
	return -1;
	
      } else if (pid2 == 0) {

	/* Left node */
	dup2(pipe_io[1], 1);
	close(pipe_io[0]);
	close(pipe_io[1]);
	exit(invoke_node(node->lhs));
	
      } else {
	
	/* Parent */
	close(pipe_io[0]);
	close(pipe_io[1]);
	wait(&status);
	
      }
      close(pipe_io[0]);
      close(pipe_io[1]);
      wait(&status);
    }
    
    return status;

  case N_REDIRECT_IN:     /* foo < bar */
  case N_REDIRECT_OUT:    /* foo > bar */
  case N_REDIRECT_APPEND: /* foo >> bar */
    LOG("node->filename: %s", node->filename);
    int fd, tfd;

    /* Open the file */
    if (node->type == N_REDIRECT_IN) {
      fd = open(node->filename, O_RDONLY, 0666);
    } else if (node->type == N_REDIRECT_OUT) {
      fd = open(node->filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    } else {
      fd = open(node->filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
    }
    if (fd == -1) {
      LOG("Failed to open %s", node->filename);
      return -1;
    }

    /* Redirect */
    if (node->type == N_REDIRECT_IN) {
      tfd = fcntl(0, F_DUPFD, 10);
      close(0);
      fcntl(0, F_SETFD, FD_CLOEXEC);
      dup2(fd, 0);
    } else {
      tfd = fcntl(1, F_DUPFD, 10);
      close(1);
      fcntl(1, F_SETFD, FD_CLOEXEC);
      dup2(fd, 1);
    }
    close(fd);

    /* Execute with redirect */
    status = invoke_node(node->lhs);

    /* Restore state */
    if (node->type == N_REDIRECT_IN) {
      dup2(tfd, 0);
    } else {
      dup2(tfd, 1);
    }
    close(tfd);

    return status;

  case N_SEQUENCE: /* foo ; bar */
    LOG("node->lhs: %s", inspect_node(node->lhs));
    LOG("node->rhs: %s", inspect_node(node->rhs));

    invoke_node(node->lhs);
    status = invoke_node(node->rhs);

    return status;

  case N_AND: /* foo && bar */
  case N_OR:  /* foo || bar */
    LOG("node->lhs: %s", inspect_node(node->lhs));
    LOG("node->rhs: %s", inspect_node(node->rhs));

    status = invoke_node(node->lhs);
    if ((node->type == N_AND) && (status == 0)) {
      status = invoke_node(node->rhs);
    } else if (node->type == N_OR) {
      if (status != 0) {
	status = invoke_node(node->rhs);
      } else {
	if (node->rhs->type == N_AND) {
	  status = invoke_node(node->rhs->rhs);
	}
      }
    }
    
    return status;

  case N_SUBSHELL: /* ( foo... ) */
    LOG("node->lhs: %s", inspect_node(node->lhs));

    status = invoke_node(node->lhs);
    
    return status;

  default:
    return 0;
  }
}
