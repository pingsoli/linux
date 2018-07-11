/*
 * echo server using unix domain socket
 */
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define UNIX_DG_PATH "/tmp/unix.dg"
const int max_line = 1024;

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t pclilen) {
  int n;
  socklen_t len;
  char msg[max_line + 1];

  for ( ;; ) {
    len = pclilen;
    n = recvfrom(sockfd, msg, max_line, 0, pcliaddr, &len);
    sendto(sockfd, msg, n, 0, pcliaddr, len);
  }
}

int main(int argc, char** argv)
{
  int sockfd;
  struct sockaddr_un servaddr, cliaddr;

  if ((sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  unlink(UNIX_DG_PATH);
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;
  strcpy(servaddr.sun_path, UNIX_DG_PATH);

  if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "bind failed\n");
    exit(-1);
  }

  dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

  return 0;
}
