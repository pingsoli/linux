/*
 * simple udp echo client
 * recvfrom will block forever when one packet is lost
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

const int max_line = 1024;

/*
 * unconnected UDP
 * every sending datagram needs connect and unconnect operations
 */
void dg_cli(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t servlen)
{
  int n;
  char sendline[max_line], recvline[max_line];
  socklen_t len;
  struct sockaddr *preply_addr;
  char ipstr[INET_ADDRSTRLEN];

  while (fgets(sendline, max_line, fp) != NULL) {
    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    len = servlen;
    /* NOTE: if the server crash down, recvfrom will block forever */
    if ((n = recvfrom(sockfd, recvline, max_line, 0, preply_addr, &len)) < 0) {
     /* if (errno == EAGAIN || errno == EWOULDBLOCK) { */
     /*   printf("resent again\n"); */
     /*   continue; */
     /* } else { */
     /*   fprintf(stderr, "recvfrom failed: %s\n", strerror(errno)); */
     /*   exit(-1); */
     /* } */

      fprintf(stderr, "recvfrom failed: %s\n", strerror(errno));
      exit(-1);
    }

   /* if (len != servlen || memcmp(pservaddr, preply_addr, len) != -1) { */
   /*   printf("reply ignored\n"); */
   /*   continue; */
   /* } */

    recvline[n] = 0;
    fputs(recvline, stdout);
  }
}

/*
 * connected UDP
 * use connect twice to unconnect UDP.
 */
/* void dg_cli(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t servlen) */
/* { */
/*  int n; */
/*  char sendline[max_line], recvline[max_line]; */
/*  */
/*  if (connect(sockfd, (struct sockaddr*) pservaddr, servlen) < 0) { */
/*    fprintf(stderr, "connect failed: %s\n", strerror(errno)); */
/*    exit(-1); */
/*  } */
/*  */
/*  while (fgets(sendline, max_line, fp) != NULL) { */
/*    if (write(sockfd, sendline, strlen(sendline)) < 0) { */
/*      fprintf(stderr, "write failed: %s\n", strerror(errno)); */
/*      exit(-1); */
/*    } */
/*  */
/*    n = read(sockfd, recvline, max_line); */
/*    if (n < 0) { */
/*      fprintf(stderr, "read failed: %s\n", strerror(errno)); */
/*      exit(-1); */
/*    } */
/*  */
/*    recvline[n] = 0; */
/*    fputs(recvline, stdout); */
/*  } */
/* } */

int main(int argc, char** argv)
{
  int sockfd;
  struct sockaddr_in servaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  /*
   * set the udp socket nonblocking, recvfrom will not block.
   * but it's not a good idea.
   */
//  int flags;
//  if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) {
//    fprintf(stderr, "fcntl failed: %s\n", strerror(errno));
//    exit(-1);
//  }
//
//  flags |= O_NONBLOCK;
//  if (fcntl(sockfd, F_SETFL, flags) < 0) {
//    fprintf(stderr, "fcntl failed: %s\n", strerror(errno));
//    exit(-1);
//  }

  dg_cli(stdin, sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

  return 0;
}
