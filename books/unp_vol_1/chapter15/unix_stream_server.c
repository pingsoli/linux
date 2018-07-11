#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>

#define UNIX_STR_PATH "/tmp/unix_str.sock"
const int max_line = 1024;

void sig_child(int signo) {
  printf("child process terminated\n");
  return;
}

void str_echo(int sockfd) {
  ssize_t n;
  char buf[max_line + 1];

again:
  while ((n = read(sockfd, buf, max_line)) > 0)
    write(sockfd, buf, n);

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0) {
    fprintf(stderr, "read failed\n");
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int listenfd, connfd;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_un cliaddr, servaddr;

  if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;
  strcpy(servaddr.sun_path, UNIX_STR_PATH);

  if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (listen(listenfd, 32) < 0) {
    fprintf(stderr, "listen failed: %s\n", strerror(errno));
    exit(-1);
  }

  signal(SIGCHLD, sig_child);

  for ( ;; ) {
    clilen = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0) {
      if (errno == EINTR)
        continue;     /* back to for() */
      else {
        fprintf(stderr, "accept failed\n");
        exit(-1);
      }
    }

    if ((childpid = fork()) == 0) { /* child process */
      close(listenfd); /* close listening socket */
      str_echo(connfd); /* process request */
      exit(0);
    }

    close(connfd); /* parent process connected socket */
  }

  return 0;
}
