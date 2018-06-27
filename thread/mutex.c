/*
 * using mutex to synchronize threads when the data is shared between threads.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *functionc() {
  pthread_mutex_lock(&mutex1);

  for (int i = 0; i < 10; ++i) {
    counter++;
    printf("counter value: %d\n", counter);
  }

  pthread_mutex_unlock(&mutex1);
}

int main(int argc, char *argv[])
{
  int rc1, rc2;
  pthread_t thread1, thread2;

  if ((rc1 = pthread_create(&thread1, NULL, &functionc, NULL)) < 0) {
    fprintf(stderr, "pthread_create failed\n");
    exit(-1);
  }

  if ((rc2 = pthread_create(&thread2, NULL, &functionc, NULL)) < 0) {
    fprintf(stderr, "pthread_create failed\n");
    exit(-1);
  }

  /* wait for threads to finish */
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  return 0;
}
