#include "pipe_ipc.h"
#include <unistd.h>
#include "signal_v2.h"

int main(int argc, char *argv[])
{
//  pipe_test();
//  pipe_do_task();

  signal_ipc(argc, argv);

  return 0;
}