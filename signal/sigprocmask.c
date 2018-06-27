/*
 * the following code include how to block a signal and unblock it(reset signal
 * set).
 *
 * the standard useages of sigprocmask() function.
 * 1) set signal set
 * 2) block signals
 * 3) unblock signals
 *
 * how to block signals ?
 * block a signal, and do something in critical code, and then unblock the
 * signal.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_int(int signo) {
  printf("captured SIGINT signal...\n");
}

void sig_term(int signo) {
  printf("captured SIGTERM signal...\n");
}

typedef void (*sighandler_t) (int);

void Signal(int signum, sighandler_t handler, const char *errmsg) {
  if (signal(signum, handler) == SIG_ERR) {
    fprintf(stderr, "%s\n", errmsg);
    exit(-1);
  }
}

int main(int argc, char *argv[])
{
  sigset_t newmask, oldmask;

  Signal(SIGINT,  sig_int,  "SIG_INT register failed");
  Signal(SIGTERM, sig_term, "SIG_TERM register failed");

  sigemptyset(&newmask);
  sigemptyset(&oldmask);
  sigaddset(&newmask, SIGINT);

  /* block SIGINT signal */
  if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
    fprintf(stderr, "block newmask failed\n");
    exit(-1);
  }

  /* critical region of code */
  printf("block SIGINT signal, and execute something important...\n");
  pause(); /* wait for a signal except SIGINT, because we block it */

  /* unblock SIGINT signal */
  if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
    fprintf(stderr, "recover the origin signal mask failed\n");
    exit(-1);
  }

  printf("unblock SIGINT signal, now we can capture the SIGINT signals\n");

  return 0;
}
