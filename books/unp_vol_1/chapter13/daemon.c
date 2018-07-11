/*
 * create a daemon process runing in background
 */
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h> /* for basename() */

const int MAXFD = 64;
const int log_level = LOG_NOTICE | LOG_USER;

/* SIGTERM */
void sigterm_handler(int signum) {
  syslog(log_level, "[pingsoli] daemon terminates");
}

int daemon_init(const char *pname, int facility)
{
  int i;
  pid_t pid;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork failed\n");
    exit(-1);
  } else if (pid)
    _exit(0); /* parent terminates */

  /* child 1 continues ... */
  if (setsid() < 0) /* become a session leader */
    return -1;

  if (signal(SIGHUP, SIG_IGN) < 0) {
    fprintf(stderr, "signal failed");
    return -1;
  }

  if (signal(SIGTERM, sigterm_handler) < 0) {
    fprintf(stderr, "signal failed");
    return -1;
  }

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork failed\n");
    exit(-1);
  } else if (pid)
    _exit(0); /* child 1 terminates */

  /* child 2 continues ... */

  chdir("/");

  /* close off file descriptors */
  for (i = 0; i < MAXFD; ++i)
    close(i);

  /* redirect stdin, stdout and stderr to /dev/null */
  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);

  /*
   * openlog() is optional, syslog() will send message to syslogd process,
   * and record the message in /var/log/syslog file
   */
  openlog(pname, LOG_PID, facility);
  syslog(log_level, "[pingsoli]starting a daemon process...");

  return 0;
}

int main(int argc, char** argv)
{
  /*
   * if put the arguments checking behind the daemon_init() function,
   * the output information will not print to stdout.
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  daemon_init(basename(argv[0]), 0);

  for ( ;; )
    sleep(1);

  return 0;
}
