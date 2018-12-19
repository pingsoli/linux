/*
 * LT Vs. ET
 * the state of stdin changes, and LT will be triggered always.
 * but ET will be triggered only once.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>

int main(int argc, char *argv[])
{
  int epfd, nfds;
  struct epoll_event event, events[5];

  epfd = epoll_create(1);

  event.data.fd = STDIN_FILENO;
  /* event.events = EPOLLIN | EPOLLET; // ET mode */
  event.events = EPOLLIN; // LT mode
  epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);

  int size = sizeof(events) /sizeof(struct epoll_event);

  while (1) {
    nfds = epoll_wait(epfd, events, size, -1);

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == STDIN_FILENO) {
        printf("Hello world\n");
      }
    }
  }

  return 0;
}
