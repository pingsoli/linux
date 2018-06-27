/*
 * dup2 duplicates one file descriptor, make them aliases, and then deleting the
 * old file descriptor.
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char** argv)
{
  /* Open a file, write only */
  int fd = open("myfile.txt", O_CREAT | O_APPEND | O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "open file failed: %s\n", strerror(errno));
    exit(-1);
  }

  /* Redirect standard output to the file using dup2 */
  if (dup2(fd, 1) < 0) {
    fprintf(stderr, "dup2 failed\n");
    exit(-1);
  }

  /* write to the file */
  printf("This will print in the myfile.txt\n");

  close(fd);

  return 0;
}
