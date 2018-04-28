/**
 * simple poll server, same as select server
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdbool.h>  /* bool keyword */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  short port;        // 2
  bool  reuseable;   // 1
  int   listen_fd;   // 4
  int   backlog;     // 4
  int   max_conn;
} server_st;

void set_nonblocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) return;

  flags = flags | O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

void create_and_bind(server_st *server, char *port)
{
  struct addrinfo hints, *ai, *p;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;

  int rv;
  if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0)
  {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(errno));
    exit(1);
  }

  for (p = ai; p != NULL; p = p->ai_next)
  {
    server->listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (server->listen_fd < 0)
    {
      close(server->listen_fd);
      continue;
    }

    setsockopt(server->listen_fd, SOL_SOCKET, SO_REUSEADDR,
        &server->reuseable, sizeof(int));

    if (bind(server->listen_fd, p->ai_addr, p->ai_addrlen) < 0)
    {
      close(server->listen_fd);
      continue;
    }
    break;
  }

  set_nonblocking(server->listen_fd);

  if (p == NULL)
  {
    perror("selectserver: failed to bind\n");
    exit(2);
  }

  freeaddrinfo(ai);
}

void listen_to(server_st *server, int backlog)
{
  if (listen(server->listen_fd, backlog) == -1)
  {
    perror("selectserver: failed to bind\n");
    exit(3);
  }
}

void run(server_st *server)
{
  struct pollfd *fds = (struct pollfd *) malloc(
      sizeof(struct pollfd) * server->max_conn);
  int rc, nfds = 1;
  int curr_size;
  int new_fd;
  char buffer[256];
  int nread_bytes;

  memset(buffer, 0, sizeof(buffer));

  fds[0].fd = server->listen_fd;
  fds[0].events = POLLIN;

  for (;;) {
    rc = poll(fds, nfds, 0);
    if (rc < 0) {
      perror("poll failed");
      exit(-1);
    }

    curr_size = nfds;
    for (int i = 0; i < curr_size; ++i) {
      if (fds[i].revents == 0)
        continue;

      if (fds[i].fd == server->listen_fd)
      {
        // new incoming clients
        new_fd = accept(server->listen_fd, NULL, NULL);
        if (new_fd < 0) {
          if (errno != EWOULDBLOCK) {
            perror("accept failed");
          }
        }

        fds[nfds].fd = new_fd;
        fds[nfds].events = POLLIN;
        nfds++;

        printf("new comming client: %d\n", new_fd);
      }
      else
      {
        if (fds[i].events == POLLIN) {
          int curr_fd = fds[i].fd;
          nread_bytes = recv(curr_fd, buffer, sizeof(buffer), 0);

          if (nread_bytes <= 0)
          {
            if (nread_bytes == 0)
              printf("poll server: socket %d hung up\n", curr_fd);
            else
              fprintf(stderr, "recv error: %s\n", gai_strerror(errno));

            close(curr_fd);
          }
          else
          {
            for (int j = 1; j < nfds; ++j) {
              if (fds[j].fd != curr_fd)
              {
                if (send(fds[j].fd, buffer, sizeof(buffer), 0) == -1)
                  fprintf(stderr, "send error: %s\n", gai_strerror(errno));
              }
            }
          }
        }
      }
    }
  }
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  server_st server;

  server.port = (short) atoi(argv[1]);
  server.backlog = 32;
  server.max_conn = 200;
  create_and_bind(&server, argv[1]);
  listen_to(&server, server.backlog);
  run(&server);

  return 0;
}
