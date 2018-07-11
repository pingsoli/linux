/*
 * using standard io to write and read from socket stream
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int max_line = 1024;

FILE *Fdopen(int fd, const char *mode)
{
  FILE *fp;
  if ((fp = fdopen(fd, mode)) == NULL) {
    fprintf(stderr, "fdopen failed: %s\n", strerror(errno));
    exit(-1);
  }
  return fp;
}

void str_echo(int sockfd)
{
  char line[max_line + 1];
  FILE *fpin, *fpout;

  fpin = Fdopen(sockfd, "r");
  fpout = Fdopen(sockfd, "w");

  while (fgets(line, max_line, fpin) != NULL)
    fputs(line, fpout);
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  int listenfd, connfd;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  char cliip[INET_ADDRSTRLEN];
  int reuse_addr = 0;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(atoi(argv[1]));

  memset(&cliaddr, 0, sizeof(cliaddr));

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  /* prevent address already in use */
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
        &reuse_addr, sizeof(reuse_addr)) < 0) {
    fprintf(stderr, "address reuse failed\n");
  }

  if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (listen(listenfd, 10) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  for ( ;; ) {
    clilen = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {

      /* Handle interrupted systems calls */
      if (errno == EINTR)
        continue;
      else {
        fprintf(stderr, "accept failed: %s\n", strerror(errno));
        exit(-1);
      }
    }

    printf("connection %d from %s, port %d\n",
        connfd,
        inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, clilen),
        cliaddr.sin_port);

    if ((childpid = fork()) == 0) { /* child process */
      close(listenfd); /* close listening socket */
      str_echo(connfd); /* process the request */
      exit(0);
    }

    close(connfd);
  }

  return 0;
}
