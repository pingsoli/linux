/*
 * Simple udp echo server
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
  int n;
  socklen_t len;
  char msg[1024];
  struct sockaddr_in *cliaddr = (struct sockaddr_in*) pcliaddr;

  for ( ;; ) {
    len = clilen;
    n = recvfrom(sockfd, msg, 1024, 0, pcliaddr, &len);

    if (n > 0) {
      printf("receive %d bytes\n", n);
    }

    char cliip[INET_ADDRSTRLEN];
    printf("socket %d receive data from %s port %d\n",
        sockfd,
        inet_ntop(AF_INET,&cliaddr->sin_addr, cliip, len),
        cliaddr->sin_port);

    sendto(sockfd, msg, n, 0, pcliaddr, len);
  }
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  int sockfd;
  struct sockaddr_in servaddr, cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(atoi(argv[1]));

  if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  dg_echo(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

  return 0;
}
