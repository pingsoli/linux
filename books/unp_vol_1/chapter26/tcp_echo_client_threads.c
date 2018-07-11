/*
 * TCP echo client using threads
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

const int max_line = 1024;

static int sockfd;
static FILE *fp;

void *copyto(void *arg) {
  char sendline[max_line];

  while (fgets(sendline, max_line, fp) != NULL)
    write(sockfd, sendline, strlen(sendline));

  shutdown(sockfd, SHUT_WR);
  return NULL; /* return when EOF on stdin */
}

int read_line(int sockfd, char *dst, ssize_t nbytes) {
  int nleft = nbytes;
  int nread = 0;
  char *ptr = dst;

  while (nleft > 0) {
    nread = read(sockfd, ptr, nleft);

    if (nread == 0)
      break;
    else if (nread < 0 && errno == EINTR)
      nread = 0;
    else if (nread < 0) {
      fprintf(stderr, "read failed: %s\n", strerror(errno));
      exit(-1);
    }

    nleft -= nread;
    ptr += nread;
  }

  return (ptr - dst);
}

void str_cli(FILE *fp, int sockfd) {
  char sendline[1024], recvline[1024];

  while (fgets(sendline, 1024, fp) != NULL) {
    write(sockfd, sendline, strlen(sendline));

    /* read a line from server */
    read_line(sockfd, recvline, strlen(sendline));

    fputs(recvline, stdout);

    memset(sendline, 0, sizeof(sendline));
    memset(recvline, 0, sizeof(recvline));
  }
}

int main(int argc, char** argv)
{
  struct sockaddr_in servaddr;
  int sockfd;
  int sockfds[5];

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

  printf("conneted to server...\n");
  str_cli(stdin, sockfd); /* do it all */

  return 0;
}
