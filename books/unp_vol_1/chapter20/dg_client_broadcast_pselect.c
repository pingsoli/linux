/*
 * pselect() function to solve race condition
 * recvfrom will block forever without response-datagram.
 */
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>

const int max_line = 1024;

void recvfrom_alarm(int signum) {
  printf("alarm timeout\n");
  return;
}

void dg_cli(FILE *fp, int sockfd,
    const struct sockaddr* pservaddr, socklen_t servlen)
{
  int n;
  const int on = 1; /* enable broadcast */
  char sendline[max_line], recvline[max_line];
  fd_set rset;
  sigset_t sigset_alarm, sigset_empty;
  socklen_t len;
  struct sockaddr *preply_addr;

  preply_addr = malloc(servlen);

  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

  FD_ZERO(&rset);

  sigemptyset(&sigset_empty);
  sigemptyset(&sigset_alarm);
  sigaddset(&sigset_alarm, SIGALRM);

  signal(SIGALRM, recvfrom_alarm);

  while (fgets(sendline, max_line, fp) != NULL) {
    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    sigprocmask(SIG_BLOCK, &sigset_alarm, NULL);
    alarm(5);
    for ( ; ; ) {
      FD_SET(sockfd, &rset);

      /*
       * pselect() prevent race condition.
       * pselect() does blocking/unblocking around select().
       */
      n = pselect(sockfd + 1, &rset, NULL, NULL, NULL, &sigset_empty);
      if (n < 0) {
        if (errno == EINTR)
          break;
        else
          fprintf(stderr, "pselect error\n");
      } else if (n != 1) {
        fprintf(stderr, "pselect error: returned %d\n", n);
      }

      len = servlen;
      n = recvfrom(sockfd, recvline, max_line, 0, preply_addr, &len);
      recvline[n] = 0; /* null terminate */
      printf("receive from socket %d, %d bytes\n", sockfd, n);
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
