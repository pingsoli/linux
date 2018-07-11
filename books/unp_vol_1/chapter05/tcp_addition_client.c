#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

ssize_t readn(int fd, void *vptr, size_t n)
{
  ssize_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = vptr;
  nleft = n;

  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else
        return -1;
    } else if (nread == 0)
      break; /* EOF */

    nleft -= nread;
    ptr += nread;
  }
  return (n - nleft);
}

struct args {
  long arg1;
  long arg2;
};

struct result {
  long sum;
};

void str_cli(FILE *fp, int sockfd)
{
  char sendline[1024];
  struct args args;
  struct result result;
  int n_sent_bytes;

  memset(sendline, 0, sizeof(sendline));
  while (fgets(sendline, 1024, fp) != NULL) {
    if (sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2) != 2) {
      printf("invalid input: %s", sendline);
      continue;
    }

    write(sockfd, &args, sizeof(args) == 0);

    if (readn(sockfd, &result, sizeof(result)) == 0) {
      fprintf(stderr, "readn failed: %s\n", strerror(errno));
      exit(-1);
    }

    printf("%ld\n", result.sum);
  }
}

/*
 * the server child process has been killed, and the client send data
 * to server-child process. This would generate SIGPIPE (RST is received).
 */
void sigpipe_handler(int signum) {
  printf("handle SIGPIPE signal\n");
  return;
}

int main(int argc, char** argv)
{
  struct sockaddr_in servaddr;
  int sockfd;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
    exit(-1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket failed: %s\n", strerror(errno));
    exit(-1);
  }

  signal(SIGPIPE, sigpipe_handler);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
    exit(-1);
  }

  str_cli(stdin, sockfd); /* do it all */

  return 0;
}
