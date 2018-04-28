#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>  /* bool keyword */

typedef struct {
  short port;        // 2
  bool  reuseable;   // 1
  int   listen_fd;   // 4
  int   backlog;     // 4
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
  fd_set master;
  fd_set read_fds;
  struct sockaddr_in remoteaddr;
  socklen_t addrlen = sizeof(remoteaddr);
  int fdmax, i, j, newfd;
  int nbytes;
  char buf[256];

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  memset(&remoteaddr, 0, sizeof(remoteaddr));
  memset(buf, 0, sizeof(buf));

  FD_SET(server->listen_fd, &master);

  fdmax = server->listen_fd;

  // Main loop
  for (;;)
  {
    read_fds = master;

    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select failed");
      exit(4);
    }

    for (i = 0; i <= fdmax; ++i)
    {
      if (FD_ISSET(i, &read_fds))
      {
        // this is connection from client
        if (i == server->listen_fd)
        {
          newfd = accept(server->listen_fd,
              (struct sockaddr *)&remoteaddr, &addrlen);

          if (newfd == -1)
          {
            perror("accept failed");
          }
          else
          {
            FD_SET(newfd, &master);

            if (newfd > fdmax)
              fdmax = newfd;

            printf("selectserver: new connection from %d\n", newfd);
          }
        }
        else
        {
          // handle client data and send to every client
          if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
          {
            // recv occurs error
            if (nbytes == 0)
              printf("selectserver: socket %d hung up\n", i);
            else
              fprintf(stderr, "recv error: %s\n", gai_strerror(errno));

            close(i);
            FD_CLR(i, &master);
          }
          else
          {
            // send back to every connected client except itself
            for (j = 0; j <= fdmax; ++j)
            {
              if (FD_ISSET(j, &master))
              {
                if (j != server->listen_fd && j != i)
                {
                  if (send(j, buf, nbytes, 0) == -1)
                    fprintf(stderr, "send error: %s\n", gai_strerror(errno));
                }
              }
            }
          }
        }
      }
    }
  } // for main loop
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
  create_and_bind(&server, argv[1]);
  listen_to(&server, server.backlog);
  run(&server);

  return 0;
}
