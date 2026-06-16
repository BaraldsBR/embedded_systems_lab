#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "controller/loop.h"

void* subimageTest(void* args) {
  int id = *((int*)args);
  printf("Subimage thread with id %d started\n", id);

  int* return_value = malloc(sizeof(int));

  (*return_value) = id * 3;
  pthread_exit(return_value);
}

void* imageTest(void* args) {
  pthread_t subthread[4];
  int id[4];
  void* return_value;
  
  for (;;) {
    printf("Image thread started\n");
    
    for (int i = 0; i < 4; i++) {
      id[i] = i;
      pthread_create(&subthread[i], NULL, subimageTest, &id[i]);
    }
    
    int sum = 0;
    for (int i = 0; i < 4; i++) {
      pthread_join(subthread[i], &return_value);
      sum += *((int*)return_value);
      free(return_value);
    }
    
    printf("Image thread finished with result %d\n", sum);
    
    usleep(1000000);
  }
}

int main(int argc, char *argv[])
{
  pthread_t control_thread, image_thread;
  
  pthread_create(&control_thread, NULL, controllerLoop, NULL);
  pthread_create(&image_thread, NULL, imageTest, NULL);
  
  pthread_join(control_thread, NULL);
  pthread_join(image_thread, NULL);

  return 0;
}