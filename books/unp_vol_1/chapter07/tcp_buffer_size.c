/*
 * get the receive and send buffer size in tcp
 *
 * $ cat /proc/sys/net/ipv4/tcp_rmem    # (read buffer size)
 * 4096    87380   6291456
 * $ cat /proc/sys/net/ipv4/tcp_wmem    # (write buffer size)
 * 4096    16384   4194304
 *
 * three numbers, which are minimum, default, maximum buffer size.
 */
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>

void print_buffer_size(int sockfd)
{
  int rbs; /* read buffer size */
  int wbs; /* write buffer size */
  int rbs_s = sizeof(rbs);
  int wbs_s = sizeof(wbs);
  int socktype, socktype_s;

  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rbs, &rbs_s);
  getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &wbs, &wbs_s);
  getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &socktype, &socktype_s);

  printf(
    "send buffer size:    %d\n"
    "receive buffer size: %d\n", rbs, wbs
  );

  /* for TCP information */
  if (socktype == SOCK_STREAM) {
    int mss;
    int mss_s = sizeof(mss);

    getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &mss_s);
    printf("MSS value:           %d\n", mss);
  }
}

int main(int argc, char** argv)
{
  int sockfd;

  /* TCP */
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  printf("tcp default buffer size\n");
  print_buffer_size(sockfd);

  /* UDP */
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  printf("\nudp default buffer size\n");
  print_buffer_size(sockfd);

  return 0;
}
