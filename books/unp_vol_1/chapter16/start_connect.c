#include "web.h"

void start_connect(struct file *fptr)
{
  int fd, flags, n;
  struct addrinfo *ai;

  ai = host_serv(fptr->f_host, SERV, 0, SOCK_STREAM);

  fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
  fptr->f_fd = fd;
  printf("start connect for %s, fd %d\n", fptr->f_name, fd);

  /* set socket nonblocking */
  flags = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    fprintf(stderr, "set %d file descriptor nonblocking failed!\n", fd);
    exit(-1);
  }

  /* Initiate nonblocking connect to the server */
  if ((n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0) {
    if (errno != EINPROGRESS)
      fprintf(stderr, "nonblocking connect error\n");
    fptr->f_flags = F_CONNECTING;
    FD_SET(fd, &rset);
    FD_SET(fd, &wset);
    if (fd > maxfd)
      maxfd = fd;
  } else if (n >= 0)        /* connect is already done */
    write_get_cmd(fptr);    /* write() the GET command */
}
