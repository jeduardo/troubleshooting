#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *worker() {
  printf("Running worker thread...\n");
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
