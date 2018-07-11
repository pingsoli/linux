/*
 * tcp daytime client support ipv6
 *
 * improvement: we can make it protocol-independent.
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset */
#include <unistd.h> /* read */

const int maxline = 1024;

int main(int argc, char** argv)
{
  int sockfd, n;
  char recvline[maxline];
  struct sockaddr_in6 servaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin6_family = AF_INET6;
  servaddr.sin6_port   = htons(13);

  if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed");
    exit(-1);
  }

  if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) < 0) {
    fprintf(stderr, "inet_pton failed");
    exit(-1);
  }

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) < 0)) {
    fprintf(stderr, "connect failed");
    exit(-1);
  }

  while ((n = read(sockfd, recvline, maxline)) > 0) {
    recvline[maxline] = '\0';
    if (fputs(recvline, stdout) < 0) {
      fprintf(stderr, "fputs error");
      exit(-1);
    }
  }

  if (n < 0) {
    fprintf(stderr, "read error");
    exit(-1);
  }

  return 0;
}
