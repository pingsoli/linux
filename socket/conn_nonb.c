// nonblocking connect()

#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define max(a,b) \
     ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })

const int max_line = 1024;
int is_continue = 1;

void siginter_handler(int signo) {
  printf("received a signal[%d]\n", signo);

  if (is_continue)
  {
    is_continue = 0;
    return;
  }

  exit(0);
}

int conn_nonb(int sockfd, const struct sockaddr_in *saptr, socklen_t salen,
    int nsec)
{
  int flags, n, error, code;
  socklen_t len;
  fd_set wset;
  struct timeval tval;

  flags = fcntl(sockfd, F_GETFL, 0);
  fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

  error = 0;
  if ((n = connect(sockfd, (struct sockaddr*)saptr, salen)) == 0) {
    goto done;
  } else if (n < 0 && errno != EINPROGRESS) {
    return -1;
  }

  // if in normal state, will print 'Operation now in progress'.
  printf("connecting to socket[%d], %s\n", sockfd, strerror(errno));

  /* Do whatever we want while the connect is taking place */
  /* .... */

  FD_ZERO(&wset);
  FD_SET(sockfd, &wset);

  tval.tv_sec = nsec;
  tval.tv_usec = 0;

  if ((n = select(sockfd + 1, NULL, &wset,
            NULL, nsec ? &tval : NULL)) == 0)
  {
    close(sockfd); /* timeout */
    errno = ETIMEDOUT;
    return -1;
  }

  if (FD_ISSET(sockfd, &wset)) {
    len = sizeof(error);
    code = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);

    if (code < 0 || error) {
      close(sockfd);
      if (error)
        errno = error;
      return -1;
    }
  } else {
    fprintf(stderr, "select error: sockfd not set");
    exit(0);
  }

done:
  fcntl(sockfd, F_SETFL, flags);
  return 0;
}


/* Encounte EOF from standard stream, we send FIN to terminate the connection */
void str_cli(FILE *fp, int sockfd)
{
  int maxfd, stdineof;
  fd_set read_set;
  char buf[max_line + 1];
  int n;

  FD_ZERO(&read_set);
  stdineof = 0;

  for ( ;; ) {
    if (stdineof == 0)
      FD_SET(fileno(fp), &read_set);
    FD_SET(sockfd, &read_set);

    maxfd = max(fileno(fp), sockfd) +  1;

    /* no timeout until the event happpens */
    select(maxfd, &read_set, NULL, NULL, NULL);

    if (FD_ISSET(sockfd, &read_set)) { /* socket is readable */
      if ((n = read(sockfd, buf, max_line)) == 0) {
        if (stdineof == 1)
          return;
        else {
          fprintf(stderr, "server termination\n");
          exit(-1);
        }
      }
      write(fileno(stdout), buf, n);
    }

    if (FD_ISSET(fileno(fp), &read_set)) { /* input is readable */
      if ((n = read(fileno(fp), buf, max_line)) == 0) {
        stdineof = 1;
        /* Ctrl-D stands for EOF */
        printf("terminating connction between self and server\n");
        shutdown(sockfd, SHUT_WR); /* send FIN */
        FD_CLR(fileno(fp), &read_set);
        continue;
      }
      write(sockfd, buf, n);
    }
  }
}

int main(int argc, char** argv)
{
  struct sockaddr_in servaddr;
  int sockfd;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
    exit(-1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  // just for debugging
  signal(SIGINT, siginter_handler);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if (conn_nonb(sockfd, &servaddr, sizeof(servaddr), 2) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  printf("connect to server successfully! %d\n", sockfd);

  close(sockfd); // close the socket connection
  exit(0);

  str_cli(stdin, sockfd); /* do it all */

  return 0;
}
