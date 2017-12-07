#include "simple_select_server.h"
#include "header.h"

/*
 * Only have server exmaple, use telnet as client.
 *
 * Features:
 * 1) Use select to achieve IO multiplexing.
 * 2) Receive message from one clinet and select server will
 *    send it to every client except sender client. 
 * 3) Little tip, port reuse.
 */

#define PORT "9034"

/* Convert sockaddr to IPv4 or IPv6 */
void *get_in_addr(struct sockaddr *sa)
{
  // IPv4
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  // IPv6
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void run_select_server(int argc, char **argv)
{
  fd_set master;
  fd_set read_fds;
  int fdmax;
  int listener;
  int newfd;
  struct sockaddr_in remoteaddr; /* Client address */
  socklen_t addrlen;
  char buf[256];
  int nbytes;
  char remoteIP[INET6_ADDRSTRLEN];

  // Provide for setsockopt function to set S0L_REUSEADDR
  int yes;
  int i, j, rv;
  struct addrinfo hints, *ai, *p;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  // Init, specify AF_UNSPEC is not limited for IPv4,
  // IPv6 is available
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) 
  {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(errno));
    exit(1);
  }
  
  // Create a socket and bind it
  for (p = ai; p != NULL; p = p->ai_next) 
  {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) 
    {
      close(listener);
      continue;
    }
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) 
    {
      close(listener);
      continue;
    }
    break;
  }

  // Bind failed and release resource
  if (p == NULL) 
  {
    fprintf(stderr, "selectserver: failed to bind\n");
    exit(2);
  }
  freeaddrinfo(ai);

  // Start to listen
  if (listen(listener, 10) == -1) 
  {
    perror("listen");
    exit(3);
  }

  // Add listener to master fds to wait for connections from clients
  FD_SET(listener, &master);
  fdmax = listener;
  
  // Main loop
  for (;;) 
  {
    // Every loop deal with connection or sending data
    // The master fds may update
    read_fds = master;

    // Block to wait for a read-ready fd
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) 
    {
      perror("select");
      exit(4);
    }

    // Travse all fds
    for (i = 0; i <= fdmax; ++i) 
    {
      
      // Check is ready
      if (FD_ISSET(i, &read_fds)) 
      {
        
        // This is connection from client
        if (i == listener) 
        {
          addrlen = sizeof(remoteaddr);
          newfd = accept(listener,
                (struct sockaddr *)&remoteaddr,
                &addrlen);

          if (newfd == -1) 
          {
            perror("accept");
          } 
          else 
          {
            FD_SET(newfd, &master);

            if (newfd > fdmax)
              fdmax = newfd;

            printf("selectserver: new connection from %s on"
                "socket %d\n",
                inet_ntop(remoteaddr.sin_family,
                get_in_addr((struct sockaddr*)&remoteaddr),
                remoteIP, INET6_ADDRSTRLEN),
              newfd);
          }
        } 
        else 
        {
          // Handle client data and send to every client
          if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) 
          {
            // Geto error or closed by client
            if (nbytes == 0) 
              printf("selectserver: socket %d hung up\n", i);
            else 
              perror("recv");            
            
            // Close fd and remove from master fds
            close(i);
            FD_CLR(i, &master);
          } 
          else 
          {
            // Traverse all fds and send data
            for (j = 0; j <= fdmax; ++j) 
            {
              // Send data except listener and self
              if (FD_ISSET(j, &master)) 
              {
                if (j != listener && j!= i) 
                {
                  if (send(j, buf, nbytes, 0) == -1)
                    perror("send");
                }
              }
            }
          }
        } // END handle data from client.
      } // END got new incoming connection.
    } // END looping through file descriptors.
  } // END for(;;), and It would never end.

}
