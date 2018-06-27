/*
 * What is the mean of blocking a signal ?
 * Blocking a signal means telling the operating system to hold it and deliver
 * it later when it is generated and when it is delivered a singal is said to
 * be pending.
 *
 * How to block a signal ?
 * block the signal
 * sigprocmask(SIG_BLOCK, &blockmask, &oldmask)
 * unblock the signal
 * sigprocmask(SIG_SETMASK, &oldmask, NULL);
 *
 * NOTE:
 * 1) same signals are not been queued, may be delivered only once even
 * triggered multi-times.(we can block the signal, trigger the signal one more
 * time and unblock it, we can only see the handler has been called only once).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char** argv)
{
  sigset_t intmask;

  if ((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, SIGINT) == -1)) {
    perror("Failed to initialize the signal mask");
    exit(-1);
  }

  for ( ; ; ) {
    printf("\nEntering BLOCK state\n");
    if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1)
      break;

    printf("SIGINT signal is blocking\n");
    sleep(3);

    printf("\nLeaving Blocking State\n");
    if (sigprocmask(SIG_UNBLOCK, &intmask, NULL) == -1)
      break;

    printf("SIGINT signal is unblocking\n");
    sleep(3);
  }

  perror("Failed to change signal mask");

  return 0;
}
