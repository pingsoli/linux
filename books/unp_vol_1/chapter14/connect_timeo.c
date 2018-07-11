/*
 * using alarm() to implement timeout function
 */

/* why return directly? */
void connect_alarm(int) {
  return;
}

typedef void Sigfunc(int);

int connect_timeo(int sockfd, const struct sockaddr* saptr,
    socklen_t salen, int nsec)
{
  Sigfunc *sigfunc;
  int n;

  sigfunc = Signal(SIGALRM, connect_alarm);

  if (alarm(nsec) != 0) {
    fprintf(stderr, "alarm failed\n");
  }

  if ((n = connect(sockfd, saptr, salen)) < 0) {
    close(sockfd);
    if (errno == EINTR)
      errno = ETIMEOUT;
  }
  alarm(0);                 /* turn off the alarm */
  Signal(SIGALRM, sigfunc); /* restore previous signal handler */

  return (n);
}
