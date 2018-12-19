/*
 * first program to understand signal.
 * know that register a handler to handle all kind of signals
 *
 * Reliable signals and unreliable signals ?
 * earlier UNIX system has unreliable signals, that means the signals may be lost.
 *
 * void sigint_handler(int signo) {
 *   signal(SIGINT, sigint_handler); // reestablish handler for next time
 *   ...
 * }
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigterm_handler(int signal) {
  printf("receive SIGTERM signal: %d\n", signal);
}

void sigint_handler(int signal) {
  printf("receive SIGINT signal: %d\n", signal);
}

void sigquit_handler(int signal) {
  printf("receive SIGQUIT signal: %d\n", signal);
}

int main(int argc, char** argv)
{
  /*
   * kill -s SIGTERM <pid>
   * kill -SIGTERM <pid>
   * kill -TERM <pid>
   * kill -15 <pid>
   * kill default signal is SIGTERM, so kill <pid> will trigger the handler.
   */
  signal(SIGTERM, sigterm_handler); /* signal num: 15  */

  /*
   * Ctrl-C triggers SIGINT signal.
   * kill -2 <pid> trigger SIGINT signal from outside.
   * kill -INT <pid>
   */
  signal(SIGINT, sigint_handler);  /* signal num: 2 */

  /*
   * what if we register SIGKILL?
   * SIGKILL and SIGSTOP is not catchable, we couldn't overwrite its handler.
   * so, the following will not work.
   */
  signal(SIGKILL, sigquit_handler);

  pause(); // wait for signal
  printf("main function end\n");
  /* for ( ;; ) */
  /*   sleep(1); */

  return 0;
}
