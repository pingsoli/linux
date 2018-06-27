/*
 * parsing long options with getopt_long
 *
 * exmaple
 *   use '-' as option element.
 *   $ ./getopt_long -a -b -d h -c create -f file
 *   option -a
 *   option -b
 *   option -d with value `h'
 *   option -c with value `create'
 *   option -f with value `file'
 *
 *   use '--' as option element.
 *   $ ./getopt_long --add --append --delete del --create create --file filename
 *   option -a
 *   option -b
 *   option -d with value `del'
 *   option -c with value `create'
 *   option -f with value `filename'
 *
 *   mix '-' and '--' option element.
 *   $ ./getopt_long --add -b -d delete --create create -f filename
 *   option -a
 *   option -b
 *   option -d with value `delete'
 *   option -c with value `create'
 *   option -f with value `filename'
 *
 *   invalid argument option.
 *   $ ./getopt_long -g
 *   ./getopt_long: invalid option -- 'g'
 *
 *   optional option. (NOTE: you can not use '--output optional')
 *   $ ./getopt_long --output=optional
 *   option -o with value `optional'
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static int verbose_flag;

int main(int argc, char *argv[])
{
  int c;

  while (1) {
    static struct option long_options[] =
    {
      {"verbose", no_argument, &verbose_flag, 1},
      {"brief",   no_argument, &verbose_flag, 0},

      {"add",    no_argument,       0, 'a'},
      {"append", no_argument,       0, 'b'},
      {"delete", required_argument, 0, 'd'},
      {"create", required_argument, 0, 'c'},
      {"file",   required_argument, 0, 'f'},
      {"output", optional_argument, 0, 'o'},

      {0, 0, 0, 0}
    };

    int option_index = 0;

    c = getopt_long(argc, argv, "abc:d:f:o::",
        long_options, &option_index);

    if (c == -1)
      break;

    switch (c)
    {
      case 0:
        if (long_options[option_index].flag != 0)
          break;
        printf("option %s", long_options[option_index].name);
        if (optarg)
          printf(" with arg %s", optarg);
        printf("\n");
        break;

      case 'a':
        puts("option -a");
        break;

      case 'b':
        puts("option -b");
        break;

      case 'c':
        printf("option -c with value `%s'\n", optarg);
        break;

      case 'd':
        printf("option -d with value `%s'\n", optarg);
        break;

      case 'f':
        printf("option -f with value `%s'\n", optarg);
        break;

      case 'o':
        printf("option -o with value `%s'\n", optarg);
        break;

      case '?':
        break;

      default:
        abort();
    }
  }

  if (verbose_flag)
    puts("verbose flag is set");

  if (optind < argc)
  {
    printf("non-option ARGV-elements: ");
    while (optind < argc)
      printf("%s ", argv[optind++]);
    putchar('\n');
  }

  return 0;
}
