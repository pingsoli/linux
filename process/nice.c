/*
 * niceness scale goes from -20 to 19.
 * the lower the number the more priority that task gets. default nice value is
 * zero.
 *
 * Priority Vs. Niceness
 * PR = 20 + NI
 * PR (denotes Priority), NI (denotes Niceness).
 *
 * you don't need to change the final priority, you can modify the niceness to
 * adjust the schedule priority of CPU processing.
 * This policy is just for security.
 *
 * FAQ:
 * Q: How do I check the niceness and priority in my terminal ?
 * top, or htop command. (top is built-in comand, htop needs to be installed)
 */
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  int highest_priority, priority;
  /* get highest priority, the lower number indicates higher priority */
  highest_priority = getpriority(PRIO_PROCESS, 0);

  /* get priority range */
  struct rlimit limit;
  getrlimit(RLIMIT_NICE, &limit);
  printf("%ld %ld\n", limit.rlim_cur, limit.rlim_max);

  setpriority(PRIO_PROCESS, 0, 5);

  /* sleep 5 seconds to check the niceness changing */
  sleep(5);

  /*
   * increase 5 to niceness value.
   * NOTE: only superuser can specify negative increment
   */
  nice(5);

  while (1)
    sleep(1);

  return 0;
}


