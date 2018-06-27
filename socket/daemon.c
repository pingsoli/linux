/*
 * Create daemon, just a simple example
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

static char *pid_file_name = NULL;
static char *app_name = NULL;
static int pid_fd = -1;
static int delay = 1;
extern int errno;

static void daemonize()
{
  pid_t pid = 0;
  int fd;

  /* Fork off the parent process */
  pid = fork();

  /* An error occurred */
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  /* Success: let the parent proces terminate */
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  /* Set the child process becomes session leader */
  if (setsid() < 0) {
    exit(EXIT_FAILURE);
  }

  /* Ignore signal sent from child to parent process */
  signal(SIGCHLD, SIG_IGN);

  /* Fork off the second time */
  pid = fork();

  /* An error occurred */
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  /* Success: let the parent process terminate */
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  /* Set new file permissions */
  umask(0);

  /* Change the working directory to the root directory */
  chdir("/");

  /* Close all open file descriptions */
  for (fd = sysconf(_SC_OPEN_MAX); fd > 0; --fd) {
    close(fd);
  }

  /* Reopen stdin, stdout, stderr */
  stdin  = fopen("/dev/null", "r");
  stdout = fopen("/dev/null", "w+");
  stderr = fopen("/dev/null", "w+");

  /* Try to write PID for lockfile */
  if (pid_file_name != NULL) {
    char str[256];

    pid_fd = open(pid_file_name, O_RDWR | O_CREAT, 0640);
    if (pid_fd < 0) {
      exit(EXIT_FAILURE);
    }

    if (lockf(pid_fd, F_LOCK, 0) < 0) {
      exit(EXIT_FAILURE);
    }

    /* Get current pid */
    sprintf(str, "%d\n", getpid());

    /* Write PID to lockfile */
    write(pid_fd, str, strlen(str));
  }
}

int main(int argc, char *argv[]) {

  pid_file_name = "daemon.pid";
  app_name = argv[0];

  daemonize();

  openlog(argv[0], LOG_PID | LOG_CONS, LOG_DAEMON);
  syslog(LOG_INFO, "Start %s", app_name);
  int counter = 0;

  for (;;)
  {
    /* TODO: do useful thing here */
    syslog(LOG_INFO, "Write something %d", counter++);

    sleep(delay);
  }

  closelog();
  return 0;
}
