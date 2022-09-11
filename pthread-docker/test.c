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

  err = pthread_attr_init(&threadAttr);
  if (err != 0) {
    perror("Unable to init thread attributes");
    return EXIT_FAILURE;
  }
  
  err = pthread_create(&threadId, &threadAttr, &worker, NULL);
  if (err != 0) {
    perror("Unable to create thread");
    return EXIT_FAILURE;
  }

  printf("Running main thread...\n");
  sleep(10);
  printf("Main thread completed\n");

  return EXIT_SUCCESS;
}
