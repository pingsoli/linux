/*
 * blocking and unblocking the signal using ipc
 *
 * pipefd[2], pipefd[0] used for read, pipefd[1] used for write.
 * when alarm signal arrives, and write data to pipe[1] and select will moniter
 * the events, and invoke the correct function to avoid blocking forever.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#define max(x,y) ({                  \
    typeof(x) _max1 = (x);           \
    typeof(y) _max2 = (y);           \
    (void) (&_max1 == &_max2);       \
    _max1 > _max2 ? _max1 : _max2; })

static int pipefd[2];
const int max_line = 1024;

void recvfrom_alarm(int signum) {
  printf("alarm expire...\n");
  write(pipefd[1], "", 1);
}

void dg_cli(FILE *fp, int sockfd,
    const struct sockaddr *pservaddr, socklen_t servlen)
{
  int n, maxfd;
  const int on = 1;
  char sendline[max_line], recvline[max_line + 1];
  fd_set rset;
  socklen_t len;
  struct sockaddr *preply_addr;

  preply_addr = malloc(servlen);

  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

  pipe(pipefd);
  maxfd = max(sockfd, pipefd[0]) + 1;

  FD_ZERO(&rset);
  signal(SIGALRM, recvfrom_alarm);

  while (fgets(sendline, max_line, fp) != NULL) {
    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    alarm(5);
    for ( ; ; ) {
      FD_SET(sockfd, &rset);
      FD_SET(pipefd[0], &rset);

      if ((n = select(maxfd, &rset, NULL, NULL, NULL)) < 0) {
        if (errno == EINTR)
          continue;
        else
          fprintf(stderr, "select error\n");
      }

      if (FD_ISSET(sockfd, &rset)) {
        len = servlen;
        n = recvfrom(sockfd, recvline, max_line, 0, preply_addr, &len);

        recvline[n] = 0;
        printf("receive from socket %d, %d bytes\n", sockfd, n);
      }

      if (FD_ISSET(pipefd[0], &rset)) {
        read(pipefd[0], &n, 1); /* time expired */
        break;
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
