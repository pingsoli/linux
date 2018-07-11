#include <errno.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "interface_info.h"

int main(int argc, char** argv)
{
  int sockfd;
  struct ifi_info *ifi;
  unsigned char *ptr;
  struct arpreq arpreq;
  struct sockaddr_in *sin;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  for (ifi = get_ifi_info(AF_INET, 0);
      ifi != NULL; ifi = ifi->ifi_next)
  {
    printf("%s: ", sock_ntop_host(ifi->ifi_addr, sizeof(struct sockaddr_in)));

    sin = (struct sockaddr_in *) &arpreq.arp_pa;
    memcpy(sin, ifi->ifi_addr, sizeof(struct sockaddr_in));

    if (ioctl(sockfd, SIOCGARP, &arpreq) < 0) {
      fprintf(stderr, "ioctl failed: %s\n", strerror(errno));
      exit(-1);
    }

    ptr = &arpreq.arp_ha.sa_data[0];
    printf("%x:%x:%x:%x:%x:%x\n", *ptr, *(ptr + 1),
        *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
  }

  return 0;
}
