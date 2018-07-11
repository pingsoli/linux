/*
 * Simple tcp client
 * use tcpdump command to capture the datagram
 * tcpdump -i lo port 13000
 * tcpdump -i ens33 port 13000
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

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
//    printf("send %d bytes\n", strlen(sendline));

    /* read a line from server */
    read_line(sockfd, recvline, strlen(sendline));
//    printf("recv %d bytes\n", strlen(recvline));

    fputs(recvline, stdout);

    memset(sendline, 0, sizeof(sendline));
    memset(recvline, 0, sizeof(recvline));
  }
}

/*
 * the server child process has been killed, and the client send data
 * to server-child process. This would generate SIGPIPE (RST is received).
 */
void sigpipe_handler(int signum) {
  printf("handle SIGPIPE signal\n");
  return;
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

  //for (int i = 0; i < 5; ++i) {
    //if ((sockfds[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      //fprintf(stderr, "socket failed: %s\n", strerror(errno));
      //exit(-1);
    //}

    //memset(&servaddr, 0, sizeof(servaddr));
    //servaddr.sin_family = AF_INET;
    //servaddr.sin_port = htons(atoi(argv[2]));
    //inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    //if (connect(sockfds[i], (struct sockaddr*)& servaddr, sizeof(servaddr)) < 0)
    //{
      //fprintf(stderr, "connect failed: %s\n", strerror(errno));
      //exit(-1);
    //}
  //}

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  signal(SIGPIPE, sigpipe_handler);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  str_cli(stdin, sockfd); /* do it all */

  return 0;
}
