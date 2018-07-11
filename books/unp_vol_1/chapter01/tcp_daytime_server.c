/*
 * tcp daytime server
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset */
#include <time.h>
#include <unistd.h> /* read */

const int maxline = 1024;

int main(int argc, char** argv)
{
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  char buffer[maxline];
  time_t ticks;
  int user_count = 0;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed");
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

  if (listen(listenfd, 10) < 0) {
    fprintf(stderr, "listen failed: %s", strerror(errno));
    exit(-1);
  }

  memset(buffer, 0, maxline);

  for (;;) {
    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);

    user_count++; /* new incoming connection */

    ticks = time(NULL);
    snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(&ticks));

    if (write(connfd, buffer, maxline) < 0) {
      fprintf(stderr, "write failed");
      exit(-1);
    }

    printf("total user count: %d, current %d\n", user_count, connfd);

    close(connfd);
  }

  return 0;
}
