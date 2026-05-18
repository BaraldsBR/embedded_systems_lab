#include <error.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "soc_system.h"

typedef struct _bus_content_t {
  int16_t pitch;
  int16_t yaw;
} bus_content_t;

int main(int argc, char** argv) {
	int fd = 0;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("Couldn't open /dev/mem\n");
		return -1;
	}
	uint8_t* encoder_bus_map = NULL;
	encoder_bus_map = (uint8_t*)mmap(NULL, HPS_0_ARM_A9_0_BUS_0_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, HPS_0_ARM_A9_0_BUS_0_BASE);
	if (encoder_bus_map == MAP_FAILED) {
		perror("Couldn't map bridge.");
		close(fd);
		return -1;
	}

	while (1)
	{
		bus_content_t bus_content = *((_bus_content_t *)encoder_bus_map) 
		printf("pitch: %4.d, yaw: %4.d\n", bus_content.pitch, bus_content.yaw);
		sleep(1);
	}

	close(fd);
	return 0;
}
