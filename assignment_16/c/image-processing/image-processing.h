#ifndef IMAGE_PROCESSING
#define IMAGE_PROCESSING

typedef struct _yuyv_packet_t {
    u_int8_t Y1;
    u_int8_t U;
    u_int8_t Y2;
    u_int8_t V;
} yuyv_packet_t;

typedef struct _thread_request_processing_t {
    yuyv_packet_t* image;
    u_int32_t offset;
    u_int32_t depth;
} thread_request_processing_t;

#endif