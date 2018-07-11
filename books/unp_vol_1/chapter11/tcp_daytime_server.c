/*
 * Wrapper the socket(), bind(), listen() function to tcp_listen()
 *
 * Q&A?
 * Q: convert sockaddr_storage to sockaddr_in ?
 * A:
 * struct sockaddr_storage cliaddr;
 * struct sockaddr_in *pcliaddr = &cliaddr;
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

int
tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
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

const int max_line = 1024;

int main(int argc, char** argv)
{
  int listenfd, connfd;
  socklen_t len;
  char buff[max_line];
  time_t ticks;
  struct sockaddr_storage cliaddr;
  struct sockaddr_in *pcliaddr = (struct sockaddr_in *)&cliaddr;
  char cliip[INET_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr, "usage: %s <service or port>\n", argv[0]);
    exit(-1);
  }

  listenfd = tcp_listen(NULL, argv[1], NULL);

  memset(buff, 0, sizeof(buff));
  memset(cliip, 0, sizeof(cliip));
  for ( ;; ) {
    len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
    printf("connection from %s:%d\n",
        inet_ntop(AF_INET, &pcliaddr->sin_addr, cliip, len),
        pcliaddr->sin_port);

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    write(connfd, buff, strlen(buff));

    close(connfd);
  }

  return 0;
}
