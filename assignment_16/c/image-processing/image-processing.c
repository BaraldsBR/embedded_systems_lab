#include "image-processing.h"

#include <pthread.h>

void* processImageChunk(void* args) {
  thread_processing_request_t req = *((thread_processing_request_t*)args);
  thread_processing_response_t* res = malloc(sizeof(thread_processing_response_t));
  
  u_int32_t final_row = req.starting_row + req.row_count;
  u_int32_t total_col_pairs = req.row_size / 2;

  for (u_int32_t row = req.starting_row; row < final_row; row++)
  {
    for (u_int32_t col_pair = 0; col_pair < total_col_pairs; col_pair++)
    {
      yuyv_packet_t curr = req.image[row * total_col_pairs + col_pair];
      if (curr.U > MIN_U
        && curr.U < MAX_U
        && curr.V > MIN_V
        && curr.V < MAX_V
      ) {
        if (curr.Y1 > MIN_Y && curr.Y1 < MAX_Y) {
          res->total_vertical_sum += row;
          res->total_horizontal_sum += col_pair * 2;
          res->total_mass++;
        }

        if (curr.Y2 > MIN_Y && curr.Y2 < MAX_Y) {
          res->total_vertical_sum += row;
          res->total_horizontal_sum += col_pair * 2 + 1;
          res->total_mass++;
        }
      }
    }
  }

  pthread_exit(res);
}
