/*
 * parent process send SIGUSR1 10 times to child process.
 * child process only receives SIGUSR1 one or two times.
 *
 * pending signals of the same type are not queued, but discard.
 *
 * Real-time signal Vs. standard signals ?
 * More than one Real-time signal can be queued for the process if it has the
 * signal blocked while someone sends it.
 * In standard signals only one of a given type is queued, the rest is ignored.
 * Real-time signal guarantees the receiving order same as sending order.
 *
 * NOTE: parent process needs ignore SIGUSR1 or register own handler, or parent
 * process will terminate when receiving SIGUSR1.
 */

#include <unistd.h>
#include <signal.h>
#include <stdio.h>

typedef struct {
  int         signum;
  const char *signame;
} sig_info_t;

static sig_info_t siginfo[] = {
  { SIGINT,           "SIGINT"   },
  { SIGUSR1,          "SIGUSR1"  },
  { SIGUSR2,          "SIGUSR2"  },
  { __SIGEV_MAX_SIZE,  NULL      }
};

static const char* get_signame(int signo) {
  for (int i = 0; i < __SIGEV_MAX_SIZE; ++i)
  {
    if (siginfo[i].signum == signo) {
      return siginfo[i].signame;
    }
  }

  return NULL;
}

void endless_loop() {
  for (;;)
    sleep(1);
}

void parent_trap(int sig) {
  fprintf(stderr, "parent receive %s[%d] signal\n", get_signame(sig), sig);
}

void child_trap(int sig) {
  /* sleep(2); */
  fprintf(stderr, "children receive %s[%d] signal\n", get_signame(sig), sig);
}

int main(int argc, char *argv[])
{
  pid_t pid;
  pid = fork();

  if (pid == 0) { // child process
    signal(SIGUSR1, &child_trap);
    signal(SIGUSR2, &child_trap);

    /* printf("child process: %d\n", getpid()); */
    fprintf(stdout, "child process: %d\n", getpid());
    endless_loop();

  } else { // parent process

    /* signal(SIGUSR1, &parent_trap); */

    // parent ignore SIGUSR1 and SIGUSR2 signals.
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);

    int send_times = 10;

    printf("parent process: %d\n", getpid());
    sleep(1); // wait for child process to execute.

    // send SIGUSR1 10 times to the process group(child and parent).
    for (int i = 0; i < send_times; ++i)
    {
      kill(pid, SIGUSR1);
    }

    for (int i = 0; i < send_times; ++i)
    {
      kill(pid, SIGUSR2);
    }

    printf("parent process sending end, waiting new signal(C-c to quit) ...\n");

    /* endless_loop(); */
    pause(); // wait for a signal to terminate.
  }

  return 0;
}
