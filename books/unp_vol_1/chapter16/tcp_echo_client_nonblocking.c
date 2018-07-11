/*
 * tcp echo client with nonblocking
 * nonblocking stdin, stdout and socket descriptor
 *
 * read from stdin, write to socket
 * receive from socket and write to stdout
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const int max_line = 1024;

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

char *gf_time(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;

  if (gettimeofday(&tv, NULL) < 0)
    fprintf(stderr, "gettimeofday failed\n");

  ptr = ctime(&tv.tv_sec);
  strcpy(str, &ptr[11]);

  snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);
  return (str);
}

void set_nonblocking(int fd)
{
  int val = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, val | O_NONBLOCK) == -1)
    fprintf(stderr, "set nonblocking failed: %s\n", strerror(errno));
}

void str_cli(FILE *fp, int sockfd)
{
  int maxfd, val, stdineof;
  ssize_t n, nwritten;
  fd_set rset, wset;
  char to[max_line], fr[max_line];

  /*
   * toiptr points to the next byte into which data can be read from standard input
   * tooptr points to the next byte that must be written to the socket
   */
  char *toiptr, *tooptr, *friptr, *froptr;

  set_nonblocking(sockfd);
  set_nonblocking(fileno(stdin));
  set_nonblocking(fileno(stdout));

  toiptr = tooptr = to;
  friptr = froptr = fr;
  stdineof = 0;

  maxfd = max(max(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;

  for ( ;; ) {
    FD_ZERO(&rset);
    FD_ZERO(&wset);

    if (stdineof == 0 && toiptr < &to[max_line])
      FD_SET(STDIN_FILENO, &rset);  /* read from stdin */

    if (friptr < &fr[max_line])
      FD_SET(sockfd, &rset);        /* read from socket */

    if (tooptr != toiptr)
      FD_SET(sockfd, &wset);        /* data to write to socket */

    if (froptr != friptr)
      FD_SET(STDOUT_FILENO, &wset); /* data to write to socket */

    select(maxfd, &rset, &wset, NULL, NULL);

    /* read from stdin */
    if (FD_ISSET(STDIN_FILENO, &rset)) {
      if ((n = read(STDIN_FILENO, toiptr, &to[max_line] - toiptr)) < 0) {
        if (errno != EWOULDBLOCK)
          fprintf(stderr, "read error on stdin\n");
      } else if (n == 0) {
        fprintf(stderr, "%s: EOF on stdin\n", gf_time());
        stdineof = 1;
        if (tooptr == toiptr)
          shutdown(sockfd, SHUT_WR); /* send FIN */
      } else {
        fprintf(stderr, "%s: read %ld bytes from stdin\n", gf_time(), n);
        toiptr += n;
        FD_SET(sockfd, &wset); /* try and write to socket below */
      }
    }

    /* read from socket */
    if (FD_ISSET(sockfd, &rset)) {
      if ((n = read(sockfd, friptr, &fr[max_line] - friptr)) < 0) {
        if (errno != EWOULDBLOCK)
          fprintf(stderr, "read error on socket\n");
      } else if (n == 0) {
        fprintf(stderr, "%s: EOF on socket\n", gf_time());
        if (stdineof)
          return; /* normal termination */
        else {
          fprintf(stderr, "server terminated permaturely\n");
          exit(-1);
        }
      } else {
        fprintf(stderr, "%s: read %ld bytes from socket\n", gf_time(), n);
        friptr += n;
        FD_SET(STDOUT_FILENO, &wset); /* try and write below */
      }
    }

    /* write to stdout */
    if (FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr - froptr) > 0)) {
      if ((nwritten = write(STDOUT_FILENO, froptr, n)) < 0) {
        if (errno != EWOULDBLOCK)
          fprintf(stderr, "write error to stdout");
      } else {
        fprintf(stderr, "%s: wrote %ld bytes to stdout\n", gf_time(), nwritten);
        froptr += nwritten;
        if (froptr == friptr)
          froptr = friptr = fr; /* back to beginning of buffer */
      }
    }

    /* write to socket */
    if (FD_ISSET(sockfd, &wset) && ((n = toiptr - tooptr) > 0)) {
      if ((nwritten = write(sockfd, tooptr, n)) < 0) {
        if (errno != EWOULDBLOCK)
          fprintf(stderr, "write error to socket\n");
      } else {
        fprintf(stderr, "%s: wrote %ld bytes to socket\n",
            gf_time(), nwritten);
        tooptr += nwritten;
        if (tooptr == toiptr) {
          toiptr = tooptr = to; /* back to beginning of buffer */
          if (stdineof)
            shutdown(sockfd, SHUT_WR); /* send FIN */
        }
      }
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

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  printf("connect to server successfully! %d\n", sockfd);

  str_cli(stdin, sockfd); /* do it all */

  return 0;
}
