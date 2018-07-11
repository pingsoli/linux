#include <arpa/inet.h>
#include "ping.h"

char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
  static char str[128];   /* Unix domain is largest */

  switch (sa->sa_family) {
    case AF_INET:
      {
        struct sockaddr_in  *sin = (struct sockaddr_in *) sa;

        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
          return(NULL);
        return(str);
      }

    case AF_INET6:
      {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;

        if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
          return(NULL);
        return(str);
      }
  }
}

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype)
{
  int n;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags    = AI_CANONNAME; /* always return canonical name */
  hints.ai_family   = family;       /* AP_UNSPCE, AF_INET, AF_INET6, etc. */
  hints.ai_socktype = socktype;     /* O, SOCK_STREAM, SOCK_DGRAM, etc. */

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    return (NULL);

  return res;
}
