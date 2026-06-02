// #include <error.h> 
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "soc_system.h"

typedef struct _controller_content_t {
  int16_t pitch;
  int16_t yaw;
} controller_content_t;

int main(int argc, char** argv) {
	int fd = 0;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("Couldn't open /dev/mem\n");
		return -1;
	}

	uint8_t* controller_adapter_map = NULL;
	controller_adapter_map = (uint8_t*)mmap(NULL, 
                                     HPS_0_ARM_A9_0_CONTROLLER_ADAPTER_0_SPAN, 
                                     PROT_READ | PROT_WRITE, 
                                     MAP_SHARED, 
                                     fd, 
                                     HPS_0_ARM_A9_0_CONTROLLER_ADAPTER_0_BASE);
	if (controller_adapter_map == MAP_FAILED) {
		perror("Couldn't map controller.");
		close(fd);
		return -1;

	}

    controller_content_t controller_content;

    printf("Set pitch and yaw: ");
    scanf("%hd %hd", &controller_content.pitch, &controller_content.yaw);
    
    *((controller_content_t*)controller_adapter_map) = controller_content;

    controller_content = *((controller_content_t*)controller_adapter_map);

    printf("Pitch Encoder: %hd; Yaw Encoder: %hd\n", controller_content.pitch, controller_content.yaw);
    
	close(fd);
	return 0;
}