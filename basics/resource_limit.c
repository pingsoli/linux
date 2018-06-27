/*
 * resource limit
 * hard limit and soft limit.
 *
 * more detail: getrlimit(), setrlimit(), prlimit().
 */
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>

void print_info(int, const char *);

int main(int argc, char *argv[])
{
  print_info(RLIMIT_AS,    "Address sapce size");
  print_info(RLIMIT_CORE,  "Core file size");
  print_info(RLIMIT_CPU,   "CPU limit seconds");
  print_info(RLIMIT_DATA,  "Data segment size");
  print_info(RLIMIT_FSIZE, "Maximum files");
  print_info(RLIMIT_NICE,  "Nice");
  print_info(RLIMIT_NPROC, "Process maximum");
  print_info(RLIMIT_STACK, "Stack size");

  return 0;
}

void print_info(int resource, const char *name) {
  int rc;
  struct rlimit limit;

  if ((rc = getrlimit(resource, &limit)) < 0) {
    fprintf(stderr, "getrlimit failed\n");
    exit(-1);
  }

  printf("%s:\n\tsoft limit: %lld\n\thard limit: %lld\n",
      name, (long long) limit.rlim_cur, (long long) limit.rlim_max);
}
