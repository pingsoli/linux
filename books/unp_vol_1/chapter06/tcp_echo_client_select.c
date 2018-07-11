/*
 * TCP echo client using select
 */
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define max(a,b) \
     ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })

const int max_line = 1024;

////////////////////////////////////////////////////////////////////////////////
/*
 * close VS shutdown socket ?
 * shutdown is a flexiable way to block communication in one or both directions.
 * when the second parameter is SHUT_RDWR, it will block both sending and
 * receiving (like close). However, close is the way to actually destroy a
 * socket.
 */

//void str_cli(FILE *fp, int sockfd)
//{
//  int maxfd;
//  fd_set read_set;
//  char sendline[20], recvline[20];
//  int nbytes_sent, nbytes_recv;
//
//  FD_ZERO(&read_set);
//  memset(sendline, 0, sizeof(sendline));
//  memset(recvline, 0, sizeof(recvline));
//
//  for ( ;; ) {
//    FD_SET(fileno(fp), &read_set);
//    FD_SET(sockfd, &read_set);
//
//    maxfd = max(fileno(fp), sockfd) + 1;
//    select(maxfd, &read_set, NULL, NULL, NULL);
//
//    if (FD_ISSET(sockfd, &read_set)) {
//      if ((nbytes_recv = read(sockfd, recvline, sizeof(recvline))) == 0) {
//        fprintf(stderr, "read failed: %s\n", strerror(errno));
//        exit(-1);
//      }
//
//      printf("%s", recvline);
//      memset(recvline, 0, sizeof(recvline));
//    }
//
//    if (FD_ISSET(fileno(fp), &read_set)) {
//      if (fgets(sendline, sizeof(sendline), fp) == NULL)
//        return; /* all done */
//
//      nbytes_sent = write(sockfd, sendline, strlen(sendline));
//      memset(sendline, 0, sizeof(sendline));
//    }
//  }
//}

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
