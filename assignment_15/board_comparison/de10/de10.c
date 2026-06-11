// #include <error.h>
#include <sys/time.h> 
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <getopt.h>
#include <unistd.h>


#include "soc_system.h"

typedef struct _bus_content_t {
  int16_t pitch;
  int16_t yaw;
} bus_content_t;


double time_time(void) {
  struct timeval tv;
  double t;

  gettimeofday(&tv, 0);

  t = (double)tv.tv_sec + ((double)tv.tv_usec / 1E6);

  return t;
}


int main(int argc, char** argv) {
	int fd = 0;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("Couldn't open /dev/mem\n");
		return -1;
	}
	uint8_t* encoder_bus_map = NULL;
	encoder_bus_map = (uint8_t*)mmap(NULL, 
                                     HPS_0_ARM_A9_0_BUS_0_SPAN, 
                                     PROT_READ | PROT_WRITE, 
                                     MAP_SHARED, 
                                     fd, 
                                     HPS_0_ARM_A9_0_BUS_0_BASE);
	if (encoder_bus_map == MAP_FAILED) {
		perror("Couldn't map bridge.");
		close(fd);
		return -1;

	}

  int counter = 123;
  int response;

  double start_time;
  double end_time = 0;

  *((int*)encoder_bus_map) = counter;
	response = *((int*)encoder_bus_map);

  start_time = time_time();

	*((int*)encoder_bus_map) = counter;
	response = *((int*)encoder_bus_map);

  end_time = time_time();

  printf("Two-way single transfer, read value: %d, total time: %f \n", response, end_time - start_time);

  counter = 0;

  start_time = time_time();

  while(counter < 1000000) {
		*((int*)encoder_bus_map) = counter;
		counter++;
  }

  end_time = time_time();

  printf("One-way continous transfer, counter: %d, total time: %f \n", counter, end_time - start_time);

  counter = 0;


  while(counter < 100) {
		*((int*)encoder_bus_map) = counter;
		response = *((int*)encoder_bus_map);
		if (response != counter) printf("value mismatch\n");
    counter++;
  }


	close(fd);
	return 0;
}