/*
 * using mutex to synchronize threads. just a simple example
 * two threads increase counter simultaneously.
 */
#include <pthread.h>
#include <stdio.h>

const int NLOOP = 5000;

/* protect shared data counter */
pthread_mutex_t counter_mutex;
int counter; /* shared between threads */

void *doit(void *ptr) {
  int i, val;

  for (i = 0; i < NLOOP; ++i) {
    pthread_mutex_lock(&counter_mutex);

    val = counter;
    printf("%ld: %d\n", pthread_self(), val + 1);
    counter = val + 1;

    pthread_mutex_unlock(&counter_mutex);
  }
}

int main(int argc, char *argv[])
{
  pthread_t tida, tidb;

  /* create two threads, and increase the counter */
  pthread_create(&tida, NULL, &doit, NULL);
  pthread_create(&tidb, NULL, &doit, NULL);

  /* wait for threads to finish */
  pthread_join(tida, NULL);
  pthread_join(tidb, NULL);

  printf("final counter value: %d\n", counter);

  return 0;
}
