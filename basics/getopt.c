/*
 * int opterr: if it is zero, getopt() returns '?' to indicate an error.
 * int optind: initial value of this variable is 1.
 * int optopt: stores the option character in this variable.(diagnostic message)
 * char *optarg: for those options that accept arguments.
 *
 * refer to
 * https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int aflag = 0;
  int bflag = 0;
  int cflag = 0;
  char *cvalue = NULL;
  int index;
  int c;
  opterr = 0; /* do not print error to stderr, rather return '?' by `getopt` */

  /*
   * ':' to indicate that it takes a required argument.
   * '::' to indicate that its argument is optional.
   */
  while ((c = getopt(argc, argv, "abc:")) != -1) {
    switch (c)
    {
      case 'a':
        aflag = 1;
        break;
      case 'b':
        bflag = 1;
        break;
      case 'c':
        cvalue = optarg;
        break;
      case '?': /* indicate error */
        if (optopt == 'c')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        exit(-1);
    }
  }

  printf("aflag = %d, bflag = %d, cvalue = %s\n",
      aflag, bflag, cvalue);

  for (index = optind; index < argc; ++index)
    printf("Non-option argument %s\n", argv[index]);

  return 0;
}
