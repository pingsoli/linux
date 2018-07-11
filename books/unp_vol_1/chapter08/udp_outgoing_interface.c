/*
 * udp outgoing interface
 *
 * if our host is multihomed machine, you can test with different ip addresses.
 *
 * Example
 * $ ./udp_outgoing_interface localhost 13000
 * local address: 127.0.0.1:15090
 * $ ./udp_outgoing_interface 10.20.30.40 1000
 * local address: 192.168.123.38:9408
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const unsigned short serv_port = 13000;

int main(int argc, char** argv)
{
  int sockfd;
  struct sockaddr_in cliaddr, servaddr;
  socklen_t len;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <ip>\n", argv[0]);
    exit(-1);
  }

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(serv_port);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  len = sizeof(servaddr);
  getsockname(sockfd, (struct sockaddr*) &cliaddr, &len);

  char cliip[INET_ADDRSTRLEN];
  printf("local address: %s:%d\n",
      inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, len),
      cliaddr.sin_port);

  return 0;
}
