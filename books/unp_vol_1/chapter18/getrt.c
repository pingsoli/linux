/*
 * get route table information
 */
#include <errno.h>
#include <net/route.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "route.h"

#define BUFLEN (sizeof(struct rt_msghdr) + 512)
#define SEQ    9999

int main(int argc, char** argv)
{
  int sockfd;
  char *buf;
  pid_t pid;
  ssize_t n;
  struct rt_msghdr *rtm;
  struct sockaddr *sa, *rti_info[RTAX_MAX];
  struct sockaddr_in *sin;

  if (argc != 2) {
    fprintf(stderr, "usage: getrt <ip>\n");
    exit(-1);
  }

  if ((sockfd = socket(AF_ROUTE, SOCK_RAW, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  buf = calloc(1, BUFLEN);

  rtm = (struct rt_msghdr *) buf;
  rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
  rtm->rtm_version = RTM_VERSION;
  rtm->rtm_type = RTM_GET;
  rtm->rtm_addrs = RTA_DST;
  rtm->rtm_pid = pid = getpid();
  rtm->rtm_seq = SEQ;

  sin = (struct sockaddr_in *) (rtm + 1);
  sin->sin_family = AF_INET;
  sin->sin_len = sizeof(struct sockaddr_in);
  inet_pton(AF_INET, argv[1], &sin->sin_addr);

  write(sockfd, rtm, rtm->rtm_msglen);

  do {
    n = read(sockfd, rtm, BUFLEN);
  } while (rtm->rtm_type != RTM_GET ||
           rtm->rtm_seq  != SEQ     ||
           rtm->rtm_pid  != pid);

  rtm = (struct rt_msghdr *) buf;
  sa = (struct sockaddr *) (rtm + 1);
  get_rtaddrs(rtm->rtm_addrs, sa, rti_info);

  if ((sa = rti_info[RTAX_DST]) != NULL)
    printf("dest: %s\n", sock_ntop_host(sa, sa->sa_len));

  if ((sa = rti_info[RTAX_GATEWAY]) != NULL)
    printf("gateway: %s\n", sock_ntop_host(sa, sa->sa_len));

  if ((sa = rti_info[RTAX_NETMASK]) != NULL)
    printf("netmask: %s\n", sock_makstop(sa, sa->sa_len));

  if ((sa = rti_info[RTAX_GETMASK]) != NULL)
    printf("genmask: %s\n", sock_masktop(sa, sa->sa_len));

  return 0;
}
