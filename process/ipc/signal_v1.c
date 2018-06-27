#include <unistd.h>

void fake_infinite_loop()
{
  /*
   * It won't loop infinitely
   * SIGALARM default behavior will terminate current process
   * So, 5 seconds later, the program will termiante
   */
  alarm(5);
  for (;;)
    ;
}

int main(int argc, char** argv)
{
  fake_infinite_loop();

  return 0;
}
