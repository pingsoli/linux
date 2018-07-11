/*
 * copies a file to standard output
 */
#include <errno.h>
#include <fcntl.h>
#include <libgen.h> /* basename() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int max_line = 1024;
#define HAVE_MSGHDR_MSG_CONTROL

ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
  struct msghdr msg;
  struct iovec iov[1];
  ssize_t n;

#ifdef HAVE_MSGHDR_MSG_CONTROL
  union {
    struct cmsghdr cm;
    char control[CMSG_SPACE(sizeof(int))];
  } control_un;
  struct cmsghdr *cmptr;

  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);
#else
  int newfd;
  msg.msg_accrights = (caddr_t) &newfd;
  msg.msg_accrightslen = sizeof(int);
#endif

  msg.msg_name = NULL;
  msg.msg_namelen = 0;

  iov[0].iov_base = ptr;
  iov[0].iov_len  = nbytes;
  msg.msg_iov     = iov;
  msg.msg_iovlen  = 1;

  if ((n = recvmsg(fd, &msg, 0)) <= 0)
    return (n);

#ifdef HAVE_MSGHDR_MSG_CONTROL
  if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
      cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
    if (cmptr->cmsg_level != SOL_SOCKET) {
      fprintf(stderr, "control type != SOL_SOCKET\n");
      exit(-1);
    }

    if (cmptr->cmsg_type != SCM_RIGHTS) {
      fprintf(stderr, "control type != SCM_RIGHTS");
      exit(-1);
    }

    *recvfd = *(int *) CMSG_DATA(cmptr);
  } else {
    *recvfd = 1; /* descriptor was not passed */
  }
#else
  if (msg.msg_accrightslen == sizeof(int))
    *recvfd = newfd;
  else
    *recvfd = -1;
#endif

  return (n);
}

int
my_open(const char *pathname, int mode)
{
  int fd, sockfd[2], status;
  pid_t childpid;
  char c, argsockfd[10], argmode[10];

  socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

  if ((childpid = fork()) == 0) { /* chlid process */
    close(sockfd[0]);
    snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
    snprintf(argmode, sizeof(argmode), "%d", mode);

    printf("open file %s %s %s", argsockfd, pathname, argmode);
    execl("./openfile", "openfile", argsockfd, pathname, argmode,
        (char *)NULL);
    fprintf(stderr, "execl failed\n");
  }

  /* parent process - wait for the chlid to terminate */
  close(sockfd[1]);

  waitpid(childpid, &status, 0);
  if ((status == WEXITSTATUS(status)) == 0)
    read_fd(sockfd[0], &c, 1, &fd);
  else {
    errno = status;
    fd = -1;
  }

  close(sockfd[0]);
  return (fd);
}

int main(int argc, char** argv)
{
  int fd, n;
  char buff[max_line + 1];

  if (argc != 2) {
    fprintf(stderr, "usage: %s <pathname>\n", basename(argv[1]));
    exit(-1);
  }

  if ((fd = my_open(argv[1], O_RDONLY)) < 0) {
    fprintf(stderr, "cannot open %s\n", argv[1]);
    exit(-1);
  }

  while ((n = read(fd, buff, max_line)) > 0)
    write(fileno(stdout), buff, n);

  return 0;
}
