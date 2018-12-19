/*
 * the possible way to implement strcpy() function.
 */
#include <stdio.h>
#include <string.h>

char *strcpy(char *strDest, const char *strSrc) {
  char *temp = strDest;
  while (*strDest++ = *strSrc++);
  return temp;
}

void* my_memcpy(void *dst, void *src, unsigned int count) {
  void *ret = dst;

  // If dst and src do not overlap, copy from low address to high address.
  if (dst <= src || (char *) dst >= (char *) src + count) {
    while (count--) {
      *(char *) dst = *(char *) src;
      dst = (char *) dst + 1;
      src = (char *) src + 1;
    }
  } else {
    // dst and src overlap, copy from high address to low address.
    dst = (char *) dst + count - 1;
    src = (char *) src + count - 1;
    while (count--) {
      *(char *) dst = *(char *) src;
      dst = (char *) dst - 1;
      src = (char *) src - 1;
    }
  }

  return ret;
}


int main(int argc, char *argv[])
{
  {
    char dest[10] = { 0 };
    char src[10] = "hello";

    printf("dest str: %s|\n", strcpy(dest, src));
  }

  {
/*     // memory overlap */
    /* char str[10] = "abc\0"; */
    /* strcpy(str + 1, str); */
    /*  */
    /* printf("str: %s|\n", str); */
  }

  {
    // with overlap version
    char str[10] = "abc\0";
    my_memcpy(str + 1, str, strlen(str) + 1);
    printf("str: %s|\n", str);
  }

  return 0;
}
