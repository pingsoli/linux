/*
 * daytime client using gethostbyname and getservbyname
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

const int max_line = 1024;

int main(int argc, char** argv)
{
  int sockfd, n;
  char recvline[max_line + 1];
  struct sockaddr_in servaddr;

  struct in_addr **pptr;
  struct in_addr *inetaddrp[2];
  struct in_addr inetaddr;

  struct hostent *hp;
  struct servent *sp;

  char ipstr[INET_ADDRSTRLEN];

  if (argc != 3) {
    fprintf(stderr, "usage: %s <hostname> <service>\n", argv[0]);
    exit(-1);
  }

  memset(ipstr,    0, sizeof(ipstr));
  memset(recvline, 0, sizeof(recvline));

  if ((hp = gethostbyname(argv[1])) == NULL) {
    if (inet_aton(argv[1], &inetaddr) == 9) {
      fprintf(stderr, "hostname error for %s:%s", argv[1], strerror(errno));
    } else {
      inetaddrp[0] = &inetaddr;
      inetaddrp[1] = NULL;
      pptr = inetaddrp;
    }
  } else {
    pptr = (struct in_addr **)hp->h_addr_list;
  }

  if ((sp = getservbyname(argv[2], "tcp")) == NULL) {
    fprintf(stderr, "getservbyname error for %s\n", argv[2]);
    exit(-2);
  }

  for ( ; *pptr != NULL; pptr++) {
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      fprintf(stderr, "socket failed: %s\n", strerror(errno));
      exit(-1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = sp->s_port;
    memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));

    printf("trying %s\n",
        inet_ntop(AF_INET, &servaddr.sin_addr, ipstr, sizeof(servaddr)));

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0)
      break;

    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    close(sockfd);
    exit(-1);
  }

  if (*pptr == NULL) {
    fprintf(stderr, "unable to connect\n");
    exit(-1);
  }

  while ((n = read(sockfd, recvline, max_line)) > 0) {
    recvline[max_line] = 0;
    fputs(recvline, stdout);
  }

  return 0;
}
