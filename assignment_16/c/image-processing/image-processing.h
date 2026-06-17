#ifndef IMAGE_PROCESSING
#define IMAGE_PROCESSING

#define MIN_Y 50
#define MAX_Y 75
#define MIN_U 50
#define MAX_U 75
#define MIN_V 50
#define MAX_V 75

typedef struct _yuyv_packet_t {
    u_int8_t Y1;
    u_int8_t U;
    u_int8_t Y2;
    u_int8_t V;
} yuyv_packet_t;

typedef struct _thread_processing_request_t {
    yuyv_packet_t* image;
    u_int32_t starting_row;
    u_int32_t row_count;
    u_int32_t row_size;
} thread_processing_request_t;

typedef struct _thread_processing_response_t {
    u_int32_t total_mass;
    u_int32_t total_vertical_sum;
    u_int32_t total_horizontal_sum;
} thread_processing_response_t;

void* processImageChunk(void* args);

#endif