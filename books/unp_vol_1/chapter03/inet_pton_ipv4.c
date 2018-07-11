/*
 * simulate inet_pton function
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

/* Only support ipv4 */
int inet_pton_4(int family, const char *strptr, void *addrptr)
{
  if (family == AF_INET) {
    struct in_addr in_val;

    if (inet_aton(strptr, &in_val)) {
      memcpy(addrptr, &in_val, sizeof(struct in_addr));
      return 1;
    }
    return 0;
  }

  errno = EAFNOSUPPORT;
  return -1;
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <ip-address>\n", argv[0]);
    exit(-1);
  }

  unsigned char buf[sizeof(struct in_addr)];
  int domain = AF_INET;

  memset(buf, 0, sizeof(struct in_addr));

  if (inet_pton_4(domain, argv[1], buf) < 0) {
    fprintf(stderr, "inet_pton failed\n");
    exit(-1);
  }

  /* show final ip address using inet_ntop function */
  char str[INET_ADDRSTRLEN];
  printf("final ip address: %s\n", inet_ntop(domain, buf, str, INET_ADDRSTRLEN));

  return 0;
}
