#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "ping.h"

struct proto proto_v4 = {
  proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP
};

struct proto proto_v6 = {
  proc_v6, send_v6, NULL, NULL, NULL, 0, IPPROTO_ICMPV6
};

int main(int argc, char *argv[])
{
  int c;
  struct addrinfo *ai;
  char *h;
  int opterr = 0;
  int optind = 1;

  while ((c = getopt(argc, argv, "v")) != -1) {
    switch (c) {
    case 'v':
      verbose++;
      break;
    case '?':
      fprintf(stderr, "unrecongnized option: %c\n", c);
      exit(-1);
    }
    optind++;
  }

  if (optind != argc - 1) {
    fprintf(stderr, "usage: ping [-v] <hostname>");
    exit(-1);
  }
  host = argv[optind];

  pid = getpid() & 0xffff; /* ICMP ID field is 16 bits */
  signal(SIGALRM, sig_alrm);

  ai = host_serv(host, NULL, 0, 0);

  h = sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
  printf("ping %s (%s): %d data bytes\n",
      (ai->ai_canonname ? ai->ai_canonname : h), h, datalen);

  /* initialize according to protocol */
  if (ai->ai_family == AF_INET) {
    pr = &proto_v4;
  } else if (ai->ai_family == AF_INET6) {
    pr = &proto_v6;
  } else {
    fprintf(stderr, "unknown address family %d\n", ai->ai_family);
    exit(-1);
  }

  pr->sasend = ai->ai_addr;
  pr->sarecv = calloc(1, ai->ai_addrlen);
  pr->salen = ai->ai_addrlen;

  readloop();

  return 0;
}
