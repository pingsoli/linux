#include "web.h"

void home_page(const char *host, const char *fname)
{
  int fd, n;
  char line[max_line];

  fd = tcp_connect(host, SERV); /* blocking connect() */
  n = snprintf(line, sizeof(line), GET_CMD, fname);
  writen(fd, line, n);

  for ( ;; ) {
    if ((n = read(line, sizeof(line), max_line)) == 0)
      break; /* server closed connection */

    printf("read %d bytes of home page\n", n);
  }
  printf("end-of-file on home page\n");
  close(fd);
}
