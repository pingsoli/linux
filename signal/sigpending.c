/*
 * sigpending() function finds out which signal is pending at any time.
 */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  sigset_t base_mask, waiting_mask, zero_mask;

  sigemptyset(&base_mask);
  sigemptyset(&zero_mask);
  sigaddset(&base_mask, SIGINT);

  /* Block user interrupts while doing other process */
  sigprocmask(SIG_SETMASK, &base_mask, NULL);

  for ( ;; ) {
    printf("sleep 3 seconds (press <C-c> to quit)...\n");
    sleep(3);

    /* After a while, check to see whether any signals are pending */
    sigpending(&waiting_mask);

    if (sigismember(&waiting_mask, SIGINT)) {
      /* User has tried to kill the process */
      printf("\nSIGINT is pending\n");

      printf("unblocking any signals, SIGINT signal will be deliveried\n");
      /* unblock all signals include SIGINT */
      sigprocmask(SIG_SETMASK, &zero_mask, NULL);
    }
  }

  /* may not be executed, because the signal delivery is very fast */
  printf("program exit...\n");

  return 0;
}
