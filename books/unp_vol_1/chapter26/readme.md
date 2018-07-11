# Chapter 26: Threads
  
## mutex
```
pthread_mutex_lock(&mutex);

/* change the shared data */

pthread_mutex_lock(&mutex);
```
  
## condition variable
regular polling wait wastes too much CPU resource.  
1) sleep wait (basic condition variable)  
2) notify all threads (pthread_cond_broadcast)  
3) timeout (pthread_cond_timewait)  
  
```
/* polling */
for (;;) {
  pthread_mutex_lock(&ndone_mutex);
  if (ndone > 0) {
    /* do something */
  }
  pthread_mutex_unlock(&ndone_mutex);
}


/* condition variable */
for (;;) {
  pthread_mutex_lock(&ndone_mutex);

  while (ndone == 0)
    pthread_cond_wait(&ndone_mutex);

  for (int i = 0; i < nfiles; ++i) {
    if (file[i].f_flags & F_DONE) {
      pthread_join(file[i].f_tid, (void *)&fptr);

      /* update file[i] for terminated thread */
      ...
    }
  }

  pthread_mutex_unlock(&ndone_mutex);
}
```
