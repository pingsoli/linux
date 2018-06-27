/*
 * pthread first test example. create two threads, print some message.
 * NOTE: no synchronization, the output may be incorrect.
 *
 * 1. wait for threads to terminate or detach from the current thread ?
 *    pthread_join(): wait for other threads (block until thread finished)
 *    pthread_detach(): detach from current thread
 *
 * 2. passing parameter to thread and get the returned value ?
 *    the magic is void * type of parameter. they can be converted to any type
 *    forcely.
 *    you can get the returned value from the second parameter of pthread_join()
 *    function
 *
 * 3. check the status of thread ? exited, running, or other status ?
 *    undo.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *print_message_function(void *ptr);

int thread1_rv = 1; /* return value from threads */

int main(int argc, char *argv[])
{
  pthread_t thread1;
  const char *message1 = "thread 1";
  int tid1;
  void *t1_rv;

  tid1 = pthread_create(&thread1, NULL, print_message_function, (void*)message1);
  if (tid1) {
    fprintf(stderr, "pthread_create failed\n");
    exit(-1);
  }

  /* wait until the two threads finished */
  pthread_join(thread1, &t1_rv);

  /* check the return value */
  printf("thread 1 return value: %d\n", *(int *)t1_rv);

  return 0;
}

void *print_message_function(void *ptr) {
  char *msg;
  msg = (char *) ptr;
  printf("thread ID: %ld\n", pthread_self());
  printf("%s\n", msg);
  thread1_rv = 0; /* 0 represent success */
  return (void *) &thread1_rv;
}
