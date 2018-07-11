#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

int
udp_client(const char *host, const char *serv,
    struct sockaddr **saptr, socklen_t *lenp)
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(errno));
    exit(-1);
  }
  ressave = res;

  do {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd > 0)
      break;
  } while ((res = res->ai_next));

  *saptr = malloc(res->ai_addrlen);
  memcpy(*saptr, res->ai_addr, res->ai_addrlen);
  *lenp = res->ai_addrlen;

  freeaddrinfo(ressave);

  return (sockfd);
}

const int max_line = 1024;

int main(int argc, char** argv)
{
  int sockfd, n;
  char recvline[max_line + 1];
  socklen_t salen;
  struct sockaddr *sa;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <hostname/ip> <service/port>\n", argv[0]);
    exit(-1);
  }

  sockfd = udp_client(argv[1], argv[2], &sa, &salen);

  sendto(sockfd, "", 1, 0, sa, salen); /* send 1 bytes datagram */

  memset(recvline, 0, sizeof(recvline));
  n = recvfrom(sockfd, recvline, max_line, 0, NULL, NULL);
  recvline[max_line] = 0;
  fputs(recvline, stdout);

  return 0;
}
