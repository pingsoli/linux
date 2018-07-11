/*
 * TCP and UDP server using select
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define max(a,b) \
     ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })

const int max_line = 1024;

void sigchld_handler(int signum) {
  printf("connection terminated\n");
}

void str_echo(int sockfd) {
  ssize_t n;
  char buf[max_line];

again:
  while ((n = read(sockfd, buf, max_line)) > 0)
    write(sockfd, buf, n);

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0) {
    fprintf(stderr, "read failed: %s\n", strerror(errno));
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>", argv[0]);
    exit(-1);
  }

  int listenfd, connfd, udpfd, nready, maxfd;
  char msg[max_line];
  pid_t childpid;
  fd_set rset;
  ssize_t n;
  socklen_t len;
  const int on = 1;
  struct sockaddr_in cliaddr, servaddr;
  char cliip[INET_ADDRSTRLEN];

  //////////////////////////////////////////////////////////////////////////////
  /* create listening TCP socket */
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(atoi(argv[1]));

  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    fprintf(stderr, "reuse port failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (listen(listenfd, 32) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  /////////////////////////////////////////////////////////////////////////////
  /* create UDP socket*/
  if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(atoi(argv[1]));

  if ((bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  signal(SIGCHLD, sigchld_handler);

  FD_ZERO(&rset);
  maxfd = max(listenfd, udpfd) + 1;

  for ( ;; ) {
    FD_SET(listenfd, &rset);
    FD_SET(udpfd, &rset);

    if ((nready = select(maxfd, &rset, NULL, NULL, NULL)) < 0) {
      if (errno == EINTR)
        continue;
      else {
        fprintf(stderr, "select failed: %s\n", strerror(errno));
        exit(-1);
      }
    } /* select */

    /* TCP events */
    if (FD_ISSET(listenfd, &rset)) {
      len = sizeof(cliaddr);
      connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);

      printf("connection from %s:%d\n",
          inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, len),
          cliaddr.sin_port);

      if ((childpid = fork()) == 0) { /* child process */
        close(listenfd);
        str_echo(connfd);
        exit(0);
      }

      close(connfd);
    }

    /* UDP events */
    if (FD_ISSET(udpfd, &rset)) {
      len = sizeof(cliaddr);

      n = recvfrom(udpfd, msg, max_line, 0, (struct sockaddr *)&cliaddr, &len);

      printf("udp: receive %ld bytes from %s:%d\n", n,
          inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, len),
          cliaddr.sin_port);

      sendto(udpfd, msg, n, 0, (struct sockaddr*)&cliaddr, len);
    }
  } /* for loop: main */

  return 0;
}
