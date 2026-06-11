#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "time.h"
#include "controller.h"
#include "printtypes.h"

#define SPEED 1000000
#define BYTES 4
#define PI 3.14159

#define PITCH_ENCODER_STEPS 1000
#define YAW_ENCODER_STEPS 1000

void advanceTestInputs(int step, double* pitch, double* yaw) {
  if (step > 8000) {
    *pitch = 0;
    *yaw = 0;
  } else if (step > 6000)
  {
    *pitch = 1000 * 2 * PI / PITCH_ENCODER_STEPS;
    *yaw = -1000 * 2 * PI / YAW_ENCODER_STEPS;
  } else if (step > 4000)
  {
    *pitch = -200 * 2 * PI / PITCH_ENCODER_STEPS;
    *yaw = -200 * 2 * PI / YAW_ENCODER_STEPS;
  } else if (step > 2000)
  {
    *pitch = -1000 * 2 * PI / PITCH_ENCODER_STEPS;
    *yaw = 1000 * 2 * PI / YAW_ENCODER_STEPS;
  }
}

int main(int argc, char *argv[])
{
  int step;

  controller_input controller_in;

  controller_in.pitch_current_position = 500 * 2 * PI / PITCH_ENCODER_STEPS;
  controller_in.yaw_current_position = -500 * 2 * PI / YAW_ENCODER_STEPS;

  controller_in.pitch_target_position = - PI / 4;
  controller_in.yaw_target_position = PI / 4;

  controller_output controller_out;

  long time_loop_start;
  long time_loop_end;
  long elapsed_usec;

  controller_out = startController(controller_in);

  printf("start time;end time;elapsed utime;curr pitch;curr yaw;pwm pitch;pwm yaw\n");
  
  for (step = 0; step < 10000; step++) {
    time_loop_start = time_time();

    controller_out = advanceController(controller_in);

    advanceTestInputs(step, &controller_in.pitch_current_position, &controller_in.yaw_current_position);

    time_loop_end = time_time();

    elapsed_usec = time_loop_end - time_loop_start;
    
    printf("%ld;%ld;%ld;%f;%f;%f;%f\n",
      time_loop_start % 1000000000,
      time_loop_end % 1000000000,
      elapsed_usec,
      controller_in.pitch_current_position,
      controller_in.yaw_current_position,
      controller_out.pitch_out,
      controller_out.yaw_out);

    precise_sleep(10000 - elapsed_usec);
  }

  return 0;
}