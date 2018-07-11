#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int max_line = 1024;

void dg_cli(FILE *fp, int sockfd, const struct sockaddr* pservaddr,
    socklen_t servlen)
{
  int i;
  char sendline[max_line];

  memset(sendline, 1, sizeof(sendline));

  for (i = 0; i < 10000000; ++i)
    sendto(sockfd, sendline, max_line, 0, pservaddr, servlen);
}

int main(int argc, char** argv)
{
  int sockfd;
  struct sockaddr_in servaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  dg_cli(stdin, sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

  return 0;
}
