/*
 * TCP echo server using threads
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>

void str_echo(int sockfd) {
  ssize_t n;
  char buf[1024];

again:
  while ((n = read(sockfd, buf, 1024)) > 0) {
    if (write(sockfd, buf, n) < 0) {
      fprintf(stderr, "write failed: %s\n", strerror(errno));
      exit(-1);
    }
  }

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0) {
    fprintf(stderr, "read failed: %s\n", strerror(errno));
    exit(-1);
  }
}

/* more portable */
void * doit(void *arg) {
  int connfd = *((int *) arg);
  free(arg);

  pthread_detach(pthread_self());
  str_echo(connfd);
  close(connfd);
  return NULL;
}

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
  int listenfd, *iptr;
  pthread_t tid;
  socklen_t addrlen, len;
  struct sockaddr *cliaddr;

  if (argc == 2) {
    listenfd = tcp_listen(NULL, argv[1], &addrlen);
  } else if (argc == 3) {
    listenfd = tcp_listen(argv[1], argv[2], &addrlen);
  } else {
    fprintf(stderr, "usage: ./tcp_server <host> | <port>\n");
    exit(-1);
  }

  cliaddr = malloc(addrlen);

  for ( ;; ) {
    len = addrlen;
    iptr = malloc(sizeof(int));
    *iptr = accept(listenfd, cliaddr, &len);

    pthread_create(&tid, NULL, &doit, iptr);
  }

  return 0;
}
