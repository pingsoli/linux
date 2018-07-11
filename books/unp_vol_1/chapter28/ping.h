#ifndef PING_H_
#define PING_H_


#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>

#define BUFSIZE 1500

char sendbuf[BUFSIZE];

static int   datalen;
static char *host;
static int   nsent;
static pid_t pid;
static int   sockfd;
static int   verbose;

void init_v6();
void proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
void proc_v6(char *, ssize_t, struct msghdr *, struct timeval *);
void send_v4(void);
void send_v6(void);
void readloop(void);

void sig_alrm(int);
void tv_sub(struct timeval *, struct timeval *);

char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
uint16_t in_cksum(uint16_t *addr, int len);

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype);

struct proto {
  void (*fproc) (char *, ssize_t, struct msghdr *, struct timeval *);
  void (*fsend) (void);
  void (*finit) (void);
  struct sockaddr *sasend; /* sockaddr() for send, from getaddrinfo */
  struct sockaddr *sarecv; /* sockaddr() for receiving */
  socklen_t salen;         /* length for sockaddr()s */
  int       icmpproto;     /* IPPROTO_XXX value for ICMP */
} *pr;

#endif /* PING_H_ */
