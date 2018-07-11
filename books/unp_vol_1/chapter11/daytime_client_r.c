/*
 * daytime client revision for supporting IPv4 and IPv6
 *
 * daytime server information
 * https://tf.nist.gov/tf-cgi/servers.cgi
 */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

const int max_line = 1024;

void sock_set_port(struct sockaddr *sa, socklen_t addrlen, int port) {
  switch (sa->sa_family)
  {
  case AF_INET:
    {
      struct sockaddr_in *sin = (struct sockaddr_in *)sa;
      sin->sin_port = port;
      return;
    }
  case AF_INET6:
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
      sin6->sin6_port = port;
      return;
    }
  }
}

void sock_set_addr(struct sockaddr *sa, socklen_t addrlen, void *addr) {
  switch (sa->sa_family) {
  case AF_INET:
    {
      struct sockaddr_in *sin = (struct sockaddr_in *)sa;
      memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
      return;
    }
  case AF_INET6:
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
      memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
      return;
    }
  }
}

int main(int argc, char** argv)
{
  int sockfd, n;
  char recvline[max_line + 1];
  struct sockaddr_in servaddr;
  struct sockaddr_in6 servaddr6;
  struct sockaddr *sa;
  socklen_t salen;
  struct in_addr **pptr;
  struct hostent *hp;
  struct servent *sp;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <hostname> <service>\n", argv[0]);
    exit(-1);
  }

  if((hp = gethostbyname(argv[1])) == NULL) {
    fprintf(stderr, "gethostbyname failed %s\n", argv[1]);
    exit(-1);
  }

  if ((sp = getservbyname(argv[2], "tcp")) == NULL) {
    fprintf(stderr, "getservbyname failed: %s\n", argv[2]);
  }

  pptr = (struct in_addr**) hp->h_addr_list;
  for ( ; *pptr != NULL; pptr++) {
    sockfd = socket(hp->h_addrtype, SOCK_STREAM, 0);

    if (hp->h_addrtype == AF_INET) {
      sa = (struct sockaddr *)&servaddr;
      salen = sizeof(servaddr);
    } else if (hp->h_addrtype == AF_INET6) {
      sa = (struct sockaddr *)&servaddr6;
      salen = sizeof(servaddr6);
    } else {
      fprintf(stderr, "unknown addrtype %d\n", hp->h_addrtype);
      exit(-1);
    }

    memset(sa, 0, salen);
    sa->sa_family = hp->h_addrtype;
    sock_set_port(sa, salen, sp->s_port);
    sock_set_addr(sa, salen, *pptr);

    if (connect(sockfd, sa, salen) == 0)
      break; /* success */

    fprintf(stderr, "connect error\n");
    close(sockfd);
  }

  if (*pptr == NULL) {
    fprintf(stderr, "unable to connect\t");
    exit(-1);
  }

  while ((n = read(sockfd, recvline, max_line)) > 0) {
    recvline[max_line] = 0;
    fputs(recvline, stdout);
  }

  return 0;
}

