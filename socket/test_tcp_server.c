#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define check(expr) if (!(expr)) { printf(#expr\
    "\n ERROR: %s\n", strerror(errno)); exit(1); }

void broken_pipe_hander(int signo) {
  printf("broken pipe\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  struct sockaddr_in servaddr;
  int listenfd;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port        = htons(12345);

  listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  check(listenfd != -1);

  int reuse_addr = 1;
  check(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int)) != -1);
  check(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != -1);
  check(listen(listenfd, 10) != -1);

  struct sockaddr_in cliaddr;
  socklen_t clilen;
  char cliip[64] = {0};
  int clifd;
  char buf[1024] = {0};

  clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
  check(clifd != -1);

  inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
  printf("new connection[%d] from %s:%d\n", clifd, cliip, ntohs(cliaddr.sin_port));

  getchar();
  int ret = -1;
  /* int ret = read(clifd, buf, sizeof(buf)); */
  strcpy(buf, "hello world");

  ret = write(clifd, buf, strlen(buf) + 1);
  printf("return from %d\n", ret);

  signal(SIGPIPE, &broken_pipe_hander);
  ret = write(clifd, buf, strlen(buf) + 1);
  printf("return from %d\n", ret);

  return 0;
}
