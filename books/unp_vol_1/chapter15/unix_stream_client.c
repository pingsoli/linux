#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define UNIX_STR_PATH "/tmp/unix_str.sock"
const int max_line = 1024;

void str_cli(FILE *fp, int sockfd) {
  int maxfd, stdineof;
  fd_set rset;
  char buf[max_line + 1];
  int n;

  stdineof = 0;
  FD_ZERO(&rset);
  for ( ;; ) {
    if (stdineof == 0)
      FD_SET(fileno(fp), &rset);

    FD_SET(sockfd, &rset);
    maxfd = max(fileno(fp), sockfd) + 1;
    select(maxfd, &rset, NULL, NULL, NULL);

    if (FD_ISSET(sockfd, &rset)) { /* socket is readable */
      if ((n = read(sockfd, buf, max_line)) == 0) {
        if (stdineof == 1)
          return;  /* normal termination */
        else {
          fprintf(stderr, "read failed\n");
          exit(-1);
        }
      }

      write(fileno(fp), buf, n);
    }

    if (FD_ISSET(fileno(fp), &rset)) {
      if ((n = read(fileno(fp), buf, max_line)) == 0) {
        stdineof = 1;
        shutdown(sockfd, SHUT_WR); /* send FIN */
        FD_CLR(fileno(fp), &rset);
        continue;
      }

      write(sockfd, buf, n);
    }
  }
}

int main(int argc, char** argv)
{
  int sockfd;
  struct sockaddr_un servaddr;

  if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed:%s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;
  strcpy(servaddr.sun_path, UNIX_STR_PATH);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  str_cli(stdin, sockfd);

  return 0;
}
