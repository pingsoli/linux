/*
 * the possible way to implement memcpy() function.
 */
#include <stdio.h>
#include <stdint.h>

void memcpy(void *dst, void *src, size_t len) {

  size_t i;

  /*
   * copying a word one time for speedy copy.
   * uintptr_t is 32 bits stands for a word.
   */
  if ((uintptr_t) dst % sizeof(long) == 0 &&
      (uintptr_t) src % sizeof(long) == 0 &&
      len % sizeof(long) == 0) {

    // copy a word a time.
    printf("copying a word a time\n");

    long *d = dst;
    const long *s = src;

    for (i = 0; i < len/sizeof(long); ++i) {
      d[i] = s[i];
    }
  } else {

    printf("copying a byte a time\n");

    // copy a byte a time.
    char *d = dst;
    const char *s = src;

    for (i = 0; i < len; ++i) {
      d[i] = s[i];
    }
  }
}

int main(int argc, char *argv[])
{
  char dst[16] = { 0 };
  char src[16] = "hello world";

  memcpy(dst, src, sizeof(dst));

  printf("dst: %s|\n", dst);

  return 0;
}
