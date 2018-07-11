/*
 * sockaddr_un structure
 * +--------+----------+
 * | length | AF_LOCAL | 2 bytes
 * +--------+----------+
 * | pathname          |
 * | up to 104 bytes   |
 * |      .....        |
 * +-------------------+
 * get unix domain socket information
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  int sockfd;
  socklen_t len;
  struct sockaddr_un addr1, addr2;

  if (argc != 2) {
    fprintf(stderr, "unixbind <pathname>\n");
    exit(-1);
  }

  if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed\n");
    exit(-1);
  }

  unlink(argv[1]);

  memset(&addr1, 0, sizeof(addr1));
  addr1.sun_family = AF_LOCAL;
  strncpy(addr1.sun_path, argv[1], sizeof(addr1.sun_path) - 1);
  if (bind(sockfd, (struct sockaddr*)&addr1, SUN_LEN(&addr1)) < 0) {
    fprintf(stderr, "bind failed\n");
    exit(-1);
  }

  len = sizeof(addr2);
  getsockname(sockfd, (struct sockaddr*)&addr2, &len);
  printf("bound name = %s, returned len = %d\n", addr2.sun_path, len);

  return 0;
}
