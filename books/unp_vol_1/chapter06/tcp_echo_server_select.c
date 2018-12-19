/*
 * simple echo server using select
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-2);
  }

  int i, maxi, maxfd, listenfd, connfd, sockfd;
  int nready, client[FD_SETSIZE];
  ssize_t n;
  fd_set rset, allset;
  char buf[MAXLINE];
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  char cliip[INET_ADDRSTRLEN];

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (listen(listenfd, 32) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  maxfd = listenfd;
  maxi = -1;
  for (i = 0; i < FD_SETSIZE; ++i)
    client[i] = -1;

  FD_ZERO(&allset);
  FD_SET(listenfd, &allset);

  for ( ;; ) {
    rset = allset;
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    printf("the ready file descriptor num: %d\n", nready);

    if (FD_ISSET(listenfd, &rset)) { /* new client connection */
      /*
       * the client send RST to server, server sleep 5 seconds to receive
       * RST packet, and then accept() the destoryed connection.
       */
      printf("listening socket readable\n");
      sleep(5);

      clilen = sizeof(cliaddr);
      connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);

      /* print new client ip address and port number */
      printf("connection fd[%d] from %s port %d\n",
          connfd,
          inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, clilen),
          cliaddr.sin_port);

      for (i = 0; i < FD_SETSIZE; ++i)
        if (client[i] < 0) {
          client[i] = connfd; /* save descriptor */
          break;
        }

      if (i == FD_SETSIZE) {
        fprintf(stderr, "too many clients\n");
        exit(-1);
      }

      FD_SET(connfd, &allset);
      if (connfd > maxfd)
        maxfd = connfd; /* add new descriptor to set */

      if (i > maxi)
        maxi = i; /* max index in client[] array */

      if (--nready <= 0)
        continue; /* no more readable descriptors */
    }

    for (i = 0; i <= maxi; ++i) { /* check all clients for data */
      if ((sockfd = client[i]) < 0)
        continue;

      if (FD_ISSET(sockfd, &rset)) {
        if ((n = read(sockfd, buf, MAXLINE)) == 0) {
          /* connection close by client */
          printf("client terminate the connection\n");
          close(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
        } else {
          write(sockfd, buf, n);
        }

        if (--nready <= 0)
          break; /* no more readable descriptors */
      }
    }
  }

  return 0;
}
