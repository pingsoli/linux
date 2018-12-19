/*
 * Simple echo server
 * telnet use line buffer on linux platform. but windows not.
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

void str_echo(int sockfd) {
  ssize_t n;
  char buf[1024];

again:
  while ((n = read(sockfd, buf, 1024)) > 0) {
    if (write(sockfd, buf, n) < 0) {
      fprintf(stderr, "write failed: %s\n", strerror(errno));
      exit(-1);
    }
  }

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0) {
    fprintf(stderr, "read failed: %s\n", strerror(errno));
    exit(-1);
  }
}

/* incorrect version of SIGCHLD handler */
//void sigchld_handler(int signum) {
//  pid_t pid;
//  int state;
//
//  pid = wait(&state);
//  printf("child %d termination\n", pid);
//  return;
//}

/* correct version of SIGCHLD functions call waitpid */
void sigchld_handler(int signum) {
  pid_t pid;
  int stat;

  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d termination\n", pid);
  return;
}

void sigint_handler(int signum) {
  printf("SIGINT signal: %d\n", signum);
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

  /* register signal handler */
  signal(SIGCHLD, sigchld_handler);
//  signal(SIGINT, sigint_handler);

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
    if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {

      /* Handle interrupted systems calls */
      if (errno == EINTR)
        continue;
      else {
        fprintf(stderr, "accept failed: %s\n", strerror(errno));
        exit(-1);
      }
    }

    inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, clilen);
    printf("connection %d from %s, port %d\n",
        connfd, cliip, ntohs(cliaddr.sin_port));

    if ((childpid = fork()) == 0) { /* child process */
      close(listenfd); /* close listening socket */
      str_echo(connfd); /* process the request */
      exit(0);
    }

    close(connfd);
  }

  return 0;
}
