#ifndef IMAGE_PROCESSING
#define IMAGE_PROCESSING

#include <stdint.h>

#define MIN_Y 50
#define MAX_Y 75
#define MIN_U 50
#define MAX_U 75
#define MIN_V 50
#define MAX_V 75

typedef struct _yuyv_packet_t {
    uint8_t Y1;
    uint8_t U;
    uint8_t Y2;
    uint8_t V;
} yuyv_packet_t;

typedef struct _thread_processing_request_t {
    yuyv_packet_t* image;
    uint32_t starting_row;
    uint32_t row_count;
    uint32_t row_size;
} thread_processing_request_t;

typedef struct _thread_processing_response_t {
    uint32_t total_mass;
    uint32_t total_vertical_sum;
    uint32_t total_horizontal_sum;
} thread_processing_response_t;

void* processImageChunk(void* args);

#endif