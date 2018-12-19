/*
 * Real-time signal
 *
 * the signal will not lost.
 */
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void print_realtime_signal_info() {
  printf("supported RT signal num: %ld\n"
      "  min: %d ~ max: %d\n\n",
      sysconf(_SC_RTSIG_MAX), SIGRTMIN, SIGRTMAX);
}

void print(int sig, siginfo_t *info, void *exists) {

  printf("signal num: %2d, value: %d, ppid: %d\n", sig,
      info->si_value.sival_int, info->si_pid);

  if (!info->si_value.sival_int) exit(0);
}

#define ERR(msg) do {           \
  fprintf(stderr, "%s\n", msg); \
  exit(-1);                     \
} while(0)                      \

int main(int argc, char *argv[])
{
  pid_t pid;
  sigset_t set, old_set;

  print_realtime_signal_info();
  pid = fork();

  if (pid == 0) {
    struct sigaction action;
    if (sigaction(SIGINT, &action, NULL))
      ERR("get signal failed");

    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);
    sigaddset(&set, SIGRTMIN + 1);
    sigaddset(&set, SIGINT);

    sigprocmask(SIG_BLOCK, &set, &old_set);

    action.sa_mask      = set;
    action.sa_flags     = SA_SIGINFO; // tack care.
    action.sa_sigaction = print;

    if (sigaction(SIGRTMIN, &action, NULL)
        || sigaction(SIGRTMIN + 1, &action, NULL)
        || sigaction(SIGINT, &action, NULL))
      ERR("adding sigaction failed");

    while (1)
    {
      // temporarily replace the singal mask of the calling process with the
      // mask given by passed arguments, and then suspends the process until
      // delivery of a signal whose action is to invoke a signal handler or to
      // terminate a process.
      sigsuspend(&old_set);
    }

  } else {

    union sigval val;
    int sta;
    sleep(1);

    val.sival_int = 1;
    sigqueue(pid, SIGRTMIN + 1, val);

    val.sival_int = 2;
    sigqueue(pid, SIGRTMIN, val);

    val.sival_int = 5;
    sigqueue(pid, SIGRTMIN, val);

    val.sival_int = 3;
    sigqueue(pid, SIGINT, val);

    val.sival_int = 4;
    sigqueue(pid, SIGRTMIN, val);

    // suspends execution of the calling process until one of the children
    // terminates.
    wait(&sta);

    exit(0);
  }

  return 0;
}
