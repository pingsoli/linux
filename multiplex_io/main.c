#include "epoll_server.h"
#include <stdio.h>
#include <stdlib.h>
#include "simple_select_server.h"

int main(int argc, char *argv[])
{
/*
  if (argc != 2)
  {
    fprintf(stderr, "Usage: <exec> <port>\n");
    exit (-1);
  }
  epoll_server_start(argc, argv);
*/
  
  run_select_server(argc, argv);

  return 0;
}
