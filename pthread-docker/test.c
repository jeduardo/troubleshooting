#include <bits/types/struct_sched_param.h>
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *worker() {
  struct sched_param sp;
  int err;

  printf("Running worker thread...\n");

  // Setting scheduling parameter
  // https://man7.org/linux/man-pages/man7/sched.7.html
  sp.sched_priority = 50;

  // This works only with root, otherside there will be an EPERM
  err = pthread_setschedparam(pthread_self(), SCHED_RR, &sp);
  if (err) {
    switch (err) {
      case EPERM:
        printf("EPERM\n");
        break;
      case EINVAL:
        printf("EINVAL\n");
        break;
      case ESRCH:
        printf("ESRCH\n");
        break;
    }
    perror("Unable to set scheduling priority within thread");
  }

  sleep(5);
  printf("Worker thread completed\n");
}

int main (int argc, char **argv) {

  pthread_t threadId;
  pthread_attr_t threadAttr;
  int err = -1;

  // Trying to replicate the operations from https://github.com/LuaLanes/lanes/blob/master/src/threading.c#L767

  err = pthread_attr_init(&threadAttr);
  if (err) {
    perror("Unable to init thread attributes");
    return EXIT_FAILURE;
  }
 
  // SCHED_RR does not work here because I am not setting a priority
  err = pthread_attr_setschedpolicy(&threadAttr, SCHED_OTHER);
  if (err) {
    perror("Unable to set thread priority");
    return EXIT_FAILURE;
  }

  err = pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
  if (err) {
    perror("Unable to set detached state");
    return EXIT_FAILURE;
  }

  err = pthread_attr_setstacksize(&threadAttr, 2097152);
  if (err) {
    perror("Unable to set stack size");
    return EXIT_FAILURE;
  }

  err = pthread_attr_setinheritsched(&threadAttr,PTHREAD_EXPLICIT_SCHED);
  if (err) {
    perror("Unable to set schedule inheritance");
    return EXIT_FAILURE;
  }
  
  err = pthread_create(&threadId, &threadAttr, &worker, NULL);
  if (err) {
    perror("Unable to create thread");
    return EXIT_FAILURE;
  }

  printf("Running main thread...\n");
  sleep(10);
  printf("Main thread completed\n");

  return EXIT_SUCCESS;
}
