/*
 * client and server create a connection, setting the connected socket to keep-alive,
 * , setting the timeout value. checking the tcpdump output.
 *
 * usage: ./keepalive_option 127.0.0.1
 * sudo tcpdump -n -ilo port 12345
 *
 * you can see the following process on tcpdump:
 * 3-way handshake to create a connection
 * keep-alive package sent from server to client
 * simultaneous close
 */

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define check(expr) if (!(expr)) { perror(#expr); kill(0, SIGTERM); }

void enable_keepalive(int sockfd) {
  int yes = 1;
  check(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

  int idle = 1;
  check(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

  int interval = 1;
  check(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

  int maxpkt = 10;
  check(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
}

int main(int argc, char** argv) {
  check(argc == 2);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(12345);
  check(inet_pton(AF_INET, argv[1], &addr.sin_addr) != -1);

  int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  check(server != -1);

  int yes = 1;
  check(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != -1);

  check(bind(server, (struct sockaddr*)&addr, sizeof(addr)) != -1);
  check(listen(server, 1) != -1);

  if (fork() == 0) {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    check(client != -1);
    check(connect(client, (struct sockaddr*)&addr, sizeof(addr)) != -1);
    printf("client connected to server...\n");
    pause();
  }
  else {
    int client = accept(server, NULL, NULL);
    check(client != -1);
    enable_keepalive(client);
    printf("server accepted...\n");
    wait(NULL);
  }

  return 0;
}
