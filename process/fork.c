/*
 * system call fork() used to create processes.
 *
 * fork() function return twice. How does that happen ?
 * Because both processes will start their execution at the next statement
 * following the fork() call.
 *
 * NOTE: the program exist race condition.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int global = 0;

void ChildProcess(void);
void ParentProcess(void);

int main(int argc, char *argv[])
{
  pid_t pid;

  pid = fork();
  if (pid == 0)
    ChildProcess();
  else
    ParentProcess();

  return 0;
}

void ChildProcess(void) {
  for (int i = 1; i <= 10; ++i) {
    printf("  This line is from child, value = %d\n", i);
    global++;
  }
  printf("  global variable value(child process): %d\n", global);
  printf("    *** child process is done ***\n");
}

void ParentProcess(void) {
  for (int i = 1; i <= 10; ++i) {
    printf("This line is from parent, value = %d\n", i);
  }
  printf("global variable value(parent process): %d\n", global);
  printf("*** parent is done ***\n");
}
