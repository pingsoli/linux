/*
 * using select() function to listen event, include incoming OOB data.
 * what can we do ? select() regard oob data is a exception event.
 * so we can listen exception events, and get the OOB data.
 *
 * NOTE: only one client.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/select.h>

int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
  int listenfd, n;
  const int on = 1;
  struct addrinfo hints, *res, *ressave;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags    = AI_PASSIVE;
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "tcp_listen error for %s %s: %s",
        host, serv, gai_strerror(errno));
    exit(-1);
  }
  ressave = res;

  do {
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenfd < 0)
      continue;       /* error, try next one */

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
      fprintf(stderr, "reuse addrress failed\n");

    if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
      break;         /* success */

    close(listenfd); /* bind error, close and try next one */
  } while ((res = res->ai_next) != NULL);

  if (!res) {
    fprintf(stderr, "tcp_listen for %s %s",
        host, serv);
    exit(-1);
  }

  if (listen(listenfd, 32) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (addrlenp)
    *addrlenp = res->ai_addrlen;

  freeaddrinfo(ressave);
  return (listenfd);
}

int main(int argc, char *argv[])
{
  int listenfd, connfd, n;
  char buff[100];
  fd_set rset, xset;

  if (argc == 2) {
    listenfd = tcp_listen(NULL, argv[1], NULL);
  } else if (argc == 3) {
    listenfd = tcp_listen(argv[1], argv[2], NULL);
  } else {
    fprintf(stderr, "usage: ./tcp_server <host> | <port>\n");
    exit(-1);
  }

  if (listenfd < 0) {
    fprintf(stderr, "create socket failed\n");
    exit(-1);
  }

  if ((connfd = accept(listenfd, NULL, NULL)) < 0) {
    fprintf(stderr, "accept failed: %s\n", strerror(errno));
    exit(-1);
  }

  FD_ZERO(&rset);
  FD_ZERO(&xset);

  for ( ;; ) {
    FD_SET(connfd, &rset);
    FD_SET(connfd, &xset);

    select(connfd + 1, &rset, NULL, &xset, NULL);

    if (FD_ISSET(connfd, &xset)) {
      n = recv(connfd, buff, sizeof(buff) - 1, MSG_OOB);
      buff[n] = 0;
      printf("read %d OOB byte: %s\n", n, buff);
    }

    if (FD_ISSET(connfd, &rset)) {
      if ((n = read(connfd, buff, sizeof(buff) - 1)) == 0) {
        printf("received EOF\n");
        exit(0);
      }
      buff[n] = 0;
      printf("read %d bytes: %s\n", n, buff);
    }
  }

  return 0;
}
