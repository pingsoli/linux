/*
 * broadcast
 * prerequisites: other hosts is running NTP server.
 */
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

const int max_line = 1024;

void recvfrom_alarm(int signo) {
  printf("alarm timeout\n");
  return;
}

/* Only for IPv4 */
const char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen) {
  static char str[128];

  struct sockaddr_in *sin = (struct sockaddr_in *) sa;
  if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
    return NULL;
  return str;
}

void dg_cli(FILE *fp, int sockfd,
    const struct sockaddr *pservaddr, socklen_t servlen)
{
  int n;
  const int on = 1;
  char sendline[max_line], recvline[max_line];
  socklen_t len;
  struct sockaddr *preply_addr;

  preply_addr = (struct sockaddr *) malloc(servlen);

  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on ,sizeof(on));

  signal(SIGALRM, recvfrom_alarm);

  while (fgets(sendline, max_line, fp) != NULL) {
    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    alarm(5);
    for ( ;; ) {
      len = servlen;
      n = recvfrom(sockfd, recvline, max_line, 0, preply_addr, &len);
      if (n < 0) {
        if (errno == EINTR)
          break; /* waited long enough for replies */
        else
          fprintf(stderr, "recvfrom error\n");
      } else {
        recvline[n] = '\0';
        printf("from %s: %s",
            sock_ntop_host(preply_addr, len), recvline);
      }
    }
  }

  free(preply_addr);
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <ip>\n", argv[0]);
    exit(-1);
  }

  int sockfd;
  struct sockaddr_in servaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(13); /* daytime */
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  dg_cli(stdin, sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

  return 0;
}
