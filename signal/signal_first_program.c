/*
 * signal function first program
 * know that register a handler to handle all kind of signals
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
  signal(SIGTERM, sigterm_handler); /* signal id: 15  */

  /*
   * Ctrl-C trigger SIGINT signal
   * kill -2 <pid> trigger SIGINT signal from outside.
   * kill -INT <pid>
   */
  signal(SIGINT, sigint_handler);  /* signal id: 2 */

  /*
   * what if we register SIGKILL?
   * SIGKILL and SIGSTOP is not catchable, we couldn't overwrite its handler.
   * so, the following will not work.
   */
  signal(SIGKILL, sigquit_handler);

  for ( ;; )
    sleep(1);

  return 0;
}
