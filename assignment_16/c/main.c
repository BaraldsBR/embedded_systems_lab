#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "controller/loop.h"
#include "image-processing/loop.h"

int main(int argc, char *argv[])
{
  pthread_t control_thread, image_thread;
  
  pthread_create(&control_thread, NULL, controllerLoop, NULL);
  pthread_create(&image_thread, NULL, imageProcessingLoop, NULL);
  
  pthread_join(control_thread, NULL);
  pthread_join(image_thread, NULL);

  return 0;
}