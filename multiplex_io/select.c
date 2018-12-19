
// the following code contains select common usages.
//
//

#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  {
    // use select as a timer
    // Note: select will modify the timeout value, you need to reset the timer
    // when calling select every time.
    struct timeval timer, tmp_timer;
    timer.tv_sec = 3;
    timer.tv_usec = 0;
    tmp_timer = timer;

    while (1) {
      printf("set a timer, %ld.%ld seconds later will timeout...\n",
          timer.tv_sec, timer.tv_usec);
      select(1, NULL, NULL, NULL, &tmp_timer);
      printf("timeout...\n");

      tmp_timer = timer;
    }
  }

  return 0;
}
