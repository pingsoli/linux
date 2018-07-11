/*
 * tcp daytime client
 * MSG_PEEK to get the receive buffer size and print to screen
 *
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype)
{
  int n;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags    = AI_CANONNAME; /* always return canonical name */
  hints.ai_family   = family;       /* AP_UNSPCE, AF_INET, AF_INET6, etc. */
  hints.ai_socktype = socktype;     /* O, SOCK_STREAM, SOCK_DGRAM, etc. */

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    return (NULL);

  return res;
}

int
tcp_connect(const char *host, const char *serv)
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo failed(%s, %s): %s\n",
        host, serv, gai_strerror(errno));
    exit(-1);
  }
  ressave = res;

  do {
    sockfd = socket(res->ai_family , res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
      continue;     /* ignore this one */

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break;        /* success */

    close(sockfd);  /* ignore this one */
  } while ((res = res->ai_next));

  if (res == NULL) {
    fprintf(stderr, "tcp_connect error %s, %s\n", host, serv);
  }

  freeaddrinfo(ressave);

  return (sockfd);
}

const int max_line = 1024;

int main(int argc, char** argv)
{
  int sockfd, n, npend;
  char recvline[max_line + 1];
  socklen_t len;
  struct sockaddr_storage ss;

  if (argc != 3) {
    fprintf(stderr, "%s <hostname/ip> <service/port>\n", argv[0]);
    exit(-1);
  }

  sockfd = tcp_connect(argv[1], argv[2]);

  len = sizeof(ss);
  if (getpeername(sockfd, (struct sockaddr *)&ss, &len) < 0) {
    fprintf(stderr, "getperrname failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(recvline, 0, sizeof(recvline));

  for ( ;; ) {
    if ((n = recv(sockfd, recvline, max_line, MSG_PEEK)) == 0)
      break; /* server closed connection */

    ioctl(sockfd, FIONREAD, &npend);
    printf("%d bytes from PEEK, %d bytes pending\n", n, npend);

    n = read(sockfd, recvline, max_line);
    recvline[n] = 0; /* null terminate */
    fputs(recvline, stdout);
  }

  return 0;
}
