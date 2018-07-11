/*
 * simple implementation inet_ntop for ipv4
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

const char*
inet_ntop_4(int family, const void *addrptr, char *strptr, size_t len)
{
  const u_char *p = (const u_char*) addrptr;

  if (family = AF_INET) {
    char temp[INET_ADDRSTRLEN];

    snprintf(temp, sizeof(temp), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    if (strlen(temp) >= len) {
      errno = ENOSPC;
      return NULL;
    }

    strcpy(strptr, temp);
    return strptr;
  }
  errno = EAFNOSUPPORT;
  return NULL;
}

int main(int argc, char** argv)
{
  u_char buff[sizeof(struct in_addr)];

  /* 0.0.0.0 */
  memset(buff, 0, sizeof(struct in_addr));

  buff[0] = 192;
  buff[1] = 168;
  buff[2] = 0;
  buff[3] = 0;
  /* 192.168.0.0 */

  char str[INET_ADDRSTRLEN];
  printf("ip address presentation: %s\n",
      inet_ntop_4(AF_INET, buff, str, INET_ADDRSTRLEN));

  return 0;
}
