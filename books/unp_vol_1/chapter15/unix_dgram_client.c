#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define UNIX_DG_PATH "/tmp/unix.dg"
const int max_line = 1024;

void dg_cli(FILE *fp, int sockfd, struct sockaddr* pservaddr, socklen_t servlen)
{
  int n;
  char sendline[max_line + 1], recvline[max_line + 1];

  while (fgets(sendline, max_line, fp) != NULL) {
    sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    n = recvfrom(sockfd, recvline, max_line, 0, NULL, NULL);

    recvline[max_line] = 0; /* null terminate */
    fputs(recvline, stdout);

    memset(sendline, 0, sizeof(sendline));
    memset(recvline, 0, sizeof(recvline));
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

  memset(&cliaddr, 0, sizeof(cliaddr));
  cliaddr.sun_family = AF_LOCAL;
  memset(cliaddr.sun_path, 0, 104);

  if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
    fprintf(stderr, "bind failed: %s\n", strerror(errno));
    exit(-1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;
  strcpy(servaddr.sun_path, UNIX_DG_PATH);

  dg_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  return 0;
}
