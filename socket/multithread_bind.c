/*
 * SO_REUSEPORT option
 * This option aimed to improve the multithread network service performance.
 * multiple sockets can reuse the same source address and same port.
 *
 * the following code create multiple threads to reuse the same address and port.
 * and wait for new connections from clients.
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* do_work(void *arg)
{
  int *port = (int *) arg;

  int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  int one = 1;
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(*port);

  int ret = bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (ret == -1)
  {
    printf("Failed to bind to port\n");
    return NULL;
  }

  listen(listen_socket, 5);

  struct sockaddr_in cli_addr;
  memset(&cli_addr, 0, sizeof(cli_addr));
  int addr_length = sizeof(cli_addr);

  /* pthread_mutex_lock(&mutex1); */
  /* printf("thread[%ld] is starting accept new connection\n", pthread_self()); */
  /* pthread_mutex_unlock(&mutex1); */

  char cliip[64] = {0};
  do
  {
    int cli_sock = accept(listen_socket, (struct sockaddr *) &cli_addr, (socklen_t *) &addr_length);
    inet_ntop(AF_INET, &cli_addr.sin_addr, cliip, sizeof(cliip));

    pthread_mutex_lock(&mutex1);
    printf("thread[%ld] accepted new connection[%d] from %s:%d\n",
        pthread_self(), cli_sock, cliip, ntohs(cli_addr.sin_port));
    pthread_mutex_unlock(&mutex1);

    close(cli_sock);
  } while (true);

  close(listen_socket);

  return 0;
}

int main(int ac, const char *av[])
{
  if (ac != 2)
  {
    printf("Usage: liser <port>\n");
    return 1;
  }

  int port = atoi(av[1]);

  const int MAX_THREADS = 10;
  pthread_t tid[MAX_THREADS];
  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_create(&tid[i], NULL, do_work, &port);
  }

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }
  return 0;
}
