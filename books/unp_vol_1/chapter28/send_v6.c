#include <sys/time.h>
#include "ping.h"

void send_v6()
{
  int len;
  struct icmp6_hdr *icmp6;

  icmp6 = (struct icmp6_hdr *) sendbuf;
  icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
  icmp6->icmp6_code = 0;
  icmp6->icmp6_id = pid;
  icmp6->icmp6_seq = nsent++;
  memset((icmp6 + 1), 0xa5, datalen);

  gettimeofday((struct timeval *) (icmp6 + 1), NULL);

  len = 8 + datalen;

  sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
}
