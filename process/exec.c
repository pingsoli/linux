/*
 * exec() family of functions.
 * replaces current running process with a new process. there shall be no return
 * from a successful exec, because the calling process image is overlaid by the
 * new process image.
 *
 * 'v', 'l', 'e', 'p' in exec() family functions ?
 *   the call with 'v' in the name take array parameter to specify the argv[]
 *   array of the program.
 *
 *   the call with 'l' in the name take the arguments of the new program as a
 *   variable-length argument list to the function itself.
 *
 *   the call with 'e' in the name take an extra argument to provide the
 *   environment of the new program; otherwise, the program inherits the current
 *   process's environment.
 *
 *   the call with 'p' in the name search the PATH environment variable to find
 *   the program if it doesn't have a directory in it. Otherwise, the program
 *   name is always treated as a path to the executable.
 *
 * refer to
 * https://stackoverflow.com/questions/20823371/what-is-the-difference-between-the-functions-of-the-exec-family-of-system-calls
 *
 * more detail: 'man 3 exec'
 */
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  char *cmd[] = {"ls", "--color=always", ".", NULL};
  char *env[] = {"HOME=/usr/home", "LONGNAME=home", NULL};

  /* print system environment variable */
  extern char **environ;
  int i = 0;
  printf("----------------------------\n");
  while (environ[i])
    printf("%s\n", environ[i++]);
  printf("----------------------------\n");

  /* execl("/bin/ls", "ls", "--color=always", ".", NULL); */
  /* execlp("ls", "ls", "--color=always", ".", NULL); */

  /*
   * each program has its own set of environment variables
   * if we don't specify environ, default is `extern char **environ`;
   */
  execle("/bin/ls", "ls", "--color=always", ".", NULL, env);

  /* execv("/bin/ls", cmd); */
  /* execvp("ls", cmd); */

  return 0;
}
