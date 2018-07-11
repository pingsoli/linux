#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int tcp_connect(const char *host, const char *serv)
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo failed(%s, %s): %s\n",
        host, serv, gai_strerror(errno));
    exit(-1);
  }
  ressave = res;

  do {
    sockfd = socket(res->ai_family , res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
      continue;     /* ignore this one */

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break;        /* success */

    close(sockfd);  /* ignore this one */
  } while ((res = res->ai_next));

  if (res == NULL) {
    fprintf(stderr, "tcp_connect error %s, %s\n", host, serv);
  }

  freeaddrinfo(ressave);

  return (sockfd);
}

int main(int argc, char *argv[])
{
  int sockfd, n;

  if (argc != 3) {
    fprintf(stderr, "usage: tcp_client <host> <port>\n");
    exit(-1);
  }

  sockfd = tcp_connect(argv[1], argv[2]);

  /*
   * what if the receiver and sender buff not equal ?
   */
  /* int size = 32768; */
  /* char buff[16384]; */
  /* setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)); */
  /*  */
  /* memset(buff, 1, sizeof(buff)); */
  /* n = write(sockfd, buff, 16384); */
  /* if (n >= 0) { */
  /*   printf("sleep for 5 seconds\n"); */
  /*   sleep(5); */
  /* } */
  /*  */
  /* n = send(sockfd, "a", 1, MSG_OOB); */
  /* if (n >= 0) { */
  /*   printf("wrote 1 byte of OOB data\n"); */
  /* } */
  /*  */
  /* n = write(sockfd, buff, 1024); */
  /* if (n >= 0) */
  /*   printf("wrote 1024 bytes of normal data\n"); */

  n = write(sockfd, "123", 3);
  if (n >= 0) {
    printf("wrote 3 bytes of normal mode\n");
    sleep(1);
  }

  n = send(sockfd, "4", 1, MSG_OOB);
  if (n >= 0) {
    printf("wrote 1 byte of OOB data\n");
    sleep(1);
  }

  n = write(sockfd, "56", 2);
  if (n >= 0) {
    printf("wrote 2 bytes of normal data\n");
    sleep(1);
  }

  n = send(sockfd, "7", 1, MSG_OOB);
  if (n >= 0) {
    printf("wrote 1 byte of OOB data\n");
    sleep(1);
  }

  n = send(sockfd, "0", 1, MSG_OOB);
  if (n >= 0) {
    printf("wrote 1 byte of OOB data\n");
    sleep(1);
  }

  n = write(sockfd, "89", 2);
  if (n >= 0) {
    printf("wrote 2 bytes of normal data\n");
    sleep(1);
  }

  return 0;
}
