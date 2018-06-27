#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

/**
 * This program use signal to achieve IPC
 * One process write, another is ready to read.
 * Use SIGUSR1 and SIGUSR2 to be 0 and 1, every bit will
 * be transferred and decode to correct byte.
 */

int get_next_signal();

int recv_bit(pid_t friend);
char recv_char(pid_t friend);

void send_bit(pid_t friend, int bit);
void send_char(pid_t friend, char c);

/*
 * Signal is transmitted unorder
 * Use blocking way to keep order right
 */
void signal_ipc(int argc, char **argv)
{
  pid_t friend;

  sigset_t signals_to_mask;
  sigfillset(&signals_to_mask);
  sigprocmask(SIG_SETMASK, &signals_to_mask, NULL);

  /*
   * reader process should run: <exec> read
   */
  printf("I'm process %d. Who should I talk to? ", getpid());
  scanf("%d", &friend);

  /* Reader */
  if (!strcmp(argv[1], "read"))
  {
    while (1)
    {
      putchar(recv_char(friend));
      fflush(stdout);
    }
  }
  else /* Writer */
    while (1)
      send_char(friend, getchar());
}

int get_next_signal()
{
  int sig;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);

  sigwait(&set, &sig);
  return sig;
}

int recv_bit(pid_t friend)
{
  int bit = (get_next_signal() == SIGUSR2) ? 1 : 0;
  /*
   * It's important, we just need to send a signal,
   * SIGUSR1 and SIGUSR2 are ok.
   */
  kill(friend, SIGUSR1);
  return bit;
}

char recv_char(pid_t friend)
{
  int i;
  char c = 0;
  for (i = 0; i < 8; i++)
    c |= recv_bit(friend) << i;

  return c;
}

void send_bit(pid_t friend, int bit)
{
  int signal = bit ? SIGUSR2 : SIGUSR1;
  kill(friend, signal);
}

void send_char(pid_t friend, char c)
{
  int i;

  assert (sizeof(char) == 1);
  for (i = 0; i < 8; i++)
  {
    /* Send message */
    send_bit(friend, c & (1 << i));
    /* Wait until my friend acknowleges the message */
    get_next_signal();
  }
}

int main(int argc, char** argv)
{
  signal_ipc(argc, argv);
  return 0;
}
