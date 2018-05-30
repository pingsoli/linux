/*
 * register same signal handler function with two different signals
 * and the signal handler function modify the global variable
 *
 * Attack scenario:
 * 1) the attacker send SIGHUP, which invoke handler
 * 2) SIGHUP-handler begins to execute, calling syslog()
 * 3) syslog() calls malloc(), which is non-reentrant. malloc() begins to modify
 *    metadata to manage the heap.
 * 4) the attacker send SIGTERM.
 * 5) SIGHUP-handler is interrupted, but syslog's malloc call is still executing
 *    and has not finished modifying its metadata.
 * 6) The SISTERM handler is invoked.
 * 7) SIGTERM-handler records the log message using syslog(), then frees the
 *    logMessage variable.
 *
 * PS:
 * syslog() function write the log message to /var/log/syslog file in ubuntu.
 *
 * Solution:
 * 1) Avoiding shared state.
 * 2) Using synchronization in the signal handler.
 * 3) Using synchronization in the regular code.
 * 4) Disabling or masking other signals, which provides atomicity.
 *
 * reference:
 * https://cwe.mitre.org/data/definitions/364.html
 */
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *logMessage;

void handler(int signo)
{
  syslog(LOG_NOTICE, "%s\n", logMessage);
  free(logMessage);

  /*
   * artificially increase the size of the timing window to make demonstration
   * of this weakness easier.
   */

  sleep(10);
  exit(0);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <message>\n", argv[0]);
    exit(-1);
  }

  logMessage = strdup(argv[1]);

  /* register signal handler */
  signal(SIGHUP,  handler);
  signal(SIGTERM, handler);

  sleep(10);

  return 0;
}
