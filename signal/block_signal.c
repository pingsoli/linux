/*
 * What is the meaning of blocking a signal ?
 * Blocking a signal means telling the operating system to hold it and deliver
 * it later when it is generated and when it is delivered a singal is said to
 * be pending.
 *
 * How to block or unblock a signal ?
 * sigprocmask(SIG_BLOCK, &blockmask, &oldmask); // block
 * sigprocmask(SIG_SETMASK, &oldmask, NULL);     // unblock
 * NOTE: it's impossible to block SIGKILL or SIGSTOP.
 *
 * NOTE:
 * 1) same signals are not been queued, may be delivered only once even
 * triggered multi-times. (if you want ordered signal, realtime signal
 * satisfies you).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void sigint_handler(int signo) {
  printf("terminate ...\n");
  exit(0); // terminate current process
}

void sigusr_handler(int signo) {
  printf("SIGUSR1 triggered\n");
}

int main(int argc, char** argv)
{
  pid_t pid;

  pid = fork();
  if (pid == 0) {

    // child process
    sigset_t intmask;

    signal(SIGINT, sigint_handler);
    signal(SIGUSR1, sigusr_handler);

    if ((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, SIGUSR1) == -1)) {
      perror("Failed to initialize the signal mask");
      exit(-1);
    }

    for ( ; ; ) {
      printf("\nEntering blocking state\n");
      if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1)
        break;

      printf("Signal is blocking\n");
      sleep(3);

      printf("\nLeaving blocking state\n");
      if (sigprocmask(SIG_UNBLOCK, &intmask, NULL) == -1)
        break;

      printf("Signal is unblocking\n");
      sleep(3);
    }

    perror("failed to change signal status(blocking or unblocking)");
    exit(0);
  }

  // parent process

  sleep(2);
  // send two SIGUSR1 to child process, you will find only one SIGUSR1 received.
  kill(pid, SIGUSR1);
  kill(pid, SIGUSR1);

  wait(NULL);

  return 0;
}
