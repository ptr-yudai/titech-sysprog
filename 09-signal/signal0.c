#include <signal.h> /* signal */
#include <unistd.h> /* write */


/* SIGINT handler */
void abort_handler(int sig)
{
  /* do nothing */
  return;
}

/* SIGALRM handler */
void alarm_handler(int sig)
{
  write(STDOUT_FILENO, "*", 1);
  alarm(5);
  return;
}

int main(void) {
  /* 課題1: 端末で ^C (Ctrl-C) をタイプしても殺せない(無視する)ようにせよ．*/
  if (signal(SIGINT, abort_handler) == SIG_ERR) {
    return 1;
  }

  /* 課題2: 5秒ごとに標準エラー出力に「*」を表示せよ．*/
  if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
    return 1;
  }
  alarm(5);

  /* 標準エラー出力にドットを表示し続ける */
  for (;;) {
    for (int j = 0; j < 500000000; j++)
      ; /* busy loop */
    write(STDERR_FILENO, ".", 1);
  }
  return 0;
}
