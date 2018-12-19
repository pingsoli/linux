/*
 * Check socket whether is writeable ?
 * use select linux API to do it.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR_EXIT(msg) \
  do { \
    fprintf(stderr, "ERROR[%d]: %s, %s\n", errno, msg, strerror(errno));\
    exit(-1);  \
  } while(0)

int main(int argc, char *argv[])
{
  int listenfd, connfd;
  struct sockaddr_in servaddr, cliaddr;
  socklen_t clilen = 0;
  char cliip[256] = { 0 };
  fd_set wset;
  struct timeval timer, tmp_timer;
  timer.tv_sec = 3;
  timer.tv_usec = 0;
  tmp_timer = timer;

  /* SO_LINGER option */
  struct linger lin;
  lin.l_onoff = 1;
  lin.l_linger = 0;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    ERR_EXIT("socket failed");

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(12345);

  // reuse address
  int reuse_addr = 0;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
  /* setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin)); */

  if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    ERR_EXIT("bind failed");

  if (listen(listenfd, 2) < 0)
    ERR_EXIT("listen failed");

  if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0)
    ERR_EXIT("accept failed");

  // close() the connction when we set SO_LINGER
  // If set SO_LINGER on, and the timeout to 0, TIME-WAIT state will be ignored.
  /* close(connfd); */
  /* return 0; */

  // print client ip address and port
  inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, clilen);
  printf("connection fd[%d] from %s port %d\n",
      connfd, cliip, ntohs(cliaddr.sin_port));

  FD_ZERO(&wset);
  FD_SET(connfd, &wset); // no more new connection for testing

  printf("Press any key to continue...\n");
  getchar();
  write(connfd, "\n", 1);
  printf("Continue to execute...\n");

  int nready = 0;
  if ((nready = select(connfd + 1, NULL, &wset, NULL, &tmp_timer)) < 0)
    ERR_EXIT("select failed");

  printf("ready fd num: %d\n", nready);
  if (nready == 0) {
    printf("no ready connection...\n");
    return 0;
  }

  printf("connection fd: %d\n", connfd);
  for (int i = 0; i < connfd + 1; ++i)
  {
    printf("current fd: %d\n", i);
    if (FD_ISSET(connfd, &wset)) {
      printf("fd[%d] is writeable\n", i);

      /* // is writable */
      /* char buf[256] = { 0 }; */
      /* strncpy(buf, "hello world", 11); */
      /*  */
      /* int ret = write(connfd, buf, sizeof(buf)); */
      /* printf("write: %d\n", ret); */
    }
  }

  return 0;
}
