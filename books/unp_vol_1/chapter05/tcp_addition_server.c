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

ssize_t read_line(int fd, void *vptr, size_t maxlen)
{
  ssize_t n, rc;
  char    c, *ptr;

  ptr = vptr;
  for (n = 1; n < maxlen; ++n) {
again:
    if ((rc = read(fd, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;        /* newline is stored, like fgets() */
    } else if (rc == 0) {
      *ptr = 0;
      return (n - 1); /* EOF, n - 1 bytes were read */
    } else {
      if (errno == EINTR)
        goto again;
      return -1;      /* error, errno set by read() */
    }
  }

  *ptr = 0;           /* null termination like fgets() */
  return n;
}

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

void str_echo(int sockfd) {
  long    arg1, arg2;
  ssize_t n;
  char    line[1024];

  memset(line, 0, sizeof(line));
  for ( ;; ) {
    read_line(sockfd, line, 1024);

    printf("receive from client: %s\n", line);

    if (sscanf(line, "%ld%ld", &arg1, &arg2) == 2)
      snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
    else
      snprintf(line, sizeof(line), "input error\n");

    n = strlen(line);
    if (write(sockfd, line, n) < 0) {
      fprintf(stderr, "write failed: %s\n", strerror(errno));
      exit(-1);
    }
  }
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
