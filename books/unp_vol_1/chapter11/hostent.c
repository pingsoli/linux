/*
 * ~$ ./hostent www.baidu.com www.google.com www.yahoo.com
 * official hostname: www.wshifen.com
 *         alias: www.baidu.com
 *         alias: www.a.shifen.com
 *         address: 104.193.88.77
 *         address: 104.193.88.123
 * official hostname: www.google.com
 *         address: 69.171.224.12
 * official hostname: atsv2-fp.wg1.b.yahoo.com
 *         alias: www.yahoo.com
 *         address: 98.137.246.7
 *         address: 98.137.246.8
 *         address: 98.138.219.231
 *         address: 98.138.219.232
 * NOTE:
 * It's not re-entrant functions
 */
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char** argv)
{
  char *ptr, **pptr;
  char str[INET_ADDRSTRLEN];
  struct hostent *hptr;

  while (--argc) {
    ptr = *++argv;

    if ((hptr = gethostbyname(ptr)) == NULL) {
      fprintf(stderr, "gethostbyname failed: %s\n", strerror(errno));
      exit(-1);
    }

    printf("official hostname: %s\n", hptr->h_name);

    for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
      printf("\talias: %s\n", *pptr);

    switch (hptr->h_addrtype)
    {
    case AF_INET:
      pptr = hptr->h_addr_list;
      for ( ; *pptr != NULL; pptr++) {
        printf("\taddress: %s\n",
            inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
//        if ((hptr = gethostbyaddr(*pptr, hptr->h_length, hptr->h_addrtype))
//            == NULL)
//          printf("\t(gethostbyaddr failed)\n");
//        else if (hptr->h_name != NULL)
//          printf("\tname = %s\n", hptr->h_name);
//        else
//          printf("\t(no hostname returned by gethostbyaddr)\n");
      }
      break;

    default:
      printf("unknown address\n");
      break;
    }
  }

  return 0;
}
