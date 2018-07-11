/*
 * tcp client connected to tcp server, and meanwhile tcp server is busy,
 * cannot deal with the connection immediately. then client send RST packet to
 * server, the connection is destoryed by server, so the accept() will block
 * until having completed connections.
 *
 * we can set the socket file descriptor to nonblocking, and accept will return
 * immediately.
 */
#include <libgen.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  int sockfd;
  struct linger ling;
  struct sockaddr_in servaddr;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <ip> <port>\n", basename(argv[0]));
    exit(-1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  ling.l_onoff  = 1;  /* cause RST to be sent on close() */
  ling.l_linger = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
  close(sockfd);

  return 0;
}
