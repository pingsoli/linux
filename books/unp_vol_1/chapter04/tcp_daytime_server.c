/*
 * simple tcp daytime server, print client address and port number
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  int listenfd, connfd;
  socklen_t len;
  struct sockaddr_in servaddr, cliaddr;
  char buf[1024];
  time_t ticks;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(13000);

  memset(buf, 0, sizeof(buf));

  if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (listen(listenfd, 10) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  for ( ;; ) {
    len = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &len)) < 0) {
      fprintf(stderr, "accept failed: %s\n", strerror(errno));
      exit(-1);
    }

    /* print client address and port information */
    printf("connection from %s, port %d\n",
        inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)),
        cliaddr.sin_port);

    ticks = time(NULL);
    snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
    write(connfd, buf, strlen(buf));

    close(connfd);
  }

  return 0;
}
