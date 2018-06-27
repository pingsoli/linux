/*
 * dup() system call creates a copy of a file descriptor.
 *
 * 1) It use the lowest-numbered unused descriptor for the new descriptor.
 * 2) If the copy is successfully created, then the original and copy file
 * descriptors may be used interchangeably.
 * 3) They both refer to the same open file description and thus share file
 * offset and file status flags.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int fd = open("dup.txt", O_CREAT | O_WRONLY | O_TRUNC);

  if (fd < 0) {
    fprintf(stderr, "open file failed\n");
    exit(-1);
  }

  int copy_fd = dup(fd);

  write(copy_fd, "hello world!\n", 13);
  write(fd,      "goodbye\n",      8);

  close(fd);

  return 0;
}

