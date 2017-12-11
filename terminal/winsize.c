#include "winsize.h"

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

/*
 * Handle SIGWINCH signal, when window size is changed, 
 * hanlder will print windows current size info to screen.
 *
 * Test:
 * We can change the windows size, and see the result.
 */

static void 
pr_winsize(int fd)
{
  struct winsize size;

  if (ioctl(fd, TIOCGWINSZ, (char *) &size) < 0)
  {
    fprintf(stderr, "ioctl error");
    exit(1);
  }

  printf("%d rows, %d colunms\n", size.ws_row, size.ws_col);
}

static void 
sig_winch(int signo)
{
  printf("SIGWINCH received\n");
  pr_winsize(STDIN_FILENO);
}

void monitor_win_size()
{
  if (isatty(STDIN_FILENO) == 0)
    return;
  if (signal(SIGWINCH, sig_winch) == SIG_ERR)
  {
    fprintf(stderr, "signal error");
    exit(1);
  }

  /* Print initial size, and sleep forever */
  pr_winsize(STDIN_FILENO);
  for (; ;)
    pause();
}
