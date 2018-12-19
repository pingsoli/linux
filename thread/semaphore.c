// use semaphore to sychronize threads communication.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NITER 100000

int cnt = 0;

void *Count(void *a) {
  int i, tmp;
  sem_t* sem = (sem_t*) a;

  for (i = 0; i < NITER; ++i)
  {
    sem_wait(sem); // lock and wait the semaphore, decrement semaphore value, if
                   // value is 0, wait till somebody has done sem_post.
    tmp = cnt;
    tmp = tmp + 1;
    cnt = tmp;
    sem_post(sem); // increase semaphore value
  }
}

int main(int argc, char *argv[])
{
  pthread_t tid1, tid2, tid3;
  sem_t sem;

  sem_init(&sem, 0, 1); // value is 1, stands for block/unblock state.

  // No error checking
  pthread_create(&tid1, NULL, Count, &sem);
  pthread_create(&tid2, NULL, Count, &sem);
  pthread_create(&tid3, NULL, Count, &sem);

  pthread_join(tid1, NULL); // wait thread 1 to finish
  pthread_join(tid2, NULL); // wait thread 2 to finish
  pthread_join(tid3, NULL);

  sem_destroy(&sem); // destroy semaphore

  if (cnt < 3 * NITER)
    printf("\n cnt is [%d], should be %d\n", cnt, 3*NITER);
  else
    printf("\n OK! cnt is[%d]\n", cnt);

  return 0;
}
