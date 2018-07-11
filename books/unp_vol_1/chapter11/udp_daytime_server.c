/*
 * wrapper function socket(), bind()
 * udp daytime server
 */
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>

/* create a connected UDP socket */
int
udp_connect(const char *host, const char *serv)
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
    if (sockfd < 0)
      continue; /* ignore this one */

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break; /* success */

    close(sockfd);
  } while ((res = res->ai_next));

  if (!res) {
    fprintf(stderr, "udp connect failed\n");
    exit(-1);
  }

  freeaddrinfo(ressave);

  return (sockfd);
}

int
udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags    = AI_PASSIVE;
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "get addrinfo failed: %s\n", gai_strerror(errno));
    exit(-1);
  }
  ressave = res;

  do {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
      continue;

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break;

    close(sockfd);
  } while ((res = res->ai_next));

  if (!res) {                   /* errno from final socket() or bind() */
    fprintf(stderr, "bind failed\n");
    exit(-1);
  }

  if (addrlenp)
    *addrlenp = res->ai_addrlen; /* return size of protocol address */

  freeaddrinfo(ressave);

  return (sockfd);
}

const int max_line = 1024;

int main(int argc, char** argv)
{
  int sockfd;
  ssize_t n;
  char buff[max_line];
  time_t ticks;
  socklen_t len;
  struct sockaddr_storage cliaddr;

  if (argc == 2)
    sockfd = udp_server(NULL, argv[1], NULL);
  else if (argc == 3)
    sockfd = udp_server(argv[1], argv[2], NULL);
  else {
    fprintf(stderr, "usage: %s <hostname/ip> <service/port>\n", argv[0]);
    exit(-1);
  }

  for ( ;; ) {
    len = sizeof(cliaddr);
    n = recvfrom(sockfd, buff, max_line, 0, (struct sockaddr*) &cliaddr, &len);

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&cliaddr, len);
  }

  return 0;
}
