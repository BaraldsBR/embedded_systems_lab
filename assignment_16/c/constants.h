#ifndef CONSTANTS
#define CONSTANTS

#define PI 3.14159

/* SPI Config */
#define SPI_SPEED 1000000
#define SPI_BYTES_PER_TRANSFER 4

/* PWM Config */
#define PITCH_PWM_STEPS 5000
#define YAW_PWM_STEPS 5000

/* GStreamer Config */
#define DEVICE_NAME "/dev/video0"
#define STREAM_IMAGE 1
#define STREAM_IP "145.126.52.203"
#define STREAM_PORT 5001

/* Camera Config */
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define VIDEO_FPS 30
#define HORIZONTAL_FOV 44
#define VERTICAL_FOV 33

/* Image Processing Config */
#define SUBTHREADS 4
#define MIN_Y 18
#define MAX_Y 149
#define MIN_U 65
#define MAX_U 115
#define MIN_V 80
#define MAX_V 130

#endif