/*
 * TCP daytime client
 * only support IPv4.
 *
 * useage: ./tcp_daytime_client 129.6.15.28
 *
 * 58245 18-05-07 09:14:35 50 0 0 941.2 UTC(NIST) *
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset */
#include <unistd.h> /* read */
#include <errno.h>

const int maxline = 1024;

int main(int argc, char** argv)
{
  int sockfd, n;
  char recvline[maxline];
  struct sockaddr_in servaddr;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <ip-address> <port>", argv[0]);
    exit(-1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed\n");
    exit(-1);
  }

  /* port 13 is well-known port of the daytime server */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));  /* daytime server */

  /* convert ipv4 address to binary form (network byte order) big-endian */
  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    fprintf(stderr, "inet_pton failed");
    exit(-1);
  }

  if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  /* empty recv buffer, it's a good habit */
  memset(recvline, 0, maxline);

  while ((n = read(sockfd, recvline, maxline)) > 0) {
    recvline[maxline] = 0;  /* null terminate */
    if (fputs(recvline, stdout) == EOF) {
      fprintf(stderr, "fputs failed");
      exit(-1);
    }
  }

  if (n < 0) {
    fprintf(stderr, "read failed");
    exit(-1);
  }

  return 0;
}
