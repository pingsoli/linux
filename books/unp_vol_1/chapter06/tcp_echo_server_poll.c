#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#define MAXLINE  1024
#define OPEN_MAX 256

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  int i, maxi, listenfd, connfd, sockfd;
  int nready;
  ssize_t  n;
  char buf[MAXLINE];
  socklen_t clilen;
  struct pollfd client[OPEN_MAX];
  struct sockaddr_in cliaddr, servaddr;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(atoi(argv[1]));

  if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (listen(listenfd, 32) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  client[0].fd = listenfd;
  client[0].events = POLLRDNORM;
  for (i = 1; i < OPEN_MAX; ++i)
    client[i].fd = -1;

  maxi = 0;

  for ( ;; ) {
    nready = poll(client, maxi + 1, -1);

    if (client[0].revents & POLLRDNORM) { /* new client connection */
      clilen = sizeof(cliaddr);
      connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);

      printf("new connection from %d\n", connfd);

      for (i = 1; i < OPEN_MAX; ++i) {
        if (client[i].fd < 0) {
          client[i].fd = connfd; /* save descriptor */
          break;
        }
      }

      if (i == OPEN_MAX) {
        fprintf(stderr, "too many clients\n");
        exit(-1);
      }

      client[i].events = POLLRDNORM;
      if (i > maxi)
        maxi = i; /* max index in client[] array */

      if (--nready <= 0)
        continue; /* no more readable descriptors */
    }

    for (i = 1; i <= maxi; ++i) {
      if ((sockfd = client[i].fd) < 0)
        continue;

      if (client[i].revents & (POLLRDNORM | POLLERR)) {
        if ((n = read(sockfd, buf, MAXLINE)) < 0) {
          if (errno == ECONNRESET) {
            /* connection rest by client */
            close(sockfd);
            client[i].fd = -1;
          } else {
            fprintf(stderr, "read error");
            exit(-1);
          }
        } else if (n == 0) {
          /* connection closed by client */
          printf("client %d ask for termination the connection\n",
              client[i].fd);
          close(sockfd);
          client[i].fd = -1;
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
