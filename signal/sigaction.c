/*
 * sigaction structure and sigaction function
 *
 * Versus to signal, sigaction has more flexibility. we will see it from the
 * example.
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void sigterm_handler(int signum, siginfo_t *siginfo, void *context) {
  /* cleaning up */
  printf("do some cleaning work\n");
}

int main(int argc, char** argv)
{
  struct sigaction action;
  memset(&action, 0, sizeof(action));

  /*
   * use the sa_sigaction field because the handles
   * has two additional parameters
   */
  action.sa_sigaction = &sigterm_handler;

  /*
   * the sa_mask indicates any signal that should be blocked while the signal
   * is being executed
   */
  sigemptyset(&action.sa_mask);

  /*
   * the SA_SIGINFO flags tells sigaction() to use the sa_sigaction field,
   * not sa_handler.
   * whether system calls that are interrupted by the signal are automatically
   * SA_RESTART.
   */
  action.sa_flags = SA_RESTART | SA_SIGINFO;

  if (sigaction(SIGTERM, &action, NULL) < 0) {
    fprintf(stderr, "sigaction failed\n");
    exit(-1);
  }

  for ( ;; )
    sleep(1);

  return 0;
}
