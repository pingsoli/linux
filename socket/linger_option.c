/*
 * in normal case, one side active-close and receives ACK from other side,
 * then enter TIME-WAIT state, usually lasting 2MSL. if we enable SO_LINGER option,
 * and set the timeout to 0, we will not enter TIME-WAIT state rather close
 * the connection immediately.
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
  short port = 12345;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    ERR_EXIT("socket failed");

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);

  printf("server is starting using %d port\n", port);

  /* reuse address */
  int reuse_addr = 0;

  /* SO_LINGER option */
  struct linger lin;
  lin.l_onoff = 1;
  lin.l_linger = 0;

  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
  setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));

  if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    ERR_EXIT("bind failed");

  if (listen(listenfd, 2) < 0)
    ERR_EXIT("listen failed");

  if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0)
    ERR_EXIT("accept failed");

  close(connfd); // set linger timeout to 0, close the connection immediately.

  return 0;
}
