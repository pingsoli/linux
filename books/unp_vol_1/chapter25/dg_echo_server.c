/*
 * udp echo server using SIGIO signal.
 * udp has only two states, data arrives and exception.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

/*
 * Global variables
 */
static int sockfd;

#define QSIZE 8      /* size of input queue */
#define MAXDG 4096   /* max datagram size */

typedef struct {
  void  *dg_data;         /* ptr to actual datagram */
  size_t dg_len;          /* length of datagram */
  struct sockaddr *dg_sa; /* ptr to sockaddr() w/client's address */
  socklen_t dg_salen;     /* length of sockaddr() */
} DG;

static DG dg[QSIZE];            /* quque of datagram to process */
static long cntread[QSIZE + 1]; /* diagnostic counter */

static int iget;          /* next one for main loop to process */
static int iput;          /* next one for signal handler to read into */
static int nqueue;        /* on queue of main loop to process */
static socklen_t clilen;  /* max length of sockaddr() */

/* SIGIO signal handler */
static void sig_io(int signo) {
  ssize_t len;
  int nread;
  DG *ptr;

  for (nread = 0 ; ; ) {
    if (nqueue >= QSIZE) {
      fprintf(stderr, "received overflow\n");
      exit(-1);
    }

    ptr = &dg[iput];
    ptr->dg_salen = clilen;
    len = recvfrom(sockfd, ptr->dg_data, MAXDG, 0,
        ptr->dg_sa, &ptr->dg_salen);

    if (len < 0) {
      if (errno == EWOULDBLOCK)
        break; /* all done; no more queue to read */
      else
        fprintf(stderr, "recvfrom error");
    }
    ptr->dg_len = len;

    nread++;
    nqueue++;
    if (++iput >= QSIZE)
      iput = 0;
  }

  cntread[nread]++;  /* histogram # datagrams read per signal */
  printf("SIGIO handler finished....\n");
}

/* SIGHUP signal handler */
static void sig_hup(int signo) {
  for (int i = 0; i <= QSIZE ; ++i) {
    printf("cntread[%d] = %ld\n", i, cntread[i]);
  }
}

static void sig_int(int signo) {
  printf("capture SIGINT signal....\n");
  nqueue++;
}

void dg_echo(int sockfd_arg, struct sockaddr *plicaddr, socklen_t clilen_arg) {
  int i;
  const int on = 1;
  sigset_t zeormask, newmask, oldmask;

  sockfd = sockfd_arg;
  clilen = clilen_arg;

  for (int i = 0; i < QSIZE; ++i) { /* init queue of buffers */
    dg[i].dg_data  = malloc(MAXDG);
    dg[i].dg_sa    = malloc(clilen);
    dg[i].dg_salen = clilen;
  }
  iget = iput = nqueue = 0;

  signal(SIGHUP,  sig_hup);
  //signal(SIGIO,   sig_io);
  signal(SIGINT,  sig_int);

  fcntl(sockfd, F_SETOWN, getpid());
  ioctl(sockfd, FIOASYNC, &on);
  ioctl(sockfd, FIONBIO, &on);

  sigemptyset(&zeormask);
  sigemptyset(&oldmask);
  sigemptyset(&newmask);
  //sigaddset(&newmask, SIGIO); /* signal we want to block */
  sigaddset(&newmask, SIGINT);

  /*
   * what the differences with the below code ?
   */
  //for ( ;; ) {
    //sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    //while (nqueue == 0)
      //sigsuspend(&zeormask);
    //nqueue--;

    ///* unblock SIGIO */
    //sigprocmask(SIG_SETMASK, &oldmask, NULL);

    //sendto(sockff, dg[iget].dg_data, dg[iget].dg_len, 0,
        //dg[iget].dg_sa, dg[iget].dg_salen);

    //if (++iget >= QSIZE)
      //iget = 0;
  //}

  printf("blocking SIGIO signal\n");
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);
  for ( ;; ) {
    while (nqueue == 0)
      sigsuspend(&zeormask); /* wait until SIGIO handler return */

    /* unblock SIGIO */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    printf("unblocking SIGIO signal\n");

    sendto(sockfd, dg[iget].dg_data, dg[iget].dg_len, 0,
        dg[iget].dg_sa, dg[iget].dg_salen);
    printf("send data to client(%ld): %s\n",
        dg[iget].dg_len, (char *) dg[iget].dg_data);

    if (++iget >= QSIZE)
      iget = 0;

    /* block SIGIO */
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    nqueue--;
  }
}

int main(int argc, char *argv[])
{
  int fd;
  unsigned short port = 13000;
  struct sockaddr_in servaddr, cliaddr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);
  printf("server is starting... (port: %d)\n", port);

  bind(fd, (struct sockaddr*) &servaddr, sizeof(servaddr));
  dg_echo(fd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

  return 0;
}
