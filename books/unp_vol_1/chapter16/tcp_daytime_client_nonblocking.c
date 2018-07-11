/*
 * tcp daytime client nonblocking connect
 *
 * ./tcp_daytime 129.6.15.30 daytime
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

const int max_line = 1024;

int
connect_nonb(int sockfd, const struct sockaddr *saptr,
    socklen_t salen, int nsec)
{
  int flags, n, error;
  socklen_t len;
  fd_set rset, wset;
  struct timeval tval;

  /* set socket nonblocking */
  flags = fcntl(sockfd, F_GETFL, 0);
  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    fprintf(stderr, "set %d nonblocking failed\n", sockfd);

  error = 0;
  if ((n = connect(sockfd, saptr, salen)) < 0)
    if (errno != EINPROGRESS)
      return (-1);

  /* do whatever we want while the connect is taking place */

  if (n == 0)
    goto done;

  FD_ZERO(&rset);
  FD_SET(sockfd, &rset);
  wset = rset;
  tval.tv_sec  = nsec;
  tval.tv_usec = 0;

  if ((n = select(sockfd + 1, &rset, &wset, NULL,
          nsec ? &tval : NULL)) == 0) {
    close(sockfd); /* timeout */
    errno = ETIMEDOUT;
    return (-1);
  }

  if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
    len = sizeof(error);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
      return (-1); /* Solaris pending error */
  } else {
    fprintf(stderr, "select faild: socket not set\n");
    exit(-1);
  }

done:
  if (fcntl(sockfd, F_SETFL, flags) == -1)
    fprintf(stderr, "restore file status flags failed\n");

  if (error) {
    close(sockfd);
    errno = error;
    return (-1);
  }

  return 0;
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <ip>\n", basename(argv[1]));
    exit(-1);
  }

  char recvline[max_line + 1];
  char sockfd;
  struct sockaddr_in servaddr;
  int ret, n;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed\n");
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(13); /* tcp daytime server */
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  ret = connect_nonb(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr), 5);
  if (ret != 0) {
    errno = ret;
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(recvline, 0, sizeof(recvline));
  while ((n = read(sockfd, recvline, max_line)) > 0) {
    recvline[max_line] = 0;
    fputs(recvline, stdout);
  }

  return 0;
}
