#include "web.h"

void write_get_cmd(struct file *fptr)
{
  int n;
  char line[max_line];

  n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);
  writen(fptr->f_fd, line, n);
  printf("wrote %d bytes for %s\n", n, fptr->f_name);

  fptr->f_flags = F_READING; /* clear F_CONNECTING */

  FD_SET(fptr->f_fd, &rset);
  if (fptr->f_fd > maxfd)
    maxfd = fptr->f_fd;
}
