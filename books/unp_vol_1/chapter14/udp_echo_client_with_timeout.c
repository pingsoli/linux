/*
 * udp_client with timeout function
 * 1) alarm() function to implement dg_cli()
 * 2) select()
 * 3) socket options
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
#include <sys/select.h>

const int max_line = 1024;

////////////////////////////////////////////////////////////////////////////////
/* alarm() to implement timeout function */
//void sig_alarm(int signo) {
//  printf("alarm timeout\n");
//  return; /* just interrupt the recvfrom */
//}
//
//void dg_cli(FILE *fp, int sockfd, const struct sockaddr* pservaddr,
//    socklen_t servlen)
//{
//  int n;
//  char recvline[max_line + 1], sendline[max_line + 1];
//
//  signal(SIGALRM, sig_alarm);
//
//  while (fgets(sendline, max_line, fp) != NULL) {
//    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
//
//    alarm(5);
//    if ((n = recvfrom(sockfd, recvline, max_line, 0, NULL, NULL)) < 0) {
//      if (errno == EINTR)
//        fprintf(stderr, "socket timeout\n");
//      else
//        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
//    } else {
//      alarm(0);
//      recvline[n] = 0;
//      fputs(recvline, stdout);
//    }
//  }
//}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/* select() with timeout function */
//int readable_timeo(int fd, int sec) {
//  fd_set rset;
//  struct timeval tv;
//
//  FD_ZERO(&rset);
//  FD_SET(fd, &rset);
//
//  tv.tv_sec = sec;
//  tv.tv_usec = 0;
//
//  return (select(fd + 1, &rset, NULL, NULL, &tv));
//  /* > 0 if descriptor is readable */
//}
//
//void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr,
//    socklen_t servlen)
//{
//  int n;
//  char sendline[max_line + 1], recvline[max_line + 1];
//
//  while (fgets(sendline, max_line, fp) != NULL) {
//    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
//
//    if (readable_timeo(sockfd, 5) == 0) {
//      fprintf(stderr, "socket tiemout\n");
//    } else {
//      n = recvfrom(sockfd, recvline, max_line, 0, NULL, NULL);
//      recvline[n] = 0; /* null terminate */
//      fputs(recvline, stdout);
//    }
//  }
//}
////////////////////////////////////////////////////////////////////////////////
/* SO_RCVTIMEO options for socket */
void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr,
    socklen_t servlen)
{
  int n;
  char sendline[max_line + 1], recvline[max_line + 1];
  struct timeval tv;

  tv.tv_sec = 5;
  tv.tv_usec = 0;

  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
    fprintf(stderr, "setsockopt recv timeout failed\n");
    exit(-1);
  }

  while (fgets(sendline, max_line, fp) != NULL) {
    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    n = recvfrom(sockfd, recvline, max_line, 0, NULL, NULL);
    if (n < 0) {
      if (errno = EWOULDBLOCK) {
        fprintf(stderr, "socket timeout\n");
        continue;
      } else {
        fprintf(stderr, "recvfrom failed\n");
        exit(-1);
      }
    }

    recvline[max_line] = 0; /* null terminate */
    fputs(recvline, stdout);
  }

}
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  struct sockaddr_in servaddr;
  socklen_t servlen;
  int sockfd, n;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  if (inet_pton(servaddr.sin_family,
        argv[1], &servaddr.sin_addr) != 1) {
    fprintf(stderr, "inet_pton failed\n");
    exit(-1);
  }

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed\n");
    exit(-1);
  }

  servlen = sizeof(servaddr);
  dg_cli(stdin, sockfd, (struct sockaddr*) &servaddr, servlen);

  return 0;
}
