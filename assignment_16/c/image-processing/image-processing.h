#ifndef IMAGE_PROCESSING
#define IMAGE_PROCESSING

#include <stdint.h>

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