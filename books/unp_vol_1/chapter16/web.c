#include "web.h"

const int max_line = 1024;

#define min(x,y) ({                  \
    typeof(x) _min1 = (x);           \
    typeof(y) _min2 = (y);           \
    (void) (&_min1 == &_min2);       \
    _min1 < _min2 ? _min1 : _min2; })

int main(int argc, char** argv)
{
  int i, fd, maxnconn, flags, error;
  char buf[max_line];
  fd_set rs, ws;

  if (argc < 5) {
    fprintf(stderr, "usage: web <conns> <hostname> <homepage> <file1> ...\n");
    exit(-1);
  }

  maxnconn = atoi(argv[1]);
  nfiles = min(argc - 4, MAXFILES);
  for (i = 0; i < nfiles; i++) {
    file[i].f_name = argv[i + 4];
    file[i].f_host = argv[2];
    file[i].f_flags = 0;
  }
  printf("nfiles = %d\n", nfiles);

  home_page(argv[2], argv[3]);

  FD_ZERO(&rset);
  FD_ZERO(&wset);
  maxfd = -1;
  nlefttoread = nlefttoconn = nfiles;
  nconn = 0;

  while (nlefttoread > 0) {
    while (nconn < maxnconn && nlefttoconn > 0) {
      /* find a file to read */
      for (i = 0; i < nfiles; ++i)
        if (file[i].f_flags == 0)
          break;

      if (i == nfiles) {
        fprintf(stderr, "nlefttoconn = %d but nothing found\n", nlefttoconn);
        exit(-1);
      }
      start_connect(&file[i]);
      nconn++;
      nlefttoconn--;
    }
  }

  rs = rset;
  ws = wset;

  n = select(maxfd + 1, &rs, &ws, NULL, NULL);

  for (i = 0; i < nfiles; ++i) {
    flags = file[i].f_flags;
    if (flags == 0 || flags & F_DONE)
      continue;

    fd = file[i].f_fd;
    if (flags & F_CONNECTING &&
        (FD_ISSET(fd, &rs) || FD_ISSET(fd, &ws))) {
      n = sizeof(error);
      if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &n) < 0 || 
          error != 0) {
        fprintf(stderr, "nonblocking connect failed for %s", file[i].f_name);
      }

      /* connection established */
      printf("connection established for %s\n", file[i].f_name);
      FD_CLR(fd, &wset);
      write_get_cmd(&file[i]); /* write the GET command */
    } else if (flags & F_READING && FD_ISSET(fd, &rs)) {
      if ((n = read(fd, buf, sizeof(buf))) == 0) {
        printf("end of file on %s\n", file[i].f_name);
        close(fd);
        file[i].f_flags = F_DONE; /* clear F_READING */
        FD_CLR(fd, &wset);
        nconn--;
      } else {
        printf("read %d bytes from %s\n", n, file[i].f_name);
      }
    }
  }

  return 0;
}
