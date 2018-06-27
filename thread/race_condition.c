/*
 * just a simple example to show the race condition between threads.
 * and we use mutex to synchronize threads.
 */
#include <stdio.h>
#include <pthread.h>

int counter;
const int NLOOP = 5000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* initialization */

void *doit(void *ptr) {
  int i, val;

  for (i = 0; i < NLOOP; ++i) {
    pthread_mutex_lock(&mutex);

    val = counter;
    printf("%ld: %d\n", pthread_self(), val + 1);
    counter = val + 1;

    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t tida, tidb;

  pthread_create(&tida, NULL, &doit, NULL);
  pthread_create(&tidb, NULL, &doit, NULL);

  /* wait for both threads to terminate */
  pthread_join(tida, NULL);
  pthread_join(tidb, NULL);

  printf("final counter: %d\n", counter);

  return 0;
}
