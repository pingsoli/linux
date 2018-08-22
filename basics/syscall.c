////////////////////////////////////////////////////////////////////////////////
// what's the advantage of syscall instead of calling the function directly?
// there is no advantage to using syscall if the C library in your compilation
// environment has what you need.
////////////////////////////////////////////////////////////////////////////////
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  // get current thread id
  printf("%ld\n", syscall(SYS_gettid));
  /* printf("%ld\n", gettid()); */

  /*
   * NOTE: there is a bug for glibc.
   * you cannot use gettid() in glibc. more detail see: man gettid.
   * so, you should always use syscall(SYS_gettid).
   */

  return 0;
}
