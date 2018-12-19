#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

#define check(expr) if (!(expr)) { printf(#expr\
    "\n ERROR: %s\n", strerror(errno)); exit(1); }

int main(int argc, char *argv[])
{
  struct sockaddr_in servaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_port        = htons(12345);
  check(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != -1);

  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  check(sockfd != -1);

  check(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0);

  for (int i = 0; i < 1000000; ++i) ;
  close(sockfd);

  return 0;
}
