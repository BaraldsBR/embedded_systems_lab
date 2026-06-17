#ifndef IMAGE_PROCESSING_LOOP
#define IMAGE_PROCESSING_LOOP

#define DEVICE_NAME "/dev/video0"
#define STREAM_IP ""
#define STREAM_PORT 5001

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define VIDEO_FPS 30

#define SUBTHREADS 4

void* imageProcessingLoop(void* args);

#endif